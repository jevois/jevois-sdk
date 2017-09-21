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

#ifndef  __toc_v2_h
#define  __toc_v2_h


#include "spare_head.h"

#define  TOC0_MAGIC             "TOC0.GLH"
#define  TOC_MAIN_INFO_MAGIC    0x89119800


#define  SECURE_SWITCH_OTHER   0
#define  SECURE_SWITCH_NORMAL  1
#define  SECURE_NON_SECUREOS   2

typedef struct
{
	u8  name[8];	  //字符串，可以更改，没有作限制
	u32 magic;	      //必须是0x89119800
	u32 check_sum;    //整个数据的校验和，参考现在boot0做法

	u32 serial_num;   //序列号，可以更改，没有限制
	u32 status;       //可以更改，没有限制

	u32 items_nr;	  //总的项目个数，对TOC0来说，必须是2
	u32 length;	      //TOC0的长度
	u8  platform[4];  //toc_platform[0]标示启动介质
	u32 reserved[2];  //保留位
	u32 end;          //表示头部结构体结束，必须是0x3b45494d

}
toc0_private_head_t;

typedef struct sbrom_toc1_head_info
{
	char name[16]	;
	u32  magic	;
	u32  add_sum	;

	u32  serial_num	;
	u32  status		;

	u32  items_nr;
	u32  valid_len;
	u32  reserved[5];
	u32  end;
}
sbrom_toc1_head_info_t;


typedef struct sbrom_toc1_item_info
{
	char name[64];		
	u32  data_offset;
	u32  data_len;
	u32  encrypt;		
	u32  type;			
	u32  run_addr;         
	u32  index;            
	u32  reserved[69];	  
	u32  end;
}sbrom_toc1_item_info_t;


typedef struct sbrom_toc0_config
{
	unsigned char    	config_vsn[4];
	unsigned int      	dram_para[32];  	// dram参数
	int				  	uart_port;      	// UART控制器编号
	normal_gpio_cfg   	uart_ctrl[2];    	// UART控制器GPIO
	int              	enable_jtag;    	// JTAG使能
	normal_gpio_cfg   	jtag_gpio[5];    	// JTAG控制器GPIO
	normal_gpio_cfg  	storage_gpio[50]; 	// 存储设备 GPIO信息
	char   				storage_data[384];  // 0-159,存储nand信息；160-255,存放卡信息
	unsigned int        secure_dram_mbytes;
	unsigned int        drm_start_mbytes;  
	unsigned int        drm_size_mbytes;   
	unsigned int        boot_cpu;          
	special_gpio_cfg  a15_power_gpio; 
	unsigned int        next_exe_pa;
        unsigned int        secure_without_OS;  
        unsigned int        debug_mode;        
	unsigned int      	res[3];   			// 总共1024字节

}
sbrom_toc0_config_t;

#endif    

/* end of toc.h */
