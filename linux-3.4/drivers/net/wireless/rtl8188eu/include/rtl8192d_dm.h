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
#ifndef	__RTL8192D_DM_H__
#define __RTL8192D_DM_H__
enum{
	UP_LINK,
	DOWN_LINK,	
};
/*------------------------Export global variable----------------------------*/
/*------------------------Export global variable----------------------------*/
/*------------------------Export Marco Definition---------------------------*/

#define IQK_MAC_REG_NUM		4
#define IQK_ADDA_REG_NUM		16
#define IQK_BB_REG_NUM			10
#define IQK_BB_REG_NUM_92C	9
#define IQK_BB_REG_NUM_92D	10
#define IQK_BB_REG_NUM_test	6
#define index_mapping_NUM		13
#define Rx_index_mapping_NUM	15
#define AVG_THERMAL_NUM		8
#define IQK_Matrix_REG_NUM	8
#define IQK_Matrix_Settings_NUM	1+24+21
struct 	dm_priv	
{
	u8	DM_Type;
	u8	DMFlag;
	u8	InitDMFlag;
	u32	InitODMFlag;
	
	int	UndecoratedSmoothedPWDB;
	int	EntryMinUndecoratedSmoothedPWDB;
	int	EntryMaxUndecoratedSmoothedPWDB;
	int	MinUndecoratedPWDBForDM;
	int	LastMinUndecoratedPWDBForDM;
/*
	u8	bDMInitialGainEnable;
	DIG_T	DM_DigTable;

	PS_T	DM_PSTable;

	FALSE_ALARM_STATISTICS	FalseAlmCnt;	
	
	u8	bUseRAMask;
	RATE_ADAPTIVE	RateAdaptive;
*/
	
	u8	bDynamicTxPowerEnable;
	u8	LastDTPLvl;
	u8	DynamicTxHighPowerLvl;//Add by Jacken Tx Power Control for Near/Far Range 2008/03/06
		
	u8	bTXPowerTracking;
	u8	TXPowercount;
	u8	bTXPowerTrackingInit;	
	u8	TxPowerTrackControl;
	u8	TM_Trigger;

	u8	ThermalMeter[2];
	u8	ThermalValue;
	u8	ThermalValue_LCK;
	u8	ThermalValue_IQK;
	u8	ThermalValue_AVG[AVG_THERMAL_NUM];
	u8	ThermalValue_AVG_index;
	u8	ThermalValue_RxGain;
	u8	ThermalValue_Crystal;
	u8	Delta_IQK;
	u8	Delta_LCK;
	u8	bRfPiEnable;
	u8	bReloadtxpowerindex;
	u8	bDoneTxpower;

	u32	APKoutput[2][2];
	u8	bAPKdone;
	u8	bAPKThermalMeterIgnore;
	u32	RegA24;
	
	u32	Reg874;
	u32	RegC08;
	u32	Reg88C;
	u8	Reg522;
	u8	Reg550;
	u8	Reg551;
	u32	Reg870;
	u32	ADDA_backup[IQK_ADDA_REG_NUM];
	u32	IQK_MAC_backup[IQK_MAC_REG_NUM];
	u32	IQK_BB_backup[IQK_BB_REG_NUM];

	u8	bCCKinCH14;
	
	char	CCK_index;
	char	OFDM_index[2];
	
	BOOLEAN	bDPKdone[2];

	u8	PowerIndex_backup[6];
	
	BOOLEAN		bPathDiv_Enable;
	BOOLEAN		RSSI_test;
	s32			RSSI_sum_A;
	s32			RSSI_cnt_A;
	s32			RSSI_sum_B;
	s32			RSSI_cnt_B;
	struct sta_info	*RSSI_target;
	_timer		PathDivSwitchTimer;

	int	RegE94;
	int 	RegE9C;
	int	RegEB4;
	int	RegEBC;
#if MP_DRIVER == 1
	u8	RegC04_MP;
	u32	RegD04_MP;
#endif
	u32	TXPowerTrackingCallbackCnt;

	u32	prv_traffic_idx;

	u32	RegRF3C[2];
	u8	INIDATA_RATE[32];
};


void rtl8192d_init_dm_priv(IN PADAPTER Adapter);
void rtl8192d_deinit_dm_priv(IN PADAPTER Adapter);

void	rtl8192d_InitHalDm(IN PADAPTER Adapter);
void	rtl8192d_HalDmWatchDog(IN PADAPTER Adapter);

#endif

