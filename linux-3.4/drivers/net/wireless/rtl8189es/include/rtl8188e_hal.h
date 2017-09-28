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


#include "rtl8188e_spec.h"
#include "Hal8188EPhyReg.h"
#include "Hal8188EPhyCfg.h"
#include "rtl8188e_rf.h"
#include "rtl8188e_dm.h"
#include "rtl8188e_recv.h"
#include "rtl8188e_xmit.h"
#include "rtl8188e_cmd.h"
#include "Hal8188EPwrSeq.h"
#ifdef DBG_CONFIG_ERROR_DETECT
#include "rtl8188e_sreset.h"
#endif
#include "rtw_efuse.h"

#include "../hal/OUTSRC/odm_precomp.h"

	#define Rtl8188E_FwImageArray				Rtl8188EFwImgArray
	#define Rtl8188E_FWImgArrayLength			Rtl8188EFWImgArrayLength
#ifdef CONFIG_WOWLAN
	#define Rtl8188E_FwWoWImageArray			Array_8188E_FW_WoWLAN
	#define Rtl8188E_FwWoWImgArrayLength		ArrayLength_8188E_FW_WoWLAN
#endif

	
#ifdef CONFIG_SDIO_HCI
	
	#define RTL8188E_FW_UMC_IMG				"rtl8188E\\rtl8188efw.bin"
	#define RTL8188E_PHY_REG					"rtl8188E\\PHY_REG_1T.txt"
	#define RTL8188E_PHY_RADIO_A				"rtl8188E\\radio_a_1T.txt"
	#define RTL8188E_PHY_RADIO_B				"rtl8188E\\radio_b_1T.txt"
	#define RTL8188E_AGC_TAB					"rtl8188E\\AGC_TAB_1T.txt"
	#define RTL8188E_PHY_MACREG 				"rtl8188E\\MAC_REG.txt"
	#define RTL8188E_PHY_REG_PG				"rtl8188E\\PHY_REG_PG.txt"
	#define RTL8188E_PHY_REG_MP				"rtl8188E\\PHY_REG_MP.txt"

#if 0
	#define Rtl8188E_PHY_REG_Array_PG			Rtl8188ESPHY_REG_Array_PG
	#define Rtl8188E_PHY_REG_Array_PGLength	Rtl8188ESPHY_REG_Array_PGLength	

#endif
	
	#define Rtl8188E_NIC_PWR_ON_FLOW				rtl8188E_power_on_flow
	#define Rtl8188E_NIC_RF_OFF_FLOW				rtl8188E_radio_off_flow
	#define Rtl8188E_NIC_DISABLE_FLOW				rtl8188E_card_disable_flow
	#define Rtl8188E_NIC_ENABLE_FLOW				rtl8188E_card_enable_flow
	#define Rtl8188E_NIC_SUSPEND_FLOW				rtl8188E_suspend_flow
	#define Rtl8188E_NIC_RESUME_FLOW				rtl8188E_resume_flow
	#define Rtl8188E_NIC_PDN_FLOW					rtl8188E_hwpdn_flow
	#define Rtl8188E_NIC_LPS_ENTER_FLOW			rtl8188E_enter_lps_flow
	#define Rtl8188E_NIC_LPS_LEAVE_FLOW			rtl8188E_leave_lps_flow

#elif defined(CONFIG_USB_HCI)
	#define RTL8188E_FW_UMC_IMG				"rtl8188E\\rtl8188efw.bin"
	#define RTL8188E_PHY_REG					"rtl8188E\\PHY_REG_1T.txt"
	#define RTL8188E_PHY_RADIO_A				"rtl8188E\\radio_a_1T.txt"
	#define RTL8188E_PHY_RADIO_B				"rtl8188E\\radio_b_1T.txt"
	#define RTL8188E_AGC_TAB					"rtl8188E\\AGC_TAB_1T.txt"
	#define RTL8188E_PHY_MACREG 				"rtl8188E\\MAC_REG.txt"
	#define RTL8188E_PHY_REG_PG				"rtl8188E\\PHY_REG_PG.txt"
	#define RTL8188E_PHY_REG_MP				"rtl8188E\\PHY_REG_MP.txt"

#if 0
	#define Rtl8188E_PHY_REG_Array_PG			Rtl8188EUPHY_REG_Array_PG
	#define Rtl8188E_PHY_REG_Array_PGLength	Rtl8188EUPHY_REG_Array_PGLength	
	
#endif	
	
	#define Rtl8188E_NIC_PWR_ON_FLOW				rtl8188E_power_on_flow
	#define Rtl8188E_NIC_RF_OFF_FLOW				rtl8188E_radio_off_flow
	#define Rtl8188E_NIC_DISABLE_FLOW				rtl8188E_card_disable_flow
	#define Rtl8188E_NIC_ENABLE_FLOW				rtl8188E_card_enable_flow
	#define Rtl8188E_NIC_SUSPEND_FLOW				rtl8188E_suspend_flow
	#define Rtl8188E_NIC_RESUME_FLOW				rtl8188E_resume_flow
	#define Rtl8188E_NIC_PDN_FLOW					rtl8188E_hwpdn_flow
	#define Rtl8188E_NIC_LPS_ENTER_FLOW			rtl8188E_enter_lps_flow
	#define Rtl8188E_NIC_LPS_LEAVE_FLOW			rtl8188E_leave_lps_flow

#elif defined(CONFIG_PCI_HCI)
	#define RTL8188E_FW_UMC_IMG				"rtl8188E\\rtl8188efw.bin"
	#define RTL8188E_PHY_REG					"rtl8188E\\PHY_REG_1T.txt"
	#define RTL8188E_PHY_RADIO_A				"rtl8188E\\radio_a_1T.txt"
	#define RTL8188E_PHY_RADIO_B				"rtl8188E\\radio_b_1T.txt"
	#define RTL8188E_AGC_TAB					"rtl8188E\\AGC_TAB_1T.txt"
	#define RTL8188E_PHY_MACREG 				"rtl8188E\\MAC_REG.txt"
	#define RTL8188E_PHY_REG_PG				"rtl8188E\\PHY_REG_PG.txt"
	#define RTL8188E_PHY_REG_MP				"rtl8188E\\PHY_REG_MP.txt"

	#define Rtl8188E_PHY_REG_Array_PG			Rtl8188EEPHY_REG_Array_PG
	#define Rtl8188E_PHY_REG_Array_PGLength	Rtl8188EEPHY_REG_Array_PGLength	

		
	#ifndef CONFIG_PHY_SETTING_WITH_ODM		
	#if MP_DRIVER == 1
	#define Rtl8188ES_PHY_REG_Array_MP 			Rtl8188ESPHY_REG_Array_MP
	#endif
	#endif
	
	#define Rtl8188E_NIC_PWR_ON_FLOW				rtl8188E_power_on_flow
	#define Rtl8188E_NIC_RF_OFF_FLOW				rtl8188E_radio_off_flow
	#define Rtl8188E_NIC_DISABLE_FLOW				rtl8188E_card_disable_flow
	#define Rtl8188E_NIC_ENABLE_FLOW				rtl8188E_card_enable_flow
	#define Rtl8188E_NIC_SUSPEND_FLOW				rtl8188E_suspend_flow
	#define Rtl8188E_NIC_RESUME_FLOW				rtl8188E_resume_flow
	#define Rtl8188E_NIC_PDN_FLOW					rtl8188E_hwpdn_flow
	#define Rtl8188E_NIC_LPS_ENTER_FLOW			rtl8188E_enter_lps_flow
	#define Rtl8188E_NIC_LPS_LEAVE_FLOW			rtl8188E_leave_lps_flow
#endif


#define DRVINFO_SZ	4
#define PageNum_128(_Len)		(u32)(((_Len)>>7) + ((_Len)&0x7F ? 1:0))


#if 1
#define FW_8188E_SIZE				0x4000
#define FW_8188E_START_ADDRESS	0x1000
#define FW_8188E_END_ADDRESS		0x1FFF

#define MAX_PAGE_SIZE			4096

#define IS_FW_HEADER_EXIST(_pFwHdr)	((le16_to_cpu(_pFwHdr->Signature)&0xFFF0) == 0x92C0 ||\
									(le16_to_cpu(_pFwHdr->Signature)&0xFFF0) == 0x88C0 ||\
									(le16_to_cpu(_pFwHdr->Signature)&0xFFF0) == 0x2300 ||\
									(le16_to_cpu(_pFwHdr->Signature)&0xFFF0) == 0x88E0)

typedef enum _FIRMWARE_SOURCE {
	FW_SOURCE_IMG_FILE = 0,
	FW_SOURCE_HEADER_FILE = 1,	
} FIRMWARE_SOURCE, *PFIRMWARE_SOURCE;

typedef struct _RT_FIRMWARE {
	FIRMWARE_SOURCE	eFWSource;
#ifdef CONFIG_EMBEDDED_FWIMG
	u8*			szFwBuffer;
#else
	u8			szFwBuffer[FW_8188E_SIZE];
#endif
	u32			ulFwLength;

#ifdef CONFIG_WOWLAN
	u8*			szWoWLANFwBuffer;
	u32			ulWoWLANFwLength;
#endif
} RT_FIRMWARE, *PRT_FIRMWARE, RT_FIRMWARE_8188E, *PRT_FIRMWARE_8188E;


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


#define DRIVER_EARLY_INT_TIME		0x05
#define BCN_DMA_ATIME_INT_TIME		0x02

#ifdef CONFIG_USB_RX_AGGREGATION

typedef enum _USB_RX_AGG_MODE{
	USB_RX_AGG_DISABLE,
	USB_RX_AGG_DMA,
	USB_RX_AGG_USB,
	USB_RX_AGG_MIX
}USB_RX_AGG_MODE;


#endif


#define MAX_RX_DMA_BUFFER_SIZE_88E	      0x2400

#define MAX_TX_REPORT_BUFFER_SIZE			0x0400


#define MAX_TX_QUEUE		9

#define TX_SELE_HQ			BIT(0)	
#define TX_SELE_LQ			BIT(1)	
#define TX_SELE_NQ			BIT(2)	


#define TX_TOTAL_PAGE_NUMBER_88E		0xA9//  169 (21632=> 21k)

#ifdef RTL8188ES_MAC_LOOPBACK
#define TX_PAGE_BOUNDARY_88E 0x48
#else
#define TX_PAGE_BOUNDARY_88E (TX_TOTAL_PAGE_NUMBER_88E + 1)
#endif


#define WMM_NORMAL_TX_TOTAL_PAGE_NUMBER	TX_TOTAL_PAGE_NUMBER_88E 
#define WMM_NORMAL_TX_PAGE_BOUNDARY_88E	(WMM_NORMAL_TX_TOTAL_PAGE_NUMBER + 1)



#define CHIP_BONDING_IDENTIFIER(_value)	(((_value)>>22)&0x3)
#define CHIP_BONDING_92C_1T2R	0x1
#define CHIP_BONDING_88C_USB_MCARD	0x2
#define CHIP_BONDING_88C_USB_HP	0x1
#include "HalVerDef.h"
#include "hal_com.h"

enum ChannelPlan
{
	CHPL_FCC	= 0,
	CHPL_IC		= 1,
	CHPL_ETSI	= 2,
	CHPL_SPAIN	= 3,
	CHPL_FRANCE	= 4,
	CHPL_MKK	= 5,
	CHPL_MKK1	= 6,
	CHPL_ISRAEL	= 7,
	CHPL_TELEC	= 8,
	CHPL_GLOBAL	= 9,
	CHPL_WORLD	= 10,
};

typedef struct _TxPowerInfo
{
	u8 CCKIndex[RF_PATH_MAX][CHANNEL_GROUP_MAX_88E];
	u8 HT40_1SIndex[RF_PATH_MAX][CHANNEL_GROUP_MAX_88E];
	u8 HT40_2SIndexDiff[RF_PATH_MAX][CHANNEL_GROUP_MAX_88E];
	u8 HT20IndexDiff[RF_PATH_MAX][CHANNEL_GROUP_MAX_88E];
	u8 OFDMIndexDiff[RF_PATH_MAX][CHANNEL_GROUP_MAX_88E];
	u8 HT40MaxOffset[RF_PATH_MAX][CHANNEL_GROUP_MAX_88E];
	u8 HT20MaxOffset[RF_PATH_MAX][CHANNEL_GROUP_MAX_88E];
	u8 TSSI_A[3];
	u8 TSSI_B[3];
	u8 TSSI_A_5G[3];	
	u8 TSSI_B_5G[3];
} TxPowerInfo, *PTxPowerInfo;

typedef struct _TxPowerInfo24G{
	u1Byte IndexCCK_Base[MAX_RF_PATH][MAX_CHNL_GROUP_24G];
	u1Byte IndexBW40_Base[MAX_RF_PATH][MAX_CHNL_GROUP_24G-1];
	s1Byte CCK_Diff[MAX_RF_PATH][MAX_TX_COUNT];	
	s1Byte OFDM_Diff[MAX_RF_PATH][MAX_TX_COUNT];
	s1Byte BW20_Diff[MAX_RF_PATH][MAX_TX_COUNT];
	s1Byte BW40_Diff[MAX_RF_PATH][MAX_TX_COUNT];
}TxPowerInfo24G, *PTxPowerInfo24G;

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

typedef enum _RT_MULTI_FUNC {
	RT_MULTI_FUNC_NONE = 0x00,
	RT_MULTI_FUNC_WIFI = 0x01,
	RT_MULTI_FUNC_BT = 0x02,
	RT_MULTI_FUNC_GPS = 0x04,
} RT_MULTI_FUNC, *PRT_MULTI_FUNC;

typedef enum _RT_POLARITY_CTL {
	RT_POLARITY_LOW_ACT = 0,
	RT_POLARITY_HIGH_ACT = 1,	
} RT_POLARITY_CTL, *PRT_POLARITY_CTL;

typedef enum _RT_REGULATOR_MODE {
	RT_SWITCHING_REGULATOR = 0,
	RT_LDO_REGULATOR = 1,
} RT_REGULATOR_MODE, *PRT_REGULATOR_MODE;


typedef struct hal_data_8188e
{
	HAL_VERSION			VersionID;
	RT_MULTI_FUNC		MultiFunc;
	RT_POLARITY_CTL		PolarityCtl;
	RT_REGULATOR_MODE	RegulatorMode;
	u16	CustomerID;

	u16	FirmwareVersion;
	u16	FirmwareVersionRev;
	u16	FirmwareSubVersion;
	u16	FirmwareSignature;
	u8	PGMaxGroup;
	u32	ReceiveConfig;
	WIRELESS_MODE		CurrentWirelessMode;
	HT_CHANNEL_WIDTH	CurrentChannelBW;
	u8	CurrentChannel;
	u8	nCur40MhzPrimeSC;// Control channel sub-carrier

	u16	BasicRateSet;

	u8	rf_chip;
	u8	rf_type;
	u8	NumTotalRFPath;

	u8	BoardType;

	u16	EEPROMVID;
	u16	EEPROMPID;
	u16	EEPROMSVID;
	u16	EEPROMSDID;
	u8	EEPROMCustomerID;
	u8	EEPROMSubCustomerID;
	u8	EEPROMVersion;
	u8	EEPROMRegulatory;

	u8	bTXPowerDataReadFromEEPORM;
	u8	EEPROMThermalMeter;
	u8	bAPKThermalMeterIgnore;

	BOOLEAN 			EepromOrEfuse;
	u8				EfuseMap[2][HWSET_MAX_SIZE_512];
	u8				EfuseUsedPercentage;
	EFUSE_HAL			EfuseHal;
	

	
	u8	Index24G_CCK_Base[MAX_RF_PATH][CHANNEL_MAX_NUMBER];
	u8	Index24G_BW40_Base[MAX_RF_PATH][CHANNEL_MAX_NUMBER];
	s8	CCK_24G_Diff[MAX_RF_PATH][MAX_TX_COUNT];	
	s8	OFDM_24G_Diff[MAX_RF_PATH][MAX_TX_COUNT];
	s8	BW20_24G_Diff[MAX_RF_PATH][MAX_TX_COUNT];
	s8	BW40_24G_Diff[MAX_RF_PATH][MAX_TX_COUNT];
	
	u8	TxPwrLevelCck[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	u8	TxPwrLevelHT40_1S[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	u8	TxPwrLevelHT40_2S[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	u8	TxPwrHt20Diff[RF_PATH_MAX][CHANNEL_MAX_NUMBER];// HT 20<->40 Pwr diff
	u8	TxPwrLegacyHtDiff[RF_PATH_MAX][CHANNEL_MAX_NUMBER];// For HT<->legacy pwr diff
	u8	PwrGroupHT20[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	u8	PwrGroupHT40[RF_PATH_MAX][CHANNEL_MAX_NUMBER];

	u8	LegacyHTTxPowerDiff;// Legacy to HT rate power diff
	u8	CurrentCckTxPwrIdx;
	u8	CurrentOfdm24GTxPwrIdx;
	u8	CurrentBW2024GTxPwrIdx;
	u8	CurrentBW4024GTxPwrIdx;
	

	u8	framesync;
	u32	framesyncC34;
	u8	framesyncMonitor;
	u8	DefaultInitialGain[4];
	u8	pwrGroupCnt;
	u32	MCSTxPowerLevelOriginalOffset[MAX_PG_GROUP][16];
	u32	CCKTxPowerLevelOriginalOffset;

	u8	CrystalCap;
	u32	AntennaTxPath;				
	u32	AntennaRxPath;				
	u8	BluetoothCoexist;
	u8	ExternalPA;

	u8	bLedOpenDrain;

	u8	b1x1RecvCombine;

	u32	AcParam_BE;

	BB_REGISTER_DEFINITION_T	PHYRegDef[4];

	u32	RfRegChnlVal[2];

	BOOLEAN	 bRDGEnable;

	u8	LastHMEBoxNum;

	u8	fw_ractrl;
	u8	RegTxPause;
	u32	RegBcnCtrlVal;
	u8	RegFwHwTxQCtrl;
	u8	RegReg542;
	u8	RegCR_1;

	struct dm_priv	dmpriv;
	DM_ODM_T 		odmpriv;
#ifdef DBG_CONFIG_ERROR_DETECT
	struct sreset_priv srestpriv;
#endif

#ifdef CONFIG_BT_COEXIST
	struct btcoexist_priv	bt_coexist;
#endif

	u8	CurAntenna;
	u8	AntDivCfg;
	u8	TRxAntDivType;


	u8	bDumpRxPkt;//for debug
	u8	bDumpTxPkt;//for debug
	u8	FwRsvdPageStartOffset;

	BOOLEAN		pwrdown;

	u32	interfaceIndex;

	u8	OutEpQueueSel;
	u8	OutEpNumber;

	BOOLEAN		UsbRxHighSpeedMode;

	BOOLEAN		SlimComboDbg;

	u16	EfuseUsedBytes;

#ifdef CONFIG_P2P
	struct P2P_PS_Offload_t	p2p_ps_offload;
#endif

	u8			bMacPwrCtrlOn;

#ifdef CONFIG_SDIO_HCI

	u32			sdio_himr;
	u32			sdio_hisr;

	u8			SdioTxFIFOFreePage[SDIO_TX_FREE_PG_QUEUE];
	_lock		SdioTxFIFOFreePageLock;
#ifndef CONFIG_SDIO_TX_TASKLET
	_thread_hdl_ 	SdioXmitThread;
	_sema		SdioXmitSema;
	_sema		SdioXmitTerminateSema;
#endif

	u8			SdioRxFIFOCnt;
	u16			SdioRxFIFOSize;
#endif

#ifdef CONFIG_USB_HCI
	u32	UsbBulkOutSize;

	u32	IntArray[3];//HISR0,HISR1,HSISR
	u32	IntrMask[3];
	u8	C2hArray[16];
#ifdef CONFIG_USB_TX_AGGREGATION
	u8	UsbTxAggMode;
	u8	UsbTxAggDescNum;
#endif
#ifdef CONFIG_USB_RX_AGGREGATION
	u16	HwRxPageSize;			
	u32	MaxUsbRxAggBlock;

	USB_RX_AGG_MODE	UsbRxAggMode;
	u8	UsbRxAggBlockCount;		
	u8	UsbRxAggBlockTimeout;
	u8	UsbRxAggPageCount;		
	u8	UsbRxAggPageTimeout;
#endif
#endif


#ifdef CONFIG_PCI_HCI


	u16	EEPROMDID;
	u16	EEPROMSMID;
	u16	EEPROMChannelPlan;
	
	u8	EEPROMTSSI[2];
	u8	EEPROMBoardType;
	u32	TransmitConfig;	

	u32	IntrMask[2];
	u32	IntrMaskToSet[2];
	
	u8	bDefaultAntenna;
	u8	bIQKInitialized;
	
	u8	bInterruptMigration;
	u8	bDisableTxInt;
	u8	bGpioHwWpsPbc;
#endif


#ifdef CONFIG_TX_EARLY_MODE
	u8 			bEarlyModeEnable;
#endif
} HAL_DATA_8188E, *PHAL_DATA_8188E;

typedef struct hal_data_8188e HAL_DATA_TYPE, *PHAL_DATA_TYPE;


#define GET_HAL_DATA(__pAdapter)	((HAL_DATA_TYPE *)((__pAdapter)->HalData))
#define GET_RF_TYPE(priv)			(GET_HAL_DATA(priv)->rf_type)

#define INCLUDE_MULTI_FUNC_BT(_Adapter)	(GET_HAL_DATA(_Adapter)->MultiFunc & RT_MULTI_FUNC_BT)
#define INCLUDE_MULTI_FUNC_GPS(_Adapter)	(GET_HAL_DATA(_Adapter)->MultiFunc & RT_MULTI_FUNC_GPS)



#ifdef CONFIG_PCI_HCI
void InterruptRecognized8188EE(PADAPTER Adapter, PRT_ISR_CONTENT pIsrContent);
void UpdateInterruptMask8188EE(PADAPTER Adapter, u32 AddMSR, u32 AddMSR1, u32 RemoveMSR, u32 RemoveMSR1);
#endif

#ifdef CONFIG_WOWLAN
s32 rtl8188e_FirmwareDownload(PADAPTER padapter, BOOLEAN  bUsedWoWLANFw);
#else
s32 rtl8188e_FirmwareDownload(PADAPTER padapter);
#endif
void _8051Reset88E(PADAPTER padapter);
void rtl8188e_InitializeFirmwareVars(PADAPTER padapter);


s32 InitLLTTable(PADAPTER padapter, u8 txpktbuf_bndy);
void Read_LLT_Tab(PADAPTER padapter);

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

#ifdef CONFIG_RF_GAIN_OFFSET
void Hal_ReadRFGainOffset(PADAPTER pAdapter,u8* hwinfo,BOOLEAN AutoLoadFail);
#endif

void Hal_InitChannelPlan(PADAPTER padapter);

void rtl8188e_set_hal_ops(struct hal_ops *pHalFunc);

void SetBcnCtrlReg(PADAPTER padapter, u8 SetBits, u8 ClearBits);

void rtl8188e_clone_haldata(_adapter *dst_adapter, _adapter *src_adapter);
void rtl8188e_start_thread(_adapter *padapter);
void rtl8188e_stop_thread(_adapter *padapter);

void rtw_IOL_cmd_tx_pkt_buf_dump(ADAPTER *Adapter,int data_len);
#ifdef CONFIG_IOL_EFUSE_PATCH
s32 rtl8188e_iol_efuse_patch(PADAPTER padapter);
#endif//CONFIG_IOL_EFUSE_PATCH

#endif

