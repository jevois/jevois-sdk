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
/******************************************************************************
 *
 *
 * Module:	rtl8192c_rf6052.c	( Source C File)
 *
 * Note:	Provide RF 6052 series relative API.
 *
 * Function:
 *
 * Export:
 *
 * Abbrev:
 *
 * History:
 * Data			Who		Remark
 *
 * 09/25/2008	MHC		Create initial version.
 * 11/05/2008 	MHC		Add API for tw power setting.
 *
 *
******************************************************************************/

#define _RTL8188E_RF6052_C_

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>
#include <rtw_byteorder.h>

#include <rtl8188e_hal.h>

/*---------------------------Define Local Constant---------------------------*/
typedef struct RF_Shadow_Compare_Map {
	u32		Value;
	u8		Compare;
	u8		ErrorOrNot;
	u8		Recorver;
	u8		Driver_Write;
}RF_SHADOW_T;
/*---------------------------Define Local Constant---------------------------*/


/*------------------------Define global variable-----------------------------*/
/*------------------------Define global variable-----------------------------*/


/*------------------------Define local variable------------------------------*/
static	RF_SHADOW_T	RF_Shadow[RF6052_MAX_PATH][RF6052_MAX_REG];
/*------------------------Define local variable------------------------------*/


/*-----------------------------------------------------------------------------
 * Function:	RF_ChangeTxPath
 *
 * Overview:	For RL6052, we must change some RF settign for 1T or 2T.
 *
 * Input:		u2Byte DataRate	
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 * When			Who		Remark
 * 09/25/2008 	MHC		Create Version 0.
 *						Firmwaer support the utility later.
 *
 *---------------------------------------------------------------------------*/
void rtl8188e_RF_ChangeTxPath(	IN	PADAPTER	Adapter,
										IN	u16		DataRate)
{
#if 0//(RTL92SE_FPGA_VERIFY == 0)
	static	u1Byte	RF_Path_Type = 2;
	static	u4Byte	tx_gain_tbl1[6]
			= {0x17f50, 0x11f40, 0x0cf30, 0x08720, 0x04310, 0x00100};
	static	u4Byte	tx_gain_tbl2[6]
			= {0x15ea0, 0x10e90, 0x0c680, 0x08250, 0x04040, 0x00030};
	u1Byte	i;

	if (RF_Path_Type == 2 && (DataRate&0xF) <= 0x7)
	{
		PHY_SetRFReg(Adapter, (RF_RADIO_PATH_E)RF_PATH_A,
					RF_TXPA_G2, bRFRegOffsetMask, 0x0f000);
		PHY_SetRFReg(Adapter, (RF_RADIO_PATH_E)RF_PATH_A,
					RF_TXPA_G3, bRFRegOffsetMask, 0xeacf1);

		for (i = 0; i < 6; i++)
			PHY_SetRFReg(Adapter, (RF_RADIO_PATH_E)RF_PATH_A,
						RF_TX_AGC, bRFRegOffsetMask, tx_gain_tbl1[i]);

		PHY_SetRFReg(Adapter, (RF_RADIO_PATH_E)RF_PATH_A,
					RF_TXPA_G2, bRFRegOffsetMask, 0x01e39);
	}
	else if (RF_Path_Type == 1 && (DataRate&0xF) >= 0x8)
	{
		PHY_SetRFReg(Adapter, (RF_RADIO_PATH_E)RF_PATH_A,
					RF_TXPA_G2, bRFRegOffsetMask, 0x04440);
		PHY_SetRFReg(Adapter, (RF_RADIO_PATH_E)RF_PATH_A,
					RF_TXPA_G3, bRFRegOffsetMask, 0xea4f1);

		for (i = 0; i < 6; i++)
			PHY_SetRFReg(Adapter, (RF_RADIO_PATH_E)RF_PATH_A,
						RF_TX_AGC, bRFRegOffsetMask, tx_gain_tbl2[i]);

		PHY_SetRFReg(Adapter, (RF_RADIO_PATH_E)RF_PATH_A,
					RF_TXPA_G2, bRFRegOffsetMask, 0x01e19);
	}
#endif

}	/* RF_ChangeTxPath */


/*-----------------------------------------------------------------------------
 * Function:    PHY_RF6052SetBandwidth()
 *
 * Overview:    This function is called by SetBWModeCallback8190Pci() only
 *
 * Input:       PADAPTER				Adapter
 *			WIRELESS_BANDWIDTH_E	Bandwidth
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Note:		For RF type 0222D
 *---------------------------------------------------------------------------*/
VOID
rtl8188e_PHY_RF6052SetBandwidth(
	IN	PADAPTER				Adapter,
	IN	HT_CHANNEL_WIDTH		Bandwidth)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	switch(Bandwidth)
	{
		case HT_CHANNEL_WIDTH_20:
			pHalData->RfRegChnlVal[0] = ((pHalData->RfRegChnlVal[0] & 0xfffff3ff) | BIT(10) | BIT(11));
			PHY_SetRFReg(Adapter, RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask, pHalData->RfRegChnlVal[0]);
			break;

		case HT_CHANNEL_WIDTH_40:
			pHalData->RfRegChnlVal[0] = ((pHalData->RfRegChnlVal[0] & 0xfffff3ff)| BIT(10));
			PHY_SetRFReg(Adapter, RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask, pHalData->RfRegChnlVal[0]);
			break;

		default:
			break;
	}

}


/*-----------------------------------------------------------------------------
 * Function:	PHY_RF6052SetCckTxPower
 *
 * Overview:
 *
 * Input:       NONE
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 * When			Who		Remark
 * 11/05/2008 	MHC		Simulate 8192series..
 *
 *---------------------------------------------------------------------------*/

VOID
rtl8188e_PHY_RF6052SetCckTxPower(
	IN	PADAPTER		Adapter,
	IN	u8*			pPowerlevel)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	struct mlme_priv	*pmlmepriv = &Adapter->mlmepriv;
	struct dm_priv		*pdmpriv = &pHalData->dmpriv;
	struct mlme_ext_priv 		*pmlmeext = &Adapter->mlmeextpriv;
	u32			TxAGC[2]={0, 0}, tmpval=0,pwrtrac_value;
	BOOLEAN		TurboScanOff = _FALSE;
	u8			idx1, idx2;
	u8*			ptr;
	u8			direction;
	TurboScanOff = _TRUE;
	

	if(pmlmeext->sitesurvey_res.state == SCAN_PROCESS)
	{
		TxAGC[RF_PATH_A] = 0x3f3f3f3f;
		TxAGC[RF_PATH_B] = 0x3f3f3f3f;

		TurboScanOff = _TRUE;//disable turbo scan

		if(TurboScanOff)
		{
			for(idx1=RF_PATH_A; idx1<=RF_PATH_B; idx1++)
			{
				TxAGC[idx1] =
					pPowerlevel[idx1] | (pPowerlevel[idx1]<<8) |
					(pPowerlevel[idx1]<<16) | (pPowerlevel[idx1]<<24);
#ifdef CONFIG_USB_HCI
				if (TxAGC[idx1] > 0x20 && pHalData->ExternalPA)
					TxAGC[idx1] = 0x20;
#endif
			}
		}
	}
	else
	{
		if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level1)
		{
			TxAGC[RF_PATH_A] = 0x10101010;
			TxAGC[RF_PATH_B] = 0x10101010;
		}
		else if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level2)
		{
			TxAGC[RF_PATH_A] = 0x00000000;
			TxAGC[RF_PATH_B] = 0x00000000;
		}
		else
		{
			for(idx1=RF_PATH_A; idx1<=RF_PATH_B; idx1++)
			{
				TxAGC[idx1] =
					pPowerlevel[idx1] | (pPowerlevel[idx1]<<8) |
					(pPowerlevel[idx1]<<16) | (pPowerlevel[idx1]<<24);
			}

			if(pHalData->EEPROMRegulatory==0)
			{
				tmpval = (pHalData->MCSTxPowerLevelOriginalOffset[0][6]) +
						(pHalData->MCSTxPowerLevelOriginalOffset[0][7]<<8);
				TxAGC[RF_PATH_A] += tmpval;

				tmpval = (pHalData->MCSTxPowerLevelOriginalOffset[0][14]) +
						(pHalData->MCSTxPowerLevelOriginalOffset[0][15]<<24);
				TxAGC[RF_PATH_B] += tmpval;
			}
		}
	}

	for(idx1=RF_PATH_A; idx1<=RF_PATH_B; idx1++)
	{
		ptr = (u8*)(&(TxAGC[idx1]));
		for(idx2=0; idx2<4; idx2++)
		{
			if(*ptr > RF6052_MAX_TX_PWR)
				*ptr = RF6052_MAX_TX_PWR;
			ptr++;
		}
	}
	ODM_TxPwrTrackAdjust88E(&pHalData->odmpriv, 1, &direction, &pwrtrac_value);

	if (direction == 1)		
	{
		TxAGC[0] += pwrtrac_value;
		TxAGC[1] += pwrtrac_value;
	}
	else if (direction == 2)
	{
		TxAGC[0] -=  pwrtrac_value;
		TxAGC[1] -=  pwrtrac_value;
	}


	tmpval = TxAGC[RF_PATH_A]&0xff;
	PHY_SetBBReg(Adapter, rTxAGC_A_CCK1_Mcs32, bMaskByte1, tmpval);
	tmpval = TxAGC[RF_PATH_A]>>8;
	PHY_SetBBReg(Adapter, rTxAGC_B_CCK11_A_CCK2_11, 0xffffff00, tmpval);

	tmpval = TxAGC[RF_PATH_B]>>24;
	PHY_SetBBReg(Adapter, rTxAGC_B_CCK11_A_CCK2_11, bMaskByte0, tmpval);
	tmpval = TxAGC[RF_PATH_B]&0x00ffffff;
	PHY_SetBBReg(Adapter, rTxAGC_B_CCK1_55_Mcs32, 0xffffff00, tmpval);

}	/* PHY_RF6052SetCckTxPower */

#if 0
static void getPowerBase(
	IN	PADAPTER	Adapter,
	IN	u8*		pPowerLevel,
	IN	u8		Channel,
	IN OUT u32*	OfdmBase,
	IN OUT u32*	MCSBase
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32			powerBase0, powerBase1;
	u8			Legacy_pwrdiff=0, HT20_pwrdiff=0;
	u8			i, powerlevel[2];

	for(i=0; i<2; i++)
	{
		powerlevel[i] = pPowerLevel[i];
		Legacy_pwrdiff = pHalData->TxPwrLegacyHtDiff[i][Channel-1];
		powerBase0 = powerlevel[i] + Legacy_pwrdiff;

		powerBase0 = (powerBase0<<24) | (powerBase0<<16) |(powerBase0<<8) |powerBase0;
		*(OfdmBase+i) = powerBase0;
	}

	for(i=0; i<2; i++)
	{
		if(pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_20)
		{
			HT20_pwrdiff = pHalData->TxPwrHt20Diff[i][Channel-1];
			powerlevel[i] += HT20_pwrdiff;
		}
		powerBase1 = powerlevel[i];
		powerBase1 = (powerBase1<<24) | (powerBase1<<16) |(powerBase1<<8) |powerBase1;
		*(MCSBase+i) = powerBase1;
	}
}
#endif
void getPowerBase88E(
	IN	PADAPTER	Adapter,
	IN	u8*			pPowerLevelOFDM,
	IN	u8*			pPowerLevelBW20,
	IN	u8*			pPowerLevelBW40,
	IN	u8			Channel,
	IN OUT u32*		OfdmBase,
	IN OUT u32*		MCSBase
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32			powerBase0, powerBase1;
	u8			Legacy_pwrdiff=0;
	s8			HT20_pwrdiff=0;
	u8			i, powerlevel[2];

	for(i=0; i<2; i++)
	{
		powerBase0 = pPowerLevelOFDM[i];

		powerBase0 = (powerBase0<<24) | (powerBase0<<16) |(powerBase0<<8) |powerBase0;
		*(OfdmBase+i) = powerBase0;
	}

	for(i=0; i<pHalData->NumTotalRFPath; i++)
	{
		if(pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_20)
		{
			powerlevel[i] = pPowerLevelBW20[i];
		}
		else
		{
			powerlevel[i] = pPowerLevelBW40[i];
		}	
		powerBase1 = powerlevel[i];
		powerBase1 = (powerBase1<<24) | (powerBase1<<16) |(powerBase1<<8) |powerBase1;
		*(MCSBase+i) = powerBase1;
	}
}
#if 0
static void getTxPowerWriteValByRegulatory(
	IN		PADAPTER	Adapter,
	IN		u8			Channel,
	IN		u8			index,
	IN		u32*		powerBase0,
	IN		u32*		powerBase1,
	OUT		u32*		pOutWriteVal
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	u8	i, chnlGroup, pwr_diff_limit[4];
	u32 	writeVal, customer_limit, rf;

	for(rf=0; rf<2; rf++)
	{
		switch(pHalData->EEPROMRegulatory)
		{
			case 0:
				chnlGroup = 0;
				writeVal = pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)] +
					((index<2)?powerBase0[rf]:powerBase1[rf]);
				break;
			case 1:
				{
					if(pHalData->pwrGroupCnt == 1)
						chnlGroup = 0;
					if(pHalData->pwrGroupCnt >= 3)
					{
						if(Channel <= 3)
							chnlGroup = 0;
						else if(Channel >= 4 && Channel <= 9)
							chnlGroup = 1;
						else if(Channel > 9)
							chnlGroup = 2;

						if(pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_20)
							chnlGroup++;
						else
							chnlGroup+=4;
					}
					writeVal = pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)] +
							((index<2)?powerBase0[rf]:powerBase1[rf]);
				}
				break;
			case 2:
				writeVal = ((index<2)?powerBase0[rf]:powerBase1[rf]);
				break;
			case 3:
				chnlGroup = 0;

				if (pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_40)
				{
				}
				else
				{
				}
				for (i=0; i<4; i++)
				{
					pwr_diff_limit[i] = (u8)((pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)]&(0x7f<<(i*8)))>>(i*8));
					if (pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_40)
					{
						if(pwr_diff_limit[i] > pHalData->PwrGroupHT40[rf][Channel-1])
							pwr_diff_limit[i] = pHalData->PwrGroupHT40[rf][Channel-1];
					}
					else
					{
						if(pwr_diff_limit[i] > pHalData->PwrGroupHT20[rf][Channel-1])
							pwr_diff_limit[i] = pHalData->PwrGroupHT20[rf][Channel-1];
					}
				}
				customer_limit = (pwr_diff_limit[3]<<24) | (pwr_diff_limit[2]<<16) |
								(pwr_diff_limit[1]<<8) | (pwr_diff_limit[0]);

				writeVal = customer_limit + ((index<2)?powerBase0[rf]:powerBase1[rf]);
				break;
			default:
				chnlGroup = 0;
				writeVal = pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)] +
						((index<2)?powerBase0[rf]:powerBase1[rf]);
				break;
		}


		if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level1)
			writeVal = 0x14141414;
		else if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level2)
			writeVal = 0x00000000;


		if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_BT1)
		{
			writeVal = writeVal - 0x06060606;
		}
		else if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_BT2)
		{
			writeVal = writeVal;
		}
		*(pOutWriteVal+rf) = writeVal;
	}
}
#endif
void getTxPowerWriteValByRegulatory88E(
	IN		PADAPTER	Adapter,
	IN		u8			Channel,
	IN		u8			index,
	IN		u32*		powerBase0,
	IN		u32*		powerBase1,
	OUT		u32*		pOutWriteVal
	)
{
	
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	u1Byte			i, chnlGroup=0, pwr_diff_limit[4], customer_pwr_limit;
	s1Byte			pwr_diff=0;
	u4Byte 			writeVal, customer_limit, rf;
	u1Byte			Regulatory = pHalData->EEPROMRegulatory;

#if 0
	if(pMgntInfo->IntelProximityModeInfo.PowerOutput > 0)
		Regulatory = 2;
#endif

	for(rf=0; rf<2; rf++)
	{
		switch(Regulatory)
		{
			case 0:
				chnlGroup = 0;
				writeVal = pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)] + 
					((index<2)?powerBase0[rf]:powerBase1[rf]);
				break;
			case 1:
				{
					if(pHalData->pwrGroupCnt == 1)
						chnlGroup = 0;
					{
						if (Channel < 3)		
							chnlGroup = 0;
						else if (Channel < 6)	
							chnlGroup = 1;
						else	 if(Channel <9)	
							chnlGroup = 2;
						else if(Channel <12)	
							chnlGroup = 3;
						else if(Channel <14)	
							chnlGroup = 4;
						else if(Channel ==14)	
							chnlGroup = 4;	
						
						if(pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_20)
							chnlGroup++;
						else
							chnlGroup+=6;
				
/*
						if(Channel <= 3)
							chnlGroup = 0;
						else if(Channel >= 4 && Channel <= 9)
							chnlGroup = 1;
						else if(Channel > 9)
							chnlGroup = 2;
						
						
						if(pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_20)
							chnlGroup++;
						else
							chnlGroup+=4;
*/							
					}
					writeVal = pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)] + 
							((index<2)?powerBase0[rf]:powerBase1[rf]);
				}
				break;
			case 2:
				writeVal = ((index<2)?powerBase0[rf]:powerBase1[rf]);
				break;
			case 3:
				chnlGroup = 0;

				/*
				if (pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_20_40)
				{
					RTPRINT(FPHY, PHY_TXPWR, ("customer's limit, 40MHz rf(%c) = 0x%x\n", 
						((rf==0)?'A':'B'), pHalData->PwrGroupHT40[rf][Channel-1]));
				}
				else
				{
					RTPRINT(FPHY, PHY_TXPWR, ("customer's limit, 20MHz rf(%c) = 0x%x\n", 
						((rf==0)?'A':'B'), pHalData->PwrGroupHT20[rf][Channel-1]));
				}*/

				if(index < 2)
					pwr_diff = pHalData->TxPwrLegacyHtDiff[rf][Channel-1];
				else if (pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_20)
					pwr_diff = pHalData->TxPwrHt20Diff[rf][Channel-1];


				if (pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_40)
					customer_pwr_limit = pHalData->PwrGroupHT40[rf][Channel-1];
				else
					customer_pwr_limit = pHalData->PwrGroupHT20[rf][Channel-1];


				if(pwr_diff >= customer_pwr_limit)
					pwr_diff = 0;
				else
					pwr_diff = customer_pwr_limit - pwr_diff;
				
				for (i=0; i<4; i++)
					{
					pwr_diff_limit[i] = (u1Byte)((pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)]&(0x7f<<(i*8)))>>(i*8));
					
					if(pwr_diff_limit[i] > pwr_diff)
						pwr_diff_limit[i] = pwr_diff;
				}
				customer_limit = (pwr_diff_limit[3]<<24) | (pwr_diff_limit[2]<<16) |
								(pwr_diff_limit[1]<<8) | (pwr_diff_limit[0]);
				writeVal = customer_limit + ((index<2)?powerBase0[rf]:powerBase1[rf]);
				break;
			default:
				chnlGroup = 0;
				writeVal = pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)] + 
						((index<2)?powerBase0[rf]:powerBase1[rf]);
				break;
		}

		if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level1)
			writeVal = 0x14141414;
		else if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level2)
			writeVal = 0x00000000;

		if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_BT1)
		{
			writeVal = writeVal - 0x06060606;
		}
		else if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_BT2)
		{
			writeVal = writeVal ;		
		}
		/*
		if(pMgntInfo->bDisableTXPowerByRate)
		{
			writeVal = 0x2c2c2c2c;
		}
		*/
		*(pOutWriteVal+rf) = writeVal;
	}
}

static void writeOFDMPowerReg88E(
	IN		PADAPTER	Adapter,
	IN		u8		index,
	IN 		u32*		pValue
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u16 RegOffset_A[6] = {	rTxAGC_A_Rate18_06, rTxAGC_A_Rate54_24,
							rTxAGC_A_Mcs03_Mcs00, rTxAGC_A_Mcs07_Mcs04,
							rTxAGC_A_Mcs11_Mcs08, rTxAGC_A_Mcs15_Mcs12};
	u16 RegOffset_B[6] = {	rTxAGC_B_Rate18_06, rTxAGC_B_Rate54_24,
							rTxAGC_B_Mcs03_Mcs00, rTxAGC_B_Mcs07_Mcs04,
							rTxAGC_B_Mcs11_Mcs08, rTxAGC_B_Mcs15_Mcs12};
	u8 i, rf, pwr_val[4];
	u32 writeVal;
	u16 RegOffset;

	for(rf=0; rf<2; rf++)
	{
		writeVal = pValue[rf];
		for(i=0; i<4; i++)
		{
			pwr_val[i] = (u8)((writeVal & (0x7f<<(i*8)))>>(i*8));
			if (pwr_val[i]  > RF6052_MAX_TX_PWR)
				pwr_val[i]  = RF6052_MAX_TX_PWR;
		}
		writeVal = (pwr_val[3]<<24) | (pwr_val[2]<<16) |(pwr_val[1]<<8) |pwr_val[0];

		if(rf == 0)
			RegOffset = RegOffset_A[index];
		else
			RegOffset = RegOffset_B[index];

		PHY_SetBBReg(Adapter, RegOffset, bMaskDWord, writeVal);

		if(((pHalData->rf_type == RF_2T2R) &&
				(RegOffset == rTxAGC_A_Mcs15_Mcs12 || RegOffset == rTxAGC_B_Mcs15_Mcs12))||
		     ((pHalData->rf_type != RF_2T2R) &&
				(RegOffset == rTxAGC_A_Mcs07_Mcs04 || RegOffset == rTxAGC_B_Mcs07_Mcs04))	)
		{
			writeVal = pwr_val[3];
			if(RegOffset == rTxAGC_A_Mcs15_Mcs12 || RegOffset == rTxAGC_A_Mcs07_Mcs04)
				RegOffset = 0xc90;
			if(RegOffset == rTxAGC_B_Mcs15_Mcs12 || RegOffset == rTxAGC_B_Mcs07_Mcs04)
				RegOffset = 0xc98;
			for(i=0; i<3; i++)
			{
				if(i!=2)
					writeVal = (writeVal>8)?(writeVal-8):0;
				else
					writeVal = (writeVal>6)?(writeVal-6):0;
				rtw_write8(Adapter, (u32)(RegOffset+i), (u8)writeVal);
			}
		}
	}
}


/*-----------------------------------------------------------------------------
 * Function:	PHY_RF6052SetOFDMTxPower
 *
 * Overview:	For legacy and HY OFDM, we must read EEPROM TX power index for
 *			different channel and read original value in TX power register area from
 *			0xe00. We increase offset and original value to be correct tx pwr.
 *
 * Input:       NONE
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 * When			Who		Remark
 * 11/05/2008 	MHC		Simulate 8192 series method.
 * 01/06/2009	MHC		1. Prevent Path B tx power overflow or underflow dure to
 *						A/B pwr difference or legacy/HT pwr diff.
 *						2. We concern with path B legacy/HT OFDM difference.
 * 01/22/2009	MHC		Support new EPRO format from SD3.
 *
 *---------------------------------------------------------------------------*/

VOID 
rtl8188e_PHY_RF6052SetOFDMTxPower(
	IN	PADAPTER	Adapter,
	IN	u8*		pPowerLevelOFDM,
	IN	u8*		pPowerLevelBW20,
	IN	u8*		pPowerLevelBW40,	
	IN	u8		Channel)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32 writeVal[2], powerBase0[2], powerBase1[2], pwrtrac_value;
	u8 direction;
	u8 index = 0;	
	


	getPowerBase88E(Adapter, pPowerLevelOFDM,pPowerLevelBW20,pPowerLevelBW40, Channel, &powerBase0[0], &powerBase1[0]);

	ODM_TxPwrTrackAdjust88E(&pHalData->odmpriv, 0, &direction, &pwrtrac_value);
	
	for(index=0; index<6; index++)
	{
		getTxPowerWriteValByRegulatory88E(Adapter, Channel, index, 
			&powerBase0[0], &powerBase1[0], &writeVal[0]);

		if (direction == 1)
		{
			writeVal[0] += pwrtrac_value;
			writeVal[1] += pwrtrac_value;
		}
		else if (direction == 2)
		{
			writeVal[0] -= pwrtrac_value;
			writeVal[1] -= pwrtrac_value;
		}
		
		writeOFDMPowerReg88E(Adapter, index, &writeVal[0]);
	}
}


static VOID
phy_RF6052_Config_HardCode(
	IN	PADAPTER		Adapter
	)
{



}

static int
phy_RF6052_Config_ParaFile(
	IN	PADAPTER		Adapter
	)
{
	u32					u4RegValue;
	u8					eRFPath;
	BB_REGISTER_DEFINITION_T	*pPhyReg;

	int					rtStatus = _SUCCESS;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);

	static char			sz88eRadioAFile[] = RTL8188E_PHY_RADIO_A;
	static char			sz88eRadioBFile[] = RTL8188E_PHY_RADIO_B;
	char					*pszRadioAFile, *pszRadioBFile;



	pszRadioAFile = sz88eRadioAFile;
	pszRadioBFile = sz88eRadioBFile;


	for(eRFPath = 0; eRFPath <pHalData->NumTotalRFPath; eRFPath++)
	{

		pPhyReg = &pHalData->PHYRegDef[eRFPath];

		/*----Store original RFENV control type----*/
		switch(eRFPath)
		{
		case RF_PATH_A:
		case RF_PATH_C:
			u4RegValue = PHY_QueryBBReg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV);
			break;
		case RF_PATH_B :
		case RF_PATH_D:
			u4RegValue = PHY_QueryBBReg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV<<16);
			break;
		}

		/*----Set RF_ENV enable----*/
		PHY_SetBBReg(Adapter, pPhyReg->rfintfe, bRFSI_RFENV<<16, 0x1);
		rtw_udelay_os(1);//PlatformStallExecution(1);

		/*----Set RF_ENV output high----*/
		PHY_SetBBReg(Adapter, pPhyReg->rfintfo, bRFSI_RFENV, 0x1);
		rtw_udelay_os(1);//PlatformStallExecution(1);

		/* Set bit number of Address and Data for RF register */
		PHY_SetBBReg(Adapter, pPhyReg->rfHSSIPara2, b3WireAddressLength, 0x0); 
		rtw_udelay_os(1);//PlatformStallExecution(1);

		PHY_SetBBReg(Adapter, pPhyReg->rfHSSIPara2, b3WireDataLength, 0x0);
		rtw_udelay_os(1);//PlatformStallExecution(1);

		/*----Initialize RF fom connfiguration file----*/
		switch(eRFPath)
		{
		case RF_PATH_A:
#ifdef CONFIG_EMBEDDED_FWIMG
			#ifdef CONFIG_PHY_SETTING_WITH_ODM
			if(HAL_STATUS_FAILURE ==ODM_ConfigRFWithHeaderFile(&pHalData->odmpriv,(ODM_RF_RADIO_PATH_E)eRFPath, (ODM_RF_RADIO_PATH_E)eRFPath))
				rtStatus= _FAIL;
			#else
				rtStatus= rtl8188e_PHY_ConfigRFWithHeaderFile(Adapter,(RF_RADIO_PATH_E)eRFPath);
			#endif//#ifdef CONFIG_PHY_SETTING_WITH_ODM
#else
			rtStatus = rtl8188e_PHY_ConfigRFWithParaFile(Adapter, pszRadioAFile, (RF_RADIO_PATH_E)eRFPath);
#endif//#ifdef CONFIG_EMBEDDED_FWIMG
			break;
		case RF_PATH_B:
#ifdef CONFIG_EMBEDDED_FWIMG
			#ifdef CONFIG_PHY_SETTING_WITH_ODM
			if(HAL_STATUS_FAILURE ==ODM_ConfigRFWithHeaderFile(&pHalData->odmpriv,(ODM_RF_RADIO_PATH_E)eRFPath, (ODM_RF_RADIO_PATH_E)eRFPath))
				rtStatus= _FAIL;	
			#else
				rtStatus = rtl8188e_PHY_ConfigRFWithHeaderFile(Adapter,(RF_RADIO_PATH_E)eRFPath);
			#endif
#else
			rtStatus =rtl8188e_PHY_ConfigRFWithParaFile(Adapter, pszRadioBFile, (RF_RADIO_PATH_E)eRFPath);
#endif
			break;
		case RF_PATH_C:
			break;
		case RF_PATH_D:
			break;
		}

		/*----Restore RFENV control type----*/;
		switch(eRFPath)
		{
		case RF_PATH_A:
		case RF_PATH_C:
			PHY_SetBBReg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV, u4RegValue);
			break;
		case RF_PATH_B :
		case RF_PATH_D:
			PHY_SetBBReg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV<<16, u4RegValue);
			break;
		}

		if(rtStatus != _SUCCESS){
			goto phy_RF6052_Config_ParaFile_Fail;
		}

	}

	return rtStatus;

phy_RF6052_Config_ParaFile_Fail:
	return rtStatus;
}


int
PHY_RF6052_Config8188E(
	IN	PADAPTER		Adapter)
{
	HAL_DATA_TYPE				*pHalData = GET_HAL_DATA(Adapter);
	int					rtStatus = _SUCCESS;

	if(pHalData->rf_type == RF_1T1R)
		pHalData->NumTotalRFPath = 1;
	else
		pHalData->NumTotalRFPath = 2;

	rtStatus = phy_RF6052_Config_ParaFile(Adapter);
#if 0
	switch( Adapter->MgntInfo.bRegHwParaFile )
	{
		case 0:
			phy_RF6052_Config_HardCode(Adapter);
			break;

		case 1:
			rtStatus = phy_RF6052_Config_ParaFile(Adapter);
			break;

		case 2:
			phy_RF6052_Config_HardCode(Adapter);
			phy_RF6052_Config_ParaFile(Adapter);
			break;

		default:
			phy_RF6052_Config_HardCode(Adapter);
			break;
	}
#endif
	return rtStatus;

}


/*-----------------------------------------------------------------------------
 * Function:	PHY_RFShadowRead
 *				PHY_RFShadowWrite
 *				PHY_RFShadowCompare
 *				PHY_RFShadowRecorver
 *				PHY_RFShadowCompareAll
 *				PHY_RFShadowRecorverAll
 *				PHY_RFShadowCompareFlagSet
 *				PHY_RFShadowRecorverFlagSet
 *
 * Overview:	When we set RF register, we must write shadow at first.
 *			When we are running, we must compare shadow abd locate error addr.
 *			Decide to recorver or not.
 *
 * Input:       NONE
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 * When			Who		Remark
 * 11/20/2008 	MHC		Create Version 0.
 *
 *---------------------------------------------------------------------------*/
u32
PHY_RFShadowRead(
	IN	PADAPTER			Adapter,
	IN	RF_RADIO_PATH_E	eRFPath,
	IN	u32				Offset)
{
	return	RF_Shadow[eRFPath][Offset].Value;

}	/* PHY_RFShadowRead */


VOID
PHY_RFShadowWrite(
	IN	PADAPTER			Adapter,
	IN	RF_RADIO_PATH_E	eRFPath,
	IN	u32				Offset,
	IN	u32				Data)
{
	RF_Shadow[eRFPath][Offset].Value = (Data & bRFRegOffsetMask);
	RF_Shadow[eRFPath][Offset].Driver_Write = _TRUE;

}	/* PHY_RFShadowWrite */


BOOLEAN
PHY_RFShadowCompare(
	IN	PADAPTER			Adapter,
	IN	RF_RADIO_PATH_E	eRFPath,
	IN	u32				Offset)
{
	u32	reg;
	if (RF_Shadow[eRFPath][Offset].Compare == _TRUE)
	{
		reg = PHY_QueryRFReg(Adapter, eRFPath, Offset, bRFRegOffsetMask);
		if (RF_Shadow[eRFPath][Offset].Value != reg)
		{
			RF_Shadow[eRFPath][Offset].ErrorOrNot = _TRUE;
		}
		return RF_Shadow[eRFPath][Offset].ErrorOrNot ;
	}
	return _FALSE;
}	/* PHY_RFShadowCompare */


VOID
PHY_RFShadowRecorver(
	IN	PADAPTER			Adapter,
	IN	RF_RADIO_PATH_E	eRFPath,
	IN	u32				Offset)
{
	if (RF_Shadow[eRFPath][Offset].ErrorOrNot == _TRUE)
	{
		if (RF_Shadow[eRFPath][Offset].Recorver == _TRUE)
		{
			PHY_SetRFReg(Adapter, eRFPath, Offset, bRFRegOffsetMask,
							RF_Shadow[eRFPath][Offset].Value);
		}
	}

}	/* PHY_RFShadowRecorver */


VOID
PHY_RFShadowCompareAll(
	IN	PADAPTER			Adapter)
{
	u32		eRFPath;
	u32		Offset;

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++)
	{
		for (Offset = 0; Offset <= RF6052_MAX_REG; Offset++)
		{
			PHY_RFShadowCompare(Adapter, (RF_RADIO_PATH_E)eRFPath, Offset);
		}
	}

}	/* PHY_RFShadowCompareAll */


VOID
PHY_RFShadowRecorverAll(
	IN	PADAPTER			Adapter)
{
	u32		eRFPath;
	u32		Offset;

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++)
	{
		for (Offset = 0; Offset <= RF6052_MAX_REG; Offset++)
		{
			PHY_RFShadowRecorver(Adapter, (RF_RADIO_PATH_E)eRFPath, Offset);
		}
	}

}	/* PHY_RFShadowRecorverAll */


VOID
PHY_RFShadowCompareFlagSet(
	IN	PADAPTER			Adapter,
	IN	RF_RADIO_PATH_E	eRFPath,
	IN	u32				Offset,
	IN	u8				Type)
{
	RF_Shadow[eRFPath][Offset].Compare = Type;

}	/* PHY_RFShadowCompareFlagSet */


VOID
PHY_RFShadowRecorverFlagSet(
	IN	PADAPTER			Adapter,
	IN	RF_RADIO_PATH_E	eRFPath,
	IN	u32				Offset,
	IN	u8				Type)
{
	RF_Shadow[eRFPath][Offset].Recorver= Type;

}	/* PHY_RFShadowRecorverFlagSet */


VOID
PHY_RFShadowCompareFlagSetAll(
	IN	PADAPTER			Adapter)
{
	u32		eRFPath;
	u32		Offset;

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++)
	{
		for (Offset = 0; Offset <= RF6052_MAX_REG; Offset++)
		{
			if (Offset != 0x26 && Offset != 0x27)
				PHY_RFShadowCompareFlagSet(Adapter, (RF_RADIO_PATH_E)eRFPath, Offset, _FALSE);
			else
				PHY_RFShadowCompareFlagSet(Adapter, (RF_RADIO_PATH_E)eRFPath, Offset, _TRUE);
		}
	}

}	/* PHY_RFShadowCompareFlagSetAll */


VOID
PHY_RFShadowRecorverFlagSetAll(
	IN	PADAPTER			Adapter)
{
	u32		eRFPath;
	u32		Offset;

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++)
	{
		for (Offset = 0; Offset <= RF6052_MAX_REG; Offset++)
		{
			if (Offset != 0x26 && Offset != 0x27)
				PHY_RFShadowRecorverFlagSet(Adapter, (RF_RADIO_PATH_E)eRFPath, Offset, _FALSE);
			else
				PHY_RFShadowRecorverFlagSet(Adapter, (RF_RADIO_PATH_E)eRFPath, Offset, _TRUE);
		}
	}

}	/* PHY_RFShadowCompareFlagSetAll */

VOID
PHY_RFShadowRefresh(
	IN	PADAPTER			Adapter)
{
	u32		eRFPath;
	u32		Offset;

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++)
	{
		for (Offset = 0; Offset <= RF6052_MAX_REG; Offset++)
		{
			RF_Shadow[eRFPath][Offset].Value = 0;
			RF_Shadow[eRFPath][Offset].Compare = _FALSE;
			RF_Shadow[eRFPath][Offset].Recorver  = _FALSE;
			RF_Shadow[eRFPath][Offset].ErrorOrNot = _FALSE;
			RF_Shadow[eRFPath][Offset].Driver_Write = _FALSE;
		}
	}

}	/* PHY_RFShadowRead */

/* End of HalRf6052.c */

