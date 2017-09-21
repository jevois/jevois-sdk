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
/*****************************************************************************
 * Module:	__INC_HAL8192CPHYCFG_H
 *
 *
 * Note:	
 *			
 *
 * Export:	Constants, macro, functions(API), global variables(None).
 *
 * Abbrev:	
 *
 * History:
 *		Data		Who		Remark 
 *      08/07/2007  MHC    	1. Porting from 9x series PHYCFG.h.
 *							2. Reorganize code architecture.
 * 
 *****************************************************************************/
 /* Check to see if the file has been included already.  */
#ifndef __INC_HAL8192CPHYCFG_H
#define __INC_HAL8192CPHYCFG_H


/*--------------------------Define Parameters-------------------------------*/
#define LOOP_LIMIT				5
#define MAX_STALL_TIME			50	
#define AntennaDiversityValue	0x80
#define MAX_TXPWR_IDX_NMODE_92S	63
#define Reset_Cnt_Limit			3


#ifdef CONFIG_PCI_HCI
#define MAX_AGGR_NUM	0x0A0A
#else
#define MAX_AGGR_NUM	0x0909
#endif

#ifdef CONFIG_PCI_HCI
#define	SET_RTL8192SE_RF_SLEEP(_pAdapter)							\
{																	\
	u1Byte		u1bTmp;												\
	u1bTmp = PlatformEFIORead1Byte(_pAdapter, REG_LDOV12D_CTRL);		\
	u1bTmp |= BIT0;													\
	PlatformEFIOWrite1Byte(_pAdapter, REG_LDOV12D_CTRL, u1bTmp);		\
	PlatformEFIOWrite1Byte(_pAdapter, REG_SPS_OCP_CFG, 0x0);				\
	PlatformEFIOWrite1Byte(_pAdapter, TXPAUSE, 0xFF);				\
	PlatformEFIOWrite2Byte(_pAdapter, CMDR, 0x57FC);				\
	delay_us(100);													\
	PlatformEFIOWrite2Byte(_pAdapter, CMDR, 0x77FC);				\
	PlatformEFIOWrite1Byte(_pAdapter, PHY_CCA, 0x0);				\
	delay_us(10);													\
	PlatformEFIOWrite2Byte(_pAdapter, CMDR, 0x37FC);				\
	delay_us(10);													\
	PlatformEFIOWrite2Byte(_pAdapter, CMDR, 0x77FC);				\
	delay_us(10);													\
	PlatformEFIOWrite2Byte(_pAdapter, CMDR, 0x57FC);				\
}
#endif


/*--------------------------Define Parameters-------------------------------*/


/*------------------------------Define structure----------------------------*/ 
typedef enum _SwChnlCmdID{
	CmdID_End,
	CmdID_SetTxPowerLevel,
	CmdID_BBRegWrite10,
	CmdID_WritePortUlong,
	CmdID_WritePortUshort,
	CmdID_WritePortUchar,
	CmdID_RF_WriteReg,
}SwChnlCmdID;


/* 1. Switch channel related */
typedef struct _SwChnlCmd{
	SwChnlCmdID	CmdID;
	u32			Para1;
	u32			Para2;
	u32			msDelay;
}SwChnlCmd;

typedef enum _HW90_BLOCK{
	HW90_BLOCK_MAC = 0,
	HW90_BLOCK_PHY0 = 1,
	HW90_BLOCK_PHY1 = 2,
	HW90_BLOCK_RF = 3,
	HW90_BLOCK_MAXIMUM = 4,
}HW90_BLOCK_E, *PHW90_BLOCK_E;

typedef enum _RF_RADIO_PATH{
	RF_PATH_A = 0,		
	RF_PATH_B = 1,		
	RF_PATH_C = 2,		
	RF_PATH_D = 3,		
}RF_RADIO_PATH_E, *PRF_RADIO_PATH_E;

#define	RF_PATH_MAX			2

#define CHANNEL_MAX_NUMBER		14
#define CHANNEL_GROUP_MAX		3

typedef enum _WIRELESS_MODE {
	WIRELESS_MODE_UNKNOWN = 0x00,
	WIRELESS_MODE_A 		= BIT2,
	WIRELESS_MODE_B 		= BIT0,
	WIRELESS_MODE_G 		= BIT1,
	WIRELESS_MODE_AUTO 	= BIT5,
	WIRELESS_MODE_N_24G 	= BIT3,
	WIRELESS_MODE_N_5G 	= BIT4,
	WIRELESS_MODE_AC		= BIT6
} WIRELESS_MODE;

typedef enum _BaseBand_Config_Type{
	BaseBand_Config_PHY_REG = 0,		
	BaseBand_Config_AGC_TAB = 1,		
}BaseBand_Config_Type, *PBaseBand_Config_Type;


typedef enum _PHY_Rate_Tx_Power_Offset_Area{
	RA_OFFSET_LEGACY_OFDM1,
	RA_OFFSET_LEGACY_OFDM2,
	RA_OFFSET_HT_OFDM1,
	RA_OFFSET_HT_OFDM2,
	RA_OFFSET_HT_OFDM3,
	RA_OFFSET_HT_OFDM4,
	RA_OFFSET_HT_CCK,
}RA_OFFSET_AREA,*PRA_OFFSET_AREA;


/* BB/RF related */
typedef	enum _RF_TYPE_8190P{
	RF_TYPE_MIN,
	RF_8225=1,		
	RF_8256=2,		
	RF_8258=3,		
	RF_6052=4,	
	RF_PSEUDO_11N=5,
}RF_TYPE_8190P_E,*PRF_TYPE_8190P_E;


typedef struct _BB_REGISTER_DEFINITION{
	u32 rfintfs;		
							
	u32 rfintfi;		
							
	u32 rfintfo; 	
							
	u32 rfintfe; 	
							
	u32 rf3wireOffset;
							
	u32 rfLSSI_Select;
							
	u32 rfTxGainStage;
							
	u32 rfHSSIPara1; 
							
	u32 rfHSSIPara2; 
								
	u32 rfSwitchControl;
								
	u32 rfAGCControl1; 
								
	u32 rfAGCControl2; 
							
	u32 rfRxIQImbalance;
							
	u32 rfRxAFE;  	
							
	u32 rfTxIQImbalance;
							
	u32 rfTxAFE; 	
								
	u32 rfLSSIReadBack; 

	u32 rfLSSIReadBackPi; 

}BB_REGISTER_DEFINITION_T, *PBB_REGISTER_DEFINITION_T;

typedef struct _R_ANTENNA_SELECT_OFDM{	
	u32			r_tx_antenna:4;	
	u32			r_ant_l:4;
	u32			r_ant_non_ht:4;	
	u32			r_ant_ht1:4;
	u32			r_ant_ht2:4;
	u32			r_ant_ht_s1:4;
	u32			r_ant_non_ht_s1:4;
	u32			OFDM_TXSC:2;
	u32			Reserved:2;
}R_ANTENNA_SELECT_OFDM;

typedef struct _R_ANTENNA_SELECT_CCK{
	u8			r_cckrx_enable_2:2;	
	u8			r_cckrx_enable:2;
	u8			r_ccktx_enable:4;
}R_ANTENNA_SELECT_CCK;

/*------------------------------Define structure----------------------------*/ 


/*------------------------Export global variable----------------------------*/
/*------------------------Export global variable----------------------------*/


/*------------------------Export Marco Definition---------------------------*/
/*------------------------Export Marco Definition---------------------------*/


/*--------------------------Exported Function prototype---------------------*/
u32	rtl8192c_PHY_QueryBBReg(	IN	PADAPTER	Adapter,
								IN	u32		RegAddr,
								IN	u32		BitMask	);
void	rtl8192c_PHY_SetBBReg(	IN	PADAPTER	Adapter,
								IN	u32		RegAddr,
								IN	u32		BitMask,
								IN	u32		Data	);
u32	rtl8192c_PHY_QueryRFReg(	IN	PADAPTER			Adapter,
								IN	RF_RADIO_PATH_E	eRFPath,
								IN	u32				RegAddr,
								IN	u32				BitMask	);
void	rtl8192c_PHY_SetRFReg(	IN	PADAPTER			Adapter,
								IN	RF_RADIO_PATH_E	eRFPath,
								IN	u32				RegAddr,
								IN	u32				BitMask,
								IN	u32				Data	);

/* MAC/BB/RF HAL config */
int	PHY_MACConfig8192C(	IN	PADAPTER	Adapter	);
int	PHY_BBConfig8192C(	IN	PADAPTER	Adapter	);
int	PHY_RFConfig8192C(	IN	PADAPTER	Adapter	);
/* RF config */
int	rtl8192c_PHY_ConfigRFWithParaFile(	IN	PADAPTER	Adapter,
												IN	u8* 	pFileName,
												IN	RF_RADIO_PATH_E	eRFPath);
int	rtl8192c_PHY_ConfigRFWithHeaderFile(	IN	PADAPTER			Adapter,
												IN	RF_RADIO_PATH_E	eRFPath);

/* BB/RF readback check for making sure init OK */
int	rtl8192c_PHY_CheckBBAndRFOK(	IN	PADAPTER			Adapter,
										IN	HW90_BLOCK_E		CheckBlock,
										IN	RF_RADIO_PATH_E	eRFPath	  );
/* Read initi reg value for tx power setting. */
void	rtl8192c_PHY_GetHWRegOriginalValue(	IN	PADAPTER		Adapter	);


void	PHY_GetTxPowerLevel8192C(	IN	PADAPTER		Adapter,
											OUT u32*    		powerlevel	);
void	PHY_SetTxPowerLevel8192C(	IN	PADAPTER		Adapter,
											IN	u8			channel	);
BOOLEAN	PHY_UpdateTxPowerDbm8192C(	IN	PADAPTER	Adapter,
											IN	int		powerInDbm	);

VOID 
PHY_ScanOperationBackup8192C(IN	PADAPTER	Adapter,
										IN	u8		Operation	);

void	PHY_SetBWMode8192C(	IN	PADAPTER			pAdapter,
									IN	HT_CHANNEL_WIDTH	ChnlWidth,
									IN	unsigned char	Offset	);


extern	void FillA2Entry8192C(		IN	PADAPTER			Adapter,
										IN	u8				index,
										IN	u8*				val);


void	PHY_SwChnl8192C(	IN	PADAPTER		pAdapter,
									IN	u8			channel	);
void	PHY_SwChnlPhy8192C(	IN	PADAPTER		pAdapter,
									IN	u8			channel	);

void ChkFwCmdIoDone(	IN	PADAPTER	Adapter);
				
void	PHY_SetMonitorMode8192C(IN	PADAPTER	pAdapter,
										IN	BOOLEAN		bEnableMonitorMode	);

BOOLEAN	PHY_CheckIsLegalRfPath8192C(IN	PADAPTER	pAdapter,
											IN	u32		eRFPath	);


VOID rtl8192c_PHY_SetRFPathSwitch(IN	PADAPTER	pAdapter, IN	BOOLEAN		bMain);

void	
rtl8192c_PHY_SetBeaconHwReg(	IN	PADAPTER		Adapter,
					IN	u16			BeaconInterval	);


extern	VOID
PHY_SwitchEphyParameter(
	IN	PADAPTER			Adapter
	);

extern	VOID
PHY_EnableHostClkReq(
	IN	PADAPTER			Adapter
	);

BOOLEAN
SetAntennaConfig92C(
	IN	PADAPTER	Adapter,
	IN	u8		DefaultAnt	
	);

#ifdef RTL8192C_RECONFIG_TO_1T1R
extern void	PHY_Reconfig_To_1T1R(_adapter *padapter);
#endif
/*--------------------------Exported Function prototype---------------------*/

#define PHY_QueryBBReg(Adapter, RegAddr, BitMask) rtl8192c_PHY_QueryBBReg((Adapter), (RegAddr), (BitMask))
#define PHY_SetBBReg(Adapter, RegAddr, BitMask, Data) rtl8192c_PHY_SetBBReg((Adapter), (RegAddr), (BitMask), (Data))
#define PHY_QueryRFReg(Adapter, eRFPath, RegAddr, BitMask) rtl8192c_PHY_QueryRFReg((Adapter), (eRFPath), (RegAddr), (BitMask))
#define PHY_SetRFReg(Adapter, eRFPath, RegAddr, BitMask, Data) rtl8192c_PHY_SetRFReg((Adapter), (eRFPath), (RegAddr), (BitMask), (Data))

#define PHY_SetMacReg	PHY_SetBBReg

#endif

