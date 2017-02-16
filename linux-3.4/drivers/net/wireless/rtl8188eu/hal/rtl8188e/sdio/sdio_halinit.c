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
#define _SDIO_HALINIT_C_

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>

#ifndef CONFIG_SDIO_HCI
#error "CONFIG_SDIO_HCI shall be on!\n"
#endif

#include <rtw_efuse.h>
#include <rtl8188e_hal.h>
#include <rtl8188e_led.h>
#include <HalPwrSeqCmd.h>
#include <Hal8188EPwrSeq.h>
#include <sdio_ops.h>

#ifdef CONFIG_EFUSE_CONFIG_FILE
#include <linux/fs.h>
#include <asm/uaccess.h>
#endif


/*
 * Description:
 *  Call this function to make sure power on successfully
 *
 * Return:
 *  _SUCCESS  enable success
 *  _FAIL enable fail
 */

static int PowerOnCheck (PADAPTER padapter)
{
  u32 val_offset0, val_offset1, val_offset2, val_offset3;
  u32 val_mix = 0;
  u32 res = 0;
  u8  ret = _FAIL;
  int index = 0;
  
  val_offset0 = rtw_read8 (padapter, REG_CR);
  val_offset1 = rtw_read8 (padapter, REG_CR + 1);
  val_offset2 = rtw_read8 (padapter, REG_CR + 2);
  val_offset3 = rtw_read8 (padapter, REG_CR + 3);
  
  if (val_offset0 == 0xEA || val_offset1 == 0xEA ||
      val_offset2 == 0xEA || val_offset3 == 0xEA) {
    DBG_871X ("%s: power on fail, do Power on again\n", __func__);
    return ret;
  }
  
  val_mix = val_offset3 << 24 | val_mix;
  val_mix = val_offset2 << 16 | val_mix;
  val_mix = val_offset1 << 8 | val_mix;
  val_mix = val_offset0 | val_mix;
  
  res = rtw_read32 (padapter, REG_CR);
  
  DBG_871X ("%s: val_mix:0x%08x, res:0x%08x\n", __func__, val_mix, res);
  
  while (index < 100) {
    if (res == val_mix) {
      DBG_871X ("%s: 0x100 the result of cmd52 and cmd53 is the same.\n", __func__);
      ret = _SUCCESS;
      break;
    }
    else {
      DBG_871X ("%s: 0x100 cmd52 and cmd53 is not the same(index:%d).\n", __func__, index);
      res = rtw_read32 (padapter, REG_CR);
      index ++;
      ret = _FAIL;
    }
  }
  
  if (ret) {
    index = 0;
    while (index < 100) {
      rtw_write32 (padapter, 0x1B8, 0x12345678);
      res = rtw_read32 (padapter, 0x1B8);
      if (res == 0x12345678) {
        DBG_871X ("%s: 0x1B8 test Pass.\n", __func__);
        ret = _SUCCESS;
        break;
      }
      else {
        index ++;
        DBG_871X ("%s: 0x1B8 test Fail(index: %d).\n", __func__, index);
        ret = _FAIL;
      }
    }
  }
  else {
    DBG_871X ("%s: fail at cmd52, cmd53.\n", __func__);
  }
  return ret;
}

#ifdef CONFIG_EXT_CLK
void EnableGpio5ClockReq (PADAPTER Adapter, u8 in_interrupt, u32 Enable)
{
  u32 value32;
  HAL_DATA_TYPE * pHalData;
  
  pHalData = GET_HAL_DATA (Adapter);
  if (IS_D_CUT (pHalData->VersionID) )
  { return; }
  
  
  if (in_interrupt)
  { value32 = _sdio_read32 (Adapter, REG_GPIO_PIN_CTRL); }
  else
  { value32 = rtw_read32 (Adapter, REG_GPIO_PIN_CTRL); }
  
  if (Enable)
  { value32 |= BIT (13); }
  else
  { value32 &= ~BIT (13); }
  
  value32 |= BIT (21);
  
  
  if (in_interrupt)
  { _sdio_write32 (Adapter, REG_GPIO_PIN_CTRL, value32); }
  else
  { rtw_write32 (Adapter, REG_GPIO_PIN_CTRL, value32); }
  
}

void _InitClockTo26MHz (
  IN  PADAPTER Adapter
)
{
  u8 u1temp = 0;
  HAL_DATA_TYPE * pHalData;
  
  pHalData = GET_HAL_DATA (Adapter);
  
  if (IS_D_CUT (pHalData->VersionID) ) {
    u1temp =  rtw_read8 (Adapter, REG_XCK_OUT_CTRL);
    u1temp |= 0x18;
    rtw_write8 (Adapter, REG_XCK_OUT_CTRL, u1temp);
    MSG_8192C ("D cut version\n");
  }
  
  EnableGpio5ClockReq (Adapter, _FALSE, 1);
  
  u1temp = rtw_read8 (Adapter, REG_APE_PLL_CTRL_EXT);
  u1temp = (u1temp & 0xF0) | 0x05;
  rtw_write8 (Adapter, REG_APE_PLL_CTRL_EXT, u1temp);
}
#endif


static void rtl8188es_interface_configure (PADAPTER padapter)
{
  HAL_DATA_TYPE  * pHalData = GET_HAL_DATA (padapter);
  struct dvobj_priv  * pdvobjpriv = adapter_to_dvobj (padapter);
  struct registry_priv * pregistrypriv = &padapter->registrypriv;
  BOOLEAN   bWiFiConfig = pregistrypriv->wifi_spec;
  
  
  pdvobjpriv->RtOutPipe[0] = WLAN_TX_HIQ_DEVICE_ID;
  pdvobjpriv->RtOutPipe[1] = WLAN_TX_MIQ_DEVICE_ID;
  pdvobjpriv->RtOutPipe[2] = WLAN_TX_LOQ_DEVICE_ID;
  
  if (bWiFiConfig)
  { pHalData->OutEpNumber = 2; }
  else
  { pHalData->OutEpNumber = SDIO_MAX_TX_QUEUE; }
  
  switch (pHalData->OutEpNumber) {
  case 3:
    pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_LQ | TX_SELE_NQ;
    break;
  case 2:
    pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_NQ;
    break;
  case 1:
    pHalData->OutEpQueueSel = TX_SELE_HQ;
    break;
  default:
    break;
  }
  
  Hal_MappingOutPipe (padapter, pHalData->OutEpNumber);
}

/*
 * Description:
 *  Call power on sequence to enable card
 *
 * Return:
 *  _SUCCESS  enable success
 *  _FAIL   enable fail
 */
static u8 _CardEnable (PADAPTER padapter)
{
  u8 bMacPwrCtrlOn;
  u8 ret;
  
  DBG_871X ("=>%s\n", __FUNCTION__);
  
  rtw_hal_get_hwreg (padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
  if (bMacPwrCtrlOn == _FALSE)
  {
    #ifdef CONFIG_PLATFORM_SPRD
    u8 val8;
    #endif
    
    rtw_write8 (padapter, REG_RSV_CTRL, 0x0);
    
    #ifdef CONFIG_PLATFORM_SPRD
    #ifdef CONFIG_EXT_CLK
    _InitClockTo26MHz (padapter);
    #endif
    
    val8 =  rtw_read8 (padapter, 0x4);
    val8 = val8 & ~BIT (5);
    rtw_write8 (padapter, 0x4, val8);
    #endif
    
    ret = HalPwrSeqCmdParsing (padapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK, Rtl8188E_NIC_ENABLE_FLOW);
    if (ret == _SUCCESS) {
      u8 bMacPwrCtrlOn = _TRUE;
      rtw_hal_set_hwreg (padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
    }
    else
    {
      DBG_871X (KERN_ERR "%s: run power on flow fail\n", __func__);
      return _FAIL;
    }
    
  }
  else
  {
    ret = _SUCCESS;
  }
  
  DBG_871X ("<=%s\n", __FUNCTION__);
  
  return ret;
  
}

static u32 rtl8188es_InitPowerOn (PADAPTER padapter)
{
  u8 value8;
  u16 value16;
  u32 value32;
  u8 ret;
  
  DBG_871X ("=>%s\n", __FUNCTION__);
  
  ret = _CardEnable (padapter);
  if (ret == _FAIL) {
    return ret;
  }
  
  /*
    value8 = rtw_read8(padapter, REG_GPIO_INTM+1);
    value8 |= BIT(1);
    rtw_write8(padapter, REG_GPIO_INTM+1, value8);
    value8 = rtw_read8(padapter, REG_GPIO_IO_SEL_2+1);
    value8 |= BIT(1);
    rtw_write8(padapter, REG_GPIO_IO_SEL_2+1, value8);
  */
  
  value16 = rtw_read16 (padapter, REG_APS_FSMCO);
  value16 |= EnPDN;
  rtw_write16 (padapter, REG_APS_FSMCO, value16);
  
  
  value16 = rtw_read16 (padapter, REG_CR);
  value16 |= (HCI_TXDMA_EN | HCI_RXDMA_EN | TXDMA_EN | RXDMA_EN
              | PROTOCOL_EN | SCHEDULE_EN | ENSEC | CALTMR_EN);
  
  rtw_write16 (padapter, REG_CR, value16);
  
  
  

  DBG_871X ("<=%s\n", __FUNCTION__);
  
  return _SUCCESS;
  
}

static void _InitQueueReservedPage (PADAPTER padapter)
{
  #ifdef RTL8188ES_MAC_LOOPBACK


  rtw_write16 (padapter, REG_RQPN_NPQ, 0x0b0b);
  rtw_write32 (padapter, REG_RQPN, 0x80260b0b);
  
  #else
  HAL_DATA_TYPE  * pHalData = GET_HAL_DATA (padapter);
  struct registry_priv * pregistrypriv = &padapter->registrypriv;
  u32     outEPNum  = (u32) pHalData->OutEpNumber;
  u32     numHQ   = 0;
  u32     numLQ   = 0;
  u32     numNQ   = 0;
  u32     numPubQ;
  u32     value32;
  u8      value8;
  BOOLEAN     bWiFiConfig = pregistrypriv->wifi_spec;
  
  if (bWiFiConfig)
  {
    if (pHalData->OutEpQueueSel & TX_SELE_HQ)
    {
      numHQ =  0x29;
    }
  
    if (pHalData->OutEpQueueSel & TX_SELE_LQ)
    {
      numLQ = 0x1C;
    }
  
    if (pHalData->OutEpQueueSel & TX_SELE_NQ) {
      numNQ = 0x1C;
    }
    value8 = (u8) _NPQ (numNQ);
    rtw_write8 (padapter, REG_RQPN_NPQ, value8);
  
    numPubQ = 0xA9 - numHQ - numLQ - numNQ;
  
    value32 = _HPQ (numHQ) | _LPQ (numLQ) | _PUBQ (numPubQ) | LD_RQPN;
    rtw_write32 (padapter, REG_RQPN, value32);
  }
  else
  {
    rtw_write16 (padapter, REG_RQPN_NPQ, 0x0000);
    rtw_write32 (padapter, REG_RQPN, 0x80a00900);
  }
  #endif
  return;
}

static void _InitTxBufferBoundary (PADAPTER padapter, u8 txpktbuf_bndy)
{
  struct registry_priv * pregistrypriv = &padapter->registrypriv;
  
  
  rtw_write8 (padapter, REG_TXPKTBUF_BCNQ_BDNY, txpktbuf_bndy);
  rtw_write8 (padapter, REG_TXPKTBUF_MGQ_BDNY, txpktbuf_bndy);
  rtw_write8 (padapter, REG_TXPKTBUF_WMAC_LBK_BF_HD, txpktbuf_bndy);
  rtw_write8 (padapter, REG_TRXFF_BNDY, txpktbuf_bndy);
  rtw_write8 (padapter, REG_TDECTRL + 1, txpktbuf_bndy);
  
}

static VOID
_InitNormalChipRegPriority (
  IN  PADAPTER  Adapter,
  IN  u16   beQ,
  IN  u16   bkQ,
  IN  u16   viQ,
  IN  u16   voQ,
  IN  u16   mgtQ,
  IN  u16   hiQ
)
{
  u16 value16 = (rtw_read16 (Adapter, REG_TRXDMA_CTRL) & 0x7);
  
  value16 |=  _TXDMA_BEQ_MAP (beQ)   | _TXDMA_BKQ_MAP (bkQ) |
              _TXDMA_VIQ_MAP (viQ)   | _TXDMA_VOQ_MAP (voQ) |
              _TXDMA_MGQ_MAP (mgtQ) | _TXDMA_HIQ_MAP (hiQ);
              
  rtw_write16 (Adapter, REG_TRXDMA_CTRL, value16);
}

static VOID
_InitNormalChipOneOutEpPriority (
  IN  PADAPTER Adapter
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
  u16 value = 0;
  switch (pHalData->OutEpQueueSel)
  {
  case TX_SELE_HQ:
    value = QUEUE_HIGH;
    break;
  case TX_SELE_LQ:
    value = QUEUE_LOW;
    break;
  case TX_SELE_NQ:
    value = QUEUE_NORMAL;
    break;
  default:
    break;
  }
  
  _InitNormalChipRegPriority (Adapter,
                              value,
                              value,
                              value,
                              value,
                              value,
                              value
                             );
                             
}

static VOID
_InitNormalChipTwoOutEpPriority (
  IN  PADAPTER Adapter
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  struct registry_priv * pregistrypriv = &Adapter->registrypriv;
  u16     beQ, bkQ, viQ, voQ, mgtQ, hiQ;
  
  
  u16 valueHi = 0;
  u16 valueLow = 0;
  
  switch (pHalData->OutEpQueueSel)
  {
  case (TX_SELE_HQ | TX_SELE_LQ) :
    valueHi = QUEUE_HIGH;
    valueLow = QUEUE_LOW;
    break;
  case (TX_SELE_NQ | TX_SELE_LQ) :
    valueHi = QUEUE_NORMAL;
    valueLow = QUEUE_LOW;
    break;
  case (TX_SELE_HQ | TX_SELE_NQ) :
    valueHi = QUEUE_HIGH;
    valueLow = QUEUE_NORMAL;
    break;
  default:
    break;
  }
  
  if (!pregistrypriv->wifi_spec ) {
    beQ   = valueLow;
    bkQ   = valueLow;
    viQ   = valueHi;
    voQ   = valueHi;
    mgtQ  = valueHi;
    hiQ   = valueHi;
  }
  else {
    beQ   = valueLow;
    bkQ   = valueHi;
    viQ   = valueHi;
    voQ   = valueLow;
    mgtQ  = valueHi;
    hiQ   = valueHi;
  }
  
  _InitNormalChipRegPriority (Adapter, beQ, bkQ, viQ, voQ, mgtQ, hiQ);
  
}

static VOID
_InitNormalChipThreeOutEpPriority (
  IN  PADAPTER padapter
)
{
  struct registry_priv * pregistrypriv = &padapter->registrypriv;
  u16     beQ, bkQ, viQ, voQ, mgtQ, hiQ;
  
  if (!pregistrypriv->wifi_spec) {
    beQ   = QUEUE_LOW;
    bkQ     = QUEUE_LOW;
    viQ     = QUEUE_NORMAL;
    voQ     = QUEUE_HIGH;
    mgtQ  = QUEUE_HIGH;
    hiQ     = QUEUE_HIGH;
  }
  else {// for WMM
    beQ   = QUEUE_LOW;
    bkQ     = QUEUE_NORMAL;
    viQ     = QUEUE_NORMAL;
    voQ     = QUEUE_HIGH;
    mgtQ  = QUEUE_HIGH;
    hiQ     = QUEUE_HIGH;
  }
  _InitNormalChipRegPriority (padapter, beQ, bkQ, viQ, voQ, mgtQ, hiQ);
}

static VOID
_InitNormalChipQueuePriority (
  IN  PADAPTER Adapter
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
  switch (pHalData->OutEpNumber)
  {
  case 1:
    _InitNormalChipOneOutEpPriority (Adapter);
    break;
  case 2:
    _InitNormalChipTwoOutEpPriority (Adapter);
    break;
  case 3:
    _InitNormalChipThreeOutEpPriority (Adapter);
    break;
  default:
    break;
  }
  
  
}


static void _InitQueuePriority (PADAPTER padapter)
{
  _InitNormalChipQueuePriority (padapter);
}

static void _InitPageBoundary (PADAPTER padapter)
{
  u16 rxff_bndy = MAX_RX_DMA_BUFFER_SIZE_88E-1;
  
  rtw_write16 (padapter, (REG_TRXFF_BNDY + 2), rxff_bndy);
  
}

static void _InitTransferPageSize (PADAPTER padapter)
{
  
  u8 value8;
  value8 = _PSRX (PBP_128) | _PSTX (PBP_128);
  rtw_write8 (padapter, REG_PBP, value8);
}

void _InitDriverInfoSize (PADAPTER padapter, u8 drvInfoSize)
{
  rtw_write8 (padapter, REG_RX_DRVINFO_SZ, drvInfoSize);
}

void _InitNetworkType (PADAPTER padapter)
{
  u32 value32;
  
  value32 = rtw_read32 (padapter, REG_CR);
  
  value32 = (value32 & ~MASK_NETTYPE) | _NETTYPE (NT_LINK_AP);
  
  rtw_write32 (padapter, REG_CR, value32);
}

void _InitWMACSetting (PADAPTER padapter)
{
  u16 value16;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  
  
  pHalData->ReceiveConfig = RCR_APM | RCR_AM | RCR_AB | RCR_CBSSID_DATA | RCR_CBSSID_BCN | RCR_AMF | RCR_HTC_LOC_CTRL | RCR_APP_PHYST_RXFF | RCR_APP_ICV | RCR_APP_MIC;
  
  rtw_write32 (padapter, REG_RCR, pHalData->ReceiveConfig);
  
  value16 = 0xFFFF;
  rtw_write16 (padapter, REG_RXFLTMAP2, value16);
  
  value16 = 0x400;
  rtw_write16 (padapter, REG_RXFLTMAP1, value16);
  
  value16 = 0xFFFF;
  rtw_write16 (padapter, REG_RXFLTMAP0, value16);
  
}

void _InitAdaptiveCtrl (PADAPTER padapter)
{
  u16 value16;
  u32 value32;
  
  value32 = rtw_read32 (padapter, REG_RRSR);
  value32 &= ~RATE_BITMAP_ALL;
  value32 |= RATE_RRSR_CCK_ONLY_1M;
  rtw_write32 (padapter, REG_RRSR, value32);
  
  
  value16 = _SPEC_SIFS_CCK (0x10) | _SPEC_SIFS_OFDM (0x10);
  rtw_write16 (padapter, REG_SPEC_SIFS, value16);
  
  value16 = _LRL (0x30) | _SRL (0x30);
  rtw_write16 (padapter, REG_RL, value16);
}

void _InitEDCA (PADAPTER padapter)
{
  rtw_write16 (padapter, REG_SPEC_SIFS, 0x100a);
  rtw_write16 (padapter, REG_MAC_SPEC_SIFS, 0x100a);
  
  rtw_write16 (padapter, REG_SIFS_CTX, 0x100a);
  
  rtw_write16 (padapter, REG_SIFS_TRX, 0x100a);
  
  rtw_write32 (padapter, REG_EDCA_BE_PARAM, 0x005EA42B);
  rtw_write32 (padapter, REG_EDCA_BK_PARAM, 0x0000A44F);
  rtw_write32 (padapter, REG_EDCA_VI_PARAM, 0x005EA324);
  rtw_write32 (padapter, REG_EDCA_VO_PARAM, 0x002FA226);
}

void _InitRateFallback (PADAPTER padapter)
{
  rtw_write32 (padapter, REG_DARFRC, 0x00000000);
  rtw_write32 (padapter, REG_DARFRC + 4, 0x10080404);
  rtw_write32 (padapter, REG_RARFRC, 0x04030201);
  rtw_write32 (padapter, REG_RARFRC + 4, 0x08070605);
  
}

void _InitRetryFunction (PADAPTER padapter)
{
  u8  value8;
  
  value8 = rtw_read8 (padapter, REG_FWHW_TXQ_CTRL);
  value8 |= EN_AMPDU_RTY_NEW;
  rtw_write8 (padapter, REG_FWHW_TXQ_CTRL, value8);
  
  rtw_write8 (padapter, REG_ACKTO, 0x40);
}

static void HalRxAggr8188ESdio (PADAPTER padapter)
{
  #if 1
  struct registry_priv * pregistrypriv;
  u8  valueDMATimeout;
  u8  valueDMAPageCount;
  
  
  pregistrypriv = &padapter->registrypriv;
  
  if (pregistrypriv->wifi_spec)
  {
    valueDMATimeout = 0x0f;
    valueDMAPageCount = 0x01;
  }
  else
  {
    valueDMATimeout = 0x06;
    valueDMAPageCount = 0x24;
  }
  
  rtw_write8 (padapter, REG_RXDMA_AGG_PG_TH + 1, valueDMATimeout);
  rtw_write8 (padapter, REG_RXDMA_AGG_PG_TH, valueDMAPageCount);
  #endif
}

void sdio_AggSettingRxUpdate (PADAPTER padapter)
{
  #if 1
  u8 valueDMA;
  
  
  
  valueDMA = rtw_read8 (padapter, REG_TRXDMA_CTRL);
  valueDMA |= RXDMA_AGG_EN;
  rtw_write8 (padapter, REG_TRXDMA_CTRL, valueDMA);
  
  #if 0
  switch (RX_PAGE_SIZE_REG_VALUE)
  {
  case PBP_64:
    pHalData->HwRxPageSize = 64;
    break;
  case PBP_128:
    pHalData->HwRxPageSize = 128;
    break;
  case PBP_256:
    pHalData->HwRxPageSize = 256;
    break;
  case PBP_512:
    pHalData->HwRxPageSize = 512;
    break;
  case PBP_1024:
    pHalData->HwRxPageSize = 1024;
    break;
  default:
    RT_TRACE (_module_hci_hal_init_c_, _drv_err_,
              ("%s: RX_PAGE_SIZE_REG_VALUE definition is incorrect!\n", __FUNCTION__) );
    break;
  }
  #endif
  #endif
}

void _initSdioAggregationSetting (PADAPTER padapter)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  
  
  HalRxAggr8188ESdio (padapter);
  sdio_AggSettingRxUpdate (padapter);
  
  pHalData->UsbRxHighSpeedMode = _FALSE;
}


void _InitOperationMode (PADAPTER padapter)
{
  PHAL_DATA_TYPE pHalData;
  struct mlme_ext_priv * pmlmeext;
  u8        regBwOpMode = 0;
  u32       regRATR = 0, regRRSR = 0;
  u8        MinSpaceCfg;
  
  
  pHalData = GET_HAL_DATA (padapter);
  pmlmeext = &padapter->mlmeextpriv;
  
  switch (pmlmeext->cur_wireless_mode)
  {
  case WIRELESS_MODE_B:
    regBwOpMode = BW_OPMODE_20MHZ;
    regRATR = RATE_ALL_CCK;
    regRRSR = RATE_ALL_CCK;
    break;
  case WIRELESS_MODE_A:
    #if 0
    regBwOpMode = BW_OPMODE_5G | BW_OPMODE_20MHZ;
    regRATR = RATE_ALL_OFDM_AG;
    regRRSR = RATE_ALL_OFDM_AG;
    #endif
    break;
  case WIRELESS_MODE_G:
    regBwOpMode = BW_OPMODE_20MHZ;
    regRATR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
    regRRSR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
    break;
  case WIRELESS_MODE_AUTO:
    #if 0
    if (padapter->bInHctTest)
    {
      regBwOpMode = BW_OPMODE_20MHZ;
      regRATR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
      regRRSR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
    }
    else
    #endif
    {
      regBwOpMode = BW_OPMODE_20MHZ;
      regRATR = RATE_ALL_CCK | RATE_ALL_OFDM_AG | RATE_ALL_OFDM_1SS | RATE_ALL_OFDM_2SS;
      regRRSR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
    }
    break;
  case WIRELESS_MODE_N_24G:
    regBwOpMode = BW_OPMODE_20MHZ;
    regRATR = RATE_ALL_CCK | RATE_ALL_OFDM_AG | RATE_ALL_OFDM_1SS | RATE_ALL_OFDM_2SS;
    regRRSR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
    break;
  case WIRELESS_MODE_N_5G:
    #if 0
    regBwOpMode = BW_OPMODE_5G;
    regRATR = RATE_ALL_OFDM_AG | RATE_ALL_OFDM_1SS | RATE_ALL_OFDM_2SS;
    regRRSR = RATE_ALL_OFDM_AG;
    #endif
    break;
    
  default:
    break;
  }
  
  rtw_write8 (padapter, REG_BWOPMODE, regBwOpMode);
  
  switch (pHalData->rf_type)
  {
  case RF_1T2R:
  case RF_1T1R:
    RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("Initializepadapter: RF_Type%s\n", (pHalData->rf_type == RF_1T1R ? "(1T1R)" : "(1T2R)") ) );
    MinSpaceCfg = (MAX_MSS_DENSITY_1T << 3);
    break;
  case RF_2T2R:
  case RF_2T2R_GREEN:
    RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("Initializepadapter:RF_Type(2T2R)\n") );
    MinSpaceCfg = (MAX_MSS_DENSITY_2T << 3);
    break;
  }
  
  rtw_write8 (padapter, REG_AMPDU_MIN_SPACE, MinSpaceCfg);
}


void _InitBeaconParameters (PADAPTER padapter)
{
  PHAL_DATA_TYPE pHalData;
  
  
  pHalData = GET_HAL_DATA (padapter);
  
  rtw_write16 (padapter, REG_BCN_CTRL, 0x1010);
  
  rtw_write16 (padapter, REG_TBTT_PROHIBIT, 0x6404);
  rtw_write8 (padapter, REG_DRVERLYINT, DRIVER_EARLY_INT_TIME);
  rtw_write8 (padapter, REG_BCNDMATIM, BCN_DMA_ATIME_INT_TIME);
  
  rtw_write16 (padapter, REG_BCNTCFG, 0x660F);
  
  
  pHalData->RegBcnCtrlVal = rtw_read8 (padapter, REG_BCN_CTRL);
  pHalData->RegTxPause = rtw_read8 (padapter, REG_TXPAUSE);
  pHalData->RegFwHwTxQCtrl = rtw_read8 (padapter, REG_FWHW_TXQ_CTRL + 2);
  pHalData->RegReg542 = rtw_read8 (padapter, REG_TBTT_PROHIBIT + 2);
  pHalData->RegCR_1 = rtw_read8 (padapter, REG_CR + 1);
  
}

void _InitBeaconMaxError (PADAPTER padapter, BOOLEAN InfraMode)
{
  #ifdef RTL8192CU_ADHOC_WORKAROUND_SETTING
  rtw_write8 (padapter, REG_BCN_MAX_ERR, 0xFF);
  #endif
}

void _InitInterrupt (PADAPTER padapter)
{

  rtw_write32 (padapter, REG_HISR_88E, 0xFFFFFFFF);
  
  rtw_write32 (padapter, REG_HIMR_88E, 0);
  
  InitInterrupt8188ESdio (padapter);
  
  
  
  
}

void _InitRDGSetting (PADAPTER padapter)
{
  rtw_write8 (padapter, REG_RD_CTRL, 0xFF);
  rtw_write16 (padapter, REG_RD_NAV_NXT, 0x200);
  rtw_write8 (padapter, REG_RD_RESP_PKT_TH, 0x05);
}


static void _InitRxSetting (PADAPTER padapter)
{
  rtw_write32 (padapter, REG_MACID, 0x87654321);
  rtw_write32 (padapter, 0x0700, 0x87654321);
}


static void _InitRFType (PADAPTER padapter)
{
  struct registry_priv * pregpriv = &padapter->registrypriv;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  BOOLEAN  is2T2R = IS_2T2R (pHalData->VersionID);
  
  #if DISABLE_BB_RF
  pHalData->rf_chip = RF_PSEUDO_11N;
  return;
  #endif
  
  pHalData->rf_chip = RF_6052;
  
  if (_FALSE == is2T2R) {
    pHalData->rf_type = RF_1T1R;
    DBG_8192C ("Set RF Chip ID to RF_6052 and RF type to 1T1R.\n");
    return;
  }
  
  MSG_8192C ("Set RF Chip ID to RF_6052 and RF type to %d.\n", pHalData->rf_type);
}

static void _BBTurnOnBlock (PADAPTER padapter)
{
  #if (DISABLE_BB_RF)
  return;
  #endif
  
  PHY_SetBBReg (padapter, rFPGA0_RFMOD, bCCKEn, 0x1);
  PHY_SetBBReg (padapter, rFPGA0_RFMOD, bOFDMEn, 0x1);
}

#if 0
static void _InitAntenna_Selection (PADAPTER padapter)
{
  rtw_write8 (padapter, REG_LEDCFG2, 0x82);
}
#endif

static void _InitPABias (PADAPTER padapter)
{
  HAL_DATA_TYPE  * pHalData = GET_HAL_DATA (padapter);
  u8      pa_setting;
  BOOLEAN   is92C = IS_92C_SERIAL (pHalData->VersionID);
  
  pa_setting = EFUSE_Read1Byte (padapter, 0x1FA);
  
  
  if (! (pa_setting & BIT0) )
  {
    PHY_SetRFReg (padapter, RF_PATH_A, 0x15, 0x0FFFFF, 0x0F406);
    PHY_SetRFReg (padapter, RF_PATH_A, 0x15, 0x0FFFFF, 0x4F406);
    PHY_SetRFReg (padapter, RF_PATH_A, 0x15, 0x0FFFFF, 0x8F406);
    PHY_SetRFReg (padapter, RF_PATH_A, 0x15, 0x0FFFFF, 0xCF406);
  }
  
  if (! (pa_setting & BIT1) && is92C)
  {
    PHY_SetRFReg (padapter, RF_PATH_B, 0x15, 0x0FFFFF, 0x0F406);
    PHY_SetRFReg (padapter, RF_PATH_B, 0x15, 0x0FFFFF, 0x4F406);
    PHY_SetRFReg (padapter, RF_PATH_B, 0x15, 0x0FFFFF, 0x8F406);
    PHY_SetRFReg (padapter, RF_PATH_B, 0x15, 0x0FFFFF, 0xCF406);
  }
  
  if (! (pa_setting & BIT4) )
  {
    pa_setting = rtw_read8 (padapter, 0x16);
    pa_setting &= 0x0F;
    rtw_write8 (padapter, 0x16, pa_setting | 0x80);
    rtw_write8 (padapter, 0x16, pa_setting | 0x90);
  }
}

#if 0
VOID
_InitRDGSetting_8188E (
  IN  PADAPTER Adapter
)
{
  PlatformEFIOWrite1Byte (Adapter, REG_RD_CTRL, 0xFF);
  PlatformEFIOWrite2Byte (Adapter, REG_RD_NAV_NXT, 0x200);
  PlatformEFIOWrite1Byte (Adapter, REG_RD_RESP_PKT_TH, 0x05);
}
#endif

static u32 rtl8188es_hal_init (PADAPTER padapter)
{
  s32 ret;
  u8  txpktbuf_bndy;
  HAL_DATA_TYPE  * pHalData = GET_HAL_DATA (padapter);
  struct pwrctrl_priv  * pwrctrlpriv = &padapter->pwrctrlpriv;
  struct registry_priv * pregistrypriv = &padapter->registrypriv;
  u8 is92C = IS_92C_SERIAL (pHalData->VersionID);
  rt_rf_power_state eRfPowerStateToSet;
  u8 value8;
  u16 value16;
  
  u32 init_start_time = rtw_get_current_time();
  
  #ifdef DBG_HAL_INIT_PROFILING
  enum HAL_INIT_STAGES {
    HAL_INIT_STAGES_BEGIN = 0,
    HAL_INIT_STAGES_INIT_PW_ON,
    HAL_INIT_STAGES_MISC01,
    HAL_INIT_STAGES_DOWNLOAD_FW,
    HAL_INIT_STAGES_MAC,
    HAL_INIT_STAGES_BB,
    HAL_INIT_STAGES_RF,
    HAL_INIT_STAGES_EFUSE_PATCH,
    HAL_INIT_STAGES_INIT_LLTT,
    
    HAL_INIT_STAGES_MISC02,
    HAL_INIT_STAGES_TURN_ON_BLOCK,
    HAL_INIT_STAGES_INIT_SECURITY,
    HAL_INIT_STAGES_MISC11,
    HAL_INIT_STAGES_INIT_HAL_DM,
    HAL_INIT_STAGES_IQK,
    HAL_INIT_STAGES_PW_TRACK,
    HAL_INIT_STAGES_LCK,
    HAL_INIT_STAGES_INIT_PABIAS,
    HAL_INIT_STAGES_MISC31,
    HAL_INIT_STAGES_END,
    HAL_INIT_STAGES_NUM
  };
  
  char * hal_init_stages_str[] = {
    "HAL_INIT_STAGES_BEGIN",
    "HAL_INIT_STAGES_INIT_PW_ON",
    "HAL_INIT_STAGES_MISC01",
    "HAL_INIT_STAGES_DOWNLOAD_FW",
    "HAL_INIT_STAGES_MAC",
    "HAL_INIT_STAGES_BB",
    "HAL_INIT_STAGES_RF",
    "HAL_INIT_STAGES_EFUSE_PATCH",
    "HAL_INIT_STAGES_INIT_LLTT",
    "HAL_INIT_STAGES_MISC02",
    "HAL_INIT_STAGES_TURN_ON_BLOCK",
    "HAL_INIT_STAGES_INIT_SECURITY",
    "HAL_INIT_STAGES_MISC11",
    "HAL_INIT_STAGES_INIT_HAL_DM",
    "HAL_INIT_STAGES_IQK",
    "HAL_INIT_STAGES_PW_TRACK",
    "HAL_INIT_STAGES_LCK",
    "HAL_INIT_STAGES_INIT_PABIAS"
    "HAL_INIT_STAGES_MISC31",
    "HAL_INIT_STAGES_END",
  };
  
  
  int hal_init_profiling_i;
  u32 hal_init_stages_timestamp[HAL_INIT_STAGES_NUM];
  
  for (hal_init_profiling_i = 0; hal_init_profiling_i < HAL_INIT_STAGES_NUM; hal_init_profiling_i++)
  { hal_init_stages_timestamp[hal_init_profiling_i] = 0; }
  
#define HAL_INIT_PROFILE_TAG(stage) hal_init_stages_timestamp[(stage)]=rtw_get_current_time();
  #else
#define HAL_INIT_PROFILE_TAG(stage) do {} while(0)
  #endif
  
  DBG_8192C ("+rtl8188es_hal_init\n");
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_BEGIN);

  #ifdef CONFIG_WOWLAN
  if (rtw_read8 (padapter, REG_MCUFWDL) &BIT7 &&
      (padapter->pwrctrlpriv.wowlan_wake_reason & FWDecisionDisconnect) ) {
    u8 reg_val = 0;
    DBG_8192C ("+Reset Entry+\n");
    rtw_write8 (padapter, REG_MCUFWDL, 0x00);
    _8051Reset88E (padapter);
    reg_val = rtw_read8 (padapter, REG_SYS_FUNC_EN);
    reg_val &= ~ (BIT (0) | BIT (1) );
    rtw_write8 (padapter, REG_SYS_FUNC_EN, reg_val);
    rtw_write8 (padapter, REG_RF_CTRL, 0);
    rtw_write16 (padapter, REG_CR, 0);
    reg_val = rtw_read8 (padapter, REG_SYS_FUNC_EN + 1);
    reg_val &= ~ (BIT (4) | BIT (7) );
    rtw_write8 (padapter, REG_SYS_FUNC_EN + 1, reg_val);
    reg_val = rtw_read8 (padapter, REG_SYS_FUNC_EN + 1);
    reg_val |= BIT (4) | BIT (7);
    rtw_write8 (padapter, REG_SYS_FUNC_EN + 1, reg_val);
    DBG_8192C ("-Reset Entry-\n");
  }
  #endif
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_INIT_PW_ON);
  ret = rtl8188es_InitPowerOn (padapter);
  if (_FAIL == ret) {
    RT_TRACE (_module_hci_hal_init_c_, _drv_err_, ("Failed to init Power On!\n") );
    goto exit;
  }
  
  ret = PowerOnCheck (padapter);
  if (_FAIL == ret ) {
    DBG_871X ("Power on Fail! do it again\n");
    ret = rtl8188es_InitPowerOn (padapter);
    if (_FAIL == ret) {
      DBG_871X ("Failed to init Power On!\n");
      goto exit;
    }
  }
  DBG_871X ("Power on ok!\n");
  
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_MISC01);
  if (!pregistrypriv->wifi_spec) {
    txpktbuf_bndy = TX_PAGE_BOUNDARY_88E;
  }
  else {
    txpktbuf_bndy = WMM_NORMAL_TX_PAGE_BOUNDARY_88E;
  }
  _InitQueueReservedPage (padapter);
  _InitQueuePriority (padapter);
  _InitPageBoundary (padapter);
  _InitTransferPageSize (padapter);
  #ifdef CONFIG_IOL_IOREG_CFG
  _InitTxBufferBoundary (padapter, 0);
  #endif
  value8 = SdioLocalCmd52Read1Byte (padapter, SDIO_REG_TX_CTRL);
  SdioLocalCmd52Write1Byte (padapter, SDIO_REG_TX_CTRL, 0x02);
  
  rtw_write8 (padapter, SDIO_LOCAL_BASE | SDIO_REG_HRPWM1, 0);
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_DOWNLOAD_FW);
  #if (MP_DRIVER == 1)
  if (padapter->registrypriv.mp_mode == 1)
  {
    _InitRxSetting (padapter);
  }
  #endif
  {
    #if 0
    padapter->bFWReady = _FALSE;
    pHalData->fw_ractrl = _FALSE;
    #else
    #ifdef CONFIG_WOWLAN
    ret = rtl8188e_FirmwareDownload (padapter, _FALSE);
    #else
    ret = rtl8188e_FirmwareDownload (padapter);
    #endif
    
    if (ret != _SUCCESS) {
      DBG_871X ("%s: Download Firmware failed!!\n", __FUNCTION__);
      padapter->bFWReady = _FALSE;
      pHalData->fw_ractrl = _FALSE;
      goto exit;
    }
    else {
      RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("Initializepadapter8192CSdio(): Download Firmware Success!!\n") );
      padapter->bFWReady = _TRUE;
      pHalData->fw_ractrl = _FALSE;
    }
    #endif
  }
  
  rtl8188e_InitializeFirmwareVars (padapter);
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_MAC);
  #if (HAL_MAC_ENABLE == 1)
  ret = PHY_MACConfig8188E (padapter);
  if (ret != _SUCCESS) {
    goto exit;
  }
  #endif
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_BB);
  #if (HAL_BB_ENABLE == 1)
  ret = PHY_BBConfig8188E (padapter);
  if (ret != _SUCCESS) {
    goto exit;
  }
  #endif
  
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_RF);
  
  #if (HAL_RF_ENABLE == 1)
  ret = PHY_RFConfig8188E (padapter);
  
  if (ret != _SUCCESS) {
    goto exit;
  }
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_EFUSE_PATCH);
  #if defined(CONFIG_IOL_EFUSE_PATCH)
  ret = rtl8188e_iol_efuse_patch (padapter);
  if (ret != _SUCCESS) {
    DBG_871X ("%s  rtl8188e_iol_efuse_patch failed \n", __FUNCTION__);
    goto exit;
  }
  #endif
  _InitTxBufferBoundary (padapter, txpktbuf_bndy);
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_INIT_LLTT);
  ret = InitLLTTable (padapter, txpktbuf_bndy);
  if (_SUCCESS != ret) {
    RT_TRACE (_module_hci_hal_init_c_, _drv_err_, ("Failed to init LLT Table!\n") );
    goto exit;
  }
  
  #if (RATE_ADAPTIVE_SUPPORT==1)
  {
    value8 = rtw_read8 (padapter, REG_TX_RPT_CTRL);
    rtw_write8 (padapter,  REG_TX_RPT_CTRL, (value8 | BIT1 | BIT0) );
    rtw_write8 (padapter,  REG_TX_RPT_CTRL + 1, 2);
    rtw_write16 (padapter, REG_TX_RPT_TIME, 0xCdf0);
  }
  #endif
  
  #if 0
  if (pHTInfo->bRDGEnable) {
    _InitRDGSetting_8188E (Adapter);
  }
  #endif
  
  #ifdef CONFIG_TX_EARLY_MODE
  if ( pHalData->bEarlyModeEnable)
  {
    RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("EarlyMode Enabled!!!\n") );
    
    value8 = rtw_read8 (padapter, REG_EARLY_MODE_CONTROL);
    #if RTL8188E_EARLY_MODE_PKT_NUM_10 == 1
    value8 = value8 | 0x1f;
    #else
    value8 = value8 | 0xf;
    #endif
    rtw_write8 (padapter, REG_EARLY_MODE_CONTROL, value8);
    
    rtw_write8 (padapter, REG_EARLY_MODE_CONTROL + 3, 0x80);
    
    value8 = rtw_read8 (padapter, REG_TCR + 1);
    value8 = value8 | 0x40;
    rtw_write8 (padapter, REG_TCR + 1, value8);
  }
  else
  #endif
  {
    rtw_write8 (padapter, REG_EARLY_MODE_CONTROL, 0);
  }
  
  
  #if(SIC_ENABLE == 1)
  SIC_Init (padapter);
  #endif
  
  
  if (pwrctrlpriv->reg_rfoff == _TRUE) {
    pwrctrlpriv->rf_pwrstate = rf_off;
  }
  
  HalDetectPwrDownMode88E (padapter);
  
  
  _InitRFType (padapter);
  
  pHalData->CurrentChannel = 1;
  
  
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_MISC02);
  _InitDriverInfoSize (padapter, 4);
  hal_init_macaddr (padapter);
  _InitNetworkType (padapter);
  _InitWMACSetting (padapter);
  _InitAdaptiveCtrl (padapter);
  _InitEDCA (padapter);
  _InitRateFallback (padapter);
  _InitRetryFunction (padapter);
  _initSdioAggregationSetting (padapter);
  _InitOperationMode (padapter);
  _InitBeaconParameters (padapter);
  _InitBeaconMaxError (padapter, _TRUE);
  _InitInterrupt (padapter);
  
  value16 = rtw_read16 (padapter, REG_CR);
  value16 |= (MACTXEN | MACRXEN);
  rtw_write8 (padapter, REG_CR, value16);
  
  #if defined(CONFIG_CONCURRENT_MODE) || defined(CONFIG_TX_MCAST2UNI)
  
  #ifdef CONFIG_CHECK_AC_LIFETIME
  rtw_write8 (padapter, REG_LIFETIME_EN, 0x0F);
  #endif 
  
  #ifdef CONFIG_TX_MCAST2UNI
  rtw_write16 (padapter, REG_PKT_VO_VI_LIFE_TIME, 0x0400);
  rtw_write16 (padapter, REG_PKT_BE_BK_LIFE_TIME, 0x0400);
  #else
  rtw_write16 (padapter, REG_PKT_VO_VI_LIFE_TIME, 0x3000);
  rtw_write16 (padapter, REG_PKT_BE_BK_LIFE_TIME, 0x3000);
  #endif 
  #endif 
  
  
  
  
  #endif
  
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_TURN_ON_BLOCK);
  _BBTurnOnBlock (padapter);
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_INIT_SECURITY);
  #if 1
  invalidate_cam_all (padapter);
  #else
  CamResetAllEntry (padapter);
  padapter->HalFunc.EnableHWSecCfgHandler (padapter);
  #endif
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_MISC11);
  PHY_SetTxPowerLevel8188E (padapter, pHalData->CurrentChannel);

  #if 0
  #if 0
  if (!IS_92C_SERIAL ( pHalData->VersionID) && (pHalData->AntDivCfg != 0) )
  #else
  if (IS_1T1R ( pHalData->VersionID) && (pHalData->AntDivCfg != 0) )
  #endif
  {
    _InitAntenna_Selection (padapter);
  }
  #endif
  
  rtw_write32 (padapter, REG_BAR_MODE_CTRL, 0x0201ffff);
  
  rtw_write8 (padapter, REG_HWSEQ_CTRL, 0xFF);
  
  
  #ifdef RTL8188ES_MAC_LOOPBACK
  value8 = rtw_read8 (padapter, REG_SYS_FUNC_EN);
  value8 &= ~ (FEN_BBRSTB | FEN_BB_GLB_RSTn);
  rtw_write8 (padapter, REG_SYS_FUNC_EN, value8);
  
  rtw_write8 (padapter, REG_RD_CTRL, 0x0F);
  rtw_write8 (padapter, REG_RD_CTRL + 1, 0xCF);
  rtw_write32 (padapter, REG_CR, 0x0b0202ff);
  #endif
  
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_INIT_HAL_DM);
  rtl8188e_InitHalDm (padapter);
  
  
  #if (MP_DRIVER == 1)
  if (padapter->registrypriv.mp_mode == 1)
  {
    padapter->mppriv.channel = pHalData->CurrentChannel;
    MPT_InitializeAdapter (padapter, padapter->mppriv.channel);
  }
  else
  #endif
  {
    pwrctrlpriv->rf_pwrstate = rf_on;
    RT_CLEAR_PS_LEVEL (pwrctrlpriv, RT_RF_OFF_LEVL_HALT_NIC);
    
    pwrctrlpriv->b_hw_radio_off = _FALSE;
    eRfPowerStateToSet = rf_on;
    
    if (pHalData->pwrdown && eRfPowerStateToSet == rf_off)
    {
      rtw_write8 (padapter, REG_RSV_CTRL, 0x0);
      
      if (IS_HARDWARE_TYPE_8723AS (padapter) )
      {
        value8 = rtw_read8 (padapter, REG_MULTI_FUNC_CTRL);
        rtw_write8 (padapter, REG_MULTI_FUNC_CTRL, (value8 | WL_HWPDN_EN) );
      }
      else
      {
        rtw_write16 (padapter, REG_APS_FSMCO, 0x8812);
      }
    }
    
    if (pwrctrlpriv->rf_pwrstate == rf_on)
    {
    
      HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_IQK);
      if (pHalData->odmpriv.RFCalibrateInfo.bIQKInitialized) {
        PHY_IQCalibrate_8188E (padapter, _TRUE);
      }
      else
      {
        PHY_IQCalibrate_8188E (padapter, _FALSE);
        pHalData->odmpriv.RFCalibrateInfo.bIQKInitialized = _TRUE;
      }
      

      HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_PW_TRACK);
      ODM_TXPowerTrackingCheck (&pHalData->odmpriv );
      
      HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_LCK);
      PHY_LCCalibrate_8188E (padapter);
      
      
    }
  }
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_INIT_PABIAS);
  {
    _InitPABias (padapter);
  }
  


  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_MISC31);
  #if 0
  if (pDevice->RegUsbSS)
  {
    RT_TRACE (COMP_INIT, DBG_LOUD, (" call GpioDetectTimerStart\n") );
    GpioDetectTimerStart (padapter);
  }
  #endif
  
  
  

  HalQueryTxBufferStatus8189ESdio (padapter);
  
  
  if (pregistrypriv->wifi_spec)
  { rtw_write16 (padapter, REG_FAST_EDCA_CTRL , 0); }
  
  
  
  if (IS_HARDWARE_TYPE_8188ES (padapter) )
  {
    value8 = rtw_read8 (padapter, 0x4d3);
    rtw_write8 (padapter, 0x4d3, (value8 | 0x1) );
  }
  
  
  
  rtw_write8 (padapter,  REG_FWHW_TXQ_CTRL + 1, 0x0F);
  /*
    PlatformEFIOWrite1Byte(Adapter, REG_EARLY_MODE_CONTROL+3, 0x01);
  
  */ 
  rtw_write16 (padapter, REG_TX_RPT_TIME, 0x3DF0);
  
  
  rtw_write16 (padapter, REG_TXDMA_OFFSET_CHK, (rtw_read16 (padapter, REG_TXDMA_OFFSET_CHK) | DROP_DATA_EN) );
  
  DBG_8192C ("DISABLE_BB_RF=%d\n", DISABLE_BB_RF);
  DBG_8192C ("IS_HARDWARE_TYPE_8188ES=%d\n", IS_HARDWARE_TYPE_8188ES (padapter) );

  #ifdef CONFIG_PLATFORM_SPRD
  rtw_write8 (padapter, GPIO_IO_SEL, 0xFF);
  
  value8 = rtw_read8 (padapter, REG_GPIO_IO_SEL);
  rtw_write8 (padapter, REG_GPIO_IO_SEL, (value8 << 4) | value8);
  value8 = rtw_read8 (padapter, REG_GPIO_IO_SEL + 1);
  rtw_write8 (padapter, REG_GPIO_IO_SEL + 1, value8 | 0x0F);
  #endif
  
  
  #ifdef CONFIG_XMIT_ACK
  rtw_write32 (padapter, REG_FWHW_TXQ_CTRL, rtw_read32 (padapter, REG_FWHW_TXQ_CTRL) | BIT (12) );
  #endif
  
  
  
  DBG_8192C ("-rtl8188es_hal_init\n");
  
exit:
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_END);
  
  DBG_871X ("%s in %dms\n", __FUNCTION__, rtw_get_passing_time_ms (init_start_time) );
  
  #ifdef DBG_HAL_INIT_PROFILING
  hal_init_stages_timestamp[HAL_INIT_STAGES_END] = rtw_get_current_time();
  
  for (hal_init_profiling_i = 0; hal_init_profiling_i < HAL_INIT_STAGES_NUM - 1; hal_init_profiling_i++) {
    DBG_871X ("DBG_HAL_INIT_PROFILING: %35s, %u, %5u, %5u\n"
              , hal_init_stages_str[hal_init_profiling_i]
              , hal_init_stages_timestamp[hal_init_profiling_i]
              , (hal_init_stages_timestamp[hal_init_profiling_i + 1] - hal_init_stages_timestamp[hal_init_profiling_i])
              , rtw_get_time_interval_ms (hal_init_stages_timestamp[hal_init_profiling_i], hal_init_stages_timestamp[hal_init_profiling_i + 1])
             );
  }
  #endif
  
  return ret;
  
}

static void CardDisableRTL8188ESdio (PADAPTER padapter)
{
  u8    u1bTmp;
  u16   u2bTmp;
  u32   u4bTmp;
  u8    bMacPwrCtrlOn;
  u8    ret;
  #ifdef CONFIG_PLATFORM_SPRD
  struct pwrctrl_priv * pwrpriv = &padapter->pwrctrlpriv;
  #endif
  
  DBG_871X ("=>%s\n", __FUNCTION__);
  
  
  u1bTmp = rtw_read8 (padapter, REG_TX_RPT_CTRL);
  rtw_write8 (padapter, REG_TX_RPT_CTRL, u1bTmp & (~BIT1) );
  
  rtw_write8 (padapter, REG_CR, 0x0);
  
  
  #ifdef CONFIG_EXT_CLK
  EnableGpio5ClockReq (padapter, _FALSE, 0);
  #endif
  
  #if 1
  u1bTmp = rtw_read8 (padapter, GPIO_IN);
  rtw_write8 (padapter, GPIO_OUT, u1bTmp);
  rtw_write8 (padapter, GPIO_IO_SEL, 0xFF);
  
  u1bTmp = rtw_read8 (padapter, REG_GPIO_IO_SEL);
  rtw_write8 (padapter, REG_GPIO_IO_SEL, (u1bTmp << 4) | u1bTmp);
  u1bTmp = rtw_read8 (padapter, REG_GPIO_IO_SEL + 1);
  rtw_write8 (padapter, REG_GPIO_IO_SEL + 1, u1bTmp | 0x0F);
  #endif
  
  
  ret = HalPwrSeqCmdParsing (padapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK, Rtl8188E_NIC_LPS_ENTER_FLOW);
  if (ret == _FALSE) {
    DBG_871X ("%s: run RF OFF flow fail!\n", __func__);
  }
  
  
  u1bTmp = rtw_read8 (padapter, REG_MCUFWDL);
  if ( (u1bTmp & RAM_DL_SEL) && padapter->bFWReady)
  {
    
    u1bTmp = rtw_read8 (padapter, REG_SYS_FUNC_EN + 1);
    u1bTmp &= ~BIT (2);
    rtw_write8 (padapter, REG_SYS_FUNC_EN + 1, u1bTmp);
  }
  
  
  rtw_write8 (padapter, REG_MCUFWDL, 0);
  
  
  
  bMacPwrCtrlOn = _FALSE;
  rtw_hal_set_hwreg (padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
  
  
  /*
    if((pMgntInfo->RfOffReason & RF_CHANGE_BY_HW) && pHalData->pwrdown)
    {// Power Down
  
      ret = HalPwrSeqCmdParsing(Adapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK, Rtl8188E_NIC_PDN_FLOW);
    }
    else
  */
  #ifdef CONFIG_PLATFORM_SPRD
  if (pwrpriv->bpower_saving == _TRUE)
  #endif
  {
  
    ret = HalPwrSeqCmdParsing (padapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK, Rtl8188E_NIC_DISABLE_FLOW);
    
    
    if (ret == _FALSE) {
      DBG_871X ("%s: run CARD DISABLE flow fail!\n", __func__);
    }
  }
  
  
  /*
    u1bTmp = rtw_read8(padapter, REG_RSV_CTRL+1);
    u1bTmp &= ~BIT(0);
    rtw_write8(padapter, REG_RSV_CTRL+1, u1bTmp);
    u1bTmp = rtw_read8(padapter, REG_RSV_CTRL+1);
    u1bTmp |= BIT(0);
    rtw_write8(padapter, REG_RSV_CTRL+1, u1bTmp);
  */
  
  
  rtw_write8 (padapter, REG_RSV_CTRL, 0x0E);
  
  padapter->bFWReady = _FALSE;
  DBG_871X ("<=%s\n", __FUNCTION__);
  
}

static u32 rtl8188es_hal_deinit (PADAPTER padapter)
{
  DBG_871X ("=>%s\n", __FUNCTION__);
  
  if (padapter->hw_init_completed == _TRUE)
  { CardDisableRTL8188ESdio (padapter); }
  
  DBG_871X ("<=%s\n", __FUNCTION__);
  
  return _SUCCESS;
}

static u32 rtl8188es_inirp_init (PADAPTER padapter)
{
  u32 status;
  
  _func_enter_;
  
  status = _SUCCESS;
  
  _func_exit_;
  
  return status;
}

static u32 rtl8188es_inirp_deinit (PADAPTER padapter)
{
  RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("+rtl8188es_inirp_deinit\n") );
  
  RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("-rtl8188es_inirp_deinit\n") );
  
  return _SUCCESS;
}

static void rtl8188es_init_default_value (PADAPTER padapter)
{
  PHAL_DATA_TYPE pHalData;
  struct pwrctrl_priv * pwrctrlpriv;
  struct dm_priv * pdmpriv;
  u8 i;
  
  pHalData = GET_HAL_DATA (padapter);
  pwrctrlpriv = &padapter->pwrctrlpriv;
  pdmpriv = &pHalData->dmpriv;
  
  
  pHalData->fw_ractrl = _FALSE;
  if (!pwrctrlpriv->bkeepfwalive)
  { pHalData->LastHMEBoxNum = 0; }
  
  pHalData->odmpriv.RFCalibrateInfo.bIQKInitialized = _FALSE;
  pHalData->odmpriv.RFCalibrateInfo.TM_Trigger = 0;//for IQK
  pHalData->pwrGroupCnt = 0;
  pHalData->PGMaxGroup = 13;
  pHalData->odmpriv.RFCalibrateInfo.ThermalValue_HP_index = 0;
  for (i = 0; i < HP_THERMAL_NUM; i++)
  { pHalData->odmpriv.RFCalibrateInfo.ThermalValue_HP[i] = 0; }
  
  pHalData->SdioRxFIFOCnt = 0;
}

static void _EfuseCellSel (
  IN  PADAPTER  padapter
)
{
  
  u32     value32;
  
  {
    value32 = rtw_read32 (padapter, EFUSE_TEST);
    value32 = (value32 & ~EFUSE_SEL_MASK) | EFUSE_SEL (EFUSE_WIFI_SEL_0);
    rtw_write32 (padapter, EFUSE_TEST, value32);
  }
}

static VOID
_ReadRFType (
  IN  PADAPTER  Adapter
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
  #if DISABLE_BB_RF
  pHalData->rf_chip = RF_PSEUDO_11N;
  #else
  pHalData->rf_chip = RF_6052;
  #endif
}

static void
Hal_EfuseParsePIDVID_8188ES (
  IN  PADAPTER    pAdapter,
  IN  u8   *  hwinfo,
  IN  BOOLEAN     AutoLoadFail
)
{

  
}

static void
Hal_EfuseParseMACAddr_8188ES (
  IN  PADAPTER    padapter,
  IN  u8   *  hwinfo,
  IN  BOOLEAN     AutoLoadFail
)
{
  u16     i, usValue;
  u8      sMacAddr[6] = {0x00, 0xE0, 0x4C, 0x81, 0x88, 0x77};
  EEPROM_EFUSE_PRIV * pEEPROM = GET_EEPROM_EFUSE_PRIV (padapter);
  
  if (AutoLoadFail)
  {
    for (i = 0; i < 6; i++)
    { pEEPROM->mac_addr[i] = sMacAddr[i]; }
  }
  else
  {
    _rtw_memcpy (pEEPROM->mac_addr, &hwinfo[EEPROM_MAC_ADDR_88ES], ETH_ALEN);
    
  }

  DBG_871X ("Hal_EfuseParseMACAddr_8188ES: Permanent Address = %02x-%02x-%02x-%02x-%02x-%02x\n",
            pEEPROM->mac_addr[0], pEEPROM->mac_addr[1],
            pEEPROM->mac_addr[2], pEEPROM->mac_addr[3],
            pEEPROM->mac_addr[4], pEEPROM->mac_addr[5]);
}


#ifdef CONFIG_EFUSE_CONFIG_FILE
static u32 Hal_readPGDataFromConfigFile (
  PADAPTER  padapter)
{
  u32 i;
  struct file * fp;
  mm_segment_t fs;
  u8 temp[3];
  loff_t pos = 0;
  EEPROM_EFUSE_PRIV * pEEPROM = GET_EEPROM_EFUSE_PRIV (padapter);
  u8 * PROMContent = pEEPROM->efuse_eeprom_data;
  
  
  temp[2] = 0;
  
  fp = filp_open ("/system/etc/wifi/wifi_efuse.map", O_RDWR,  0644);
  if (IS_ERR (fp) ) {
    pEEPROM->bloadfile_fail_flag = _TRUE;
    DBG_871X ("Error, Efuse configure file doesn't exist.\n");
    return _FAIL;
  }
  
  fs = get_fs();
  set_fs (KERNEL_DS);
  
  DBG_871X ("Efuse configure file:\n");
  for (i = 0; i < HWSET_MAX_SIZE_88E; i++) {
    vfs_read (fp, temp, 2, &pos);
    PROMContent[i] = simple_strtoul (temp, NULL, 16 );
    pos += 1;
    DBG_871X ("%02X \n", PROMContent[i]);
  }
  DBG_871X ("\n");
  set_fs (fs);
  
  filp_close (fp, NULL);
  pEEPROM->bloadfile_fail_flag = _FALSE;
  return _SUCCESS;
}

static void
Hal_ReadMACAddrFromFile_8188ES (
  PADAPTER    padapter
)
{
  u32 i;
  struct file * fp;
  mm_segment_t fs;
  u8 source_addr[18];
  loff_t pos = 0;
  u32 curtime = rtw_get_current_time();
  EEPROM_EFUSE_PRIV * pEEPROM = GET_EEPROM_EFUSE_PRIV (padapter);
  u8 * head, *end;
  
  u8 null_mac_addr[ETH_ALEN] = {0, 0, 0, 0, 0, 0};
  u8 multi_mac_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  
  _rtw_memset (source_addr, 0, 18);
  _rtw_memset (pEEPROM->mac_addr, 0, ETH_ALEN);
  
  fp = filp_open ("/data/wifimac.txt", O_RDWR,  0644);
  if (IS_ERR (fp) ) {
    pEEPROM->bloadmac_fail_flag = _TRUE;
    DBG_871X ("Error, wifi mac address file doesn't exist.\n");
  }
  else {
    fs = get_fs();
    set_fs (KERNEL_DS);
    
    DBG_871X ("wifi mac address:\n");
    vfs_read (fp, source_addr, 18, &pos);
    source_addr[17] = ':';
    
    head = end = source_addr;
    for (i = 0; i < ETH_ALEN; i++) {
      while (end && (*end != ':') )
      { end++; }
      
      if (end && (*end == ':') )
      { *end = '\0'; }
      
      pEEPROM->mac_addr[i] = simple_strtoul (head, NULL, 16 );
      
      if (end) {
        end++;
        head = end;
      }
      DBG_871X ("%02x \n", pEEPROM->mac_addr[i]);
    }
    DBG_871X ("\n");
    set_fs (fs);
    pEEPROM->bloadmac_fail_flag = _FALSE;
    filp_close (fp, NULL);
  }
  
  if ( (_rtw_memcmp (pEEPROM->mac_addr, null_mac_addr, ETH_ALEN) ) ||
       (_rtw_memcmp (pEEPROM->mac_addr, multi_mac_addr, ETH_ALEN) ) ) {
    pEEPROM->mac_addr[0] = 0x00;
    pEEPROM->mac_addr[1] = 0xe0;
    pEEPROM->mac_addr[2] = 0x4c;
    pEEPROM->mac_addr[3] = (u8) (curtime & 0xff) ;
    pEEPROM->mac_addr[4] = (u8) ( (curtime >> 8) & 0xff) ;
    pEEPROM->mac_addr[5] = (u8) ( (curtime >> 16) & 0xff) ;
  }
  
  DBG_871X ("Hal_ReadMACAddrFromFile_8188ES: Permanent Address = %02x-%02x-%02x-%02x-%02x-%02x\n",
            pEEPROM->mac_addr[0], pEEPROM->mac_addr[1],
            pEEPROM->mac_addr[2], pEEPROM->mac_addr[3],
            pEEPROM->mac_addr[4], pEEPROM->mac_addr[5]);
}
#endif

static VOID
readAdapterInfo_8188ES (
  IN PADAPTER     padapter
)
{
  EEPROM_EFUSE_PRIV * pEEPROM = GET_EEPROM_EFUSE_PRIV (padapter);
  
  /* parse the eeprom/efuse content */
  Hal_EfuseParseIDCode88E (padapter, pEEPROM->efuse_eeprom_data);
  Hal_EfuseParsePIDVID_8188ES (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  
  #ifdef CONFIG_EFUSE_CONFIG_FILE
  Hal_ReadMACAddrFromFile_8188ES (padapter);
  #else
  Hal_EfuseParseMACAddr_8188ES (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  #endif
  
  Hal_ReadPowerSavingMode88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_ReadTxPowerInfo88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_EfuseParseEEPROMVer88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  rtl8188e_EfuseParseChnlPlan (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_EfuseParseXtal_8188E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_EfuseParseCustomerID88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_EfuseParseBoardType88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_ReadThermalMeter_88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_InitChannelPlan (padapter);
  #ifdef CONFIG_WOWLAN
  Hal_DetectWoWMode (padapter);
  #endif 
  #ifdef CONFIG_RF_GAIN_OFFSET
  Hal_ReadRFGainOffset (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  #endif 
}

static void _ReadPROMContent (
  IN PADAPTER     padapter
)
{
  EEPROM_EFUSE_PRIV * pEEPROM = GET_EEPROM_EFUSE_PRIV (padapter);
  u8      eeValue;
  
  /* check system boot selection */
  eeValue = rtw_read8 (padapter, REG_9346CR);
  pEEPROM->EepromOrEfuse = (eeValue & BOOT_FROM_EEPROM) ? _TRUE : _FALSE;
  pEEPROM->bautoload_fail_flag = (eeValue & EEPROM_EN) ? _FALSE : _TRUE;
  
  DBG_871X ("%s: 9346CR=0x%02X, Boot from %s, Autoload %s\n",
            __FUNCTION__, eeValue,
            (pEEPROM->EepromOrEfuse ? "EEPROM" : "EFUSE"),
            (pEEPROM->bautoload_fail_flag ? "Fail" : "OK") );
            

  #ifdef CONFIG_EFUSE_CONFIG_FILE
  Hal_readPGDataFromConfigFile (padapter);
  #else
  Hal_InitPGData88E (padapter);
  #endif
  readAdapterInfo_8188ES (padapter);
}

static VOID
_InitOtherVariable (
  IN PADAPTER     Adapter
)
{
  
  
  
  
}

static s32 _ReadAdapterInfo8188ES (PADAPTER padapter)
{
  u32 start;
  
  
  RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("+_ReadAdapterInfo8188ES\n") );
  
  if (_CardEnable (padapter) == _FAIL)
  {
    DBG_871X (KERN_ERR "%s: run power on flow fail\n", __func__);
    return _FAIL;
  }
  
  start = rtw_get_current_time();
  

  _ReadRFType (padapter);
  _ReadPROMContent (padapter);
  
  _InitOtherVariable (padapter);
  
  
  MSG_8192C ("<==== ReadAdapterInfo8188ES in %d ms\n", rtw_get_passing_time_ms (start) );
  
  return _SUCCESS;
}

static void ReadAdapterInfo8188ES (PADAPTER padapter)
{
  padapter->EepromAddressSize = GetEEPROMSize8188E (padapter);
  
  _ReadAdapterInfo8188ES (padapter);
}

static void ResumeTxBeacon (PADAPTER padapter)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  
  
  RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("+ResumeTxBeacon\n") );
  
  rtw_write8 (padapter, REG_FWHW_TXQ_CTRL + 2, (pHalData->RegFwHwTxQCtrl) | BIT6);
  pHalData->RegFwHwTxQCtrl |= BIT6;
  rtw_write8 (padapter, REG_TBTT_PROHIBIT + 1, 0xff);
  pHalData->RegReg542 |= BIT0;
  rtw_write8 (padapter, REG_TBTT_PROHIBIT + 2, pHalData->RegReg542);
}

static void StopTxBeacon (PADAPTER padapter)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  
  
  RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("+StopTxBeacon\n") );
  
  rtw_write8 (padapter, REG_FWHW_TXQ_CTRL + 2, (pHalData->RegFwHwTxQCtrl) & (~BIT6) );
  pHalData->RegFwHwTxQCtrl &= (~BIT6);
  rtw_write8 (padapter, REG_TBTT_PROHIBIT + 1, 0x64);
  pHalData->RegReg542 &= ~ (BIT0);
  rtw_write8 (padapter, REG_TBTT_PROHIBIT + 2, pHalData->RegReg542);
  
  CheckFwRsvdPageContent (padapter);
}

void hw_var_set_opmode (PADAPTER Adapter, u8 variable, u8 * val)
{
  u8  val8;
  u8  mode = * ( (u8 *) val);
  
  #ifdef CONFIG_CONCURRENT_MODE
  if (Adapter->iface_type == IFACE_PORT1)
  {
    rtw_write8 (Adapter, REG_BCN_CTRL_1, rtw_read8 (Adapter, REG_BCN_CTRL_1) | BIT (4) );
    
    val8 = rtw_read8 (Adapter, MSR) & 0x03;
    val8 |= (mode << 2);
    rtw_write8 (Adapter, MSR, val8);
    
    DBG_871X ("%s()-%d mode = %d\n", __FUNCTION__, __LINE__, mode);
    
    if ( (mode == _HW_STATE_STATION_) || (mode == _HW_STATE_NOLINK_) )
    {
      if (!check_buddy_mlmeinfo_state (Adapter, WIFI_FW_AP_STATE) )
      {
        #ifdef CONFIG_INTERRUPT_BASED_TXBCN
      
        #ifdef  CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
        rtw_write8 (Adapter, REG_DRVERLYINT, 0x05);
        UpdateInterruptMask8188ESdio (Adapter, 0, SDIO_HIMR_BCNERLY_INT_MSK);
        #endif
        
        #ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
        UpdateInterruptMask8188ESdio (Adapter, 0, (SDIO_HIMR_TXBCNOK_MSK | SDIO_HIMR_TXBCNERR_MSK) );
        #endif// CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
        
        #endif
        
        
        StopTxBeacon (Adapter);
      }
      
      rtw_write8 (Adapter, REG_BCN_CTRL_1, 0x19);
    }
    else
      if ( (mode == _HW_STATE_ADHOC_) /*|| (mode == _HW_STATE_AP_)*/)
      {
        ResumeTxBeacon (Adapter);
        rtw_write8 (Adapter, REG_BCN_CTRL_1, 0x1a);
      }
      else
        if (mode == _HW_STATE_AP_)
        {
          #ifdef CONFIG_INTERRUPT_BASED_TXBCN
          #ifdef  CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
          UpdateInterruptMask8188ESdio (Adapter, SDIO_HIMR_BCNERLY_INT_MSK, 0);
          #endif//CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
          
          #ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
          UpdateInterruptMask8188ESdio (Adapter, (SDIO_HIMR_TXBCNOK_MSK | SDIO_HIMR_TXBCNERR_MSK), 0);
          #endif//CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
          
          #endif
          
          ResumeTxBeacon (Adapter);
          
          rtw_write8 (Adapter, REG_BCN_CTRL_1, 0x12);
          
          rtw_write32 (Adapter, REG_CR, rtw_read32 (Adapter, REG_CR) | BIT (8) );
          
          rtw_write32 (Adapter, REG_RCR, 0x7000208e);
          
          rtw_write16 (Adapter, REG_RXFLTMAP2, 0xFFFF);
          rtw_write16 (Adapter, REG_RXFLTMAP1, 0x0400);
          
          rtw_write8 (Adapter, REG_BCNDMATIM, 0x02);
          
          rtw_write8 (Adapter, REG_ATIMWND_1, 0x0a);
          rtw_write16 (Adapter, REG_BCNTCFG, 0x00);
          rtw_write16 (Adapter, REG_TBTT_PROHIBIT, 0xff04);
          rtw_write16 (Adapter, REG_TSFTR_SYN_OFFSET, 0x7fff);
          
          rtw_write8 (Adapter, REG_DUAL_TSF_RST, BIT (1) );
          
          
          rtw_write8 (Adapter, REG_MBID_NUM, rtw_read8 (Adapter, REG_MBID_NUM) | BIT (3) | BIT (4) );
          rtw_write8 (Adapter, REG_BCN_CTRL_1, (DIS_TSF_UDT0_NORMAL_CHIP | EN_BCN_FUNCTION | EN_TXBCN_RPT | BIT (1) ) );
          
          #ifdef CONFIG_CONCURRENT_MODE
          if (check_buddy_fwstate (Adapter, WIFI_FW_NULL_STATE) )
            rtw_write8 (Adapter, REG_BCN_CTRL,
                        rtw_read8 (Adapter, REG_BCN_CTRL) & ~EN_BCN_FUNCTION);
          #endif
          
          rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) | BIT (0) );
          
          #ifdef CONFIG_TSF_RESET_OFFLOAD
          if ( check_buddy_fwstate (Adapter, (WIFI_STATION_STATE | WIFI_ASOC_STATE) ) ) {
            if (reset_tsf (Adapter, IFACE_PORT1) == _FALSE)
              DBG_871X ("ERROR! %s()-%d: Reset port1 TSF fail\n",
                        __FUNCTION__, __LINE__);
          }
          #endif 
        }
  }
  else
  #endif
  {
    rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) | BIT (4) );
    
    val8 = rtw_read8 (Adapter, MSR) & 0x0c;
    val8 |= mode;
    rtw_write8 (Adapter, MSR, val8);
    
    DBG_871X ("%s()-%d mode = %d\n", __FUNCTION__, __LINE__, mode);
    
    if ( (mode == _HW_STATE_STATION_) || (mode == _HW_STATE_NOLINK_) )
    {
      #ifdef CONFIG_CONCURRENT_MODE
      if (!check_buddy_mlmeinfo_state (Adapter, WIFI_FW_AP_STATE) )
      #endif
      {
        #ifdef CONFIG_INTERRUPT_BASED_TXBCN
        #ifdef  CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
        rtw_write8 (Adapter, REG_DRVERLYINT, 0x05);
        UpdateInterruptMask8188ESdio (Adapter, 0, SDIO_HIMR_BCNERLY_INT_MSK);
        #endif//CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
        
        #ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
        UpdateInterruptMask8188ESdio (Adapter, 0, (SDIO_HIMR_TXBCNOK_MSK | SDIO_HIMR_TXBCNERR_MSK) );
        #endif
        #endif
        StopTxBeacon (Adapter);
      }
      
      rtw_write8 (Adapter, REG_BCN_CTRL, 0x19);
    }
    else
      if ( (mode == _HW_STATE_ADHOC_) /*|| (mode == _HW_STATE_AP_)*/)
      {
        ResumeTxBeacon (Adapter);
        rtw_write8 (Adapter, REG_BCN_CTRL, 0x1a);
      }
      else
        if (mode == _HW_STATE_AP_)
        {
        
          #ifdef CONFIG_INTERRUPT_BASED_TXBCN
          #ifdef  CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
          UpdateInterruptMask8188ESdio (Adapter, SDIO_HIMR_BCNERLY_INT_MSK, 0);
          #endif//CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
          
          #ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
          UpdateInterruptMask8188ESdio (Adapter, (SDIO_HIMR_TXBCNOK_MSK | SDIO_HIMR_TXBCNERR_MSK), 0);
          #endif//CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
          #endif
          
          
          ResumeTxBeacon (Adapter);
          
          rtw_write8 (Adapter, REG_BCN_CTRL, 0x12);
          
          rtw_write32 (Adapter, REG_CR, rtw_read32 (Adapter, REG_CR) | BIT (8) );
          
          rtw_write32 (Adapter, REG_RCR, 0x7000208e);
          rtw_write16 (Adapter, REG_RXFLTMAP2, 0xFFFF);
          rtw_write16 (Adapter, REG_RXFLTMAP1, 0x0400);
          
          rtw_write8 (Adapter, REG_BCNDMATIM, 0x02);
          
          rtw_write8 (Adapter, REG_ATIMWND, 0x0a);
          rtw_write16 (Adapter, REG_BCNTCFG, 0x00);
          rtw_write16 (Adapter, REG_TBTT_PROHIBIT, 0xff04);
          rtw_write16 (Adapter, REG_TSFTR_SYN_OFFSET, 0x7fff);
          
          rtw_write8 (Adapter, REG_DUAL_TSF_RST, BIT (0) );
          
          rtw_write8 (Adapter, REG_MBID_NUM, rtw_read8 (Adapter, REG_MBID_NUM) | BIT (3) | BIT (4) );
          
          rtw_write8 (Adapter, REG_BCN_CTRL, (DIS_TSF_UDT0_NORMAL_CHIP | EN_BCN_FUNCTION | EN_TXBCN_RPT | BIT (1) ) );
          
          #ifdef CONFIG_CONCURRENT_MODE
          if (check_buddy_fwstate (Adapter, WIFI_FW_NULL_STATE) )
            rtw_write8 (Adapter, REG_BCN_CTRL_1,
                        rtw_read8 (Adapter, REG_BCN_CTRL_1) & ~EN_BCN_FUNCTION);
          #endif
                        
          rtw_write8 (Adapter, REG_BCN_CTRL_1, rtw_read8 (Adapter, REG_BCN_CTRL_1) | BIT (0) );
          #ifdef CONFIG_TSF_RESET_OFFLOAD
          if ( check_buddy_fwstate (Adapter, (WIFI_STATION_STATE | WIFI_ASOC_STATE) ) ) {
            if (reset_tsf (Adapter, IFACE_PORT0) == _FALSE)
              DBG_871X ("ERROR! %s()-%d: Reset port0 TSF fail\n",
                        __FUNCTION__, __LINE__);
          }
          #endif 
        }
  }
  
}

static void hw_var_set_macaddr (PADAPTER Adapter, u8 variable, u8 * val)
{
  u8 idx = 0;
  u32 reg_macid;
  
  #ifdef CONFIG_CONCURRENT_MODE
  if (Adapter->iface_type == IFACE_PORT1)
  {
    reg_macid = REG_MACID1;
  }
  else
  #endif
  {
    reg_macid = REG_MACID;
  }
  
  for (idx = 0 ; idx < 6; idx++)
  {
    rtw_write8 (Adapter, (reg_macid + idx), val[idx]);
  }
  
}

static void hw_var_set_bssid (PADAPTER Adapter, u8 variable, u8 * val)
{
  u8  idx = 0;
  u32 reg_bssid;
  
  
  #ifdef CONFIG_CONCURRENT_MODE
  if (Adapter->iface_type == IFACE_PORT1)
  {
    reg_bssid = REG_BSSID1;
  }
  else
  #endif
  {
    reg_bssid = REG_BSSID;
  }
  
  printk ("hw_var_set_bssid   reg=%x \n", reg_bssid);
  
  for (idx = 0 ; idx < 6; idx++)
  {
    rtw_write8 (Adapter, (reg_bssid + idx), val[idx]);
  }
  
}

static void hw_var_set_bcn_func (PADAPTER Adapter, u8 variable, u8 * val)
{
  u32 bcn_ctrl_reg;
  
  #ifdef CONFIG_CONCURRENT_MODE
  if (Adapter->iface_type == IFACE_PORT1)
  {
    bcn_ctrl_reg = REG_BCN_CTRL_1;
  }
  else
  #endif
  {
    bcn_ctrl_reg = REG_BCN_CTRL;
  }
  
  if (* ( (u8 *) val) )
  {
    rtw_write8 (Adapter, bcn_ctrl_reg, (EN_BCN_FUNCTION | EN_TXBCN_RPT) );
  }
  else
  {
    rtw_write8 (Adapter, bcn_ctrl_reg, rtw_read8 (Adapter, bcn_ctrl_reg) & (~ (EN_BCN_FUNCTION | EN_TXBCN_RPT) ) );
  }
  
  
}

static void hw_var_set_correct_tsf (PADAPTER Adapter, u8 variable, u8 * val)
{
  #ifdef CONFIG_CONCURRENT_MODE
  u64 tsf;
  struct mlme_ext_priv * pmlmeext = &Adapter->mlmeextpriv;
  struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
  PADAPTER pbuddy_adapter = Adapter->pbuddy_adapter;
  
  tsf = pmlmeext->TSFValue - rtw_modular64 (pmlmeext->TSFValue, (pmlmeinfo->bcn_interval * 1024) ) - 1024;
  
  if ( ( (pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) || ( (pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) )
  {
    StopTxBeacon (Adapter);
  }
  
  if (Adapter->iface_type == IFACE_PORT1)
  {
    rtw_write8 (Adapter, REG_BCN_CTRL_1, rtw_read8 (Adapter, REG_BCN_CTRL_1) & (~BIT (3) ) );
    
    rtw_write32 (Adapter, REG_TSFTR1, tsf);
    rtw_write32 (Adapter, REG_TSFTR1 + 4, tsf >> 32);
    rtw_write8 (Adapter, REG_BCN_CTRL_1, rtw_read8 (Adapter, REG_BCN_CTRL_1) | BIT (3) );
    if ( (pmlmeinfo->state & 0x03) == WIFI_FW_STATION_STATE
         && check_buddy_fwstate (Adapter, WIFI_AP_STATE)
       ) {
      rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) & (~BIT (3) ) );
      
      rtw_write32 (Adapter, REG_TSFTR, tsf);
      rtw_write32 (Adapter, REG_TSFTR + 4, tsf >> 32);
      
      rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) | BIT (3) );
      #ifdef CONFIG_TSF_RESET_OFFLOAD
      if (reset_tsf (Adapter, IFACE_PORT0) == _FALSE)
        DBG_871X ("ERROR! %s()-%d: Reset port0 TSF fail\n",
                  __FUNCTION__, __LINE__);
                  
      #endif 
    }
    
  }
  else
  {
    rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) & (~BIT (3) ) );
    
    rtw_write32 (Adapter, REG_TSFTR, tsf);
    rtw_write32 (Adapter, REG_TSFTR + 4, tsf >> 32);
    
    rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) | BIT (3) );
    if ( (pmlmeinfo->state & 0x03) == WIFI_FW_STATION_STATE
         && check_buddy_fwstate (Adapter, WIFI_AP_STATE)
       ) {
      rtw_write8 (Adapter, REG_BCN_CTRL_1, rtw_read8 (Adapter, REG_BCN_CTRL_1) & (~BIT (3) ) );
      
      rtw_write32 (Adapter, REG_TSFTR1, tsf);
      rtw_write32 (Adapter, REG_TSFTR1 + 4, tsf >> 32);
      
      rtw_write8 (Adapter, REG_BCN_CTRL_1, rtw_read8 (Adapter, REG_BCN_CTRL_1) | BIT (3) );
      #ifdef CONFIG_TSF_RESET_OFFLOAD
      if (reset_tsf (Adapter, IFACE_PORT1) == _FALSE)
        DBG_871X ("ERROR! %s()-%d: Reset port1 TSF fail\n",
                  __FUNCTION__, __LINE__);
      #endif 
    }
  }
  
  
  if ( ( (pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) || ( (pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) )
  {
    ResumeTxBeacon (Adapter);
  }
  #endif
}

static void hw_var_set_mlme_disconnect (PADAPTER Adapter, u8 variable, u8 * val)
{
  #ifdef CONFIG_CONCURRENT_MODE
  PADAPTER pbuddy_adapter = Adapter->pbuddy_adapter;
  
  
  if (check_buddy_mlmeinfo_state (Adapter, _HW_STATE_NOLINK_) )
  { rtw_write16 (Adapter, REG_RXFLTMAP2, 0x00); }
  
  
  if (Adapter->iface_type == IFACE_PORT1)
  {
    rtw_write8 (Adapter, REG_DUAL_TSF_RST, BIT (1) );
    
    rtw_write8 (Adapter, REG_BCN_CTRL_1, rtw_read8 (Adapter, REG_BCN_CTRL_1) | BIT (4) );
  }
  else
  {
    rtw_write8 (Adapter, REG_DUAL_TSF_RST, BIT (0) );
    
    rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) | BIT (4) );
  }
  #endif
}

static void hw_var_set_mlme_sitesurvey (PADAPTER Adapter, u8 variable, u8 * val)
{
  #ifdef CONFIG_CONCURRENT_MODE
  
  struct mlme_ext_priv * pmlmeext = &Adapter->mlmeextpriv;
  struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
  
  if (* ( (u8 *) val) )
  {
    u32 v = rtw_read32 (Adapter, REG_RCR);
    v &= ~ (RCR_CBSSID_BCN);
    rtw_write32 (Adapter, REG_RCR, v);
    
    if ( (pmlmeinfo->state & 0x03) == WIFI_FW_STATION_STATE)
    {
      if (Adapter->iface_type == IFACE_PORT1)
      {
        rtw_write8 (Adapter, REG_BCN_CTRL_1, rtw_read8 (Adapter, REG_BCN_CTRL_1) | BIT (4) );
      }
      else
      {
        rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) | BIT (4) );
      }
    }
    
    if (check_buddy_mlmeinfo_state (Adapter, WIFI_FW_AP_STATE) &&
        check_buddy_fwstate (Adapter, _FW_LINKED) )
    {
      StopTxBeacon (Adapter);
    }
    
  }
  else//sitesurvey done
  {
    rtw_write16 (Adapter, REG_RXFLTMAP2, 0xFFFF);
    
    if (Adapter->iface_type == IFACE_PORT1)
    { rtw_write8 (Adapter, REG_BCN_CTRL_1, rtw_read8 (Adapter, REG_BCN_CTRL_1) & (~BIT (4) ) ); }
    else
    { rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) & (~BIT (4) ) ); }
    
    rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | RCR_CBSSID_BCN);
    
    if (check_buddy_mlmeinfo_state (Adapter, WIFI_FW_AP_STATE) &&
        check_buddy_fwstate (Adapter, _FW_LINKED) )
    {
      ResumeTxBeacon (Adapter);
    }
    
  }
  #endif
}

static void hw_var_set_mlme_join (PADAPTER Adapter, u8 variable, u8 * val)
{
  #ifdef CONFIG_CONCURRENT_MODE
  u8  RetryLimit = 0x30;
  u8  type = * ( (u8 *) val);
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  struct mlme_priv * pmlmepriv = &Adapter->mlmepriv;
  
  if (type == 0)
  {
    if (check_buddy_mlmeinfo_state (Adapter, WIFI_FW_AP_STATE) &&
        check_buddy_fwstate (Adapter, _FW_LINKED) )
    {
      StopTxBeacon (Adapter);
    }
    
    rtw_write16 (Adapter, REG_RXFLTMAP2, 0xFFFF);
    
    if (check_buddy_mlmeinfo_state (Adapter, WIFI_FW_AP_STATE) )
    { rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | RCR_CBSSID_BCN); }
    else
    { rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | RCR_CBSSID_DATA | RCR_CBSSID_BCN); }
    
    if (check_fwstate (pmlmepriv, WIFI_STATION_STATE) == _TRUE)
    {
      RetryLimit = (pHalData->CustomerID == RT_CID_CCX) ? 7 : 48;
    }
    else
    {
      RetryLimit = 0x7;
    }
  }
  else
    if (type == 1)
    {
      if (check_buddy_mlmeinfo_state (Adapter, _HW_STATE_NOLINK_) )
      { rtw_write16 (Adapter, REG_RXFLTMAP2, 0x00); }
      
      if (check_buddy_mlmeinfo_state (Adapter, WIFI_FW_AP_STATE) &&
          check_buddy_fwstate (Adapter, _FW_LINKED) )
      {
        ResumeTxBeacon (Adapter);
        
        rtw_write8 (Adapter, REG_DUAL_TSF_RST, BIT (1) | BIT (0) );
        
      }
    }
    else
      if (type == 2)
      {
      
        if (Adapter->iface_type == IFACE_PORT1)
        { rtw_write8 (Adapter, REG_BCN_CTRL_1, rtw_read8 (Adapter, REG_BCN_CTRL_1) & (~BIT (4) ) ); }
        else
        { rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) & (~BIT (4) ) ); }
        
        
        if (check_fwstate (pmlmepriv, WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE) )
        {
          rtw_write8 (Adapter, 0x542 , 0x02);
          RetryLimit = 0x7;
        }
        
        
        if (check_buddy_mlmeinfo_state (Adapter, WIFI_FW_AP_STATE) &&
            check_buddy_fwstate (Adapter, _FW_LINKED) )
        {
          ResumeTxBeacon (Adapter);
          
          rtw_write8 (Adapter, REG_DUAL_TSF_RST, BIT (1) | BIT (0) );
        }
        
      }
      
  rtw_write16 (Adapter, REG_RL, RetryLimit << RETRY_LIMIT_SHORT_SHIFT | RetryLimit << RETRY_LIMIT_LONG_SHIFT);
  
  #endif
}
static void SetHwReg8188ES (PADAPTER Adapter, u8 variable, u8 * val)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  struct dm_priv * pdmpriv = &pHalData->dmpriv;
  DM_ODM_T  *  podmpriv = &pHalData->odmpriv;
  _func_enter_;
  
  switch (variable)
  {
  case HW_VAR_MEDIA_STATUS:
    {
      u8 val8;
      
      val8 = rtw_read8 (Adapter, MSR) & 0x0c;
      val8 |= * ( (u8 *) val);
      rtw_write8 (Adapter, MSR, val8);
    }
    break;
  case HW_VAR_MEDIA_STATUS1:
    {
      u8 val8;
      
      val8 = rtw_read8 (Adapter, MSR) & 0x03;
      val8 |= * ( (u8 *) val) << 2;
      rtw_write8 (Adapter, MSR, val8);
    }
    break;
  case HW_VAR_SET_OPMODE:
    hw_var_set_opmode (Adapter, variable, val);
    break;
  case HW_VAR_MAC_ADDR:
    hw_var_set_macaddr (Adapter, variable, val);
    break;
  case HW_VAR_BSSID:
    hw_var_set_bssid (Adapter, variable, val);
    break;
  case HW_VAR_BASIC_RATE:
    {
      u16     BrateCfg = 0;
      u8      RateIndex = 0;
      
      HalSetBrateCfg ( Adapter, val, &BrateCfg );
      DBG_8192C ("HW_VAR_BASIC_RATE: BrateCfg(%#x)\n", BrateCfg);
      
      
      pHalData->BasicRateSet = BrateCfg = (BrateCfg | 0xd) & 0x15d;
      
      BrateCfg |= 0x01;
      rtw_write8 (Adapter, REG_RRSR, BrateCfg & 0xff);
      rtw_write8 (Adapter, REG_RRSR + 1, (BrateCfg >> 8) & 0xff);
      rtw_write8 (Adapter, REG_RRSR + 2, rtw_read8 (Adapter, REG_RRSR + 2) & 0xf0);
      
      while (BrateCfg > 0x1)
      {
        BrateCfg = (BrateCfg >> 1);
        RateIndex++;
      }
      rtw_write8 (Adapter, REG_INIRTS_RATE_SEL, RateIndex);
    }
    break;
  case HW_VAR_TXPAUSE:
    rtw_write8 (Adapter, REG_TXPAUSE, * ( (u8 *) val) );
    break;
  case HW_VAR_BCN_FUNC:
    hw_var_set_bcn_func (Adapter, variable, val);
    break;
  case HW_VAR_CORRECT_TSF:
    #ifdef CONFIG_CONCURRENT_MODE
    hw_var_set_correct_tsf (Adapter, variable, val);
    #else
    {
      u64 tsf;
      struct mlme_ext_priv * pmlmeext = &Adapter->mlmeextpriv;
      struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
    
      tsf = pmlmeext->TSFValue - rtw_modular64 (pmlmeext->TSFValue, (pmlmeinfo->bcn_interval * 1024) ) - 1024;
    
      if ( ( (pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) ||
           ( (pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) )
      {
        StopTxBeacon (Adapter);
      }
    
      rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) & (~BIT (3) ) );
    
      rtw_write32 (Adapter, REG_TSFTR, tsf);
      rtw_write32 (Adapter, REG_TSFTR + 4, tsf >> 32);
    
      rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) | BIT (3) );
    
      if ( ( (pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) ||
           ( (pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) )
      {
        ResumeTxBeacon (Adapter);
      }
    }
    #endif
    break;
  case HW_VAR_CHECK_BSSID:
    if (* ( (u8 *) val) )
    {
      rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | RCR_CBSSID_DATA | RCR_CBSSID_BCN);
    }
    else
    {
      u32 val32;
      
      val32 = rtw_read32 (Adapter, REG_RCR);
      
      val32 &= ~ (RCR_CBSSID_DATA | RCR_CBSSID_BCN);
      
      rtw_write32 (Adapter, REG_RCR, val32);
    }
    break;
  case HW_VAR_MLME_DISCONNECT:
    #ifdef CONFIG_CONCURRENT_MODE
    hw_var_set_mlme_disconnect (Adapter, variable, val);
    #else
    {
      rtw_write16 (Adapter, REG_RXFLTMAP2, 0x00);
    
      rtw_write8 (Adapter, REG_DUAL_TSF_RST, (BIT (0) | BIT (1) ) );
    
      rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) | BIT (4) );
    }
    #endif
    break;
  case HW_VAR_MLME_SITESURVEY:
    #ifdef CONFIG_CONCURRENT_MODE
    hw_var_set_mlme_sitesurvey (Adapter, variable,  val);
    #else
    if (* ( (u8 *) val) )
    {
      u32 v = rtw_read32 (Adapter, REG_RCR);
      v &= ~ (RCR_CBSSID_BCN);
      rtw_write32 (Adapter, REG_RCR, v);
      rtw_write16 (Adapter, REG_RXFLTMAP2, 0x00);
    
      rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) | BIT (4) );
    }
    else//sitesurvey done
    {
      struct mlme_ext_priv * pmlmeext = &Adapter->mlmeextpriv;
      struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
    
      if ( (is_client_associated_to_ap (Adapter) == _TRUE) ||
           ( (pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) )
      {
        rtw_write16 (Adapter, REG_RXFLTMAP2, 0xFFFF);
    
        rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) & (~BIT (4) ) );
      }
      else
        if ( (pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE)
        {
          rtw_write16 (Adapter, REG_RXFLTMAP2, 0xFFFF);
    
          rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) & (~BIT (4) ) );
        }
    
      if (Adapter->in_cta_test)
      {
        if ( (pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE)
        {
          rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | RCR_CBSSID_BCN);
        }
        else
        {
          u32 v = rtw_read32 (Adapter, REG_RCR);
          v &= ~ (RCR_CBSSID_DATA | RCR_CBSSID_BCN );
          rtw_write32 (Adapter, REG_RCR, v);
        }
      }
      else
      {
        rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | RCR_CBSSID_BCN);
      }
    }
    #endif
    break;
  case HW_VAR_MLME_JOIN:
    #ifdef CONFIG_CONCURRENT_MODE
    hw_var_set_mlme_join (Adapter, variable,  val);
    #else
    {
      u8  RetryLimit = 0x30;
      u8  type = * ( (u8 *) val);
      struct mlme_priv * pmlmepriv = &Adapter->mlmepriv;
    
      if (type == 0)
      {
        rtw_write16 (Adapter, REG_RXFLTMAP2, 0xFFFF);
        if (Adapter->in_cta_test)
        {
          u32 v = rtw_read32 (Adapter, REG_RCR);
          v &= ~ (RCR_CBSSID_DATA | RCR_CBSSID_BCN );
          rtw_write32 (Adapter, REG_RCR, v);
        }
        else
        {
          rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | RCR_CBSSID_DATA | RCR_CBSSID_BCN);
        }
        if (check_fwstate (pmlmepriv, WIFI_STATION_STATE) == _TRUE)
        {
          RetryLimit = (pHalData->CustomerID == RT_CID_CCX) ? 7 : 48;
        }
        else
        {
          RetryLimit = 0x7;
        }
      }
      else
        if (type == 1)
        {
          rtw_write16 (Adapter, REG_RXFLTMAP2, 0x00);
        }
        else
          if (type == 2)
          {
            rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) & (~BIT (4) ) );
    
            if (check_fwstate (pmlmepriv, WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE) )
            {
              RetryLimit = 0x7;
            }
          }
    
      rtw_write16 (Adapter, REG_RL, RetryLimit << RETRY_LIMIT_SHORT_SHIFT | RetryLimit << RETRY_LIMIT_LONG_SHIFT);
    }
    #endif
    break;
  case HW_VAR_ON_RCR_AM:
    rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | RCR_AM);
    DBG_871X ("%s, %d, RCR= %x \n", __FUNCTION__, __LINE__, rtw_read32 (Adapter, REG_RCR) );
    break;
  case HW_VAR_OFF_RCR_AM:
    rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) & (~RCR_AM) );
    DBG_871X ("%s, %d, RCR= %x \n", __FUNCTION__, __LINE__, rtw_read32 (Adapter, REG_RCR) );
    break;
  case HW_VAR_BEACON_INTERVAL:
    rtw_write16 (Adapter, REG_BCN_INTERVAL, * ( (u16 *) val) );
    
    #ifdef  CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
    {
      struct mlme_ext_priv * pmlmeext = &Adapter->mlmeextpriv;
      struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
      u16 bcn_interval =  * ( (u16 *) val);
      if ( (pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) {
        DBG_8192C ("%s==> bcn_interval:%d, eraly_int:%d \n", __FUNCTION__, bcn_interval, bcn_interval >> 1);
        rtw_write8 (Adapter, REG_DRVERLYINT, bcn_interval >> 1);
      }
      else {
      
      }
    }
    #endif//CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
    
    break;
  case HW_VAR_SLOT_TIME:
    {
      u8  u1bAIFS, aSifsTime;
      struct mlme_ext_priv * pmlmeext = &Adapter->mlmeextpriv;
      struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
      
      rtw_write8 (Adapter, REG_SLOT, val[0]);
      
      if (pmlmeinfo->WMM_enable == 0)
      {
        if ( pmlmeext->cur_wireless_mode == WIRELESS_11B)
        { aSifsTime = 10; }
        else
        { aSifsTime = 16; }
        
        u1bAIFS = aSifsTime + (2 * pmlmeinfo->slotTime);
        
        rtw_write8 (Adapter, REG_EDCA_VO_PARAM, u1bAIFS);
        rtw_write8 (Adapter, REG_EDCA_VI_PARAM, u1bAIFS);
        rtw_write8 (Adapter, REG_EDCA_BE_PARAM, u1bAIFS);
        rtw_write8 (Adapter, REG_EDCA_BK_PARAM, u1bAIFS);
      }
    }
    break;
  case HW_VAR_RESP_SIFS:
    {
      #if 0
      rtw_write8 (Adapter, REG_SIFS_CTX + 1, val[0]);
      rtw_write8 (Adapter, REG_SIFS_TRX + 1, val[1]);
      
      rtw_write8 (Adapter, REG_SPEC_SIFS + 1, val[0]);
      rtw_write8 (Adapter, REG_MAC_SPEC_SIFS + 1, val[0]);
      
      rtw_write8 (Adapter, REG_R2T_SIFS + 1, val[0]);
      rtw_write8 (Adapter, REG_T2T_SIFS + 1, val[0]);
      #else
      rtw_write8 (Adapter, REG_R2T_SIFS, val[0]);
      rtw_write8 (Adapter, REG_R2T_SIFS + 1, val[1]);
      rtw_write8 (Adapter, REG_T2T_SIFS, val[2]);
      rtw_write8 (Adapter, REG_T2T_SIFS + 1, val[3]);
      #endif
    }
    break;
  case HW_VAR_ACK_PREAMBLE:
    {
      u8  regTmp;
      u8  bShortPreamble = * ( (PBOOLEAN) val );
      regTmp = (pHalData->nCur40MhzPrimeSC) << 5;
      if (bShortPreamble)
      { regTmp |= 0x80; }
      
      rtw_write8 (Adapter, REG_RRSR + 2, regTmp);
    }
    break;
  case HW_VAR_SEC_CFG:
    #ifdef CONFIG_CONCURRENT_MODE
    rtw_write8 (Adapter, REG_SECCFG, 0x0c | BIT (5) );
    #else
    rtw_write8 (Adapter, REG_SECCFG, * ( (u8 *) val) );
    #endif
    break;
  case HW_VAR_DM_FLAG:
    podmpriv->SupportAbility = * ( (u8 *) val);
    break;
  case HW_VAR_DM_FUNC_OP:
    if (val[0])
    {
      podmpriv->BK_SupportAbility = podmpriv->SupportAbility;
    }
    else
    {
      podmpriv->SupportAbility = podmpriv->BK_SupportAbility;
    }
    break;
  case HW_VAR_DM_FUNC_SET:
    if (* ( (u32 *) val) == DYNAMIC_ALL_FUNC_ENABLE) {
      pdmpriv->DMFlag = pdmpriv->InitDMFlag;
      podmpriv->SupportAbility =  pdmpriv->InitODMFlag;
    }
    else {
      podmpriv->SupportAbility |= * ( (u32 *) val);
    }
    break;
  case HW_VAR_DM_FUNC_CLR:
    podmpriv->SupportAbility &= * ( (u32 *) val);
    break;
  case HW_VAR_CAM_EMPTY_ENTRY:
    {
      u8  ucIndex = * ( (u8 *) val);
      u8  i;
      u32 ulCommand = 0;
      u32 ulContent = 0;
      u32 ulEncAlgo = CAM_AES;
      
      for (i = 0; i < CAM_CONTENT_COUNT; i++)
      {
        if ( i == 0)
        {
          ulContent |= (ucIndex & 0x03) | ( (u16) (ulEncAlgo) << 2);
        }
        else
        {
          ulContent = 0;
        }
        ulCommand = CAM_CONTENT_COUNT * ucIndex + i;
        ulCommand = ulCommand | CAM_POLLINIG | CAM_WRITE;
        rtw_write32 (Adapter, WCAMI, ulContent);
        rtw_write32 (Adapter, RWCAM, ulCommand);
      }
    }
    break;
  case HW_VAR_CAM_INVALID_ALL:
    rtw_write32 (Adapter, RWCAM, BIT (31) | BIT (30) );
    break;
  case HW_VAR_CAM_WRITE:
    {
      u32 cmd;
      u32 * cam_val = (u32 *) val;
      rtw_write32 (Adapter, WCAMI, cam_val[0]);
      
      cmd = CAM_POLLINIG | CAM_WRITE | cam_val[1];
      rtw_write32 (Adapter, RWCAM, cmd);
    }
    break;
  case HW_VAR_AC_PARAM_VO:
    rtw_write32 (Adapter, REG_EDCA_VO_PARAM, ( (u32 *) (val) ) [0]);
    break;
  case HW_VAR_AC_PARAM_VI:
    rtw_write32 (Adapter, REG_EDCA_VI_PARAM, ( (u32 *) (val) ) [0]);
    break;
  case HW_VAR_AC_PARAM_BE:
    pHalData->AcParam_BE = ( (u32 *) (val) ) [0];
    rtw_write32 (Adapter, REG_EDCA_BE_PARAM, ( (u32 *) (val) ) [0]);
    break;
  case HW_VAR_AC_PARAM_BK:
    rtw_write32 (Adapter, REG_EDCA_BK_PARAM, ( (u32 *) (val) ) [0]);
    break;
  case HW_VAR_AMPDU_MIN_SPACE:
    {
      u8  MinSpacingToSet;
      u8  SecMinSpace;
      
      MinSpacingToSet = * ( (u8 *) val);
      if (MinSpacingToSet <= 7)
      {
        switch (Adapter->securitypriv.dot11PrivacyAlgrthm)
        {
        case _NO_PRIVACY_:
        case _AES_:
          SecMinSpace = 0;
          break;
          
        case _WEP40_:
        case _WEP104_:
        case _TKIP_:
        case _TKIP_WTMIC_:
          SecMinSpace = 6;
          break;
        default:
          SecMinSpace = 7;
          break;
        }
        
        if (MinSpacingToSet < SecMinSpace) {
          MinSpacingToSet = SecMinSpace;
        }
        
        rtw_write8 (Adapter, REG_AMPDU_MIN_SPACE, (rtw_read8 (Adapter, REG_AMPDU_MIN_SPACE) & 0xf8) | MinSpacingToSet);
      }
    }
    break;
  case HW_VAR_AMPDU_FACTOR:
    {
      u8  RegToSet_Normal[4] = {0x41, 0xa8, 0x72, 0xb9};
      u8  RegToSet_BT[4] = {0x31, 0x74, 0x42, 0x97};
      u8  FactorToSet;
      u8 * pRegToSet;
      u8  index = 0;
      
      #ifdef CONFIG_BT_COEXIST
      if ( (pHalData->bt_coexist.BT_Coexist) &&
           (pHalData->bt_coexist.BT_CoexistType == BT_CSR_BC4) )
      { pRegToSet = RegToSet_BT; }
      else
      #endif
        pRegToSet = RegToSet_Normal;
        
      FactorToSet = * ( (u8 *) val);
      if (FactorToSet <= 3)
      {
        FactorToSet = (1 << (FactorToSet + 2) );
        if (FactorToSet > 0xf)
        { FactorToSet = 0xf; }
        
        for (index = 0; index < 4; index++)
        {
          if ( (pRegToSet[index] & 0xf0) > (FactorToSet << 4) )
          { pRegToSet[index] = (pRegToSet[index] & 0x0f) | (FactorToSet << 4); }
          
          if ( (pRegToSet[index] & 0x0f) > FactorToSet)
          { pRegToSet[index] = (pRegToSet[index] & 0xf0) | (FactorToSet); }
          
          rtw_write8 (Adapter, (REG_AGGLEN_LMT + index), pRegToSet[index]);
        }
        
      }
    }
    break;
  case HW_VAR_RXDMA_AGG_PG_TH:
    rtw_write8 (Adapter, REG_RXDMA_AGG_PG_TH, * ( (u8 *) val) );
    break;
  case HW_VAR_SET_RPWM:
    #ifdef CONFIG_LPS_LCLK
    {
      u8  ps_state = * ( (u8 *) val);
      ps_state = ps_state & 0xC1;
      
      #ifdef CONFIG_EXT_CLK
      if (ps_state & BIT (6) )
      {
        EnableGpio5ClockReq (Adapter, _FALSE, 1);
      }
      #endif
      
      rtw_write8 (Adapter, SDIO_LOCAL_BASE | SDIO_REG_HRPWM1, ps_state);
    }
    #endif
    break;
  case HW_VAR_H2C_FW_PWRMODE:
    {
      u8  psmode = (* (u8 *) val);
      
      if ( (psmode != PS_MODE_ACTIVE) && (!IS_92C_SERIAL (pHalData->VersionID) ) )
      {
        ODM_RF_Saving (podmpriv, _TRUE);
      }
      rtl8188e_set_FwPwrMode_cmd (Adapter, psmode);
    }
    break;
  case HW_VAR_H2C_FW_JOINBSSRPT:
    {
      u8  mstatus = (* (u8 *) val);
      rtl8188e_set_FwJoinBssReport_cmd (Adapter, mstatus);
    }
    break;
    #ifdef CONFIG_P2P_PS
  case HW_VAR_H2C_FW_P2P_PS_OFFLOAD:
    {
      u8  p2p_ps_state = (* (u8 *) val);
      rtl8188e_set_p2p_ps_offload_cmd (Adapter, p2p_ps_state);
    }
    break;
    #endif
  case HW_VAR_INITIAL_GAIN:
    {
      DIG_T * pDigTable = &podmpriv->DM_DigTable;
      u32     rx_gain = ( (u32 *) (val) ) [0];
      
      if (rx_gain == 0xff) {
        ODM_Write_DIG (podmpriv, pDigTable->BackupIGValue);
      }
      else {
        pDigTable->BackupIGValue = pDigTable->CurIGValue;
        ODM_Write_DIG (podmpriv, rx_gain);
      }
    }
    break;
  case HW_VAR_TRIGGER_GPIO_0:
    break;
    #ifdef CONFIG_BT_COEXIST
  case HW_VAR_BT_SET_COEXIST:
    {
      u8  bStart = (* (u8 *) val);
      rtl8192c_set_dm_bt_coexist (Adapter, bStart);
    }
    break;
  case HW_VAR_BT_ISSUE_DELBA:
    {
      u8  dir = (* (u8 *) val);
      rtl8192c_issue_delete_ba (Adapter, dir);
    }
    break;
    #endif
    #if (RATE_ADAPTIVE_SUPPORT==1)
  case HW_VAR_RPT_TIMER_SETTING:
    {
      u16 min_rpt_time = (* (u16 *) val);
      
      
      ODM_RA_Set_TxRPT_Time (podmpriv, min_rpt_time);
    }
    break;
    #endif
    
    #ifdef CONFIG_SW_ANTENNA_DIVERSITY
  case HW_VAR_ANTENNA_DIVERSITY_LINK:
    ODM_SwAntDivRestAfterLink (podmpriv);
    break;
    #endif
    #ifdef CONFIG_ANTENNA_DIVERSITY
  case HW_VAR_ANTENNA_DIVERSITY_SELECT:
    {
      u8  Optimum_antenna = (* (u8 *) val);
      u8  Ant ;
      if (pHalData->CurAntenna !=  Optimum_antenna)
      {
        Ant = (Optimum_antenna == 2) ? MAIN_ANT : AUX_ANT;
        ODM_UpdateRxIdleAnt_88E (&pHalData->odmpriv, Ant);
        
        pHalData->CurAntenna = Optimum_antenna ;
      }
    }
    break;
    #endif
  case HW_VAR_EFUSE_BYTES:
    pHalData->EfuseUsedBytes = * ( (u16 *) val);
    break;
  case HW_VAR_FIFO_CLEARN_UP:
    {
      struct pwrctrl_priv * pwrpriv = &Adapter->pwrctrlpriv;
      u8 trycnt = 100;
      
      rtw_write8 (Adapter, REG_TXPAUSE, 0xff);
      
      Adapter->xmitpriv.nqos_ssn = rtw_read16 (Adapter, REG_NQOS_SEQ);
      
      rtw_write32 (Adapter, REG_RXPKT_NUM, (rtw_read32 (Adapter, REG_RXPKT_NUM) | RW_RELEASE_EN) );
      do {
        if (! (rtw_read32 (Adapter, REG_RXPKT_NUM) &RXDMA_IDLE) )
        { break; }
      }
      while (trycnt--);
      if (trycnt == 0)
      { DBG_8192C ("Stop RX DMA failed...... \n"); }
      
      rtw_write16 (Adapter, REG_RQPN_NPQ, 0x0);
      rtw_write32 (Adapter, REG_RQPN, 0x80000000);
      rtw_mdelay_os (10);
      
    }
    break;
  case HW_VAR_CHECK_TXBUF:
    #ifdef CONFIG_CONCURRENT_MODE
    {
      int i;
      u8  RetryLimit = 0x01;
      
      rtw_write16 (Adapter, REG_RL, RetryLimit << RETRY_LIMIT_SHORT_SHIFT | RetryLimit << RETRY_LIMIT_LONG_SHIFT);
      for (i = 0; i < 1000; i++)
      {
        if (rtw_read32 (Adapter, 0x200) != rtw_read32 (Adapter, 0x204) )
        {
          rtw_msleep_os (10);
        }
        else
        {
          DBG_871X ("no packet in tx packet buffer (%d)\n", i);
          break;
        }
      }
      
      RetryLimit = 0x30;
      rtw_write16 (Adapter, REG_RL, RetryLimit << RETRY_LIMIT_SHORT_SHIFT | RetryLimit << RETRY_LIMIT_LONG_SHIFT);
      
    }
    #endif
    break;
    #ifdef CONFIG_WOWLAN
  case HW_VAR_WOWLAN:
    {
      struct wowlan_ioctl_param * poidparam;
      struct recv_buf * precvbuf;
      int res, i;
      u32 tmp;
      u16 len = 0;
      u8 mstatus = (* (u8 *) val);
      u8 trycnt = 100;
      u8 data[4];
      
      poidparam = (struct wowlan_ioctl_param *) val;
      switch (poidparam->subcode) {
      case WOWLAN_ENABLE:
        DBG_871X_LEVEL (_drv_always_, "WOWLAN_ENABLE\n");
        
        SetFwRelatedForWoWLAN8188ES (Adapter, _TRUE);
        
        
        DBG_871X_LEVEL (_drv_always_, "Pause DMA\n");
        rtw_write32 (Adapter, REG_RXPKT_NUM, (rtw_read32 (Adapter, REG_RXPKT_NUM) | RW_RELEASE_EN) );
        do {
          if ( (rtw_read32 (Adapter, REG_RXPKT_NUM) &RXDMA_IDLE) ) {
            DBG_871X_LEVEL (_drv_always_, "RX_DMA_IDLE is true\n");
            break;
          }
          else {
            res = sdio_local_read (Adapter, SDIO_REG_RX0_REQ_LEN, 4, (u8 *) &tmp);
            if (tmp == 0) {
              res = sdio_local_read (Adapter, SDIO_REG_HISR, 4, (u8 *) &tmp);
              DBG_871X_LEVEL (_drv_info_, "read SDIO_REG_HISR: 0x%08x\n", tmp);
            }
            res = RecvOnePkt (Adapter, tmp);
            DBG_871X_LEVEL (_drv_always_, "RecvOnePkt Result: %d\n", res);
          }
        }
        while (trycnt--);
        if (trycnt == 0)
        { DBG_871X_LEVEL (_drv_always_, "Stop RX DMA failed...... \n"); }
        
        DBG_871X_LEVEL (_drv_always_, "Enable only CPWM2\n");
        res = sdio_local_read (Adapter, SDIO_REG_HIMR, 4, (u8 *) &tmp);
        if (!res)
        { DBG_871X_LEVEL (_drv_info_, "read SDIO_REG_HIMR: 0x%08x\n", tmp); }
        else
        { DBG_871X_LEVEL (_drv_info_, "sdio_local_read fail\n"); }
        
        tmp = SDIO_HIMR_CPWM2_MSK;
        
        res = sdio_local_write (Adapter, SDIO_REG_HIMR, 4, (u8 *) &tmp);
        
        if (!res) {
          res = sdio_local_read (Adapter, SDIO_REG_HIMR, 4, (u8 *) &tmp);
          DBG_871X_LEVEL (_drv_info_, "read again SDIO_REG_HIMR: 0x%08x\n", tmp);
        }
        else
        { DBG_871X_LEVEL (_drv_info_, "sdio_local_write fail\n"); }
        
        DBG_871X_LEVEL (_drv_always_, "Set WOWLan cmd\n");
        rtl8188es_set_wowlan_cmd (Adapter, 1);
        
        mstatus = rtw_read8 (Adapter, REG_WOW_CTRL);
        trycnt = 10;
        
        while (! (mstatus & BIT1) && trycnt > 1) {
          mstatus = rtw_read8 (Adapter, REG_WOW_CTRL);
          DBG_871X_LEVEL (_drv_always_, "Loop index: %d :0x%02x\n", trycnt, mstatus);
          trycnt --;
          rtw_msleep_os (2);
        }
        
        Adapter->pwrctrlpriv.wowlan_wake_reason = rtw_read8 (Adapter, REG_WOWLAN_WAKE_REASON);
        DBG_871X_LEVEL (_drv_always_, "wowlan_wake_reason: 0x%02x\n",
                        Adapter->pwrctrlpriv.wowlan_wake_reason);
                        
        break;
      case WOWLAN_DISABLE:
        DBG_871X_LEVEL (_drv_always_, "WOWLAN_DISABLE\n");
        trycnt = 10;
        Adapter->pwrctrlpriv.wowlan_wake_reason =
          rtw_read8 (Adapter, REG_WOWLAN_WAKE_REASON);
        DBG_871X_LEVEL (_drv_always_, "wakeup_reason: 0x%02x\n",
                        Adapter->pwrctrlpriv.wowlan_wake_reason);
        rtl8188es_set_wowlan_cmd (Adapter, 0);
        mstatus = rtw_read8 (Adapter, REG_WOW_CTRL);
        DBG_871X_LEVEL (_drv_info_, "%s mstatus:0x%02x\n", __func__, mstatus);
        
        while (mstatus & BIT1 && trycnt > 1) {
          mstatus = rtw_read8 (Adapter, REG_WOW_CTRL);
          DBG_871X_LEVEL (_drv_always_, "Loop index: %d :0x%02x\n", trycnt, mstatus);
          trycnt --;
          rtw_msleep_os (2);
        }
        
        if (mstatus & BIT1)
        { printk ("System did not release RX_DMA\n"); }
        else
        { SetFwRelatedForWoWLAN8188ES (Adapter, _FALSE); }
        
        EnableInterrupt8188ESdio (Adapter);
        rtw_msleep_os (2);
        if (! (Adapter->pwrctrlpriv.wowlan_wake_reason & FWDecisionDisconnect) ||
            ! (Adapter->pwrctrlpriv.wowlan_wake_reason & Rx_GTK) )
        { rtl8188e_set_FwJoinBssReport_cmd (Adapter, 1); }
        break;
      default:
        break;
      }
    }
    break;
    #endif
  case HW_VAR_APFM_ON_MAC:
    pHalData->bMacPwrCtrlOn = *val;
    DBG_871X ("%s: bMacPwrCtrlOn=%d\n", __func__, pHalData->bMacPwrCtrlOn);
    break;
    #if (RATE_ADAPTIVE_SUPPORT == 1)
  case HW_VAR_TX_RPT_MAX_MACID:
    {
      u8 maxMacid = *val;
      DBG_8192C ("### MacID(%d),Set Max Tx RPT MID(%d)\n", maxMacid, maxMacid + 1);
      rtw_write8 (Adapter, REG_TX_RPT_CTRL + 1, maxMacid + 1);
    }
    break;
    #endif 
  case HW_VAR_H2C_MEDIA_STATUS_RPT:
    {
      rtl8188e_set_FwMediaStatus_cmd (Adapter , (* (u16 *) val) );
    }
    break;
  case HW_VAR_BCN_VALID:
    rtw_write8 (Adapter, REG_TDECTRL + 2, rtw_read8 (Adapter, REG_TDECTRL + 2) | BIT0);
    break;
  default:
  
    break;
  }
  
  _func_exit_;
}

static void GetHwReg8188ES (PADAPTER padapter, u8 variable, u8 * val)
{
  PHAL_DATA_TYPE  pHalData = GET_HAL_DATA (padapter);
  DM_ODM_T  *  podmpriv = &pHalData->odmpriv;
  _func_enter_;
  
  switch (variable)
  {
  case HW_VAR_BASIC_RATE:
    * ( (u16 *) val) = pHalData->BasicRateSet;
    break;
    
  case HW_VAR_TXPAUSE:
    val[0] = rtw_read8 (padapter, REG_TXPAUSE);
    break;
    
  case HW_VAR_BCN_VALID:
    val[0] = (BIT0 & rtw_read8 (padapter, REG_TDECTRL + 2) ) ? _TRUE : _FALSE;
    break;
    
  case HW_VAR_DM_FLAG:
    val[0] = podmpriv->SupportAbility;
    break;
    
  case HW_VAR_RF_TYPE:
    val[0] = pHalData->rf_type;
    break;
    
  case HW_VAR_FWLPS_RF_ON:
    {
      if ( (padapter->bSurpriseRemoved == _TRUE) ||
           (padapter->pwrctrlpriv.rf_pwrstate == rf_off) )
      {
        val[0] = _TRUE;
      }
      else
      {
        u32 valRCR;
        valRCR = rtw_read32 (padapter, REG_RCR);
        valRCR &= 0x00070000;
        if (valRCR)
        { val[0] = _FALSE; }
        else
        { val[0] = _TRUE; }
      }
    }
    break;
    #ifdef CONFIG_ANTENNA_DIVERSITY
  case HW_VAR_CURRENT_ANTENNA:
    val[0] = pHalData->CurAntenna;
    break;
    #endif
  case HW_VAR_EFUSE_BYTES:
    * ( (u16 *) val) = pHalData->EfuseUsedBytes;
    break;
    
  case HW_VAR_APFM_ON_MAC:
    *val = pHalData->bMacPwrCtrlOn;
    break;
  case HW_VAR_CHK_HI_QUEUE_EMPTY:
    *val = ( (rtw_read32 (padapter, REG_HGQ_INFORMATION) & 0x0000ff00) == 0) ? _TRUE : _FALSE;
    break;
  default:
    break;
  }
  
  _func_exit_;
}

u8
GetHalDefVar8188ESDIO (
  IN  PADAPTER        Adapter,
  IN  HAL_DEF_VARIABLE    eVariable,
  IN  PVOID         pValue
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  u8      bResult = _SUCCESS;
  
  switch (eVariable)
  {
  case HAL_DEF_UNDERCORATEDSMOOTHEDPWDB:
    {
      struct mlme_priv * pmlmepriv = &Adapter->mlmepriv;
      struct sta_priv * pstapriv = &Adapter->stapriv;
      struct sta_info * psta;
      psta = rtw_get_stainfo (pstapriv, pmlmepriv->cur_network.network.MacAddress);
      if (psta)
      {
        * ( (int *) pValue) = psta->rssi_stat.UndecoratedSmoothedPWDB;
      }
    }
    
    break;
  case HAL_DEF_IS_SUPPORT_ANT_DIV:
    #ifdef CONFIG_ANTENNA_DIVERSITY
    * ( (u8 *) pValue) = (pHalData->AntDivCfg == 0) ? _FALSE : _TRUE;
    #endif
    break;
  case HAL_DEF_CURRENT_ANTENNA:
    #ifdef CONFIG_ANTENNA_DIVERSITY
    * ( ( u8 *) pValue) = pHalData->CurAntenna;
    #endif
    break;
  case HAL_DEF_DBG_DM_FUNC:
    * ( ( u32 *) pValue) = pHalData->odmpriv.SupportAbility;
    break;
    #if (RATE_ADAPTIVE_SUPPORT == 1)
  case HAL_DEF_RA_DECISION_RATE:
    {
      u8 MacID = * ( (u8 *) pValue);
      * ( (u8 *) pValue) = ODM_RA_GetDecisionRate_8188E (& (pHalData->odmpriv), MacID);
    }
    break;
    
  case HAL_DEF_RA_SGI:
    {
      u8 MacID = * ( (u8 *) pValue);
      * ( (u8 *) pValue) = ODM_RA_GetShortGI_8188E (& (pHalData->odmpriv), MacID);
    }
    break;
    #endif
    
    
  case HAL_DEF_PT_PWR_STATUS:
    #if(POWER_TRAINING_ACTIVE==1)
    {
      u8 MacID = * ( (u8 *) pValue);
      * ( (u8 *) pValue) = ODM_RA_GetHwPwrStatus_8188E (& (pHalData->odmpriv), MacID);
    }
    #endif
    break;
    
  case HW_VAR_MAX_RX_AMPDU_FACTOR:
    * ( ( u32 *) pValue) = MAX_AMPDU_FACTOR_16K;
    break;
    
  case HW_DEF_RA_INFO_DUMP:
    #if (RATE_ADAPTIVE_SUPPORT == 1)
    {
      u8 entry_id = * ( (u8 *) pValue);
      if (check_fwstate (&Adapter->mlmepriv, _FW_LINKED) == _TRUE)
      {
        DBG_871X ("============ RA status check ===================\n");
        DBG_8192C ("Mac_id:%d ,RateID = %d,RAUseRate = 0x%08x,RateSGI = %d, DecisionRate = 0x%02x ,PTStage = %d\n",
                   entry_id,
                   pHalData->odmpriv.RAInfo[entry_id].RateID,
                   pHalData->odmpriv.RAInfo[entry_id].RAUseRate,
                   pHalData->odmpriv.RAInfo[entry_id].RateSGI,
                   pHalData->odmpriv.RAInfo[entry_id].DecisionRate,
                   pHalData->odmpriv.RAInfo[entry_id].PTStage);
      }
    }
    #endif
    break;
    
  case HAL_DEF_DBG_DUMP_RXPKT:
    * ( ( u8 *) pValue) = pHalData->bDumpRxPkt;
    break;
  case HAL_DEF_DBG_DUMP_TXPKT:
    * ( ( u8 *) pValue) = pHalData->bDumpTxPkt;
    break;
  case HW_DEF_ODM_DBG_FLAG:
    {
      u8Byte  DebugComponents = * ( (u32 *) pValue);
      PDM_ODM_T pDM_Odm = & (pHalData->odmpriv);
      printk ("pDM_Odm->DebugComponents = 0x%llx \n", pDM_Odm->DebugComponents );
    }
    break;
  default:
    bResult = _FAIL;
    break;
  }
  
  return bResult;
}




u8
SetHalDefVar8188ESDIO (
  IN  PADAPTER        Adapter,
  IN  HAL_DEF_VARIABLE    eVariable,
  IN  PVOID         pValue
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  u8      bResult = _TRUE;
  
  switch (eVariable)
  {
  case HAL_DEF_DBG_DM_FUNC:
    {
      u8 dm_func = * ( ( u8 *) pValue);
      struct dm_priv * pdmpriv = &pHalData->dmpriv;
      DM_ODM_T  *  podmpriv = &pHalData->odmpriv;
      
      if (dm_func == 0) {
        podmpriv->SupportAbility = DYNAMIC_FUNC_DISABLE;
        DBG_8192C ("==> Disable all dynamic function...\n");
      }
      else
        if (dm_func == 1) {
          podmpriv->SupportAbility  &= (~DYNAMIC_BB_DIG);
          DBG_8192C ("==> Disable DIG...\n");
        }
        else
          if (dm_func == 2) {
            podmpriv->SupportAbility  &= (~DYNAMIC_BB_DYNAMIC_TXPWR);
          }
          else
            if (dm_func == 3) {
              podmpriv->SupportAbility  &= (~DYNAMIC_RF_CALIBRATION);
              DBG_8192C ("==> Disable tx power tracking...\n");
            }
            else
              if (dm_func == 5) {
                podmpriv->SupportAbility  &= (~DYNAMIC_BB_ANT_DIV);
              }
              else
                if (dm_func == 6) {
                  if (! (podmpriv->SupportAbility  & DYNAMIC_BB_DIG) )
                  {
                    DIG_T * pDigTable = &podmpriv->DM_DigTable;
                    pDigTable->CurIGValue = rtw_read8 (Adapter, 0xc50);
                  }
                  podmpriv->SupportAbility = DYNAMIC_ALL_FUNC_ENABLE;
                  DBG_8192C ("==> Turn on all dynamic function...\n");
                }
    }
    break;
  case HAL_DEF_DBG_DUMP_RXPKT:
    pHalData->bDumpRxPkt = * ( ( u8 *) pValue);
    break;
  case HAL_DEF_DBG_DUMP_TXPKT:
    pHalData->bDumpTxPkt = * ( ( u8 *) pValue);
    break;
  case HW_DEF_FA_CNT_DUMP:
    {
      u8 bRSSIDump = * ( (u8 *) pValue);
      PDM_ODM_T   pDM_Odm = & (pHalData->odmpriv);
      if (bRSSIDump)
      { pDM_Odm->DebugComponents  = ODM_COMP_DIG | ODM_COMP_FA_CNT  ; }
      else
      { pDM_Odm->DebugComponents  = 0; }
      
    }
    break;
  case HW_DEF_ODM_DBG_FLAG:
    {
      u8Byte  DebugComponents = * ( (u8Byte *) pValue);
      PDM_ODM_T pDM_Odm = & (pHalData->odmpriv);
      pDM_Odm->DebugComponents = DebugComponents;
    }
    break;
  default:
    bResult = _FALSE;
    break;
  }
  
  return bResult;
}

void UpdateHalRAMask8188ESdio (PADAPTER padapter, u32 mac_id, u8 rssi_level)
{
  u8  init_rate = 0;
  u8  networkType, raid;
  u32 mask, rate_bitmap;
  u8  shortGIrate = _FALSE;
  int supportRateNum = 0;
  struct sta_info * psta;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  struct mlme_ext_priv * pmlmeext = &padapter->mlmeextpriv;
  struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
  WLAN_BSSID_EX   *  cur_network = & (pmlmeinfo->network);
  #ifdef CONFIG_CONCURRENT_MODE
  if (rtw_buddy_adapter_up (padapter) && padapter->adapter_type > PRIMARY_ADAPTER)
  { pHalData = GET_HAL_DATA (padapter->pbuddy_adapter); }
  #endif
  
  if (mac_id >= NUM_STA)
  {
    return;
  }
  
  psta = pmlmeinfo->FW_sta_info[mac_id].psta;
  if (psta == NULL)
  {
    return;
  }
  
  switch (mac_id)
  {
  case 0:// for infra mode
    #ifdef CONFIG_CONCURRENT_MODE
  case 2:// first station uses macid=0, second station uses macid=2
    #endif
    supportRateNum = rtw_get_rateset_len (cur_network->SupportedRates);
    networkType = judge_network_type (padapter, cur_network->SupportedRates, supportRateNum) & 0xf;
    raid = networktype_to_raid (networkType);
    
    mask = update_supported_rate (cur_network->SupportedRates, supportRateNum);
    mask |= (pmlmeinfo->HT_enable) ? update_MSC_rate (& (pmlmeinfo->HT_caps) ) : 0;
    
    
    if (support_short_GI (padapter, & (pmlmeinfo->HT_caps) ) )
    {
      shortGIrate = _TRUE;
    }
    
    break;
    
  case 1://for broadcast/multicast
    supportRateNum = rtw_get_rateset_len (pmlmeinfo->FW_sta_info[mac_id].SupportedRates);
    if (pmlmeext->cur_wireless_mode & WIRELESS_11B)
    { networkType = WIRELESS_11B; }
    else
    { networkType = WIRELESS_11G; }
    raid = networktype_to_raid (networkType);
    mask = update_basic_rate (cur_network->SupportedRates, supportRateNum);
    
    
    break;
    
  default:
    supportRateNum = rtw_get_rateset_len (pmlmeinfo->FW_sta_info[mac_id].SupportedRates);
    networkType = judge_network_type (padapter, pmlmeinfo->FW_sta_info[mac_id].SupportedRates, supportRateNum) & 0xf;
    raid = networktype_to_raid (networkType);
    mask = update_supported_rate (cur_network->SupportedRates, supportRateNum);
    
    
    break;
  }
  
  rate_bitmap = 0x0fffffff;
  #ifdef  CONFIG_ODM_REFRESH_RAMASK
  {
    rate_bitmap = ODM_Get_Rate_Bitmap (&pHalData->odmpriv, mac_id, mask, rssi_level);
    DBG_8192C ("%s => mac_id:%d, networkType:0x%02x, mask:0x%08x\n\t ==> rssi_level:%d, rate_bitmap:0x%08x\n",
               __FUNCTION__, mac_id, networkType, mask, rssi_level, rate_bitmap);
  }
  #endif
  mask &= rate_bitmap;
  
  
  init_rate = get_highest_rate_idx (mask) & 0x3f;
  
  if (pHalData->fw_ractrl == _TRUE)
  {
    u8 arg = 0;
    
    arg = mac_id & 0x1f;
    
    arg |= BIT (7);
    
    if (shortGIrate == _TRUE)
    { arg |= BIT (5); }
    mask |= ( (raid << 28) & 0xf0000000);
    
    DBG_871X ("update raid entry, mask=0x%x, arg=0x%x\n", mask, arg);
    psta->ra_mask = mask;
    #ifdef CONFIG_INTEL_PROXIM
    if (padapter->proximity.proxim_on == _TRUE) {
      arg &= ~BIT (6);
    }
    else {
      arg |= BIT (6);
    }
    #endif
    rtl8188e_set_raid_cmd (padapter, mask);
    
  }
  else
  {
  
    #if(RATE_ADAPTIVE_SUPPORT == 1)
  
    ODM_RA_UpdateRateInfo_8188E (
      & (pHalData->odmpriv),
      mac_id,
      raid,
      mask,
      shortGIrate
    );
    
    #endif
  }
  
  
  psta->raid = raid;
  psta->init_rate = init_rate;
  
  
}


static VOID
_BeaconFunctionEnable (
  IN  PADAPTER    padapter,
  IN  BOOLEAN     Enable,
  IN  BOOLEAN     Linked
)
{
  rtw_write8 (padapter, REG_BCN_CTRL, (BIT4 | BIT3 | BIT1) );

  rtw_write8 (padapter, REG_RD_CTRL + 1, 0x6F);
}

void SetBeaconRelatedRegisters8188ESdio (PADAPTER padapter)
{
  u32 value32;
  struct mlme_ext_priv * pmlmeext = & (padapter->mlmeextpriv);
  struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
  u32 bcn_ctrl_reg    = REG_BCN_CTRL;
  
  
  
  #ifdef CONFIG_CONCURRENT_MODE
  if (padapter->iface_type == IFACE_PORT1) {
    bcn_ctrl_reg = REG_BCN_CTRL_1;
  }
  #endif
  rtw_write16 (padapter, REG_ATIMWND, 2);
  
  rtw_write16 (padapter, REG_BCN_INTERVAL, pmlmeinfo->bcn_interval);
  
  _InitBeaconParameters (padapter);
  
  rtw_write8 (padapter, REG_SLOT, 0x09);
  
  
  value32 = rtw_read32 (padapter, REG_TCR);
  value32 &= ~TSFRST;
  rtw_write32 (padapter,  REG_TCR, value32);
  
  value32 |= TSFRST;
  rtw_write32 (padapter, REG_TCR, value32);
  
  if (check_fwstate (&padapter->mlmepriv, WIFI_ADHOC_STATE | WIFI_AP_STATE) == _TRUE)
  {
    rtw_write8 (padapter, REG_RXTSF_OFFSET_CCK, 0x50);
    rtw_write8 (padapter, REG_RXTSF_OFFSET_OFDM, 0x50);
  }
  
  _BeaconFunctionEnable (padapter, _TRUE, _TRUE);
  
  ResumeTxBeacon (padapter);
  rtw_write8 (padapter, bcn_ctrl_reg, rtw_read8 (padapter, bcn_ctrl_reg) | BIT (1) );
}

void rtl8188es_set_hal_ops (PADAPTER padapter)
{
  struct hal_ops * pHalFunc = &padapter->HalFunc;
  
  _func_enter_;
  
  
  padapter->HalData = rtw_zmalloc (sizeof (HAL_DATA_TYPE) );
  
  if (padapter->HalData == NULL) {
    RT_TRACE (_module_hci_hal_init_c_, _drv_err_,
              ("can't alloc memory for HAL DATA\n") );
  }
  
  
  padapter->hal_data_sz = sizeof (HAL_DATA_TYPE);
  
  pHalFunc->hal_power_on = rtl8188es_InitPowerOn;
  pHalFunc->hal_init = &rtl8188es_hal_init;
  pHalFunc->hal_deinit = &rtl8188es_hal_deinit;
  
  pHalFunc->inirp_init = &rtl8188es_inirp_init;
  pHalFunc->inirp_deinit = &rtl8188es_inirp_deinit;
  
  pHalFunc->init_xmit_priv = &rtl8188es_init_xmit_priv;
  pHalFunc->free_xmit_priv = &rtl8188es_free_xmit_priv;
  
  pHalFunc->init_recv_priv = &rtl8188es_init_recv_priv;
  pHalFunc->free_recv_priv = &rtl8188es_free_recv_priv;
  
  pHalFunc->InitSwLeds = &rtl8188es_InitSwLeds;
  pHalFunc->DeInitSwLeds = &rtl8188es_DeInitSwLeds;
  
  pHalFunc->init_default_value = &rtl8188es_init_default_value;
  pHalFunc->intf_chip_configure = &rtl8188es_interface_configure;
  pHalFunc->read_adapter_info = &ReadAdapterInfo8188ES;
  
  pHalFunc->enable_interrupt = &EnableInterrupt8188ESdio;
  pHalFunc->disable_interrupt = &DisableInterrupt8188ESdio;
  
  #ifdef COWFIG_WOWLAN
  pHalFunc->disable_interrupt = &ClearInterrupt8189ESdio;
  #endif
  
  pHalFunc->SetHwRegHandler = &SetHwReg8188ES;
  pHalFunc->GetHwRegHandler = &GetHwReg8188ES;
  
  pHalFunc->GetHalDefVarHandler = &GetHalDefVar8188ESDIO;
  pHalFunc->SetHalDefVarHandler = &SetHalDefVar8188ESDIO;
  
  pHalFunc->UpdateRAMaskHandler = &UpdateHalRAMask8188ESdio;
  pHalFunc->SetBeaconRelatedRegistersHandler = &SetBeaconRelatedRegisters8188ESdio;
  
  pHalFunc->hal_xmit = &rtl8188es_hal_xmit;
  pHalFunc->mgnt_xmit = &rtl8188es_mgnt_xmit;
  pHalFunc->hal_xmitframe_enqueue = &rtl8188es_hal_xmitframe_enqueue;
  
  #ifdef CONFIG_HOSTAPD_MLME
  pHalFunc->hostap_mgnt_xmit_entry = NULL;
  #endif
  rtl8188e_set_hal_ops (pHalFunc);
  _func_exit_;
  
}

