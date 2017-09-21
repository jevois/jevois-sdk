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
#ifndef __RTL8723B_SPEC_H__
#define __RTL8723B_SPEC_H__

#include <drv_conf.h>


#define HAL_NAV_UPPER_UNIT_8723B		128	

#define REG_RSV_CTRL_8723B				0x001C
#define REG_BT_WIFI_ANTENNA_SWITCH_8723B	0x0038
#define REG_HSISR_8723B					0x005c
#define REG_PAD_CTRL1_8723B		0x0064
#define REG_AFE_CTRL_4_8723B		0x0078
#define REG_HMEBOX_DBG_0_8723B	0x0088
#define REG_HMEBOX_DBG_1_8723B	0x008A
#define REG_HMEBOX_DBG_2_8723B	0x008C
#define REG_HMEBOX_DBG_3_8723B	0x008E
#define REG_HIMR0_8723B					0x00B0
#define REG_HISR0_8723B					0x00B4
#define REG_HIMR1_8723B					0x00B8
#define REG_HISR1_8723B					0x00BC
#define REG_PMC_DBG_CTRL2_8723B			0x00CC

#define REG_C2HEVT_CMD_ID_8723B	0x01A0
#define REG_C2HEVT_CMD_LEN_8723B	0x01AE
#define REG_WOWLAN_WAKE_REASON 0x01C7
#define REG_WOWLAN_GTK_DBG1	0x630
#define REG_WOWLAN_GTK_DBG2	0x634

#define REG_HMEBOX_EXT0_8723B			0x01F0
#define REG_HMEBOX_EXT1_8723B			0x01F4
#define REG_HMEBOX_EXT2_8723B			0x01F8
#define REG_HMEBOX_EXT3_8723B			0x01FC


#define REG_RXDMA_CONTROL_8723B		0x0286
#define REG_RXDMA_MODE_CTRL_8723B		0x0290

#define	REG_PCIE_CTRL_REG_8723B		0x0300
#define	REG_INT_MIG_8723B				0x0304
#define	REG_BCNQ_DESA_8723B			0x0308
#define	REG_HQ_DESA_8723B				0x0310
#define	REG_MGQ_DESA_8723B			0x0318
#define	REG_VOQ_DESA_8723B			0x0320
#define	REG_VIQ_DESA_8723B				0x0328
#define	REG_BEQ_DESA_8723B			0x0330
#define	REG_BKQ_DESA_8723B			0x0338
#define	REG_RX_DESA_8723B				0x0340
#define	REG_DBI_WDATA_8723B			0x0348
#define	REG_DBI_RDATA_8723B			0x034C
#define	REG_DBI_ADDR_8723B				0x0350
#define	REG_DBI_FLAG_8723B				0x0352
#define	REG_MDIO_WDATA_8723B		0x0354
#define	REG_MDIO_RDATA_8723B			0x0356
#define	REG_MDIO_CTL_8723B			0x0358
#define	REG_DBG_SEL_8723B				0x0360
#define	REG_PCIE_HRPWM_8723B			0x0361
#define	REG_PCIE_HCPWM_8723B			0x0363
#define	REG_PCIE_MULTIFET_CTRL_8723B	0x036A

#define REG_TXPKTBUF_BCNQ_BDNY_8723B	0x0424
#define REG_TXPKTBUF_MGQ_BDNY_8723B	0x0425
#define REG_TXPKTBUF_WMAC_LBK_BF_HD_8723B	0x045D
#ifdef CONFIG_WOWLAN
#define REG_TXPKTBUF_IV_LOW             0x0484
#define REG_TXPKTBUF_IV_HIGH            0x0488
#endif
#define REG_AMPDU_BURST_MODE_8723B	0x04BC

#define REG_SECONDARY_CCA_CTRL_8723B	0x0577






#define SDIO_REG_HCPWM1_8723B	0x025



#define MASK_HSISR_CLEAR		(HSISR_GPIO12_0_INT |\
								HSISR_SPS_OCP_INT |\
								HSISR_RON_INT |\
								HSISR_PDNINT |\
								HSISR_GPIO9_INT)




#define BIT_USB_RXDMA_AGG_EN	BIT(31)
#define RXDMA_AGG_MODE_EN		BIT(1)

#ifdef CONFIG_WOWLAN
#define RXPKT_RELEASE_POLL		BIT(16)
#define RXDMA_IDLE				BIT(17)
#define RW_RELEASE_EN			BIT(18)
#endif


#define BIT_BCN_PORT_SEL		BIT5


#ifdef CONFIG_RF_GAIN_OFFSET

#ifdef CONFIG_RTL8723B
#define EEPROM_RF_GAIN_OFFSET			0xC1
#endif

#define EEPROM_RF_GAIN_VAL				0x1F6
#endif


#define	IMR_DISABLED_8723B					0
#define	IMR_TIMER2_8723B					BIT31	
#define	IMR_TIMER1_8723B					BIT30	
#define	IMR_PSTIMEOUT_8723B				BIT29	
#define	IMR_GTINT4_8723B					BIT28	
#define	IMR_GTINT3_8723B					BIT27	
#define	IMR_TXBCN0ERR_8723B				BIT26	
#define	IMR_TXBCN0OK_8723B				BIT25	
#define	IMR_TSF_BIT32_TOGGLE_8723B		BIT24	
#define	IMR_BCNDMAINT0_8723B				BIT20	
#define	IMR_BCNDERR0_8723B				BIT16	
#define	IMR_HSISR_IND_ON_INT_8723B		BIT15	
#define	IMR_BCNDMAINT_E_8723B			BIT14	
#define	IMR_ATIMEND_8723B				BIT12	
#define	IMR_C2HCMD_8723B					BIT10	
#define	IMR_CPWM2_8723B					BIT9		
#define	IMR_CPWM_8723B					BIT8		
#define	IMR_HIGHDOK_8723B				BIT7		
#define	IMR_MGNTDOK_8723B				BIT6		
#define	IMR_BKDOK_8723B					BIT5		
#define	IMR_BEDOK_8723B					BIT4		
#define	IMR_VIDOK_8723B					BIT3		
#define	IMR_VODOK_8723B					BIT2		
#define	IMR_RDU_8723B					BIT1		
#define	IMR_ROK_8723B					BIT0		

#define	IMR_BCNDMAINT7_8723B				BIT27	
#define	IMR_BCNDMAINT6_8723B				BIT26	
#define	IMR_BCNDMAINT5_8723B				BIT25	
#define	IMR_BCNDMAINT4_8723B				BIT24	
#define	IMR_BCNDMAINT3_8723B				BIT23	
#define	IMR_BCNDMAINT2_8723B				BIT22	
#define	IMR_BCNDMAINT1_8723B				BIT21	
#define	IMR_BCNDOK7_8723B					BIT20	
#define	IMR_BCNDOK6_8723B					BIT19	
#define	IMR_BCNDOK5_8723B					BIT18	
#define	IMR_BCNDOK4_8723B					BIT17	
#define	IMR_BCNDOK3_8723B					BIT16	
#define	IMR_BCNDOK2_8723B					BIT15	
#define	IMR_BCNDOK1_8723B					BIT14	
#define	IMR_ATIMEND_E_8723B				BIT13	
#define	IMR_TXERR_8723B					BIT11	
#define	IMR_RXERR_8723B					BIT10	
#define	IMR_TXFOVW_8723B					BIT9		
#define	IMR_RXFOVW_8723B					BIT8		

#ifdef CONFIG_PCI_HCI
#define IMR_TX_MASK			(IMR_VODOK_8723B|IMR_VIDOK_8723B|IMR_BEDOK_8723B|IMR_BKDOK_8723B|IMR_MGNTDOK_8723B|IMR_HIGHDOK_8723B)

#define RT_BCN_INT_MASKS	(IMR_BCNDMAINT0_8723B | IMR_TXBCN0OK_8723B | IMR_TXBCN0ERR_8723B | IMR_BCNDERR0_8723B)

#define RT_AC_INT_MASKS	(IMR_VIDOK_8723B | IMR_VODOK_8723B | IMR_BEDOK_8723B|IMR_BKDOK_8723B)
#endif

#endif

#ifdef CONFIG_USB_HCI
typedef	enum _BOARD_TYPE_8192CUSB{
	BOARD_USB_DONGLE 			= 0,	
	BOARD_USB_High_PA 		= 1,	
	BOARD_MINICARD		  	= 2,	
	BOARD_USB_SOLO 		 	= 3,	
	BOARD_USB_COMBO			= 4,	
} BOARD_TYPE_8723BUSB, *PBOARD_TYPE_8723BUSB;

#endif
