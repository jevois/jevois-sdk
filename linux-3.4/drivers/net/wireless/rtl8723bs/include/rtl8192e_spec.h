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
#ifndef __RTL8192E_SPEC_H__
#define __RTL8192E_SPEC_H__

#include <drv_conf.h>




#define REG_AFE_CTRL1_8192E     0x0024
#define REG_AFE_CTRL2_8192E     0x0028
#define REG_AFE_CTRL3_8192E     0x002c


#define REG_SDIO_CTRL_8192E     0x0070
#define REG_OPT_CTRL_8192E        0x0074
#define REG_RF_B_CTRL_8192E     0x0076
#define REG_AFE_CTRL4_8192E     0x0078
#define REG_LDO_SWR_CTRL        0x007C
#define REG_FW_DRV_MSG_8192E      0x0088
#define REG_HMEBOX_E2_E3_8192E    0x008C
#define REG_HIMR0_8192E       0x00B0
#define REG_HISR0_8192E         0x00B4
#define REG_HIMR1_8192E         0x00B8
#define REG_HISR1_8192E         0x00BC

#define REG_SYS_CFG1_8192E        0x00F0
#define REG_SYS_CFG2_8192E        0x00FC
#define REG_PKTBUF_DBG_ADDR       (REG_PKTBUF_DBG_CTRL)
#define REG_RXPKTBUF_DBG        (REG_PKTBUF_DBG_CTRL+2)
#define REG_TXPKTBUF_DBG        (REG_PKTBUF_DBG_CTRL+3)
#define REG_WOWLAN_WAKE_REASON    REG_MCUTST_WOWLAN

#define REG_RSVD3_8192E         0x0168
#define REG_C2HEVT_CMD_SEQ_88XX   0x01A1
#define REG_C2hEVT_CMD_CONTENT_88XX 0x01A2
#define REG_C2HEVT_CMD_LEN_88XX   0x01AE

#define REG_HMEBOX_EXT0_8192E     0x01F0
#define REG_HMEBOX_EXT1_8192E     0x01F4
#define REG_HMEBOX_EXT2_8192E     0x01F8
#define REG_HMEBOX_EXT3_8192E     0x01FC


#define REG_RXDMA_8192E         0x0290
#define REG_EARLY_MODE_CONTROL_8192E    0x02BC

#define REG_RSVD5_8192E         0x02F0
#define REG_RSVD6_8192E         0x02F4
#define REG_RSVD7_8192E         0x02F8
#define REG_RSVD8_8192E         0x02FC

#define REG_PCIE_MULTIFET_CTRL_8192E  0x036A 

#define REG_TXBF_CTRL_8192E       0x042C
#define REG_ARFR1_8192E         0x044C
#define REG_CCK_CHECK_8192E       0x0454
#define REG_AMPDU_MAX_TIME_8192E      0x0456
#define REG_BCNQ1_BDNY_8192E        0x0457

#define REG_AMPDU_MAX_LENGTH_8192E  0x0458
#define REG_NDPA_OPT_CTRL_8192E   0x045F
#define REG_DATA_SC_8192E       0x0483
#define REG_TXRPT_START_OFFSET      0x04AC
#define REG_AMPDU_BURST_MODE_8192E  0x04BC
#define REG_HT_SINGLE_AMPDU_8192E   0x04C7
#define REG_MACID_PKT_DROP0_8192E   0x04D0

#define REG_CTWND_8192E         0x0572
#define REG_SECONDARY_CCA_CTRL_8192E  0x0577
#define REG_SCH_TXCMD_8192E     0x05F8

#define REG_MAC_CR_8192E        0x0600

#define REG_MAC_TX_SM_STATE_8192E   0x06B4

#define REG_BFMER0_INFO_8192E     0x06E4
#define REG_BFMER1_INFO_8192E     0x06EC
#define REG_CSI_RPT_PARAM_BW20_8192E  0x06F4
#define REG_CSI_RPT_PARAM_BW40_8192E  0x06F8
#define REG_CSI_RPT_PARAM_BW80_8192E  0x06FC

#define REG_BFMEE_SEL_8192E       0x0714
#define REG_SND_PTCL_CTRL_8192E   0x0718



#define ISR_8192E             REG_HISR0_8192E

#define IMR_DISABLED_8192E          0
#define IMR_TIMER2_8192E          BIT31  
#define IMR_TIMER1_8192E          BIT30  
#define IMR_PSTIMEOUT_8192E       BIT29  
#define IMR_GTINT4_8192E          BIT28  
#define IMR_GTINT3_8192E          BIT27  
#define IMR_TXBCN0ERR_8192E       BIT26  
#define IMR_TXBCN0OK_8192E          BIT25  
#define IMR_TSF_BIT32_TOGGLE_8192E    BIT24  
#define IMR_BCNDMAINT0_8192E        BIT20  
#define IMR_BCNDERR0_8192E          BIT16  
#define IMR_HSISR_IND_ON_INT_8192E    BIT15  
#define IMR_BCNDMAINT_E_8192E       BIT14  
#define IMR_ATIMEND_8192E         BIT12  
#define IMR_C2HCMD_8192E          BIT10  
#define IMR_CPWM2_8192E         BIT9     
#define IMR_CPWM_8192E            BIT8     
#define IMR_HIGHDOK_8192E         BIT7     
#define IMR_MGNTDOK_8192E         BIT6     
#define IMR_BKDOK_8192E         BIT5     
#define IMR_BEDOK_8192E         BIT4     
#define IMR_VIDOK_8192E         BIT3     
#define IMR_VODOK_8192E         BIT2     
#define IMR_RDU_8192E           BIT1     
#define IMR_ROK_8192E           BIT0     

#define IMR_BCNDMAINT7_8192E        BIT27  
#define IMR_BCNDMAINT6_8192E        BIT26  
#define IMR_BCNDMAINT5_8192E        BIT25  
#define IMR_BCNDMAINT4_8192E        BIT24  
#define IMR_BCNDMAINT3_8192E        BIT23  
#define IMR_BCNDMAINT2_8192E        BIT22  
#define IMR_BCNDMAINT1_8192E        BIT21  
#define IMR_BCNDOK7_8192E         BIT20  
#define IMR_BCNDOK6_8192E         BIT19  
#define IMR_BCNDOK5_8192E         BIT18  
#define IMR_BCNDOK4_8192E         BIT17  
#define IMR_BCNDOK3_8192E         BIT16  
#define IMR_BCNDOK2_8192E         BIT15  
#define IMR_BCNDOK1_8192E         BIT14  
#define IMR_ATIMEND_E_8192E       BIT13  
#define IMR_TXERR_8192E         BIT11  
#define IMR_RXERR_8192E         BIT10  
#define IMR_TXFOVW_8192E          BIT9     
#define IMR_RXFOVW_8192E          BIT8     


#define BIT_DMA_MODE      BIT1
#define BIT_USB_RXDMA_AGG_EN  BIT31

#define BIT_SPSLDO_SEL      BIT24


#define BIT_BCN_PORT_SEL    BIT5


#define AcmHw_HwEn_8192E        BIT(0)
#define AcmHw_VoqEn_8192E       BIT(1)
#define AcmHw_ViqEn_8192E       BIT(2)
#define AcmHw_BeqEn_8192E       BIT(3)
#define AcmHw_VoqStatus_8192E     BIT(5)
#define AcmHw_ViqStatus_8192E     BIT(6)
#define AcmHw_BeqStatus_8192E     BIT(7)




#endif

