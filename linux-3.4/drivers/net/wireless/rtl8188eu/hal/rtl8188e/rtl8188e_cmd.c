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
#define _RTL8188E_CMD_C_

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>
#include <recv_osdep.h>
#include <cmd_osdep.h>
#include <mlme_osdep.h>
#include <rtw_byteorder.h>
#include <circ_buf.h>
#include <rtw_ioctl_set.h>

#include <rtl8188e_hal.h>

#define CONFIG_H2C_EF

#define RTL88E_MAX_H2C_BOX_NUMS 4
#define RTL88E_MAX_CMD_LEN  7
#define RTL88E_MESSAGE_BOX_SIZE   4
#define RTL88E_EX_MESSAGE_BOX_SIZE  4


static u8 _is_fw_read_cmd_down (_adapter * padapter, u8 msgbox_num)
{
  u8  read_down = _FALSE;
  int   retry_cnts = 100;
  
  u8 valid;
  
  
  do {
    valid = rtw_read8 (padapter, REG_HMETFR) & BIT (msgbox_num);
    if (0 == valid ) {
      read_down = _TRUE;
    }
    #ifdef CONFIG_WOWLAN
    rtw_msleep_os (2);
    #endif
  }
  while ( (!read_down) && (retry_cnts--) );
  
  return read_down;
  
}


/*****************************************
* H2C Msg format :
* 0x1DF - 0x1D0
*| 31 - 8 | 7-5    4 - 0  |
*| h2c_msg  |Class_ID CMD_ID  |
*
* Extend 0x1FF - 0x1F0
*|31 - 0    |
*|ext_msg|
******************************************/
static s32 FillH2CCmd_88E (PADAPTER padapter, u8 ElementID, u32 CmdLen, u8 * pCmdBuffer)
{
  u8 bcmd_down = _FALSE;
  s32 retry_cnts = 100;
  u8 h2c_box_num;
  u32 msgbox_addr;
  u32 msgbox_ex_addr;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  u8 cmd_idx, ext_cmd_len;
  u32 h2c_cmd = 0;
  u32 h2c_cmd_ex = 0;
  s32 ret = _FAIL;
  
  _func_enter_;
  
  padapter = GET_PRIMARY_ADAPTER (padapter);
  pHalData = GET_HAL_DATA (padapter);
  
  if (padapter->bFWReady == _FALSE)
  {
    DBG_8192C ("FillH2CCmd_88E(): return H2C cmd because fw is not ready\n");
    return ret;
  }
  
  _enter_critical_mutex (& (adapter_to_dvobj (padapter)->h2c_fwcmd_mutex), NULL);
  
  if (!pCmdBuffer) {
    goto exit;
  }
  if (CmdLen > RTL88E_MAX_CMD_LEN) {
    goto exit;
  }
  if (padapter->bSurpriseRemoved == _TRUE)
  { goto exit; }
  
  do {
    h2c_box_num = pHalData->LastHMEBoxNum;
    
    if (!_is_fw_read_cmd_down (padapter, h2c_box_num) ) {
      DBG_8192C (" fw read cmd failed...\n");
      goto exit;
    }
    
    * (u8 *) (&h2c_cmd) = ElementID;
    
    if (CmdLen <= 3)
    {
      _rtw_memcpy ( (u8 *) (&h2c_cmd) + 1, pCmdBuffer, CmdLen );
    }
    else {
      _rtw_memcpy ( (u8 *) (&h2c_cmd) + 1, pCmdBuffer, 3);
      ext_cmd_len = CmdLen - 3;
      _rtw_memcpy ( (u8 *) (&h2c_cmd_ex), pCmdBuffer + 3, ext_cmd_len );
      
      msgbox_ex_addr = REG_HMEBOX_EXT_0 + (h2c_box_num * RTL88E_EX_MESSAGE_BOX_SIZE);
      #ifdef CONFIG_H2C_EF
      for (cmd_idx = 0; cmd_idx < ext_cmd_len; cmd_idx++ ) {
        rtw_write8 (padapter, msgbox_ex_addr + cmd_idx, * ( (u8 *) (&h2c_cmd_ex) + cmd_idx) );
      }
      #else
      h2c_cmd_ex = le32_to_cpu ( h2c_cmd_ex );
      rtw_write32 (padapter, msgbox_ex_addr, h2c_cmd_ex);
      #endif
    }
    msgbox_addr = REG_HMEBOX_0 + (h2c_box_num * RTL88E_MESSAGE_BOX_SIZE);
    #ifdef CONFIG_H2C_EF
    for (cmd_idx = 0; cmd_idx < RTL88E_MESSAGE_BOX_SIZE; cmd_idx++ ) {
      rtw_write8 (padapter, msgbox_addr + cmd_idx, * ( (u8 *) (&h2c_cmd) + cmd_idx) );
    }
    #else
    h2c_cmd = le32_to_cpu ( h2c_cmd );
    rtw_write32 (padapter, msgbox_addr, h2c_cmd);
    #endif
    
    bcmd_down = _TRUE;
    
    
    pHalData->LastHMEBoxNum = (h2c_box_num + 1) % RTL88E_MAX_H2C_BOX_NUMS;
    
  }
  while ( (!bcmd_down) && (retry_cnts--) );
  
  ret = _SUCCESS;
  
exit:

  _exit_critical_mutex (& (adapter_to_dvobj (padapter)->h2c_fwcmd_mutex), NULL);
  
  _func_exit_;
  
  return ret;
}

u8 rtl8192c_h2c_msg_hdl (_adapter * padapter, unsigned char * pbuf)
{
  u8 ElementID, CmdLen;
  u8 * pCmdBuffer;
  struct cmd_msg_parm * pcmdmsg;
  
  if (!pbuf)
  { return H2C_PARAMETERS_ERROR; }
  
  pcmdmsg = (struct cmd_msg_parm *) pbuf;
  ElementID = pcmdmsg->eid;
  CmdLen = pcmdmsg->sz;
  pCmdBuffer = pcmdmsg->buf;
  
  FillH2CCmd_88E (padapter, ElementID, CmdLen, pCmdBuffer);
  
  return H2C_SUCCESS;
}
/*
#if defined(CONFIG_AUTOSUSPEND) && defined(SUPPORT_HW_RFOFF_DETECTED)
u8 rtl8192c_set_FwSelectSuspend_cmd(_adapter *padapter ,u8 bfwpoll, u16 period)
{
  u8  res=_SUCCESS;
  struct H2C_SS_RFOFF_PARAM param;
  DBG_8192C("==>%s bfwpoll(%x)\n",__FUNCTION__,bfwpoll);
  param.gpio_period = period;//Polling GPIO_11 period time
  param.ROFOn = (_TRUE == bfwpoll)?1:0;
  FillH2CCmd_88E(padapter, SELECTIVE_SUSPEND_ROF_CMD, sizeof(param), (u8*)(&param));
  return res;
}
#endif
*/
u8 rtl8188e_set_rssi_cmd (_adapter * padapter, u8 * param)
{
  u8  res = _SUCCESS;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  _func_enter_;
  
  if (pHalData->fw_ractrl == _TRUE) {
    #if 0
    * ( (u32 *) param ) = cpu_to_le32 ( * ( (u32 *) param ) );
    
    FillH2CCmd_88E (padapter, RSSI_SETTING_EID, 3, param);
    #endif
  }
  else {
    DBG_8192C ("==>%s fw dont support RA \n", __FUNCTION__);
    res = _FAIL;
  }
  
  _func_exit_;
  
  return res;
}

u8 rtl8188e_set_raid_cmd (_adapter * padapter, u32 mask)
{
  u8  buf[3];
  u8  res = _SUCCESS;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  _func_enter_;
  if (pHalData->fw_ractrl == _TRUE) {
    _rtw_memset (buf, 0, 3);
    mask = cpu_to_le32 ( mask );
    _rtw_memcpy (buf, &mask, 3);
    
    FillH2CCmd_88E (padapter, H2C_DM_MACID_CFG, 3, buf);
  }
  else {
    DBG_8192C ("==>%s fw dont support RA \n", __FUNCTION__);
    res = _FAIL;
  }
  
  _func_exit_;
  
  return res;
  
}

void rtl8188e_Add_RateATid (PADAPTER pAdapter, u32 bitmap, u8 arg, u8 rssi_level)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (pAdapter);
  
  u8 macid, init_rate, raid, shortGIrate = _FALSE;
  
  #ifdef CONFIG_CONCURRENT_MODE
  if (rtw_buddy_adapter_up (pAdapter) && pAdapter->adapter_type > PRIMARY_ADAPTER)
  { pHalData = GET_HAL_DATA (pAdapter->pbuddy_adapter); }
  #endif
  
  macid = arg & 0x1f;
  
  #ifdef CONFIG_ODM_REFRESH_RAMASK
  raid = (bitmap >> 28) & 0x0f;
  bitmap &= 0x0fffffff;
  
  if (rssi_level != DM_RATR_STA_INIT)
  { bitmap = ODM_Get_Rate_Bitmap (&pHalData->odmpriv, macid, bitmap, rssi_level); }
  
  bitmap |= ( (raid << 28) & 0xf0000000);
  #endif
  
  
  init_rate = get_highest_rate_idx (bitmap & 0x0fffffff) & 0x3f;
  
  shortGIrate = (arg & BIT (5) ) ? _TRUE : _FALSE;
  
  if (shortGIrate == _TRUE)
  { init_rate |= BIT (6); }
  
  
  
  raid = (bitmap >> 28) & 0x0f;
  
  bitmap &= 0x0fffffff;
  
  DBG_871X ("%s=> mac_id:%d , raid:%d , ra_bitmap=0x%x, shortGIrate=0x%02x\n",
            __FUNCTION__, macid , raid , bitmap, shortGIrate);
            
            
  #if(RATE_ADAPTIVE_SUPPORT == 1)
  ODM_RA_UpdateRateInfo_8188E (
    & (pHalData->odmpriv),
    macid,
    raid,
    bitmap,
    shortGIrate
  );
  #endif
  
}

void rtl8188e_set_FwPwrMode_cmd (PADAPTER padapter, u8 Mode)
{
  SETPWRMODE_PARM H2CSetPwrMode;
  struct pwrctrl_priv * pwrpriv = &padapter->pwrctrlpriv;
  u8  RLBM = 0;
  _func_enter_;
  
  DBG_871X ("%s: Mode=%d SmartPS=%d UAPSD=%d\n", __FUNCTION__,
            Mode, pwrpriv->smart_ps, padapter->registrypriv.uapsd_enable);
            
  switch (Mode)
  {
  case PS_MODE_ACTIVE:
    H2CSetPwrMode.Mode = 0;
    break;
  case PS_MODE_MIN:
    H2CSetPwrMode.Mode = 1;
    break;
  case PS_MODE_MAX:
    RLBM = 1;
    H2CSetPwrMode.Mode = 1;
    break;
  case PS_MODE_DTIM:
    RLBM = 2;
    H2CSetPwrMode.Mode = 1;
    break;
  case PS_MODE_UAPSD_WMM:
    H2CSetPwrMode.Mode = 2;
    break;
  default:
    H2CSetPwrMode.Mode = 0;
    break;
  }
  
  
  H2CSetPwrMode.SmartPS_RLBM = ( ( (pwrpriv->smart_ps << 4) & 0xf0) | (RLBM & 0x0f) );
  
  H2CSetPwrMode.AwakeInterval = 1;
  
  H2CSetPwrMode.bAllQueueUAPSD = padapter->registrypriv.uapsd_enable;
  
  if (Mode > 0)
  {
    H2CSetPwrMode.PwrState = 0x00;// AllON(0x0C), RFON(0x04), RFOFF(0x00)
    #ifdef CONFIG_EXT_CLK
    H2CSetPwrMode.Mode |= BIT (7);
    #endif
  }
  else
  { H2CSetPwrMode.PwrState = 0x0C; }// AllON(0x0C), RFON(0x04), RFOFF(0x00)
  
  FillH2CCmd_88E (padapter, H2C_PS_PWR_MODE, sizeof (H2CSetPwrMode), (u8 *) &H2CSetPwrMode);
  
  
  _func_exit_;
}

void rtl8188e_set_FwMediaStatus_cmd (PADAPTER padapter, u16 mstatus_rpt )
{
  u8 opmode, macid;
  u16 mst_rpt = cpu_to_le16 (mstatus_rpt);
  opmode = (u8) mst_rpt;
  macid = (u8) (mst_rpt >> 8)  ;
  
  DBG_871X ("### %s: MStatus=%x MACID=%d \n", __FUNCTION__, opmode, macid);
  FillH2CCmd_88E (padapter, H2C_COM_MEDIA_STATUS_RPT, sizeof (mst_rpt), (u8 *) &mst_rpt);
}

void ConstructBeacon (_adapter * padapter, u8 * pframe, u32 * pLength)
{
  struct rtw_ieee80211_hdr * pwlanhdr;
  u16     *    fctrl;
  u32         rate_len, pktlen;
  struct mlme_ext_priv * pmlmeext = & (padapter->mlmeextpriv);
  struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
  WLAN_BSSID_EX   *  cur_network = & (pmlmeinfo->network);
  u8  bc_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  
  
  
  pwlanhdr = (struct rtw_ieee80211_hdr *) pframe;
  
  fctrl = & (pwlanhdr->frame_ctl);
  * (fctrl) = 0;
  
  _rtw_memcpy (pwlanhdr->addr1, bc_addr, ETH_ALEN);
  _rtw_memcpy (pwlanhdr->addr2, myid (& (padapter->eeprompriv) ), ETH_ALEN);
  _rtw_memcpy (pwlanhdr->addr3, get_my_bssid (cur_network), ETH_ALEN);
  
  SetSeqNum (pwlanhdr, 0/*pmlmeext->mgnt_seq*/);
  SetFrameSubType (pframe, WIFI_BEACON);
  
  pframe += sizeof (struct rtw_ieee80211_hdr_3addr);
  pktlen = sizeof (struct rtw_ieee80211_hdr_3addr);
  
  pframe += 8;
  pktlen += 8;
  
  _rtw_memcpy (pframe, (unsigned char *) (rtw_get_beacon_interval_from_ie (cur_network->IEs) ), 2);
  
  pframe += 2;
  pktlen += 2;
  
  _rtw_memcpy (pframe, (unsigned char *) (rtw_get_capability_from_ie (cur_network->IEs) ), 2);
  
  pframe += 2;
  pktlen += 2;
  
  if ( (pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE)
  {
    pktlen += cur_network->IELength - sizeof (NDIS_802_11_FIXED_IEs);
    _rtw_memcpy (pframe, cur_network->IEs + sizeof (NDIS_802_11_FIXED_IEs), pktlen);
    
    goto _ConstructBeacon;
  }
  
  
  pframe = rtw_set_ie (pframe, _SSID_IE_, cur_network->Ssid.SsidLength, cur_network->Ssid.Ssid, &pktlen);
  
  rate_len = rtw_get_rateset_len (cur_network->SupportedRates);
  pframe = rtw_set_ie (pframe, _SUPPORTEDRATES_IE_, ( (rate_len > 8) ? 8 : rate_len), cur_network->SupportedRates, &pktlen);
  
  pframe = rtw_set_ie (pframe, _DSSET_IE_, 1, (unsigned char *) & (cur_network->Configuration.DSConfig), &pktlen);
  
  if ( (pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE)
  {
    u32 ATIMWindow;
    ATIMWindow = 0;
    pframe = rtw_set_ie (pframe, _IBSS_PARA_IE_, 2, (unsigned char *) (&ATIMWindow), &pktlen);
  }
  
  
  
  
  if (rate_len > 8)
  {
    pframe = rtw_set_ie (pframe, _EXT_SUPPORTEDRATES_IE_, (rate_len - 8), (cur_network->SupportedRates + 8), &pktlen);
  }
  
  
  
_ConstructBeacon:

  if ( (pktlen + TXDESC_SIZE) > 512)
  {
    DBG_871X ("beacon frame too large\n");
    return;
  }
  
  *pLength = pktlen;
  
  
}

void ConstructPSPoll (_adapter * padapter, u8 * pframe, u32 * pLength)
{
  struct rtw_ieee80211_hdr * pwlanhdr;
  u16     *    fctrl;
  u32         pktlen;
  struct mlme_ext_priv * pmlmeext = & (padapter->mlmeextpriv);
  struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
  
  
  pwlanhdr = (struct rtw_ieee80211_hdr *) pframe;
  
  fctrl = & (pwlanhdr->frame_ctl);
  * (fctrl) = 0;
  SetPwrMgt (fctrl);
  SetFrameSubType (pframe, WIFI_PSPOLL);
  
  SetDuration (pframe, (pmlmeinfo->aid | 0xc000) );
  
  _rtw_memcpy (pwlanhdr->addr1, get_my_bssid (& (pmlmeinfo->network) ), ETH_ALEN);
  
  _rtw_memcpy (pwlanhdr->addr2, myid (& (padapter->eeprompriv) ), ETH_ALEN);
  
  *pLength = 16;
}

void ConstructNullFunctionData (
  PADAPTER padapter,
  u8  *  pframe,
  u32  * pLength,
  u8  *  StaAddr,
  u8    bQoS,
  u8    AC,
  u8    bEosp,
  u8    bForcePowerSave)
{
  struct rtw_ieee80211_hdr * pwlanhdr;
  u16      *     fctrl;
  u32           pktlen;
  struct mlme_priv  *  pmlmepriv = &padapter->mlmepriv;
  struct wlan_network  * cur_network = &pmlmepriv->cur_network;
  struct mlme_ext_priv * pmlmeext = & (padapter->mlmeextpriv);
  struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
  
  
  
  pwlanhdr = (struct rtw_ieee80211_hdr *) pframe;
  
  fctrl = &pwlanhdr->frame_ctl;
  * (fctrl) = 0;
  if (bForcePowerSave)
  {
    SetPwrMgt (fctrl);
  }
  
  switch (cur_network->network.InfrastructureMode)
  {
  case Ndis802_11Infrastructure:
    SetToDs (fctrl);
    _rtw_memcpy (pwlanhdr->addr1, get_my_bssid (& (pmlmeinfo->network) ), ETH_ALEN);
    _rtw_memcpy (pwlanhdr->addr2, myid (& (padapter->eeprompriv) ), ETH_ALEN);
    _rtw_memcpy (pwlanhdr->addr3, StaAddr, ETH_ALEN);
    break;
  case Ndis802_11APMode:
    SetFrDs (fctrl);
    _rtw_memcpy (pwlanhdr->addr1, StaAddr, ETH_ALEN);
    _rtw_memcpy (pwlanhdr->addr2, get_my_bssid (& (pmlmeinfo->network) ), ETH_ALEN);
    _rtw_memcpy (pwlanhdr->addr3, myid (& (padapter->eeprompriv) ), ETH_ALEN);
    break;
  case Ndis802_11IBSS:
  default:
    _rtw_memcpy (pwlanhdr->addr1, StaAddr, ETH_ALEN);
    _rtw_memcpy (pwlanhdr->addr2, myid (& (padapter->eeprompriv) ), ETH_ALEN);
    _rtw_memcpy (pwlanhdr->addr3, get_my_bssid (& (pmlmeinfo->network) ), ETH_ALEN);
    break;
  }
  
  SetSeqNum (pwlanhdr, 0);
  
  if (bQoS == _TRUE) {
    struct rtw_ieee80211_hdr_3addr_qos * pwlanqoshdr;
    
    SetFrameSubType (pframe, WIFI_QOS_DATA_NULL);
    
    pwlanqoshdr = (struct rtw_ieee80211_hdr_3addr_qos *) pframe;
    SetPriority (&pwlanqoshdr->qc, AC);
    SetEOSP (&pwlanqoshdr->qc, bEosp);
    
    pktlen = sizeof (struct rtw_ieee80211_hdr_3addr_qos);
  }
  else {
    SetFrameSubType (pframe, WIFI_DATA_NULL);
    
    pktlen = sizeof (struct rtw_ieee80211_hdr_3addr);
  }
  
  *pLength = pktlen;
}

void ConstructProbeRsp (_adapter * padapter, u8 * pframe, u32 * pLength, u8 * StaAddr, BOOLEAN bHideSSID)
{
  struct rtw_ieee80211_hdr * pwlanhdr;
  u16     *    fctrl;
  u8     *     mac, *bssid;
  u32         pktlen;
  struct mlme_ext_priv * pmlmeext = & (padapter->mlmeextpriv);
  struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
  WLAN_BSSID_EX   *  cur_network = & (pmlmeinfo->network);
  
  
  
  pwlanhdr = (struct rtw_ieee80211_hdr *) pframe;
  
  mac = myid (& (padapter->eeprompriv) );
  bssid = cur_network->MacAddress;
  
  fctrl = & (pwlanhdr->frame_ctl);
  * (fctrl) = 0;
  _rtw_memcpy (pwlanhdr->addr1, StaAddr, ETH_ALEN);
  _rtw_memcpy (pwlanhdr->addr2, mac, ETH_ALEN);
  _rtw_memcpy (pwlanhdr->addr3, bssid, ETH_ALEN);
  
  SetSeqNum (pwlanhdr, 0);
  SetFrameSubType (fctrl, WIFI_PROBERSP);
  
  pktlen = sizeof (struct rtw_ieee80211_hdr_3addr);
  pframe += pktlen;
  
  if (cur_network->IELength > MAX_IE_SZ)
  { return; }
  
  _rtw_memcpy (pframe, cur_network->IEs, cur_network->IELength);
  pframe += cur_network->IELength;
  pktlen += cur_network->IELength;
  
  *pLength = pktlen;
}

VOID
CheckFwRsvdPageContent (
  IN  PADAPTER    Adapter
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  u32 MaxBcnPageNum;
  
  if (pHalData->FwRsvdPageStartOffset != 0)
  {
    /*MaxBcnPageNum = PageNum_128(pMgntInfo->MaxBeaconSize);
    RT_ASSERT((MaxBcnPageNum <= pHalData->FwRsvdPageStartOffset),
      ("CheckFwRsvdPageContent(): The reserved page content has been"\
      "destroyed by beacon!!! MaxBcnPageNum(%d) FwRsvdPageStartOffset(%d)\n!",
      MaxBcnPageNum, pHalData->FwRsvdPageStartOffset));*/
  }
}

static void SetFwRsvdPagePkt (PADAPTER padapter, BOOLEAN bDLFinished)
{
  PHAL_DATA_TYPE pHalData;
  struct xmit_frame * pmgntframe;
  struct pkt_attrib * pattrib;
  struct xmit_priv * pxmitpriv;
  struct mlme_ext_priv * pmlmeext;
  struct mlme_ext_info * pmlmeinfo;
  u32 BeaconLength, ProbeRspLength, PSPollLength;
  u32 NullDataLength, QosNullLength, BTQosNullLength;
  u8 * ReservedPagePacket;
  u8  PageNum, PageNeed, TxDescLen;
  u16 BufIndex;
  u32 TotalPacketLen;
  RSVDPAGE_LOC  RsvdPageLoc;
  
  
  DBG_871X ("%s\n", __FUNCTION__);
  
  ReservedPagePacket = (u8 *) rtw_zmalloc (1000);
  if (ReservedPagePacket == NULL) {
    DBG_871X ("%s: alloc ReservedPagePacket fail!\n", __FUNCTION__);
    return;
  }
  
  pHalData = GET_HAL_DATA (padapter);
  pxmitpriv = &padapter->xmitpriv;
  pmlmeext = &padapter->mlmeextpriv;
  pmlmeinfo = &pmlmeext->mlmext_info;
  
  TxDescLen = TXDESC_SIZE;
  PageNum = 0;
  
  BufIndex = TXDESC_OFFSET;
  ConstructBeacon (padapter, &ReservedPagePacket[BufIndex], &BeaconLength);
  
  PageNeed = (u8) PageNum_128 (TxDescLen + BeaconLength);
  if (PageNeed == 1)
  { PageNeed += 1; }
  PageNum += PageNeed;
  pHalData->FwRsvdPageStartOffset = PageNum;
  
  BufIndex += PageNeed * 128;
  
  RsvdPageLoc.LocPsPoll = PageNum;
  ConstructPSPoll (padapter, &ReservedPagePacket[BufIndex], &PSPollLength);
  rtl8188e_fill_fake_txdesc (padapter, &ReservedPagePacket[BufIndex - TxDescLen], PSPollLength, _TRUE, _FALSE);
  
  PageNeed = (u8) PageNum_128 (TxDescLen + PSPollLength);
  PageNum += PageNeed;
  
  BufIndex += PageNeed * 128;
  
  RsvdPageLoc.LocNullData = PageNum;
  ConstructNullFunctionData (
    padapter,
    &ReservedPagePacket[BufIndex],
    &NullDataLength,
    get_my_bssid (&pmlmeinfo->network),
    _FALSE, 0, 0, _FALSE);
  rtl8188e_fill_fake_txdesc (padapter, &ReservedPagePacket[BufIndex - TxDescLen], NullDataLength, _FALSE, _FALSE);
  
  PageNeed = (u8) PageNum_128 (TxDescLen + NullDataLength);
  PageNum += PageNeed;
  
  BufIndex += PageNeed * 128;
  
  RsvdPageLoc.LocProbeRsp = PageNum;
  ConstructProbeRsp (
    padapter,
    &ReservedPagePacket[BufIndex],
    &ProbeRspLength,
    get_my_bssid (&pmlmeinfo->network),
    _FALSE);
  rtl8188e_fill_fake_txdesc (padapter, &ReservedPagePacket[BufIndex - TxDescLen], ProbeRspLength, _FALSE, _FALSE);
  
  PageNeed = (u8) PageNum_128 (TxDescLen + ProbeRspLength);
  PageNum += PageNeed;
  
  BufIndex += PageNeed * 128;
  
  RsvdPageLoc.LocQosNull = PageNum;
  ConstructNullFunctionData (
    padapter,
    &ReservedPagePacket[BufIndex],
    &QosNullLength,
    get_my_bssid (&pmlmeinfo->network),
    _TRUE, 0, 0, _FALSE);
  rtl8188e_fill_fake_txdesc (padapter, &ReservedPagePacket[BufIndex - TxDescLen], QosNullLength, _FALSE, _FALSE);
  
  PageNeed = (u8) PageNum_128 (TxDescLen + QosNullLength);
  PageNum += PageNeed;
  
  TotalPacketLen = BufIndex + QosNullLength;
  /*
    BufIndex += PageNeed*128;
  
    RsvdPageLoc.LocBTQosNull = PageNum;
    ConstructNullFunctionData(
      padapter,
      &ReservedPagePacket[BufIndex],
      &BTQosNullLength,
      get_my_bssid(&pmlmeinfo->network),
      _TRUE, 0, 0, _FALSE);
    rtl8188e_fill_fake_txdesc(padapter, &ReservedPagePacket[BufIndex-TxDescLen], BTQosNullLength, _FALSE, _TRUE);
  
    TotalPacketLen = BufIndex + BTQosNullLength;
  */
  
  pmgntframe = alloc_mgtxmitframe (pxmitpriv);
  if (pmgntframe == NULL)
  { goto exit; }
  
  pattrib = &pmgntframe->attrib;
  update_mgntframe_attrib (padapter, pattrib);
  pattrib->qsel = 0x10;
  pattrib->pktlen = pattrib->last_txcmdsz = TotalPacketLen - TXDESC_OFFSET;
  _rtw_memcpy (pmgntframe->buf_addr, ReservedPagePacket, TotalPacketLen);
  
  rtw_hal_mgnt_xmit (padapter, pmgntframe);
  
  DBG_871X ("%s: Set RSVD page location to Fw\n", __FUNCTION__);
  FillH2CCmd_88E (padapter, H2C_COM_RSVD_PAGE, sizeof (RsvdPageLoc), (u8 *) &RsvdPageLoc);
  
exit:
  rtw_mfree (ReservedPagePacket, 1000);
}

void rtl8188e_set_FwJoinBssReport_cmd (PADAPTER padapter, u8 mstatus)
{
  JOINBSSRPT_PARM JoinBssRptParm;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  struct mlme_ext_priv * pmlmeext = & (padapter->mlmeextpriv);
  struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
  BOOLEAN   bSendBeacon = _FALSE;
  BOOLEAN   bcn_valid = _FALSE;
  u8  DLBcnCount = 0;
  u32 poll = 0;
  
  _func_enter_;
  
  DBG_871X ("%s mstatus(%x)\n", __FUNCTION__, mstatus);
  
  if (mstatus == 1)
  {
    rtw_write16 (padapter, REG_BCN_PSR_RPT, (0xC000 | pmlmeinfo->aid) );
    
    pHalData->RegCR_1 |= BIT0;
    rtw_write8 (padapter,  REG_CR + 1, pHalData->RegCR_1);
    
    rtw_write8 (padapter, REG_BCN_CTRL, rtw_read8 (padapter, REG_BCN_CTRL) & (~BIT (3) ) );
    rtw_write8 (padapter, REG_BCN_CTRL, rtw_read8 (padapter, REG_BCN_CTRL) | BIT (4) );
    
    if (pHalData->RegFwHwTxQCtrl & BIT6)
    {
      DBG_871X ("HalDownloadRSVDPage(): There is an Adapter is sending beacon.\n");
      bSendBeacon = _TRUE;
    }
    
    rtw_write8 (padapter, REG_FWHW_TXQ_CTRL + 2, (pHalData->RegFwHwTxQCtrl & (~BIT6) ) );
    pHalData->RegFwHwTxQCtrl &= (~BIT6);
    
    rtw_hal_set_hwreg (padapter, HW_VAR_BCN_VALID, NULL);
    DLBcnCount = 0;
    poll = 0;
    do
    {
      SetFwRsvdPagePkt (padapter, _FALSE);
      DLBcnCount++;
      do
      {
        rtw_yield_os();
        rtw_hal_get_hwreg (padapter, HW_VAR_BCN_VALID, (u8 *) (&bcn_valid) );
        poll++;
      }
      while (!bcn_valid && (poll % 10) != 0 && !padapter->bSurpriseRemoved && !padapter->bDriverStopped);
      
    }
    while (!bcn_valid && DLBcnCount <= 100 && !padapter->bSurpriseRemoved && !padapter->bDriverStopped);
    
    if (padapter->bSurpriseRemoved || padapter->bDriverStopped)
    {
    }
    else
      if (!bcn_valid)
      { DBG_871X ("%s: 1 Download RSVD page failed! DLBcnCount:%u, poll:%u\n", __FUNCTION__ , DLBcnCount, poll); }
      else
      { DBG_871X ("%s: 1 Download RSVD success! DLBcnCount:%u, poll:%u\n", __FUNCTION__, DLBcnCount, poll); }
    if (0)
    {
      if (bSendBeacon)
      {
        rtw_hal_set_hwreg (padapter, HW_VAR_BCN_VALID, NULL);
        DLBcnCount = 0;
        poll = 0;
        do
        {
          SetFwRsvdPagePkt (padapter, _TRUE);
          DLBcnCount++;
          
          do
          {
            rtw_yield_os();
            rtw_hal_get_hwreg (padapter, HW_VAR_BCN_VALID, (u8 *) (&bcn_valid) );
            poll++;
          }
          while (!bcn_valid && (poll % 10) != 0 && !padapter->bSurpriseRemoved && !padapter->bDriverStopped);
        }
        while (!bcn_valid && DLBcnCount <= 100 && !padapter->bSurpriseRemoved && !padapter->bDriverStopped);
        
        if (padapter->bSurpriseRemoved || padapter->bDriverStopped)
        {
        }
        else
          if (!bcn_valid)
          { DBG_871X ("%s: 2 Download RSVD page failed! DLBcnCount:%u, poll:%u\n", __FUNCTION__ , DLBcnCount, poll); }
          else
          { DBG_871X ("%s: 2 Download RSVD success! DLBcnCount:%u, poll:%u\n", __FUNCTION__, DLBcnCount, poll); }
      }
    }
    
    rtw_write8 (padapter, REG_BCN_CTRL, rtw_read8 (padapter, REG_BCN_CTRL) | BIT (3) );
    rtw_write8 (padapter, REG_BCN_CTRL, rtw_read8 (padapter, REG_BCN_CTRL) & (~BIT (4) ) );
    
    if (bSendBeacon)
    {
      rtw_write8 (padapter, REG_FWHW_TXQ_CTRL + 2, (pHalData->RegFwHwTxQCtrl | BIT6) );
      pHalData->RegFwHwTxQCtrl |= BIT6;
    }
    
    if (bcn_valid)
    {
      rtw_hal_set_hwreg (padapter, HW_VAR_BCN_VALID, NULL);
      DBG_871X ("Set RSVD page location to Fw.\n");
    }
    
    {
      pHalData->RegCR_1 &= (~BIT0);
      rtw_write8 (padapter,  REG_CR + 1, pHalData->RegCR_1);
    }
  }
  #ifdef CONFIG_WOWLAN
  if (padapter->pwrctrlpriv.wowlan_mode) {
    JoinBssRptParm.OpMode = mstatus;
    JoinBssRptParm.MacID = 0;
    FillH2CCmd_88E (padapter, H2C_COM_MEDIA_STATUS_RPT, sizeof (JoinBssRptParm), (u8 *) &JoinBssRptParm);
    DBG_871X_LEVEL (_drv_info_, "%s opmode:%d MacId:%d\n", __func__, JoinBssRptParm.OpMode, JoinBssRptParm.MacID);
  }
  else {
    DBG_871X_LEVEL (_drv_info_, "%s wowlan_mode is off\n", __func__);
  }
  #endif
  _func_exit_;
}

#ifdef CONFIG_P2P_PS
void rtl8188e_set_p2p_ps_offload_cmd (_adapter * padapter, u8 p2p_ps_state)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  struct pwrctrl_priv  * pwrpriv = &padapter->pwrctrlpriv;
  struct wifidirect_info * pwdinfo = & ( padapter->wdinfo );
  struct P2P_PS_Offload_t * p2p_ps_offload = &pHalData->p2p_ps_offload;
  u8  i;
  
  _func_enter_;
  
  #if 1
  switch (p2p_ps_state)
  {
  case P2P_PS_DISABLE:
    DBG_8192C ("P2P_PS_DISABLE \n");
    _rtw_memset (p2p_ps_offload, 0 , 1);
    break;
  case P2P_PS_ENABLE:
    DBG_8192C ("P2P_PS_ENABLE \n");
    if ( pwdinfo->ctwindow > 0 )
    {
      p2p_ps_offload->CTWindow_En = 1;
      rtw_write8 (padapter, REG_P2P_CTWIN, pwdinfo->ctwindow);
    }
    
    for ( i = 0 ; i < pwdinfo->noa_num ; i++)
    {
      rtw_write8 (padapter, REG_NOA_DESC_SEL, (i << 4) );
      if (i == 0)
      { p2p_ps_offload->NoA0_En = 1; }
      else
      { p2p_ps_offload->NoA1_En = 1; }
      
      rtw_write32 (padapter, REG_NOA_DESC_DURATION, pwdinfo->noa_duration[i]);
      
      rtw_write32 (padapter, REG_NOA_DESC_INTERVAL, pwdinfo->noa_interval[i]);
      
      rtw_write32 (padapter, REG_NOA_DESC_START, pwdinfo->noa_start_time[i]);
      
      rtw_write8 (padapter, REG_NOA_DESC_COUNT, pwdinfo->noa_count[i]);
    }
    
    if ( (pwdinfo->opp_ps == 1) || (pwdinfo->noa_num > 0) )
    {
      rtw_write8 (padapter, REG_DUAL_TSF_RST, BIT (4) );
      
      p2p_ps_offload->Offload_En = 1;
      
      if (pwdinfo->role == P2P_ROLE_GO)
      {
        p2p_ps_offload->role = 1;
        p2p_ps_offload->AllStaSleep = 0;
      }
      else
      {
        p2p_ps_offload->role = 0;
      }
      
      p2p_ps_offload->discovery = 0;
    }
    break;
  case P2P_PS_SCAN:
    DBG_8192C ("P2P_PS_SCAN \n");
    p2p_ps_offload->discovery = 1;
    break;
  case P2P_PS_SCAN_DONE:
    DBG_8192C ("P2P_PS_SCAN_DONE \n");
    p2p_ps_offload->discovery = 0;
    pwdinfo->p2p_ps_state = P2P_PS_ENABLE;
    break;
  default:
    break;
  }
  
  FillH2CCmd_88E (padapter, H2C_PS_P2P_OFFLOAD, 1, (u8 *) p2p_ps_offload);
  #endif
  
  _func_exit_;
  
}
#endif

#ifdef CONFIG_TSF_RESET_OFFLOAD
/*
  ask FW to Reset sync register at Beacon early interrupt
*/
u8 rtl8188e_reset_tsf (_adapter * padapter, u8 reset_port )
{
  u8  buf[2];
  u8  res = _SUCCESS;
  
  s32 ret;
  _func_enter_;
  if (IFACE_PORT0 == reset_port) {
    buf[0] = 0x1; buf[1] = 0;
  }
  else {
    buf[0] = 0x0; buf[1] = 0x1;
  }
  
  ret = FillH2CCmd_88E (padapter, H2C_RESET_TSF, 2, buf);
  
  _func_exit_;
  
  return res;
}

int reset_tsf (PADAPTER Adapter, u8 reset_port )
{
  u8 reset_cnt_before = 0, reset_cnt_after = 0, loop_cnt = 0;
  u32 reg_reset_tsf_cnt = (IFACE_PORT0 == reset_port) ?
                          REG_FW_RESET_TSF_CNT_0 : REG_FW_RESET_TSF_CNT_1;
  u32 reg_bcncrtl = (IFACE_PORT0 == reset_port) ?
                    REG_BCN_CTRL_1 : REG_BCN_CTRL;
                    
  rtw_scan_abort (Adapter->pbuddy_adapter); /*  site survey will cause reset_tsf fail */
  reset_cnt_after = reset_cnt_before = rtw_read8 (Adapter, reg_reset_tsf_cnt);
  rtl8188e_reset_tsf (Adapter, reset_port);
  
  while ( (reset_cnt_after == reset_cnt_before ) && (loop_cnt < 10) ) {
    rtw_msleep_os (100);
    loop_cnt++;
    reset_cnt_after = rtw_read8 (Adapter, reg_reset_tsf_cnt);
  }
  
  return (loop_cnt >= 10) ? _FAIL : _TRUE;
}


#endif 

#ifdef CONFIG_WOWLAN
void rtl8188es_set_wowlan_cmd (_adapter * padapter, u8 enable)
{
  u8    res = _SUCCESS;
  u32   test = 0;
  struct recv_priv * precvpriv = &padapter->recvpriv;
  SETWOWLAN_PARM    pwowlan_parm;
  SETAOAC_GLOBAL_INFO paoac_global_info_parm;
  struct pwrctrl_priv * pwrpriv = &padapter->pwrctrlpriv;
  
  _func_enter_;
  DBG_871X_LEVEL (_drv_always_, "+%s+\n", __func__);
  
  pwowlan_parm.mode = 0;
  pwowlan_parm.gpio_index = 0;
  pwowlan_parm.gpio_duration = 0;
  pwowlan_parm.second_mode = 0;
  pwowlan_parm.reserve = 0;
  
  if (enable) {
  
    pwowlan_parm.mode |= FW_WOWLAN_FUN_EN;
    pwrpriv->wowlan_magic = _TRUE;
    pwrpriv->wowlan_unicast = _TRUE;
    
    if (pwrpriv->wowlan_pattern == _TRUE) {
      pwowlan_parm.mode |= FW_WOWLAN_PATTERN_MATCH;
      DBG_871X_LEVEL (_drv_info_, "%s 2.pwowlan_parm.mode=0x%x \n", __FUNCTION__, pwowlan_parm.mode );
    }
    if (pwrpriv->wowlan_magic == _TRUE) {
      pwowlan_parm.mode |= FW_WOWLAN_MAGIC_PKT;
      DBG_871X_LEVEL (_drv_info_, "%s 3.pwowlan_parm.mode=0x%x \n", __FUNCTION__, pwowlan_parm.mode );
    }
    if (pwrpriv->wowlan_unicast == _TRUE) {
      pwowlan_parm.mode |= FW_WOWLAN_UNICAST;
      DBG_871X_LEVEL (_drv_info_, "%s 4.pwowlan_parm.mode=0x%x \n", __FUNCTION__, pwowlan_parm.mode );
    }
    
    if (! (padapter->pwrctrlpriv.wowlan_wake_reason & FWDecisionDisconnect) )
    { rtl8188e_set_FwJoinBssReport_cmd (padapter, 1); }
    else
    { DBG_871X_LEVEL (_drv_always_, "%s, disconnected, no FwJoinBssReport\n", __FUNCTION__); }
    rtw_msleep_os (2);
    
    pwowlan_parm.mode |= FW_WOWLAN_REKEY_WAKEUP;
    pwowlan_parm.mode |= FW_WOWLAN_DEAUTH_WAKEUP;
    
    #ifdef CONFIG_USB_HCI
    pwowlan_parm.gpio_index = 0x0;
    #endif
    
    #ifdef CONFIG_SDIO_HCI
    pwowlan_parm.gpio_index = 0x80;
    #endif
    
    DBG_871X_LEVEL (_drv_info_, "%s 5.pwowlan_parm.mode=0x%x \n", __FUNCTION__, pwowlan_parm.mode);
    DBG_871X_LEVEL (_drv_info_, "%s 6.pwowlan_parm.index=0x%x \n", __FUNCTION__, pwowlan_parm.gpio_index);
    res = FillH2CCmd_88E (padapter, H2C_COM_WWLAN, 2, (u8 *) &pwowlan_parm);
    
    rtw_msleep_os (2);
    
    pwowlan_parm.mode = 1;
    pwowlan_parm.gpio_index = 0;
    pwowlan_parm.gpio_duration = 0;
    FillH2CCmd_88E (padapter, H2C_COM_DISCNT_DECISION, 3, (u8 *) &pwowlan_parm);
    
    pwowlan_parm.mode = 1;
    pwowlan_parm.gpio_index = 10;
    res = FillH2CCmd_88E (padapter, H2C_COM_KEEP_ALIVE, 2, (u8 *) &pwowlan_parm);
    
    rtw_msleep_os (2);
    paoac_global_info_parm.pairwiseEncAlg =
      padapter->securitypriv.dot11PrivacyAlgrthm;
    paoac_global_info_parm.groupEncAlg =
      padapter->securitypriv.dot118021XGrpPrivacy;
    res = FillH2CCmd_88E (padapter, H2C_COM_AOAC_GLOBAL_INFO, 2, (u8 *) &paoac_global_info_parm);
    
    rtw_msleep_os (2);
    pwowlan_parm.mode = 1;
    pwowlan_parm.gpio_index = 0;
    pwowlan_parm.gpio_duration = 0;
    res = FillH2CCmd_88E (padapter, H2C_COM_REMOTE_WAKE_CTRL, 3, (u8 *) &pwowlan_parm);
  }
  else {
    pwrpriv->wowlan_magic = _FALSE;
    res = FillH2CCmd_88E (padapter, H2C_COM_WWLAN, 2, (u8 *) &pwowlan_parm);
    rtw_msleep_os (2);
    res = FillH2CCmd_88E (padapter, H2C_COM_REMOTE_WAKE_CTRL, 3, (u8 *) &pwowlan_parm);
  }
  _func_exit_;
  DBG_871X_LEVEL (_drv_always_, "-%s res:%d-\n", __func__, res);
  return ;
}
#endif 