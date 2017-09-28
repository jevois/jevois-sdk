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
 *
 ******************************************************************************/
#ifndef __RTL8192C_SPEC_H__
#define __RTL8192C_SPEC_H__

#include <drv_conf.h>




#define REG_HMEBOX_EXT_0			0x0088
#define REG_HMEBOX_EXT_1			0x008A
#define REG_HMEBOX_EXT_2			0x008C
#define REG_HMEBOX_EXT_3			0x008E











#ifdef CONFIG_PCI_HCI
#define IMR_RX_MASK		(IMR_ROK|IMR_RDU|IMR_RXFOVW)
#define IMR_TX_MASK		(IMR_VODOK|IMR_VIDOK|IMR_BEDOK|IMR_BKDOK|IMR_MGNTDOK|IMR_HIGHDOK|IMR_BDOK)

#define RT_BCN_INT_MASKS				(IMR_BcnInt | IMR_TBDOK | IMR_TBDER)
#define RT_AC_INT_MASKS				(IMR_VIDOK | IMR_VODOK | IMR_BEDOK|IMR_BKDOK)
#endif


#ifdef CONFIG_PCI_HCI
#endif 

#ifdef CONFIG_USB_HCI

typedef	enum _BOARD_TYPE_8192CUSB{
	BOARD_USB_DONGLE 			= 0,	
	BOARD_USB_High_PA 		= 1,	
	BOARD_MINICARD		  	= 2,	
	BOARD_USB_SOLO 		 	= 3,	
	BOARD_USB_COMBO			= 4,	
} BOARD_TYPE_8192CUSB, *PBOARD_TYPE_8192CUSB;

#define	SUPPORT_HW_RADIO_DETECT(pHalData)	(pHalData->BoardType == BOARD_MINICARD||\
													pHalData->BoardType == BOARD_USB_SOLO||\
													pHalData->BoardType == BOARD_USB_COMBO)

#endif


#define EFUSE_ACCESS_ON			0x69
#define EFUSE_ACCESS_OFF			0x00



#include "basic_types.h"

#endif

