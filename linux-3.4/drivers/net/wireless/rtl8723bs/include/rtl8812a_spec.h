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
#ifndef __RTL8812A_SPEC_H__
#define __RTL8812A_SPEC_H__

#include <drv_conf.h>




#define REG_HSIMR_8812          0x0058
#define REG_HSISR_8812          0x005c
#define REG_GPIO_EXT_CTRL       0x0060
#define REG_GPIO_STATUS_8812      0x006C
#define REG_SDIO_CTRL_8812        0x0070
#define REG_OPT_CTRL_8812       0x0074
#define REG_RF_B_CTRL_8812        0x0076
#define REG_FW_DRV_MSG_8812     0x0088
#define REG_HMEBOX_E2_E3_8812     0x008C
#define REG_HIMR0_8812          0x00B0
#define REG_HISR0_8812          0x00B4
#define REG_HIMR1_8812          0x00B8
#define REG_HISR1_8812          0x00BC
#define REG_EFUSE_BURN_GNT_8812   0x00CF
#define REG_SYS_CFG1_8812       0x00FC

#define REG_PKTBUF_DBG_ADDR       (REG_PKTBUF_DBG_CTRL)
#define REG_RXPKTBUF_DBG        (REG_PKTBUF_DBG_CTRL+2)
#define REG_TXPKTBUF_DBG        (REG_PKTBUF_DBG_CTRL+3)

#define REG_RSVD3_8812          0x0168
#define REG_C2HEVT_CMD_SEQ_88XX   0x01A1
#define REG_C2hEVT_CMD_CONTENT_88XX 0x01A2
#define REG_C2HEVT_CMD_LEN_88XX   0x01AE

#define REG_HMEBOX_EXT0_8812      0x01F0
#define REG_HMEBOX_EXT1_8812      0x01F4
#define REG_HMEBOX_EXT2_8812      0x01F8
#define REG_HMEBOX_EXT3_8812      0x01FC

#define REG_DWBCN0_CTRL_8812        REG_TDECTRL
#define REG_DWBCN1_CTRL_8812        0x0228

#define REG_RXDMA_CONTROL_8812    0x0286
#define REG_RXDMA_PRO_8812      0x0290
#define REG_EARLY_MODE_CONTROL_8812 0x02BC
#define REG_RSVD5_8812          0x02F0
#define REG_RSVD6_8812          0x02F4
#define REG_RSVD7_8812          0x02F8
#define REG_RSVD8_8812          0x02FC


#define REG_DBI_WDATA_8812      0x0348 
#define REG_DBI_RDATA_8812      0x034C 
#define REG_DBI_ADDR_8812       0x0350 
#define REG_DBI_FLAG_8812       0x0352 
#define REG_MDIO_WDATA_8812     0x0354 
#define REG_MDIO_RDATA_8812     0x0356 
#define REG_MDIO_CTL_8812       0x0358 
#define REG_PCIE_MULTIFET_CTRL_8812 0x036A 

#define REG_TXBF_CTRL_8812        0x042C
#define REG_ARFR0_8812          0x0444
#define REG_ARFR1_8812          0x044C
#define REG_CCK_CHECK_8812        0x0454
#define REG_AMPDU_MAX_TIME_8812   0x0456
#define REG_TXPKTBUF_BCNQ_BDNY1_8812  0x0457

#define REG_AMPDU_MAX_LENGTH_8812 0x0458
#define REG_TXPKTBUF_WMAC_LBK_BF_HD_8812  0x045D
#define REG_NDPA_OPT_CTRL_8812    0x045F
#define REG_DATA_SC_8812        0x0483
#define REG_ARFR2_8812          0x048C
#define REG_ARFR3_8812          0x0494
#define REG_TXRPT_START_OFFSET    0x04AC
#define REG_AMPDU_BURST_MODE_8812 0x04BC
#define REG_HT_SINGLE_AMPDU_8812    0x04C7
#define REG_MACID_PKT_DROP0_8812    0x04D0

#define REG_CTWND_8812          0x0572
#define REG_SECONDARY_CCA_CTRL_8812 0x0577
#define REG_SCH_TXCMD_8812      0x05F8

#define REG_MAC_CR_8812       0x0600

#define REG_MAC_TX_SM_STATE_8812    0x06B4

#define REG_BFMER0_INFO_8812      0x06E4
#define REG_BFMER1_INFO_8812      0x06EC
#define REG_CSI_RPT_PARAM_BW20_8812 0x06F4
#define REG_CSI_RPT_PARAM_BW40_8812 0x06F8
#define REG_CSI_RPT_PARAM_BW80_8812 0x06FC

#define REG_BFMEE_SEL_8812        0x0714
#define REG_SND_PTCL_CTRL_8812    0x0718



#define ISR_8812              REG_HISR0_8812

#define IMR_DISABLED_8812         0
#define IMR_TIMER2_8812         BIT31  
#define IMR_TIMER1_8812         BIT30  
#define IMR_PSTIMEOUT_8812        BIT29  
#define IMR_GTINT4_8812         BIT28  
#define IMR_GTINT3_8812         BIT27  
#define IMR_TXBCN0ERR_8812        BIT26  
#define IMR_TXBCN0OK_8812         BIT25  
#define IMR_TSF_BIT32_TOGGLE_8812   BIT24  
#define IMR_BCNDMAINT0_8812       BIT20  
#define IMR_BCNDERR0_8812         BIT16  
#define IMR_HSISR_IND_ON_INT_8812   BIT15  
#define IMR_BCNDMAINT_E_8812        BIT14  
#define IMR_ATIMEND_8812          BIT12  
#define IMR_C2HCMD_8812         BIT10  
#define IMR_CPWM2_8812          BIT9     
#define IMR_CPWM_8812           BIT8     
#define IMR_HIGHDOK_8812          BIT7     
#define IMR_MGNTDOK_8812          BIT6     
#define IMR_BKDOK_8812          BIT5     
#define IMR_BEDOK_8812          BIT4     
#define IMR_VIDOK_8812          BIT3     
#define IMR_VODOK_8812          BIT2     
#define IMR_RDU_8812            BIT1     
#define IMR_ROK_8812            BIT0     

#define IMR_BCNDMAINT7_8812       BIT27  
#define IMR_BCNDMAINT6_8812       BIT26  
#define IMR_BCNDMAINT5_8812       BIT25  
#define IMR_BCNDMAINT4_8812       BIT24  
#define IMR_BCNDMAINT3_8812       BIT23  
#define IMR_BCNDMAINT2_8812       BIT22  
#define IMR_BCNDMAINT1_8812       BIT21  
#define IMR_BCNDOK7_8812          BIT20  
#define IMR_BCNDOK6_8812          BIT19  
#define IMR_BCNDOK5_8812          BIT18  
#define IMR_BCNDOK4_8812          BIT17  
#define IMR_BCNDOK3_8812          BIT16  
#define IMR_BCNDOK2_8812          BIT15  
#define IMR_BCNDOK1_8812          BIT14  
#define IMR_ATIMEND_E_8812        BIT13  
#define IMR_TXERR_8812          BIT11  
#define IMR_RXERR_8812          BIT10  
#define IMR_TXFOVW_8812         BIT9     
#define IMR_RXFOVW_8812         BIT8     


#ifdef CONFIG_PCI_HCI
#define IMR_TX_MASK     (IMR_VODOK_8812|IMR_VIDOK_8812|IMR_BEDOK_8812|IMR_BKDOK_8812|IMR_MGNTDOK_8812|IMR_HIGHDOK_8812)

#define RT_BCN_INT_MASKS  (IMR_BCNDMAINT0_8812 | IMR_TXBCN0OK_8812 | IMR_TXBCN0ERR_8812 | IMR_BCNDERR0_8812)

#define RT_AC_INT_MASKS (IMR_VIDOK_8812 | IMR_VODOK_8812 | IMR_BEDOK_8812|IMR_BKDOK_8812)
#endif



#define AcmHw_HwEn_8812       BIT(0)
#define AcmHw_VoqEn_8812        BIT(1)
#define AcmHw_ViqEn_8812        BIT(2)
#define AcmHw_BeqEn_8812        BIT(3)
#define AcmHw_VoqStatus_8812      BIT(5)
#define AcmHw_ViqStatus_8812      BIT(6)
#define AcmHw_BeqStatus_8812      BIT(7)

#endif

#ifdef CONFIG_RTL8821A
#include "rtl8821a_spec.h"
#endif

