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
#include "dram_i.h"

int dram_power_save_process(void)
{
	unsigned int reg_val;

	mctl_self_refresh_entry(0);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_self_refresh_entry(1);
	}
	mctl_write_w(0 + SDR_PIR, 0x11);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
		mctl_write_w(0x1000 + SDR_PIR, 0x11);

	reg_val = mctl_read_w(SDR_COM_CCR);
	reg_val &= ~(0x7<<0);
	mctl_write_w(SDR_COM_CCR, reg_val);

	reg_val = mctl_read_w(SDR_COM_CCR);
	reg_val |= (0x3<<3);
	mctl_write_w(SDR_COM_CCR, reg_val);
	reg_val = mctl_read_w(CCM_AHB1_GATE0_CTRL);
	reg_val &=~(0x1<<14);
	mctl_write_w(CCM_AHB1_GATE0_CTRL, reg_val);

	reg_val = mctl_read_w(CCM_MDFS_CLK_CTRL);
	reg_val &= ~(0x1U<<31);
	mctl_write_w(CCM_MDFS_CLK_CTRL, reg_val);

	return 0;
}

int dram_power_up_process(void)
{
	unsigned int reg_val;
	boot_dram_para_t  *dram_parameters = (boot_dram_para_t *)BOOT_STANDBY_DRAM_PARA_ADDR;

	standby_timer_delay(10);

	reg_val = mctl_read_w(CCM_MDFS_CLK_CTRL);
	reg_val &= ~((0x3<<24) | (0x3<<16) | (0xf<<0));
	reg_val |= (0x1u<<31) | (0x1<<24) | (0x0<<16) | (0x2<<0);
	mctl_write_w(CCM_MDFS_CLK_CTRL, reg_val);

	reg_val = mctl_read_w(CCM_AHB1_GATE0_CTRL);
	reg_val |= (0x1<<14);
	mctl_write_w(CCM_AHB1_GATE0_CTRL, reg_val);
	reg_val = mctl_read_w(SDR_COM_CCR);
	reg_val &= ~(0x3<<3);
	mctl_write_w(SDR_COM_CCR, reg_val);

	standby_timer_delay(10);

	mctl_write_w(0 + SDR_ACDLLCR,0x80000000);
	mctl_write_w(0 + SDR_DX0DLLCR,0x80000000);
	mctl_write_w(0 + SDR_DX1DLLCR,0x80000000);
	mctl_write_w(0 + SDR_DX2DLLCR,0x80000000);
	mctl_write_w(0 + SDR_DX3DLLCR,0x80000000);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_write_w(0x1000 + SDR_ACDLLCR,0x80000000);
		mctl_write_w(0x1000 + SDR_DX0DLLCR,0x80000000);
		mctl_write_w(0x1000 + SDR_DX1DLLCR,0x80000000);
		mctl_write_w(0x1000 + SDR_DX2DLLCR,0x80000000);
		mctl_write_w(0x1000 + SDR_DX3DLLCR,0x80000000);
	}

	standby_timer_delay(1);

	mctl_write_w(0 + SDR_ACDLLCR,0x0);
	mctl_write_w(0 + SDR_DX0DLLCR,0x0);
	mctl_write_w(0 + SDR_DX1DLLCR,0x0);
	mctl_write_w(0 + SDR_DX2DLLCR,0x0);
	mctl_write_w(0 + SDR_DX3DLLCR,0x0);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_write_w(0x1000 + SDR_ACDLLCR,0x0);
		mctl_write_w(0x1000 + SDR_DX0DLLCR,0x0);
		mctl_write_w(0x1000 + SDR_DX1DLLCR,0x0);
		mctl_write_w(0x1000 + SDR_DX2DLLCR,0x0);
		mctl_write_w(0x1000 + SDR_DX3DLLCR,0x0);
	}

	standby_timer_delay(1);

	mctl_write_w(0 + SDR_ACDLLCR,0x40000000);
	mctl_write_w(0 + SDR_DX0DLLCR,0x40000000);
	mctl_write_w(0 + SDR_DX1DLLCR,0x40000000);
	mctl_write_w(0 + SDR_DX2DLLCR,0x40000000);
	mctl_write_w(0 + SDR_DX3DLLCR,0x40000000);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_write_w(0x1000 + SDR_ACDLLCR,0x40000000);
		mctl_write_w(0x1000 + SDR_DX0DLLCR,0x40000000);
		mctl_write_w(0x1000 + SDR_DX1DLLCR,0x40000000);
		mctl_write_w(0x1000 + SDR_DX2DLLCR,0x40000000);
		mctl_write_w(0x1000 + SDR_DX3DLLCR,0x40000000);
	}

	mctl_write_w(0 + SDR_PIR, 0x01);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
		mctl_write_w(0x1000 + SDR_PIR, 0x01);
	reg_val = mctl_read_w(SDR_COM_CCR);
	reg_val |= (0x5<<0);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
		reg_val |= (0x7<<0);
	mctl_write_w(SDR_COM_CCR, reg_val);

	mctl_self_refresh_exit(0);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_self_refresh_exit(1);
	}

	return 0;
}

int dram_enter_self_refresh(void)
{
	unsigned int reg_val;

	mctl_self_refresh_entry(0);


	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_self_refresh_entry(1);

	}

	reg_val = mctl_read_w(CCM_PLL5_DDR_CTRL);
  	reg_val &= ~(0x1U<<31);
  	mctl_write_w(CCM_PLL5_DDR_CTRL, reg_val);

  	reg_val = mctl_read_w(CCM_PLL5_DDR_CTRL);
  	reg_val |= 0x1U<<20;
  	mctl_write_w(CCM_PLL5_DDR_CTRL, reg_val);


	mctl_write_w(0 + SDR_ACDLLCR,0xC0000000);
	mctl_write_w(0 + SDR_DX0DLLCR,0xC0000000);
	mctl_write_w(0 + SDR_DX1DLLCR,0xC0000000);
	mctl_write_w(0 + SDR_DX2DLLCR,0xC0000000);
	mctl_write_w(0 + SDR_DX3DLLCR,0xC0000000);

	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_write_w(0x1000 + SDR_ACDLLCR,0xC0000000);
		mctl_write_w(0x1000 + SDR_DX0DLLCR,0xC0000000);
		mctl_write_w(0x1000 + SDR_DX1DLLCR,0xC0000000);
		mctl_write_w(0x1000 + SDR_DX2DLLCR,0xC0000000);
		mctl_write_w(0x1000 + SDR_DX3DLLCR,0xC0000000);
	}


	return 0;
}

int dram_exit_self_refresh(void)
{
	unsigned int reg_val;
	boot_dram_para_t  *dram_parameters = (boot_dram_para_t *)BOOT_STANDBY_DRAM_PARA_ADDR;

	mctl_write_w(0 + SDR_ACDLLCR,0x80000000);
	mctl_write_w(0 + SDR_DX0DLLCR,0x80000000);
	mctl_write_w(0 + SDR_DX1DLLCR,0x80000000);
	mctl_write_w(0 + SDR_DX2DLLCR,0x80000000);
	mctl_write_w(0 + SDR_DX3DLLCR,0x80000000);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_write_w(0X1000 + SDR_ACDLLCR,0x80000000);
		mctl_write_w(0X1000 + SDR_DX0DLLCR,0x80000000);
		mctl_write_w(0X1000 + SDR_DX1DLLCR,0x80000000);
		mctl_write_w(0X1000 + SDR_DX2DLLCR,0x80000000);
		mctl_write_w(0X1000 + SDR_DX3DLLCR,0x80000000);
	}


	standby_timer_delay(0x200);

	mctl_write_w(0 + SDR_ACDLLCR,0x0);
	mctl_write_w(0 + SDR_DX0DLLCR,0x0);
	mctl_write_w(0 + SDR_DX1DLLCR,0x0);
	mctl_write_w(0 + SDR_DX2DLLCR,0x0);
	mctl_write_w(0 + SDR_DX3DLLCR,0x0);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_write_w(0x1000 + SDR_ACDLLCR,0x0);
		mctl_write_w(0x1000 + SDR_DX0DLLCR,0x0);
		mctl_write_w(0x1000 + SDR_DX1DLLCR,0x0);
		mctl_write_w(0x1000 + SDR_DX2DLLCR,0x0);
		mctl_write_w(0x1000 + SDR_DX3DLLCR,0x0);
	}


	standby_timer_delay(0x200);

	mctl_write_w(0 + SDR_ACDLLCR,0x40000000);
	mctl_write_w(0 + SDR_DX0DLLCR,0x40000000);
	mctl_write_w(0 + SDR_DX1DLLCR,0x40000000);
	mctl_write_w(0 + SDR_DX2DLLCR,0x40000000);
	mctl_write_w(0 + SDR_DX3DLLCR,0x40000000);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_write_w(0x1000 + SDR_ACDLLCR,0x40000000);
		mctl_write_w(0x1000 + SDR_DX0DLLCR,0x40000000);
		mctl_write_w(0x1000 + SDR_DX1DLLCR,0x40000000);
		mctl_write_w(0x1000 + SDR_DX2DLLCR,0x40000000);
		mctl_write_w(0x1000 + SDR_DX3DLLCR,0x40000000);
	}


	standby_timer_delay(0x200);

	reg_val = mctl_read_w(CCM_PLL5_DDR_CTRL);
	reg_val &= ~((0x3<<0) | (0x3<<4) | (0x1F<<8));
	reg_val |= ((0x0<<0) | (0x1<<4));
	reg_val |= ((dram_parameters->dram_clk/24-1)<<0x8);//N
	mctl_write_w(CCM_PLL5_DDR_CTRL, reg_val);

	reg_val = mctl_read_w(CCM_PLL5_DDR_CTRL);
  	reg_val |= 0x1U<<31;
  	mctl_write_w(CCM_PLL5_DDR_CTRL, reg_val);

  	reg_val = mctl_read_w(CCM_PLL5_DDR_CTRL);
  	reg_val |= 0x1U<<20;
  	mctl_write_w(CCM_PLL5_DDR_CTRL, reg_val);

  	while(mctl_read_w(CCM_PLL5_DDR_CTRL) & (0x1<<20)){
  	}

  	while(!(mctl_read_w(CCM_PLL5_DDR_CTRL) & (0x1<<28))){
  	}

	mctl_self_refresh_exit(0);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_self_refresh_exit(1);
	}

	return 0;
}
