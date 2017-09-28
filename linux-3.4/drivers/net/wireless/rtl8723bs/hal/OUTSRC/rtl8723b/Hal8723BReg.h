/*****************************************************************************
 *	Copyright(c) 2009,  RealTEK Technology Inc. All Right Reserved.
 *
 * Module:	__INC_HAL8723BREG_H
 *
 *
 * Note:	1. Define Mac register address and corresponding bit mask map
 *			
 *
 * Export:	Constants, macro, functions(API), global variables(None).
 *
 * Abbrev:	
 *
 * History:
 *		Data		Who		Remark 
 * 
 *****************************************************************************/
#ifndef __INC_HAL8723BREG_H
#define __INC_HAL8723BREG_H




#define REG_SYS_ISO_CTRL_8723B			0x0000
#define REG_SYS_FUNC_EN_8723B			0x0002
#define REG_APS_FSMCO_8723B			0x0004
#define REG_SYS_CLKR_8723B				0x0008
#define REG_9346CR_8723B				0x000A
#define REG_EE_VPD_8723B				0x000C
#define REG_AFE_MISC_8723B				0x0010
#define REG_SPS0_CTRL_8723B				0x0011
#define REG_SPS_OCP_CFG_8723B			0x0018
#define REG_RSV_CTRL_8723B				0x001C
#define REG_RF_CTRL_8723B				0x001F
#define REG_LPLDO_CTRL_8723B			0x0023
#define REG_AFE_XTAL_CTRL_8723B		0x0024
#define REG_AFE_PLL_CTRL_8723B			0x0028
#define REG_MAC_PLL_CTRL_EXT_8723B		0x002c 
#define REG_EFUSE_CTRL_8723B			0x0030
#define REG_EFUSE_TEST_8723B			0x0034
#define REG_PWR_DATA_8723B				0x0038
#define REG_CAL_TIMER_8723B				0x003C
#define REG_ACLK_MON_8723B				0x003E
#define REG_GPIO_MUXCFG_8723B			0x0040
#define REG_GPIO_IO_SEL_8723B			0x0042
#define REG_MAC_PINMUX_CFG_8723B		0x0043
#define REG_GPIO_PIN_CTRL_8723B			0x0044
#define REG_GPIO_INTM_8723B				0x0048
#define REG_LEDCFG0_8723B				0x004C
#define REG_LEDCFG1_8723B				0x004D
#define REG_LEDCFG2_8723B				0x004E
#define REG_LEDCFG3_8723B				0x004F
#define REG_FSIMR_8723B					0x0050
#define REG_FSISR_8723B					0x0054
#define REG_HSIMR_8723B					0x0058
#define REG_HSISR_8723B					0x005c
#define REG_GPIO_EXT_CTRL				0x0060
#define REG_MULTI_FUNC_CTRL_8723B		0x0068
#define REG_GPIO_STATUS_8723B			0x006C
#define REG_SDIO_CTRL_8723B				0x0070
#define REG_OPT_CTRL_8723B				0x0074
#define REG_AFE_XTAL_CTRL_EXT_8723B	0x0078
#define REG_MCUFWDL_8723B				0x0080
#define REG_BT_PATCH_STATUS_8723B		0x0088
#define REG_HIMR0_8723B					0x00B0
#define REG_HISR0_8723B					0x00B4
#define REG_HIMR1_8723B					0x00B8
#define REG_HISR1_8723B					0x00BC
#define REG_PMC_DBG_CTRL2_8723B			0x00CC
#define	REG_EFUSE_BURN_GNT_8723B		0x00CF
#define REG_HPON_FSM_8723B				0x00EC
#define REG_SYS_CFG_8723B				0x00F0
#define REG_SYS_CFG1_8723B				0x00FC
#define REG_ROM_VERSION					0x00FD

#define REG_CR_8723B						0x0100
#define REG_PBP_8723B					0x0104
#define REG_PKT_BUFF_ACCESS_CTRL_8723B	0x0106
#define REG_TRXDMA_CTRL_8723B			0x010C
#define REG_TRXFF_BNDY_8723B			0x0114
#define REG_TRXFF_STATUS_8723B			0x0118
#define REG_RXFF_PTR_8723B				0x011C
#define REG_CPWM_8723B					0x012F
#define REG_FWIMR_8723B					0x0130
#define REG_FWISR_8723B					0x0134
#define REG_FTIMR_8723B					0x0138
#define REG_PKTBUF_DBG_CTRL_8723B		0x0140
#define REG_RXPKTBUF_CTRL_8723B		0x0142
#define REG_PKTBUF_DBG_DATA_L_8723B	0x0144
#define REG_PKTBUF_DBG_DATA_H_8723B	0x0148

#define REG_TC0_CTRL_8723B				0x0150
#define REG_TC1_CTRL_8723B				0x0154
#define REG_TC2_CTRL_8723B				0x0158
#define REG_TC3_CTRL_8723B				0x015C
#define REG_TC4_CTRL_8723B				0x0160
#define REG_TCUNIT_BASE_8723B			0x0164
#define REG_RSVD3_8723B					0x0168
#define REG_C2HEVT_MSG_NORMAL_8723B	0x01A0
#define REG_C2HEVT_CMD_SEQ_88XX		0x01A1
#define REG_C2hEVT_CMD_CONTENT_88XX	0x01A2
#define REG_C2HEVT_CMD_LEN_88XX		0x01AE
#define REG_C2HEVT_CLEAR_8723B			0x01AF
#define REG_MCUTST_1_8723B				0x01C0
#define REG_MCUTST_WOWLAN_8723B		0x01C7
#define REG_FMETHR_8723B				0x01C8
#define REG_HMETFR_8723B				0x01CC
#define REG_HMEBOX_0_8723B				0x01D0
#define REG_HMEBOX_1_8723B				0x01D4
#define REG_HMEBOX_2_8723B				0x01D8
#define REG_HMEBOX_3_8723B				0x01DC
#define REG_LLT_INIT_8723B				0x01E0
#define REG_HMEBOX_EXT0_8723B			0x01F0
#define REG_HMEBOX_EXT1_8723B			0x01F4
#define REG_HMEBOX_EXT2_8723B			0x01F8
#define REG_HMEBOX_EXT3_8723B			0x01FC

#define REG_RQPN_8723B					0x0200
#define REG_FIFOPAGE_8723B				0x0204
#define REG_DWBCN0_CTRL_8723B			REG_TDECTRL
#define REG_TXDMA_OFFSET_CHK_8723B	0x020C
#define REG_TXDMA_STATUS_8723B		0x0210
#define REG_RQPN_NPQ_8723B			0x0214
#define REG_DWBCN1_CTRL_8723B			0x0228


#define REG_RXDMA_AGG_PG_TH_8723B		0x0280
#define REG_FW_UPD_RDPTR_8723B		0x0284
#define REG_RXDMA_CONTROL_8723B		0x0286
#define REG_RXPKT_NUM_8723B			0x0287
#define REG_RXDMA_STATUS_8723B			0x0288
#define REG_RXDMA_PRO_8723B			0x0290
#define REG_EARLY_MODE_CONTROL_8723B	0x02BC
#define REG_RSVD5_8723B					0x02F0
#define REG_RSVD6_8723B					0x02F4


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

#define REG_VOQ_INFORMATION_8723B		0x0400
#define REG_VIQ_INFORMATION_8723B		0x0404
#define REG_BEQ_INFORMATION_8723B		0x0408
#define REG_BKQ_INFORMATION_8723B		0x040C
#define REG_MGQ_INFORMATION_8723B		0x0410
#define REG_HGQ_INFORMATION_8723B		0x0414
#define REG_BCNQ_INFORMATION_8723B	0x0418
#define REG_TXPKT_EMPTY_8723B			0x041A

#define REG_FWHW_TXQ_CTRL_8723B		0x0420
#define REG_HWSEQ_CTRL_8723B			0x0423
#define REG_TXPKTBUF_BCNQ_BDNY_8723B	0x0424
#define REG_TXPKTBUF_MGQ_BDNY_8723B	0x0425
#define REG_LIFECTRL_CTRL_8723B			0x0426
#define REG_MULTI_BCNQ_OFFSET_8723B	0x0427
#define REG_SPEC_SIFS_8723B				0x0428
#define REG_RL_8723B						0x042A
#define REG_TXBF_CTRL_8723B				0x042C
#define REG_DARFRC_8723B				0x0430
#define REG_RARFRC_8723B				0x0438
#define REG_RRSR_8723B					0x0440
#define REG_ARFR0_8723B					0x0444
#define REG_ARFR1_8723B					0x044C
#define REG_CCK_CHECK_8723B				0x0454
#define REG_AMPDU_MAX_TIME_8723B		0x0456
#define REG_TXPKTBUF_BCNQ_BDNY1_8723B	0x0457

#define REG_AMPDU_MAX_LENGTH_8723B	0x0458
#define REG_TXPKTBUF_WMAC_LBK_BF_HD_8723B	0x045D
#define REG_NDPA_OPT_CTRL_8723B		0x045F
#define REG_FAST_EDCA_CTRL_8723B		0x0460
#define REG_RD_RESP_PKT_TH_8723B		0x0463
#define REG_DATA_SC_8723B				0x0483
#define REG_TXRPT_START_OFFSET		0x04AC
#define REG_POWER_STAGE1_8723B		0x04B4
#define REG_POWER_STAGE2_8723B		0x04B8
#define REG_AMPDU_BURST_MODE_8723B	0x04BC
#define REG_PKT_VO_VI_LIFE_TIME_8723B	0x04C0
#define REG_PKT_BE_BK_LIFE_TIME_8723B	0x04C2
#define REG_STBC_SETTING_8723B			0x04C4
#define REG_HT_SINGLE_AMPDU_8723B		0x04C7
#define REG_PROT_MODE_CTRL_8723B		0x04C8
#define REG_MAX_AGGR_NUM_8723B		0x04CA
#define REG_RTS_MAX_AGGR_NUM_8723B	0x04CB
#define REG_BAR_MODE_CTRL_8723B		0x04CC
#define REG_RA_TRY_RATE_AGG_LMT_8723B	0x04CF
#define REG_MACID_PKT_DROP0_8723B		0x04D0
#define REG_MACID_PKT_SLEEP_8723B		0x04D4

#define REG_EDCA_VO_PARAM_8723B		0x0500
#define REG_EDCA_VI_PARAM_8723B		0x0504
#define REG_EDCA_BE_PARAM_8723B		0x0508
#define REG_EDCA_BK_PARAM_8723B		0x050C
#define REG_BCNTCFG_8723B				0x0510
#define REG_PIFS_8723B					0x0512
#define REG_RDG_PIFS_8723B				0x0513
#define REG_SIFS_CTX_8723B				0x0514
#define REG_SIFS_TRX_8723B				0x0516
#define REG_AGGR_BREAK_TIME_8723B		0x051A
#define REG_SLOT_8723B					0x051B
#define REG_TX_PTCL_CTRL_8723B			0x0520
#define REG_TXPAUSE_8723B				0x0522
#define REG_DIS_TXREQ_CLR_8723B		0x0523
#define REG_RD_CTRL_8723B				0x0524
#define REG_TBTT_PROHIBIT_8723B			0x0540
#define REG_RD_NAV_NXT_8723B			0x0544
#define REG_NAV_PROT_LEN_8723B			0x0546
#define REG_BCN_CTRL_8723B				0x0550
#define REG_BCN_CTRL_1_8723B			0x0551
#define REG_MBID_NUM_8723B				0x0552
#define REG_DUAL_TSF_RST_8723B			0x0553
#define REG_BCN_INTERVAL_8723B			0x0554
#define REG_DRVERLYINT_8723B			0x0558
#define REG_BCNDMATIM_8723B			0x0559
#define REG_ATIMWND_8723B				0x055A
#define REG_USTIME_TSF_8723B			0x055C
#define REG_BCN_MAX_ERR_8723B			0x055D
#define REG_RXTSF_OFFSET_CCK_8723B		0x055E
#define REG_RXTSF_OFFSET_OFDM_8723B	0x055F	
#define REG_TSFTR_8723B					0x0560
#define REG_CTWND_8723B					0x0572
#define REG_SECONDARY_CCA_CTRL_8723B	0x0577
#define REG_PSTIMER_8723B				0x0580
#define REG_TIMER0_8723B				0x0584
#define REG_TIMER1_8723B				0x0588
#define REG_ACMHWCTRL_8723B			0x05C0
#define REG_SCH_TXCMD_8723B			0x05F8

#define REG_MAC_CR_8723B				0x0600
#define REG_TCR_8723B					0x0604
#define REG_RCR_8723B					0x0608
#define REG_RX_PKT_LIMIT_8723B			0x060C
#define REG_RX_DLK_TIME_8723B			0x060D
#define REG_RX_DRVINFO_SZ_8723B		0x060F

#define REG_MACID_8723B					0x0610
#define REG_BSSID_8723B					0x0618
#define REG_MAR_8723B					0x0620
#define REG_MBIDCAMCFG_8723B			0x0628

#define REG_USTIME_EDCA_8723B			0x0638
#define REG_MAC_SPEC_SIFS_8723B		0x063A
#define REG_RESP_SIFP_CCK_8723B			0x063C
#define REG_RESP_SIFS_OFDM_8723B		0x063E
#define REG_ACKTO_8723B					0x0640
#define REG_CTS2TO_8723B				0x0641
#define REG_EIFS_8723B					0x0642

#define REG_NAV_UPPER_8723B			0x0652
#define REG_TRXPTCL_CTL_8723B			0x0668

#define REG_CAMCMD_8723B				0x0670
#define REG_CAMWRITE_8723B				0x0674
#define REG_CAMREAD_8723B				0x0678
#define REG_CAMDBG_8723B				0x067C
#define REG_SECCFG_8723B				0x0680

#define REG_WOW_CTRL_8723B				0x0690
#define REG_PS_RX_INFO_8723B			0x0692
#define REG_UAPSD_TID_8723B				0x0693
#define REG_WKFMCAM_CMD_8723B			0x0698
#define REG_WKFMCAM_NUM_8723B			0x0698
#define REG_WKFMCAM_RWD_8723B			0x069C
#define REG_RXFLTMAP0_8723B				0x06A0
#define REG_RXFLTMAP1_8723B				0x06A2
#define REG_RXFLTMAP2_8723B				0x06A4
#define REG_BCN_PSR_RPT_8723B			0x06A8
#define REG_BT_COEX_TABLE_8723B		0x06C0
#define REG_BFMER0_INFO_8723B			0x06E4
#define REG_BFMER1_INFO_8723B			0x06EC
#define REG_CSI_RPT_PARAM_BW20_8723B	0x06F4
#define REG_CSI_RPT_PARAM_BW40_8723B	0x06F8
#define REG_CSI_RPT_PARAM_BW80_8723B	0x06FC

#define REG_MACID1_8723B				0x0700
#define REG_BSSID1_8723B				0x0708
#define REG_BFMEE_SEL_8723B				0x0714
#define REG_SND_PTCL_CTRL_8723B		0x0718



#define	EFUSE_CTRL_8723B					REG_EFUSE_CTRL_8723B	
#define	EFUSE_TEST_8723B					REG_EFUSE_TEST_8723B	
#define	MSR_8723B							(REG_CR_8723B + 2)	
#define	ISR_8723B							REG_HISR0_8723B
#define	TSFR_8723B							REG_TSFTR_8723B		
					
#define 	PBP_8723B							REG_PBP_8723B

#define	IDR0_8723B							REG_MACID_8723B		
#define	IDR4_8723B							(REG_MACID_8723B + 4)


#define	RWCAM_8723B						REG_CAMCMD_8723B	
#define	WCAMI_8723B						REG_CAMWRITE_8723B	
#define	RCAMO_8723B						REG_CAMREAD_8723B	
#define	CAMDBG_8723B						REG_CAMDBG_8723B
#define	SECR_8723B							REG_SECCFG_8723B	


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
/*
#define		StopBecon			BIT6
#define		StopHigh				BIT5
#define		StopMgt				BIT4
#define		StopVO				BIT3
#define		StopVI				BIT2
#define		StopBE				BIT1
#define		StopBK				BIT0
*/


/*#define REG_USB_INFO					0xFE17
#define REG_USB_SPECIAL_OPTION			0xFE55
#define REG_USB_DMA_AGG_TO			0xFE5B
#define REG_USB_AGG_TO				0xFE5C
#define REG_USB_AGG_TH				0xFE5D

#define REG_USB_HRPWM				0xFE58
#define REG_USB_HCPWM				0xFE57

#define USB_IS_HIGH_SPEED					0
#define USB_IS_FULL_SPEED					1
#define USB_SPEED_MASK					BIT(5)

#define USB_NORMAL_SIE_EP_MASK			0xF
#define USB_NORMAL_SIE_EP_SHIFT			4

#define USB_AGG_EN						BIT(3)

*/
/*
#define ISO_MD2PP						BIT(0)
#define ISO_UA2USB					BIT(1)
#define ISO_UD2CORE					BIT(2)
#define ISO_PA2PCIE					BIT(3)
#define ISO_PD2CORE					BIT(4)
#define ISO_IP2MAC					BIT(5)
#define ISO_DIOP						BIT(6)
#define ISO_DIOE						BIT(7)
#define ISO_EB2CORE					BIT(8)
#define ISO_DIOR						BIT(9)
#define PWC_EV12V						BIT(15)


#define FEN_BBRSTB					BIT(0)
#define FEN_BB_GLB_RSTn				BIT(1)
#define FEN_USBA						BIT(2)
#define FEN_UPLL						BIT(3)
#define FEN_USBD						BIT(4)
#define FEN_DIO_PCIE					BIT(5)
#define FEN_PCIEA						BIT(6)
#define FEN_PPLL						BIT(7)
#define FEN_PCIED						BIT(8)
#define FEN_DIOE						BIT(9)
#define FEN_CPUEN						BIT(10)
#define FEN_DCORE						BIT(11)
#define FEN_ELDR						BIT(12)
#define FEN_DIO_RF					BIT(13)
#define FEN_HWPDN					BIT(14)
#define FEN_MREGEN					BIT(15)

#define PFM_LDALL						BIT(0)
#define PFM_ALDN						BIT(1)
#define PFM_LDKP						BIT(2)
#define PFM_WOWL					BIT(3)
#define EnPDN							BIT(4)
#define PDN_PL						BIT(5)
#define APFM_ONMAC					BIT(8)
#define APFM_OFF						BIT(9)
#define APFM_RSM						BIT(10)
#define AFSM_HSUS						BIT(11)
#define AFSM_PCIE						BIT(12)
#define APDM_MAC					BIT(13)
#define APDM_HOST					BIT(14)
#define APDM_HPDN					BIT(15)
#define RDY_MACON					BIT(16)
#define SUS_HOST						BIT(17)
#define ROP_ALD						BIT(20)
#define ROP_PWR						BIT(21)
#define ROP_SPS						BIT(22)
#define SOP_MRST						BIT(25)
#define SOP_FUSE						BIT(26)
#define SOP_ABG						BIT(27)
#define SOP_AMB						BIT(28)
#define SOP_RCK						BIT(29)
#define SOP_A8M						BIT(30)
#define XOP_BTCK						BIT(31)

#define ANAD16V_EN					BIT(0)
#define ANA8M						BIT(1)
#define MACSLP						BIT(4)
#define LOADER_CLK_EN					BIT(5)



#define		BOOT_FROM_EEPROM		BIT(4)
#define		EEPROM_EN				BIT(5)


#define RF_EN						BIT(0)
#define RF_RSTB					BIT(1)
#define RF_SDMRSTB				BIT(2)

#define LDV12_EN					BIT(0)
#define LDV12_SDBY				BIT(1)
#define LPLDO_HSM					BIT(2)
#define LPLDO_LSM_DIS				BIT(3)
#define _LDV12_VADJ(x)				(((x) & 0xF) << 4)


#define EF_TRPT						BIT(7)
#define EF_CELL_SEL						(BIT(8)|BIT(9))
#define LDOE25_EN						BIT(31)
#define EFUSE_SEL(x)					(((x) & 0x3) << 8)
#define EFUSE_SEL_MASK				0x300
#define EFUSE_WIFI_SEL_0				0x0
#define EFUSE_BT_SEL_0					0x1
#define EFUSE_BT_SEL_1					0x2
#define EFUSE_BT_SEL_2					0x3


#define MCUFWDL_EN					BIT(0)
#define MCUFWDL_RDY					BIT(1)
#define FWDL_ChkSum_rpt				BIT(2)
#define MACINI_RDY					BIT(3)
#define BBINI_RDY						BIT(4)
#define RFINI_RDY						BIT(5)
#define WINTINI_RDY					BIT(6)
#define RAM_DL_SEL					BIT(7)
#define ROM_DLEN						BIT(19)
#define CPRST							BIT(23)



#define XCLK_VLD						BIT(0)
#define ACLK_VLD						BIT(1)
#define UCLK_VLD						BIT(2)
#define PCLK_VLD						BIT(3)
#define PCIRSTB						BIT(4)
#define V15_VLD						BIT(5)
#define TRP_B15V_EN					BIT(7)
#define SIC_IDLE						BIT(8)
#define BD_MAC2						BIT(9)
#define BD_MAC1						BIT(10)
#define IC_MACPHY_MODE				BIT(11)
#define CHIP_VER						(BIT(12)|BIT(13)|BIT(14)|BIT(15))
#define BT_FUNC						BIT(16)
#define VENDOR_ID						BIT(19)
#define PAD_HWPD_IDN					BIT(22)
#define TRP_VAUX_EN					BIT(23)
#define TRP_BT_EN						BIT(24)
#define BD_PKG_SEL					BIT(25)
#define BD_HCI_SEL						BIT(26)
#define TYPE_ID						BIT(27)

#define CHIP_VER_RTL_MASK				0xF000
#define CHIP_VER_RTL_SHIFT				12

*/
/*



#define HCI_TXDMA_EN					BIT(0)
#define HCI_RXDMA_EN					BIT(1)
#define TXDMA_EN						BIT(2)
#define RXDMA_EN						BIT(3)
#define PROTOCOL_EN					BIT(4)
#define SCHEDULE_EN					BIT(5)
#define MACTXEN						BIT(6)
#define MACRXEN						BIT(7)
#define ENSWBCN						BIT(8)
#define ENSEC							BIT(9)
#define CALTMR_EN					BIT(10)

#define _NETTYPE(x)					(((x) & 0x3) << 16)
#define MASK_NETTYPE					0x30000
#define NT_NO_LINK					0x0
#define NT_LINK_AD_HOC				0x1
#define NT_LINK_AP						0x2
#define NT_AS_AP						0x3


#define GET_RX_PAGE_SIZE(value)			((value) & 0xF)
#define GET_TX_PAGE_SIZE(value)			(((value) & 0xF0) >> 4)
#define _PSRX_MASK					0xF
#define _PSTX_MASK					0xF0
#define _PSRX(x)						(x)
#define _PSTX(x)						((x) << 4)

#define PBP_64						0x0
#define PBP_128						0x1
#define PBP_256						0x2
#define PBP_512						0x3
#define PBP_1024						0x4


#define RXDMA_ARBBW_EN				BIT(0)
#define RXSHFT_EN						BIT(1)
#define RXDMA_AGG_EN					BIT(2)
#define QS_VO_QUEUE					BIT(8)
#define QS_VI_QUEUE					BIT(9)
#define QS_BE_QUEUE					BIT(10)
#define QS_BK_QUEUE					BIT(11)
#define QS_MANAGER_QUEUE			BIT(12)
#define QS_HIGH_QUEUE					BIT(13)

#define HQSEL_VOQ						BIT(0)
#define HQSEL_VIQ						BIT(1)
#define HQSEL_BEQ						BIT(2)
#define HQSEL_BKQ						BIT(3)
#define HQSEL_MGTQ					BIT(4)
#define HQSEL_HIQ						BIT(5)

#define _TXDMA_HIQ_MAP(x) 	 		(((x)&0x3) << 14)
#define _TXDMA_MGQ_MAP(x) 	 		(((x)&0x3) << 12)
#define _TXDMA_BKQ_MAP(x) 	 		(((x)&0x3) << 10)		
#define _TXDMA_BEQ_MAP(x) 	 		(((x)&0x3) << 8 )
#define _TXDMA_VIQ_MAP(x) 	 		(((x)&0x3) << 6 )
#define _TXDMA_VOQ_MAP(x) 	 		(((x)&0x3) << 4 )

#define QUEUE_LOW					1
#define QUEUE_NORMAL				2
#define QUEUE_HIGH					3


#define	C2H_EVT_HOST_CLOSE			0x00
#define	C2H_EVT_FW_CLOSE			0xFF	



#define _LLT_NO_ACTIVE					0x0
#define _LLT_WRITE_ACCESS				0x1
#define _LLT_READ_ACCESS				0x2

#define _LLT_INIT_DATA(x)				((x) & 0xFF)
#define _LLT_INIT_ADDR(x)				(((x) & 0xFF) << 8)
#define _LLT_OP(x)						(((x) & 0x3) << 30)
#define _LLT_OP_VALUE(x)				(((x) >> 30) & 0x3)

*/
/*
#define BLK_DESC_NUM_SHIFT			4
#define BLK_DESC_NUM_MASK			0xF


#define DROP_DATA_EN				BIT(9)
*/
/*

#define	RXPKT_RELEASE_POLL			BIT(0)
#define	RXDMA_IDLE					BIT(1)	
#define	RW_RELEASE_EN				BIT(2)
*/
/*
#define EN_AMPDU_RTY_NEW			BIT(7)


#define	HAL92C_EN_PKT_LIFE_TIME_BK		BIT3
#define	HAL92C_EN_PKT_LIFE_TIME_BE		BIT2
#define	HAL92C_EN_PKT_LIFE_TIME_VI		BIT1
#define	HAL92C_EN_PKT_LIFE_TIME_VO		BIT0

#define	HAL92C_MSDU_LIFE_TIME_UNIT		128	


#define _SPEC_SIFS_CCK(x)				((x) & 0xFF)
#define _SPEC_SIFS_OFDM(x)				(((x) & 0xFF) << 8)

#define	RETRY_LIMIT_SHORT_SHIFT		8
#define	RETRY_LIMIT_LONG_SHIFT		0

#define _LRL(x)							((x) & 0x3F)
#define _SRL(x)							(((x) & 0x3F) << 8)
*/

/*
#define AC_PARAM_TXOP_LIMIT_OFFSET		16
#define AC_PARAM_ECW_MAX_OFFSET		12
#define AC_PARAM_ECW_MIN_OFFSET		8
#define AC_PARAM_AIFS_OFFSET			0


#define EN_TXBCN_RPT						BIT(2)
#define EN_BCN_FUNCTION					BIT(3)

#define STOP_BCNQ						BIT(6)
*/


#define	AcmHw_HwEn_8723B				BIT(0)
#define	AcmHw_VoqEn_8723B				BIT(1)
#define	AcmHw_ViqEn_8723B				BIT(2)
#define	AcmHw_BeqEn_8723B				BIT(3)
#define	AcmHw_VoqStatus_8723B			BIT(5)
#define	AcmHw_ViqStatus_8723B			BIT(6)
#define	AcmHw_BeqStatus_8723B			BIT(7)



/*

#define DIS_GCLK							BIT(1)
#define PAD_SEL							BIT(2)
#define PWR_ST							BIT(6)
#define PWRBIT_OW_EN					BIT(7)
#define ACRC								BIT(8)
#define CFENDFORM						BIT(9)
#define ICV								BIT(10)
*/

/*
#define	RCR_APPFCS					BIT31	
#define	RCR_APP_MIC					BIT30	
#define	RCR_APP_ICV					BIT29      
#define	RCR_APP_PHYST_RXFF			BIT28      
#define	RCR_APP_BA_SSN				BIT27	
#define	RCR_RSVD_BIT26				BIT26	
*/
#define	RCR_TCPOFLD_EN				BIT25	
/*#define	RCR_ENMBID					BIT24	
#define	RCR_LSIGEN					BIT23	
#define	RCR_MFBEN					BIT22	
*/
/*#define	RCR_RSVD_BIT19				BIT19	
#define	RCR_TIM_PARSER_EN			BIT18	
#define	RCR_BM_DATA_EN				BIT17	
#define	RCR_UC_DATA_EN				BIT16	
*/
/*#define	RCR_HTC_LOC_CTRL				BIT14      
#define	RCR_AMF						BIT13	
#define	RCR_ACF						BIT12	
#define	RCR_ADF						BIT11	
*/
/*#define	RCR_AICV						BIT9	
#define	RCR_ACRC32					BIT8	
#define	RCR_CBSSID_BCN				BIT7	
#define	RCR_CBSSID_DATA				BIT6	
#define	RCR_CBSSID					RCR_CBSSID_DATA	
#define	RCR_APWRMGT				BIT5	
#define	RCR_ADD3					BIT4	
#define	RCR_AB						BIT3	
#define	RCR_AM						BIT2	
#define	RCR_APM					BIT1	
#define	RCR_AAP						BIT0	

#define AAP								BIT(0)
#define APM								BIT(1)
#define AM								BIT(2)
#define AB								BIT(3)
#define ADD3								BIT(4)
#define APWRMGT						BIT(5)
#define CBSSID							BIT(6)
#define CBSSID_DATA						BIT(6)
#define CBSSID_BCN						BIT(7)
#define ACRC32							BIT(8)
#define AICV								BIT(9)
#define ADF								BIT(11)
#define ACF								BIT(12)
#define AMF								BIT(13)
#define HTC_LOC_CTRL					BIT(14)
#define UC_DATA_EN						BIT(16)
#define BM_DATA_EN						BIT(17)
#define MFBEN							BIT(22)
#define LSIGEN							BIT(23)
#define EnMBID							BIT(24)
#define APP_BASSN						BIT(27)
#define APP_PHYSTS						BIT(28)
#define APP_ICV							BIT(29)
#define APP_MIC							BIT(30)
*/

/*
#define	SCR_TxUseDK					BIT(0)		
#define	SCR_RxUseDK					BIT(1)		
#define	SCR_TxEncEnable				BIT(2)		
#define	SCR_RxDecEnable				BIT(3)		
#define	SCR_SKByA2						BIT(4)		
#define	SCR_NoSKMC						BIT(5)		
#define 	SCR_TXBCUSEDK					BIT(6)		
#define 	SCR_RXBCUSEDK					BIT(7)		

#define	CAM_NONE						0x0
#define	CAM_WEP40						0x01
#define	CAM_TKIP						0x02
#define	CAM_AES						0x04
#define	CAM_WEP104					0x05
#define	CAM_SMS4						0x6
        		
#define	TOTAL_CAM_ENTRY				32
#define	HALF_CAM_ENTRY				16	
       		
#define	CAM_CONFIG_USEDK				TRUE
#define	CAM_CONFIG_NO_USEDK			FALSE
       		

#define	SCR_UseDK						0x01
#define	SCR_TxSecEnable				0x02
#define	SCR_RxSecEnable				0x04
*/


/*
#define 	SDIO_LOCAL_MSK					0x0FFF
#define 	WLAN_IOREG_MSK 	             			0x7FFF
#define 	WLAN_FIFO_MSK			      		0x1FFF

#define  	SDIO_WITHOUT_REF_DEVICE_ID   		0
#define  	SDIO_LOCAL_DEVICE_ID           			0
#define   WLAN_TX_HIQ_DEVICE_ID 			4
#define   WLAN_TX_MIQ_DEVICE_ID 			5
#define   WLAN_TX_LOQ_DEVICE_ID 			6
#define   WLAN_RX0FF_DEVICE_ID 				7
#define   WLAN_IOREG_DEVICE_ID 				8

#define 	HI_QUEUE_IDX           				0
#define 	MID_QUEUE_IDX         				1
#define 	LOW_QUEUE_IDX 	      				2
#define 	PUBLIC_QUEUE_IDX    				3

#define	SDIO_REG_TX_CTRL					0x0000
#define	SDIO_REG_HIMR						0x0014
#define	SDIO_REG_HISR						0x0018
#define	SDIO_REG_HCPWM					0x0019
#define	SDIO_REG_RX0_REQ_LEN				0x001C
#define	SDIO_REG_FREE_TXPG				0x0020
#define	SDIO_REG_HCPWM1					0x0024
#define	SDIO_REG_HCPWM2					0x0026
#define	SDIO_REG_HTSFR_INFO				0x0030
#define   SDIO_REG_HRPWM1					0x0080
#define   SDIO_REG_HRPWM2					0x0082
#define	SDIO_REG_HPS_CLKR					0x0084
#define   SDIO_REG_HSUS_CTRL				0x0086
#define	SDIO_REG_HIMR_ON					0x0090
#define	SDIO_REG_HISR_ON					0x0091

#define	SDIO_HIMR_DISABLED				0

#define	SDIO_HIMR_RX_REQUEST_MSK		BIT0
#define	SDIO_HIMR_AVAL_MSK				BIT1
#define	SDIO_HIMR_TXERR_MSK				BIT2
#define	SDIO_HIMR_RXERR_MSK				BIT3
#define	SDIO_HIMR_TXFOVW_MSK			BIT4
#define	SDIO_HIMR_RXFOVW_MSK			BIT5
#define	SDIO_HIMR_TXBCNOK_MSK			BIT6
#define	SDIO_HIMR_TXBCNERR_MSK			BIT7
#define	SDIO_HIMR_BCNERLY_INT_MSK		BIT16
#define	SDIO_HIMR_C2HCMD_MSK			BIT17
#define	SDIO_HIMR_CPWM1_MSK			BIT18
#define	SDIO_HIMR_CPWM2_MSK			BIT19
#define	SDIO_HIMR_HSISR_IND_MSK			BIT20
#define	SDIO_HIMR_GTINT3_IND_MSK		BIT21
#define	SDIO_HIMR_GTINT4_IND_MSK		BIT22
#define	SDIO_HIMR_PSTIMEOUT_MSK		BIT23
#define	SDIO_HIMR_OCPINT_MSK			BIT24
#define	SDIO_HIMR_ATIMEND_MSK			BIT25
#define	SDIO_HIMR_ATIMEND_E_MSK		BIT26
#define	SDIO_HIMR_CTWEND_MSK			BIT27

#define	SDIO_HIMR_MCU_ERR_MSK			BIT28
#define	SDIO_HIMR_TSF_BIT32_TOGGLE_MSK	BIT29

#define	SDIO_HISR_RX_REQUEST				BIT0
#define	SDIO_HISR_AVAL					BIT1
#define	SDIO_HISR_TXERR					BIT2
#define	SDIO_HISR_RXERR					BIT3
#define	SDIO_HISR_TXFOVW				BIT4
#define	SDIO_HISR_RXFOVW				BIT5
#define	SDIO_HISR_TXBCNOK				BIT6
#define	SDIO_HISR_TXBCNERR				BIT7
#define	SDIO_HISR_BCNERLY_INT			BIT16
#define	SDIO_HISR_C2HCMD				BIT17
#define	SDIO_HISR_CPWM1					BIT18
#define	SDIO_HISR_CPWM2					BIT19
#define	SDIO_HISR_HSISR_IND				BIT20
#define	SDIO_HISR_GTINT3_IND				BIT21
#define	SDIO_HISR_GTINT4_IND				BIT22
#define	SDIO_HISR_PSTIMEOUT				BIT23
#define	SDIO_HISR_OCPINT					BIT24
#define	SDIO_HISR_ATIMEND				BIT25
#define	SDIO_HISR_ATIMEND_E				BIT26
#define	SDIO_HISR_CTWEND				BIT27

#define	SDIO_HISR_MCU_ERR					BIT28
#define	SDIO_HISR_TSF_BIT32_TOGGLE		BIT29


#define	HCI_RESUME_PWR_RDY				BIT1
#define	HCI_SUS_CTRL						BIT0


#if DEV_BUS_TYPE == RT_SDIO_INTERFACE
	#define 	MAX_TX_AGG_PACKET_NUMBER 	0x8
#else
	#define 	MAX_TX_AGG_PACKET_NUMBER 	0xFF
#endif

*/
#endif
