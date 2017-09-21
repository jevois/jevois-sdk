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
#ifndef __RTL8188E_HAL_H__
#define __RTL8188E_HAL_H__

#if 1
#include "hal_data.h"
#else
#include "../hal/OUTSRC/odm_precomp.h"
#endif

#include "rtl8188e_spec.h"
#include "Hal8188EPhyReg.h"
#include "Hal8188EPhyCfg.h"
#include "rtl8188e_rf.h"
#include "rtl8188e_dm.h"
#include "rtl8188e_recv.h"
#include "rtl8188e_xmit.h"
#include "rtl8188e_cmd.h"
#include "rtl8188e_led.h"
#include "Hal8188EPwrSeq.h"
#ifdef DBG_CONFIG_ERROR_DETECT
#include "rtl8188e_sreset.h"
#endif

#if 0
	#define Rtl8188E_FwImageArray				Rtl8188EFwImgArray
	#define Rtl8188E_FWImgArrayLength			Rtl8188EFWImgArrayLength
#ifdef CONFIG_WOWLAN
	#define Rtl8188E_FwWoWImageArray			Array_MP_8188E_FW_WoWLAN
	#define Rtl8188E_FwWoWImgArrayLength		ArrayLength_MP_8188E_FW_WoWLAN
#endif
#endif


	#define RTL8188E_FW_IMG					"rtl8188e/FW_NIC.bin"
	#define RTL8188E_FW_WW_IMG				"rtl8188e/FW_WoWLAN.bin"
	#define RTL8188E_PHY_REG					"rtl8188e/PHY_REG.txt" 
	#define RTL8188E_PHY_RADIO_A				"rtl8188e/RadioA.txt"
	#define RTL8188E_PHY_RADIO_B				"rtl8188e/RadioB.txt"
	#define RTL8188E_TXPWR_TRACK				"rtl8188e/TxPowerTrack.txt"			
	#define RTL8188E_AGC_TAB					"rtl8188e/AGC_TAB.txt"
	#define RTL8188E_PHY_MACREG 				"rtl8188e/MAC_REG.txt"
	#define RTL8188E_PHY_REG_PG				"rtl8188e/PHY_REG_PG.txt"
	#define RTL8188E_PHY_REG_MP 				"rtl8188e/PHY_REG_MP.txt" 
	#define RTL8188E_TXPWR_LMT				"rtl8188e/TXPWR_LMT.txt" 

	#define Rtl8188E_NIC_PWR_ON_FLOW				rtl8188E_power_on_flow
	#define Rtl8188E_NIC_RF_OFF_FLOW				rtl8188E_radio_off_flow
	#define Rtl8188E_NIC_DISABLE_FLOW				rtl8188E_card_disable_flow
	#define Rtl8188E_NIC_ENABLE_FLOW				rtl8188E_card_enable_flow
	#define Rtl8188E_NIC_SUSPEND_FLOW				rtl8188E_suspend_flow
	#define Rtl8188E_NIC_RESUME_FLOW				rtl8188E_resume_flow
	#define Rtl8188E_NIC_PDN_FLOW					rtl8188E_hwpdn_flow
	#define Rtl8188E_NIC_LPS_ENTER_FLOW			rtl8188E_enter_lps_flow
	#define Rtl8188E_NIC_LPS_LEAVE_FLOW			rtl8188E_leave_lps_flow


#if 1
#define FW_8188E_SIZE				0x4000
#define FW_8188E_START_ADDRESS	0x1000
#define FW_8188E_END_ADDRESS		0x1FFF




#define IS_FW_HEADER_EXIST_88E(_pFwHdr)	((le16_to_cpu(_pFwHdr->Signature)&0xFFF0) == 0x88E0)

typedef struct _RT_FIRMWARE_8188E {
	FIRMWARE_SOURCE	eFWSource;
#ifdef CONFIG_EMBEDDED_FWIMG
	u8*			szFwBuffer;
#else
	u8			szFwBuffer[FW_8188E_SIZE];
#endif
	u32			ulFwLength;
} RT_FIRMWARE_8188E, *PRT_FIRMWARE_8188E;


typedef struct _RT_8188E_FIRMWARE_HDR
{

	u16		Signature;
	u8		Category;
	u8		Function;
	u16		Version;	
	u8		Subversion;
	u16		Rsvd1;


	u8		Month;
	u8		Date;
	u8		Hour;
	u8		Minute;
	u16		RamCodeSize;
	u8		Foundry;
	u8		Rsvd2;

	u32		SvnIdx;
	u32		Rsvd3;

	u32		Rsvd4;
	u32		Rsvd5;
}RT_8188E_FIRMWARE_HDR, *PRT_8188E_FIRMWARE_HDR;
#endif


#define DRIVER_EARLY_INT_TIME_8188E			0x05
#define BCN_DMA_ATIME_INT_TIME_8188E		0x02


#define MAX_RX_DMA_BUFFER_SIZE_88E	      0x2400


#define MAX_TX_REPORT_BUFFER_SIZE			0x0400



#define BCNQ_PAGE_NUM_88E		0x08

#ifdef CONFIG_WOWLAN
#define WOWLAN_PAGE_NUM_88E	0x00
#else
#define WOWLAN_PAGE_NUM_88E	0x00
#endif

#define TX_TOTAL_PAGE_NUMBER_88E	(0xB0 - BCNQ_PAGE_NUM_88E - WOWLAN_PAGE_NUM_88E)
#define TX_PAGE_BOUNDARY_88E		(TX_TOTAL_PAGE_NUMBER_88E + 1)

#define WMM_NORMAL_TX_TOTAL_PAGE_NUMBER_88E	TX_TOTAL_PAGE_NUMBER_88E
#define WMM_NORMAL_TX_PAGE_BOUNDARY_88E		(WMM_NORMAL_TX_TOTAL_PAGE_NUMBER_88E + 1)

#define NORMAL_PAGE_NUM_HPQ_88E		0x00
#define NORMAL_PAGE_NUM_LPQ_88E		0x09
#define NORMAL_PAGE_NUM_NPQ_88E		0x00

#define WMM_NORMAL_PAGE_NUM_HPQ_88E		0x29
#define WMM_NORMAL_PAGE_NUM_LPQ_88E		0x1C
#define WMM_NORMAL_PAGE_NUM_NPQ_88E		0x1C


#define CHIP_BONDING_IDENTIFIER(_value)	(((_value)>>22)&0x3)
#define CHIP_BONDING_92C_1T2R	0x1
#define CHIP_BONDING_88C_USB_MCARD	0x2
#define CHIP_BONDING_88C_USB_HP	0x1



#define EFUSE_REAL_CONTENT_LEN		512
#define EFUSE_MAP_LEN				128
#define EFUSE_MAX_SECTION			16
#define EFUSE_IC_ID_OFFSET			506
#define AVAILABLE_EFUSE_ADDR(addr) 	(addr < EFUSE_REAL_CONTENT_LEN)
#define EFUSE_OOB_PROTECT_BYTES 		15

#define		HWSET_MAX_SIZE_88E		512

#define		EFUSE_REAL_CONTENT_LEN_88E	256
#define		EFUSE_MAP_LEN_88E		512
#define		EFUSE_MAX_SECTION_88E		64
#define		EFUSE_MAX_WORD_UNIT_88E		4
#define		EFUSE_IC_ID_OFFSET_88E			506
#define 		AVAILABLE_EFUSE_ADDR_88E(addr) 	(addr < EFUSE_REAL_CONTENT_LEN_88E)
#define 		EFUSE_OOB_PROTECT_BYTES_88E	18 
#define		EFUSE_PROTECT_BYTES_BANK_88E	16


#define EFUSE_BT_REAL_CONTENT_LEN		1536
#define EFUSE_BT_MAP_LEN				1024
#define EFUSE_BT_MAX_SECTION			128	

#define EFUSE_PROTECT_BYTES_BANK		16

#define INCLUDE_MULTI_FUNC_BT(_Adapter)	(GET_HAL_DATA(_Adapter)->MultiFunc & RT_MULTI_FUNC_BT)
#define INCLUDE_MULTI_FUNC_GPS(_Adapter)	(GET_HAL_DATA(_Adapter)->MultiFunc & RT_MULTI_FUNC_GPS)



#ifdef CONFIG_PCI_HCI
void InterruptRecognized8188EE(PADAPTER Adapter, PRT_ISR_CONTENT pIsrContent);
void UpdateInterruptMask8188EE(PADAPTER Adapter, u32 AddMSR, u32 AddMSR1, u32 RemoveMSR, u32 RemoveMSR1);
#endif


s32 rtl8188e_FirmwareDownload(PADAPTER padapter, BOOLEAN  bUsedWoWLANFw);
void _8051Reset88E(PADAPTER padapter);
void rtl8188e_InitializeFirmwareVars(PADAPTER padapter);


s32 InitLLTTable(PADAPTER padapter, u8 txpktbuf_bndy);

u8 GetEEPROMSize8188E(PADAPTER padapter);
void Hal_InitPGData88E(PADAPTER padapter);
void Hal_EfuseParseIDCode88E(PADAPTER padapter, u8 *hwinfo);
void Hal_ReadTxPowerInfo88E(PADAPTER padapter,u8* hwinfo,BOOLEAN	AutoLoadFail);
	
void Hal_EfuseParseEEPROMVer88E(PADAPTER padapter, u8 *hwinfo, BOOLEAN AutoLoadFail);
void rtl8188e_EfuseParseChnlPlan(PADAPTER padapter, u8 *hwinfo, BOOLEAN AutoLoadFail);
void Hal_EfuseParseCustomerID88E(PADAPTER padapter, u8 *hwinfo, BOOLEAN AutoLoadFail);
void Hal_ReadAntennaDiversity88E	(PADAPTER pAdapter,u8*PROMContent,BOOLEAN AutoLoadFail);
void Hal_ReadThermalMeter_88E(PADAPTER	Adapter,u8* PROMContent,BOOLEAN 	AutoloadFail);
void Hal_EfuseParseXtal_8188E(PADAPTER pAdapter,u8* hwinfo,BOOLEAN AutoLoadFail);
void Hal_EfuseParseBoardType88E(PADAPTER pAdapter,u8* hwinfo,BOOLEAN AutoLoadFail);
void Hal_ReadPowerSavingMode88E(PADAPTER pAdapter,u8* hwinfo,BOOLEAN AutoLoadFail);

BOOLEAN HalDetectPwrDownMode88E(PADAPTER Adapter);
	
#ifdef CONFIG_WOWLAN
void Hal_DetectWoWMode(PADAPTER pAdapter);
#endif
void Hal_InitChannelPlan(PADAPTER padapter);

void rtl8188e_set_hal_ops(struct hal_ops *pHalFunc);

void SetBcnCtrlReg(PADAPTER padapter, u8 SetBits, u8 ClearBits);

void rtl8188e_start_thread(_adapter *padapter);
void rtl8188e_stop_thread(_adapter *padapter);

void rtw_IOL_cmd_tx_pkt_buf_dump(ADAPTER *Adapter,int data_len);
#ifdef CONFIG_IOL_EFUSE_PATCH
s32 rtl8188e_iol_efuse_patch(PADAPTER padapter);
#endif//CONFIG_IOL_EFUSE_PATCH
void _InitTransferPageSize(PADAPTER padapter);

void SetHwReg8188E(PADAPTER padapter, u8 variable, u8 *val);
void GetHwReg8188E(PADAPTER padapter, u8 variable, u8 *val);

#endif

