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
#ifndef __RTL8192C_HAL_H__
#define __RTL8192C_HAL_H__

#include "rtl8192c_spec.h"
#include "Hal8192CPhyReg.h"
#include "Hal8192CPhyCfg.h"
#include "rtl8192c_rf.h"
#include "rtl8192c_dm.h"
#include "rtl8192c_recv.h"
#include "rtl8192c_xmit.h"
#include "rtl8192c_cmd.h"

#ifdef DBG_CONFIG_ERROR_DETECT
#include "rtl8192c_sreset.h"
#endif
#include "rtw_efuse.h"

#include "../hal/OUTSRC/odm_precomp.h"


#ifdef CONFIG_PCI_HCI
	
	#define RTL819X_DEFAULT_RF_TYPE			RF_2T2R
	#define RTL819X_TOTAL_RF_PATH				2

	#define RTL8192C_FW_TSMC_IMG				"rtl8192CE\\rtl8192cfwT.bin"
	#define RTL8192C_FW_UMC_IMG				"rtl8192CE\\rtl8192cfwU.bin"
	#define RTL8192C_FW_UMC_B_IMG				"rtl8192CE\\rtl8192cfwU_B.bin"

	#define RTL8188C_PHY_REG					"rtl8192CE\\PHY_REG_1T.txt"
	#define RTL8188C_PHY_RADIO_A				"rtl8192CE\\radio_a_1T.txt"
	#define RTL8188C_PHY_RADIO_B				"rtl8192CE\\radio_b_1T.txt"
	#define RTL8188C_AGC_TAB					"rtl8192CE\\AGC_TAB_1T.txt"
	#define RTL8188C_PHY_MACREG				"rtl8192CE\\MACREG_1T.txt"

	#define RTL8192C_PHY_REG					"rtl8192CE\\PHY_REG_2T.txt"
	#define RTL8192C_PHY_RADIO_A				"rtl8192CE\\radio_a_2T.txt"
	#define RTL8192C_PHY_RADIO_B				"rtl8192CE\\radio_b_2T.txt"
	#define RTL8192C_AGC_TAB					"rtl8192CE\\AGC_TAB_2T.txt"
	#define RTL8192C_PHY_MACREG				"rtl8192CE\\MACREG_2T.txt"

	#define RTL819X_PHY_MACPHY_REG			"rtl8192CE\\MACPHY_reg.txt"
	#define RTL819X_PHY_MACPHY_REG_PG		"rtl8192CE\\MACPHY_reg_PG.txt"
	#define RTL819X_PHY_MACREG				"rtl8192CE\\MAC_REG.txt"
	#define RTL819X_PHY_REG					"rtl8192CE\\PHY_REG.txt"
	#define RTL819X_PHY_REG_1T2R				"rtl8192CE\\PHY_REG_1T2R.txt"
	#define RTL819X_PHY_REG_to1T1R				"rtl8192CE\\phy_to1T1R_a.txt"
	#define RTL819X_PHY_REG_to1T2R				"rtl8192CE\\phy_to1T2R.txt"
	#define RTL819X_PHY_REG_to2T2R				"rtl8192CE\\phy_to2T2R.txt"
	#define RTL819X_PHY_REG_PG					"rtl8192CE\\PHY_REG_PG.txt"
	#define RTL819X_AGC_TAB					"rtl8192CE\\AGC_TAB.txt"
	#define RTL819X_PHY_RADIO_A				"rtl8192CE\\radio_a.txt"
	#define RTL819X_PHY_RADIO_A_1T			"rtl8192CE\\radio_a_1t.txt"
	#define RTL819X_PHY_RADIO_A_2T			"rtl8192CE\\radio_a_2t.txt"
	#define RTL819X_PHY_RADIO_B				"rtl8192CE\\radio_b.txt"
	#define RTL819X_PHY_RADIO_B_GM			"rtl8192CE\\radio_b_gm.txt"
	#define RTL819X_PHY_RADIO_C				"rtl8192CE\\radio_c.txt"
	#define RTL819X_PHY_RADIO_D				"rtl8192CE\\radio_d.txt"
	#define RTL819X_EEPROM_MAP				"rtl8192CE\\8192ce.map"
	#define RTL819X_EFUSE_MAP					"rtl8192CE\\8192ce.map"


	#define Rtl819XFwTSMCImageArray			Rtl8192CEFwTSMCImgArray
	#define Rtl819XFwUMCACutImageArray			Rtl8192CEFwUMCACutImgArray
	#define Rtl819XFwUMCBCutImageArray			Rtl8192CEFwUMCBCutImgArray
	
	#define Rtl819XMAC_Array					Rtl8192CEMAC_2T_Array
	#define Rtl819XAGCTAB_2TArray				Rtl8192CEAGCTAB_2TArray
	#define Rtl819XAGCTAB_1TArray				Rtl8192CEAGCTAB_1TArray
	#define Rtl819XPHY_REG_2TArray				Rtl8192CEPHY_REG_2TArray
	#define Rtl819XPHY_REG_1TArray				Rtl8192CEPHY_REG_1TArray
	#define Rtl819XRadioA_2TArray				Rtl8192CERadioA_2TArray
	#define Rtl819XRadioA_1TArray				Rtl8192CERadioA_1TArray
	#define Rtl819XRadioB_2TArray				Rtl8192CERadioB_2TArray
	#define Rtl819XRadioB_1TArray				Rtl8192CERadioB_1TArray
	#define Rtl819XPHY_REG_Array_PG 			Rtl8192CEPHY_REG_Array_PG
	#define Rtl819XPHY_REG_Array_MP 			Rtl8192CEPHY_REG_Array_MP

	#define PHY_REG_2TArrayLength 				Rtl8192CEPHY_REG_2TArrayLength 
	#define PHY_REG_1TArrayLength 				Rtl8192CEPHY_REG_1TArrayLength 
	#define PHY_ChangeTo_1T1RArrayLength 		Rtl8192CEPHY_ChangeTo_1T1RArrayLength 
	#define PHY_ChangeTo_1T2RArrayLength  		Rtl8192CEPHY_ChangeTo_1T2RArrayLength 
	#define PHY_ChangeTo_2T2RArrayLength  		Rtl8192CEPHY_ChangeTo_2T2RArrayLength 
	#define PHY_REG_Array_PGLength  			Rtl8192CEPHY_REG_Array_PGLength 
	#define PHY_REG_Array_MPLength 			Rtl8192CEPHY_REG_Array_MPLength 
	#define PHY_REG_Array_MPLength 			Rtl8192CEPHY_REG_Array_MPLength 
	#define RadioA_2TArrayLength  				Rtl8192CERadioA_2TArrayLength 
	#define RadioB_2TArrayLength 				Rtl8192CERadioB_2TArrayLength 
	#define RadioA_1TArrayLength  				Rtl8192CERadioA_1TArrayLength 
	#define RadioB_1TArrayLength 				Rtl8192CERadioB_1TArrayLength 
	#define RadioB_GM_ArrayLength 				Rtl8192CERadioB_GM_ArrayLength 
	#define MAC_2T_ArrayLength					Rtl8192CEMAC_2T_ArrayLength 
	#define MACPHY_Array_PGLength 				Rtl8192CEMACPHY_Array_PGLength 
	#define AGCTAB_2TArrayLength 				Rtl8192CEAGCTAB_2TArrayLength 
	#define AGCTAB_1TArrayLength 				Rtl8192CEAGCTAB_1TArrayLength 

#elif defined(CONFIG_USB_HCI)


	#define RTL819X_DEFAULT_RF_TYPE			RF_1T2R
	#define RTL819X_TOTAL_RF_PATH				2

	#define RTL8192C_FW_TSMC_IMG				"rtl8192CU\\rtl8192cfwT.bin"
	#define RTL8192C_FW_UMC_IMG				"rtl8192CU\\rtl8192cfwU.bin"
	#define RTL8192C_FW_UMC_B_IMG				"rtl8192CU\\rtl8192cfwU_B.bin"


	#define RTL8188C_PHY_REG					"rtl8188CU\\PHY_REG.txt"
	#define RTL8188C_PHY_RADIO_A				"rtl8188CU\\radio_a.txt"
	#define RTL8188C_PHY_RADIO_B				"rtl8188CU\\radio_b.txt"
	#define RTL8188C_PHY_RADIO_A_mCard		"rtl8192CU\\radio_a_1T_mCard.txt"
	#define RTL8188C_PHY_RADIO_B_mCard		"rtl8192CU\\radio_b_1T_mCard.txt" 
	#define RTL8188C_PHY_RADIO_A_HP			"rtl8192CU\\radio_a_1T_HP.txt"
	#define RTL8188C_AGC_TAB					"rtl8188CU\\AGC_TAB.txt"
	#define RTL8188C_PHY_MACREG				"rtl8188CU\\MACREG.txt"

	#define RTL8192C_PHY_REG					"rtl8192CU\\PHY_REG.txt"
	#define RTL8192C_PHY_RADIO_A				"rtl8192CU\\radio_a.txt"
	#define RTL8192C_PHY_RADIO_B				"rtl8192CU\\radio_b.txt"
	#define RTL8192C_AGC_TAB					"rtl8192CU\\AGC_TAB.txt"
	#define RTL8192C_PHY_MACREG				"rtl8192CU\\MACREG.txt"

	#define RTL819X_PHY_REG_PG					"rtl8192CU\\PHY_REG_PG.txt"


	#define Rtl819XFwImageArray					Rtl8192CUFwTSMCImgArray
	#define Rtl819XFwTSMCImageArray			Rtl8192CUFwTSMCImgArray
	#define Rtl819XFwUMCACutImageArray			Rtl8192CUFwUMCACutImgArray
	#define Rtl819XFwUMCBCutImageArray			Rtl8192CUFwUMCBCutImgArray

	#define Rtl819XMAC_Array					Rtl8192CUMAC_2T_Array
	#define Rtl819XAGCTAB_2TArray				Rtl8192CUAGCTAB_2TArray
	#define Rtl819XAGCTAB_1TArray				Rtl8192CUAGCTAB_1TArray
	#define Rtl819XAGCTAB_1T_HPArray			Rtl8192CUAGCTAB_1T_HPArray
	#define Rtl819XPHY_REG_2TArray				Rtl8192CUPHY_REG_2TArray
	#define Rtl819XPHY_REG_1TArray				Rtl8192CUPHY_REG_1TArray
	#define Rtl819XPHY_REG_1T_mCardArray		Rtl8192CUPHY_REG_1T_mCardArray 					
	#define Rtl819XPHY_REG_2T_mCardArray		Rtl8192CUPHY_REG_2T_mCardArray	
	#define Rtl819XPHY_REG_1T_HPArray			Rtl8192CUPHY_REG_1T_HPArray
	#define Rtl819XRadioA_2TArray				Rtl8192CURadioA_2TArray
	#define Rtl819XRadioA_1TArray				Rtl8192CURadioA_1TArray
	#define Rtl819XRadioA_1T_mCardArray			Rtl8192CURadioA_1T_mCardArray			
	#define Rtl819XRadioB_2TArray				Rtl8192CURadioB_2TArray
	#define Rtl819XRadioB_1TArray				Rtl8192CURadioB_1TArray	
	#define Rtl819XRadioB_1T_mCardArray			Rtl8192CURadioB_1T_mCardArray
	#define Rtl819XRadioA_1T_HPArray			Rtl8192CURadioA_1T_HPArray	
	#define Rtl819XPHY_REG_Array_PG 			Rtl8192CUPHY_REG_Array_PG
	#define Rtl819XPHY_REG_Array_PG_mCard 		Rtl8192CUPHY_REG_Array_PG_mCard			
	#define Rtl819XPHY_REG_Array_PG_HP			Rtl8192CUPHY_REG_Array_PG_HP
	#define Rtl819XPHY_REG_Array_MP 			Rtl8192CUPHY_REG_Array_MP

	#define PHY_REG_2TArrayLength 				Rtl8192CUPHY_REG_2TArrayLength 
	#define PHY_REG_1TArrayLength 				Rtl8192CUPHY_REG_1TArrayLength 
	#define PHY_ChangeTo_1T1RArrayLength 		Rtl8192CUPHY_ChangeTo_1T1RArrayLength 
	#define PHY_ChangeTo_1T2RArrayLength  		Rtl8192CUPHY_ChangeTo_1T2RArrayLength 
	#define PHY_ChangeTo_2T2RArrayLength  		Rtl8192CUPHY_ChangeTo_2T2RArrayLength 
	#define PHY_REG_Array_PGLength  			Rtl8192CUPHY_REG_Array_PGLength 
	#define PHY_REG_Array_PG_mCardLength 		Rtl8192CUPHY_REG_Array_PG_mCardLength 
	#define PHY_REG_Array_MPLength 			Rtl8192CUPHY_REG_Array_MPLength 
	#define PHY_REG_Array_MPLength 			Rtl8192CUPHY_REG_Array_MPLength 
	#define PHY_REG_1T_mCardArrayLength 		Rtl8192CUPHY_REG_1T_mCardArrayLength 
	#define PHY_REG_2T_mCardArrayLength  		Rtl8192CUPHY_REG_2T_mCardArrayLength 
	#define PHY_REG_Array_PG_HPLength 			Rtl8192CUPHY_REG_Array_PG_HPLength 
	#define RadioA_2TArrayLength  				Rtl8192CURadioA_2TArrayLength 
	#define RadioB_2TArrayLength 				Rtl8192CURadioB_2TArrayLength 
	#define RadioA_1TArrayLength  				Rtl8192CURadioA_1TArrayLength 
	#define RadioB_1TArrayLength 				Rtl8192CURadioB_1TArrayLength 
	#define RadioA_1T_mCardArrayLength 			Rtl8192CURadioA_1T_mCardArrayLength 
	#define RadioB_1T_mCardArrayLength 			Rtl8192CURadioB_1T_mCardArrayLength 
	#define RadioA_1T_HPArrayLength 				Rtl8192CURadioA_1T_HPArrayLength 
	#define RadioB_GM_ArrayLength 				Rtl8192CURadioB_GM_ArrayLength 
	#define MAC_2T_ArrayLength					Rtl8192CUMAC_2T_ArrayLength 
	#define MACPHY_Array_PGLength 				Rtl8192CUMACPHY_Array_PGLength 
	#define AGCTAB_2TArrayLength 				Rtl8192CUAGCTAB_2TArrayLength 
	#define AGCTAB_1TArrayLength 				Rtl8192CUAGCTAB_1TArrayLength 
	#define AGCTAB_1T_HPArrayLength 			Rtl8192CUAGCTAB_1T_HPArrayLength 
	#define PHY_REG_1T_HPArrayLength			Rtl8192CUPHY_REG_1T_HPArrayLength

#endif

#define DRVINFO_SZ	4
#define PageNum_128(_Len)		(u32)(((_Len)>>7) + ((_Len)&0x7F ? 1:0))

#define FW_8192C_SIZE					16384+32//16k
#define FW_8192C_START_ADDRESS		0x1000
#define FW_8192C_END_ADDRESS		0x1FFF

#define MAX_PAGE_SIZE			4096

#define IS_FW_HEADER_EXIST(_pFwHdr)	((le16_to_cpu(_pFwHdr->Signature)&0xFFF0) == 0x92C0 ||\
									(le16_to_cpu(_pFwHdr->Signature)&0xFFF0) == 0x88C0 ||\
									(le16_to_cpu(_pFwHdr->Signature)&0xFFF0) == 0x2300)

typedef enum _FIRMWARE_SOURCE{
	FW_SOURCE_IMG_FILE = 0,
	FW_SOURCE_HEADER_FILE = 1,	
}FIRMWARE_SOURCE, *PFIRMWARE_SOURCE;

typedef struct _RT_FIRMWARE{
	FIRMWARE_SOURCE	eFWSource;
	u8*			szFwBuffer;
	u32			ulFwLength;
}RT_FIRMWARE, *PRT_FIRMWARE, RT_FIRMWARE_92C, *PRT_FIRMWARE_92C;

typedef struct _RT_8192C_FIRMWARE_HDR {//8-byte alinment required

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
	u16		Rsvd2;

	u32		SvnIdx;
	u32		Rsvd3;

	u32		Rsvd4;
	u32		Rsvd5;

}RT_8192C_FIRMWARE_HDR, *PRT_8192C_FIRMWARE_HDR;

#define DRIVER_EARLY_INT_TIME		0x05
#define BCN_DMA_ATIME_INT_TIME		0x02

#ifdef CONFIG_USB_RX_AGGREGATION

typedef enum _USB_RX_AGG_MODE{
	USB_RX_AGG_DISABLE,
	USB_RX_AGG_DMA,
	USB_RX_AGG_USB,
	USB_RX_AGG_MIX
}USB_RX_AGG_MODE;

#define MAX_RX_DMA_BUFFER_SIZE	10240	

#endif


#define TX_SELE_HQ			BIT(0)	
#define TX_SELE_LQ			BIT(1)	
#define TX_SELE_NQ			BIT(2)	



#define TX_TOTAL_PAGE_NUMBER		0xF8
#define TX_PAGE_BOUNDARY		(TX_TOTAL_PAGE_NUMBER + 1)

#define NORMAL_PAGE_NUM_PUBQ			0xE7
#define NORMAL_PAGE_NUM_HPQ			0x0C
#define NORMAL_PAGE_NUM_LPQ			0x02
#define NORMAL_PAGE_NUM_NPQ			0x02


#define TEST_PAGE_NUM_PUBQ		0x7E


#define WMM_TEST_TX_TOTAL_PAGE_NUMBER	0xF5
#define WMM_TEST_TX_PAGE_BOUNDARY	(WMM_TEST_TX_TOTAL_PAGE_NUMBER + 1)

#define WMM_TEST_PAGE_NUM_PUBQ		0xA3
#define WMM_TEST_PAGE_NUM_HPQ		0x29
#define WMM_TEST_PAGE_NUM_LPQ		0x29


#define WMM_NORMAL_TX_TOTAL_PAGE_NUMBER	0xF5
#define WMM_NORMAL_TX_PAGE_BOUNDARY	(WMM_TEST_TX_TOTAL_PAGE_NUMBER + 1)

#define WMM_NORMAL_PAGE_NUM_PUBQ		0xB0
#define WMM_NORMAL_PAGE_NUM_HPQ		0x29
#define WMM_NORMAL_PAGE_NUM_LPQ			0x1C
#define WMM_NORMAL_PAGE_NUM_NPQ		0x1C

#define CHIP_BONDING_IDENTIFIER(_value)	(((_value)>>22)&0x3)
#define CHIP_BONDING_92C_1T2R	0x1
#define CHIP_BONDING_88C_USB_MCARD	0x2
#define CHIP_BONDING_88C_USB_HP	0x1

#include "HalVerDef.h"
#include "hal_com.h"

enum ChannelPlan{
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

typedef struct _TxPowerInfo{
	u8 CCKIndex[RF_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 HT40_1SIndex[RF_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 HT40_2SIndexDiff[RF_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 HT20IndexDiff[RF_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 OFDMIndexDiff[RF_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 HT40MaxOffset[RF_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 HT20MaxOffset[RF_PATH_MAX][CHANNEL_GROUP_MAX];
	u8 TSSI_A;
	u8 TSSI_B;
}TxPowerInfo, *PTxPowerInfo;

#define		EFUSE_REAL_CONTENT_LEN		512
#define		EFUSE_MAP_LEN					128
#define		EFUSE_MAX_SECTION			16
#define		EFUSE_IC_ID_OFFSET			506
#define 		AVAILABLE_EFUSE_ADDR(addr) 	(addr < EFUSE_REAL_CONTENT_LEN)
#define		EFUSE_OOB_PROTECT_BYTES 		15


#define		EFUSE_MAP_LEN_8723			256
#define		EFUSE_MAX_SECTION_8723		32

#define		EFUSE_BT_REAL_CONTENT_LEN		1536
#define		EFUSE_BT_MAP_LEN					1024
#define		EFUSE_BT_MAX_SECTION				128	

#define		EFUSE_PROTECT_BYTES_BANK			16

typedef enum _RT_MULTI_FUNC{
	RT_MULTI_FUNC_NONE = 0x00,
	RT_MULTI_FUNC_WIFI = 0x01,
	RT_MULTI_FUNC_BT = 0x02,
	RT_MULTI_FUNC_GPS = 0x04,
}RT_MULTI_FUNC,*PRT_MULTI_FUNC;

typedef enum _RT_POLARITY_CTL{
	RT_POLARITY_LOW_ACT = 0,
	RT_POLARITY_HIGH_ACT = 1,	
}RT_POLARITY_CTL,*PRT_POLARITY_CTL;

typedef enum _RT_REGULATOR_MODE{
	RT_SWITCHING_REGULATOR = 0,
	RT_LDO_REGULATOR = 1,	
}RT_REGULATOR_MODE,*PRT_REGULATOR_MODE;

enum c2h_id_8192c {
	C2H_DBG = 0,
	C2H_TSF = 1,
	C2H_AP_RPT_RSP = 2,
	C2H_CCX_TX_RPT = 3,
	C2H_BT_RSSI = 4,
	C2H_BT_OP_MODE = 5,
	C2H_EXT_RA_RPT = 6,
	C2H_HW_INFO_EXCH = 10,
	C2H_C2H_H2C_TEST = 11,
	C2H_BT_INFO = 12,
	C2H_BT_MP_INFO = 15,
	MAX_C2HEVENT
};

#ifdef CONFIG_PCI_HCI
struct hal_data_8192ce
{
	HAL_VERSION			VersionID;
	RT_MULTI_FUNC		MultiFunc;
	RT_POLARITY_CTL		PolarityCtl;
	RT_REGULATOR_MODE	RegulatorMode;
	u16	CustomerID;

	u16	FirmwareVersion;
	u16	FirmwareVersionRev;
	u16	FirmwareSubVersion;

	u32	IntrMask[2];
	u32	IntrMaskToSet[2];

	u32	DisabledFunctions;
	
	u32	ReceiveConfig;
	u32	TransmitConfig;
	WIRELESS_MODE		CurrentWirelessMode;
	HT_CHANNEL_WIDTH	CurrentChannelBW;
	u8	CurrentChannel;
	u8	nCur40MhzPrimeSC;// Control channel sub-carrier

	u16	BasicRateSet;

	_lock	rf_lock;
	u8	rf_chip;
	u8	rf_type;
	u8	NumTotalRFPath;

	INTERFACE_SELECT_8192CPCIe	InterfaceSel;

	u16	EEPROMVID;
	u16	EEPROMDID;
	u16	EEPROMSVID;
	u16	EEPROMSMID;
	u16	EEPROMChannelPlan;
	u16	EEPROMVersion;

	u8	EEPROMChnlAreaTxPwrCCK[2][3];	
	u8	EEPROMChnlAreaTxPwrHT40_1S[2][3];	
	u8	EEPROMChnlAreaTxPwrHT40_2SDiff[2][3];
	u8	EEPROMPwrLimitHT20[3];
	u8	EEPROMPwrLimitHT40[3];

	u8	bTXPowerDataReadFromEEPORM;
	u8	EEPROMThermalMeter;
	u8	EEPROMTSSI[2];

	u8	EEPROMCustomerID;
	u8	EEPROMBoardType;
	u8	EEPROMRegulatory;

	u8	bDefaultAntenna;
	u8	bIQKInitialized;

	u8	TxPwrLevelCck[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	u8	TxPwrLevelHT40_1S[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	u8	TxPwrLevelHT40_2S[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	u8	TxPwrHt20Diff[RF_PATH_MAX][CHANNEL_MAX_NUMBER];// HT 20<->40 Pwr diff
	u8	TxPwrLegacyHtDiff[RF_PATH_MAX][CHANNEL_MAX_NUMBER];// For HT<->legacy pwr diff
	u8	PwrGroupHT20[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	u8	PwrGroupHT40[RF_PATH_MAX][CHANNEL_MAX_NUMBER];

	u8	LegacyHTTxPowerDiff;// Legacy to HT rate power diff

	BOOLEAN 			EepromOrEfuse;
	u8				EfuseMap[2][HWSET_MAX_SIZE_512];
	u8				EfuseUsedPercentage;
	EFUSE_HAL			EfuseHal;
	
#ifdef CONFIG_BT_COEXIST
	struct btcoexist_priv	bt_coexist;
#endif

	u8	framesync;
	u32	framesyncC34;
	u8	framesyncMonitor;
	u8	DefaultInitialGain[4];
	u8	pwrGroupCnt;
	u32	MCSTxPowerLevelOriginalOffset[7][16];
	u32	CCKTxPowerLevelOriginalOffset;

	u32	AntennaTxPath;				
	u32	AntennaRxPath;				
	u8	BluetoothCoexist;
	u8	ExternalPA;

	u8	bLedOpenDrain;
	u8	b1x1RecvCombine;
	
	u32	AcParam_BE;

	u8	CurrentCckTxPwrIdx;
	u8	CurrentOfdm24GTxPwrIdx;

	BB_REGISTER_DEFINITION_T	PHYRegDef[4];
	
	u32	RfRegChnlVal[2];
	
	BOOLEAN	 bRDGEnable;

	u8	LastHMEBoxNum;

	u8	fw_ractrl;
	u8	RegTxPause;
	u32	RegBcnCtrlVal;
	u8	RegFwHwTxQCtrl;
	u8	RegReg542;
	u8	CurAntenna;	
	
	u8	AntDivCfg;
/*
#ifdef CONFIG_SW_ANTENNA_DIVERSITY
	s32				RSSI_sum_A;
	s32				RSSI_sum_B;
	s32				RSSI_cnt_A;
	s32				RSSI_cnt_B;
	BOOLEAN		RSSI_test;
#endif
#ifdef CONFIG_HW_ANTENNA_DIVERSITY
	u32				CCK_Ant1_Cnt;
	u32				CCK_Ant2_Cnt;
	u32				OFDM_Ant1_Cnt;
	u32				OFDM_Ant2_Cnt;
#endif
*/
	struct dm_priv	dmpriv;
	DM_ODM_T 		odmpriv;
	u8	bDumpRxPkt;//for debug
#ifdef DBG_CONFIG_ERROR_DETECT
	struct sreset_priv srestpriv;
#endif
	u8	bInterruptMigration;
	u8	bDisableTxInt;
	u8	bGpioHwWpsPbc;

	u8	FwRsvdPageStartOffset;

	u16	EfuseUsedBytes;
	
#ifdef CONFIG_P2P
	struct P2P_PS_Offload_t	p2p_ps_offload;
#endif
};

typedef struct hal_data_8192ce HAL_DATA_TYPE, *PHAL_DATA_TYPE;

#define DF_TX_BIT		BIT0
#define DF_RX_BIT		BIT1
#define DF_IO_BIT		BIT2
#define DF_IO_D3_BIT			BIT3

#define RT_DF_TYPE		u32
#define RT_DISABLE_FUNC(__pAdapter, __FuncBits) ((__pAdapter)->DisabledFunctions |= ((RT_DF_TYPE)(__FuncBits)))
#define RT_ENABLE_FUNC(__pAdapter, __FuncBits) ((__pAdapter)->DisabledFunctions &= (~((RT_DF_TYPE)(__FuncBits))))
#define RT_IS_FUNC_DISABLED(__pAdapter, __FuncBits) ( (__pAdapter)->DisabledFunctions & (__FuncBits) )
#define IS_MULTI_FUNC_CHIP(_Adapter)	(((((PHAL_DATA_TYPE)(_Adapter->HalData))->MultiFunc) & (RT_MULTI_FUNC_BT|RT_MULTI_FUNC_GPS)) ? _TRUE : _FALSE)

void InterruptRecognized8192CE(PADAPTER Adapter, PRT_ISR_CONTENT pIsrContent);
VOID UpdateInterruptMask8192CE(PADAPTER Adapter, u32 AddMSR, u32 AddMSR1, u32 RemoveMSR, u32 RemoveMSR1);
#endif

#ifdef CONFIG_USB_HCI
struct hal_data_8192cu
{
	HAL_VERSION VersionID;
	RT_MULTI_FUNC		MultiFunc;
	RT_POLARITY_CTL		PolarityCtl;
	RT_REGULATOR_MODE	RegulatorMode;
	u16	CustomerID;

	u16	FirmwareVersion;
	u16	FirmwareVersionRev;
	u16	FirmwareSubVersion;

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

	u8	bIQKInitialized;

	u8	TxPwrLevelCck[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	u8	TxPwrLevelHT40_1S[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	u8	TxPwrLevelHT40_2S[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	s8	TxPwrHt20Diff[RF_PATH_MAX][CHANNEL_MAX_NUMBER];// HT 20<->40 Pwr diff
	u8	TxPwrLegacyHtDiff[RF_PATH_MAX][CHANNEL_MAX_NUMBER];// For HT<->legacy pwr diff
	u8	PwrGroupHT20[RF_PATH_MAX][CHANNEL_MAX_NUMBER];
	u8	PwrGroupHT40[RF_PATH_MAX][CHANNEL_MAX_NUMBER];

	u8	LegacyHTTxPowerDiff;// Legacy to HT rate power diff

	u8	framesync;
	u32	framesyncC34;
	u8	framesyncMonitor;
	u8	DefaultInitialGain[4];
	u8	pwrGroupCnt;
	u32	MCSTxPowerLevelOriginalOffset[7][16];
	u32	CCKTxPowerLevelOriginalOffset;

	u32	AntennaTxPath;				
	u32	AntennaRxPath;				
	u8	BluetoothCoexist;
	u8	ExternalPA;

	u8	bLedOpenDrain;

	u8	b1x1RecvCombine;

	u32	AcParam_BE;

	u8	CurrentCckTxPwrIdx;
	u8	CurrentOfdm24GTxPwrIdx;

	BB_REGISTER_DEFINITION_T	PHYRegDef[4];

	u32	RfRegChnlVal[2];

	BOOLEAN	 bRDGEnable;

	u8	LastHMEBoxNum;

	u8	fw_ractrl;
	u8	RegTxPause;
	u32	RegBcnCtrlVal;
	u8	RegFwHwTxQCtrl;
	u8	RegReg542;
	
	struct dm_priv	dmpriv;
	DM_ODM_T 		odmpriv;
#ifdef DBG_CONFIG_ERROR_DETECT
	struct sreset_priv srestpriv;
#endif	

#ifdef CONFIG_BT_COEXIST
	struct btcoexist_priv	bt_coexist;
#endif
	u8	CurAntenna;	
	
/*****ODM duplicate data********/
	u8	AntDivCfg;
/*
#ifdef CONFIG_SW_ANTENNA_DIVERSITY

	s32				RSSI_sum_A;
	s32				RSSI_sum_B;
	s32				RSSI_cnt_A;
	s32				RSSI_cnt_B;
	BOOLEAN		RSSI_test;
#endif

#ifdef CONFIG_HW_ANTENNA_DIVERSITY
	u32				CCK_Ant1_Cnt;
	u32				CCK_Ant2_Cnt;
	u32				OFDM_Ant1_Cnt;
	u32				OFDM_Ant2_Cnt;
#endif
*/
	u8	bDumpRxPkt;//for debug
	u8	FwRsvdPageStartOffset;

	BOOLEAN		pwrdown;


	u32	UsbBulkOutSize;

	u32	interfaceIndex;

	u8	OutEpQueueSel;
	u8	OutEpNumber;

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

	BOOLEAN		UsbRxHighSpeedMode;

	BOOLEAN		SlimComboDbg;

	u16	EfuseUsedBytes;

	BOOLEAN 			EepromOrEfuse;
	u8				EfuseMap[2][HWSET_MAX_SIZE_512];
	u8				EfuseUsedPercentage;
	EFUSE_HAL			EfuseHal;
	

#ifdef CONFIG_P2P
	struct P2P_PS_Offload_t	p2p_ps_offload;
#endif
};

typedef struct hal_data_8192cu HAL_DATA_TYPE, *PHAL_DATA_TYPE;
#endif

#define GET_HAL_DATA(__pAdapter)	((HAL_DATA_TYPE *)((__pAdapter)->HalData))
#define GET_RF_TYPE(priv)	(GET_HAL_DATA(priv)->rf_type)

#define INCLUDE_MULTI_FUNC_BT(_Adapter)	(GET_HAL_DATA(_Adapter)->MultiFunc & RT_MULTI_FUNC_BT)
#define INCLUDE_MULTI_FUNC_GPS(_Adapter)	(GET_HAL_DATA(_Adapter)->MultiFunc & RT_MULTI_FUNC_GPS)

VOID rtl8192c_FirmwareSelfReset(IN PADAPTER Adapter);
int FirmwareDownload92C(IN PADAPTER Adapter);
VOID InitializeFirmwareVars92C(PADAPTER Adapter);
u8 GetEEPROMSize8192C(PADAPTER Adapter);
void rtl8192c_EfuseParseChnlPlan(PADAPTER padapter, u8 *hwinfo, BOOLEAN AutoLoadFail);

HAL_VERSION rtl8192c_ReadChipVersion(IN PADAPTER Adapter);
void rtl8192c_ReadBluetoothCoexistInfo(PADAPTER Adapter, u8 *PROMContent, BOOLEAN AutoloadFail);
VOID rtl8192c_EfuseParseIDCode(PADAPTER pAdapter, u8 *hwinfo);
void rtl8192c_set_hal_ops(struct hal_ops *pHalFunc);
void	rtl8192c_clone_haldata(_adapter* dst_adapter, _adapter* src_adapter);

s32 c2h_id_filter_ccx_8192c(u8 id);
#endif

