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



#include "odm_precomp.h"

#define READ_AND_CONFIG_MP(ic, txt) (ODM_ReadAndConfig_MP_##ic##txt(pDM_Odm))
#define READ_AND_CONFIG_TC(ic, txt) (ODM_ReadAndConfig_TC_##ic##txt(pDM_Odm))


#if (TESTCHIP_SUPPORT == 1)
#define READ_AND_CONFIG(ic, txt) do {\
                                            if (pDM_Odm->bIsMPChip)\
                                    		    READ_AND_CONFIG_MP(ic,txt);\
                                            else\
                                                READ_AND_CONFIG_TC(ic,txt);\
                                    } while(0)
#else
  #define READ_AND_CONFIG     READ_AND_CONFIG_MP
#endif


#define READ_FIRMWARE_MP(ic, txt) 		(ODM_ReadFirmware_MP_##ic##txt(pDM_Odm, pFirmware, pSize))
#define READ_FIRMWARE_TC(ic, txt) 		(ODM_ReadFirmware_TC_##ic##txt(pDM_Odm, pFirmware, pSize))		

#if (TESTCHIP_SUPPORT == 1)
#define READ_FIRMWARE(ic, txt) do {\
						if (pDM_Odm->bIsMPChip)\
							READ_FIRMWARE_MP(ic,txt);\
						else\
							READ_FIRMWARE_TC(ic,txt);\
					} while(0) 
#else
#define READ_FIRMWARE     READ_FIRMWARE_MP
#endif
						
#define GET_VERSION_MP(ic, txt) 		(ODM_GetVersion_MP_##ic##txt())
#define GET_VERSION_TC(ic, txt) 		(ODM_GetVersion_TC_##ic##txt())
	
#define GET_VERSION(ic, txt) do {\
							if (pDM_Odm->bIsMPChip)\
								GET_VERSION_MP(ic,txt);\
							else\
								GET_VERSION_TC(ic,txt);\
						} while(0)


u1Byte
odm_QueryRxPwrPercentage(
	IN		s1Byte		AntPower
	)
{
	if ((AntPower <= -100) || (AntPower >= 20))
	{
		return	0;
	}
	else if (AntPower >= 0)
	{
		return	100;
	}
	else
	{
		return	(100+AntPower);
	}
	
}

#if (DM_ODM_SUPPORT_TYPE != ODM_WIN)
s4Byte
odm_SignalScaleMapping_92CSeries_patch_RT_CID_819x_Lenovo(	
	IN OUT PDM_ODM_T pDM_Odm,
	s4Byte CurrSig 
)
{	
	s4Byte RetSig = 0;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	{
		if(CurrSig >= 54 && CurrSig <= 100)
		{
			RetSig = 100;
		}
		else if(CurrSig>=42 && CurrSig <= 53 )
		{
			RetSig = 95;
		}
		else if(CurrSig>=36 && CurrSig <= 41 )
		{
			RetSig = 74 + ((CurrSig - 36) *20)/6;
		}
		else if(CurrSig>=33 && CurrSig <= 35 )
		{
			RetSig = 65 + ((CurrSig - 33) *8)/2;
		}
		else if(CurrSig>=18 && CurrSig <= 32 )
		{
			RetSig = 62 + ((CurrSig - 18) *2)/15;
		}
		else if(CurrSig>=15 && CurrSig <= 17 )
		{
			RetSig = 33 + ((CurrSig - 15) *28)/2;
		}
		else if(CurrSig>=10 && CurrSig <= 14 )
		{
			RetSig = 39;
		}
		else if(CurrSig>=8 && CurrSig <= 9 )
		{
			RetSig = 33;
		}
		else if(CurrSig <= 8 )
		{
			RetSig = 19;
		}
	}
#endif
	return RetSig;
}

s4Byte
odm_SignalScaleMapping_92CSeries_patch_RT_CID_819x_Netcore(	
	IN OUT PDM_ODM_T pDM_Odm,
	s4Byte CurrSig 
)
{
	s4Byte RetSig = 0;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	{
		if(CurrSig >= 31 && CurrSig <= 100)
		{
			RetSig = 100;
		}	
		else if(CurrSig >= 21 && CurrSig <= 30)
		{
			RetSig = 90 + ((CurrSig - 20) / 1);
		}
		else if(CurrSig >= 11 && CurrSig <= 20)
		{
			RetSig = 80 + ((CurrSig - 10) / 1);
		}
		else if(CurrSig >= 7 && CurrSig <= 10)
		{
			RetSig = 69 + (CurrSig - 7);
		}
		else if(CurrSig == 6)
		{
			RetSig = 54;
		}
		else if(CurrSig == 5)
		{
			RetSig = 45;
		}
		else if(CurrSig == 4)
		{
			RetSig = 36;
		}
		else if(CurrSig == 3)
		{
			RetSig = 27;
		}
		else if(CurrSig == 2)
		{
			RetSig = 18;
		}
		else if(CurrSig == 1)
		{
			RetSig = 9;
		}
		else
		{
			RetSig = CurrSig;
		}
	}
#endif
	return RetSig;
}


s4Byte
odm_SignalScaleMapping_92CSeries(	
	IN OUT PDM_ODM_T pDM_Odm,
	IN s4Byte CurrSig 
)
{
	s4Byte RetSig = 0; 
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE) 
	if(pDM_Odm->SupportInterface  == ODM_ITRF_PCIE) 
	{
		if(CurrSig >= 61 && CurrSig <= 100)
		{
			RetSig = 90 + ((CurrSig - 60) / 4);
		}
		else if(CurrSig >= 41 && CurrSig <= 60)
		{
			RetSig = 78 + ((CurrSig - 40) / 2);
		}
		else if(CurrSig >= 31 && CurrSig <= 40)
		{
			RetSig = 66 + (CurrSig - 30);
		}
		else if(CurrSig >= 21 && CurrSig <= 30)
		{
			RetSig = 54 + (CurrSig - 20);
		}
		else if(CurrSig >= 5 && CurrSig <= 20)
		{
			RetSig = 42 + (((CurrSig - 5) * 2) / 3);
		}
		else if(CurrSig == 4)
		{
			RetSig = 36;
		}
		else if(CurrSig == 3)
		{
			RetSig = 27;
		}
		else if(CurrSig == 2)
		{
			RetSig = 18;
		}
		else if(CurrSig == 1)
		{
			RetSig = 9;
		}
		else
		{
			RetSig = CurrSig;
		}
	}
#endif

#if ((DEV_BUS_TYPE == RT_USB_INTERFACE) ||(DEV_BUS_TYPE == RT_SDIO_INTERFACE))
	if((pDM_Odm->SupportInterface  == ODM_ITRF_USB) || (pDM_Odm->SupportInterface  == ODM_ITRF_SDIO))
	{
		if(CurrSig >= 51 && CurrSig <= 100)
		{
			RetSig = 100;
		}
		else if(CurrSig >= 41 && CurrSig <= 50)
		{
			RetSig = 80 + ((CurrSig - 40)*2);
		}
		else if(CurrSig >= 31 && CurrSig <= 40)
		{
			RetSig = 66 + (CurrSig - 30);
		}
		else if(CurrSig >= 21 && CurrSig <= 30)
		{
			RetSig = 54 + (CurrSig - 20);
		}
		else if(CurrSig >= 10 && CurrSig <= 20)
		{
			RetSig = 42 + (((CurrSig - 10) * 2) / 3);
		}
		else if(CurrSig >= 5 && CurrSig <= 9)
		{
			RetSig = 22 + (((CurrSig - 5) * 3) / 2);
		}
		else if(CurrSig >= 1 && CurrSig <= 4)
		{
			RetSig = 6 + (((CurrSig - 1) * 3) / 2);
		}
		else
		{
			RetSig = CurrSig;
		}
	}

#endif
	return RetSig;
}
s4Byte
odm_SignalScaleMapping(	
	IN OUT PDM_ODM_T pDM_Odm,
	IN	s4Byte CurrSig 
)
{	
	if(	(pDM_Odm->SupportPlatform == ODM_WIN) && 
		(pDM_Odm->SupportInterface  != ODM_ITRF_PCIE) &&
		(pDM_Odm->PatchID==10))//pMgntInfo->CustomerID == RT_CID_819x_Netcore
	{
		return odm_SignalScaleMapping_92CSeries_patch_RT_CID_819x_Netcore(pDM_Odm,CurrSig);
	}
	else if(	(pDM_Odm->SupportPlatform == ODM_WIN) && 
			(pDM_Odm->SupportInterface  == ODM_ITRF_PCIE) &&
			(pDM_Odm->PatchID==19))//pMgntInfo->CustomerID == RT_CID_819x_Lenovo)
	{
		return odm_SignalScaleMapping_92CSeries_patch_RT_CID_819x_Lenovo(pDM_Odm, CurrSig);
	}
	else{		
		return odm_SignalScaleMapping_92CSeries(pDM_Odm,CurrSig);
	}
	
}
#endif


static u1Byte odm_SQ_process_patch_RT_CID_819x_Lenovo(
	IN PDM_ODM_T	pDM_Odm,
	IN u1Byte 		isCCKrate,
	IN u1Byte 		PWDB_ALL,
	IN u1Byte 		path,
	IN u1Byte 		RSSI
)
{
	u1Byte	SQ = 0;
#if (DM_ODM_SUPPORT_TYPE &  ODM_WIN)			

	if(isCCKrate){
		
		if(IS_HARDWARE_TYPE_8723AE(pDM_Odm->Adapter))
		{

			RT_TRACE(COMP_DBG, DBG_WARNING, ("odm_SQ_process_patch_RT_CID_819x_Lenovo\n"));
			
#if OS_WIN_FROM_WIN8(OS_VERSION)	
			if(PWDB_ALL >= 50)
				SQ = 100;
			else if(PWDB_ALL >= 23 && PWDB_ALL < 50)				
				SQ = 80;
			else if(PWDB_ALL >= 18 && PWDB_ALL < 23)
				SQ = 60;
			else if(PWDB_ALL >= 8 && PWDB_ALL < 18)
				SQ = 40;
			else
				SQ = 10;
#else
			if(PWDB_ALL >= 34)
				SQ = 100;
			else if(PWDB_ALL >= 23 && PWDB_ALL < 34)				
				SQ = 80;
			else if(PWDB_ALL >= 18 && PWDB_ALL < 23)
				SQ = 60;
			else if(PWDB_ALL >= 8 && PWDB_ALL < 18)
				SQ = 40;
			else
				SQ = 10;	

			if(PWDB_ALL == 0)// Abnormal case, do not indicate the value above 20 on Win7
				SQ = 20;
#endif		

		}
		else if(IS_HARDWARE_TYPE_8192E(pDM_Odm->Adapter)){


			if(PWDB_ALL >= 50)
				SQ = 100;
			else if(PWDB_ALL >= 35 && PWDB_ALL < 50)				
				SQ = 80;
			else if(PWDB_ALL >= 31 && PWDB_ALL < 35)
				SQ = 60;
			else if(PWDB_ALL >= 22 && PWDB_ALL < 31)
				SQ = 40;
			else if(PWDB_ALL >= 18 && PWDB_ALL < 22)
				SQ = 20;
			else
				SQ = 10;
		}
		else
		{
		if(PWDB_ALL >= 50)
			SQ = 100;
		else if(PWDB_ALL >= 35 && PWDB_ALL < 50)				
			SQ = 80;
		else if(PWDB_ALL >= 22 && PWDB_ALL < 35)
			SQ = 60;
		else if(PWDB_ALL >= 18 && PWDB_ALL < 22)
			SQ = 40;
		else
				SQ = 10;
		}
		
	}
	else
	{//OFDM rate		

		if(IS_HARDWARE_TYPE_8723AE(pDM_Odm->Adapter) ||
			IS_HARDWARE_TYPE_8192E(pDM_Odm->Adapter))
		{
			if(RSSI >= 45)
				SQ = 100;
			else if(RSSI >= 22 && RSSI < 45)
				SQ = 80;
			else if(RSSI >= 18 && RSSI < 22)
				SQ = 40;
			else
			SQ = 20;
	}
		else
		{
			if(RSSI >= 45)
			SQ = 100;
			else if(RSSI >= 22 && RSSI < 45)
			SQ = 80;
		else if(RSSI >= 18 && RSSI < 22)
			SQ = 40;
		else
			SQ = 20;			
	}
	}

	RT_TRACE(COMP_DBG, DBG_TRACE, ("isCCKrate(%#d), PWDB_ALL(%#d), RSSI(%#d), SQ(%#d)\n", isCCKrate, PWDB_ALL, RSSI, SQ));
	
#endif
	return SQ;
}

static u1Byte odm_SQ_process_patch_RT_CID_819x_Acer(
	IN PDM_ODM_T	pDM_Odm,
	IN u1Byte 		isCCKrate,
	IN u1Byte 		PWDB_ALL,
	IN u1Byte 		path,
	IN u1Byte 		RSSI
)
{
	u1Byte	SQ = 0;
	
#if (DM_ODM_SUPPORT_TYPE &  ODM_WIN)			

	if(isCCKrate){

			RT_TRACE(COMP_DBG, DBG_WARNING, ("odm_SQ_process_patch_RT_Acer\n"));
			
#if OS_WIN_FROM_WIN8(OS_VERSION)	

			if(PWDB_ALL >= 50)
				SQ = 100;
			else if(PWDB_ALL >= 35 && PWDB_ALL < 50)				
				SQ = 80;
			else if(PWDB_ALL >= 30 && PWDB_ALL < 35)
				SQ = 60;
			else if(PWDB_ALL >= 25 && PWDB_ALL < 30)
				SQ = 40;
			else if(PWDB_ALL >= 20 && PWDB_ALL < 25)
				SQ = 20;
			else
				SQ = 10;	
#else
			if(PWDB_ALL >= 50)
				SQ = 100;
			else if(PWDB_ALL >= 35 && PWDB_ALL < 50)				
				SQ = 80;
			else if(PWDB_ALL >= 30 && PWDB_ALL < 35)
				SQ = 60;
			else if(PWDB_ALL >= 25 && PWDB_ALL < 30)
				SQ = 40;
			else if(PWDB_ALL >= 20 && PWDB_ALL < 25)
				SQ = 20;
			else
				SQ = 10;	

			if(PWDB_ALL == 0)// Abnormal case, do not indicate the value above 20 on Win7
				SQ = 20;
#endif		

		
		
	}
	else
	{//OFDM rate		

		if(IS_HARDWARE_TYPE_8723AE(pDM_Odm->Adapter) ||
			IS_HARDWARE_TYPE_8192E(pDM_Odm->Adapter))
		{
			if(RSSI >= 45)
				SQ = 100;
			else if(RSSI >= 22 && RSSI < 45)
				SQ = 80;
			else if(RSSI >= 18 && RSSI < 22)
				SQ = 40;
			else
			SQ = 20;
	}
		else
		{
			if(RSSI >= 35)
			SQ = 100;
			else if(RSSI >= 30 && RSSI < 35)
			SQ = 80;
		else if(RSSI >= 25 && RSSI < 30)
			SQ = 40;
		else
			SQ = 20;			
	}
	}

	RT_TRACE(COMP_DBG, DBG_LOUD, ("isCCKrate(%#d), PWDB_ALL(%#d), RSSI(%#d), SQ(%#d)\n", isCCKrate, PWDB_ALL, RSSI, SQ));
	
#endif
	return SQ;
}
			
static u1Byte 
odm_EVMdbToPercentage(
    IN		s1Byte Value
    )
{
	s1Byte ret_val;
    
	ret_val = Value;
	ret_val /= 2;

		
	if(ret_val >= 0)
		ret_val = 0;
	if(ret_val <= -33)
		ret_val = -33;

	ret_val = 0 - ret_val;
	ret_val*=3;

	if(ret_val == 99)
		ret_val = 100;

	return(ret_val);
}
			
static u1Byte 
odm_EVMdbm_JaguarSeries(
	IN  s1Byte Value
	)
{
	s1Byte ret_val = Value;
	
	if(ret_val == -128)
		ret_val = 127;
	else if (ret_val < 0)
		ret_val = 0 - ret_val;
	
	ret_val  = ret_val >> 1;
	return ret_val;
}

static u2Byte
odm_Cfo(
  IN s1Byte Value
)
{
	s2Byte  ret_val;

	if (Value < 0)
	{
		ret_val = 0 - Value;
		ret_val = (ret_val << 1) + (ret_val >> 1) ; 
		ret_val = ret_val | BIT12; 
	}
	else
	{
		ret_val = Value;
		ret_val = (ret_val << 1) + (ret_val>>1) ; 
	}
	return ret_val;
}


VOID
odm_RxPhyStatus92CSeries_Parsing(
	IN OUT	PDM_ODM_T					pDM_Odm,
	OUT		PODM_PHY_INFO_T			pPhyInfo,		
	IN 		pu1Byte						pPhyStatus,
	IN		PODM_PACKET_INFO_T			pPktinfo
	)
{							
	SWAT_T				*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	u1Byte				i, Max_spatial_stream;
	s1Byte				rx_pwr[4], rx_pwr_all=0;
	u1Byte				EVM, PWDB_ALL = 0, PWDB_ALL_BT;
	u1Byte				RSSI, total_rssi=0;
	BOOLEAN				isCCKrate=FALSE;	
	u1Byte				rf_rx_num = 0;
	u1Byte				cck_highpwr = 0;
	u1Byte				LNA_idx, VGA_idx;
	PPHY_STATUS_RPT_8192CD_T pPhyStaRpt = (PPHY_STATUS_RPT_8192CD_T)pPhyStatus;

	isCCKrate = (pPktinfo->DataRate <= DESC_RATE11M)?TRUE :FALSE;
	pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_A] = -1;
	pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_B] = -1;


	if(isCCKrate)
	{
		u1Byte report;
		u1Byte cck_agc_rpt;
		
		pDM_Odm->PhyDbgInfo.NumQryPhyStatusCCK++;

			cck_highpwr = pDM_Odm->bCckHighPower;

		cck_agc_rpt =  pPhyStaRpt->cck_agc_rpt_ofdm_cfosho_a ;
		
		if(pDM_Odm->SupportICType & (ODM_RTL8188E|ODM_RTL8192E|ODM_RTL8723B))
		{
			LNA_idx = ((cck_agc_rpt & 0xE0) >>5);
			VGA_idx = (cck_agc_rpt & 0x1F); 
			if(pDM_Odm->SupportICType & (ODM_RTL8188E|ODM_RTL8192E))
			{
				switch(LNA_idx)
				{
					case 7:
						if(VGA_idx <= 27)
							rx_pwr_all = -100 + 2*(27-VGA_idx);
						else
							rx_pwr_all = -100;
						break;
					case 6:
							rx_pwr_all = -48 + 2*(2-VGA_idx);
						break;
					case 5:
							rx_pwr_all = -42 + 2*(7-VGA_idx);
						break;
					case 4:
							rx_pwr_all = -36 + 2*(7-VGA_idx);
						break;
					case 3:
							rx_pwr_all = -24 + 2*(7-VGA_idx);
						break;
					case 2:
						if(cck_highpwr)
							rx_pwr_all = -12 + 2*(5-VGA_idx);
						else
							rx_pwr_all = -6+ 2*(5-VGA_idx);
						break;
					case 1:
							rx_pwr_all = 8-2*VGA_idx;
						break;
					case 0:
							rx_pwr_all = 14-2*VGA_idx;
						break;
					default:
						break;
				}
				rx_pwr_all += 6;

				if(pDM_Odm->SupportICType == ODM_RTL8192E)
					rx_pwr_all += 10;
				
				PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);
				if(cck_highpwr == FALSE)
				{
					if(PWDB_ALL >= 80)
						PWDB_ALL = ((PWDB_ALL-80)<<1)+((PWDB_ALL-80)>>1)+80;
					else if((PWDB_ALL <= 78) && (PWDB_ALL >= 20))
						PWDB_ALL += 3;
					if(PWDB_ALL>100)
						PWDB_ALL = 100;
				}
			}
			else if(pDM_Odm->SupportICType & (ODM_RTL8723B))
			{
#if (RTL8723B_SUPPORT == 1)			
				rx_pwr_all = odm_CCKRSSI_8723B(LNA_idx,VGA_idx);
				PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);
				if(PWDB_ALL>100)
					PWDB_ALL = 100;	
#endif				
			}
		}		
		else
		{
			if(!cck_highpwr)
			{			
				report =( cck_agc_rpt & 0xc0 )>>6;
				switch(report)
				{
					case 0x3:
						rx_pwr_all = -46 - (cck_agc_rpt & 0x3e);
						break;
					case 0x2:
						rx_pwr_all = -26 - (cck_agc_rpt & 0x3e);
						break;
					case 0x1:
						rx_pwr_all = -12 - (cck_agc_rpt & 0x3e);
						break;
					case 0x0:
						rx_pwr_all = 16 - (cck_agc_rpt & 0x3e);
						break;
				}
			}
			else
			{
				
				report = (cck_agc_rpt & 0x60)>>5;
				switch(report)
				{
					case 0x3:
						rx_pwr_all = -46 - ((cck_agc_rpt & 0x1f)<<1) ;
						break;
					case 0x2:
						rx_pwr_all = -26 - ((cck_agc_rpt & 0x1f)<<1);
						break;
					case 0x1:
						rx_pwr_all = -12 - ((cck_agc_rpt & 0x1f)<<1) ;
						break;
					case 0x0:
						rx_pwr_all = 16 - ((cck_agc_rpt & 0x1f)<<1) ;
						break;
				}
			}

			PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);

			if(pDM_Odm->BoardType & (ODM_BOARD_EXT_LNA | ODM_BOARD_EXT_PA))
			{
				if((cck_agc_rpt>>7) == 0){
					PWDB_ALL = (PWDB_ALL>94)?100:(PWDB_ALL +6);
				}
				else	
	                   {
					if(PWDB_ALL > 38)
						PWDB_ALL -= 16;
					else
						PWDB_ALL = (PWDB_ALL<=16)?(PWDB_ALL>>2):(PWDB_ALL -12);
				}             

				if(PWDB_ALL > 25 && PWDB_ALL <= 60)
					PWDB_ALL += 6;
			}
			else//Modification for int-LNA board
			{
				if(PWDB_ALL > 99)
				  	PWDB_ALL -= 8;
				else if(PWDB_ALL > 50 && PWDB_ALL <= 68)
					PWDB_ALL += 4;
			}
		}
	
		pPhyInfo->RxPWDBAll = PWDB_ALL;
#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))
		pPhyInfo->BTRxRSSIPercentage = PWDB_ALL;
		pPhyInfo->RecvSignalPower = rx_pwr_all;
#endif		
		{
			u1Byte	SQ,SQ_rpt;			
			
			if((pDM_Odm->SupportPlatform == ODM_WIN) &&
				(pDM_Odm->PatchID==RT_CID_819x_Lenovo)){
				SQ = odm_SQ_process_patch_RT_CID_819x_Lenovo(pDM_Odm,isCCKrate,PWDB_ALL,0,0);
			}
			else if((pDM_Odm->SupportPlatform == ODM_WIN) &&
				(pDM_Odm->PatchID==RT_CID_819x_Acer))
			{
				SQ = odm_SQ_process_patch_RT_CID_819x_Acer(pDM_Odm,isCCKrate,PWDB_ALL,0,0);
			}
			else if(pPhyInfo->RxPWDBAll > 40 && !pDM_Odm->bInHctTest){
				SQ = 100;
			}
			else{						
				SQ_rpt = pPhyStaRpt->cck_sig_qual_ofdm_pwdb_all;
					
				if(SQ_rpt > 64)
					SQ = 0;
				else if (SQ_rpt < 20)
					SQ = 100;
				else
					SQ = ((64-SQ_rpt) * 100) / 44;
			
			}
			
			pPhyInfo->SignalQuality = SQ;
			pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_A] = SQ;
			pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_B] = -1;
		}
	}
	else
	{
		pDM_Odm->PhyDbgInfo.NumQryPhyStatusOFDM++;

		
       	 for(i = ODM_RF_PATH_A; i < ODM_RF_PATH_MAX; i++)   
		{
			if (pDM_Odm->RFPathRxEnable & BIT(i))
				rf_rx_num++;

			rx_pwr[i] = ((pPhyStaRpt->path_agc[i].gain& 0x3F)*2) - 110;


		#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
			pPhyInfo->RxPwr[i] = rx_pwr[i];
		#endif	

			/* Translate DBM to percentage. */
			RSSI = odm_QueryRxPwrPercentage(rx_pwr[i]);
			total_rssi += RSSI;

			if(pDM_Odm->SupportICType&ODM_RTL8192C)
			{	
				if(pDM_Odm->BoardType & (ODM_BOARD_EXT_LNA | ODM_BOARD_EXT_PA))
				{
					if((pPhyStaRpt->path_agc[i].trsw) == 1)
						RSSI = (RSSI>94)?100:(RSSI +6);
					else
						RSSI = (RSSI<=16)?(RSSI>>3):(RSSI -16);

					if((RSSI <= 34) && (RSSI >=4))
						RSSI -= 4;
				}		
			}
		
			pPhyInfo->RxMIMOSignalStrength[i] =(u1Byte) RSSI;

		#if (DM_ODM_SUPPORT_TYPE &  (/*ODM_WIN|*/ODM_CE|ODM_AP|ODM_ADSL))
			pPhyInfo->RxSNR[i] = pDM_Odm->PhyDbgInfo.RxSNRdB[i] = (s4Byte)(pPhyStaRpt->path_rxsnr[i]/2);
		#endif
		
			/* Record Signal Strength for next packet */
			{				
				if((pDM_Odm->SupportPlatform == ODM_WIN) &&
					(pDM_Odm->PatchID==RT_CID_819x_Lenovo))
				{
					if(i==ODM_RF_PATH_A)
						pPhyInfo->SignalQuality = odm_SQ_process_patch_RT_CID_819x_Lenovo(pDM_Odm,isCCKrate,PWDB_ALL,i,RSSI);
				
				}		
				else if((pDM_Odm->SupportPlatform == ODM_WIN) &&
					(pDM_Odm->PatchID==RT_CID_819x_Acer))
				{
					pPhyInfo->SignalQuality = odm_SQ_process_patch_RT_CID_819x_Acer(pDM_Odm,isCCKrate,PWDB_ALL,0,RSSI);
				}	
				
			}
		}
		
		
		rx_pwr_all = (((pPhyStaRpt->cck_sig_qual_ofdm_pwdb_all) >> 1 )& 0x7f) -110;		
		
		PWDB_ALL_BT = PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);	
	
		pPhyInfo->RxPWDBAll = PWDB_ALL;
	#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))
		pPhyInfo->BTRxRSSIPercentage = PWDB_ALL_BT;
		pPhyInfo->RxPower = rx_pwr_all;
		pPhyInfo->RecvSignalPower = rx_pwr_all;
	#endif
		
		if((pDM_Odm->SupportPlatform == ODM_WIN) &&(pDM_Odm->PatchID==19)){
		}else if((pDM_Odm->SupportPlatform == ODM_WIN) &&(pDM_Odm->PatchID==25)){
		}
		else{//pMgntInfo->CustomerID != RT_CID_819x_Lenovo
			if(pPktinfo->DataRate >=DESC_RATEMCS8 && pPktinfo->DataRate <=DESC_RATEMCS15)
				Max_spatial_stream = 2;
			else
				Max_spatial_stream = 1;

			for(i=0; i<Max_spatial_stream; i++)
			{
				EVM = odm_EVMdbToPercentage( (pPhyStaRpt->stream_rxevm[i] ));

				
				{
					if(i==ODM_RF_PATH_A)
					{						
						pPhyInfo->SignalQuality = (u1Byte)(EVM & 0xff);
					}					
					pPhyInfo->RxMIMOSignalQuality[i] = (u1Byte)(EVM & 0xff);
				}
			}
		}

		if(pDM_Odm->SupportAbility & ODM_BB_DYNAMIC_ATC)
		{
			if(pPktinfo->bPacketMatchBSSID && ( *(pDM_Odm->mp_mode) == 0))
			{
				
				 for(i = ODM_RF_PATH_A; i < ODM_RF_PATH_MAX; i++)   
				{
					pDM_Odm->CFO_tail[i] = (int)pPhyStaRpt->path_cfotail[i];
			 	}

				if(pDM_Odm->packetCount == 0xffffffff)
					pDM_Odm->packetCount = 0;
				else
				 	pDM_Odm->packetCount++;
				
			}
		}
		
	}
#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))
	if(isCCKrate)
	{		
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		pPhyInfo->SignalStrength = (u1Byte)(SignalScaleMapping(pDM_Odm->Adapter, PWDB_ALL));//PWDB_ALL;
#else
		pPhyInfo->SignalStrength = (u1Byte)(odm_SignalScaleMapping(pDM_Odm, PWDB_ALL));//PWDB_ALL;
#endif
	}
	else
	{	
		if (rf_rx_num != 0)
		{			
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
			pPhyInfo->SignalStrength = (u1Byte)(SignalScaleMapping(pDM_Odm->Adapter, total_rssi/=rf_rx_num));//PWDB_ALL;
#else
			pPhyInfo->SignalStrength = (u1Byte)(odm_SignalScaleMapping(pDM_Odm, total_rssi/=rf_rx_num));
#endif
		}
	}
#endif


#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))	
	pDM_SWAT_Table->antsel = pPhyStaRpt->ant_sel;
	pDM_Odm->DM_FatTable.antsel_rx_keep_0 = pPhyStaRpt->ant_sel;
	pDM_Odm->DM_FatTable.antsel_rx_keep_1 = pPhyStaRpt->ant_sel_b;
	pDM_Odm->DM_FatTable.antsel_rx_keep_2 = pPhyStaRpt->antsel_rx_keep_2;
#endif
}

#if	ODM_IC_11AC_SERIES_SUPPORT

VOID
odm_RxPhyStatusJaguarSeries_Parsing(
	IN OUT	PDM_ODM_T					pDM_Odm,
	OUT		PODM_PHY_INFO_T			pPhyInfo,		
	IN 		pu1Byte						pPhyStatus,
	IN		PODM_PACKET_INFO_T			pPktinfo
	)
{							
	u1Byte				i, Max_spatial_stream;
	s1Byte				rx_pwr[4], rx_pwr_all=0;
	u1Byte				EVM = 0, EVMdbm, PWDB_ALL = 0, PWDB_ALL_BT;
	u1Byte				RSSI, total_rssi=0;
	u1Byte				isCCKrate=0;	
	u1Byte				rf_rx_num = 0;
	u1Byte				cck_highpwr = 0;
	u1Byte				LNA_idx, VGA_idx;

	
	PPHY_STATUS_RPT_8812_T pPhyStaRpt = (PPHY_STATUS_RPT_8812_T)pPhyStatus;	

	if(pPktinfo->DataRate <= DESC_RATE54M)
	{
		switch(pPhyStaRpt->r_RFMOD){
			case 1:
				if(pPhyStaRpt->sub_chnl == 0)
					pPhyInfo->BandWidth = 1;
				else
					pPhyInfo->BandWidth = 0;
				break;

			case 2:
				if(pPhyStaRpt->sub_chnl == 0)
					pPhyInfo->BandWidth = 2;
				else if(pPhyStaRpt->sub_chnl == 9 || pPhyStaRpt->sub_chnl == 10)
					pPhyInfo->BandWidth = 1;
				else 
					pPhyInfo->BandWidth = 0;
				break;

			default:	case 0:
				pPhyInfo->BandWidth = 0;
				break;			
		}	
	}

	if(pPktinfo->DataRate <= DESC_RATE11M)
		isCCKrate = TRUE;
	else
		isCCKrate = FALSE;
	
	pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_A] = -1;
	pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_B] = -1;


	if(isCCKrate)
	{
		u1Byte cck_agc_rpt;
		pDM_Odm->PhyDbgInfo.NumQryPhyStatusCCK++;

			cck_highpwr = pDM_Odm->bCckHighPower;

		cck_agc_rpt =  pPhyStaRpt->cfosho[0] ;
		
		LNA_idx = ((cck_agc_rpt & 0xE0) >>5);
		VGA_idx = (cck_agc_rpt & 0x1F); 
		if(pDM_Odm->SupportICType == ODM_RTL8812)
		{
			switch(LNA_idx)
			{
				case 7:
					if(VGA_idx <= 27)
						rx_pwr_all = -100 + 2*(27-VGA_idx);
					else
						rx_pwr_all = -100;
					break;
				case 6:
						rx_pwr_all = -48 + 2*(2-VGA_idx);
					break;
				case 5:
						rx_pwr_all = -42 + 2*(7-VGA_idx);
					break;
				case 4:
						rx_pwr_all = -36 + 2*(7-VGA_idx);
					break;
				case 3:
						rx_pwr_all = -24 + 2*(7-VGA_idx);
					break;
				case 2:
					if(cck_highpwr)
						rx_pwr_all = -12 + 2*(5-VGA_idx);
					else
						rx_pwr_all = -6+ 2*(5-VGA_idx);
					break;
				case 1:
						rx_pwr_all = 8-2*VGA_idx;
					break;
				case 0:
						rx_pwr_all = 14-2*VGA_idx;
					break;
				default:
					break;
			}
			rx_pwr_all += 6;
			PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);
			if(cck_highpwr == FALSE)
			{
				if(PWDB_ALL >= 80)
					PWDB_ALL = ((PWDB_ALL-80)<<1)+((PWDB_ALL-80)>>1)+80;
				else if((PWDB_ALL <= 78) && (PWDB_ALL >= 20))
					PWDB_ALL += 3;
				if(PWDB_ALL>100)
					PWDB_ALL = 100;
			}
		}
		else if(pDM_Odm->SupportICType == ODM_RTL8821)
		{
			s1Byte Pout = -6;
				
			switch(LNA_idx)
				{
				case 5:
					rx_pwr_all = Pout -32 -(2*VGA_idx);
						break;
				case 4:
					rx_pwr_all = Pout -24 -(2*VGA_idx);
						break;
				case 2:
					rx_pwr_all = Pout -11 -(2*VGA_idx);
						break;
				case 1:
					rx_pwr_all = Pout + 5 -(2*VGA_idx);
						break;
				case 0:
					rx_pwr_all = Pout + 21 -(2*VGA_idx);
						break;
				}
			PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);
		}
	
		pPhyInfo->RxPWDBAll = PWDB_ALL;
#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))
		pPhyInfo->BTRxRSSIPercentage = PWDB_ALL;
		pPhyInfo->RecvSignalPower = rx_pwr_all;
#endif		
		{
			u1Byte	SQ,SQ_rpt;			
			
			if((pDM_Odm->SupportPlatform == ODM_WIN) &&
				(pDM_Odm->PatchID==RT_CID_819x_Lenovo)){
				SQ = odm_SQ_process_patch_RT_CID_819x_Lenovo(pDM_Odm,isCCKrate,PWDB_ALL,0,0);
			}
			else if(pPhyInfo->RxPWDBAll > 40 && !pDM_Odm->bInHctTest){
				SQ = 100;
			}
			else{						
				SQ_rpt = pPhyStaRpt->pwdb_all;
					
				if(SQ_rpt > 64)
					SQ = 0;
				else if (SQ_rpt < 20)
					SQ = 100;
				else
					SQ = ((64-SQ_rpt) * 100) / 44;
			
			}
			
			pPhyInfo->SignalQuality = SQ;
			pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_A] = SQ;
			pPhyInfo->RxMIMOSignalQuality[ODM_RF_PATH_B] = -1;
		}
	}
	else
	{
		pDM_Odm->PhyDbgInfo.NumQryPhyStatusOFDM++;

		
		for(i = ODM_RF_PATH_A; i < ODM_RF_PATH_MAX; i++)   
		{
			if (pDM_Odm->RFPathRxEnable & BIT(i))
			{				
				rf_rx_num++;
			}
				rx_pwr[i] = (pPhyStaRpt->gain_trsw[i]&0x7F) - 110;

		#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
			pPhyInfo->RxPwr[i] = rx_pwr[i];
		#endif	

			/* Translate DBM to percentage. */
			RSSI = odm_QueryRxPwrPercentage(rx_pwr[i]);	
		
			total_rssi += RSSI;


		
			pPhyInfo->RxMIMOSignalStrength[i] =(u1Byte) RSSI;

		#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE|ODM_AP|ODM_ADSL))
			pPhyInfo->RxSNR[i] = pDM_Odm->PhyDbgInfo.RxSNRdB[i] = pPhyStaRpt->rxsnr[i]/2;
		#endif

			pPhyInfo->Cfo_short[i] = odm_Cfo( (pPhyStaRpt->cfosho[i]) );
			pPhyInfo->Cfo_tail[i] = odm_Cfo( (pPhyStaRpt->cfotail[i]) );

			/* Record Signal Strength for next packet */
			{				
				if((pDM_Odm->SupportPlatform == ODM_WIN) &&
					(pDM_Odm->PatchID==RT_CID_819x_Lenovo))
				{
					if(i==ODM_RF_PATH_A)
						pPhyInfo->SignalQuality = odm_SQ_process_patch_RT_CID_819x_Lenovo(pDM_Odm,isCCKrate,PWDB_ALL,i,RSSI);
				
				}
			}
		}
		
		
		if((pDM_Odm->SupportICType & (ODM_RTL8812|ODM_RTL8821)) && (!pDM_Odm->bIsMPChip))
			rx_pwr_all = (pPhyStaRpt->pwdb_all& 0x7f) -110;
		else
			rx_pwr_all = (((pPhyStaRpt->pwdb_all) >> 1 )& 0x7f) -110;	


		PWDB_ALL_BT = PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);	
			
	
		pPhyInfo->RxPWDBAll = PWDB_ALL;
	#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))
		pPhyInfo->BTRxRSSIPercentage = PWDB_ALL_BT;
		pPhyInfo->RxPower = rx_pwr_all;
		pPhyInfo->RecvSignalPower = rx_pwr_all;
	#endif

	
	
		if((pDM_Odm->SupportPlatform == ODM_WIN) &&(pDM_Odm->PatchID==19)){
		}
		else{//pMgntInfo->CustomerID != RT_CID_819x_Lenovo
			if(	(pPktinfo->DataRate>=DESC_RATEMCS8) &&
		 		(pPktinfo->DataRate <=DESC_RATEMCS15))
		 		Max_spatial_stream = 2;
			else if(	(pPktinfo->DataRate>=DESC_RATEVHTSS2MCS0) &&
		 		(pPktinfo->DataRate <=DESC_RATEVHTSS2MCS9))
		 		Max_spatial_stream = 2;
			else
				Max_spatial_stream = 1; 

			{

				for(i=0; i<Max_spatial_stream; i++)
				{
					{
						
						if(i==ODM_RF_PATH_A )
						{
							EVM = odm_EVMdbToPercentage( (pPhyStaRpt->sigevm ));
							EVM += 20;
							if (EVM > 100)
								EVM = 100;
						}
					}
#if 0
					else
					{
						if (pPhyStaRpt->rxevm[i] == -128)
						{
							pPhyStaRpt->rxevm[i] = -25;
						}
						EVM = odm_EVMdbToPercentage( (pPhyStaRpt->rxevm[i] ));
					}
#endif
					EVMdbm = odm_EVMdbm_JaguarSeries(pPhyStaRpt->rxevm[i]);
					
					
					{
						if(i==ODM_RF_PATH_A)
						{
							pPhyInfo->SignalQuality = EVM;
						}					
						pPhyInfo->RxMIMOSignalQuality[i] = EVM;
						pPhyInfo->RxMIMOEVMdbm[i] = EVMdbm;
					}
				}
			}
		}
		if(pDM_Odm->SupportAbility & ODM_BB_DYNAMIC_ATC)
		{
			if(pPktinfo->bPacketMatchBSSID && ( *(pDM_Odm->mp_mode) == 0) )
			{
				 for(i = ODM_RF_PATH_A; i < ODM_RF_PATH_MAX; i++)   
				{
					pDM_Odm->CFO_tail[i] = (int)pPhyStaRpt->cfotail[i];
			 	}

				if(pDM_Odm->packetCount == 0xffffffff)
					pDM_Odm->packetCount = 0;
				else
				 	pDM_Odm->packetCount++;
				
			}
		}
	}
	
#if (DM_ODM_SUPPORT_TYPE &  (ODM_WIN|ODM_CE))
	if(isCCKrate)
	{		
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		pPhyInfo->SignalStrength = (u1Byte)(SignalScaleMapping(pDM_Odm->Adapter, PWDB_ALL));//PWDB_ALL;
#else
		pPhyInfo->SignalStrength = (u1Byte)(odm_SignalScaleMapping(pDM_Odm, PWDB_ALL));//PWDB_ALL;
#endif
	}
	else
	{	
		if (rf_rx_num != 0)
		{			
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
			pPhyInfo->SignalStrength = (u1Byte)(SignalScaleMapping(pDM_Odm->Adapter, total_rssi/=rf_rx_num));//PWDB_ALL;
#else
			pPhyInfo->SignalStrength = (u1Byte)(odm_SignalScaleMapping(pDM_Odm, total_rssi/=rf_rx_num));
#endif
		}
	}
#endif
	pDM_Odm->RxPWDBAve = pDM_Odm->RxPWDBAve + pPhyInfo->RxPWDBAll;
	
	pDM_Odm->DM_FatTable.antsel_rx_keep_0 = pPhyStaRpt->antidx_anta;
	pDM_Odm->DM_FatTable.antsel_rx_keep_1 = pPhyStaRpt->antidx_antb;



}

#endif

VOID
odm_Init_RSSIForDM(
	IN OUT	PDM_ODM_T	pDM_Odm
	)
{

}

VOID
odm_Process_RSSIForDM(	
	IN OUT	PDM_ODM_T					pDM_Odm,
	IN		PODM_PHY_INFO_T				pPhyInfo,
	IN		PODM_PACKET_INFO_T			pPktinfo
	)
{
	
	s4Byte			UndecoratedSmoothedPWDB, UndecoratedSmoothedCCK, UndecoratedSmoothedOFDM, RSSI_Ave;
	u1Byte			isCCKrate=0;	
	u1Byte			RSSI_max, RSSI_min, i;
	u4Byte			OFDM_pkt=0; 
	u4Byte			Weighting=0;
	PSTA_INFO_T           	pEntry;


	if(pPktinfo->StationID == 0xFF)
		return;

		pEntry = pDM_Odm->pODM_StaInfo[pPktinfo->StationID];							

	if(!IS_STA_VALID(pEntry) ){		
		return;
	}
	if((!pPktinfo->bPacketMatchBSSID) )
	{
		return;
	}

	if(pPktinfo->bPacketBeacon)
		pDM_Odm->PhyDbgInfo.NumQryBeaconPkt++;
	
	isCCKrate = (pPktinfo->DataRate <= DESC_RATE11M)?TRUE :FALSE;
	pDM_Odm->RxRate = pPktinfo->DataRate;
	/*
	if(!isCCKrate)
	{
		DbgPrint("OFDM: pPktinfo->StationID=%d, isCCKrate=%d, pPhyInfo->RxPWDBAll=%d\n",
			pPktinfo->StationID, isCCKrate, pPhyInfo->RxPWDBAll);
	}
	*/

	if(pDM_Odm->SupportAbility & ODM_BB_ANT_DIV)
	{
		#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))
			ODM_Process_RSSIForAntDiv(pDM_Odm,pPhyInfo,pPktinfo);
		#endif
	}
	else if(pDM_Odm->SupportAbility & ODM_BB_PATH_DIV)
	{
		#if (RTL8812A_SUPPORT == 1)
		if(pDM_Odm->SupportICType == ODM_RTL8812)
		{
			pPATHDIV_T	pDM_PathDiv = &pDM_Odm->DM_PathDiv;
			if(pPktinfo->bPacketToSelf || pPktinfo->bPacketMatchBSSID)
			{
				if(pPktinfo->DataRate > DESC_RATE11M)
					ODM_PathStatistics_8812A(pDM_Odm, pPktinfo->StationID, pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A], 
					                                                                                                      pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_B]);
			}
		}
		#endif
	}

	
	UndecoratedSmoothedCCK =  pEntry->rssi_stat.UndecoratedSmoothedCCK;
	UndecoratedSmoothedOFDM = pEntry->rssi_stat.UndecoratedSmoothedOFDM;
	UndecoratedSmoothedPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;	
	
	if(pPktinfo->bPacketToSelf || pPktinfo->bPacketBeacon)
	{

		if(!isCCKrate)//ofdm rate
		{
			if(pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_B] == 0){
				RSSI_Ave = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A];
				pDM_Odm->RSSI_A = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A];
				pDM_Odm->RSSI_B = 0;
			}
			else
			{
				pDM_Odm->RSSI_A =  pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A];
				pDM_Odm->RSSI_B = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_B];
			
				if(pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A] > pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_B])
				{
					RSSI_max = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A];
					RSSI_min = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_B];
				}
				else
				{
					RSSI_max = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_B];
					RSSI_min = pPhyInfo->RxMIMOSignalStrength[ODM_RF_PATH_A];
				}
				if((RSSI_max -RSSI_min) < 3)
					RSSI_Ave = RSSI_max;
				else if((RSSI_max -RSSI_min) < 6)
					RSSI_Ave = RSSI_max - 1;
				else if((RSSI_max -RSSI_min) < 10)
					RSSI_Ave = RSSI_max - 2;
				else
					RSSI_Ave = RSSI_max - 3;
			}
					
			if(UndecoratedSmoothedOFDM <= 0)
			{
				UndecoratedSmoothedOFDM = pPhyInfo->RxPWDBAll;
			}
			else
			{
				if(pPhyInfo->RxPWDBAll > (u4Byte)UndecoratedSmoothedOFDM)
				{
					UndecoratedSmoothedOFDM = 	
							( ((UndecoratedSmoothedOFDM)*(Rx_Smooth_Factor-1)) + 
							(RSSI_Ave)) /(Rx_Smooth_Factor);
					UndecoratedSmoothedOFDM = UndecoratedSmoothedOFDM + 1;
				}
				else
				{
					UndecoratedSmoothedOFDM = 	
							( ((UndecoratedSmoothedOFDM)*(Rx_Smooth_Factor-1)) + 
							(RSSI_Ave)) /(Rx_Smooth_Factor);
				}
			}				
			
			pEntry->rssi_stat.PacketMap = (pEntry->rssi_stat.PacketMap<<1) | BIT0;			
										
		}
		else
		{
			RSSI_Ave = pPhyInfo->RxPWDBAll;
			pDM_Odm->RSSI_A = (u1Byte) pPhyInfo->RxPWDBAll;
			pDM_Odm->RSSI_B = 0;

			if(UndecoratedSmoothedCCK <= 0)
			{
				UndecoratedSmoothedCCK = pPhyInfo->RxPWDBAll;
			}
			else
			{
				if(pPhyInfo->RxPWDBAll > (u4Byte)UndecoratedSmoothedCCK)
				{
					UndecoratedSmoothedCCK = 	
							( ((UndecoratedSmoothedCCK)*(Rx_Smooth_Factor-1)) + 
							(pPhyInfo->RxPWDBAll)) /(Rx_Smooth_Factor);
					UndecoratedSmoothedCCK = UndecoratedSmoothedCCK + 1;
				}
				else
				{
					UndecoratedSmoothedCCK = 	
							( ((UndecoratedSmoothedCCK)*(Rx_Smooth_Factor-1)) + 
							(pPhyInfo->RxPWDBAll)) /(Rx_Smooth_Factor);
				}
			}
			pEntry->rssi_stat.PacketMap = pEntry->rssi_stat.PacketMap<<1;			
		}

		{
			if(pEntry->rssi_stat.ValidBit >= 64)
				pEntry->rssi_stat.ValidBit = 64;
			else
				pEntry->rssi_stat.ValidBit++;
			
			for(i=0; i<pEntry->rssi_stat.ValidBit; i++)
				OFDM_pkt += (u1Byte)(pEntry->rssi_stat.PacketMap>>i)&BIT0;
			
			if(pEntry->rssi_stat.ValidBit == 64)
			{
				Weighting = ((OFDM_pkt<<4) > 64)?64:(OFDM_pkt<<4);
				UndecoratedSmoothedPWDB = (Weighting*UndecoratedSmoothedOFDM+(64-Weighting)*UndecoratedSmoothedCCK)>>6;
			}
			else
			{
				if(pEntry->rssi_stat.ValidBit != 0)
					UndecoratedSmoothedPWDB = (OFDM_pkt*UndecoratedSmoothedOFDM+(pEntry->rssi_stat.ValidBit-OFDM_pkt)*UndecoratedSmoothedCCK)/pEntry->rssi_stat.ValidBit;
				else
					UndecoratedSmoothedPWDB = 0;
			}

			pEntry->rssi_stat.UndecoratedSmoothedCCK = UndecoratedSmoothedCCK;
			pEntry->rssi_stat.UndecoratedSmoothedOFDM = UndecoratedSmoothedOFDM;
			pEntry->rssi_stat.UndecoratedSmoothedPWDB = UndecoratedSmoothedPWDB;

			
		}
	
	}
}


VOID
ODM_PhyStatusQuery_92CSeries(
	IN OUT	PDM_ODM_T					pDM_Odm,
	OUT		PODM_PHY_INFO_T				pPhyInfo,
	IN 		pu1Byte						pPhyStatus,	
	IN		PODM_PACKET_INFO_T			pPktinfo
	)
{
	
	odm_RxPhyStatus92CSeries_Parsing(
							pDM_Odm,
							pPhyInfo,
							pPhyStatus,
							pPktinfo);

	if( pDM_Odm->RSSI_test == TRUE)
	{
		if(pPktinfo->bPacketToSelf || pPktinfo->bPacketBeacon )
		{
				/*
			#if 0//(DM_ODM_SUPPORT_TYPE == ODM_WIN)
			dm_SWAW_RSSI_Check(
				Adapter, 
				(tmppAdapter!=NULL)?(tmppAdapter==Adapter):TRUE,
				bPacketMatchBSSID,
				pEntry,
				pRfd);
			#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
			#endif
				*/
				ODM_SwAntDivChkPerPktRssi(pDM_Odm,pPktinfo->StationID,pPhyInfo);
		}	
	}
	else
	{
		odm_Process_RSSIForDM(pDM_Odm,pPhyInfo,pPktinfo);
	}
	
}



VOID
ODM_PhyStatusQuery_JaguarSeries(
	IN OUT	PDM_ODM_T					pDM_Odm,
	OUT		PODM_PHY_INFO_T			pPhyInfo,
	IN 		pu1Byte						pPhyStatus,	
	IN		PODM_PACKET_INFO_T			pPktinfo
	)
{
	odm_RxPhyStatusJaguarSeries_Parsing(
							pDM_Odm,
							pPhyInfo,
							pPhyStatus,
							pPktinfo);
	
	odm_Process_RSSIForDM(pDM_Odm,pPhyInfo,pPktinfo);
}

VOID
ODM_PhyStatusQuery(
	IN OUT	PDM_ODM_T					pDM_Odm,
	OUT		PODM_PHY_INFO_T				pPhyInfo,
	IN 		pu1Byte						pPhyStatus,	
	IN		PODM_PACKET_INFO_T			pPktinfo
	)
{

	if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES )
	{
		ODM_PhyStatusQuery_JaguarSeries(pDM_Odm,pPhyInfo,pPhyStatus,pPktinfo);
	}
	else
	{
		ODM_PhyStatusQuery_92CSeries(pDM_Odm,pPhyInfo,pPhyStatus,pPktinfo);
	}
}
	
VOID
ODM_MacStatusQuery(
	IN OUT	PDM_ODM_T					pDM_Odm,
	IN 		pu1Byte						pMacStatus,
	IN		u1Byte						MacID,	
	IN		BOOLEAN						bPacketMatchBSSID,
	IN		BOOLEAN						bPacketToSelf,
	IN		BOOLEAN						bPacketBeacon
	)
{
	
}



HAL_STATUS
ODM_ConfigRFWithHeaderFile(
	IN 	PDM_ODM_T	        	pDM_Odm,
	IN 	ODM_RF_Config_Type 		ConfigType,
	IN 	ODM_RF_RADIO_PATH_E 	eRFPath
    )
{
	PADAPTER		Adapter = pDM_Odm->Adapter;

   ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
		 		("===>ODM_ConfigRFWithHeaderFile (%s)\n", (pDM_Odm->bIsMPChip) ? "MPChip" : "TestChip"));
    ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
				("pDM_Odm->SupportPlatform: 0x%X, pDM_Odm->SupportInterface: 0x%X, pDM_Odm->BoardType: 0x%X\n",
				pDM_Odm->SupportPlatform, pDM_Odm->SupportInterface, pDM_Odm->BoardType));

#if (RTL8723A_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8723A)
	{
		if(ConfigType == CONFIG_RF_RADIO) {
			if(eRFPath == ODM_RF_PATH_A)
				READ_AND_CONFIG_MP(8723A,_RadioA_1T);
		}
	}
#endif

#if (RTL8188E_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8188E)
	{
		if(ConfigType == CONFIG_RF_RADIO) {
			if(eRFPath == ODM_RF_PATH_A){
				if(IS_VENDOR_8188E_I_CUT_SERIES(Adapter))
					READ_AND_CONFIG(8188E,_RadioA_1T_ICUT);
				else
					READ_AND_CONFIG(8188E,_RadioA_1T);
				}
		}
		else if(ConfigType == CONFIG_RF_TXPWR_LMT) {
			READ_AND_CONFIG(8188E,_TXPWR_LMT);
		}
	}
#endif

#if (RTL8812A_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8812)
	{
		if(ConfigType == CONFIG_RF_RADIO) {
			if(eRFPath == ODM_RF_PATH_A)
			{
				READ_AND_CONFIG(8812A,_RadioA);
			}
			else if(eRFPath == ODM_RF_PATH_B)
			{
				READ_AND_CONFIG(8812A,_RadioB);
			}
		}
		else if(ConfigType == CONFIG_RF_TXPWR_LMT) {
			READ_AND_CONFIG(8812A,_TXPWR_LMT);
		}
	}
#endif

#if (RTL8821A_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8821)
	{
		if(ConfigType == CONFIG_RF_RADIO) {
	 		if(eRFPath == ODM_RF_PATH_A)
			{
				READ_AND_CONFIG(8821A,_RadioA);
			}
		}
		else if(ConfigType == CONFIG_RF_TXPWR_LMT) {
			
			if (pDM_Odm->SupportInterface == ODM_ITRF_USB) {
				if (pDM_Odm->ExtPA5G || pDM_Odm->ExtLNA5G)
					READ_AND_CONFIG(8821A,_TXPWR_LMT_8811AU_FEM);
				else
					READ_AND_CONFIG(8821A,_TXPWR_LMT_8811AU_IPA);				
			} else {
				READ_AND_CONFIG(8821A,_TXPWR_LMT_8821A);			
			}
		}
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("<===8821_ODM_ConfigRFWithHeaderFile\n"));
	}
#endif

#if (RTL8723B_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8723B)
	{
		if(ConfigType == CONFIG_RF_RADIO) {
			READ_AND_CONFIG(8723B,_RadioA);
		}
		else if(ConfigType == CONFIG_RF_TXPWR_LMT) {
			READ_AND_CONFIG(8723B,_TXPWR_LMT);
		}
	}
#endif

#if (RTL8192E_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8192E)
	{
		if(ConfigType == CONFIG_RF_RADIO) {
		 	if(eRFPath == ODM_RF_PATH_A)
				READ_AND_CONFIG(8192E,_RadioA);
			else if(eRFPath == ODM_RF_PATH_B)
				READ_AND_CONFIG(8192E,_RadioB);
		}
		else if(ConfigType == CONFIG_RF_TXPWR_LMT) {
			READ_AND_CONFIG(8192E,_TXPWR_LMT);
		}
	}
#endif

#if (RTL8813A_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8813A)
	{
		/*
		if(ConfigType == CONFIG_RF_TXPWR_LMT) {
			READ_AND_CONFIG(8813A,_TXPWR_LMT);
		}
		*/		
	}
#endif
	
	return HAL_STATUS_SUCCESS;
}

HAL_STATUS
ODM_ConfigRFWithTxPwrTrackHeaderFile(
	IN 	PDM_ODM_T	        	pDM_Odm
    )
{
   	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
		 		 ("===>ODM_ConfigRFWithTxPwrTrackHeaderFile (%s)\n", (pDM_Odm->bIsMPChip) ? "MPChip" : "TestChip"));
   	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
				 ("pDM_Odm->SupportPlatform: 0x%X, pDM_Odm->SupportInterface: 0x%X, pDM_Odm->BoardType: 0x%X\n",
				 pDM_Odm->SupportPlatform, pDM_Odm->SupportInterface, pDM_Odm->BoardType));

	if(0)
	{
	}
#if (RTL8821A_SUPPORT == 1) 
	else if(pDM_Odm->SupportICType == ODM_RTL8821)
	{
		if (pDM_Odm->SupportInterface == ODM_ITRF_PCIE)
			READ_AND_CONFIG(8821A,_TxPowerTrack_PCIE);
		else if (pDM_Odm->SupportInterface == ODM_ITRF_USB)
			READ_AND_CONFIG(8821A,_TxPowerTrack_USB);
		else
			READ_AND_CONFIG(8821A,_TxPowerTrack_PCIE);
	}
#endif
#if (RTL8812A_SUPPORT == 1)
	else if(pDM_Odm->SupportICType == ODM_RTL8812)
	{
		if (pDM_Odm->SupportInterface == ODM_ITRF_PCIE)
			READ_AND_CONFIG(8812A,_TxPowerTrack_PCIE);
		else if (pDM_Odm->SupportInterface == ODM_ITRF_USB) {
			if (pDM_Odm->RFEType == 3 && pDM_Odm->bIsMPChip) 
				READ_AND_CONFIG_MP(8812A,_TxPowerTrack_RFE3);	
			else
				READ_AND_CONFIG(8812A,_TxPowerTrack_USB);	
		}
		
	}
#endif
#if (RTL8192E_SUPPORT == 1) 
	else if(pDM_Odm->SupportICType == ODM_RTL8192E)
	{
		if (pDM_Odm->SupportInterface == ODM_ITRF_PCIE)
			READ_AND_CONFIG(8192E,_TxPowerTrack_PCIE);
		else if (pDM_Odm->SupportInterface == ODM_ITRF_USB)
			READ_AND_CONFIG(8192E,_TxPowerTrack_USB); 
	}
#endif
#if RTL8723B_SUPPORT 	
	else if(pDM_Odm->SupportICType == ODM_RTL8723B)
	{
		if (pDM_Odm->SupportInterface == ODM_ITRF_PCIE)
			READ_AND_CONFIG(8723B,_TxPowerTrack_PCIE);
		else if (pDM_Odm->SupportInterface == ODM_ITRF_USB)
			READ_AND_CONFIG(8723B,_TxPowerTrack_USB);
		else if (pDM_Odm->SupportInterface == ODM_ITRF_SDIO)
			READ_AND_CONFIG(8723B,_TxPowerTrack_SDIO); 			
	}
#endif	
#if RTL8188E_SUPPORT 	
	else if(pDM_Odm->SupportICType == ODM_RTL8188E)
	{
		if (pDM_Odm->SupportInterface == ODM_ITRF_PCIE)
			READ_AND_CONFIG(8188E,_TxPowerTrack_PCIE);
		else if (pDM_Odm->SupportInterface == ODM_ITRF_USB)
			READ_AND_CONFIG(8188E,_TxPowerTrack_USB);
	}
#endif

	return HAL_STATUS_SUCCESS;
}

HAL_STATUS
ODM_ConfigBBWithHeaderFile(
	IN 	PDM_ODM_T	             	pDM_Odm,
	IN 	ODM_BB_Config_Type 		ConfigType
	)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_CE|ODM_WIN))
	PADAPTER		Adapter = pDM_Odm->Adapter;
#if (DM_ODM_SUPPORT_TYPE &  ODM_WIN)
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);	
#endif
#endif
	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
		 		("===>ODM_ConfigBBWithHeaderFile (%s)\n", (pDM_Odm->bIsMPChip) ? "MPChip" : "TestChip"));
    ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
				("pDM_Odm->SupportPlatform: 0x%X, pDM_Odm->SupportInterface: 0x%X, pDM_Odm->BoardType: 0x%X\n",
				pDM_Odm->SupportPlatform, pDM_Odm->SupportInterface, pDM_Odm->BoardType));

#if (RTL8723A_SUPPORT == 1) 
    if(pDM_Odm->SupportICType == ODM_RTL8723A)
	{
		if(ConfigType == CONFIG_BB_PHY_REG)
		{
			READ_AND_CONFIG_MP(8723A,_PHY_REG_1T);
		}
		else if(ConfigType == CONFIG_BB_AGC_TAB)
		{
			READ_AND_CONFIG_MP(8723A,_AGC_TAB_1T);
		}
	}		
#endif

#if (RTL8188E_SUPPORT == 1)
    if(pDM_Odm->SupportICType == ODM_RTL8188E)
	{
		if(ConfigType == CONFIG_BB_PHY_REG)
		{
			if(IS_VENDOR_8188E_I_CUT_SERIES(Adapter))
				READ_AND_CONFIG(8188E,_PHY_REG_1T_ICUT);
			else
				READ_AND_CONFIG(8188E,_PHY_REG_1T);
		}
		else if(ConfigType == CONFIG_BB_AGC_TAB)
		{
			if(IS_VENDOR_8188E_I_CUT_SERIES(Adapter))
				READ_AND_CONFIG(8188E,_AGC_TAB_1T_ICUT);
			else
				READ_AND_CONFIG(8188E,_AGC_TAB_1T);
		}
		else if(ConfigType == CONFIG_BB_PHY_REG_PG)
		{
			READ_AND_CONFIG(8188E,_PHY_REG_PG);
		}
	}
#endif

#if (RTL8812A_SUPPORT == 1) 
	if(pDM_Odm->SupportICType == ODM_RTL8812)
	{
		if(ConfigType == CONFIG_BB_PHY_REG)
		{
			READ_AND_CONFIG(8812A,_PHY_REG);
		}
		else if(ConfigType == CONFIG_BB_AGC_TAB)
		{
			READ_AND_CONFIG(8812A,_AGC_TAB);
		}
		else if(ConfigType == CONFIG_BB_PHY_REG_PG)
		{
			if (pDM_Odm->RFEType == 3 && pDM_Odm->bIsMPChip) 
				READ_AND_CONFIG_MP(8812A,_PHY_REG_PG_ASUS);
#if (DM_ODM_SUPPORT_TYPE &  ODM_WIN)
			else if (pMgntInfo->CustomerID == RT_CID_WNC_NEC && pDM_Odm->bIsMPChip) 
				READ_AND_CONFIG_MP(8812A,_PHY_REG_PG_NEC);
#endif			
			else
				READ_AND_CONFIG(8812A,_PHY_REG_PG);
		}
		else if(ConfigType == CONFIG_BB_PHY_REG_MP)
		{
			READ_AND_CONFIG_MP(8812A,_PHY_REG_MP);
		}
		else if(ConfigType == CONFIG_BB_AGC_TAB_DIFF)
		{
			if ((36 <= *pDM_Odm->pChannel)  && (*pDM_Odm->pChannel  <= 64)) 
				AGC_DIFF_CONFIG_MP(8812A,LB);
			else if (100 <= *pDM_Odm->pChannel) 
				AGC_DIFF_CONFIG_MP(8812A,HB);
		}
	}		
#endif

#if (RTL8821A_SUPPORT == 1) 
	if(pDM_Odm->SupportICType == ODM_RTL8821)
	{
		if(ConfigType == CONFIG_BB_PHY_REG)
		{
			READ_AND_CONFIG(8821A,_PHY_REG);
		}
		else if(ConfigType == CONFIG_BB_AGC_TAB)
		{
			READ_AND_CONFIG(8821A,_AGC_TAB);
		}
		else if(ConfigType == CONFIG_BB_PHY_REG_PG)
		{
			READ_AND_CONFIG(8821A,_PHY_REG_PG);
		}
	}
#endif
#if (RTL8723B_SUPPORT == 1)
    if(pDM_Odm->SupportICType == ODM_RTL8723B)
	{

		if(ConfigType == CONFIG_BB_PHY_REG)
		{
			READ_AND_CONFIG(8723B,_PHY_REG);
		}
		else if(ConfigType == CONFIG_BB_AGC_TAB)
		{
			READ_AND_CONFIG(8723B,_AGC_TAB);
		}
		else if(ConfigType == CONFIG_BB_PHY_REG_PG)
		{
			READ_AND_CONFIG(8723B,_PHY_REG_PG);
		}
	}
#endif
#if (RTL8192E_SUPPORT == 1)
    if(pDM_Odm->SupportICType == ODM_RTL8192E)
	{

		if(ConfigType == CONFIG_BB_PHY_REG)
		{
			READ_AND_CONFIG(8192E,_PHY_REG);
		}
		else if(ConfigType == CONFIG_BB_AGC_TAB)
		{
			READ_AND_CONFIG(8192E,_AGC_TAB);
		}
		else if(ConfigType == CONFIG_BB_PHY_REG_PG)
		{
			READ_AND_CONFIG(8192E,_PHY_REG_PG);
		}
	}
#endif
#if (RTL8813A_SUPPORT == 1)
    if(pDM_Odm->SupportICType == ODM_RTL8813A)
	{

		if(ConfigType == CONFIG_BB_PHY_REG)
		{
			READ_AND_CONFIG(8813A,_PHY_REG);
		}
		else if(ConfigType == CONFIG_BB_AGC_TAB)
		{
			READ_AND_CONFIG(8813A,_AGC_TAB);
		}
		else if(ConfigType == CONFIG_BB_PHY_REG_PG)
		{
		}
	}
#endif
	return HAL_STATUS_SUCCESS; 
}                 

HAL_STATUS
ODM_ConfigMACWithHeaderFile(
	IN 	PDM_ODM_T	pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE &  (ODM_CE|ODM_WIN))	
	PADAPTER		Adapter = pDM_Odm->Adapter;
#endif
	u1Byte result = HAL_STATUS_SUCCESS;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
		 		("===>ODM_ConfigMACWithHeaderFile (%s)\n", (pDM_Odm->bIsMPChip) ? "MPChip" : "TestChip"));
    ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, 
				("pDM_Odm->SupportPlatform: 0x%X, pDM_Odm->SupportInterface: 0x%X, pDM_Odm->BoardType: 0x%X\n",
				pDM_Odm->SupportPlatform, pDM_Odm->SupportInterface, pDM_Odm->BoardType));
	
#if (RTL8723A_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8723A)
	{
		READ_AND_CONFIG_MP(8723A,_MAC_REG);
	}
#endif
#if (RTL8188E_SUPPORT == 1)  
	if (pDM_Odm->SupportICType == ODM_RTL8188E)
	{
		if(IS_VENDOR_8188E_I_CUT_SERIES(Adapter))
			result = READ_AND_CONFIG(8188E,_MAC_REG_ICUT);
		else
			result = READ_AND_CONFIG(8188E,_MAC_REG);
	}
#endif
#if (RTL8812A_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8812)
	{
		READ_AND_CONFIG(8812A,_MAC_REG);
	}
#endif
#if (RTL8821A_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8821)
	{
		READ_AND_CONFIG(8821A,_MAC_REG);

		ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("<===8821_ODM_ConfigMACwithHeaderFile\n"));
	}
#endif
#if (RTL8723B_SUPPORT == 1)  
	if (pDM_Odm->SupportICType == ODM_RTL8723B)
	{
		READ_AND_CONFIG(8723B,_MAC_REG);
	}
#endif
#if (RTL8192E_SUPPORT == 1)  
	if (pDM_Odm->SupportICType == ODM_RTL8192E)
	{
		READ_AND_CONFIG(8192E,_MAC_REG);
	}
#endif

	return result;
} 

HAL_STATUS
ODM_ConfigFWWithHeaderFile(
	IN 	PDM_ODM_T			pDM_Odm,
	IN 	ODM_FW_Config_Type 	ConfigType,
	OUT u1Byte				*pFirmware,
	OUT u4Byte				*pSize
	)
{

#if (RTL8188E_SUPPORT == 1)  
	if (pDM_Odm->SupportICType == ODM_RTL8188E)
	{
		if (ConfigType == CONFIG_FW_NIC)
		{
			READ_FIRMWARE(8188E,_FW_NIC_T);
		}
		else if (ConfigType == CONFIG_FW_WoWLAN)
		{
			READ_FIRMWARE(8188E,_FW_WoWLAN_T);
		}
		else if(ConfigType == CONFIG_FW_NIC_2)
		{
			READ_FIRMWARE(8188E,_FW_NIC_S);
		}
		else if (ConfigType == CONFIG_FW_WoWLAN_2)
		{
			READ_FIRMWARE(8188E,_FW_WoWLAN_S);
		}
	}
#endif
#if (RTL8723B_SUPPORT == 1)  
	if (pDM_Odm->SupportICType == ODM_RTL8723B)
	{
		if (ConfigType == CONFIG_FW_NIC)
		{
			READ_FIRMWARE(8723B,_FW_NIC);
		}
		else if (ConfigType == CONFIG_FW_WoWLAN)
		{
			READ_FIRMWARE(8723B,_FW_WoWLAN);
		}
#ifdef CONFIG_AP_WOWLAN
		else if (ConfigType == CONFIG_FW_AP_WoWLAN)
		{
			READ_FIRMWARE(8723B,_FW_AP_WoWLAN);
		}
#endif
		else if (ConfigType == CONFIG_FW_BT)
		{
			READ_FIRMWARE_MP(8723B,_FW_BT);
		}
		else if (ConfigType == CONFIG_FW_MP)
		{
			READ_FIRMWARE_MP(8723B,_FW_MP);
		}
	}
#endif
#if (RTL8812A_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8812)
	{
		if (ConfigType == CONFIG_FW_NIC)
		{
			READ_FIRMWARE(8812A,_FW_NIC);
		}
		else if (ConfigType == CONFIG_FW_WoWLAN)
		{
			READ_FIRMWARE(8812A,_FW_WoWLAN);
		}
		else if (ConfigType == CONFIG_FW_BT)
		{
			READ_FIRMWARE(8812A,_FW_NIC_BT);
		}

	}
#endif
#if (RTL8821A_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8821)
	{
		if (ConfigType == CONFIG_FW_NIC)
		{
			READ_FIRMWARE_MP(8821A,_FW_NIC);
		}
		else if (ConfigType == CONFIG_FW_WoWLAN)
		{
			READ_FIRMWARE(8821A,_FW_WoWLAN);
		}
		else if (ConfigType == CONFIG_FW_BT)
		{
			READ_FIRMWARE_MP(8821A,_FW_NIC_BT);
		}
	}
#endif
#if (RTL8192E_SUPPORT == 1)
	if (pDM_Odm->SupportICType == ODM_RTL8192E)
	{
		if (ConfigType == CONFIG_FW_NIC)
		{
			READ_FIRMWARE(8192E,_FW_NIC);
		}
		else if (ConfigType == CONFIG_FW_WoWLAN)
		{
			READ_FIRMWARE(8192E,_FW_WoWLAN);
		}
#ifdef CONFIG_AP_WOWLAN
		else if (ConfigType == CONFIG_FW_AP_WoWLAN)
		{
			READ_FIRMWARE(8192E,_FW_AP_WoWLAN);
		}
#endif

	}
#endif
	return HAL_STATUS_SUCCESS;    
} 


u4Byte 
ODM_GetHWImgVersion(
	IN	PDM_ODM_T	pDM_Odm
	)
{

#if (RTL8812A_SUPPORT == 1)  
	if (pDM_Odm->SupportICType == ODM_RTL8812)
		return GET_VERSION_MP(8812A,_MAC_REG);
#endif

	return 0;
}





