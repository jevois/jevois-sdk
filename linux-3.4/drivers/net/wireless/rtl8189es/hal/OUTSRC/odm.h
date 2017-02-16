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


#ifndef __HALDMOUTSRC_H__
#define __HALDMOUTSRC_H__







#define TP_MODE   0
#define RSSI_MODE   1
#define TRAFFIC_LOW 0
#define TRAFFIC_HIGH  1


#define   DPK_DELTA_MAPPING_NUM 13
#define   index_mapping_HP_NUM  15



#define AFH_PSD   1
#define MODE_40M    0
#define PSD_TH2   3
#define PSD_CHMIN   20  
#define SIR_STEP_SIZE 3
#define   Smooth_Size_1   5
#define Smooth_TH_1 3
#define   Smooth_Size_2   10
#define Smooth_TH_2 4
#define   Smooth_Size_3   20
#define Smooth_TH_3 4
#define   Smooth_Step_Size 5
#define Adaptive_SIR  1
#if(RTL8723_FPGA_VERIFICATION == 1)
#define PSD_RESCAN    1
#else
#define PSD_RESCAN    4
#endif
#define PSD_SCAN_INTERVAL 700



#define   DM_DIG_HIGH_PWR_IGI_LOWER_BOUND 0x22
#define     DM_DIG_Gmode_HIGH_PWR_IGI_LOWER_BOUND 0x28
#define   DM_DIG_HIGH_PWR_THRESHOLD 0x3a

#define DM_DIG_FA_TH0_LPS       4
#define DM_DIG_FA_TH1_LPS       15
#define DM_DIG_FA_TH2_LPS       30
#define RSSI_OFFSET_DIG         0x05;

#define     ANTTESTALL    0x00   
#define   ANTTESTA    0x01   
#define   ANTTESTB    0x02   



#if(DM_ODM_SUPPORT_TYPE & (ODM_CE|ODM_MP))
typedef   struct rtl8192cd_priv {
  u1Byte    temp;
  
} rtl8192cd_priv, *prtl8192cd_priv;
#endif


#if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
typedef   struct _ADAPTER {
  u1Byte    temp;
  #ifdef AP_BUILD_WORKAROUND
  HAL_DATA_TYPE  *  temp2;
  prtl8192cd_priv   priv;
  #endif
} ADAPTER, *PADAPTER;
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_AP)

typedef   struct _WLAN_STA {
  u1Byte    temp;
} WLAN_STA, *PRT_WLAN_STA;

#endif

typedef struct _Dynamic_Initial_Gain_Threshold_
{
  u1Byte    Dig_Enable_Flag;
  u1Byte    Dig_Ext_Port_Stage;
  
  int       RssiLowThresh;
  int         RssiHighThresh;
  
  u4Byte    FALowThresh;
  u4Byte    FAHighThresh;
  
  u1Byte    CurSTAConnectState;
  u1Byte    PreSTAConnectState;
  u1Byte    CurMultiSTAConnectState;
  
  u1Byte    PreIGValue;
  u1Byte    CurIGValue;
  u1Byte    BackupIGValue;
  
  s1Byte    BackoffVal;
  s1Byte    BackoffVal_range_max;
  s1Byte    BackoffVal_range_min;
  u1Byte    rx_gain_range_max;
  u1Byte    rx_gain_range_min;
  u1Byte    Rssi_val_min;
  
  u1Byte    PreCCK_CCAThres;
  u1Byte    CurCCK_CCAThres;
  u1Byte    PreCCKPDState;
  u1Byte    CurCCKPDState;
  
  u1Byte    LargeFAHit;
  u1Byte    ForbiddenIGI;
  u4Byte    Recover_cnt;
  
  u1Byte    DIG_Dynamic_MIN_0;
  u1Byte    DIG_Dynamic_MIN_1;
  BOOLEAN   bMediaConnect_0;
  BOOLEAN   bMediaConnect_1;
  
  u4Byte    AntDiv_RSSI_max;
  u4Byte    RSSI_max;
} DIG_T, *pDIG_T;

typedef struct _Dynamic_Power_Saving_
{
  u1Byte    PreCCAState;
  u1Byte    CurCCAState;
  
  u1Byte    PreRFState;
  u1Byte    CurRFState;
  
  int       Rssi_val_min;
  
  u1Byte    initialize;
  u4Byte    Reg874, RegC70, Reg85C, RegA74;
  
} PS_T, *pPS_T;

typedef struct _FALSE_ALARM_STATISTICS {
  u4Byte  Cnt_Parity_Fail;
  u4Byte  Cnt_Rate_Illegal;
  u4Byte  Cnt_Crc8_fail;
  u4Byte  Cnt_Mcs_fail;
  u4Byte  Cnt_Ofdm_fail;
  u4Byte  Cnt_Cck_fail;
  u4Byte  Cnt_all;
  u4Byte  Cnt_Fast_Fsync;
  u4Byte  Cnt_SB_Search_fail;
  u4Byte  Cnt_OFDM_CCA;
  u4Byte  Cnt_CCK_CCA;
  u4Byte  Cnt_CCA_all;
  u4Byte  Cnt_BW_USC;
  u4Byte  Cnt_BW_LSC;
} FALSE_ALARM_STATISTICS, *PFALSE_ALARM_STATISTICS;

typedef struct _Dynamic_Primary_CCA {
  u1Byte    PriCCA_flag;
  u1Byte    intf_flag;
  u1Byte    intf_type;
  u1Byte    DupRTS_flag;
  u1Byte    Monitor_flag;
} Pri_CCA_T, *pPri_CCA_T;

typedef struct _RX_High_Power_
{
  u1Byte    RXHP_flag;
  u1Byte    PSD_func_trigger;
  u1Byte    PSD_bitmap_RXHP[80];
  u1Byte    Pre_IGI;
  u1Byte    Cur_IGI;
  u1Byte    Pre_pw_th;
  u1Byte    Cur_pw_th;
  BOOLEAN   First_time_enter;
  BOOLEAN   RXHP_enable;
  u1Byte    TP_Mode;
  RT_TIMER  PSDTimer;
  #if (DM_ODM_SUPPORT_TYPE == ODM_MP)
  #if USE_WORKITEM
  RT_WORK_ITEM    PSDTimeWorkitem;
  #endif
  #endif
  
} RXHP_T, *pRXHP_T;

#if(DM_ODM_SUPPORT_TYPE & (ODM_CE))
#define ASSOCIATE_ENTRY_NUM         32
#define ODM_ASSOCIATE_ENTRY_NUM       ASSOCIATE_ENTRY_NUM

#elif(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
#define ASSOCIATE_ENTRY_NUM         NUM_STAT
#define ODM_ASSOCIATE_ENTRY_NUM       ASSOCIATE_ENTRY_NUM+1

#else
#define ODM_ASSOCIATE_ENTRY_NUM       ASSOCIATE_ENTRY_NUM+1// Default port only one
#endif

#define SWAW_STEP_PEAK    0
#define SWAW_STEP_DETERMINE 1

#define TP_MODE   0
#define RSSI_MODE   1
#define TRAFFIC_LOW 0
#define TRAFFIC_HIGH  1

typedef struct _SW_Antenna_Switch_
{
  u1Byte    try_flag;
  s4Byte    PreRSSI;
  u1Byte    CurAntenna;
  u1Byte    PreAntenna;
  u1Byte    RSSI_Trying;
  u1Byte    TestMode;
  u1Byte    bTriggerAntennaSwitch;
  u1Byte    SelectAntennaMap;
  u1Byte    RSSI_target;
  
  u1Byte    SWAS_NoLink_State;
  u4Byte    SWAS_NoLink_BK_Reg860;
  BOOLEAN   ANTA_ON; 
  BOOLEAN   ANTB_ON; 
  
  s4Byte    RSSI_sum_A;
  s4Byte    RSSI_sum_B;
  s4Byte    RSSI_cnt_A;
  s4Byte    RSSI_cnt_B;
  
  u8Byte    lastTxOkCnt;
  u8Byte    lastRxOkCnt;
  u8Byte    TXByteCnt_A;
  u8Byte    TXByteCnt_B;
  u8Byte    RXByteCnt_A;
  u8Byte    RXByteCnt_B;
  u1Byte    TrafficLoad;
  RT_TIMER  SwAntennaSwitchTimer;
  #if (DM_ODM_SUPPORT_TYPE == ODM_MP)
  #if USE_WORKITEM
  RT_WORK_ITEM      SwAntennaSwitchWorkitem;
  #endif
  #endif
  /* CE Platform use
  #ifdef CONFIG_SW_ANTENNA_DIVERSITY
    _timer SwAntennaSwitchTimer;
    u8Byte lastTxOkCnt;
    u8Byte lastRxOkCnt;
    u8Byte TXByteCnt_A;
    u8Byte TXByteCnt_B;
    u8Byte RXByteCnt_A;
    u8Byte RXByteCnt_B;
    u1Byte DoubleComfirm;
    u1Byte TrafficLoad;
  
  
  #endif
  */
  #ifdef CONFIG_HW_ANTENNA_DIVERSITY
  u4Byte    CCK_Ant1_Cnt[ASSOCIATE_ENTRY_NUM];
  u4Byte    CCK_Ant2_Cnt[ASSOCIATE_ENTRY_NUM];
  u4Byte    OFDM_Ant1_Cnt[ASSOCIATE_ENTRY_NUM];
  u4Byte    OFDM_Ant2_Cnt[ASSOCIATE_ENTRY_NUM];
  u4Byte    RSSI_Ant1_Sum[ASSOCIATE_ENTRY_NUM];
  u4Byte    RSSI_Ant2_Sum[ASSOCIATE_ENTRY_NUM];
  u1Byte    TxAnt[ASSOCIATE_ENTRY_NUM];
  u1Byte    TargetSTA;
  u1Byte    antsel;
  u1Byte    RxIdleAnt;
  
  #endif
  
} SWAT_T, *pSWAT_T;

typedef struct _EDCA_TURBO_
{
  BOOLEAN bCurrentTurboEDCA;
  BOOLEAN bIsCurRDLState;
  #if(DM_ODM_SUPPORT_TYPE == ODM_CE )
  u4Byte  prv_traffic_idx;
  #endif
  
} EDCA_T, *pEDCA_T;

typedef struct _ODM_RATE_ADAPTIVE
{
  u1Byte        Type;      
  u1Byte        HighRSSIThresh;  
  u1Byte        LowRSSIThresh;   
  u1Byte        RATRState;     
  u4Byte        LastRATR;    
  
} ODM_RATE_ADAPTIVE, *PODM_RATE_ADAPTIVE;


#if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))


#ifdef ADSL_AP_BUILD_WORKAROUND
#define MAX_TOLERANCE     5
#define IQK_DELAY_TIME      1  
#endif

typedef enum _HT_IOT_PEER
{
  HT_IOT_PEER_UNKNOWN       = 0,
  HT_IOT_PEER_REALTEK       = 1,
  HT_IOT_PEER_REALTEK_92SE    = 2,
  HT_IOT_PEER_BROADCOM    = 3,
  HT_IOT_PEER_RALINK      = 4,
  HT_IOT_PEER_ATHEROS       = 5,
  HT_IOT_PEER_CISCO         = 6,
  HT_IOT_PEER_MERU        = 7,
  HT_IOT_PEER_MARVELL       = 8,
  HT_IOT_PEER_REALTEK_SOFTAP  = 9,// peer is RealTek SOFT_AP, by Bohn, 2009.12.17
  HT_IOT_PEER_SELF_SOFTAP     = 10,
  HT_IOT_PEER_AIRGO         = 11,
  HT_IOT_PEER_INTEL         = 12,
  HT_IOT_PEER_RTK_APCLIENT    = 13,
  HT_IOT_PEER_REALTEK_81XX    = 14,
  HT_IOT_PEER_REALTEK_WOW     = 15,
  HT_IOT_PEER_MAX         = 16
} HT_IOT_PEER_E, *PHTIOT_PEER_E;
#endif//#if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))



#define IQK_MAC_REG_NUM   4
#define IQK_ADDA_REG_NUM    16
#define IQK_BB_REG_NUM_MAX  10
#if (RTL8192D_SUPPORT==1)
#define IQK_BB_REG_NUM    10
#else
#define IQK_BB_REG_NUM    9
#endif
#define HP_THERMAL_NUM    8

#define AVG_THERMAL_NUM   8
#define IQK_Matrix_REG_NUM  8
#define IQK_Matrix_Settings_NUM 1+24+21

#define   DM_Type_ByFW      0
#define   DM_Type_ByDriver    1

#define MAX_PATH_NUM_92CS   2

typedef struct _ODM_Phy_Status_Info_
{
  u1Byte    RxPWDBAll;
  u1Byte    SignalQuality;  
  u1Byte    RxMIMOSignalQuality[MAX_PATH_NUM_92CS];
  u1Byte    RxMIMOSignalStrength[MAX_PATH_NUM_92CS];// in 0~100 index
  #if (DM_ODM_SUPPORT_TYPE &  (ODM_MP|ODM_CE))
  s1Byte    RxPower;
  s1Byte    RecvSignalPower;// Real power in dBm for this packet, no beautification and aggregation. Keep this raw info to be used for the other procedures.
  u1Byte    BTRxRSSIPercentage;
  u1Byte    SignalStrength;
  u1Byte    RxPwr[MAX_PATH_NUM_92CS];//per-path's pwdb
  #endif
  u1Byte    RxSNR[MAX_PATH_NUM_92CS];//per-path's SNR
} ODM_PHY_INFO_T, *PODM_PHY_INFO_T;


typedef struct _ODM_Phy_Dbg_Info_
{
  s1Byte    RxSNRdB[MAX_PATH_NUM_92CS];
  u8Byte    NumQryPhyStatus;
  u8Byte    NumQryPhyStatusCCK;
  u8Byte    NumQryPhyStatusOFDM;
  s4Byte    RxEVM[MAX_PATH_NUM_92CS];
  
} ODM_PHY_DBG_INFO_T;

typedef struct _ODM_Per_Pkt_Info_
{
  u1Byte    Rate;
  u1Byte    StationID;
  BOOLEAN   bPacketMatchBSSID;
  BOOLEAN   bPacketToSelf;
  BOOLEAN   bPacketBeacon;
} ODM_PACKET_INFO_T, *PODM_PACKET_INFO_T;

typedef struct _ODM_Mac_Status_Info_
{
  u1Byte  test;
  
} ODM_MAC_INFO;


typedef enum tag_Dynamic_ODM_Support_Ability_Type
{
  ODM_DIG       = 0x00000001,
  ODM_HIGH_POWER    = 0x00000002,
  ODM_CCK_CCA_TH    = 0x00000004,
  ODM_FA_STATISTICS   = 0x00000008,
  ODM_RAMASK      = 0x00000010,
  ODM_RSSI_MONITOR    = 0x00000020,
  ODM_SW_ANTDIV   = 0x00000040,
  ODM_HW_ANTDIV   = 0x00000080,
  ODM_BB_PWRSV      = 0x00000100,
  ODM_2TPATHDIV     = 0x00000200,
  ODM_1TPATHDIV     = 0x00000400,
  ODM_PSD2AFH     = 0x00000800
} ODM_Ability_E;

#if 1
typedef   struct _ODM_STA_INFO {
  BOOLEAN   bUsed;       
  u1Byte    IOTPeer;     
  
  u1Byte    RSSI_Path[4];  
  u1Byte    RSSI_Ave;
  u1Byte    RXEVM[4];
  u1Byte    RXSNR[4];
  
  #if 0
  u1Byte    ANTSEL_A;    
  u1Byte    ANTSEL_B;    
  u1Byte    ANTSEL_C;    
  u1Byte    ANTSEL_D;    
  u1Byte    TX_ANTL;     
  u1Byte    TX_ANT_HT;     
  u1Byte    TX_ANT_CCK;    
  u1Byte    TXAGC_A;     
  u1Byte    TXAGC_B;     
  u1Byte    TXPWR_OFFSET;  
  u1Byte    TX_ANT;      
  #endif
  
  #if 0
  u1Byte    rate_id;
  u1Byte    rate_SGI;
  u1Byte    rssi_sta_ra; 
  u1Byte    SGI_enable;
  u1Byte    Decision_rate;
  u1Byte    Pre_rate;
  u1Byte    Active;
  
  u2Byte    RTY[4];      
  u2Byte    TOTAL;       
  u2Byte    DROP;      
  #endif
  
} ODM_STA_INFO_T, *PODM_STA_INFO_T;
#endif

typedef enum _ODM_Common_Info_Definition
{

  
  ODM_CMNINFO_PLATFORM = 0,
  ODM_CMNINFO_ABILITY,         
  ODM_CMNINFO_INTERFACE,       
  ODM_CMNINFO_MP_TEST_CHIP,
  ODM_CMNINFO_IC_TYPE,         
  ODM_CMNINFO_CUT_VER,         
  ODM_CMNINFO_FAB_VER,         
  ODM_CMNINFO_RF_TYPE,         
  ODM_CMNINFO_BOARD_TYPE,      
  ODM_CMNINFO_EXT_LNA,         
  ODM_CMNINFO_EXT_PA,
  ODM_CMNINFO_EXT_TRSW,
  ODM_CMNINFO_PATCH_ID,      
  ODM_CMNINFO_BINHCT_TEST,
  ODM_CMNINFO_BWIFI_TEST,
  ODM_CMNINFO_SMART_CONCURRENT,
  
  
  ODM_CMNINFO_MAC_PHY_MODE,    
  ODM_CMNINFO_TX_UNI,
  ODM_CMNINFO_RX_UNI,
  ODM_CMNINFO_WM_MODE,       
  ODM_CMNINFO_BAND,        
  ODM_CMNINFO_SEC_CHNL_OFFSET,   
  ODM_CMNINFO_SEC_MODE,      
  ODM_CMNINFO_BW,          
  ODM_CMNINFO_CHNL,
  
  ODM_CMNINFO_DMSP_GET_VALUE,
  ODM_CMNINFO_BUDDY_ADAPTOR,
  ODM_CMNINFO_DMSP_IS_MASTER,
  ODM_CMNINFO_SCAN,
  ODM_CMNINFO_POWER_SAVING,
  ODM_CMNINFO_ONE_PATH_CCA,    
  ODM_CMNINFO_DRV_STOP,
  ODM_CMNINFO_PNP_IN,
  ODM_CMNINFO_INIT_ON,
  ODM_CMNINFO_ANT_TEST,
  ODM_CMNINFO_NET_CLOSED,
  ODM_CMNINFO_MP_MODE,

  ODM_CMNINFO_WIFI_DIRECT,
  ODM_CMNINFO_WIFI_DISPLAY,
  ODM_CMNINFO_LINK,
  ODM_CMNINFO_RSSI_MIN,
  ODM_CMNINFO_DBG_COMP,      
  ODM_CMNINFO_DBG_LEVEL,       
  ODM_CMNINFO_RA_THRESHOLD_HIGH,   
  ODM_CMNINFO_RA_THRESHOLD_LOW,  
  ODM_CMNINFO_RF_ANTENNA_TYPE,   
  ODM_CMNINFO_BT_DISABLED,
  ODM_CMNINFO_BT_OPERATION,
  ODM_CMNINFO_BT_DIG,
  ODM_CMNINFO_BT_BUSY,         
  ODM_CMNINFO_BT_DISABLE_EDCA,

  ODM_CMNINFO_STA_STATUS,
  ODM_CMNINFO_PHY_STATUS,
  ODM_CMNINFO_MAC_STATUS,
  
  ODM_CMNINFO_MAX,
  
  
} ODM_CMNINFO_E;

typedef enum _ODM_Support_Ability_Definition
{
  ODM_BB_DIG          = BIT0,
  ODM_BB_RA_MASK        = BIT1,
  ODM_BB_DYNAMIC_TXPWR    = BIT2,
  ODM_BB_FA_CNT         = BIT3,
  ODM_BB_RSSI_MONITOR     = BIT4,
  ODM_BB_CCK_PD         = BIT5,
  ODM_BB_ANT_DIV        = BIT6,
  ODM_BB_PWR_SAVE       = BIT7,
  ODM_BB_PWR_TRAIN        = BIT8,
  ODM_BB_RATE_ADAPTIVE      = BIT9,
  ODM_BB_PATH_DIV       = BIT10,
  ODM_BB_PSD          = BIT11,
  ODM_BB_RXHP         = BIT12,
  
  ODM_MAC_EDCA_TURBO      = BIT16,
  ODM_MAC_EARLY_MODE      = BIT17,
  
  ODM_RF_TX_PWR_TRACK     = BIT24,
  ODM_RF_RX_GAIN_TRACK      = BIT25,
  ODM_RF_CALIBRATION        = BIT26,
  
} ODM_ABILITY_E;

typedef enum tag_ODM_Support_Interface_Definition
{
  ODM_ITRF_PCIE   = 0x1,
  ODM_ITRF_USB  = 0x2,
  ODM_ITRF_SDIO   = 0x4,
  ODM_ITRF_ALL  = 0x7,
} ODM_INTERFACE_E;

typedef enum tag_ODM_Support_IC_Type_Definition
{
  ODM_RTL8192S  = BIT0,
  ODM_RTL8192C  = BIT1,
  ODM_RTL8192D  = BIT2,
  ODM_RTL8723A  = BIT3,
  ODM_RTL8188E  = BIT4,
  ODM_RTL8812   = BIT5,
  ODM_RTL8821   = BIT6,
} ODM_IC_TYPE_E;

#define ODM_IC_11N_SERIES   (ODM_RTL8192S|ODM_RTL8192C|ODM_RTL8192D|ODM_RTL8723A|ODM_RTL8188E)
#define ODM_IC_11AC_SERIES    (ODM_RTL8812)

typedef enum tag_ODM_Cut_Version_Definition
{
  ODM_CUT_A     = 1,
  ODM_CUT_B     = 2,
  ODM_CUT_C     = 3,
  ODM_CUT_D     = 4,
  ODM_CUT_E     = 5,
  ODM_CUT_F     = 6,
  ODM_CUT_TEST  = 7,
} ODM_CUT_VERSION_E;

typedef enum tag_ODM_Fab_Version_Definition
{
  ODM_TSMC  = 0,
  ODM_UMC   = 1,
} ODM_FAB_E;

typedef enum tag_ODM_RF_Path_Bit_Definition
{
  ODM_RF_TX_A   = BIT0,
  ODM_RF_TX_B   = BIT1,
  ODM_RF_TX_C = BIT2,
  ODM_RF_TX_D = BIT3,
  ODM_RF_RX_A = BIT4,
  ODM_RF_RX_B = BIT5,
  ODM_RF_RX_C = BIT6,
  ODM_RF_RX_D = BIT7,
} ODM_RF_PATH_E;


typedef enum tag_ODM_RF_Type_Definition
{
  ODM_1T1R  = 0,
  ODM_1T2R  = 1,
  ODM_2T2R  = 2,
  ODM_2T3R  = 3,
  ODM_2T4R  = 4,
  ODM_3T3R  = 5,
  ODM_3T4R  = 6,
  ODM_4T4R  = 7,
} ODM_RF_TYPE_E;



typedef enum tag_ODM_MAC_PHY_Mode_Definition
{
  ODM_SMSP  = 0,
  ODM_DMSP  = 1,
  ODM_DMDP  = 2,
} ODM_MAC_PHY_MODE_E;


typedef enum tag_BT_Coexist_Definition
{
  ODM_BT_BUSY     = 1,
  ODM_BT_ON       = 2,
  ODM_BT_OFF    = 3,
  ODM_BT_NONE     = 4,
} ODM_BT_COEXIST_E;

typedef enum tag_Operation_Mode_Definition
{
  ODM_NO_LINK     = BIT0,
  ODM_LINK      = BIT1,
  ODM_SCAN      = BIT2,
  ODM_POWERSAVE   = BIT3,
  ODM_AP_MODE     = BIT4,
  ODM_CLIENT_MODE = BIT5,
  ODM_AD_HOC    = BIT6,
  ODM_WIFI_DIRECT = BIT7,
  ODM_WIFI_DISPLAY  = BIT8,
} ODM_OPERATION_MODE_E;

typedef enum tag_Wireless_Mode_Definition
{
  ODM_WM_UNKNOW = 0x0,
  ODM_WM_B      = BIT0,
  ODM_WM_G      = BIT1,
  ODM_WM_A      = BIT2,
  ODM_WM_N24G   = BIT3,
  ODM_WM_N5G    = BIT4,
  ODM_WM_AUTO   = BIT5,
  ODM_WM_AC   = BIT6,
} ODM_WIRELESS_MODE_E;

typedef enum tag_Band_Type_Definition
{
  ODM_BAND_2_4G   = BIT0,
  ODM_BAND_5G     = BIT1,
  
} ODM_BAND_TYPE_E;

typedef enum tag_Secondary_Channel_Offset_Definition
{
  ODM_DONT_CARE   = 0,
  ODM_BELOW     = 1,
  ODM_ABOVE       = 2
} ODM_SEC_CHNL_OFFSET_E;

typedef enum tag_Security_Definition
{
  ODM_SEC_OPEN      = 0,
  ODM_SEC_WEP40     = 1,
  ODM_SEC_TKIP      = 2,
  ODM_SEC_RESERVE     = 3,
  ODM_SEC_AESCCMP     = 4,
  ODM_SEC_WEP104    = 5,
  ODM_WEP_WPA_MIXED    = 6,
  ODM_SEC_SMS4      = 7,
} ODM_SECURITY_E;

typedef enum tag_Bandwidth_Definition
{
  ODM_BW20M     = 0,
  ODM_BW40M     = 1,
  ODM_BW80M     = 2,
  ODM_BW160M    = 3,
  ODM_BW10M     = 4,
} ODM_BW_E;


typedef enum tag_Board_Definition
{
  ODM_BOARD_NORMAL  = 0,
  ODM_BOARD_HIGHPWR = 1,
  ODM_BOARD_MINICARD  = 2,
  ODM_BOARD_SLIM    = 3,
  ODM_BOARD_COMBO   = 4,
  
} ODM_BOARD_TYPE_E;

typedef enum tag_CCA_Path
{
  ODM_CCA_2R      = 0,
  ODM_CCA_1R_A      = 1,
  ODM_CCA_1R_B      = 2,
} ODM_CCA_PATH_E;


typedef struct _ODM_RA_Info_
{
  u1Byte RateID;
  u4Byte RateMask;
  u4Byte RAUseRate;
  u1Byte RateSGI;
  u1Byte RssiStaRA;
  u1Byte PreRssiStaRA;
  u1Byte SGIEnable;
  u1Byte DecisionRate;
  u1Byte PreRate;
  u1Byte HighestRate;
  u1Byte LowestRate;
  u4Byte NscUp;
  u4Byte NscDown;
  u2Byte RTY[5];
  u4Byte TOTAL;
  u2Byte DROP;
  u1Byte Active;
  u2Byte RptTime;
  u1Byte RAWaitingCounter;
  u1Byte RAPendingCounter;
  #if 1
  u1Byte PTActive; 
  u1Byte PTTryState; 
  u1Byte PTStage; 
  u1Byte PTStopCount;
  u1Byte PTPreRate; 
  u1Byte PTPreRssi;
  u1Byte PTModeSS; 
  u1Byte RAstage; 
  u1Byte PTSmoothFactor;
  #endif
} ODM_RA_INFO_T, *PODM_RA_INFO_T;

typedef struct _IQK_MATRIX_REGS_SETTING {
  BOOLEAN   bIQKDone;
  s4Byte    Value[1][IQK_Matrix_REG_NUM];
} IQK_MATRIX_REGS_SETTING, *PIQK_MATRIX_REGS_SETTING;

#if (DM_ODM_SUPPORT_TYPE & ODM_MP)
typedef struct _PathDiv_Parameter_define_
{
  u4Byte org_5g_RegE30;
  u4Byte org_5g_RegC14;
  u4Byte org_5g_RegCA0;
  u4Byte swt_5g_RegE30;
  u4Byte swt_5g_RegC14;
  u4Byte swt_5g_RegCA0;
  u4Byte org_2g_RegC80;
  u4Byte org_2g_RegC4C;
  u4Byte org_2g_RegC94;
  u4Byte org_2g_RegC14;
  u4Byte org_2g_RegCA0;
  
  u4Byte swt_2g_RegC80;
  u4Byte swt_2g_RegC4C;
  u4Byte swt_2g_RegC94;
  u4Byte swt_2g_RegC14;
  u4Byte swt_2g_RegCA0;
} PATHDIV_PARA, *pPATHDIV_PARA;
#endif


typedef struct ODM_RF_Calibration_Structure
{
  
  u4Byte  RegA24;
  s4Byte  RegE94;
  s4Byte  RegE9C;
  s4Byte  RegEB4;
  s4Byte  RegEBC;
  
  u1Byte    TXPowercount;
  BOOLEAN bTXPowerTrackingInit;
  BOOLEAN bTXPowerTracking;
  u1Byte    TxPowerTrackControl;
  u1Byte    TM_Trigger;
  u1Byte    InternalPA5G[2]; 
  
  u1Byte    ThermalMeter[2];   
  u1Byte    ThermalValue;
  u1Byte    ThermalValue_LCK;
  u1Byte    ThermalValue_IQK;
  u1Byte  ThermalValue_DPK;
  u1Byte  ThermalValue_AVG[AVG_THERMAL_NUM];
  u1Byte  ThermalValue_AVG_index;
  u1Byte  ThermalValue_RxGain;
  u1Byte  ThermalValue_Crystal;
  u1Byte  ThermalValue_DPKstore;
  u1Byte  ThermalValue_DPKtrack;
  BOOLEAN TxPowerTrackingInProgress;
  BOOLEAN bDPKenable;
  
  BOOLEAN bReloadtxpowerindex;
  u1Byte  bRfPiEnable;
  u4Byte  TXPowerTrackingCallbackCnt;
  
  u1Byte  bCCKinCH14;
  u1Byte  CCK_index;
  u1Byte  OFDM_index[2];
  BOOLEAN bDoneTxpower;
  
  u1Byte  ThermalValue_HP[HP_THERMAL_NUM];
  u1Byte  ThermalValue_HP_index;
  IQK_MATRIX_REGS_SETTING IQKMatrixRegSetting[IQK_Matrix_Settings_NUM];
  
  u1Byte  Delta_IQK;
  u1Byte  Delta_LCK;
  
  u4Byte  RegC04;
  u4Byte  Reg874;
  u4Byte  RegC08;
  u4Byte  RegB68;
  u4Byte  RegB6C;
  u4Byte  Reg870;
  u4Byte  Reg860;
  u4Byte  Reg864;
  
  BOOLEAN bIQKInitialized;
  BOOLEAN bLCKInProgress;
  BOOLEAN bAntennaDetected;
  u4Byte  ADDA_backup[IQK_ADDA_REG_NUM];
  u4Byte  IQK_MAC_backup[IQK_MAC_REG_NUM];
  u4Byte  IQK_BB_backup_recover[9];
  u4Byte  IQK_BB_backup[IQK_BB_REG_NUM];
  
  u4Byte  APKoutput[2][2];
  u1Byte  bAPKdone;
  u1Byte  bAPKThermalMeterIgnore;
  u1Byte  bDPdone;
  u1Byte  bDPPathAOK;
  u1Byte  bDPPathBOK;
} ODM_RF_CAL_T, *PODM_RF_CAL_T;

typedef struct _FAST_ANTENNA_TRAINNING_
{
  u1Byte  Bssid[6];
  u1Byte  antsel_rx_keep_0;
  u1Byte  antsel_rx_keep_1;
  u1Byte  antsel_rx_keep_2;
  u4Byte  antSumRSSI[7];
  u4Byte  antRSSIcnt[7];
  u4Byte  antAveRSSI[7];
  u1Byte  FAT_State;
  u4Byte  TrainIdx;
  u1Byte  antsel_a[ODM_ASSOCIATE_ENTRY_NUM];
  u1Byte  antsel_b[ODM_ASSOCIATE_ENTRY_NUM];
  u1Byte  antsel_c[ODM_ASSOCIATE_ENTRY_NUM];
  u4Byte  MainAnt_Sum[ODM_ASSOCIATE_ENTRY_NUM];
  u4Byte  AuxAnt_Sum[ODM_ASSOCIATE_ENTRY_NUM];
  u4Byte  MainAnt_Cnt[ODM_ASSOCIATE_ENTRY_NUM];
  u4Byte  AuxAnt_Cnt[ODM_ASSOCIATE_ENTRY_NUM];
  u1Byte  RxIdleAnt;
  BOOLEAN   bBecomeLinked;
  
} FAT_T, *pFAT_T;

typedef enum _FAT_STATE
{
  FAT_NORMAL_STATE      = 0,
  FAT_TRAINING_STATE    = 1,
} FAT_STATE_E, *PFAT_STATE_E;

typedef enum _ANT_DIV_TYPE
{
  NO_ANTDIV         = 0xFF,
  CG_TRX_HW_ANTDIV      = 0x01,
  CGCS_RX_HW_ANTDIV     = 0x02,
  FIXED_HW_ANTDIV             = 0x03,
  CG_TRX_SMART_ANTDIV   = 0x04,
  CGCS_RX_SW_ANTDIV   = 0x05,
  
} ANT_DIV_TYPE_E, *PANT_DIV_TYPE_E;













#if(DM_ODM_SUPPORT_TYPE & ODM_MP)
#if (RT_PLATFORM != PLATFORM_LINUX)
typedef
#endif
struct DM_Out_Source_Dynamic_Mechanism_Structure
#else// for AP,ADSL,CE Team
typedef  struct DM_Out_Source_Dynamic_Mechanism_Structure
#endif
{
  PADAPTER    Adapter;   
  prtl8192cd_priv priv;    
  BOOLEAN     odm_ready;
  
  #if(DM_ODM_SUPPORT_TYPE & (ODM_CE|ODM_MP))
  rtl8192cd_priv    fake_priv;
  #endif
  #if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
  ADAPTER     fake_adapter;
  #endif
  
  u8Byte      DebugComponents;
  u4Byte      DebugLevel;
  
  BOOLEAN     bCckHighPower;
  u1Byte      RFPathRxEnable;  
  u1Byte      ControlChannel;




  u1Byte      SupportPlatform;
  u4Byte      SupportAbility;
  u1Byte      SupportInterface;
  u4Byte      SupportICType;
  u1Byte      CutVersion;
  u1Byte      FabVersion;
  u1Byte      RFType;
  u1Byte      BoardType;
  u1Byte      ExtLNA;
  u1Byte      ExtPA;
  u1Byte      ExtTRSW;
  u1Byte      PatchID;
  BOOLEAN     bInHctTest;
  BOOLEAN     bWIFITest;
  
  BOOLEAN     bDualMacSmartConcurrent;
  u4Byte      BK_SupportAbility;
  u1Byte      AntDivType;


  u1Byte      u1Byte_temp;
  BOOLEAN     BOOLEAN_temp;
  PADAPTER    PADAPTER_temp;
  
  u1Byte   *   pMacPhyMode;
  u8Byte   *   pNumTxBytesUnicast;
  u8Byte   *   pNumRxBytesUnicast;
  u1Byte   *   pWirelessMode;
  u1Byte   *   pBandType;
  u1Byte   *   pSecChOffset;
  u1Byte   *   pSecurity;
  u1Byte   *   pBandWidth;
  u1Byte   *   pChannel; 
  
  BOOLEAN   *  pbGetValueFromOtherMac;
  PADAPTER  *  pBuddyAdapter;
  BOOLEAN   *  pbMasterOfDMSP;
  BOOLEAN   *  pbScanInProcess;
  BOOLEAN   *  pbPowerSaving;
  u1Byte   *   pOnePathCCA;
  u1Byte   *   pAntennaTest;
  BOOLEAN   *  pbNet_closed;
  BOOLEAN     bWIFI_Direct;
  BOOLEAN     bWIFI_Display;
  BOOLEAN     bLinked;
  u1Byte      RSSI_Min;
  u1Byte            InterfaceIndex;
  BOOLEAN           bIsMPChip;
  BOOLEAN     bOneEntryOnly;
  BOOLEAN     bBtDisabled;     
  BOOLEAN     bBtHsOperation;  
  u1Byte      btHsDigVal;    
  BOOLEAN     bBtDisableEdcaTurbo; 
  BOOLEAN     bBtBusy;       

  PSTA_INFO_T   pODM_StaInfo[ODM_ASSOCIATE_ENTRY_NUM];
  
  #if (RATE_ADAPTIVE_SUPPORT == 1)
  u2Byte      CurrminRptTime;
  ODM_RA_INFO_T   RAInfo[ODM_ASSOCIATE_ENTRY_NUM];
  #endif
  BOOLEAN       RaSupport88E;
  
  
  ODM_PHY_DBG_INFO_T   PhyDbgInfo;
  
  ODM_MAC_INFO  *  pMacInfo;
  
  
  
  FAT_T   DM_FatTable;
  DIG_T   DM_DigTable;
  PS_T    DM_PSTable;
  Pri_CCA_T DM_PriCCA;
  RXHP_T    DM_RXHP_Table;
  FALSE_ALARM_STATISTICS  FalseAlmCnt;
  FALSE_ALARM_STATISTICS  FlaseAlmCntBuddyAdapter;
  SWAT_T    DM_SWAT_Table;
  BOOLEAN   RSSI_test;
  
  #if (DM_ODM_SUPPORT_TYPE & ODM_MP)
  PATHDIV_PARA  pathIQK;
  #endif
  
  EDCA_T    DM_EDCA_Table;
  u4Byte    WMMEDCA_BE;
  
  
  BOOLEAN   *  pbDriverStopped;
  BOOLEAN   *  pbDriverIsGoingToPnpSetPowerSleep;
  BOOLEAN   *  pinit_adpt_in_progress;
  
  BOOLEAN     bUserAssignLevel;
  RT_TIMER    PSDTimer;
  u1Byte      RSSI_BT;     
  BOOLEAN     bPSDinProcess;
  BOOLEAN     bDMInitialGainEnable;
  
  u1Byte      bUseRAMask;
  
  ODM_RATE_ADAPTIVE RateAdaptive;
  
  
  ODM_RF_CAL_T  RFCalibrateInfo;
  
  u1Byte      BbSwingIdxOfdm;
  u1Byte      BbSwingIdxOfdmCurrent;
  u1Byte      BbSwingIdxOfdmBase;
  BOOLEAN     BbSwingFlagOfdm;
  u1Byte      BbSwingIdxCck;
  u1Byte      BbSwingIdxCckCurrent;
  u1Byte      BbSwingIdxCckBase;
  BOOLEAN     BbSwingFlagCck;
  u1Byte   *   mp_mode;
  
  RT_TIMER        PathDivSwitchTimer;
  RT_TIMER        CCKPathDiversityTimer;
  RT_TIMER  FastAntTrainingTimer;
  
  #if (DM_ODM_SUPPORT_TYPE == ODM_MP)
  #if USE_WORKITEM
  RT_WORK_ITEM      PathDivSwitchWorkitem;
  RT_WORK_ITEM      CCKPathDiversityWorkitem;
  RT_WORK_ITEM      FastAntTrainingWorkitem;
  #endif
  #endif
  
  #if(DM_ODM_SUPPORT_TYPE & ODM_MP)
  
  #if (RT_PLATFORM != PLATFORM_LINUX)
} DM_ODM_T, *PDM_ODM_T;  
  #else
};
  #endif

  #else// for AP,ADSL,CE Team
} DM_ODM_T, *PDM_ODM_T;  
  #endif



#if 1
#define ODM_RF_PATH_MAX 2
#else
#define ODM_RF_PATH_MAX 4
#endif

typedef enum _ODM_RF_RADIO_PATH {
  ODM_RF_PATH_A = 0,  
  ODM_RF_PATH_B = 1,  
  ODM_RF_PATH_C = 2,  
  ODM_RF_PATH_D = 3,  
} ODM_RF_RADIO_PATH_E, *PODM_RF_RADIO_PATH_E;

typedef enum _ODM_RF_CONTENT {
  odm_radioa_txt = 0x1000,
  odm_radiob_txt = 0x1001,
  odm_radioc_txt = 0x1002,
  odm_radiod_txt = 0x1003
} ODM_RF_CONTENT;

typedef enum _ODM_BB_Config_Type {
  CONFIG_BB_PHY_REG,
  CONFIG_BB_AGC_TAB,
  CONFIG_BB_AGC_TAB_2G,
  CONFIG_BB_AGC_TAB_5G,
  CONFIG_BB_PHY_REG_PG,
} ODM_BB_Config_Type, *PODM_BB_Config_Type;

#if (DM_ODM_SUPPORT_TYPE != ODM_MP)
typedef enum _RT_STATUS {
  RT_STATUS_SUCCESS,
  RT_STATUS_FAILURE,
  RT_STATUS_PENDING,
  RT_STATUS_RESOURCE,
  RT_STATUS_INVALID_CONTEXT,
  RT_STATUS_INVALID_PARAMETER,
  RT_STATUS_NOT_SUPPORT,
  RT_STATUS_OS_API_FAILED,
} RT_STATUS, *PRT_STATUS;
#endif

#ifdef REMOVE_PACK
#pragma pack()
#endif



typedef enum tag_Dynamic_Init_Gain_Operation_Type_Definition
{
  DIG_TYPE_THRESH_HIGH  = 0,
  DIG_TYPE_THRESH_LOW = 1,
  DIG_TYPE_BACKOFF    = 2,
  DIG_TYPE_RX_GAIN_MIN  = 3,
  DIG_TYPE_RX_GAIN_MAX  = 4,
  DIG_TYPE_ENABLE     = 5,
  DIG_TYPE_DISABLE    = 6,
  DIG_OP_TYPE_MAX
} DM_DIG_OP_E;
/*
typedef enum tag_CCK_Packet_Detection_Threshold_Type_Definition
{
  CCK_PD_STAGE_LowRssi = 0,
  CCK_PD_STAGE_HighRssi = 1,
  CCK_PD_STAGE_MAX = 3,
}DM_CCK_PDTH_E;

typedef enum tag_DIG_EXT_PORT_ALGO_Definition
{
  DIG_EXT_PORT_STAGE_0 = 0,
  DIG_EXT_PORT_STAGE_1 = 1,
  DIG_EXT_PORT_STAGE_2 = 2,
  DIG_EXT_PORT_STAGE_3 = 3,
  DIG_EXT_PORT_STAGE_MAX = 4,
}DM_DIG_EXT_PORT_ALG_E;

typedef enum tag_DIG_Connect_Definition
{
  DIG_STA_DISCONNECT = 0,
  DIG_STA_CONNECT = 1,
  DIG_STA_BEFORE_CONNECT = 2,
  DIG_MultiSTA_DISCONNECT = 3,
  DIG_MultiSTA_CONNECT = 4,
  DIG_CONNECT_MAX
}DM_DIG_CONNECT_E;


#define DM_MultiSTA_InitGainChangeNotify(Event) {DM_DigTable.CurMultiSTAConnectState = Event;}

#define DM_MultiSTA_InitGainChangeNotify_CONNECT(_ADAPTER)  \
  DM_MultiSTA_InitGainChangeNotify(DIG_MultiSTA_CONNECT)

#define DM_MultiSTA_InitGainChangeNotify_DISCONNECT(_ADAPTER) \
  DM_MultiSTA_InitGainChangeNotify(DIG_MultiSTA_DISCONNECT)
*/
#define   DM_DIG_THRESH_HIGH      40
#define   DM_DIG_THRESH_LOW     35

#define   DM_SCAN_RSSI_TH       0x14


#define   DM_FALSEALARM_THRESH_LOW  400
#define   DM_FALSEALARM_THRESH_HIGH 1000

#define   DM_DIG_MAX_NIC        0x3e
#define   DM_DIG_MIN_NIC        0x1e

#define   DM_DIG_MAX_AP         0x32
#define   DM_DIG_MIN_AP         0x20

#define   DM_DIG_MAX_NIC_HP     0x46
#define   DM_DIG_MIN_NIC_HP     0x2e

#define   DM_DIG_MAX_AP_HP        0x42
#define   DM_DIG_MIN_AP_HP        0x30

#ifdef CONFIG_SPECIAL_SETTING_FOR_FUNAI_TV
#define   DM_DIG_FA_TH0       0x80//0x20
#else
#define   DM_DIG_FA_TH0       0x200//0x20
#endif
#define   DM_DIG_FA_TH1       0x300//0x100
#define   DM_DIG_FA_TH2       0x400//0x200
#define   DM_DIG_FA_TH0_92D     0x100
#define   DM_DIG_FA_TH1_92D     0x400
#define   DM_DIG_FA_TH2_92D     0x600

#define   DM_DIG_BACKOFF_MAX      12
#define   DM_DIG_BACKOFF_MIN      -4
#define   DM_DIG_BACKOFF_DEFAULT    10

#define          LNA_Low_Gain_1                      0x64
#define          LNA_Low_Gain_2                      0x5A
#define          LNA_Low_Gain_3                      0x58

#define          FA_RXHP_TH1                           5000
#define          FA_RXHP_TH2                           1500
#define          FA_RXHP_TH3                             800
#define          FA_RXHP_TH4                             600
#define          FA_RXHP_TH5                             500


#define   TX_POWER_NEAR_FIELD_THRESH_LVL2 74
#define   TX_POWER_NEAR_FIELD_THRESH_LVL1 67
#define   TX_POWER_NEAR_FIELD_THRESH_AP   0x3F

#define   TxHighPwrLevel_Normal   0
#define   TxHighPwrLevel_Level1   1
#define   TxHighPwrLevel_Level2   2
#define   TxHighPwrLevel_BT1      3
#define   TxHighPwrLevel_BT2      4
#define   TxHighPwrLevel_15     5
#define   TxHighPwrLevel_35     6
#define   TxHighPwrLevel_50     7
#define   TxHighPwrLevel_70     8
#define   TxHighPwrLevel_100      9

#if 0
#define OFDM_TABLE_SIZE   37
#define OFDM_TABLE_SIZE_92D   43
#define CCK_TABLE_SIZE    33
#endif


#define   DM_RATR_STA_INIT      0
#define   DM_RATR_STA_HIGH      1
#define     DM_RATR_STA_MIDDLE    2
#define     DM_RATR_STA_LOW     3



typedef enum tag_1R_CCA_Type_Definition
{
  CCA_1R = 0,
  CCA_2R = 1,
  CCA_MAX = 2,
} DM_1R_CCA_E;

typedef enum tag_RF_Type_Definition
{
  RF_Save = 0,
  RF_Normal = 1,
  RF_MAX = 2,
} DM_RF_E;

typedef enum tag_SW_Antenna_Switch_Definition
{
  Antenna_A = 1,
  Antenna_B = 2,
  Antenna_MAX = 3,
} DM_SWAS_E;


#define MAX_ANTENNA_DETECTION_CNT 10

#define OFDM_TABLE_SIZE_92C   37
#define OFDM_TABLE_SIZE_92D   43
#define CCK_TABLE_SIZE    33

extern  u4Byte OFDMSwingTable[OFDM_TABLE_SIZE_92D];
extern  u1Byte CCKSwingTable_Ch1_Ch13[CCK_TABLE_SIZE][8];
extern  u1Byte CCKSwingTable_Ch14 [CCK_TABLE_SIZE][8];



#if (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
#define IS_STA_VALID(pSta)    (pSta && pSta->expire_to)
#elif (DM_ODM_SUPPORT_TYPE & ODM_MP)
#define IS_STA_VALID(pSta)    (pSta && pSta->bUsed)
#else
#define IS_STA_VALID(pSta)    (pSta)
#endif
#define SWAW_STEP_PEAK    0
#define SWAW_STEP_DETERMINE 1

VOID ODM_Write_DIG (IN PDM_ODM_T pDM_Odm,  IN  u1Byte  CurrentIGI);
VOID ODM_Write_CCK_CCA_Thres (IN PDM_ODM_T pDM_Odm, IN u1Byte  CurCCK_CCAThres);

VOID
ODM_SetAntenna (
  IN  PDM_ODM_T pDM_Odm,
  IN  u1Byte    Antenna);


#define dm_RF_Saving  ODM_RF_Saving
void ODM_RF_Saving ( IN  PDM_ODM_T pDM_Odm,
                     IN  u1Byte    bForceInNormal );

#define SwAntDivRestAfterLink ODM_SwAntDivRestAfterLink
VOID ODM_SwAntDivRestAfterLink ( IN  PDM_ODM_T pDM_Odm);

#define dm_CheckTXPowerTracking   ODM_TXPowerTrackingCheck
VOID
ODM_TXPowerTrackingCheck (
  IN    PDM_ODM_T   pDM_Odm
);

BOOLEAN
ODM_RAStateCheck (
  IN    PDM_ODM_T   pDM_Odm,
  IN    s4Byte      RSSI,
  IN    BOOLEAN     bForceUpdate,
  OUT   pu1Byte     pRATRState
);

#if(DM_ODM_SUPPORT_TYPE & (ODM_MP|ODM_AP|ODM_ADSL))
VOID
ODM_ChangeDynamicInitGainThresh (
  IN  PDM_ODM_T pDM_Odm,
  IN  u4Byte  DM_Type,
  IN  u4Byte DM_Value
);

BOOLEAN
ODM_CheckPowerStatus (
  IN  PADAPTER    Adapter
);


#if (DM_ODM_SUPPORT_TYPE != ODM_ADSL)
VOID
ODM_RateAdaptiveStateApInit (
  IN  PADAPTER  Adapter ,
  IN  PRT_WLAN_STA  pEntry
);
#endif
#define AP_InitRateAdaptiveState  ODM_RateAdaptiveStateApInit


#if(DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
#ifdef WIFI_WMM
VOID
ODM_IotEdcaSwitch (
  IN  PDM_ODM_T pDM_Odm,
  IN  unsigned char   enable
);
#endif

BOOLEAN
ODM_ChooseIotMainSTA (
  IN  PDM_ODM_T   pDM_Odm,
  IN  PSTA_INFO_T   pstat
);
#endif

#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
#ifdef HW_ANT_SWITCH
u1Byte
ODM_Diversity_AntennaSelect (
  IN  PDM_ODM_T pDM_Odm,
  IN  u1Byte * data
);
#endif
#endif

#define SwAntDivResetBeforeLink   ODM_SwAntDivResetBeforeLink
VOID ODM_SwAntDivResetBeforeLink (IN PDM_ODM_T pDM_Odm);

#define SwAntDivCheckBeforeLink ODM_SwAntDivCheckBeforeLink8192C
BOOLEAN
ODM_SwAntDivCheckBeforeLink8192C (
  IN    PDM_ODM_T   pDM_Odm
);


#endif

#define dm_SWAW_RSSI_Check  ODM_SwAntDivChkPerPktRssi
VOID ODM_SwAntDivChkPerPktRssi (
  IN PDM_ODM_T    pDM_Odm,
  IN u1Byte     StationID,
  IN PODM_PHY_INFO_T pPhyInfo
);

#if((DM_ODM_SUPPORT_TYPE==ODM_MP)||(DM_ODM_SUPPORT_TYPE==ODM_CE))

u4Byte ConvertTo_dB (u4Byte Value);

u4Byte
GetPSDData (
  PDM_ODM_T pDM_Odm,
  unsigned int  point,
  u1Byte initial_gain_psd);

#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)

VOID
odm_DIGbyRSSI_LPS (
  IN    PDM_ODM_T   pDM_Odm
);

u4Byte ODM_Get_Rate_Bitmap (
  IN  PDM_ODM_T pDM_Odm,
  IN  u4Byte    macid,
  IN  u4Byte    ra_mask,
  IN  u1Byte    rssi_level);
#endif


#if(DM_ODM_SUPPORT_TYPE & (ODM_MP))
#define dm_PSDMonitorCallback odm_PSDMonitorCallback
VOID  odm_PSDMonitorCallback (PRT_TIMER    pTimer);

VOID
odm_PSDMonitorWorkItemCallback (
  IN PVOID            pContext
);


VOID
PatchDCTone (
  IN  PDM_ODM_T pDM_Odm,
  pu4Byte   PSD_report,
  u1Byte    initial_gain_psd
);
VOID
ODM_PSDMonitor (
  IN  PDM_ODM_T pDM_Odm
);
VOID  odm_PSD_Monitor (PDM_ODM_T pDM_Odm);
VOID  odm_PSDMonitorInit (PDM_ODM_T  pDM_Odm);

VOID
ODM_PSDDbgControl (
  IN  PADAPTER  Adapter,
  IN  u4Byte    mode,
  IN  u4Byte    btRssi
);

#endif 



VOID ODM_DMInit ( IN PDM_ODM_T pDM_Odm);

VOID
ODM_DMWatchdog (
  IN    PDM_ODM_T     pDM_Odm    
);

VOID
ODM_CmnInfoInit (
  IN    PDM_ODM_T   pDM_Odm,
  IN    ODM_CMNINFO_E CmnInfo,
  IN    u4Byte      Value
);

VOID
ODM_CmnInfoHook (
  IN    PDM_ODM_T   pDM_Odm,
  IN    ODM_CMNINFO_E CmnInfo,
  IN    PVOID     pValue
);

VOID
ODM_CmnInfoPtrArrayHook (
  IN    PDM_ODM_T   pDM_Odm,
  IN    ODM_CMNINFO_E CmnInfo,
  IN    u2Byte      Index,
  IN    PVOID     pValue
);

VOID
ODM_CmnInfoUpdate (
  IN    PDM_ODM_T   pDM_Odm,
  IN    u4Byte      CmnInfo,
  IN    u8Byte      Value
);

VOID
ODM_InitAllTimers (
  IN PDM_ODM_T  pDM_Odm
);

VOID
ODM_CancelAllTimers (
  IN PDM_ODM_T    pDM_Odm
);

VOID
ODM_ReleaseAllTimers (
  IN PDM_ODM_T  pDM_Odm
);

VOID
ODM_ResetIQKResult (
  IN PDM_ODM_T pDM_Odm
);


#if (DM_ODM_SUPPORT_TYPE == ODM_MP)
VOID ODM_InitAllWorkItems (IN PDM_ODM_T  pDM_Odm );
VOID ODM_FreeAllWorkItems (IN PDM_ODM_T  pDM_Odm );

VOID odm_PathDivChkAntSwitch (PDM_ODM_T pDM_Odm);
VOID ODM_PathDivRestAfterLink (
  IN  PDM_ODM_T   pDM_Odm
);




#define TP_MODE          0
#define RSSI_MODE          1
#define TRAFFIC_LOW        0
#define TRAFFIC_HIGH         1


VOID ODM_PathDivRestAfterLink (
  IN  PDM_ODM_T pDM_Odm
);

#define dm_PathDiv_RSSI_Check ODM_PathDivChkPerPktRssi
VOID ODM_PathDivChkPerPktRssi (PADAPTER    Adapter,
                               BOOLEAN     bIsDefPort,
                               BOOLEAN     bMatchBSSID,
                               PRT_WLAN_STA  pEntry,
                               PRT_RFD     pRfd  );

u8Byte
PlatformDivision64 (
  IN u8Byte x,
  IN u8Byte y
);


#define SWAW_STEP_PEAK    0
#define SWAW_STEP_DETERMINE 1


#define PathDivCheckBeforeLink8192C ODM_PathDiversityBeforeLink92C
BOOLEAN
ODM_PathDiversityBeforeLink92C (
  IN    PDM_ODM_T   pDM_Odm
);

#define DM_ChangeDynamicInitGainThresh    ODM_ChangeDynamicInitGainThresh


VOID
ODM_CCKPathDiversityChkPerPktRssi (
  PADAPTER    Adapter,
  BOOLEAN     bIsDefPort,
  BOOLEAN     bMatchBSSID,
  PRT_WLAN_STA  pEntry,
  PRT_RFD     pRfd,
  pu1Byte     pDesc
);


typedef enum tag_DIG_Connect_Definition
{
  DIG_STA_DISCONNECT = 0,
  DIG_STA_CONNECT = 1,
  DIG_STA_BEFORE_CONNECT = 2,
  DIG_MultiSTA_DISCONNECT = 3,
  DIG_MultiSTA_CONNECT = 4,
  DIG_CONNECT_MAX
} DM_DIG_CONNECT_E;


VOID
ODM_FillTXPathInTXDESC (
  IN  PADAPTER  Adapter,
  IN  PRT_TCB   pTcb,
  IN  pu1Byte   pDesc
);


#define dm_SWAW_RSSI_Check  ODM_SwAntDivChkPerPktRssi

#define HAL_ADAPTER_STS_CHK(pDM_Odm)\
  if (pDM_Odm->Adapter == NULL)\
  {\
    return;\
  }\
   

#define odm_TXPowerTrackingDirectCall(_Adapter) \
  IS_HARDWARE_TYPE_8192D(_Adapter) ? odm_TXPowerTrackingCallback_ThermalMeter_92D(_Adapter) : \
  IS_HARDWARE_TYPE_8192C(_Adapter) ? odm_TXPowerTrackingCallback_ThermalMeter_92C(_Adapter) : \
  IS_HARDWARE_TYPE_8723A(_Adapter) ? odm_TXPowerTrackingCallback_ThermalMeter_8723A(_Adapter) :\
  odm_TXPowerTrackingCallback_ThermalMeter_8188E(_Adapter)

VOID
ODM_SetTxAntByTxInfo_88C_92D (
  IN    PDM_ODM_T   pDM_Odm,
  IN    pu1Byte     pDesc,
  IN    u1Byte      macId
);
#endif 
VOID
ODM_AntselStatistics_88C (
  IN    PDM_ODM_T   pDM_Odm,
  IN    u1Byte      MacId,
  IN    u4Byte      PWDBAll,
  IN    BOOLEAN     isCCKrate
);

#if( DM_ODM_SUPPORT_TYPE & (ODM_MP |ODM_CE))

VOID
ODM_SingleDualAntennaDefaultSetting (
  IN    PDM_ODM_T   pDM_Odm
);

BOOLEAN
ODM_SingleDualAntennaDetection (
  IN    PDM_ODM_T   pDM_Odm,
  IN    u1Byte      mode
);

#endif 

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
void odm_dtc (PDM_ODM_T pDM_Odm);
#endif /* #if (DM_ODM_SUPPORT_TYPE == ODM_CE) */

#endif

