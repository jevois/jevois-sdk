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
#ifndef __RTL8188E_SPEC_H__
#define __RTL8188E_SPEC_H__





#define REG_BB_PAD_CTRL				0x0064
#define REG_HMEBOX_E0					0x0088
#define REG_HMEBOX_E1					0x008A
#define REG_HMEBOX_E2					0x008C
#define REG_HMEBOX_E3					0x008E
#define REG_HMEBOX_EXT_0				0x01F0
#define REG_HMEBOX_EXT_1				0x01F4
#define REG_HMEBOX_EXT_2				0x01F8
#define REG_HMEBOX_EXT_3				0x01FC
#define REG_HIMR_88E					0x00B0
#define REG_HISR_88E					0x00B4
#define REG_HIMRE_88E					0x00B8
#define REG_HISRE_88E					0x00BC
#define REG_MACID_NO_LINK_0			0x0484
#define REG_MACID_NO_LINK_1			0x0488

#define REG_PKTBUF_DBG_ADDR 			(REG_PKTBUF_DBG_CTRL)
#define REG_RXPKTBUF_DBG				(REG_PKTBUF_DBG_CTRL+2)
#define REG_TXPKTBUF_DBG				(REG_PKTBUF_DBG_CTRL+3)
#define REG_WOWLAN_WAKE_REASON		REG_MCUTST_WOWLAN




#ifdef CONFIG_WOWLAN
#define REG_TXPKTBUF_IV_LOW             0x01a4
#define REG_TXPKTBUF_IV_HIGH            0x01a8
#endif


#define CMD_INIT_LLT					BIT0
#define CMD_READ_EFUSE_MAP		BIT1
#define CMD_EFUSE_PATCH			BIT2
#define CMD_IOCONFIG				BIT3
#define CMD_INIT_LLT_ERR			BIT4
#define CMD_READ_EFUSE_MAP_ERR	BIT5
#define CMD_EFUSE_PATCH_ERR		BIT6
#define CMD_IOCONFIG_ERR			BIT7


#define ISR_88E				REG_HISR_88E

#ifdef CONFIG_PCI_HCI
#define IMR_TX_MASK			(IMR_VODOK_88E|IMR_VIDOK_88E|IMR_BEDOK_88E|IMR_BKDOK_88E|IMR_MGNTDOK_88E|IMR_HIGHDOK_88E|IMR_BCNDERR0_88E)

#ifdef CONFIG_CONCURRENT_MODE
#define RT_BCN_INT_MASKS	(IMR_BCNDMAINT0_88E | IMR_TBDOK_88E | IMR_TBDER_88E | IMR_BCNDMAINT_E_88E)
#else
#define RT_BCN_INT_MASKS	(IMR_BCNDMAINT0_88E | IMR_TBDOK_88E | IMR_TBDER_88E)
#endif

#define RT_AC_INT_MASKS	(IMR_VIDOK_88E | IMR_VODOK_88E | IMR_BEDOK_88E|IMR_BKDOK_88E)
#endif





#define EFUSE_ACCESS_ON			0x69
#define EFUSE_ACCESS_OFF			0x00

#endif

