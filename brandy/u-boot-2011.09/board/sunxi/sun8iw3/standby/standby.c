/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include "standby_i.h"
#include "standby.h"
#include "sunxi_board.h"
#include <asm/arch/gic.h>


DECLARE_GLOBAL_DATA_PTR;

static int boot_enter_standby(void);
static int boot_exit_standby(void);
static int boot_standby_detect(void);
static int boot_mod_enter_standby(void);
static int boot_mod_exit_standby(void);
static int boot_early_standby_mode(void);

static int standby_flag = 0;

#if 0
	#define STANDBY_DEBUG(c)  standby_serial_putc(c)
#else
	#define STANDBY_DEBUG(c)
#endif

int boot_standby_mode(void)
{
	int status;


	boot_store_sp();
	boot_set_sp();

	STANDBY_DEBUG('A');
	status = boot_early_standby_mode();
	STANDBY_DEBUG('[');

	boot_restore_sp();
	STANDBY_DEBUG('{');

	return status;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static int boot_early_standby_mode(void)
{
	__s32 key_status, usb_status;
	__s32 status;
	__u32 int_id;

	key_status = standby_axp_probe_key();
	if(key_status & 0x01)			//长按键的情况下，不管电源是否移除，直接进入系统
	{
		if(standby_flag)
		{
			boot_mod_exit_standby();
		}
		return 3;
	}
	if(standby_axp_probe_power_exist() <= 0)
	{
		if(standby_flag)
		{
			boot_mod_exit_standby();
		}
		return 4;
	}
	if(key_status & 0x02)			//短按电压按键的情况下，显示充电动画
	{
		if(standby_flag)
		{
			boot_mod_exit_standby();
		}
		return 2;
	}
	if(!standby_flag)
	{
		boot_mod_enter_standby();      //控制模块进入standby
	}
	usb_status = standby_axp_probe_usb();
	if(usb_status > 0)
	{
		return 8;
	}
	status = -1;
	boot_enter_standby();
	do
	{
		boot_halt();
		int_id = standby_gic_probe_pengding();
		if(int_id == AW_IRQ_NMI)
		{
			status = boot_standby_detect();	
		}
		else if((int_id == AW_IRQ_RALARM0) || (int_id == AW_IRQ_RALARM1))
		{
			status = 3;
		}
	}
	while(status <= 0);
	boot_exit_standby();
	if((status != 8) && (status != 9))
	{
		STANDBY_DEBUG('x');
		boot_mod_exit_standby();
		standby_flag = 0;
	}
	else
	{
		standby_flag = 1;
	}
	STANDBY_DEBUG('[');

	return status;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static int boot_enter_standby(void)
{
	standby_int_disable();
	dram_power_save_process();

	STANDBY_DEBUG('1');

	standby_gic_store();
	STANDBY_DEBUG('m');
	standby_axp_store_int_status();
	STANDBY_DEBUG('n');
	standby_clock_to_24M();
	STANDBY_DEBUG('o');
	standby_clock_plldisable();
	STANDBY_DEBUG('p');
	standby_axp_output_control(0);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static int boot_exit_standby(void)
{
	standby_axp_output_control(1);
	standby_clock_pllenable();
	standby_clock_to_pll1();
	STANDBY_DEBUG('7');
	dram_power_up_process();
	STANDBY_DEBUG('8');
	standby_axp_restore_int_status();
	standby_gic_restore();
	standby_int_enable();
	STANDBY_DEBUG('9');

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static int boot_standby_detect(void)
{
	__u8  power_int_status[8];

	standby_axp_int_query(power_int_status);
	STANDBY_DEBUG('1');
	if(power_int_status[2] & 0x02)			//电源按键短按
	{
		return 2;
	}
	if(power_int_status[2] & 0x01)			//电源按键长按
	{
		return 3;
	}
	if(power_int_status[0] & 0x24)			//外部电源移除
	{
		if(standby_axp_probe_power_exist() <= 0)	//没有外部电源存在
		{
			return 4;
		}
	}
	if(power_int_status[0] & 0x08)			//外部电源插入
	{
		return 8;
	}

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static int boot_mod_enter_standby(void)
{
	uint addr;
	int (* boot_mod_standby)(uint cmd, void *pArg);

	addr = (uint)drv_disp_standby + gd->reloc_off;
	boot_mod_standby = (int (* )(uint cmd, void *pArg))addr;

	boot_mod_standby(BOOT_MOD_ENTER_STANDBY, 0);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static int boot_mod_exit_standby(void)
{
	uint addr;
	int (* boot_mod_standby)(uint cmd, void *pArg);

	addr = (uint)drv_disp_standby + gd->reloc_off;
	boot_mod_standby = (int (* )(uint cmd, void *pArg))addr;

	boot_mod_standby(BOOT_MOD_EXIT_STANDBY, 0);

	return 0;
}




