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
#define _RTL8188E_DM_C_

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>
#include <rtw_byteorder.h>

#include <rtl8188e_hal.h>



static VOID
dm_CheckProtection (
  IN  PADAPTER  Adapter
)
{
  #if 0
  PMGNT_INFO    pMgntInfo = & (Adapter->MgntInfo);
  u1Byte      CurRate, RateThreshold;
  
  if (pMgntInfo->pHTInfo->bCurBW40MHz)
  { RateThreshold = MGN_MCS1; }
  else
  { RateThreshold = MGN_MCS3; }
  
  if (Adapter->TxStats.CurrentInitTxRate <= RateThreshold)
  {
    pMgntInfo->bDmDisableProtect = TRUE;
    DbgPrint ("Forced disable protect: %x\n", Adapter->TxStats.CurrentInitTxRate);
  }
  else
  {
    pMgntInfo->bDmDisableProtect = FALSE;
    DbgPrint ("Enable protect: %x\n", Adapter->TxStats.CurrentInitTxRate);
  }
  #endif
}

static VOID
dm_CheckStatistics (
  IN  PADAPTER  Adapter
)
{
  #if 0
  if (!Adapter->MgntInfo.bMediaConnect)
  { return; }
  
  rtw_hal_get_hwreg ( Adapter, HW_VAR_INIT_TX_RATE, (pu1Byte) (&Adapter->TxStats.CurrentInitTxRate) );
  
  
  
  rtw_hal_get_hwreg ( Adapter, HW_VAR_RETRY_COUNT, (pu1Byte) (&Adapter->TxStats.NumTxRetryCount) );
  #endif
}

static void dm_CheckPbcGPIO (_adapter * padapter)
{
  u8  tmp1byte;
  u8  bPbcPressed = _FALSE;
  
  if (!padapter->registrypriv.hw_wps_pbc)
  { return; }
  
  #ifdef CONFIG_USB_HCI
  tmp1byte = rtw_read8 (padapter, GPIO_IO_SEL);
  tmp1byte |= (HAL_8192C_HW_GPIO_WPS_BIT);
  rtw_write8 (padapter, GPIO_IO_SEL, tmp1byte);
  
  tmp1byte &= ~ (HAL_8192C_HW_GPIO_WPS_BIT);
  rtw_write8 (padapter,  GPIO_IN, tmp1byte); 
  
  tmp1byte = rtw_read8 (padapter, GPIO_IO_SEL);
  tmp1byte &= ~ (HAL_8192C_HW_GPIO_WPS_BIT);
  rtw_write8 (padapter, GPIO_IO_SEL, tmp1byte);
  
  tmp1byte = rtw_read8 (padapter, GPIO_IN);
  
  if (tmp1byte == 0xff)
  { return ; }
  
  if (tmp1byte & HAL_8192C_HW_GPIO_WPS_BIT)
  {
    bPbcPressed = _TRUE;
  }
  #else
  tmp1byte = rtw_read8 (padapter, GPIO_IN);
  
  if (tmp1byte == 0xff || padapter->init_adpt_in_progress)
  { return ; }
  
  if ( (tmp1byte & HAL_8192C_HW_GPIO_WPS_BIT) == 0)
  {
    bPbcPressed = _TRUE;
  }
  #endif
  
  if ( _TRUE == bPbcPressed)
  {
    DBG_8192C ("CheckPbcGPIO - PBC is pressed\n");
    
    #ifdef RTK_DMP_PLATFORM
    #if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,12))
    kobject_uevent (&padapter->pnetdev->dev.kobj, KOBJ_NET_PBC);
    #else
    kobject_hotplug (&padapter->pnetdev->class_dev.kobj, KOBJ_NET_PBC);
    #endif
    #else
    
    if ( padapter->pid[0] == 0 )
    {
      return;
    }
    
    #ifdef PLATFORM_LINUX
    rtw_signal_process (padapter->pid[0], SIGUSR1);
    #endif
    #endif
  }
}

#ifdef CONFIG_PCI_HCI
VOID
dm_InterruptMigration (
  IN  PADAPTER  Adapter
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  struct mlme_priv * pmlmepriv = & (Adapter->mlmepriv);
  BOOLEAN     bCurrentIntMt, bCurrentACIntDisable;
  BOOLEAN     IntMtToSet = _FALSE;
  BOOLEAN     ACIntToSet = _FALSE;
  
  
  bCurrentIntMt = pHalData->bInterruptMigration;
  bCurrentACIntDisable = pHalData->bDisableTxInt;
  
  if (!Adapter->registrypriv.wifi_spec &&
      (check_fwstate (pmlmepriv, _FW_LINKED) == _TRUE) &&
      pmlmepriv->LinkDetectInfo.bHigherBusyTraffic)
  {
    IntMtToSet = _TRUE;
    
    if (pmlmepriv->LinkDetectInfo.bHigherBusyRxTraffic )
    { ACIntToSet = _TRUE; }
  }
  
  if ( bCurrentIntMt != IntMtToSet ) {
    DBG_8192C ("%s(): Update interrrupt migration(%d)\n", __FUNCTION__, IntMtToSet);
    if (IntMtToSet)
    {
      rtw_write32 (Adapter, REG_INT_MIG, 0xff000fa0);
      pHalData->bInterruptMigration = IntMtToSet;
    }
    else
    {
      rtw_write32 (Adapter, REG_INT_MIG, 0);
      pHalData->bInterruptMigration = IntMtToSet;
    }
  }
  
  /*if( bCurrentACIntDisable != ACIntToSet ){
    DBG_8192C("%s(): Update AC interrrupt(%d)\n",__FUNCTION__,ACIntToSet);
    if(ACIntToSet)
    {
      UpdateInterruptMask8192CE( Adapter, 0, RT_AC_INT_MASKS );
      pHalData->bDisableTxInt = ACIntToSet;
    }
    else// Enable four ACs interrupts.
    {
      UpdateInterruptMask8192CE( Adapter, RT_AC_INT_MASKS, 0 );
      pHalData->bDisableTxInt = ACIntToSet;
    }
  }*/
  
}

#endif

static void
dm_InitGPIOSetting (
  IN  PADAPTER  Adapter
)
{
  PHAL_DATA_TYPE    pHalData = GET_HAL_DATA (Adapter);
  
  u8  tmp1byte;
  
  tmp1byte = rtw_read8 (Adapter, REG_GPIO_MUXCFG);
  tmp1byte &= (GPIOSEL_GPIO | ~GPIOSEL_ENBT);
  
  #ifdef CONFIG_BT_COEXIST
  if (IS_81xxC_VENDOR_UMC_B_CUT (pHalData->VersionID) &&
      pHalData->bt_coexist.BT_Coexist)
  {
    tmp1byte |= (BIT5);
  }
  #endif
  rtw_write8 (Adapter, REG_GPIO_MUXCFG, tmp1byte);
  
}

static void Init_ODM_ComInfo_88E (PADAPTER Adapter)
{

  PHAL_DATA_TYPE  pHalData = GET_HAL_DATA (Adapter);
  struct dm_priv * pdmpriv = &pHalData->dmpriv;
  PDM_ODM_T   pDM_Odm = & (pHalData->odmpriv);
  u8  cut_ver, fab_ver;
  
  _rtw_memset (pDM_Odm, 0, sizeof (pDM_Odm) );
  
  pDM_Odm->Adapter = Adapter;
  
  ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_PLATFORM, ODM_CE);
  
  if (Adapter->interface_type == RTW_GSPI )
  { ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_INTERFACE, ODM_ITRF_SDIO); }
  else
  { ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_INTERFACE, Adapter->interface_type); }
  
  ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_IC_TYPE, ODM_RTL8188E);
  
  fab_ver = ODM_TSMC;
  cut_ver = ODM_CUT_A;
  
  ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_FAB_VER, fab_ver);
  ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_CUT_VER, cut_ver);
  
  ODM_CmnInfoInit (pDM_Odm,  ODM_CMNINFO_MP_TEST_CHIP, IS_NORMAL_CHIP (pHalData->VersionID) );
  
  #if 0
  ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_BOARD_TYPE, pHalData->BoardType);
  
  if (pHalData->BoardType == BOARD_USB_High_PA) {
    ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_EXT_LNA, _TRUE);
    ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_EXT_PA, _TRUE);
  }
  #endif
  ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_PATCH_ID, pHalData->CustomerID);
  ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_BWIFI_TEST, Adapter->registrypriv.wifi_spec);
  
  
  if (pHalData->rf_type == RF_1T1R) {
    ODM_CmnInfoUpdate (pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_1T1R);
  }
  else
    if (pHalData->rf_type == RF_2T2R) {
      ODM_CmnInfoUpdate (pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_2T2R);
    }
    else
      if (pHalData->rf_type == RF_1T2R) {
        ODM_CmnInfoUpdate (pDM_Odm, ODM_CMNINFO_RF_TYPE, ODM_1T2R);
      }
      
  ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_RF_ANTENNA_TYPE, pHalData->TRxAntDivType);
  
  #ifdef CONFIG_DISABLE_ODM
  pdmpriv->InitODMFlag = 0;
  #else
  pdmpriv->InitODMFlag =  ODM_RF_CALIBRATION    |
                          ODM_RF_TX_PWR_TRACK
                          ;
  #endif
  
  ODM_CmnInfoUpdate (pDM_Odm, ODM_CMNINFO_ABILITY, pdmpriv->InitODMFlag);
  
}
static void Update_ODM_ComInfo_88E (PADAPTER Adapter)
{
  struct mlme_ext_priv * pmlmeext = &Adapter->mlmeextpriv;
  struct mlme_priv * pmlmepriv = &Adapter->mlmepriv;
  struct pwrctrl_priv * pwrctrlpriv = &Adapter->pwrctrlpriv;
  PHAL_DATA_TYPE  pHalData = GET_HAL_DATA (Adapter);
  PDM_ODM_T   pDM_Odm = & (pHalData->odmpriv);
  struct dm_priv * pdmpriv = &pHalData->dmpriv;
  int i;
  #ifdef CONFIG_DISABLE_ODM
  pdmpriv->InitODMFlag = 0;
  #else
  
  pdmpriv->InitODMFlag =  ODM_BB_DIG        |
                          #ifdef  CONFIG_ODM_REFRESH_RAMASK
                          ODM_BB_RA_MASK    |
                          #endif
                          ODM_BB_DYNAMIC_TXPWR  |
                          ODM_BB_FA_CNT     |
                          ODM_BB_RSSI_MONITOR |
                          ODM_BB_CCK_PD     |
                          ODM_BB_PWR_SAVE   |
                          ODM_MAC_EDCA_TURBO  |
                          ODM_RF_CALIBRATION    |
                          ODM_RF_TX_PWR_TRACK
                          ;
  if (pHalData->AntDivCfg)
  { pdmpriv->InitODMFlag |= ODM_BB_ANT_DIV; }
  
  #if (MP_DRIVER==1)
  if (Adapter->registrypriv.mp_mode == 1)
  {
    pdmpriv->InitODMFlag =  ODM_RF_CALIBRATION  |
                            ODM_RF_TX_PWR_TRACK;
  }
  #endif//(MP_DRIVER==1)
  
  #endif//CONFIG_DISABLE_ODM
  ODM_CmnInfoUpdate (pDM_Odm, ODM_CMNINFO_ABILITY, pdmpriv->InitODMFlag);
  
  ODM_CmnInfoHook (pDM_Odm, ODM_CMNINFO_TX_UNI, & (Adapter->xmitpriv.tx_bytes) );
  ODM_CmnInfoHook (pDM_Odm, ODM_CMNINFO_RX_UNI, & (Adapter->recvpriv.rx_bytes) );
  ODM_CmnInfoHook (pDM_Odm, ODM_CMNINFO_WM_MODE, & (pmlmeext->cur_wireless_mode) );
  ODM_CmnInfoHook (pDM_Odm, ODM_CMNINFO_SEC_CHNL_OFFSET, & (pHalData->nCur40MhzPrimeSC) );
  ODM_CmnInfoHook (pDM_Odm, ODM_CMNINFO_SEC_MODE, & (Adapter->securitypriv.dot11PrivacyAlgrthm) );
  ODM_CmnInfoHook (pDM_Odm, ODM_CMNINFO_BW, & (pHalData->CurrentChannelBW ) );
  ODM_CmnInfoHook (pDM_Odm, ODM_CMNINFO_CHNL, & ( pHalData->CurrentChannel) );
  ODM_CmnInfoHook (pDM_Odm, ODM_CMNINFO_NET_CLOSED, & ( Adapter->net_closed) );
  ODM_CmnInfoHook (pDM_Odm, ODM_CMNINFO_MP_MODE, & (Adapter->registrypriv.mp_mode) );
  /*
  ODM_CmnInfoHook(pDM_Odm,ODM_CMNINFO_BAND,&(pDM_Odm->u1Byte_temp));
  ODM_CmnInfoHook(pDM_Odm,ODM_CMNINFO_DMSP_GET_VALUE,&(pDM_Odm->u1Byte_temp));
  ODM_CmnInfoHook(pDM_Odm,ODM_CMNINFO_BUDDY_ADAPTOR,&(pDM_Odm->PADAPTER_temp));
  ODM_CmnInfoHook(pDM_Odm,ODM_CMNINFO_DMSP_IS_MASTER,&(pDM_Odm->u1Byte_temp));
  ODM_CmnInfoHook(pDM_Odm,ODM_CMNINFO_BT_OPERATION,&(pDM_Odm->u1Byte_temp));
  ODM_CmnInfoHook(pDM_Odm,ODM_CMNINFO_BT_DISABLE_EDCA,&(pDM_Odm->u1Byte_temp));
  */
  
  ODM_CmnInfoHook (pDM_Odm, ODM_CMNINFO_SCAN, & (pmlmepriv->bScanInProcess) );
  ODM_CmnInfoHook (pDM_Odm, ODM_CMNINFO_POWER_SAVING, & (pwrctrlpriv->bpower_saving) );
  ODM_CmnInfoInit (pDM_Odm, ODM_CMNINFO_RF_ANTENNA_TYPE, pHalData->TRxAntDivType);
  
  for (i = 0; i < NUM_STA; i++)
  {
    ODM_CmnInfoPtrArrayHook (pDM_Odm, ODM_CMNINFO_STA_STATUS, i, NULL);
  }
}

void
rtl8188e_InitHalDm (
  IN  PADAPTER  Adapter
)
{
  PHAL_DATA_TYPE  pHalData = GET_HAL_DATA (Adapter);
  struct dm_priv * pdmpriv = &pHalData->dmpriv;
  PDM_ODM_T   pDM_Odm = & (pHalData->odmpriv);
  u8  i;
  
  #ifdef CONFIG_USB_HCI
  dm_InitGPIOSetting (Adapter);
  #endif
  
  pdmpriv->DM_Type = DM_Type_ByDriver;
  pdmpriv->DMFlag = DYNAMIC_FUNC_DISABLE;
  
  Update_ODM_ComInfo_88E (Adapter);
  ODM_DMInit (pDM_Odm);
  
  Adapter->fix_rate = 0xFF;
  
}


VOID
rtl8188e_HalDmWatchDog (
  IN  PADAPTER  Adapter
)
{
  BOOLEAN   bFwCurrentInPSMode = _FALSE;
  BOOLEAN   bFwPSAwake = _TRUE;
  u8 hw_init_completed = _FALSE;
  PHAL_DATA_TYPE  pHalData = GET_HAL_DATA (Adapter);
  struct dm_priv * pdmpriv = &pHalData->dmpriv;
  PDM_ODM_T   pDM_Odm = & (pHalData->odmpriv);
  #ifdef CONFIG_CONCURRENT_MODE
  PADAPTER pbuddy_adapter = Adapter->pbuddy_adapter;
  #endif
  
  _func_enter_;
  
  hw_init_completed = Adapter->hw_init_completed;
  
  if (hw_init_completed == _FALSE)
  { goto skip_dm; }
  
  #ifdef CONFIG_LPS
  #ifdef CONFIG_CONCURRENT_MODE
  if (Adapter->iface_type != IFACE_PORT0 && pbuddy_adapter) {
    bFwCurrentInPSMode = pbuddy_adapter->pwrctrlpriv.bFwCurrentInPSMode;
    rtw_hal_get_hwreg (pbuddy_adapter, HW_VAR_FWLPS_RF_ON, (u8 *) (&bFwPSAwake) );
  }
  else
  #endif
  {
    bFwCurrentInPSMode = Adapter->pwrctrlpriv.bFwCurrentInPSMode;
    rtw_hal_get_hwreg (Adapter, HW_VAR_FWLPS_RF_ON, (u8 *) (&bFwPSAwake) );
  }
  #endif
  
  #ifdef CONFIG_P2P_PS
  if (Adapter->wdinfo.p2p_ps_mode)
  { bFwPSAwake = _FALSE; }
  #endif
  
  if ( (hw_init_completed == _TRUE)
       && ( (!bFwCurrentInPSMode) && bFwPSAwake) )
  {
    dm_CheckStatistics (Adapter);
    
    
    #ifdef CONFIG_PCI_HCI
    
    #endif
    
  }
  
  
  if (hw_init_completed == _TRUE)
  {
    u8  bLinked = _FALSE;
    
    #ifdef CONFIG_DISABLE_ODM
    pHalData->odmpriv.SupportAbility = 0;
    #endif
    
    if (rtw_linked_check (Adapter) )
    { bLinked = _TRUE; }
    
    #ifdef CONFIG_CONCURRENT_MODE
    if (pbuddy_adapter && rtw_linked_check (pbuddy_adapter) )
    { bLinked = _TRUE; }
    #endif
    
    ODM_CmnInfoUpdate (&pHalData->odmpriv , ODM_CMNINFO_LINK, bLinked);
    ODM_DMWatchdog (&pHalData->odmpriv);
    
  }
  
skip_dm:

  #ifdef CONFIG_PCI_HCI
  if (pHalData->bGpioHwWpsPbc)
  #endif
  {
  }
  return;
}

void rtl8188e_init_dm_priv (IN PADAPTER Adapter)
{
  PHAL_DATA_TYPE  pHalData = GET_HAL_DATA (Adapter);
  struct dm_priv * pdmpriv = &pHalData->dmpriv;
  PDM_ODM_T     podmpriv = &pHalData->odmpriv;
  _rtw_memset (pdmpriv, 0, sizeof (struct dm_priv) );
  Init_ODM_ComInfo_88E (Adapter);
  #ifdef CONFIG_SW_ANTENNA_DIVERSITY
  ODM_InitAllTimers (podmpriv );
  #endif
  ODM_InitDebugSetting (podmpriv);
}

void rtl8188e_deinit_dm_priv (IN PADAPTER Adapter)
{
  PHAL_DATA_TYPE  pHalData = GET_HAL_DATA (Adapter);
  struct dm_priv * pdmpriv = &pHalData->dmpriv;
  PDM_ODM_T     podmpriv = &pHalData->odmpriv;
  #ifdef CONFIG_SW_ANTENNA_DIVERSITY
  ODM_CancelAllTimers (podmpriv);
  #endif
}


#ifdef CONFIG_ANTENNA_DIVERSITY
void  AntDivCompare8188E (PADAPTER Adapter, WLAN_BSSID_EX * dst, WLAN_BSSID_EX * src)
{
  
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  if (0 != pHalData->AntDivCfg )
  {
    if (dst->Rssi >=  src->Rssi )
    {
      src->Rssi = dst->Rssi;
      src->PhyInfo.Optimum_antenna = dst->PhyInfo.Optimum_antenna;
    }
  }
}

u8 AntDivBeforeLink8188E (PADAPTER Adapter )
{

  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  PDM_ODM_T   pDM_Odm = &pHalData->odmpriv;
  SWAT_T  *  pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
  struct mlme_priv * pmlmepriv = & (Adapter->mlmepriv);
  
  if (pHalData->AntDivCfg == 0)
  {
    return _FALSE;
  }
  
  if (check_fwstate (pmlmepriv, _FW_LINKED) == _TRUE)
  {
    return _FALSE;
  }
  
  
  if (pDM_SWAT_Table->SWAS_NoLink_State == 0) {
    pDM_SWAT_Table->SWAS_NoLink_State = 1;
    pDM_SWAT_Table->CurAntenna = (pDM_SWAT_Table->CurAntenna == Antenna_A) ? Antenna_B : Antenna_A;
    
    rtw_antenna_select_cmd (Adapter, pDM_SWAT_Table->CurAntenna, _FALSE);
    return _TRUE;
  }
  else
  {
    pDM_SWAT_Table->SWAS_NoLink_State = 0;
    return _FALSE;
  }
  
}
#endif

