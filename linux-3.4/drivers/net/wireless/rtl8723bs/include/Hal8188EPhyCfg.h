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
#ifndef __INC_HAL8188EPHYCFG_H__
#define __INC_HAL8188EPHYCFG_H__


/*--------------------------Define Parameters-------------------------------*/
#define LOOP_LIMIT				5
#define MAX_STALL_TIME			50	
#define AntennaDiversityValue		0x80
#define MAX_TXPWR_IDX_NMODE_92S	63
#define Reset_Cnt_Limit			3

#ifdef CONFIG_PCI_HCI
#define MAX_AGGR_NUM	0x0B
#else
#define MAX_AGGR_NUM	0x07
#endif


/*--------------------------Define Parameters-------------------------------*/


/*------------------------------Define structure----------------------------*/ 

#define MAX_PG_GROUP 13

#define	MAX_TX_COUNT_8188E			1

/* BB/RF related */


/*------------------------------Define structure----------------------------*/ 


/*------------------------Export global variable----------------------------*/
/*------------------------Export global variable----------------------------*/


/*------------------------Export Marco Definition---------------------------*/
/*------------------------Export Marco Definition---------------------------*/


/*--------------------------Exported Function prototype---------------------*/
u32	PHY_QueryBBReg8188E(	IN	PADAPTER	Adapter,
								IN	u32		RegAddr,
								IN	u32		BitMask	);
void	PHY_SetBBReg8188E(	IN	PADAPTER	Adapter,
								IN	u32		RegAddr,
								IN	u32		BitMask,
								IN	u32		Data	);
u32	PHY_QueryRFReg8188E(	IN	PADAPTER	Adapter,
								IN	u8				eRFPath,
								IN	u32				RegAddr,
								IN	u32				BitMask	);
void	PHY_SetRFReg8188E(	IN	PADAPTER		Adapter,
								IN	u8				eRFPath,
								IN	u32				RegAddr,
								IN	u32				BitMask,
								IN	u32				Data	);

/* MAC/BB/RF HAL config */
int	PHY_MACConfig8188E(IN	PADAPTER	Adapter	);
int	PHY_BBConfig8188E(IN	PADAPTER	Adapter	);
int	PHY_RFConfig8188E(IN	PADAPTER	Adapter	);

/* RF config */
int	rtl8188e_PHY_ConfigRFWithParaFile(IN PADAPTER Adapter, IN u8 * pFileName, u8 eRFPath);

/* Read initi reg value for tx power setting. */
void	rtl8192c_PHY_GetHWRegOriginalValue(	IN	PADAPTER		Adapter	);


void	PHY_GetTxPowerLevel8188E(	IN	PADAPTER		Adapter,
											OUT s32*    		powerlevel	);
void	PHY_SetTxPowerLevel8188E(	IN	PADAPTER		Adapter,
											IN	u8			channel	);
BOOLEAN	PHY_UpdateTxPowerDbm8188E(	IN	PADAPTER	Adapter,
											IN	int		powerInDbm	);

VOID
PHY_SetTxPowerIndex_8188E(
	IN	PADAPTER			Adapter,
	IN	u32					PowerIndex,
	IN	u8					RFPath,	
	IN	u8					Rate
	);

u8
PHY_GetTxPowerIndex_8188E(
	IN	PADAPTER		pAdapter,
	IN	u8				RFPath,
	IN	u8				Rate,	
	IN	CHANNEL_WIDTH	BandWidth,	
	IN	u8				Channel
	);

void	PHY_SetBWMode8188E(	IN	PADAPTER			pAdapter,
									IN	CHANNEL_WIDTH	ChnlWidth,
									IN	unsigned char	Offset	);


extern	void FillA2Entry8192C(		IN	PADAPTER			Adapter,
										IN	u8				index,
										IN	u8*				val);


void	PHY_SwChnl8188E(	IN	PADAPTER		pAdapter,
									IN	u8			channel	);

VOID
PHY_SetSwChnlBWMode8188E(
	IN	PADAPTER			Adapter,
	IN	u8					channel,
	IN	CHANNEL_WIDTH	Bandwidth,
	IN	u8					Offset40,
	IN	u8					Offset80
);

void	PHY_SetMonitorMode8192C(IN	PADAPTER	pAdapter,
										IN	BOOLEAN		bEnableMonitorMode	);

BOOLEAN	PHY_CheckIsLegalRfPath8192C(IN	PADAPTER	pAdapter,
											IN	u32		eRFPath	);

VOID PHY_SetRFPathSwitch_8188E(IN	PADAPTER	pAdapter, IN	BOOLEAN		bMain);

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

VOID
storePwrIndexDiffRateOffset(
	IN	PADAPTER	Adapter,
	IN	u32		RegAddr,
	IN	u32		BitMask,
	IN	u32		Data
	);
/*--------------------------Exported Function prototype---------------------*/

/* MAC/BB/RF HAL config */



#if (RTL8188E_SUPPORT == 1) && (RTL8188E_FPGA_TRUE_PHY_VERIFICATION == 1)
#define	SIC_ENABLE				1
#define	SIC_HW_SUPPORT		1
#else
#define	SIC_ENABLE				0
#define	SIC_HW_SUPPORT		0
#endif


#define	SIC_MAX_POLL_CNT		5

#if(SIC_HW_SUPPORT == 1)
#define	SIC_CMD_READY			0
#define	SIC_CMD_PREWRITE		0x1
#if(RTL8188E_SUPPORT == 1)
#define	SIC_CMD_WRITE			0x40
#define	SIC_CMD_PREREAD		0x2
#define	SIC_CMD_READ			0x80
#define	SIC_CMD_INIT			0xf0
#define	SIC_INIT_VAL			0xff

#define	SIC_INIT_REG			0x1b7
#define	SIC_CMD_REG			0x1EB	
#define	SIC_ADDR_REG			0x1E8	
#define	SIC_DATA_REG			0x1EC	
#else
#define	SIC_CMD_WRITE			0x11
#define	SIC_CMD_PREREAD		0x2
#define	SIC_CMD_READ			0x12
#define	SIC_CMD_INIT			0x1f
#define	SIC_INIT_VAL			0xff

#define	SIC_INIT_REG			0x1b7
#define	SIC_CMD_REG			0x1b6	
#define	SIC_ADDR_REG			0x1b4	
#define	SIC_DATA_REG			0x1b0	
#endif
#else
#define	SIC_CMD_READY			0
#define	SIC_CMD_WRITE			1
#define	SIC_CMD_READ			2

#if(RTL8188E_SUPPORT == 1)
#define	SIC_CMD_REG			0x1EB	
#define	SIC_ADDR_REG			0x1E8	
#define	SIC_DATA_REG			0x1EC	
#else
#define	SIC_CMD_REG			0x1b8	
#define	SIC_ADDR_REG			0x1b9	
#define	SIC_DATA_REG			0x1bc	
#endif
#endif

#if(SIC_ENABLE == 1)
VOID SIC_Init(IN PADAPTER Adapter);
#endif


#endif

