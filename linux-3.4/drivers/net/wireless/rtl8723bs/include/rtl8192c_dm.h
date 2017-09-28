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
#ifndef	__RTL8192C_DM_H__
#define __RTL8192C_DM_H__


typedef	enum _BT_CurState{
	BT_OFF		= 0,	
	BT_ON		= 1,
} BT_CurState, *PBT_CurState;

typedef	enum _BT_ServiceType{
	BT_SCO		= 0,	
	BT_A2DP		= 1,
	BT_HID		= 2,
	BT_HID_Idle	= 3,
	BT_Scan		= 4,
	BT_Idle		= 5,
	BT_OtherAction	= 6,
	BT_Busy			= 7,
	BT_OtherBusy		= 8,
	BT_PAN			= 9,
} BT_ServiceType, *PBT_ServiceType;

struct btcoexist_priv	{
	u8					BT_Coexist;
	u8					BT_Ant_Num;
	u8					BT_CoexistType;
	u8					BT_State;
	u8					BT_CUR_State;	
	u8					BT_Ant_isolation;
	u8					BT_PapeCtrl;	
	u8					BT_Service;
	u8					BT_Ampdu;
	u8					BT_RadioSharedType;
	u32					Ratio_Tx;
	u32					Ratio_PRI;
	u8					BtRfRegOrigin1E;
	u8					BtRfRegOrigin1F;
	u8					BtRssiState;
	u32					BtEdcaUL;
	u32					BtEdcaDL;
	u32					BT_EDCA[2];
	u8					bCOBT;

	u8					bInitSet;
	u8					bBTBusyTraffic;
	u8					bBTTrafficModeSet;
	u8					bBTNonTrafficModeSet;
	u32					CurrentState;
	u32					PreviousState;
	u8					BtPreRssiState;
	u8					bFWCoexistAllOff;
	u8					bSWCoexistAllOff;
};


#ifdef CONFIG_BT_COEXIST
void rtl8192c_set_dm_bt_coexist(_adapter *padapter, u8 bStart);
void rtl8192c_issue_delete_ba(_adapter *padapter, u8 dir);
#endif

void rtl8192c_init_dm_priv(IN PADAPTER Adapter);
void rtl8192c_deinit_dm_priv(IN PADAPTER Adapter);

void rtl8192c_InitHalDm(	IN	PADAPTER	Adapter);
void rtl8192c_HalDmWatchDog(IN PADAPTER Adapter);

#endif

