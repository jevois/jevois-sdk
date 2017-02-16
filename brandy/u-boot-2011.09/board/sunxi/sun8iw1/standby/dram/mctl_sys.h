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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef MCTL_SYS_H_
#define MCTL_SYS_H_

extern void mctl_self_refresh_entry (unsigned int ch_index);
extern void mctl_self_refresh_exit (unsigned int ch_index);
extern unsigned int mctl_selfrefesh_test (void);
extern unsigned int mctl_deep_sleep_test (void);
extern void mctl_deep_sleep_entry (void);
extern void mctl_deep_sleep_exit (boot_dram_para_t * para);

#endif

