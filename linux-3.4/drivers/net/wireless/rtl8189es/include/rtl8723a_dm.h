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
#ifndef __RTL8723A_DM_H__
#define __RTL8723A_DM_H__
#define DYNAMIC_FUNC_BT BIT(0)

enum{
	UP_LINK,
	DOWN_LINK,	
};

#define IQK_MAC_REG_NUM		4
#define IQK_ADDA_REG_NUM		16
#define IQK_BB_REG_NUM			9
#define HP_THERMAL_NUM		8
struct dm_priv
{
	u8	DM_Type;
	u8	DMFlag;
	u8	InitDMFlag;
	u32	InitODMFlag;

	int	UndecoratedSmoothedPWDB;
	int	UndecoratedSmoothedCCK;
	int	EntryMinUndecoratedSmoothedPWDB;
	int	EntryMaxUndecoratedSmoothedPWDB;
	int	MinUndecoratedPWDBForDM;
	int	LastMinUndecoratedPWDBForDM;

	s32	UndecoratedSmoothedBeacon;
	#ifdef CONFIG_BT_COEXIST
	s32 BT_EntryMinUndecoratedSmoothedPWDB;
	s32 BT_EntryMaxUndecoratedSmoothedPWDB;
	#endif

/*
	u8	bDMInitialGainEnable;
	u8	binitialized;
	DIG_T	DM_DigTable;

	PS_T	DM_PSTable;

	FALSE_ALARM_STATISTICS FalseAlmCnt;

	u8 bUseRAMask;
	RATE_ADAPTIVE RateAdaptive;
*/
	u8 bDynamicTxPowerEnable;
	u8 LastDTPLvl;
	u8 DynamicTxHighPowerLvl;//Add by Jacken Tx Power Control for Near/Far Range 2008/03/06

	u8	bTXPowerTracking;
	u8	TXPowercount;
	u8	bTXPowerTrackingInit;
	u8	TxPowerTrackControl;
	u8	TM_Trigger;

	u8	ThermalMeter[2];			
	u8	ThermalValue;
	u8	ThermalValue_LCK;
	u8	ThermalValue_IQK;
	u8	ThermalValue_DPK;

	u8	bRfPiEnable;

	u32	APKoutput[2][2];
	u8	bAPKdone;
	u8	bAPKThermalMeterIgnore;
	u8	bDPdone;
	u8	bDPPathAOK;
	u8	bDPPathBOK;

	u32	RegC04;
	u32	Reg874;
	u32	RegC08;
	u32	RegB68;
	u32	RegB6C;
	u32	Reg870;
	u32	Reg860;
	u32	Reg864;
	u32	ADDA_backup[IQK_ADDA_REG_NUM];
	u32	IQK_MAC_backup[IQK_MAC_REG_NUM];
	u32	IQK_BB_backup_recover[9];
	u32	IQK_BB_backup[IQK_BB_REG_NUM];
	u8	PowerIndex_backup[6];

	u8	bCCKinCH14;

	u8	CCK_index;
	u8	OFDM_index[2];

	u8	bDoneTxpower;
	u8	CCK_index_HP;
	u8	OFDM_index_HP[2];
	u8	ThermalValue_HP[HP_THERMAL_NUM];
	u8	ThermalValue_HP_index;

	s32	RegE94;
	s32     RegE9C;
	s32	RegEB4;
	s32	RegEBC;

	u32	TXPowerTrackingCallbackCnt;

	u32	prv_traffic_idx;

/*
	u8	initialize;
	u32	rf_saving_Reg874;
	u32	rf_saving_RegC70;
	u32	rf_saving_Reg85C;
	u32	rf_saving_RegA74;
*/
#ifdef CONFIG_ANTENNA_DIVERSITY
#endif

#ifdef CONFIG_SW_ANTENNA_DIVERSITY
/*	
	u64	lastTxOkCnt;
	u64	lastRxOkCnt;
	u64	TXByteCnt_A;
	u64	TXByteCnt_B;
	u64	RXByteCnt_A;
	u64	RXByteCnt_B;
	u8	DoubleComfirm;
	u8	TrafficLoad;
*/
#endif

	s32	OFDM_Pkt_Cnt;
	u8	RSSI_Select;
	u8	INIDATA_RATE[32];
};



void rtl8723a_init_dm_priv(PADAPTER padapter);
void rtl8723a_deinit_dm_priv(PADAPTER padapter);
	
void rtl8723a_InitHalDm(PADAPTER padapter);
void rtl8723a_HalDmWatchDog(PADAPTER padapter);



#endif

