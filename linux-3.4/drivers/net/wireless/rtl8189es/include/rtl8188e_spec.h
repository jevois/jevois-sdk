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

#include <drv_conf.h>

#ifndef BIT
#define BIT(x)    (1 << (x))
#endif

#define BIT0    0x00000001
#define BIT1    0x00000002
#define BIT2    0x00000004
#define BIT3    0x00000008
#define BIT4    0x00000010
#define BIT5    0x00000020
#define BIT6    0x00000040
#define BIT7    0x00000080
#define BIT8    0x00000100
#define BIT9    0x00000200
#define BIT10 0x00000400
#define BIT11 0x00000800
#define BIT12 0x00001000
#define BIT13 0x00002000
#define BIT14 0x00004000
#define BIT15 0x00008000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000


#define   HAL_PS_TIMER_INT_DELAY  50   
#define   HAL_92C_NAV_UPPER_UNIT  128  

#define MAC_ADDR_LEN          6
#define TXPKT_BUF_SELECT        0x69
#define RXPKT_BUF_SELECT        0xA5
#define DISABLE_TRXPKT_BUF_ACCESS 0x0


#define REG_SYS_ISO_CTRL        0x0000
#define REG_SYS_FUNC_EN       0x0002
#define REG_APS_FSMCO         0x0004
#define REG_SYS_CLKR          0x0008
#define REG_9346CR            0x000A
#define REG_EE_VPD            0x000C
#define REG_AFE_MISC          0x0010
#define REG_SPS0_CTRL         0x0011
#define REG_SPS_OCP_CFG       0x0018
#define REG_RSV_CTRL          0x001C
#define REG_RF_CTRL           0x001F
#define REG_LDOA15_CTRL       0x0020
#define REG_LDOV12D_CTRL        0x0021
#define REG_LDOHCI12_CTRL       0x0022
#define REG_LPLDO_CTRL          0x0023
#define REG_AFE_XTAL_CTRL       0x0024
#define REG_AFE_PLL_CTRL        0x0028
#define REG_APE_PLL_CTRL_EXT      0x002c
#define REG_EFUSE_CTRL          0x0030
#define REG_EFUSE_TEST          0x0034
#define REG_GPIO_MUXCFG       0x0040
#define REG_GPIO_IO_SEL         0x0042
#define REG_MAC_PINMUX_CFG      0x0043
#define REG_GPIO_PIN_CTRL       0x0044
#define REG_GPIO_INTM         0x0048
#define REG_LEDCFG0           0x004C
#define REG_LEDCFG1           0x004D
#define REG_LEDCFG2           0x004E
#define REG_LEDCFG3           0x004F
#define REG_FSIMR           0x0050
#define REG_FSISR           0x0054
#define REG_HSIMR           0x0058
#define REG_HSISR           0x005c
#define REG_GPIO_PIN_CTRL_2     0x0060
#define REG_GPIO_IO_SEL_2       0x0062
#define REG_BB_PAD_CTRL       0x0064
#define REG_MULTI_FUNC_CTRL     0x0068
#define REG_GPIO_OUTPUT       0x006c
#define REG_AFE_XTAL_CTRL_EXT       0x0078
#define REG_XCK_OUT_CTRL        0x007c
#define REG_MCUFWDL         0x0080
#define REG_WOL_EVENT         0x0081
#define REG_MCUTSTCFG         0x0084
#define REG_HMEBOX_E0         0x0088
#define REG_HMEBOX_E1         0x008A
#define REG_HMEBOX_E2         0x008C
#define REG_HMEBOX_E3         0x008E
#define REG_HMEBOX_EXT_0        0x01F0
#define REG_HMEBOX_EXT_1        0x01F4
#define REG_HMEBOX_EXT_2        0x01F8
#define REG_HMEBOX_EXT_3        0x01FC
#define REG_HIMR_88E          0x00B0
#define REG_HISR_88E          0x00B4
#define REG_HIMRE_88E         0x00B8
#define REG_HISRE_88E         0x00BC
#define REG_EFUSE_ACCESS        0x00CF 
#define REG_BIST_SCAN         0x00D0
#define REG_BIST_RPT          0x00D4
#define REG_BIST_ROM_RPT        0x00D8
#define REG_USB_SIE_INTF        0x00E0
#define REG_PCIE_MIO_INTF       0x00E4
#define REG_PCIE_MIO_INTD       0x00E8
#define REG_HPON_FSM          0x00EC
#define REG_SYS_CFG         0x00F0
#define REG_GPIO_OUTSTS       0x00F4 
#define REG_TYPE_ID         0x00FC

#define REG_MAC_PHY_CTRL_NORMAL   0x00f8

#define REG_CR            0x0100
#define REG_PBP           0x0104
#define REG_PKT_BUFF_ACCESS_CTRL  0x0106
#define REG_TRXDMA_CTRL       0x010C
#define REG_TRXFF_BNDY        0x0114
#define REG_TRXFF_STATUS        0x0118
#define REG_RXFF_PTR          0x011C
#define REG_HIMRE           0x0128
#define REG_HISRE           0x012C
#define REG_CPWM            0x012F
#define REG_FWIMR         0x0130
#define REG_FTIMR           0x0138
#define REG_FWISR           0x0134
#define REG_PKTBUF_DBG_CTRL     0x0140
#define REG_PKTBUF_DBG_ADDR       (REG_PKTBUF_DBG_CTRL)
#define REG_RXPKTBUF_DBG        (REG_PKTBUF_DBG_CTRL+2)
#define REG_TXPKTBUF_DBG        (REG_PKTBUF_DBG_CTRL+3)
#define REG_RXPKTBUF_CTRL     (REG_PKTBUF_DBG_CTRL+2)
#define REG_PKTBUF_DBG_DATA_L     0x0144
#define REG_PKTBUF_DBG_DATA_H   0x0148

#define REG_TC0_CTRL          0x0150
#define REG_TC1_CTRL          0x0154
#define REG_TC2_CTRL          0x0158
#define REG_TC3_CTRL          0x015C
#define REG_TC4_CTRL          0x0160
#define REG_TCUNIT_BASE       0x0164
#define REG_MBIST_START       0x0174
#define REG_MBIST_DONE        0x0178
#define REG_MBIST_FAIL          0x017C
#define REG_32K_CTRL          0x0194
#define REG_C2HEVT_MSG_NORMAL   0x01A0
#define REG_C2HEVT_CLEAR        0x01AF
#define REG_MCUTST_1          0x01c0
#ifdef CONFIG_WOWLAN
#define REG_WOWLAN_WAKE_REASON      0x01c7
#endif
#define REG_FMETHR          0x01C8
#define REG_HMETFR          0x01CC
#define REG_HMEBOX_0          0x01D0
#define REG_HMEBOX_1          0x01D4
#define REG_HMEBOX_2          0x01D8
#define REG_HMEBOX_3          0x01DC

#define REG_LLT_INIT          0x01E0


#define REG_RQPN            0x0200
#define REG_FIFOPAGE          0x0204
#define REG_TDECTRL         0x0208
#define REG_TXDMA_OFFSET_CHK      0x020C
#define REG_TXDMA_STATUS        0x0210
#define REG_RQPN_NPQ          0x0214

#define   REG_RXDMA_AGG_PG_TH     0x0280
#define REG_RXPKT_NUM         0x0284
#define   REG_RXDMA_STATUS        0x0288

#define REG_PCIE_CTRL_REG     0x0300
#define REG_INT_MIG         0x0304 
#define REG_BCNQ_DESA       0x0308 
#define REG_HQ_DESA         0x0310 
#define REG_MGQ_DESA        0x0318 
#define REG_VOQ_DESA        0x0320 
#define REG_VIQ_DESA        0x0328 
#define REG_BEQ_DESA        0x0330 
#define REG_BKQ_DESA        0x0338 
#define REG_RX_DESA         0x0340 
#define REG_MDIO          0x0354 
#define REG_DBG_SEL         0x0360 
#define REG_PCIE_HRPWM        0x0361 
#define REG_PCIE_HCPWM        0x0363 
#define REG_WATCH_DOG       0x0368

#define REG_PCIE_HISR       0x03A0

#define REG_VOQ_INFORMATION     0x0400
#define REG_VIQ_INFORMATION     0x0404
#define REG_BEQ_INFORMATION     0x0408
#define REG_BKQ_INFORMATION     0x040C
#define REG_MGQ_INFORMATION     0x0410
#define REG_HGQ_INFORMATION     0x0414
#define REG_BCNQ_INFORMATION      0x0418
#define REG_TXPKT_EMPTY       0x041A

#define REG_CPU_MGQ_INFORMATION   0x041C
#define REG_FWHW_TXQ_CTRL     0x0420
#define REG_HWSEQ_CTRL        0x0423
#define REG_TXPKTBUF_BCNQ_BDNY    0x0424
#define REG_TXPKTBUF_MGQ_BDNY   0x0425
#define REG_LIFETIME_EN       0x0426
#define REG_MULTI_BCNQ_OFFSET     0x0427
#define REG_SPEC_SIFS         0x0428
#define REG_RL            0x042A
#define REG_DARFRC          0x0430
#define REG_RARFRC          0x0438
#define REG_RRSR            0x0440
#define REG_ARFR0           0x0444
#define REG_ARFR1           0x0448
#define REG_ARFR2           0x044C
#define REG_ARFR3           0x0450
#define REG_AGGLEN_LMT        0x0458
#define REG_AMPDU_MIN_SPACE     0x045C
#define REG_TXPKTBUF_WMAC_LBK_BF_HD 0x045D
#define REG_FAST_EDCA_CTRL      0x0460
#define REG_RD_RESP_PKT_TH      0x0463
#define REG_INIRTS_RATE_SEL       0x0480
#define REG_POWER_STATUS        0x04A4
#define REG_POWER_STAGE1        0x04B4
#define REG_POWER_STAGE2        0x04B8
#define REG_PKT_VO_VI_LIFE_TIME     0x04C0
#define REG_PKT_BE_BK_LIFE_TIME     0x04C2
#define REG_STBC_SETTING        0x04C4
#define REG_PROT_MODE_CTRL      0x04C8
#define REG_MAX_AGGR_NUM      0x04CA
#define REG_RTS_MAX_AGGR_NUM    0x04CB
#define REG_BAR_MODE_CTRL     0x04CC
#define REG_RA_TRY_RATE_AGG_LMT   0x04CF
#define REG_EARLY_MODE_CONTROL    0x4D0
#define REG_NQOS_SEQ          0x04DC
#define REG_QOS_SEQ         0x04DE
#define REG_NEED_CPU_HANDLE     0x04E0
#define REG_PKT_LOSE_RPT        0x04E1
#define REG_PTCL_ERR_STATUS     0x04E2
#define REG_TX_RPT_CTRL       0x04EC
#define REG_TX_RPT_TIME       0x04F0 
#define REG_DUMMY         0x04FC

#define REG_EDCA_VO_PARAM     0x0500
#define REG_EDCA_VI_PARAM     0x0504
#define REG_EDCA_BE_PARAM     0x0508
#define REG_EDCA_BK_PARAM     0x050C
#define REG_BCNTCFG         0x0510
#define REG_PIFS            0x0512
#define REG_RDG_PIFS          0x0513
#define REG_SIFS_CTX          0x0514
#define REG_SIFS_TRX          0x0516
#define REG_TSFTR_SYN_OFFSET      0x0518
#define REG_AGGR_BREAK_TIME     0x051A
#define REG_SLOT            0x051B
#define REG_TX_PTCL_CTRL        0x0520
#define REG_TXPAUSE         0x0522
#define REG_DIS_TXREQ_CLR       0x0523
#define REG_RD_CTRL         0x0524
#define REG_TBTT_PROHIBIT       0x0540
#define REG_RD_NAV_NXT        0x0544
#define REG_NAV_PROT_LEN        0x0546
#define REG_BCN_CTRL          0x0550
#define REG_BCN_CTRL_1        0x0551
#define REG_MBID_NUM          0x0552
#define REG_DUAL_TSF_RST        0x0553
#define REG_BCN_INTERVAL        0x0554 
#define REG_DRVERLYINT        0x0558
#define REG_BCNDMATIM       0x0559
#define REG_ATIMWND         0x055A
#define REG_BCN_MAX_ERR       0x055D
#define REG_RXTSF_OFFSET_CCK      0x055E
#define REG_RXTSF_OFFSET_OFDM     0x055F
#define REG_TSFTR           0x0560
#define REG_TSFTR1          0x0568
#define REG_ATIMWND_1       0x0570
#define REG_PSTIMER         0x0580
#define REG_TIMER0          0x0584
#define REG_TIMER1          0x0588
#define REG_ACMHWCTRL       0x05C0

#define REG_FW_RESET_TSF_CNT_1        0x05FC
#define REG_FW_RESET_TSF_CNT_0        0x05FD
#define REG_FW_BCN_DIS_CNT        0x05FE

#define REG_APSD_CTRL         0x0600
#define REG_BWOPMODE        0x0603
#define REG_TCR           0x0604
#define REG_RCR           0x0608
#define REG_RX_PKT_LIMIT        0x060C
#define REG_RX_DLK_TIME       0x060D
#define REG_RX_DRVINFO_SZ       0x060F

#define REG_MACID           0x0610
#define REG_BSSID           0x0618
#define REG_MAR           0x0620
#define REG_MBIDCAMCFG        0x0628

#define REG_USTIME_EDCA       0x0638
#define REG_MAC_SPEC_SIFS       0x063A

#define REG_R2T_SIFS        0x063C 
#define REG_T2T_SIFS        0x063E 
#define REG_ACKTO         0x0640
#define REG_CTS2TO          0x0641
#define REG_EIFS            0x0642


#define RXERR_TYPE_OFDM_PPDU      0
#define RXERR_TYPE_OFDM_FALSE_ALARM   1
#define RXERR_TYPE_OFDM_MPDU_OK     2
#define RXERR_TYPE_OFDM_MPDU_FAIL   3
#define RXERR_TYPE_CCK_PPDU       4
#define RXERR_TYPE_CCK_FALSE_ALARM    5
#define RXERR_TYPE_CCK_MPDU_OK      6
#define RXERR_TYPE_CCK_MPDU_FAIL    7
#define RXERR_TYPE_HT_PPDU        8
#define RXERR_TYPE_HT_FALSE_ALARM   9
#define RXERR_TYPE_HT_MPDU_TOTAL    10
#define RXERR_TYPE_HT_MPDU_OK     11
#define RXERR_TYPE_HT_MPDU_FAIL     12
#define RXERR_TYPE_RX_FULL_DROP     15

#define RXERR_COUNTER_MASK        0xFFFFF
#define RXERR_RPT_RST           BIT(27)
#define _RXERR_RPT_SEL(type)        ((type) << 28)


#define REG_NAV_UPPER     0x0652 

#define REG_BACAMCMD        0x0654
#define REG_BACAMCONTENT      0x0658
#define REG_LBDLY         0x0660
#define REG_FWDLY         0x0661
#define REG_RXERR_RPT       0x0664
#define REG_WMAC_TRXPTCL_CTL    0x0668

#define REG_CAMCMD          0x0670
#define REG_CAMWRITE          0x0674
#define REG_CAMREAD         0x0678
#define REG_CAMDBG          0x067C
#define REG_SECCFG          0x0680

#define REG_WOW_CTRL          0x0690
#define REG_PS_RX_INFO        0x0692
#define REG_UAPSD_TID       0x0693
#define REG_WKFMCAM_CMD     0x0698
#define REG_WKFMCAM_NUM_88E       0x698
#define REG_RXFLTMAP0         0x06A0
#define REG_RXFLTMAP1         0x06A2
#define REG_RXFLTMAP2         0x06A4
#define REG_BCN_PSR_RPT       0x06A8
#define REG_BT_COEX_TABLE       0x06C0


#define REG_MACID1          0x0700
#define REG_BSSID1          0x0708


#define REG_USB_INFO          0xFE17
#define REG_USB_SPECIAL_OPTION      0xFE55
#define REG_USB_DMA_AGG_TO      0xFE5B
#define REG_USB_AGG_TO        0xFE5C
#define REG_USB_AGG_TH        0xFE5D


#define REG_NORMAL_SIE_VID      0xFE60   
#define REG_NORMAL_SIE_PID      0xFE62   
#define REG_NORMAL_SIE_OPTIONAL   0xFE64
#define REG_NORMAL_SIE_EP       0xFE65   
#define REG_NORMAL_SIE_PHY      0xFE68   
#define REG_NORMAL_SIE_OPTIONAL2    0xFE6C
#define REG_NORMAL_SIE_GPS_EP     0xFE6D 
#define REG_NORMAL_SIE_MAC_ADDR   0xFE70   
#define REG_NORMAL_SIE_STRING     0xFE80   




#define EFUSE_CTRL          REG_EFUSE_CTRL   
#define EFUSE_TEST          REG_EFUSE_TEST   
#define MSR             (REG_CR + 2)   
#define ISR             REG_HISR_88E
#define TSFR            REG_TSFTR    

#define   PBP             REG_PBP

#define IDR0            REG_MACID    
#define IDR4            (REG_MACID + 4)  


#define RWCAM           REG_CAMCMD   
#define WCAMI           REG_CAMWRITE 
#define RCAMO           REG_CAMREAD  
#define CAMDBG            REG_CAMDBG
#define SECR            REG_SECCFG   

#define UnusedRegister        0x1BF
#define DCAM            UnusedRegister
#define PSR             UnusedRegister
#define   BBAddr            UnusedRegister
#define PhyDataR          UnusedRegister

#define MAX_MSS_DENSITY_2T      0x13
#define MAX_MSS_DENSITY_1T      0x0A

#define CmdEEPROM_En        BIT5  
#define CmdEERPOMSEL        BIT4 
#define Cmd9346CR_9356SEL     BIT4

#define GPIOSEL_GPIO          0
#define GPIOSEL_ENBT          BIT5

#define GPIO_IN           REG_GPIO_PIN_CTRL  
#define GPIO_OUT          (REG_GPIO_PIN_CTRL+1)
#define GPIO_IO_SEL         (REG_GPIO_PIN_CTRL+2)
#define GPIO_MOD          (REG_GPIO_PIN_CTRL+3)

#define HSIMR_GPIO12_0_INT_EN   BIT0
#define HSIMR_SPS_OCP_INT_EN    BIT5
#define HSIMR_RON_INT_EN      BIT6
#define HSIMR_PDN_INT_EN      BIT7
#define HSIMR_GPIO9_INT_EN    BIT25


#define HSISR_GPIO12_0_INT      BIT0
#define HSISR_SPS_OCP_INT     BIT5
#define HSISR_RON_INT_EN      BIT6
#define HSISR_PDNINT          BIT7
#define HSISR_GPIO9_INT     BIT25

/*
Network Type
00: No link
01: Link in ad hoc network
10: Link in infrastructure network
11: AP mode
Default: 00b.
*/
#define MSR_NOLINK          0x00
#define MSR_ADHOC         0x01
#define MSR_INFRA         0x02
#define MSR_AP            0x03

#define USB_INTR_CONTENT_C2H_OFFSET   0
#define USB_INTR_CONTENT_CPWM1_OFFSET 16
#define USB_INTR_CONTENT_CPWM2_OFFSET 20
#define USB_INTR_CONTENT_HISR_OFFSET    48
#define USB_INTR_CONTENT_HISRE_OFFSET 52


#define CMD_INIT_LLT          BIT0
#define CMD_READ_EFUSE_MAP    BIT1
#define CMD_EFUSE_PATCH     BIT2
#define CMD_IOCONFIG        BIT3
#define CMD_INIT_LLT_ERR      BIT4
#define CMD_READ_EFUSE_MAP_ERR  BIT5
#define CMD_EFUSE_PATCH_ERR   BIT6
#define CMD_IOCONFIG_ERR      BIT7

#define RRSR_1M           BIT0
#define RRSR_2M           BIT1
#define RRSR_5_5M         BIT2
#define RRSR_11M          BIT3
#define RRSR_6M           BIT4
#define RRSR_9M           BIT5
#define RRSR_12M          BIT6
#define RRSR_18M          BIT7
#define RRSR_24M          BIT8
#define RRSR_36M          BIT9
#define RRSR_48M          BIT10
#define RRSR_54M          BIT11
#define RRSR_MCS0         BIT12
#define RRSR_MCS1         BIT13
#define RRSR_MCS2         BIT14
#define RRSR_MCS3         BIT15
#define RRSR_MCS4         BIT16
#define RRSR_MCS5         BIT17
#define RRSR_MCS6         BIT18
#define RRSR_MCS7         BIT19

#define HAL92C_WOL_PTK_UPDATE_EVENT   BIT0
#define HAL92C_WOL_GTK_UPDATE_EVENT   BIT1

#define BW_OPMODE_20MHZ     BIT2
#define BW_OPMODE_5G        BIT1


#define CAM_VALID         BIT15
#define CAM_NOTVALID        0x0000
#define CAM_USEDK         BIT5

#define CAM_CONTENT_COUNT     8

#define CAM_NONE          0x0
#define CAM_WEP40         0x01
#define CAM_TKIP          0x02
#define CAM_AES         0x04
#define CAM_WEP104        0x05
#define CAM_SMS4          0x6

#define TOTAL_CAM_ENTRY     32
#define HALF_CAM_ENTRY      16

#define CAM_CONFIG_USEDK      _TRUE
#define CAM_CONFIG_NO_USEDK   _FALSE

#define CAM_WRITE         BIT16
#define CAM_READ          0x00000000
#define CAM_POLLINIG        BIT31

#define SCR_UseDK         0x01
#define SCR_TxSecEnable     0x02
#define SCR_RxSecEnable     0x04

#define WOW_PMEN        BIT0
#define WOW_WOMEN     BIT1
#define WOW_MAGIC       BIT2
#define WOW_UWF       BIT3

#define IMR_DISABLED_88E          0x0
#define IMR_TXCCK_88E           BIT30  
#define IMR_PSTIMEOUT_88E         BIT29  
#define IMR_GTINT4_88E            BIT28  
#define IMR_GTINT3_88E            BIT27  
#define IMR_TBDER_88E           BIT26  
#define IMR_TBDOK_88E           BIT25  
#define IMR_TSF_BIT32_TOGGLE_88E      BIT24  
#define IMR_BCNDMAINT0_88E        BIT20  
#define IMR_BCNDERR0_88E          BIT16  
#define IMR_HSISR_IND_ON_INT_88E      BIT15  
#define IMR_BCNDMAINT_E_88E       BIT14  
#define IMR_ATIMEND_88E         BIT12  
#define IMR_HISR1_IND_INT_88E       BIT11  
#define IMR_C2HCMD_88E          BIT10  
#define IMR_CPWM2_88E           BIT9     
#define IMR_CPWM_88E            BIT8     
#define IMR_HIGHDOK_88E         BIT7     
#define IMR_MGNTDOK_88E         BIT6     
#define IMR_BKDOK_88E           BIT5     
#define IMR_BEDOK_88E           BIT4     
#define IMR_VIDOK_88E           BIT3     
#define IMR_VODOK_88E           BIT2     
#define IMR_RDU_88E           BIT1     
#define IMR_ROK_88E           BIT0     

#define IMR_BCNDMAINT7_88E        BIT27  
#define IMR_BCNDMAINT6_88E        BIT26  
#define IMR_BCNDMAINT5_88E        BIT25  
#define IMR_BCNDMAINT4_88E        BIT24  
#define IMR_BCNDMAINT3_88E        BIT23  
#define IMR_BCNDMAINT2_88E        BIT22  
#define IMR_BCNDMAINT1_88E        BIT21  
#define IMR_BCNDERR7_88E          BIT20  
#define IMR_BCNDERR6_88E          BIT19  
#define IMR_BCNDERR5_88E          BIT18  
#define IMR_BCNDERR4_88E          BIT17  
#define IMR_BCNDERR3_88E          BIT16  
#define IMR_BCNDERR2_88E          BIT15  
#define IMR_BCNDERR1_88E          BIT14  
#define IMR_ATIMEND_E_88E         BIT13  
#define IMR_TXERR_88E           BIT11  
#define IMR_RXERR_88E           BIT10  
#define IMR_TXFOVW_88E          BIT9     
#define IMR_RXFOVW_88E          BIT8     

#define HAL_NIC_UNPLUG_ISR    0xFFFFFFFF 


#ifdef CONFIG_PCI_HCI
#define IMR_TX_MASK     (IMR_VODOK_88E|IMR_VIDOK_88E|IMR_BEDOK_88E|IMR_BKDOK_88E|IMR_MGNTDOK_88E|IMR_HIGHDOK_88E|IMR_BCNDERR0_88E)

#ifdef CONFIG_CONCURRENT_MODE
#define RT_IBSS_INT_MASKS       (IMR_BCNDMAINT0_88E | IMR_TBDOK_88E | IMR_TBDER_88E | IMR_BCNDMAINT_E_88E)
#else
#define RT_IBSS_INT_MASKS       (IMR_BCNDMAINT0_88E | IMR_TBDOK_88E | IMR_TBDER_88E)
#endif

#define RT_AC_INT_MASKS       (IMR_VIDOK_88E | IMR_VODOK_88E | IMR_BEDOK_88E|IMR_BKDOK_88E)
#define RT_BSS_INT_MASKS        (RT_IBSS_INT_MASKS)
#endif


#define   HWSET_MAX_SIZE        256
#define   HWSET_MAX_SIZE_88E    512



/*===================================================================
=====================================================================
Here the register defines are for 92C. When the define is as same with 92C,
we will use the 92C's define for the consistency
So the following defines for 92C is not entire!!!!!!
=====================================================================
=====================================================================*/
/*
Based on Datasheet V33---090401
Register Summary
Current IOREG MAP
0x0000h ~ 0x00FFh   System Configuration (256 Bytes)
0x0100h ~ 0x01FFh   MACTOP General Configuration (256 Bytes)
0x0200h ~ 0x027Fh   TXDMA Configuration (128 Bytes)
0x0280h ~ 0x02FFh   RXDMA Configuration (128 Bytes)
0x0300h ~ 0x03FFh   PCIE EMAC Reserved Region (256 Bytes)
0x0400h ~ 0x04FFh   Protocol Configuration (256 Bytes)
0x0500h ~ 0x05FFh   EDCA Configuration (256 Bytes)
0x0600h ~ 0x07FFh   WMAC Configuration (512 Bytes)
0x2000h ~ 0x3FFFh   8051 FW Download Region (8196 Bytes)
*/
#define   StopBecon     BIT6
#define   StopHigh        BIT5
#define   StopMgt       BIT4
#define   StopBK        BIT3
#define   StopBE        BIT2
#define   StopVI        BIT1
#define   StopVO        BIT0

#define RCR_APPFCS          BIT31  
#define RCR_APP_MIC       BIT30  
#define RCR_APP_PHYSTS      BIT28//
#define RCR_APP_ICV       BIT29      
#define RCR_APP_PHYST_RXFF    BIT28      
#define RCR_APP_BA_SSN      BIT27  
#define RCR_ENMBID          BIT24  
#define RCR_LSIGEN          BIT23
#define RCR_MFBEN         BIT22
#define RCR_HTC_LOC_CTRL      BIT14      
#define RCR_AMF         BIT13  
#define RCR_ACF         BIT12  
#define RCR_ADF         BIT11  
#define RCR_AICV          BIT9   
#define RCR_ACRC32          BIT8   
#define RCR_CBSSID_BCN      BIT7   
#define RCR_CBSSID_DATA     BIT6   
#define RCR_CBSSID          RCR_CBSSID_DATA  
#define RCR_APWRMGT       BIT5   
#define RCR_ADD3          BIT4   
#define RCR_AB            BIT3   
#define RCR_AM            BIT2   
#define RCR_APM         BIT1   
#define RCR_AAP         BIT0   
#define RCR_MXDMA_OFFSET      8
#define RCR_FIFO_OFFSET     13

#define REG_USB_INFO          0xFE17
#define REG_USB_SPECIAL_OPTION      0xFE55
#define REG_USB_DMA_AGG_TO      0xFE5B
#define REG_USB_AGG_TO        0xFE5C
#define REG_USB_AGG_TH        0xFE5D

#define REG_USB_HRPWM       0xFE58
#define REG_USB_HCPWM       0xFE57


#define ISO_MD2PP           BIT(0)
#define ISO_UA2USB          BIT(1)
#define ISO_UD2CORE         BIT(2)
#define ISO_PA2PCIE         BIT(3)
#define ISO_PD2CORE         BIT(4)
#define ISO_IP2MAC          BIT(5)
#define ISO_DIOP            BIT(6)
#define ISO_DIOE            BIT(7)
#define ISO_EB2CORE         BIT(8)
#define ISO_DIOR            BIT(9)
#define PWC_EV12V           BIT(15)


#define FEN_BBRSTB          BIT(0)
#define FEN_BB_GLB_RSTn       BIT(1)
#define FEN_USBA            BIT(2)
#define FEN_UPLL            BIT(3)
#define FEN_USBD            BIT(4)
#define FEN_DIO_PCIE          BIT(5)
#define FEN_PCIEA           BIT(6)
#define FEN_PPLL            BIT(7)
#define FEN_PCIED           BIT(8)
#define FEN_DIOE            BIT(9)
#define FEN_CPUEN           BIT(10)
#define FEN_DCORE           BIT(11)
#define FEN_ELDR            BIT(12)
#define FEN_DIO_RF          BIT(13)
#define FEN_HWPDN         BIT(14)
#define FEN_MREGEN          BIT(15)

#define PFM_LDALL           BIT(0)
#define PFM_ALDN            BIT(1)
#define PFM_LDKP            BIT(2)
#define PFM_WOWL          BIT(3)
#define EnPDN             BIT(4)
#define PDN_PL            BIT(5)
#define APFM_ONMAC          BIT(8)
#define APFM_OFF            BIT(9)
#define APFM_RSM            BIT(10)
#define AFSM_HSUS           BIT(11)
#define AFSM_PCIE           BIT(12)
#define APDM_MAC          BIT(13)
#define APDM_HOST         BIT(14)
#define APDM_HPDN         BIT(15)
#define RDY_MACON         BIT(16)
#define SUS_HOST            BIT(17)
#define ROP_ALD           BIT(20)
#define ROP_PWR           BIT(21)
#define ROP_SPS           BIT(22)
#define SOP_MRST            BIT(25)
#define SOP_FUSE            BIT(26)
#define SOP_ABG           BIT(27)
#define SOP_AMB           BIT(28)
#define SOP_RCK           BIT(29)
#define SOP_A8M           BIT(30)
#define XOP_BTCK            BIT(31)

#define ANAD16V_EN          BIT(0)
#define ANA8M           BIT(1)
#define MACSLP            BIT(4)
#define LOADER_CLK_EN         BIT(5)



#define   BOOT_FROM_EEPROM    BIT(4)
#define   EEPROM_EN       BIT(5)






#define RF_EN           BIT(0)
#define RF_RSTB         BIT(1)
#define RF_SDMRSTB        BIT(2)



#define LDV12_EN          BIT(0)
#define LDV12_SDBY        BIT(1)
#define LPLDO_HSM         BIT(2)
#define LPLDO_LSM_DIS       BIT(3)
#define _LDV12_VADJ(x)        (((x) & 0xF) << 4)


#define ALD_EN            BIT(18)
#define EF_PD           BIT(19)
#define EF_FLAG           BIT(31)

#define EF_TRPT           BIT(7)
#define EF_CELL_SEL           (BIT(8)|BIT(9))
#define LDOE25_EN           BIT(31)
#define EFUSE_SEL(x)          (((x) & 0x3) << 8)
#define EFUSE_SEL_MASK        0x300
#define EFUSE_WIFI_SEL_0        0x0
#define EFUSE_BT_SEL_0          0x1
#define EFUSE_BT_SEL_1          0x2
#define EFUSE_BT_SEL_2          0x3

#define EFUSE_ACCESS_ON     0x69 
#define EFUSE_ACCESS_OFF      0x00 


#define MCUFWDL_EN          BIT(0)
#define MCUFWDL_RDY         BIT(1)
#define FWDL_ChkSum_rpt       BIT(2)
#define MACINI_RDY          BIT(3)
#define BBINI_RDY           BIT(4)
#define RFINI_RDY           BIT(5)
#define WINTINI_RDY         BIT(6)
#define RAM_DL_SEL          BIT(7)
#define ROM_DLEN            BIT(19)
#define CPRST             BIT(23)



#define XCLK_VLD            BIT(0)
#define ACLK_VLD            BIT(1)
#define UCLK_VLD            BIT(2)
#define PCLK_VLD            BIT(3)
#define PCIRSTB             BIT(4)
#define V15_VLD             BIT(5)
#define SW_OFFLOAD_EN         BIT(7)
#define SIC_IDLE              BIT(8)
#define BD_MAC2             BIT(9)
#define BD_MAC1             BIT(10)
#define IC_MACPHY_MODE        BIT(11)
#define CHIP_VER            (BIT(12)|BIT(13)|BIT(14)|BIT(15))
#define BT_FUNC             BIT(16)
#define VENDOR_ID           BIT(19)
#define PAD_HWPD_IDN          BIT(22)
#define TRP_VAUX_EN           BIT(23)
#define TRP_BT_EN           BIT(24)
#define BD_PKG_SEL            BIT(25)
#define BD_HCI_SEL            BIT(26)
#define TYPE_ID             BIT(27)

#define CHIP_VER_RTL_MASK       0xF000 
#define CHIP_VER_RTL_SHIFT        12


#define EFS_HCI_SEL       (BIT(0)|BIT(1))
#define PAD_HCI_SEL       (BIT(2)|BIT(3))
#define HCI_SEL           (BIT(4)|BIT(5))
#define PKG_SEL_HCI       BIT(6)
#define FEN_GPS         BIT(7)
#define FEN_BT            BIT(8)
#define FEN_WL            BIT(9)
#define FEN_PCI           BIT(10)
#define FEN_USB         BIT(11)
#define BTRF_HWPDN_N        BIT(12)
#define WLRF_HWPDN_N        BIT(13)
#define PDN_BT_N          BIT(14)
#define PDN_GPS_N         BIT(15)
#define BT_CTL_HWPDN        BIT(16)
#define GPS_CTL_HWPDN       BIT(17)
#define PPHY_SUSB         BIT(20)
#define UPHY_SUSB         BIT(21)
#define PCI_SUSEN         BIT(22)
#define USB_SUSEN         BIT(23)
#define RF_RL_ID          (BIT(31)|BIT(30)|BIT(29)|BIT(28))


#define RTL_ID          BIT(23)






#define HCI_TXDMA_EN          BIT(0)
#define HCI_RXDMA_EN          BIT(1)
#define TXDMA_EN            BIT(2)
#define RXDMA_EN            BIT(3)
#define PROTOCOL_EN         BIT(4)
#define SCHEDULE_EN         BIT(5)
#define MACTXEN           BIT(6)
#define MACRXEN           BIT(7)
#define ENSWBCN           BIT(8)
#define ENSEC             BIT(9)
#define CALTMR_EN         BIT(10)

#define _NETTYPE(x)         (((x) & 0x3) << 16)
#define MASK_NETTYPE          0x30000
#define NT_NO_LINK          0x0
#define NT_LINK_AD_HOC        0x1
#define NT_LINK_AP            0x2
#define NT_AS_AP            0x3



#define GET_RX_PAGE_SIZE(value)     ((value) & 0xF)
#define GET_TX_PAGE_SIZE(value)     (((value) & 0xF0) >> 4)
#define _PSRX_MASK          0xF
#define _PSTX_MASK          0xF0
#define _PSRX(x)            (x)
#define _PSTX(x)            ((x) << 4)

#define PBP_64            0x0
#define PBP_128           0x1
#define PBP_256           0x2
#define PBP_512           0x3
#define PBP_1024            0x4


#define RXDMA_ARBBW_EN        BIT(0)
#define RXSHFT_EN           BIT(1)
#define RXDMA_AGG_EN          BIT(2)
#define QS_VO_QUEUE         BIT(8)
#define QS_VI_QUEUE         BIT(9)
#define QS_BE_QUEUE         BIT(10)
#define QS_BK_QUEUE         BIT(11)
#define QS_MANAGER_QUEUE      BIT(12)
#define QS_HIGH_QUEUE         BIT(13)

#define HQSEL_VOQ           BIT(0)
#define HQSEL_VIQ           BIT(1)
#define HQSEL_BEQ           BIT(2)
#define HQSEL_BKQ           BIT(3)
#define HQSEL_MGTQ          BIT(4)
#define HQSEL_HIQ           BIT(5)

#define _TXDMA_HIQ_MAP(x)       (((x)&0x3) << 14)
#define _TXDMA_MGQ_MAP(x)       (((x)&0x3) << 12)
#define _TXDMA_BKQ_MAP(x)       (((x)&0x3) << 10)
#define _TXDMA_BEQ_MAP(x)       (((x)&0x3) << 8 )
#define _TXDMA_VIQ_MAP(x)       (((x)&0x3) << 6 )
#define _TXDMA_VOQ_MAP(x)       (((x)&0x3) << 4 )

#define QUEUE_LOW         1
#define QUEUE_NORMAL          2
#define QUEUE_HIGH          3





#define _LLT_NO_ACTIVE          0x0
#define _LLT_WRITE_ACCESS       0x1
#define _LLT_READ_ACCESS        0x2

#define _LLT_INIT_DATA(x)       ((x) & 0xFF)
#define _LLT_INIT_ADDR(x)       (((x) & 0xFF) << 8)
#define _LLT_OP(x)            (((x) & 0x3) << 30)
#define _LLT_OP_VALUE(x)        (((x) >> 30) & 0x3)


#define _HPQ(x)           ((x) & 0xFF)
#define _LPQ(x)           (((x) & 0xFF) << 8)
#define _PUBQ(x)            (((x) & 0xFF) << 16)
#define _NPQ(x)           ((x) & 0xFF)     


#define HPQ_PUBLIC_DIS        BIT(24)
#define LPQ_PUBLIC_DIS        BIT(25)
#define LD_RQPN           BIT(31)


#define BCN_VALID         BIT(16)
#define BCN_HEAD(x)         (((x) & 0xFF) << 8)
#define BCN_HEAD_MASK       0xFF00


#define BLK_DESC_NUM_SHIFT      4
#define BLK_DESC_NUM_MASK     0xF


#define DROP_DATA_EN        BIT(9)






#define   RXPKT_RELEASE_POLL  BIT(16)
#define RXDMA_IDLE        BIT(17)
#define RW_RELEASE_EN     BIT(18)

#define EN_AMPDU_RTY_NEW      BIT(7)


#define _SPEC_SIFS_CCK(x)       ((x) & 0xFF)
#define _SPEC_SIFS_OFDM(x)        (((x) & 0xFF) << 8)

#define RETRY_LIMIT_SHORT_SHIFT   8
#define RETRY_LIMIT_LONG_SHIFT    0








#define AC_PARAM_TXOP_LIMIT_OFFSET  16
#define AC_PARAM_ECW_MAX_OFFSET   12
#define AC_PARAM_ECW_MIN_OFFSET   8
#define AC_PARAM_AIFS_OFFSET      0



#define _LRL(x)             ((x) & 0x3F)
#define _SRL(x)             (((x) & 0x3F) << 8)


#define EN_MBSSID       BIT(1)
#define EN_TXBCN_RPT        BIT(2)
#define EN_BCN_FUNCTION       BIT(3)
#define DIS_TSF_UPDATE        BIT(3)

#define DIS_TSF_UDT0_NORMAL_CHIP    BIT(4)
#define DIS_TSF_UDT0_TEST_CHIP      BIT(5)
#define STOP_BCNQ       BIT(6)


#define AcmHw_HwEn          BIT(0)
#define AcmHw_BeqEn       BIT(1)
#define AcmHw_ViqEn         BIT(2)
#define AcmHw_VoqEn       BIT(3)
#define AcmHw_BeqStatus       BIT(4)
#define AcmHw_ViqStatus       BIT(5)
#define AcmHw_VoqStatus       BIT(6)



#define APSDOFF           BIT(6)
#define APSDOFF_STATUS        BIT(7)


#define RATE_BITMAP_ALL       0xFFFFF

#define RATE_RRSR_CCK_ONLY_1M   0xFFFF1

#define TSFRST              BIT(0)
#define DIS_GCLK            BIT(1)
#define PAD_SEL           BIT(2)
#define PWR_ST            BIT(6)
#define PWRBIT_OW_EN          BIT(7)
#define ACRC              BIT(8)
#define CFENDFORM         BIT(9)
#define ICV             BIT(10)



#define AAP             BIT(0)
#define APM             BIT(1)
#define AM              BIT(2)
#define AB              BIT(3)
#define ADD3              BIT(4)
#define APWRMGT           BIT(5)
#define CBSSID              BIT(6)
#define CBSSID_DATA         BIT(6)
#define CBSSID_BCN          BIT(7)
#define ACRC32            BIT(8)
#define AICV              BIT(9)
#define ADF             BIT(11)
#define ACF             BIT(12)
#define AMF             BIT(13)
#define HTC_LOC_CTRL          BIT(14)
#define UC_DATA_EN          BIT(16)
#define BM_DATA_EN          BIT(17)
#define MFBEN           BIT(22)
#define LSIGEN              BIT(23)
#define EnMBID            BIT(24)
#define APP_BASSN           BIT(27)
#define APP_PHYSTS          BIT(28)
#define APP_ICV           BIT(29)
#define APP_MIC           BIT(30)
#define APP_FCS           BIT(31)


#define SCR_TxUseDK           BIT(0)     
#define SCR_RxUseDK           BIT(1)     
#define SCR_TxEncEnable         BIT(2)     
#define SCR_RxDecEnable         BIT(3)     
#define SCR_SKByA2            BIT(4)     
#define SCR_NoSKMC            BIT(5)     
#define   SCR_TXBCUSEDK         BIT(6)     
#define   SCR_RXBCUSEDK         BIT(7)     



#define SDIO_LOCAL_BASE       0x10250000
#define WLAN_IOREG_BASE       0x10260000
#define FIRMWARE_FIFO_BASE      0x10270000
#define TX_HIQ_BASE       0x10310000
#define TX_MIQ_BASE       0x10320000
#define TX_LOQ_BASE       0x10330000
#define RX_RX0FF_BASE       0x10340000

#define SDIO_LOCAL_MSK        0x0FFF
#define WLAN_IOREG_MSK        0x7FFF
#define WLAN_FIFO_MSK             0x1FFF 
#define WLAN_RX0FF_MSK              0x0003

#define SDIO_WITHOUT_REF_DEVICE_ID    0
#define SDIO_LOCAL_DEVICE_ID      0
#define WLAN_TX_HIQ_DEVICE_ID     4
#define WLAN_TX_MIQ_DEVICE_ID     5
#define WLAN_TX_LOQ_DEVICE_ID     6
#define WLAN_RX0FF_DEVICE_ID      7
#define WLAN_IOREG_DEVICE_ID      8

#define HI_QUEUE_IDX        0
#define MID_QUEUE_IDX       1
#define LOW_QUEUE_IDX       2
#define PUBLIC_QUEUE_IDX      3

#define SDIO_MAX_TX_QUEUE     3  
#define SDIO_MAX_RX_QUEUE     1

#define SDIO_REG_TX_CTRL      0x0000
#define SDIO_REG_HIMR       0x0014
#define SDIO_REG_HISR       0x0018
#define SDIO_REG_HCPWM        0x0019
#define SDIO_REG_RX0_REQ_LEN      0x001C
#define SDIO_REG_FREE_TXPG      0x0020
#define SDIO_REG_HCPWM1       0x0024
#define SDIO_REG_HCPWM2       0x0026
#define SDIO_REG_HTSFR_INFO     0x0030
#define SDIO_REG_HRPWM1       0x0080
#define SDIO_REG_HRPWM2       0x0082
#define SDIO_REG_HPS_CLKR     0x0084
#define SDIO_REG_HSUS_CTRL      0x0086
#define SDIO_REG_HIMR_ON      0x0090
#define SDIO_REG_HISR_ON      0x0091

#define SDIO_HIMR_DISABLED      0

#define SDIO_HIMR_RX_REQUEST_MSK    BIT0
#define SDIO_HIMR_AVAL_MSK      BIT1
#define SDIO_HIMR_TXERR_MSK     BIT2
#define SDIO_HIMR_RXERR_MSK     BIT3
#define SDIO_HIMR_TXFOVW_MSK      BIT4
#define SDIO_HIMR_RXFOVW_MSK      BIT5
#define SDIO_HIMR_TXBCNOK_MSK     BIT6
#define SDIO_HIMR_TXBCNERR_MSK      BIT7
#define SDIO_HIMR_BCNERLY_INT_MSK   BIT16
#define SDIO_HIMR_C2HCMD_MSK      BIT17
#define SDIO_HIMR_CPWM1_MSK     BIT18
#define SDIO_HIMR_CPWM2_MSK     BIT19
#define SDIO_HIMR_HSISR_IND_MSK     BIT20
#define SDIO_HIMR_GTINT3_IND_MSK    BIT21
#define SDIO_HIMR_GTINT4_IND_MSK    BIT22
#define SDIO_HIMR_PSTIMEOUT_MSK     BIT23
#define SDIO_HIMR_OCPINT_MSK      BIT24
#define SDIO_HIMR_ATIMEND_MSK     BIT25
#define SDIO_HIMR_ATIMEND_E_MSK     BIT26
#define SDIO_HIMR_CTWEND_MSK      BIT27

#define SDIO_HIMR_MCU_ERR_MSK     BIT28
#define SDIO_HIMR_TSF_BIT32_TOGGLE_MSK  BIT29

#define SDIO_HISR_RX_REQUEST      BIT0
#define SDIO_HISR_AVAL        BIT1
#define SDIO_HISR_TXERR       BIT2
#define SDIO_HISR_RXERR       BIT3
#define SDIO_HISR_TXFOVW      BIT4
#define SDIO_HISR_RXFOVW      BIT5
#define SDIO_HISR_TXBCNOK     BIT6
#define SDIO_HISR_TXBCNERR      BIT7
#define SDIO_HISR_BCNERLY_INT     BIT16
#define SDIO_HISR_C2HCMD      BIT17
#define SDIO_HISR_CPWM1       BIT18
#define SDIO_HISR_CPWM2       BIT19
#define SDIO_HISR_HSISR_IND     BIT20
#define SDIO_HISR_GTINT3_IND      BIT21
#define SDIO_HISR_GTINT4_IND      BIT22
#define SDIO_HISR_PSTIMEOUT     BIT23
#define SDIO_HISR_OCPINT      BIT24
#define SDIO_HISR_ATIMEND     BIT25
#define SDIO_HISR_ATIMEND_E     BIT26
#define SDIO_HISR_CTWEND      BIT27

#define SDIO_HISR_MCU_ERR         BIT28
#define SDIO_HISR_TSF_BIT32_TOGGLE    BIT29

#define MASK_SDIO_HISR_CLEAR    (SDIO_HISR_TXERR |\
                                 SDIO_HISR_RXERR |\
                                 SDIO_HISR_TXFOVW |\
                                 SDIO_HISR_RXFOVW |\
                                 SDIO_HISR_TXBCNOK |\
                                 SDIO_HISR_TXBCNERR |\
                                 SDIO_HISR_C2HCMD |\
                                 SDIO_HISR_CPWM1 |\
                                 SDIO_HISR_CPWM2 |\
                                 SDIO_HISR_HSISR_IND |\
                                 SDIO_HISR_GTINT3_IND |\
                                 SDIO_HISR_GTINT4_IND |\
                                 SDIO_HISR_PSTIMEOUT |\
                                 SDIO_HISR_OCPINT)

#define HCI_RESUME_PWR_RDY      BIT1
#define HCI_SUS_CTRL        BIT0

#define SDIO_TX_FREE_PG_QUEUE     4
#define SDIO_TX_FIFO_PAGE_SZ      128

/* move to rtl8188e_xmit.h
#if DEV_BUS_TYPE == RT_SDIO_INTERFACE
  #define   MAX_TX_AGG_PACKET_NUMBER  0x8
#else
  #define   MAX_TX_AGG_PACKET_NUMBER  0xFF
#endif
*/



#define USB_IS_HIGH_SPEED         0
#define USB_IS_FULL_SPEED         1
#define USB_SPEED_MASK          BIT(5)

#define USB_NORMAL_SIE_EP_MASK      0xF
#define USB_NORMAL_SIE_EP_SHIFT     4


#define USB_AGG_EN            BIT(3)

#define INT_BULK_SEL          BIT(4)



#define   C2H_EVT_HOST_CLOSE      0x00 
#define   C2H_EVT_FW_CLOSE      0xFF 


#define WL_HWPDN_EN       BIT0   
#define WL_HWPDN_SL       BIT1   
#define WL_FUNC_EN        BIT2   
#define WL_HWROF_EN       BIT3   
#define BT_HWPDN_EN       BIT16
#define BT_HWPDN_SL       BIT17
#define BT_FUNC_EN          BIT18
#define BT_HWROF_EN       BIT19
#define GPS_HWPDN_EN        BIT20
#define GPS_HWPDN_SL        BIT21
#define GPS_FUNC_EN       BIT22


#define HAL92C_EN_PKT_LIFE_TIME_BK    BIT3
#define HAL92C_EN_PKT_LIFE_TIME_BE    BIT2
#define HAL92C_EN_PKT_LIFE_TIME_VI    BIT1
#define HAL92C_EN_PKT_LIFE_TIME_VO    BIT0

#define HAL92C_MSDU_LIFE_TIME_UNIT    128  

#define LAST_ENTRY_OF_TX_PKT_BUFFER   176

#define POLLING_LLT_THRESHOLD       20
#define POLLING_READY_TIMEOUT_COUNT   1000
#define HAL_8192C_HW_GPIO_WPS_BIT   BIT2

#if 0//(RTL8188E_SUPPORT == 1)
#define IMR_DISABLED_88E          0x0
#define IMR_TXCCK_88E         BIT30  
#define IMR_PSTIMEOUT_88E       BIT29  
#define IMR_GTINT4_88E          BIT28  
#define IMR_GTINT3_88E          BIT27  
#define IMR_TBDER_88E         BIT26  
#define IMR_TBDOK_88E         BIT25  
#define IMR_TSF_BIT32_TOGGLE_88E    BIT24  
#define IMR_BcnInt_88E          BIT20  
#define IMR_BDOK_88E            BIT16  
#define IMR_HSISR_IND_ON_INT_88E    BIT15  
#define IMR_BCNDMAINT_E_88E       BIT14  
#define IMR_ATIMEND_88E         BIT12  
#define IMR_HISR1_IND_INT_88E     BIT11  
#define IMR_C2HCMD_88E          BIT10  
#define IMR_CPWM2_88E         BIT9     
#define IMR_CPWM_88E            BIT8     
#define IMR_HIGHDOK_88E         BIT7     
#define IMR_MGNTDOK_88E         BIT6     
#define IMR_BKDOK_88E         BIT5     
#define IMR_BEDOK_88E         BIT4     
#define IMR_VIDOK_88E         BIT3     
#define IMR_VODOK_88E         BIT2     
#define IMR_RDU_88E           BIT1     
#define IMR_ROK_88E           BIT0     

#define IMR_BCNDMAINT7_88E        BIT27  
#define IMR_BCNDMAINT6_88E        BIT26  
#define IMR_BCNDMAINT5_88E        BIT25  
#define IMR_BCNDMAINT4_88E        BIT24  
#define IMR_BCNDMAINT3_88E        BIT23  
#define IMR_BCNDMAINT2_88E        BIT22  
#define IMR_BCNDMAINT1_88E        BIT21  
#define IMR_BCNDOK7_88E         BIT20  
#define IMR_BCNDOK6_88E         BIT19  
#define IMR_BCNDOK5_88E         BIT18  
#define IMR_BCNDOK4_88E         BIT17  
#define IMR_BCNDOK3_88E         BIT16  
#define IMR_BCNDOK2_88E         BIT15  
#define IMR_BCNDOK1_88E         BIT14  
#define IMR_ATIMEND_E_88E       BIT13  
#define IMR_TXERR_88E         BIT11  
#define IMR_RXERR_88E         BIT10  
#define IMR_TXFOVW_88E          BIT9     
#define IMR_RXFOVW_88E          BIT8     



#endif



#define EEPROM_TX_PWR_INX_88E       0x10

#define EEPROM_ChannelPlan_88E        0xB8
#define EEPROM_XTAL_88E           0xB9
#define EEPROM_THERMAL_METER_88E      0xBA
#define EEPROM_IQK_LCK_88E          0xBB

#define EEPROM_RF_BOARD_OPTION_88E      0xC1
#define EEPROM_RF_FEATURE_OPTION_88E    0xC2
#define EEPROM_RF_BT_SETTING_88E        0xC3
#define EEPROM_VERSION_88E          0xC4
#define EEPROM_CUSTOMERID_88E       0xC5
#define EEPROM_RF_ANTENNA_OPT_88E     0xC9

#ifdef CONFIG_RF_GAIN_OFFSET
#define EEPROM_RF_GAIN_OFFSET_88E     0xFD
#endif

#define EEPROM_MAC_ADDR_88EE        0xD0
#define EEPROM_VID_88EE           0xD6
#define EEPROM_DID_88EE           0xD8
#define EEPROM_SVID_88EE            0xDA
#define EEPROM_SMID_88EE            0xDC

#define EEPROM_MAC_ADDR_88EU        0xD7
#define EEPROM_VID_88EU           0xD0
#define EEPROM_PID_88EU           0xD2
#define EEPROM_USB_OPTIONAL_FUNCTION0     0xD4

#define EEPROM_MAC_ADDR_88ES        0x11A




#define EETYPE_TX_PWR             0x0

#define EEPROM_Default_TSSI         0x0
#define EEPROM_Default_TxPowerDiff      0x0
#define EEPROM_Default_CrystalCap     0x5
#define EEPROM_Default_BoardType      0x02
#define EEPROM_Default_TxPower        0x1010
#define EEPROM_Default_HT2T_TxPwr     0x10

#define EEPROM_Default_LegacyHTTxPowerDiff  0x3
#define EEPROM_Default_ThermalMeter     0x12

#define EEPROM_Default_AntTxPowerDiff   0x0
#define EEPROM_Default_TxPwDiff_CrystalCap  0x5
#if (RTL8188ES_SUPPORT==1)
#define EEPROM_Default_TxPowerLevel     0x25
#else
#define EEPROM_Default_TxPowerLevel     0x2A
#endif

#define EEPROM_Default_HT40_2SDiff      0x0
#define EEPROM_Default_HT20_Diff      2
#define EEPROM_Default_LegacyHTTxPowerDiff  0x3
#define EEPROM_Default_HT40_PwrMaxOffset  0
#define EEPROM_Default_HT20_PwrMaxOffset  0

#define EEPROM_Default_CrystalCap_88E   0x20
#define EEPROM_Default_ThermalMeter_88E   0x18

#ifdef CONFIG_RF_GAIN_OFFSET
#define EEPROM_Default_RFGainOffset     0xff
#endif

#define   EEPROM_DEFAULT_24G_INDEX    0x2D
#define   EEPROM_DEFAULT_24G_HT20_DIFF  0X02
#define   EEPROM_DEFAULT_24G_OFDM_DIFF  0X04

#define   EEPROM_DEFAULT_5G_INDEX     0X2A
#define   EEPROM_DEFAULT_5G_HT20_DIFF   0X00
#define   EEPROM_DEFAULT_5G_OFDM_DIFF   0X04

#define   EEPROM_DEFAULT_DIFF       0XFE
#define EEPROM_DEFAULT_CHANNEL_PLAN 0x7F
#define EEPROM_DEFAULT_BOARD_OPTION 0x00
#define EEPROM_DEFAULT_FEATURE_OPTION 0x00
#define EEPROM_DEFAULT_BT_OPTION    0x10


#define EEPROM_Default_PID          0x1234
#define EEPROM_Default_VID          0x5678
#define EEPROM_Default_CustomerID     0xAB
#define EEPROM_Default_CustomerID_8188E   0x00
#define EEPROM_Default_SubCustomerID    0xCD
#define EEPROM_Default_Version        0

#define EEPROM_CHANNEL_PLAN_FCC       0x0
#define EEPROM_CHANNEL_PLAN_IC        0x1
#define EEPROM_CHANNEL_PLAN_ETSI        0x2
#define EEPROM_CHANNEL_PLAN_SPAIN     0x3
#define EEPROM_CHANNEL_PLAN_FRANCE      0x4
#define EEPROM_CHANNEL_PLAN_MKK       0x5
#define EEPROM_CHANNEL_PLAN_MKK1        0x6
#define EEPROM_CHANNEL_PLAN_ISRAEL      0x7
#define EEPROM_CHANNEL_PLAN_TELEC     0x8
#define EEPROM_CHANNEL_PLAN_GLOBAL_DOMAIN 0x9
#define EEPROM_CHANNEL_PLAN_WORLD_WIDE_13 0xA
#define EEPROM_CHANNEL_PLAN_NCC       0xB
#define EEPROM_USB_OPTIONAL1          0xE
#define EEPROM_CHANNEL_PLAN_BY_HW_MASK    0x80


#define EEPROM_CID_DEFAULT        0x0
#define EEPROM_CID_TOSHIBA          0x4
#define EEPROM_CID_CCX            0x10
#define EEPROM_CID_QMI            0x0D
#define EEPROM_CID_WHQL           0xFE


#define RTL_EEPROM_ID           0x8129

#endif

