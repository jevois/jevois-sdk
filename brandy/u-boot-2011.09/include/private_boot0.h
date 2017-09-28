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

#ifndef  __boot0_v2_h
#define  __boot0_v2_h


#include "spare_head.h"

#define BOOT0_MAGIC                     "eGON.BT0"

#define SYS_PARA_LOG                    0x4d415244
/******************************************************************************/
/*                              file head of Boot                             */
/******************************************************************************/
typedef struct _Boot_file_head
{
	__u32  jump_instruction;  
	__u8   magic[MAGIC_SIZE]; 
	__u32  check_sum;         
	__u32  length;            
	__u32  pub_head_size;     
	special_gpio_cfg  a15_power_gpio; 
	__u32  ret_addr;          
	__u32  run_addr;          
	__u32  boot_cpu;          
	__u8   platform[8];       
}boot_file_head_t;
/******************************************************************************/
/*                              file head of Boot0                            */
/******************************************************************************/
typedef struct _boot0_private_head_t
{
	__u32                       prvt_head_size;
	__u32                        debug_mode;      
	unsigned int                dram_para[32];;        
	__s32						uart_port;              // UART控制器编号
	normal_gpio_cfg             uart_ctrl[2];           // UART控制器(调试打印口)数据信息
	__s32                       enable_jtag;           
    normal_gpio_cfg	            jtag_gpio[5];           // 保存JTAG的全部GPIO信息
    normal_gpio_cfg             storage_gpio[32];       // 存储设备 GPIO信息
    char                        storage_data[512 - sizeof(normal_gpio_cfg) * 32];      // 用户保留数据信息
}boot0_private_head_t;


typedef struct _boot0_file_head_t
{
	boot_file_head_t      boot_head;
	boot0_private_head_t  prvt_head;
}boot0_file_head_t;


#endif    

/* end of boot0.h */
