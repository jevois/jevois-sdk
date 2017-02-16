/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *******************************************************************************/
#ifndef __RTL8723A_SPEC_H__
#define __RTL8723A_SPEC_H__

#include <rtl8192c_spec.h>











#define REG_BT_COEX_TABLE_1   0x06C0
#define REG_BT_COEX_TABLE_2   0x06C4


#define MASK_HSISR_CLEAR    (HSISR_GPIO12_0_INT |\
                             HSISR_SPS_OCP_INT |\
                             HSISR_RON_INT |\
                             HSISR_PDNINT |\
                             HSISR_GPIO9_INT)





#endif

