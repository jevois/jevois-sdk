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
 *
 * Module:  __INC_HAL8192DPHYCFG_H
 *
 *
 * Note:
 *
 *
 * Export:  Constants, macro, functions(API), global variables(None).
 *
 * Abbrev:
 *
 * History:
 *    Data    Who   Remark
 *      08/07/2007  MHC     1. Porting from 9x series PHYCFG.h.
 *              2. Reorganize code architecture.
 *
 *****************************************************************************/
/* Check to see if the file has been included already.  */
#ifndef __INC_HAL8192DPHYCFG_H
#define __INC_HAL8192DPHYCFG_H


/*--------------------------Define Parameters-------------------------------*/
#define LOOP_LIMIT        5
#define MAX_STALL_TIME      50   
#define AntennaDiversityValue 0x80 
#define MAX_TXPWR_IDX_NMODE_92S 63
#define Reset_Cnt_Limit     3


#ifdef CONFIG_PCI_HCI
#define SET_RTL8192SE_RF_SLEEP(_pAdapter)             \
  {                                 \
    u1Byte    u1bTmp;                       \
    u1bTmp = PlatformEFIORead1Byte(_pAdapter, REG_LDOV12D_CTRL);    \
    u1bTmp |= BIT0;                         \
    PlatformEFIOWrite1Byte(_pAdapter, REG_LDOV12D_CTRL, u1bTmp);    \
    PlatformEFIOWrite1Byte(_pAdapter, REG_SPS_OCP_CFG, 0x0);        \
    PlatformEFIOWrite1Byte(_pAdapter, TXPAUSE, 0xFF);       \
    PlatformEFIOWrite2Byte(_pAdapter, CMDR, 0x57FC);        \
    delay_us(100);                          \
    PlatformEFIOWrite2Byte(_pAdapter, CMDR, 0x77FC);        \
    PlatformEFIOWrite1Byte(_pAdapter, PHY_CCA, 0x0);        \
    delay_us(10);                         \
    PlatformEFIOWrite2Byte(_pAdapter, CMDR, 0x37FC);        \
    delay_us(10);                         \
    PlatformEFIOWrite2Byte(_pAdapter, CMDR, 0x77FC);        \
    delay_us(10);                         \
    PlatformEFIOWrite2Byte(_pAdapter, CMDR, 0x57FC);        \
  }
#endif


/*--------------------------Define Parameters-------------------------------*/


/*------------------------------Define structure----------------------------*/
typedef enum _SwChnlCmdID {
  CmdID_End,
  CmdID_SetTxPowerLevel,
  CmdID_BBRegWrite10,
  CmdID_WritePortUlong,
  CmdID_WritePortUshort,
  CmdID_WritePortUchar,
  CmdID_RF_WriteReg,
} SwChnlCmdID;


/* 1. Switch channel related */
typedef struct _SwChnlCmd {
  SwChnlCmdID CmdID;
  u32     Para1;
  u32     Para2;
  u32     msDelay;
} SwChnlCmd;

typedef enum _HW90_BLOCK {
  HW90_BLOCK_MAC = 0,
  HW90_BLOCK_PHY0 = 1,
  HW90_BLOCK_PHY1 = 2,
  HW90_BLOCK_RF = 3,
  HW90_BLOCK_MAXIMUM = 4,
} HW90_BLOCK_E, *PHW90_BLOCK_E;

typedef enum _RF_CONTENT {
  radioa_txt = 0x1000,
  radiob_txt = 0x1001,
  radioc_txt = 0x1002,
  radiod_txt = 0x1003
} RF_CONTENT;

typedef enum _RF_RADIO_PATH {
  RF_PATH_A = 0,     
  RF_PATH_B = 1,     
  RF_PATH_C = 2,     
  RF_PATH_D = 3,     
} RF_RADIO_PATH_E, *PRF_RADIO_PATH_E;

#define RF_PATH_MAX     2


typedef enum _WIRELESS_MODE {
  WIRELESS_MODE_UNKNOWN = 0x00,
  WIRELESS_MODE_A = 0x01,
  WIRELESS_MODE_B = 0x02,
  WIRELESS_MODE_G = 0x04,
  WIRELESS_MODE_AUTO = 0x08,
  WIRELESS_MODE_N_24G = 0x10,
  WIRELESS_MODE_N_5G = 0x20
} WIRELESS_MODE;


#if(TX_POWER_FOR_5G_BAND == 1)
#define CHANNEL_MAX_NUMBER    14+24+21 
#define CHANNEL_GROUP_MAX   3+9
#define MAX_PG_GROUP 13
#else
#define CHANNEL_MAX_NUMBER    14 
#define CHANNEL_GROUP_MAX   3
#define MAX_PG_GROUP 7
#endif
#define CHANNEL_GROUP_MAX_2G    3
#define CHANNEL_GROUP_IDX_5GL   3
#define CHANNEL_GROUP_IDX_5GM   6
#define CHANNEL_GROUP_IDX_5GH   9
#define CHANNEL_GROUP_MAX_5G    9
#define CHANNEL_MAX_NUMBER_2G   14

#if (RTL8192D_DUAL_MAC_MODE_SWITCH == 1)
typedef enum _BaseBand_Config_Type {
  BaseBand_Config_PHY_REG = 0,
  BaseBand_Config_AGC_TAB = 1,
  BaseBand_Config_AGC_TAB_2G = 2,
  BaseBand_Config_AGC_TAB_5G = 3,
} BaseBand_Config_Type, *PBaseBand_Config_Type;
#else
typedef enum _BaseBand_Config_Type {
  BaseBand_Config_PHY_REG = 0,     
  BaseBand_Config_AGC_TAB = 1,     
} BaseBand_Config_Type, *PBaseBand_Config_Type;
#endif


typedef enum _MACPHY_MODE_8192D {
  SINGLEMAC_SINGLEPHY, 
  DUALMAC_DUALPHY,   
  DUALMAC_SINGLEPHY, 
} MACPHY_MODE_8192D, *PMACPHY_MODE_8192D;

typedef enum _MACPHY_MODE_CHANGE_ACTION {
  DMDP2DMSP = 0,
  DMSP2DMDP = 1,
  DMDP2SMSP = 2,
  SMSP2DMDP = 3,
  DMSP2SMSP = 4,
  SMSP2DMSP = 5,
  MAXACTION
} MACPHY_MODE_CHANGE_ACTION, *PMACPHY_MODE_CHANGE_ACTION;

typedef enum _BAND_TYPE {
  BAND_ON_2_4G  = 1,
  BAND_ON_5G  = 2,
  BAND_ON_BOTH,
  BANDMAX
} BAND_TYPE, *PBAND_TYPE;

typedef enum _PHY_Rate_Tx_Power_Offset_Area {
  RA_OFFSET_LEGACY_OFDM1,
  RA_OFFSET_LEGACY_OFDM2,
  RA_OFFSET_HT_OFDM1,
  RA_OFFSET_HT_OFDM2,
  RA_OFFSET_HT_OFDM3,
  RA_OFFSET_HT_OFDM4,
  RA_OFFSET_HT_CCK,
} RA_OFFSET_AREA, *PRA_OFFSET_AREA;


/* BB/RF related */
typedef enum _RF_TYPE_8190P {
  RF_TYPE_MIN, 
  RF_8225 = 1,   
  RF_8256 = 2,   
  RF_8258 = 3,   
  RF_6052 = 4, 
  RF_PSEUDO_11N = 5,
} RF_TYPE_8190P_E, *PRF_TYPE_8190P_E;



typedef struct _BB_REGISTER_DEFINITION {
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
  
} BB_REGISTER_DEFINITION_T, *PBB_REGISTER_DEFINITION_T;


typedef struct _R_ANTENNA_SELECT_OFDM {
  u32     r_tx_antenna: 4;
  u32     r_ant_l: 4;
  u32     r_ant_non_ht: 4;
  u32     r_ant_ht1: 4;
  u32     r_ant_ht2: 4;
  u32     r_ant_ht_s1: 4;
  u32     r_ant_non_ht_s1: 4;
  u32     OFDM_TXSC: 2;
  u32     Reserved: 2;
} R_ANTENNA_SELECT_OFDM;

typedef struct _R_ANTENNA_SELECT_CCK {
  u8      r_cckrx_enable_2: 2;
  u8      r_cckrx_enable: 2;
  u8      r_ccktx_enable: 4;
} R_ANTENNA_SELECT_CCK;

/*------------------------------Define structure----------------------------*/


/*------------------------Export global variable----------------------------*/
/*------------------------Export global variable----------------------------*/


/*------------------------Export Marco Definition---------------------------*/

/*--------------------------Exported Function prototype---------------------*/
void  rtl8192d_PHY_SetBBReg1Byte ( IN  PADAPTER  Adapter,
                                   IN  u32   RegAddr,
                                   IN  u32   BitMask,
                                   IN  u32   Data  );
u32 rtl8192d_PHY_QueryBBReg (  IN  PADAPTER  Adapter,
                               IN  u32   RegAddr,
                               IN  u32   BitMask );
void  rtl8192d_PHY_SetBBReg (  IN  PADAPTER  Adapter,
                               IN  u32   RegAddr,
                               IN  u32   BitMask,
                               IN  u32   Data  );
u32 rtl8192d_PHY_QueryRFReg (  IN  PADAPTER      Adapter,
                               IN  RF_RADIO_PATH_E eRFPath,
                               IN  u32       RegAddr,
                               IN  u32       BitMask );
void  rtl8192d_PHY_SetRFReg (  IN  PADAPTER      Adapter,
                               IN  RF_RADIO_PATH_E eRFPath,
                               IN  u32       RegAddr,
                               IN  u32       BitMask,
                               IN  u32       Data  );

/* MAC/BB/RF HAL config */
extern  int PHY_MACConfig8192D ( IN  PADAPTER  Adapter );
extern  int PHY_BBConfig8192D (  IN  PADAPTER  Adapter );
extern  int PHY_RFConfig8192D (  IN  PADAPTER  Adapter );
/* RF config */
int rtl8192d_PHY_ConfigRFWithParaFile (  IN  PADAPTER  Adapter,
    IN  u8  * pFileName,
    IN  RF_RADIO_PATH_E eRFPath);
int rtl8192d_PHY_ConfigRFWithHeaderFile (  IN  PADAPTER      Adapter,
    IN  RF_CONTENT      Content,
    IN  RF_RADIO_PATH_E eRFPath);
/* BB/RF readback check for making sure init OK */
int rtl8192d_PHY_CheckBBAndRFOK (  IN  PADAPTER      Adapter,
                                   IN  HW90_BLOCK_E    CheckBlock,
                                   IN  RF_RADIO_PATH_E eRFPath   );
/* Read initi reg value for tx power setting. */
void  rtl8192d_PHY_GetHWRegOriginalValue ( IN  PADAPTER    Adapter );


void  PHY_GetTxPowerLevel8192D ( IN  PADAPTER    Adapter,
                                 OUT u32    *    powerlevel  );
void  PHY_SetTxPowerLevel8192D ( IN  PADAPTER    Adapter,
                                 IN  u8      channel );
BOOLEAN PHY_UpdateTxPowerDbm8192D (  IN  PADAPTER  Adapter,
                                     IN  int   powerInDbm  );

VOID
PHY_ScanOperationBackup8192D (IN PADAPTER  Adapter,
                              IN  u8    Operation );

void  PHY_SetBWMode8192D ( IN  PADAPTER      pAdapter,
                           IN  HT_CHANNEL_WIDTH  ChnlWidth,
                           IN  unsigned char Offset  );


extern  void FillA2Entry8192C (    IN  PADAPTER      Adapter,
                                   IN  u8        index,
                                   IN  u8    *   val);


void  PHY_SwChnl8192D (  IN  PADAPTER    pAdapter,
                         IN  u8      channel );
void  PHY_SwChnlPhy8192D ( IN  PADAPTER    pAdapter,
                           IN  u8      channel );

extern void ChkFwCmdIoDone ( IN  PADAPTER  Adapter);


void  PHY_SetMonitorMode8192D (IN  PADAPTER  pAdapter,
                               IN  BOOLEAN   bEnableMonitorMode  );

BOOLEAN PHY_CheckIsLegalRfPath8192D (IN  PADAPTER  pAdapter,
                                     IN  u32   eRFPath );


void
rtl8192d_PHY_SetBeaconHwReg (  IN  PADAPTER    Adapter,
                               IN  u16     BeaconInterval  );


extern  VOID
PHY_SwitchEphyParameter (
  IN  PADAPTER      Adapter
);

extern  VOID
PHY_EnableHostClkReq (
  IN  PADAPTER      Adapter
);

BOOLEAN
SetAntennaConfig92C (
  IN  PADAPTER  Adapter,
  IN  u8    DefaultAnt
);

VOID
PHY_UpdateBBRFConfiguration8192D (
  IN PADAPTER Adapter,
  IN BOOLEAN bisBandSwitch
);

VOID PHY_ReadMacPhyMode92D (
  IN PADAPTER Adapter,
  IN BOOLEAN  AutoloadFail
);

VOID PHY_ConfigMacPhyMode92D (
  IN PADAPTER Adapter
);

VOID PHY_ConfigMacPhyModeInfo92D (
  IN PADAPTER Adapter
);

VOID PHY_ConfigMacCoexist_RFPage92D (
  IN PADAPTER Adapter
);

VOID
rtl8192d_PHY_InitRxSetting (
  IN  PADAPTER Adapter
);


VOID
rtl8192d_PHY_SetRFPathSwitch (IN PADAPTER  pAdapter, IN  BOOLEAN   bMain);

VOID
HalChangeCCKStatus8192D (
  IN  PADAPTER  Adapter,
  IN  BOOLEAN   bCCKDisable
);

VOID
PHY_InitPABias92D (IN  PADAPTER Adapter);

/*--------------------------Exported Function prototype---------------------*/

#define PHY_SetBBReg1Byte(Adapter, RegAddr, BitMask, Data) rtl8192d_PHY_SetBBReg1Byte((Adapter), (RegAddr), (BitMask), (Data))
#define PHY_QueryBBReg(Adapter, RegAddr, BitMask) rtl8192d_PHY_QueryBBReg((Adapter), (RegAddr), (BitMask))
#define PHY_SetBBReg(Adapter, RegAddr, BitMask, Data) rtl8192d_PHY_SetBBReg((Adapter), (RegAddr), (BitMask), (Data))
#define PHY_QueryRFReg(Adapter, eRFPath, RegAddr, BitMask) rtl8192d_PHY_QueryRFReg((Adapter), (eRFPath), (RegAddr), (BitMask))
#define PHY_SetRFReg(Adapter, eRFPath, RegAddr, BitMask, Data) rtl8192d_PHY_SetRFReg((Adapter), (eRFPath), (RegAddr), (BitMask), (Data))

#define PHY_SetMacReg PHY_SetBBReg
#define PHY_QueryMacReg PHY_QueryBBReg

#endif 

