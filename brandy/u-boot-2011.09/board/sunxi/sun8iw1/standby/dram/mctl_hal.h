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

#ifndef   _MCTL_HAL_H
#define   _MCTL_HAL_H

#include "dram_i.h"

#ifdef PW2I_PRINK
#endif

#define DDR3_32B
#define TEST_MEM 0x40000000

extern unsigned int DRAMC_init(boot_dram_para_t *para);
extern unsigned int DRAMC_init_auto(boot_dram_para_t *para);
extern unsigned int mctl_sys_init(boot_dram_para_t *para);
extern unsigned int mctl_reset_release(void);
extern unsigned int mctl_dll_init(unsigned int ch_index, boot_dram_para_t *para);
extern unsigned int mctl_channel_init(unsigned int ch_index, boot_dram_para_t *para);
extern unsigned int mctl_com_init(boot_dram_para_t *para);
extern unsigned int mctl_port_cfg(void);
extern signed int init_DRAM(int type, void *para);
extern unsigned int ss_bonding_id(void);
extern void paraconfig(unsigned int *para, unsigned int mask, unsigned int value);

#endif 










