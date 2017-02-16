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
#define _HCI_HAL_INIT_C_

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>
#include <rtw_efuse.h>

#include <rtl8188e_hal.h>
#include <rtl8188e_led.h>

#ifdef CONFIG_IOL
#include <rtw_iol.h>
#endif

#if defined (PLATFORM_LINUX) && defined (PLATFORM_WINDOWS)

#error "Shall be Linux or Windows, but not both!\n"

#endif

#ifndef CONFIG_USB_HCI

#error "CONFIG_USB_HCI shall be on!\n"

#endif

#include <usb_ops.h>
#include <usb_hal.h>
#include <usb_osintf.h>

#ifdef CONFIG_EFUSE_CONFIG_FILE
#include <linux/fs.h>
#include <asm/uaccess.h>
#endif

#if DISABLE_BB_RF
#define   HAL_MAC_ENABLE  0
#define   HAL_BB_ENABLE   0
#define   HAL_RF_ENABLE   0
#else
#define   HAL_MAC_ENABLE  1
#define   HAL_BB_ENABLE   1
#define   HAL_RF_ENABLE   1
#endif


static VOID
_ConfigNormalChipOutEP_8188E (
  IN  PADAPTER  pAdapter,
  IN  u8    NumOutPipe
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (pAdapter);
  
  switch (NumOutPipe) {
  case  3:
    pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_LQ | TX_SELE_NQ;
    pHalData->OutEpNumber = 3;
    break;
  case  2:
    pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_NQ;
    pHalData->OutEpNumber = 2;
    break;
  case  1:
    pHalData->OutEpQueueSel = TX_SELE_HQ;
    pHalData->OutEpNumber = 1;
    break;
  default:
    break;
    
  }
  DBG_871X ("%s OutEpQueueSel(0x%02x), OutEpNumber(%d) \n", __FUNCTION__, pHalData->OutEpQueueSel, pHalData->OutEpNumber );
  
}

static BOOLEAN HalUsbSetQueuePipeMapping8188EUsb (
  IN  PADAPTER  pAdapter,
  IN  u8    NumInPipe,
  IN  u8    NumOutPipe
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (pAdapter);
  BOOLEAN     result    = _FALSE;
  
  _ConfigNormalChipOutEP_8188E (pAdapter, NumOutPipe);
  
  if (1 == pHalData->OutEpNumber) {
    if (1 != NumInPipe) {
      return result;
    }
  }
  
  
  result = Hal_MappingOutPipe (pAdapter, NumOutPipe);
  
  return result;
  
}

void rtl8188eu_interface_configure (_adapter * padapter)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  struct dvobj_priv * pdvobjpriv = adapter_to_dvobj (padapter);
  
  if (pdvobjpriv->ishighspeed == _TRUE)
  {
    pHalData->UsbBulkOutSize = USB_HIGH_SPEED_BULK_SIZE;//512 bytes
  }
  else
  {
    pHalData->UsbBulkOutSize = USB_FULL_SPEED_BULK_SIZE;//64 bytes
  }
  
  pHalData->interfaceIndex = pdvobjpriv->InterfaceNumber;
  
  #ifdef CONFIG_USB_TX_AGGREGATION
  pHalData->UsbTxAggMode    = 1;
  pHalData->UsbTxAggDescNum = 0x6; 
  #endif
  
  #ifdef CONFIG_USB_RX_AGGREGATION
  pHalData->UsbRxAggMode    = USB_RX_AGG_DMA;// USB_RX_AGG_DMA;
  pHalData->UsbRxAggBlockCount  = 8;
  pHalData->UsbRxAggBlockTimeout  = 0x6;
  pHalData->UsbRxAggPageCount = 48;
  pHalData->UsbRxAggPageTimeout = 0x4;
  #endif
  
  HalUsbSetQueuePipeMapping8188EUsb (padapter,
                                     pdvobjpriv->RtNumInPipes, pdvobjpriv->RtNumOutPipes);
                                     
}

static u32 rtl8188eu_InitPowerOn (_adapter * padapter)
{
  u16 value16;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  if (_TRUE == pHalData->bMacPwrCtrlOn)
  { return _SUCCESS; }
  
  if (!HalPwrSeqCmdParsing (padapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK, Rtl8188E_NIC_PWR_ON_FLOW) )
  {
    DBG_871X (KERN_ERR "%s: run power on flow fail\n", __func__);
    return _FAIL;
  }
  
  rtw_write16 (padapter, REG_CR, 0x00);
  
  
  value16 = rtw_read16 (padapter, REG_CR);
  value16 |= (HCI_TXDMA_EN | HCI_RXDMA_EN | TXDMA_EN | RXDMA_EN
              | PROTOCOL_EN | SCHEDULE_EN | ENSEC | CALTMR_EN);
  
  rtw_write16 (padapter, REG_CR, value16);
  pHalData->bMacPwrCtrlOn = _TRUE;
  
  return _SUCCESS;
  
}


static void _dbg_dump_macreg (_adapter * padapter)
{
  u32 offset = 0;
  u32 val32 = 0;
  u32 index = 0 ;
  for (index = 0; index < 64; index++)
  {
    offset = index * 4;
    val32 = rtw_read32 (padapter, offset);
    DBG_8192C ("offset : 0x%02x ,val:0x%08x\n", offset, val32);
  }
}


static void _InitPABias (_adapter * padapter)
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
#ifdef CONFIG_BT_COEXIST
static void _InitBTCoexist (_adapter * padapter)
{
  HAL_DATA_TYPE  * pHalData = GET_HAL_DATA (padapter);
  struct btcoexist_priv * pbtpriv = & (pHalData->bt_coexist);
  u8 u1Tmp;
  
  if (pbtpriv->BT_Coexist && pbtpriv->BT_CoexistType == BT_CSR_BC4)
  {
  
    if (padapter->registrypriv.mp_mode == 0)
    {
      if (pbtpriv->BT_Ant_isolation)
      {
        rtw_write8 ( padapter, REG_GPIO_MUXCFG, 0xa0);
        DBG_8192C ("BT write 0x%x = 0x%x\n", REG_GPIO_MUXCFG, 0xa0);
      }
    }

    u1Tmp = rtw_read8 (padapter, 0x4fd) & BIT0;
    u1Tmp = u1Tmp |
            ( (pbtpriv->BT_Ant_isolation == 1) ? 0 : BIT1) |
            ( (pbtpriv->BT_Service == BT_SCO) ? 0 : BIT2);
    rtw_write8 ( padapter, 0x4fd, u1Tmp);
    DBG_8192C ("BT write 0x%x = 0x%x for non-isolation\n", 0x4fd, u1Tmp);
    
    
    rtw_write32 (padapter, REG_BT_COEX_TABLE + 4, 0xaaaa9aaa);
    DBG_8192C ("BT write 0x%x = 0x%x\n", REG_BT_COEX_TABLE + 4, 0xaaaa9aaa);
    
    rtw_write32 (padapter, REG_BT_COEX_TABLE + 8, 0xffbd0040);
    DBG_8192C ("BT write 0x%x = 0x%x\n", REG_BT_COEX_TABLE + 8, 0xffbd0040);
    
    rtw_write32 (padapter,  REG_BT_COEX_TABLE + 0xc, 0x40000010);
    DBG_8192C ("BT write 0x%x = 0x%x\n", REG_BT_COEX_TABLE + 0xc, 0x40000010);
    
    u1Tmp =  rtw_read8 (padapter, rOFDM0_TRxPathEnable);
    u1Tmp &= ~ (BIT1);
    rtw_write8 ( padapter, rOFDM0_TRxPathEnable, u1Tmp);
    DBG_8192C ("BT write 0xC04 = 0x%x\n", u1Tmp);
    
    u1Tmp = rtw_read8 (padapter, rOFDM1_TRxPathEnable);
    u1Tmp &= ~ (BIT1);
    rtw_write8 ( padapter, rOFDM1_TRxPathEnable, u1Tmp);
    DBG_8192C ("BT write 0xD04 = 0x%x\n", u1Tmp);
    
  }
}
#endif



static VOID
_SetMacID (
  IN  PADAPTER Adapter, u8 * MacID
)
{
  u32 i;
  for (i = 0 ; i < MAC_ADDR_LEN ; i++) {
    #ifdef  CONFIG_CONCURRENT_MODE
    if (Adapter->iface_type == IFACE_PORT1)
    { rtw_write32 (Adapter, REG_MACID1 + i, MacID[i]); }
    else
    #endif
      rtw_write32 (Adapter, REG_MACID + i, MacID[i]);
  }
}

static VOID
_SetBSSID (
  IN  PADAPTER Adapter, u8 * BSSID
)
{
  u32 i;
  for (i = 0 ; i < MAC_ADDR_LEN ; i++) {
    #ifdef  CONFIG_CONCURRENT_MODE
    if (Adapter->iface_type == IFACE_PORT1)
    { rtw_write32 (Adapter, REG_BSSID1 + i, BSSID[i]); }
    else
    #endif
      rtw_write32 (Adapter, REG_BSSID + i, BSSID[i]);
  }
}


static VOID
_InitInterrupt (
  IN  PADAPTER Adapter
)
{
  u32 imr, imr_ex;
  u8  usb_opt;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
  rtw_write32 (Adapter, REG_HISR_88E, 0xFFFFFFFF);
  imr = IMR_PSTIMEOUT_88E | IMR_TBDER_88E | IMR_CPWM_88E | IMR_CPWM2_88E ;
  rtw_write32 (Adapter, REG_HIMR_88E, imr);
  pHalData->IntrMask[0] = imr;
  
  imr_ex = IMR_TXERR_88E | IMR_RXERR_88E | IMR_TXFOVW_88E | IMR_RXFOVW_88E;
  rtw_write32 (Adapter, REG_HIMRE_88E, imr_ex);
  pHalData->IntrMask[1] = imr_ex;
  
  #ifdef CONFIG_SUPPORT_USB_INT
  usb_opt = rtw_read8 (Adapter, REG_USB_SPECIAL_OPTION);
  
  
  if (!adapter_to_dvobj (Adapter)->ishighspeed
    #ifdef CONFIG_USB_INTERRUPT_IN_PIPE
      || pHalData->RtIntInPipe == 0x05
    #endif
     )
  { usb_opt = usb_opt & (~INT_BULK_SEL); }
  else
  { usb_opt = usb_opt | (INT_BULK_SEL); }
  
  rtw_write8 (Adapter, REG_USB_SPECIAL_OPTION, usb_opt );
  
  #endif//CONFIG_SUPPORT_USB_INT
  
}


static VOID
_InitQueueReservedPage (
  IN  PADAPTER Adapter
)
{
  HAL_DATA_TYPE  * pHalData = GET_HAL_DATA (Adapter);
  struct registry_priv * pregistrypriv = &Adapter->registrypriv;
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
    rtw_write8 (Adapter, REG_RQPN_NPQ, value8);
    
    numPubQ = 0xA8 - numHQ - numLQ - numNQ;
    
    value32 = _HPQ (numHQ) | _LPQ (numLQ) | _PUBQ (numPubQ) | LD_RQPN;
    rtw_write32 (Adapter, REG_RQPN, value32);
  }
  else
  {
    rtw_write16 (Adapter, REG_RQPN_NPQ, 0x0000);
    rtw_write16 (Adapter, REG_RQPN_NPQ, 0x0d);
    rtw_write32 (Adapter, REG_RQPN, 0x808E000d);
  }
}

static VOID
_InitTxBufferBoundary (
  IN PADAPTER Adapter,
  IN u8 txpktbuf_bndy
)
{
  struct registry_priv * pregistrypriv = &Adapter->registrypriv;
  
  
  rtw_write8 (Adapter, REG_TXPKTBUF_BCNQ_BDNY, txpktbuf_bndy);
  rtw_write8 (Adapter, REG_TXPKTBUF_MGQ_BDNY, txpktbuf_bndy);
  rtw_write8 (Adapter, REG_TXPKTBUF_WMAC_LBK_BF_HD, txpktbuf_bndy);
  rtw_write8 (Adapter, REG_TRXFF_BNDY, txpktbuf_bndy);
  rtw_write8 (Adapter, REG_TDECTRL + 1, txpktbuf_bndy);
  
}

static VOID
_InitPageBoundary (
  IN  PADAPTER Adapter
)
{
  u16 rxff_bndy = MAX_RX_DMA_BUFFER_SIZE_88E-1;
  
  #if 0
  
  if (bSupportRemoteWakeUp)
  {
    Offset = MAX_RX_DMA_BUFFER_SIZE_88E+MAX_TX_REPORT_BUFFER_SIZE - MAX_SUPPORT_WOL_PATTERN_NUM (Adapter) * WKFMCAM_SIZE;
    Offset = Offset / 128;
    rxff_bndy = (Offset * 128) - 1;
  }
  else
  
  #endif
    rtw_write16 (Adapter, (REG_TRXFF_BNDY + 2), rxff_bndy);
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
  IN  PADAPTER Adapter
)
{
  struct registry_priv * pregistrypriv = &Adapter->registrypriv;
  u16     beQ, bkQ, viQ, voQ, mgtQ, hiQ;
  
  if (!pregistrypriv->wifi_spec ) {
    beQ   = QUEUE_LOW;
    bkQ     = QUEUE_LOW;
    viQ     = QUEUE_NORMAL;
    voQ     = QUEUE_HIGH;
    mgtQ  = QUEUE_HIGH;
    hiQ     = QUEUE_HIGH;
  }
  else {
    beQ   = QUEUE_LOW;
    bkQ     = QUEUE_NORMAL;
    viQ     = QUEUE_NORMAL;
    voQ     = QUEUE_HIGH;
    mgtQ  = QUEUE_HIGH;
    hiQ     = QUEUE_HIGH;
  }
  _InitNormalChipRegPriority (Adapter, beQ, bkQ, viQ, voQ, mgtQ, hiQ);
}

static VOID
_InitQueuePriority (
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



static VOID
_InitHardwareDropIncorrectBulkOut (
  IN  PADAPTER Adapter
)
{
  u32 value32 = rtw_read32 (Adapter, REG_TXDMA_OFFSET_CHK);
  value32 |= DROP_DATA_EN;
  rtw_write32 (Adapter, REG_TXDMA_OFFSET_CHK, value32);
}

static VOID
_InitNetworkType (
  IN  PADAPTER Adapter
)
{
  u32 value32;
  
  value32 = rtw_read32 (Adapter, REG_CR);
  value32 = (value32 & ~MASK_NETTYPE) | _NETTYPE (NT_LINK_AP);
  
  rtw_write32 (Adapter, REG_CR, value32);
}

static VOID
_InitTransferPageSize (
  IN  PADAPTER Adapter
)
{
  
  u8  value8;
  value8 = _PSRX (PBP_128) | _PSTX (PBP_128);
  rtw_write8 (Adapter, REG_PBP, value8);
}

static VOID
_InitDriverInfoSize (
  IN  PADAPTER  Adapter,
  IN  u8    drvInfoSize
)
{
  rtw_write8 (Adapter, REG_RX_DRVINFO_SZ, drvInfoSize);
}

static VOID
_InitWMACSetting (
  IN  PADAPTER Adapter
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
  pHalData->ReceiveConfig = RCR_APM | RCR_AM | RCR_AB | RCR_CBSSID_DATA | RCR_CBSSID_BCN | RCR_APP_ICV | RCR_AMF | RCR_HTC_LOC_CTRL | RCR_APP_MIC | RCR_APP_PHYSTS;
  
  #if (1 == RTL8188E_RX_PACKET_INCLUDE_CRC)
  pHalData->ReceiveConfig |= ACRC32;
  #endif
  
  rtw_write32 (Adapter, REG_RCR, pHalData->ReceiveConfig);
  
  rtw_write32 (Adapter, REG_MAR, 0xFFFFFFFF);
  rtw_write32 (Adapter, REG_MAR + 4, 0xFFFFFFFF);
  
  
  
  
  
  
}

static VOID
_InitAdaptiveCtrl (
  IN  PADAPTER Adapter
)
{
  u16 value16;
  u32 value32;
  
  value32 = rtw_read32 (Adapter, REG_RRSR);
  value32 &= ~RATE_BITMAP_ALL;
  value32 |= RATE_RRSR_CCK_ONLY_1M;
  rtw_write32 (Adapter, REG_RRSR, value32);
  
  
  value16 = _SPEC_SIFS_CCK (0x10) | _SPEC_SIFS_OFDM (0x10);
  rtw_write16 (Adapter, REG_SPEC_SIFS, value16);
  
  value16 = _LRL (0x30) | _SRL (0x30);
  rtw_write16 (Adapter, REG_RL, value16);
  
}

static VOID
_InitRateFallback (
  IN  PADAPTER Adapter
)
{
  rtw_write32 (Adapter, REG_DARFRC, 0x00000000);
  rtw_write32 (Adapter, REG_DARFRC + 4, 0x10080404);
  rtw_write32 (Adapter, REG_RARFRC, 0x04030201);
  rtw_write32 (Adapter, REG_RARFRC + 4, 0x08070605);
  
}


static VOID
_InitEDCA (
  IN  PADAPTER Adapter
)
{
  rtw_write16 (Adapter, REG_SPEC_SIFS, 0x100a);
  rtw_write16 (Adapter, REG_MAC_SPEC_SIFS, 0x100a);
  
  rtw_write16 (Adapter, REG_SIFS_CTX, 0x100a);
  
  rtw_write16 (Adapter, REG_SIFS_TRX, 0x100a);
  
  rtw_write32 (Adapter, REG_EDCA_BE_PARAM, 0x005EA42B);
  rtw_write32 (Adapter, REG_EDCA_BK_PARAM, 0x0000A44F);
  rtw_write32 (Adapter, REG_EDCA_VI_PARAM, 0x005EA324);
  rtw_write32 (Adapter, REG_EDCA_VO_PARAM, 0x002FA226);
}


static VOID
_InitBeaconMaxError (
  IN  PADAPTER  Adapter,
  IN  BOOLEAN   InfraMode
)
{

}


#ifdef CONFIG_LED
static void _InitHWLed (PADAPTER Adapter)
{
  struct led_priv * pledpriv = & (Adapter->ledpriv);
  
  if ( pledpriv->LedStrategy != HW_LED)
  { return; }
  

}
#endif

static VOID
_InitRDGSetting (
  IN  PADAPTER Adapter
)
{
  rtw_write8 (Adapter, REG_RD_CTRL, 0xFF);
  rtw_write16 (Adapter, REG_RD_NAV_NXT, 0x200);
  rtw_write8 (Adapter, REG_RD_RESP_PKT_TH, 0x05);
}

static VOID
_InitRxSetting (
  IN  PADAPTER Adapter
)
{
  rtw_write32 (Adapter, REG_MACID, 0x87654321);
  rtw_write32 (Adapter, 0x0700, 0x87654321);
}

static VOID
_InitRetryFunction (
  IN  PADAPTER Adapter
)
{
  u8  value8;
  
  value8 = rtw_read8 (Adapter, REG_FWHW_TXQ_CTRL);
  value8 |= EN_AMPDU_RTY_NEW;
  rtw_write8 (Adapter, REG_FWHW_TXQ_CTRL, value8);
  
  rtw_write8 (Adapter, REG_ACKTO, 0x40);
}

/*-----------------------------------------------------------------------------
 * Function:  usb_AggSettingTxUpdate()
 *
 * Overview:  Seperate TX/RX parameters update independent for TP detection and
 *      dynamic TX/RX aggreagtion parameters update.
 *
 * Input:     PADAPTER
 *
 * Output/Return: NONE
 *
 * Revised History:
 *  When    Who   Remark
 *  12/10/2010  MHC   Seperate to smaller function.
 *
 *---------------------------------------------------------------------------*/
static VOID
usb_AggSettingTxUpdate (
  IN  PADAPTER      Adapter
)
{
  #ifdef CONFIG_USB_TX_AGGREGATION
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  u32     value32;
  
  if (Adapter->registrypriv.wifi_spec)
  { pHalData->UsbTxAggMode = _FALSE; }
  
  if (pHalData->UsbTxAggMode) {
    value32 = rtw_read32 (Adapter, REG_TDECTRL);
    value32 = value32 & ~ (BLK_DESC_NUM_MASK << BLK_DESC_NUM_SHIFT);
    value32 |= ( (pHalData->UsbTxAggDescNum & BLK_DESC_NUM_MASK) << BLK_DESC_NUM_SHIFT);
    
    rtw_write32 (Adapter, REG_TDECTRL, value32);
  }
  
  #endif
}


/*-----------------------------------------------------------------------------
 * Function:  usb_AggSettingRxUpdate()
 *
 * Overview:  Seperate TX/RX parameters update independent for TP detection and
 *      dynamic TX/RX aggreagtion parameters update.
 *
 * Input:     PADAPTER
 *
 * Output/Return: NONE
 *
 * Revised History:
 *  When    Who   Remark
 *  12/10/2010  MHC   Seperate to smaller function.
 *
 *---------------------------------------------------------------------------*/
static VOID
usb_AggSettingRxUpdate (
  IN  PADAPTER      Adapter
)
{
  #ifdef CONFIG_USB_RX_AGGREGATION
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  u8      valueDMA;
  u8      valueUSB;
  
  valueDMA = rtw_read8 (Adapter, REG_TRXDMA_CTRL);
  valueUSB = rtw_read8 (Adapter, REG_USB_SPECIAL_OPTION);
  
  switch (pHalData->UsbRxAggMode)
  {
  case USB_RX_AGG_DMA:
    valueDMA |= RXDMA_AGG_EN;
    valueUSB &= ~USB_AGG_EN;
    break;
  case USB_RX_AGG_USB:
    valueDMA &= ~RXDMA_AGG_EN;
    valueUSB |= USB_AGG_EN;
    break;
  case USB_RX_AGG_MIX:
    valueDMA |= RXDMA_AGG_EN;
    valueUSB |= USB_AGG_EN;
    break;
  case USB_RX_AGG_DISABLE:
  default:
    valueDMA &= ~RXDMA_AGG_EN;
    valueUSB &= ~USB_AGG_EN;
    break;
  }
  
  rtw_write8 (Adapter, REG_TRXDMA_CTRL, valueDMA);
  rtw_write8 (Adapter, REG_USB_SPECIAL_OPTION, valueUSB);
  
  switch (pHalData->UsbRxAggMode)
  {
  case USB_RX_AGG_DMA:
    rtw_write8 (Adapter, REG_RXDMA_AGG_PG_TH, pHalData->UsbRxAggPageCount);
    rtw_write8 (Adapter, REG_RXDMA_AGG_PG_TH + 1, pHalData->UsbRxAggPageTimeout);
    break;
  case USB_RX_AGG_USB:
    rtw_write8 (Adapter, REG_USB_AGG_TH, pHalData->UsbRxAggBlockCount);
    rtw_write8 (Adapter, REG_USB_AGG_TO, pHalData->UsbRxAggBlockTimeout);
    break;
  case USB_RX_AGG_MIX:
    rtw_write8 (Adapter, REG_RXDMA_AGG_PG_TH, pHalData->UsbRxAggPageCount);
    rtw_write8 (Adapter, REG_RXDMA_AGG_PG_TH + 1, (pHalData->UsbRxAggPageTimeout & 0x1F) );
    
    rtw_write8 (Adapter, REG_USB_AGG_TH, pHalData->UsbRxAggBlockCount);
    rtw_write8 (Adapter, REG_USB_AGG_TO, pHalData->UsbRxAggBlockTimeout);
    
    break;
  case USB_RX_AGG_DISABLE:
  default:
    break;
  }
  
  switch (PBP_128)
  {
  case PBP_128:
    pHalData->HwRxPageSize = 128;
    break;
  case PBP_64:
    pHalData->HwRxPageSize = 64;
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
    break;
  }
  #endif
}

static VOID
InitUsbAggregationSetting (
  IN  PADAPTER Adapter
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
  usb_AggSettingTxUpdate (Adapter);
  
  usb_AggSettingRxUpdate (Adapter);
  
  pHalData->UsbRxHighSpeedMode = _FALSE;
}
VOID
HalRxAggr8188EUsb (
  IN  PADAPTER Adapter,
  IN BOOLEAN  Value
)
{
  #if 0//USB_RX_AGGREGATION_92C

  PMGNT_INFO    pMgntInfo = &Adapter->MgntInfo;
  u1Byte      valueDMATimeout;
  u1Byte      valueDMAPageCount;
  u1Byte      valueUSBTimeout;
  u1Byte      valueUSBBlockCount;
  
  if ( IS_WIRELESS_MODE_B (Adapter) || IS_WIRELESS_MODE_G (Adapter) || IS_WIRELESS_MODE_A (Adapter) || pMgntInfo->bWiFiConfg)
  {
    valueDMATimeout = 0x0f;
    valueDMAPageCount = 0x01;
    valueUSBTimeout = 0x0f;
    valueUSBBlockCount = 0x01;
    rtw_hal_set_hwreg (Adapter, HW_VAR_RX_AGGR_PGTO, (pu1Byte) &valueDMATimeout);
    rtw_hal_set_hwreg (Adapter, HW_VAR_RX_AGGR_PGTH, (pu1Byte) &valueDMAPageCount);
    rtw_hal_set_hwreg (Adapter, HW_VAR_RX_AGGR_USBTO, (pu1Byte) &valueUSBTimeout);
    rtw_hal_set_hwreg (Adapter, HW_VAR_RX_AGGR_USBTH, (pu1Byte) &valueUSBBlockCount);
  }
  else
  {
    rtw_hal_set_hwreg (Adapter, HW_VAR_RX_AGGR_USBTO, (pu1Byte) &pMgntInfo->RegRxAggBlockTimeout);
    rtw_hal_set_hwreg (Adapter, HW_VAR_RX_AGGR_USBTH, (pu1Byte) &pMgntInfo->RegRxAggBlockCount);
  }
  
  #endif
}

/*-----------------------------------------------------------------------------
 * Function:  USB_AggModeSwitch()
 *
 * Overview:  When RX traffic is more than 40M, we need to adjust some parameters to increase
 *      RX speed by increasing batch indication size. This will decrease TCP ACK speed, we
 *      need to monitor the influence of FTP/network share.
 *      For TX mode, we are still ubder investigation.
 *
 * Input:   PADAPTER
 *
 * Output:    NONE
 *
 * Return:    NONE
 *
 * Revised History:
 *  When    Who   Remark
 *  12/10/2010  MHC   Create Version 0.
 *
 *---------------------------------------------------------------------------*/
VOID
USB_AggModeSwitch (
  IN  PADAPTER      Adapter
)
{
  #if 0
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  PMGNT_INFO    pMgntInfo = & (Adapter->MgntInfo);
  
  if (pMgntInfo->bRegAggDMEnable == FALSE)
  {
    return;
  }
  
  
  if (pMgntInfo->LinkDetectInfo.bHigherBusyRxTraffic == TRUE &&
      pHalData->UsbRxHighSpeedMode == FALSE)
  {
    pHalData->UsbRxHighSpeedMode = TRUE;
    RT_TRACE (COMP_INIT, DBG_LOUD, ("UsbAggModeSwitchCheck to HIGH\n") );
  }
  else
    if (pMgntInfo->LinkDetectInfo.bHigherBusyRxTraffic == FALSE &&
        pHalData->UsbRxHighSpeedMode == TRUE)
    {
      pHalData->UsbRxHighSpeedMode = FALSE;
      RT_TRACE (COMP_INIT, DBG_LOUD, ("UsbAggModeSwitchCheck to LOW\n") );
    }
    else
    {
      return;
    }
    
    
  #if USB_RX_AGGREGATION_92C
  if (pHalData->UsbRxHighSpeedMode == TRUE)
  {
    #if (RT_PLATFORM == PLATFORM_LINUX)
    if (pMgntInfo->LinkDetectInfo.bTxBusyTraffic)
    {
      pHalData->RxAggBlockCount = 16;
      pHalData->RxAggBlockTimeout = 7;
    }
    else
    #endif
    {
      pHalData->RxAggBlockCount = 40;
      pHalData->RxAggBlockTimeout = 5;
    }
    pHalData->RxAggPageCount  = 72;
    pHalData->RxAggPageTimeout  = 6;
  }
  else
  {
    pHalData->RxAggBlockCount = pMgntInfo->RegRxAggBlockCount;
    pHalData->RxAggBlockTimeout = pMgntInfo->RegRxAggBlockTimeout;
    pHalData->RxAggPageCount    = pMgntInfo->RegRxAggPageCount;
    pHalData->RxAggPageTimeout  = pMgntInfo->RegRxAggPageTimeout;
  }
  
  if (pHalData->RxAggBlockCount > MAX_RX_AGG_BLKCNT)
  { pHalData->RxAggBlockCount = MAX_RX_AGG_BLKCNT; }
  #if (OS_WIN_FROM_VISTA(OS_VERSION)) || (RT_PLATFORM == PLATFORM_LINUX) 
  if (IS_WIRELESS_MODE_N_24G (Adapter) || IS_WIRELESS_MODE_N_5G (Adapter) )
  {
    usb_AggSettingRxUpdate_8188E (Adapter);
    
  }
  #endif
  
  #endif
  #endif
}

static VOID
_InitOperationMode (
  IN  PADAPTER      Adapter
)
{
  #if 0//gtest
  PHAL_DATA_TYPE  pHalData = GET_HAL_DATA (Adapter);
  u1Byte        regBwOpMode = 0;
  u4Byte        regRATR = 0, regRRSR = 0;
  
  
  switch (Adapter->RegWirelessMode)
  {
  case WIRELESS_MODE_B:
    regBwOpMode = BW_OPMODE_20MHZ;
    regRATR = RATE_ALL_CCK;
    regRRSR = RATE_ALL_CCK;
    break;
  case WIRELESS_MODE_A:
    regBwOpMode = BW_OPMODE_5G | BW_OPMODE_20MHZ;
    regRATR = RATE_ALL_OFDM_AG;
    regRRSR = RATE_ALL_OFDM_AG;
    break;
  case WIRELESS_MODE_G:
    regBwOpMode = BW_OPMODE_20MHZ;
    regRATR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
    regRRSR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
    break;
  case WIRELESS_MODE_AUTO:
    if (Adapter->bInHctTest)
    {
      regBwOpMode = BW_OPMODE_20MHZ;
      regRATR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
      regRRSR = RATE_ALL_CCK | RATE_ALL_OFDM_AG;
    }
    else
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
    regBwOpMode = BW_OPMODE_5G;
    regRATR = RATE_ALL_OFDM_AG | RATE_ALL_OFDM_1SS | RATE_ALL_OFDM_2SS;
    regRRSR = RATE_ALL_OFDM_AG;
    break;
    
  default:
    break;
  }
  
  PlatformEFIOWrite1Byte (Adapter, REG_BWOPMODE, regBwOpMode);
  #endif
}


static VOID
_InitBeaconParameters (
  IN  PADAPTER Adapter
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
  rtw_write16 (Adapter, REG_BCN_CTRL, 0x1010);
  
  rtw_write16 (Adapter, REG_TBTT_PROHIBIT, 0x6404);
  rtw_write8 (Adapter, REG_DRVERLYINT, DRIVER_EARLY_INT_TIME);
  rtw_write8 (Adapter, REG_BCNDMATIM, BCN_DMA_ATIME_INT_TIME);
  
  rtw_write16 (Adapter, REG_BCNTCFG, 0x660F);
  
  pHalData->RegBcnCtrlVal = rtw_read8 (Adapter, REG_BCN_CTRL);
  pHalData->RegTxPause = rtw_read8 (Adapter, REG_TXPAUSE);
  pHalData->RegFwHwTxQCtrl = rtw_read8 (Adapter, REG_FWHW_TXQ_CTRL + 2);
  pHalData->RegReg542 = rtw_read8 (Adapter, REG_TBTT_PROHIBIT + 2);
  pHalData->RegCR_1 = rtw_read8 (Adapter, REG_CR + 1);
}

static VOID
_InitRFType (
  IN  PADAPTER Adapter
)
{
  struct registry_priv  * pregpriv = &Adapter->registrypriv;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  BOOLEAN     is92CU    = IS_92C_SERIAL (pHalData->VersionID);
  
  #if DISABLE_BB_RF
  pHalData->rf_chip = RF_PSEUDO_11N;
  return;
  #endif
  
  pHalData->rf_chip = RF_6052;
  
  if (_FALSE == is92CU) {
    pHalData->rf_type = RF_1T1R;
    DBG_8192C ("Set RF Chip ID to RF_6052 and RF type to 1T1R.\n");
    return;
  }
  
  MSG_8192C ("Set RF Chip ID to RF_6052 and RF type to %d.\n", pHalData->rf_type);
  
}


static VOID
_BeaconFunctionEnable (
  IN  PADAPTER    Adapter,
  IN  BOOLEAN     Enable,
  IN  BOOLEAN     Linked
)
{
  rtw_write8 (Adapter, REG_BCN_CTRL, (BIT4 | BIT3 | BIT1) );
  
  rtw_write8 (Adapter, REG_RD_CTRL + 1, 0x6F);
}


static VOID _BBTurnOnBlock (
  IN  PADAPTER    Adapter
)
{
  #if (DISABLE_BB_RF)
  return;
  #endif
  
  PHY_SetBBReg (Adapter, rFPGA0_RFMOD, bCCKEn, 0x1);
  PHY_SetBBReg (Adapter, rFPGA0_RFMOD, bOFDMEn, 0x1);
}

static VOID _RfPowerSave (
  IN  PADAPTER    Adapter
)
{
  #if 0
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  PMGNT_INFO    pMgntInfo = & (Adapter->MgntInfo);
  u1Byte      eRFPath;
  
  #if (DISABLE_BB_RF)
  return;
  #endif
  
  if (pMgntInfo->RegRfOff == TRUE) {
    RT_TRACE ( (COMP_INIT | COMP_RF), DBG_LOUD, ("InitializeAdapter8192CUsb(): Turn off RF for RegRfOff.\n") );
    MgntActSet_RF_State (Adapter, eRfOff, RF_CHANGE_BY_SW);
    for (eRFPath = 0; eRFPath < pHalData->NumTotalRFPath; eRFPath++)
    { PHY_SetRFReg (Adapter, (RF_RADIO_PATH_E) eRFPath, 0x4, 0xC00, 0x0); }
  }
  else
    if (pMgntInfo->RfOffReason > RF_CHANGE_BY_PS) {
      RT_TRACE ( (COMP_INIT | COMP_RF), DBG_LOUD, ("InitializeAdapter8192CUsb(): Turn off RF for RfOffReason(%ld).\n", pMgntInfo->RfOffReason) );
      MgntActSet_RF_State (Adapter, eRfOff, pMgntInfo->RfOffReason);
    }
    else {
      pHalData->eRFPowerState = eRfOn;
      pMgntInfo->RfOffReason = 0;
      if (Adapter->bInSetPower || Adapter->bResetInProgress)
      { PlatformUsbEnableInPipes (Adapter); }
      RT_TRACE ( (COMP_INIT | COMP_RF), DBG_LOUD, ("InitializeAdapter8192CUsb(): RF is on.\n") );
    }
  #endif
}

enum {
  Antenna_Lfet = 1,
  Antenna_Right = 2,
};

static VOID
_InitAntenna_Selection (IN PADAPTER Adapter)
{

  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
  if (pHalData->AntDivCfg == 0)
  { return; }
  DBG_8192C ("==>  %s ....\n", __FUNCTION__);
  
  rtw_write32 (Adapter, REG_LEDCFG0, rtw_read32 (Adapter, REG_LEDCFG0) | BIT23);
  PHY_SetBBReg (Adapter, rFPGA0_XAB_RFParameter, BIT13, 0x01);
  
  if (PHY_QueryBBReg (Adapter, rFPGA0_XA_RFInterfaceOE, 0x300) == Antenna_A)
  { pHalData->CurAntenna = Antenna_A; }
  else
  { pHalData->CurAntenna = Antenna_B; }
  DBG_8192C ("%s,Cur_ant:(%x)%s\n", __FUNCTION__, pHalData->CurAntenna, (pHalData->CurAntenna == Antenna_A) ? "Antenna_A" : "Antenna_B");
  
  
}

static VOID
HalDetectSelectiveSuspendMode (
  IN PADAPTER       Adapter
)
{
  #if 0
  u8  tmpvalue;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  struct dvobj_priv * pdvobjpriv = adapter_to_dvobj (Adapter);
  
  
  EFUSE_ShadowRead (Adapter, 1, EEPROM_USB_OPTIONAL1, (u32 *) &tmpvalue);
  
  DBG_8192C ("HalDetectSelectiveSuspendMode(): SS ");
  if (tmpvalue & BIT1)
  {
    DBG_8192C ("Enable\n");
  }
  else
  {
    DBG_8192C ("Disable\n");
    pdvobjpriv->RegUsbSS = _FALSE;
  }
  
  if (pdvobjpriv->RegUsbSS && !SUPPORT_HW_RADIO_DETECT (pHalData) )
  {
    
    pdvobjpriv->RegUsbSS = _FALSE;
  }
  #endif
}
/*-----------------------------------------------------------------------------
 * Function:  HwSuspendModeEnable92Cu()
 *
 * Overview:  HW suspend mode switch.
 *
 * Input:   NONE
 *
 * Output:  NONE
 *
 * Return:  NONE
 *
 * Revised History:
 *  When    Who   Remark
 *  08/23/2010  MHC   HW suspend mode switch test..
 *---------------------------------------------------------------------------*/
static VOID
HwSuspendModeEnable_88eu (
  IN  PADAPTER  pAdapter,
  IN  u8      Type
)
{
  u16 reg = rtw_read16 (pAdapter, REG_GPIO_MUXCFG);
  
  {
    return;
  }
  
  if (Type == _FALSE)
  {
    reg |= BIT14;
    rtw_write16 (pAdapter, REG_GPIO_MUXCFG, reg);
    reg |= BIT12;
    rtw_write16 (pAdapter, REG_GPIO_MUXCFG, reg);
  }
  else
  {
    reg &= (~BIT12);
    rtw_write16 (pAdapter, REG_GPIO_MUXCFG, reg);
    reg &= (~BIT14);
    rtw_write16 (pAdapter, REG_GPIO_MUXCFG, reg);
  }
  
}
rt_rf_power_state RfOnOffDetect (IN  PADAPTER pAdapter )
{
  HAL_DATA_TYPE  * pHalData = GET_HAL_DATA (pAdapter);
  u8  val8;
  rt_rf_power_state rfpowerstate = rf_off;
  
  if (pAdapter->pwrctrlpriv.bHWPowerdown)
  {
    val8 = rtw_read8 (pAdapter, REG_HSISR);
    DBG_8192C ("pwrdown, 0x5c(BIT7)=%02x\n", val8);
    rfpowerstate = (val8 & BIT7) ? rf_off : rf_on;
  }
  else
  {
    rtw_write8 ( pAdapter, REG_MAC_PINMUX_CFG, rtw_read8 (pAdapter, REG_MAC_PINMUX_CFG) & ~ (BIT3) );
    val8 = rtw_read8 (pAdapter, REG_GPIO_IO_SEL);
    DBG_8192C ("GPIO_IN=%02x\n", val8);
    rfpowerstate = (val8 & BIT3) ? rf_on : rf_off;
  }
  return rfpowerstate;
}

void _ps_open_RF (_adapter * padapter);

u32 rtl8188eu_hal_init (PADAPTER Adapter)
{
  u8  value8 = 0;
  u16  value16;
  u8  txpktbuf_bndy;
  u32 status = _SUCCESS;
  HAL_DATA_TYPE  * pHalData = GET_HAL_DATA (Adapter);
  struct pwrctrl_priv  * pwrctrlpriv = &Adapter->pwrctrlpriv;
  struct registry_priv * pregistrypriv = &Adapter->registrypriv;
  
  rt_rf_power_state   eRfPowerStateToSet;
  #ifdef CONFIG_BT_COEXIST
  struct btcoexist_priv * pbtpriv = & (pHalData->bt_coexist);
  #endif
  
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
    #ifdef CONFIG_BT_COEXIST
    HAL_INIT_STAGES_BT_COEXIST,
    #endif
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
    #ifdef CONFIG_BT_COEXIST
    "HAL_INIT_STAGES_BT_COEXIST",
    #endif
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
  
  
  
  _func_enter_;
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_BEGIN);
  
  #ifdef CONFIG_WOWLAN
  
  Adapter->pwrctrlpriv.wowlan_wake_reason = rtw_read8 (Adapter, REG_WOWLAN_WAKE_REASON);
  DBG_8192C ("%s wowlan_wake_reason: 0x%02x\n",
             __func__, Adapter->pwrctrlpriv.wowlan_wake_reason);
             
  if (rtw_read8 (Adapter, REG_MCUFWDL) &BIT7) {
    /*&&
      (Adapter->pwrctrlpriv.wowlan_wake_reason & FWDecisionDisconnect)) {*/
    u8 reg_val = 0;
    DBG_8192C ("+Reset Entry+\n");
    rtw_write8 (Adapter, REG_MCUFWDL, 0x00);
    _8051Reset88E (Adapter);
    reg_val = rtw_read8 (Adapter, REG_SYS_FUNC_EN);
    reg_val &= ~ (BIT (0) | BIT (1) );
    rtw_write8 (Adapter, REG_SYS_FUNC_EN, reg_val);
    rtw_write8 (Adapter, REG_RF_CTRL, 0);
    rtw_write16 (Adapter, REG_CR, 0);
    reg_val = rtw_read8 (Adapter, REG_SYS_FUNC_EN + 1);
    reg_val &= ~ (BIT (4) | BIT (7) );
    rtw_write8 (Adapter, REG_SYS_FUNC_EN + 1, reg_val);
    reg_val = rtw_read8 (Adapter, REG_SYS_FUNC_EN + 1);
    reg_val |= BIT (4) | BIT (7);
    rtw_write8 (Adapter, REG_SYS_FUNC_EN + 1, reg_val);
    DBG_8192C ("-Reset Entry-\n");
  }
  #endif
  
  if (Adapter->pwrctrlpriv.bkeepfwalive)
  {
    _ps_open_RF (Adapter);
    
    if (pHalData->odmpriv.RFCalibrateInfo.bIQKInitialized) {
      PHY_IQCalibrate_8188E (Adapter, _TRUE);
    }
    else
    {
      PHY_IQCalibrate_8188E (Adapter, _FALSE);
      pHalData->odmpriv.RFCalibrateInfo.bIQKInitialized = _TRUE;
    }
    
    ODM_TXPowerTrackingCheck (&pHalData->odmpriv );
    PHY_LCCalibrate_8188E (Adapter);
    
    goto exit;
  }
  
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_INIT_PW_ON);
  status = rtl8188eu_InitPowerOn (Adapter);
  if (status == _FAIL) {
    RT_TRACE (_module_hci_hal_init_c_, _drv_err_, ("Failed to init power on!\n") );
    goto exit;
  }
  
  pHalData->CurrentChannel = 6;//default set to 6
  
  
  if (pwrctrlpriv->reg_rfoff == _TRUE) {
    pwrctrlpriv->rf_pwrstate = rf_off;
  }
  
  
  if (!pregistrypriv->wifi_spec) {
    txpktbuf_bndy = TX_PAGE_BOUNDARY_88E;
  }
  else {
    txpktbuf_bndy = WMM_NORMAL_TX_PAGE_BOUNDARY_88E;
  }
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_MISC01);
  _InitQueueReservedPage (Adapter);
  _InitQueuePriority (Adapter);
  _InitPageBoundary (Adapter);
  _InitTransferPageSize (Adapter);
  
  #ifdef CONFIG_IOL_IOREG_CFG
  _InitTxBufferBoundary (Adapter, 0);
  #endif
  
  
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_DOWNLOAD_FW);
  #if (MP_DRIVER == 1)
  if (Adapter->registrypriv.mp_mode == 1)
  {
    _InitRxSetting (Adapter);
  }
  #endif 
  {
    #if 0
    Adapter->bFWReady = _FALSE;
    pHalData->fw_ractrl = _FALSE;
    #else
    
    #ifdef CONFIG_WOWLAN
    status = rtl8188e_FirmwareDownload (Adapter, _FALSE);
    #else
    status = rtl8188e_FirmwareDownload (Adapter);
    #endif
    
    if (status != _SUCCESS) {
      DBG_871X ("%s: Download Firmware failed!!\n", __FUNCTION__);
      Adapter->bFWReady = _FALSE;
      pHalData->fw_ractrl = _FALSE;
      return status;
    }
    else {
      RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("Initializepadapter8192CSdio(): Download Firmware Success!!\n") );
      Adapter->bFWReady = _TRUE;
      pHalData->fw_ractrl = _FALSE;
    }
    #endif
  }
  
  
  rtl8188e_InitializeFirmwareVars (Adapter);
  
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_MAC);
  #if (HAL_MAC_ENABLE == 1)
  status = PHY_MACConfig8188E (Adapter);
  if (status == _FAIL)
  {
    DBG_871X (" ### Failed to init MAC ...... \n ");
    goto exit;
  }
  #endif
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_BB);
  #if (HAL_BB_ENABLE == 1)
  status = PHY_BBConfig8188E (Adapter);
  if (status == _FAIL)
  {
    DBG_871X (" ### Failed to init BB ...... \n ");
    goto exit;
  }
  #endif
  
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_RF);
  #if (HAL_RF_ENABLE == 1)
  status = PHY_RFConfig8188E (Adapter);
  if (status == _FAIL)
  {
    DBG_871X (" ### Failed to init RF ...... \n ");
    goto exit;
  }
  #endif
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_EFUSE_PATCH);
  #if defined(CONFIG_IOL_EFUSE_PATCH)
  status = rtl8188e_iol_efuse_patch (Adapter);
  if (status == _FAIL) {
    DBG_871X ("%s  rtl8188e_iol_efuse_patch failed \n", __FUNCTION__);
    goto exit;
  }
  #endif
  
  _InitTxBufferBoundary (Adapter, txpktbuf_bndy);
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_INIT_LLTT);
  status =  InitLLTTable (Adapter, txpktbuf_bndy);
  if (status == _FAIL) {
    RT_TRACE (_module_hci_hal_init_c_, _drv_err_, ("Failed to init LLT table\n") );
    goto exit;
  }
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_MISC02);
  _InitDriverInfoSize (Adapter, DRVINFO_SZ);
  
  _InitInterrupt (Adapter);
  hal_init_macaddr (Adapter);
  _InitNetworkType (Adapter);
  _InitWMACSetting (Adapter);
  _InitAdaptiveCtrl (Adapter);
  _InitEDCA (Adapter);
  _InitRetryFunction (Adapter);
  InitUsbAggregationSetting (Adapter);
  _InitOperationMode (Adapter);
  _InitBeaconParameters (Adapter);
  _InitBeaconMaxError (Adapter, _TRUE);
  
  value16 = rtw_read16 (Adapter, REG_CR);
  value16 |= (MACTXEN | MACRXEN);
  rtw_write8 (Adapter, REG_CR, value16);
  
  #if ENABLE_USB_DROP_INCORRECT_OUT
  _InitHardwareDropIncorrectBulkOut (Adapter);
  #endif
  
  if (pHalData->bRDGEnable) {
    _InitRDGSetting (Adapter);
  }
  
  #if (RATE_ADAPTIVE_SUPPORT==1)
  {
    value8 = rtw_read8 (Adapter, REG_TX_RPT_CTRL);
    rtw_write8 (Adapter,  REG_TX_RPT_CTRL, (value8 | BIT1 | BIT0) );
    rtw_write8 (Adapter,  REG_TX_RPT_CTRL + 1, 2);
    rtw_write16 (Adapter, REG_TX_RPT_TIME, 0xCdf0);
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
    
    value8 = rtw_read8 (Adapter, REG_EARLY_MODE_CONTROL);
    #if RTL8188E_EARLY_MODE_PKT_NUM_10 == 1
    value8 = value8 | 0x1f;
    #else
    value8 = value8 | 0xf;
    #endif
    rtw_write8 (Adapter, REG_EARLY_MODE_CONTROL, value8);
    
    rtw_write8 (Adapter, REG_EARLY_MODE_CONTROL + 3, 0x80);
    
    value8 = rtw_read8 (Adapter, REG_TCR + 1);
    value8 = value8 | 0x40;
    rtw_write8 (Adapter, REG_TCR + 1, value8);
  }
  else
  #endif
  {
    rtw_write8 (Adapter, REG_EARLY_MODE_CONTROL, 0);
  }
  
  #if defined(CONFIG_CONCURRENT_MODE) || defined(CONFIG_TX_MCAST2UNI)
  
  #ifdef CONFIG_CHECK_AC_LIFETIME
  rtw_write8 (Adapter, REG_LIFETIME_EN, 0x0F);
  #endif 
  
  #ifdef CONFIG_TX_MCAST2UNI
  rtw_write16 (Adapter, REG_PKT_VO_VI_LIFE_TIME, 0x0400);
  rtw_write16 (Adapter, REG_PKT_BE_BK_LIFE_TIME, 0x0400);
  #else
  rtw_write16 (Adapter, REG_PKT_VO_VI_LIFE_TIME, 0x3000);
  rtw_write16 (Adapter, REG_PKT_BE_BK_LIFE_TIME, 0x3000);
  #endif 
  #endif 
  
  
  #ifdef CONFIG_LED
  _InitHWLed (Adapter);
  #endif
  
  
  pHalData->RfRegChnlVal[0] = PHY_QueryRFReg (Adapter, (RF_RADIO_PATH_E) 0, RF_CHNLBW, bRFRegOffsetMask);
  pHalData->RfRegChnlVal[1] = PHY_QueryRFReg (Adapter, (RF_RADIO_PATH_E) 1, RF_CHNLBW, bRFRegOffsetMask);
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_TURN_ON_BLOCK);
  _BBTurnOnBlock (Adapter);
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_INIT_SECURITY);
  invalidate_cam_all (Adapter);
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_MISC11);
  PHY_SetTxPowerLevel8188E (Adapter, pHalData->CurrentChannel);
  

  _InitAntenna_Selection (Adapter);
  
  rtw_write32 (Adapter, REG_BAR_MODE_CTRL, 0x0201ffff);
  
  rtw_write8 (Adapter, REG_HWSEQ_CTRL, 0xFF);
  
  if (pregistrypriv->wifi_spec)
  { rtw_write16 (Adapter, REG_FAST_EDCA_CTRL , 0); }
  
  rtw_write8 (Adapter, 0x652, 0x0);
  
  HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_INIT_HAL_DM);
  rtl8188e_InitHalDm (Adapter);
  
  #if (MP_DRIVER == 1)
  if (Adapter->registrypriv.mp_mode == 1)
  {
    Adapter->mppriv.channel = pHalData->CurrentChannel;
    MPT_InitializeAdapter (Adapter, Adapter->mppriv.channel);
  }
  else
  #endif 
  {
    pwrctrlpriv->rf_pwrstate = rf_on;
    
    #if 0 
    RT_CLEAR_PS_LEVEL (pwrctrlpriv, RT_RF_OFF_LEVL_HALT_NIC);
    #if 1
    
    eRfPowerStateToSet = (rt_rf_power_state) RfOnOffDetect (Adapter);
    pwrctrlpriv->rfoff_reason |= eRfPowerStateToSet == rf_on ? RF_CHANGE_BY_INIT : RF_CHANGE_BY_HW;
    pwrctrlpriv->rfoff_reason |= (pwrctrlpriv->reg_rfoff) ? RF_CHANGE_BY_SW : 0;
    
    if (pwrctrlpriv->rfoff_reason & RF_CHANGE_BY_HW)
    { pwrctrlpriv->b_hw_radio_off = _TRUE; }
    
    DBG_8192C ("eRfPowerStateToSet=%d\n", eRfPowerStateToSet);
    
    if (pwrctrlpriv->reg_rfoff == _TRUE)
    {
      DBG_8192C ("InitializeAdapter8192CU(): Turn off RF for RegRfOff.\n");
      
    }
    else
      if (pwrctrlpriv->rfoff_reason > RF_CHANGE_BY_PS)
      {
        DBG_8192C (" Turn off RF for RfOffReason(%x) ----------\n", pwrctrlpriv->rfoff_reason);
        pwrctrlpriv->rf_pwrstate = rf_on;
      }
      else
      {
        if (pHalData->BoardType == BOARD_MINICARD /*&& (Adapter->MgntInfo.PowerSaveControl.bGpioRfSw)*/)
        {
          DBG_8192C ("InitializeAdapter8192CU(): RF=%d \n", eRfPowerStateToSet);
          if (eRfPowerStateToSet == rf_off)
          {
            pwrctrlpriv->b_hw_radio_off = _TRUE;
          }
          else
          {
            pwrctrlpriv->rf_pwrstate = rf_off;
            pwrctrlpriv->rfoff_reason = RF_CHANGE_BY_INIT;
            pwrctrlpriv->b_hw_radio_off = _FALSE;
          }
        }
        else
        {
          pwrctrlpriv->rf_pwrstate = rf_off;
          pwrctrlpriv->rfoff_reason = RF_CHANGE_BY_INIT;
        }
        
        pwrctrlpriv->rfoff_reason = 0;
        pwrctrlpriv->b_hw_radio_off = _FALSE;
        pwrctrlpriv->rf_pwrstate = rf_on;
        rtw_led_control (Adapter, LED_CTL_POWER_ON);
        
      }
      
    if (pHalData->pwrdown && eRfPowerStateToSet == rf_off)
    {
      rtw_write8 (Adapter, REG_RSV_CTRL, 0x0);
      
      rtw_write16 (Adapter, REG_APS_FSMCO, 0x8812);
    }
    #endif
    #endif
    
    
    rtw_write8 (Adapter,  REG_FWHW_TXQ_CTRL + 1, 0x0F);
    
    rtw_write8 (Adapter, REG_EARLY_MODE_CONTROL + 3, 0x01);
    
    rtw_write16 (Adapter, REG_TX_RPT_TIME, 0x3DF0);
    
    rtw_write16 (Adapter, REG_TXDMA_OFFSET_CHK, (rtw_read16 (Adapter, REG_TXDMA_OFFSET_CHK) | DROP_DATA_EN) );
    
    HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_IQK);
    if (pwrctrlpriv->rf_pwrstate == rf_on)
    {
      if (pHalData->odmpriv.RFCalibrateInfo.bIQKInitialized) {
        PHY_IQCalibrate_8188E (Adapter, _TRUE);
      }
      else
      {
        PHY_IQCalibrate_8188E (Adapter, _FALSE);
        pHalData->odmpriv.RFCalibrateInfo.bIQKInitialized = _TRUE;
      }
      
      HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_PW_TRACK);
      
      ODM_TXPowerTrackingCheck (&pHalData->odmpriv );
      
      
      HAL_INIT_PROFILE_TAG (HAL_INIT_STAGES_LCK);
      PHY_LCCalibrate_8188E (Adapter);
    }
  }
  
  rtw_write8 (Adapter, REG_USB_HRPWM, 0);
  
  #ifdef CONFIG_XMIT_ACK
  rtw_write32 (Adapter, REG_FWHW_TXQ_CTRL, rtw_read32 (Adapter, REG_FWHW_TXQ_CTRL) | BIT (12) );
  #endif
  
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
  
  
  _func_exit_;
  
  return status;
}

void _ps_open_RF (_adapter * padapter) {
}

void _ps_close_RF (_adapter * padapter) {
}


VOID
CardDisableRTL8188EU (
  IN  PADAPTER      Adapter
)
{
  u8  val8;
  u16 val16;
  u32 val32;
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
  RT_TRACE (COMP_INIT, DBG_LOUD, ("CardDisableRTL8188EU\n") );
  
  val8 = rtw_read8 (Adapter, REG_TX_RPT_CTRL);
  rtw_write8 (Adapter, REG_TX_RPT_CTRL, val8 & (~BIT1) );
  
  rtw_write8 (Adapter, REG_CR, 0x0);
  
  HalPwrSeqCmdParsing (Adapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK, Rtl8188E_NIC_LPS_ENTER_FLOW);
  
  
  
  val8 = rtw_read8 (Adapter, REG_MCUFWDL);
  if ( (val8 & RAM_DL_SEL) && Adapter->bFWReady)
  {
    
    val8 = rtw_read8 (Adapter, REG_SYS_FUNC_EN + 1);
    val8 &= ~BIT (2);
    rtw_write8 (Adapter, REG_SYS_FUNC_EN + 1, val8);
  }
  
  
  rtw_write8 (Adapter, REG_MCUFWDL, 0);
  
  val8 = rtw_read8 (Adapter, REG_32K_CTRL);
  rtw_write8 (Adapter, REG_32K_CTRL, val8 & (~BIT0) );
  
  HalPwrSeqCmdParsing (Adapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK, Rtl8188E_NIC_DISABLE_FLOW);
  
  val8 = rtw_read8 (Adapter, REG_RSV_CTRL + 1);
  rtw_write8 (Adapter, REG_RSV_CTRL + 1, (val8 & (~BIT3) ) );
  val8 = rtw_read8 (Adapter, REG_RSV_CTRL + 1);
  rtw_write8 (Adapter, REG_RSV_CTRL + 1, val8 | BIT3);
  
  #if 0
  rtw_write8 (Adapter, REG_RSV_CTRL, 0x0e);
  #endif
  #if 1
  val8 = rtw_read8 (Adapter, GPIO_IN);
  rtw_write8 (Adapter, GPIO_OUT, val8);
  rtw_write8 (Adapter, GPIO_IO_SEL, 0xFF);
  
  val8 = rtw_read8 (Adapter, REG_GPIO_IO_SEL);
  rtw_write8 (Adapter, REG_GPIO_IO_SEL, (val8 << 4) );
  val8 = rtw_read8 (Adapter, REG_GPIO_IO_SEL + 1);
  rtw_write8 (Adapter, REG_GPIO_IO_SEL + 1, val8 | 0x0F);
  rtw_write32 (Adapter, REG_BB_PAD_CTRL, 0x00080808);
  #endif
  pHalData->bMacPwrCtrlOn = _FALSE;
  Adapter->bFWReady = _FALSE;
}
static void rtl8192cu_hw_power_down (_adapter * padapter)
{
  
  rtw_write8 (padapter, REG_RSV_CTRL, 0x0);
  rtw_write16 (padapter, REG_APS_FSMCO, 0x8812);
}

u32 rtl8188eu_hal_deinit (PADAPTER Adapter)
{

  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  DBG_8192C ("==> %s \n", __FUNCTION__);
  
  #ifdef CONFIG_SUPPORT_USB_INT
  rtw_write32 (Adapter, REG_HIMR_88E, IMR_DISABLED_88E);
  rtw_write32 (Adapter, REG_HIMRE_88E, IMR_DISABLED_88E);
  #endif
  
  #ifdef SUPPORT_HW_RFOFF_DETECTED
  DBG_8192C ("bkeepfwalive(%x)\n", Adapter->pwrctrlpriv.bkeepfwalive);
  if (Adapter->pwrctrlpriv.bkeepfwalive)
  {
    _ps_close_RF (Adapter);
    if ( (Adapter->pwrctrlpriv.bHWPwrPindetect) && (Adapter->pwrctrlpriv.bHWPowerdown) )
    { rtl8192cu_hw_power_down (Adapter); }
  }
  else
  #endif
  {
    if (Adapter->hw_init_completed == _TRUE) {
      CardDisableRTL8188EU (Adapter);
      
      if ( (Adapter->pwrctrlpriv.bHWPwrPindetect ) && (Adapter->pwrctrlpriv.bHWPowerdown) )
      { rtl8192cu_hw_power_down (Adapter); }
      
    }
  }
  return _SUCCESS;
}


unsigned int rtl8188eu_inirp_init (PADAPTER Adapter)
{
  u8 i;
  struct recv_buf * precvbuf;
  uint  status;
  struct dvobj_priv * pdev = adapter_to_dvobj (Adapter);
  struct intf_hdl * pintfhdl = &Adapter->iopriv.intf;
  struct recv_priv * precvpriv = & (Adapter->recvpriv);
  u32 (*_read_port) (struct intf_hdl * pintfhdl, u32 addr, u32 cnt, u8 * pmem);
  #ifdef CONFIG_USB_INTERRUPT_IN_PIPE
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  u32 (*_read_interrupt) (struct intf_hdl * pintfhdl, u32 addr);
  #endif
  
  _func_enter_;
  
  _read_port = pintfhdl->io_ops._read_port;
  
  status = _SUCCESS;
  
  RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("===> usb_inirp_init \n") );
  
  precvpriv->ff_hwaddr = RECV_BULK_IN_ADDR;
  
  precvbuf = (struct recv_buf *) precvpriv->precv_buf;
  for (i = 0; i < NR_RECVBUFF; i++)
  {
    if (_read_port (pintfhdl, precvpriv->ff_hwaddr, 0, (unsigned char *) precvbuf) == _FALSE )
    {
      RT_TRACE (_module_hci_hal_init_c_, _drv_err_, ("usb_rx_init: usb_read_port error \n") );
      status = _FAIL;
      goto exit;
    }
    
    precvbuf++;
    precvpriv->free_recv_buf_queue_cnt--;
  }
  
  #ifdef CONFIG_USB_INTERRUPT_IN_PIPE
  if (pHalData->RtIntInPipe != 0x05)
  {
    status = _FAIL;
    DBG_871X ("%s =>Warning !! Have not USB Int-IN pipe,  pHalData->RtIntInPipe(%d)!!!\n", __FUNCTION__, pHalData->RtIntInPipe);
    goto exit;
  }
  _read_interrupt = pintfhdl->io_ops._read_interrupt;
  if (_read_interrupt (pintfhdl, RECV_INT_IN_ADDR) == _FALSE )
  {
    RT_TRACE (_module_hci_hal_init_c_, _drv_err_, ("usb_rx_init: usb_read_interrupt error \n") );
    status = _FAIL;
  }
  #endif
  
exit:

  RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("<=== usb_inirp_init \n") );
  
  _func_exit_;
  
  return status;
  
}

unsigned int rtl8188eu_inirp_deinit (PADAPTER Adapter)
{
  RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("\n ===> usb_rx_deinit \n") );
  
  rtw_read_port_cancel (Adapter);
  
  RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("\n <=== usb_rx_deinit \n") );
  
  return _SUCCESS;
}



#if 0
static VOID
_ReadPowerValueFromPROM (
  IN  PTxPowerInfo  pwrInfo,
  IN  u8   *  PROMContent,
  IN  BOOLEAN     AutoLoadFail
)
{
  u32 rfPath, eeAddr, group;
  
  _rtw_memset (pwrInfo, 0, sizeof (TxPowerInfo) );
  
  if (AutoLoadFail) {
    for (group = 0 ; group < CHANNEL_GROUP_MAX ; group++) {
      for (rfPath = 0 ; rfPath < RF_PATH_MAX ; rfPath++) {
        pwrInfo->CCKIndex[rfPath][group]    = EEPROM_Default_TxPowerLevel;
        pwrInfo->HT40_1SIndex[rfPath][group]  = EEPROM_Default_TxPowerLevel;
        pwrInfo->HT40_2SIndexDiff[rfPath][group] = EEPROM_Default_HT40_2SDiff;
        pwrInfo->HT20IndexDiff[rfPath][group] = EEPROM_Default_HT20_Diff;
        pwrInfo->OFDMIndexDiff[rfPath][group] = EEPROM_Default_LegacyHTTxPowerDiff;
        pwrInfo->HT40MaxOffset[rfPath][group] = EEPROM_Default_HT40_PwrMaxOffset;
        pwrInfo->HT20MaxOffset[rfPath][group] = EEPROM_Default_HT20_PwrMaxOffset;
      }
    }
    
    pwrInfo->TSSI_A = EEPROM_Default_TSSI;
    pwrInfo->TSSI_B = EEPROM_Default_TSSI;
    
    return;
  }
  
  for (rfPath = 0 ; rfPath < RF_PATH_MAX ; rfPath++) {
    for (group = 0 ; group < CHANNEL_GROUP_MAX ; group++) {
      eeAddr = EEPROM_CCK_TX_PWR_INX + (rfPath * 3) + group;
      pwrInfo->CCKIndex[rfPath][group] = PROMContent[eeAddr];
      
      eeAddr = EEPROM_HT40_1S_TX_PWR_INX + (rfPath * 3) + group;
      pwrInfo->HT40_1SIndex[rfPath][group] = PROMContent[eeAddr];
    }
  }
  
  for (group = 0 ; group < CHANNEL_GROUP_MAX ; group++) {
    for (rfPath = 0 ; rfPath < RF_PATH_MAX ; rfPath++) {
      pwrInfo->HT40_2SIndexDiff[rfPath][group] =
        (PROMContent[EEPROM_HT40_2S_TX_PWR_INX_DIFF + group] >> (rfPath * 4) ) & 0xF;
        
      #if 1
      pwrInfo->HT20IndexDiff[rfPath][group] =
        (PROMContent[EEPROM_HT20_TX_PWR_INX_DIFF + group] >> (rfPath * 4) ) & 0xF;
      if (pwrInfo->HT20IndexDiff[rfPath][group] & BIT3)
      { pwrInfo->HT20IndexDiff[rfPath][group] |= 0xF0; }
      #else
      pwrInfo->HT20IndexDiff[rfPath][group] =
        (PROMContent[EEPROM_HT20_TX_PWR_INX_DIFF + group] >> (rfPath * 4) ) & 0xF;
      #endif
      
      pwrInfo->OFDMIndexDiff[rfPath][group] =
        (PROMContent[EEPROM_OFDM_TX_PWR_INX_DIFF + group] >> (rfPath * 4) ) & 0xF;
        
      pwrInfo->HT40MaxOffset[rfPath][group] =
        (PROMContent[EEPROM_HT40_MAX_PWR_OFFSET + group] >> (rfPath * 4) ) & 0xF;
        
      pwrInfo->HT20MaxOffset[rfPath][group] =
        (PROMContent[EEPROM_HT20_MAX_PWR_OFFSET + group] >> (rfPath * 4) ) & 0xF;
    }
  }
  
  pwrInfo->TSSI_A = PROMContent[EEPROM_TSSI_A];
  pwrInfo->TSSI_B = PROMContent[EEPROM_TSSI_B];
  
}


static u32
_GetChannelGroup (
  IN  u32 channel
)
{
  
  if (channel < 3) {
    return 0;
  }
  else
    if (channel < 9) {
      return 1;
    }
    
  return 2;      
}


static VOID
ReadTxPowerInfo (
  IN  PADAPTER    Adapter,
  IN  u8   *  PROMContent,
  IN  BOOLEAN     AutoLoadFail
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  TxPowerInfo   pwrInfo;
  u32     rfPath, ch, group;
  u8      pwr, diff;
  
  _ReadPowerValueFromPROM (&pwrInfo, PROMContent, AutoLoadFail);
  
  if (!AutoLoadFail)
  { pHalData->bTXPowerDataReadFromEEPORM = _TRUE; }
  
  for (rfPath = 0 ; rfPath < RF_PATH_MAX ; rfPath++) {
    for (ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++) {
      group = _GetChannelGroup (ch);
      
      pHalData->TxPwrLevelCck[rfPath][ch]   = pwrInfo.CCKIndex[rfPath][group];
      pHalData->TxPwrLevelHT40_1S[rfPath][ch] = pwrInfo.HT40_1SIndex[rfPath][group];
      
      pHalData->TxPwrHt20Diff[rfPath][ch]   = pwrInfo.HT20IndexDiff[rfPath][group];
      pHalData->TxPwrLegacyHtDiff[rfPath][ch] = pwrInfo.OFDMIndexDiff[rfPath][group];
      pHalData->PwrGroupHT20[rfPath][ch]    = pwrInfo.HT20MaxOffset[rfPath][group];
      pHalData->PwrGroupHT40[rfPath][ch]    = pwrInfo.HT40MaxOffset[rfPath][group];
      
      pwr   = pwrInfo.HT40_1SIndex[rfPath][group];
      diff  = pwrInfo.HT40_2SIndexDiff[rfPath][group];
      
      pHalData->TxPwrLevelHT40_2S[rfPath][ch]  = (pwr > diff) ? (pwr - diff) : 0;
    }
  }
  
  #if 0
  
  for (rfPath = 0 ; rfPath < RF_PATH_MAX ; rfPath++) {
    for (ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++) {
      RTPRINT (FINIT, INIT_TxPower,
               ("RF(%d)-Ch(%d) [CCK / HT40_1S / HT40_2S] = [0x%x / 0x%x / 0x%x]\n",
                rfPath, ch, pHalData->TxPwrLevelCck[rfPath][ch],
                pHalData->TxPwrLevelHT40_1S[rfPath][ch],
                pHalData->TxPwrLevelHT40_2S[rfPath][ch]) );
                
    }
  }
  
  for (ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++) {
    RTPRINT (FINIT, INIT_TxPower, ("RF-A Ht20 to HT40 Diff[%d] = 0x%x\n", ch, pHalData->TxPwrHt20Diff[RF_PATH_A][ch]) );
  }
  
  for (ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++) {
    RTPRINT (FINIT, INIT_TxPower, ("RF-A Legacy to Ht40 Diff[%d] = 0x%x\n", ch, pHalData->TxPwrLegacyHtDiff[RF_PATH_A][ch]) );
  }
  
  for (ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++) {
    RTPRINT (FINIT, INIT_TxPower, ("RF-B Ht20 to HT40 Diff[%d] = 0x%x\n", ch, pHalData->TxPwrHt20Diff[RF_PATH_B][ch]) );
  }
  
  for (ch = 0 ; ch < CHANNEL_MAX_NUMBER ; ch++) {
    RTPRINT (FINIT, INIT_TxPower, ("RF-B Legacy to HT40 Diff[%d] = 0x%x\n", ch, pHalData->TxPwrLegacyHtDiff[RF_PATH_B][ch]) );
  }
  
  #endif
  if (!AutoLoadFail)
  {
    pHalData->EEPROMRegulatory = (PROMContent[RF_OPTION1] & 0x7);
  }
  else
  {
    pHalData->EEPROMRegulatory = 0;
  }
  DBG_8192C ("EEPROMRegulatory = 0x%x\n", pHalData->EEPROMRegulatory);
  
}
#endif

static void
_ReadIDs (
  IN  PADAPTER  Adapter,
  IN  u8  * PROMContent,
  IN  BOOLEAN   AutoloadFail
)
{
  #if 0
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
  if (_FALSE == AutoloadFail) {
    pHalData->EEPROMVID = le16_to_cpu ( * (u16 *) &PROMContent[EEPROM_VID]);
    pHalData->EEPROMPID = le16_to_cpu ( * (u16 *) &PROMContent[EEPROM_PID]);
    
    pHalData->EEPROMCustomerID = * (u8 *) &PROMContent[EEPROM_CUSTOMER_ID];
    pHalData->EEPROMSubCustomerID = * (u8 *) &PROMContent[EEPROM_SUBCUSTOMER_ID];
    
  }
  else {
    pHalData->EEPROMVID  = EEPROM_Default_VID;
    pHalData->EEPROMPID  = EEPROM_Default_PID;
    
    pHalData->EEPROMCustomerID  = EEPROM_Default_CustomerID;
    pHalData->EEPROMSubCustomerID = EEPROM_Default_SubCustomerID;
    
  }
  
  if ( (pHalData->EEPROMVID == 0x103C) && (pHalData->EEPROMVID == 0x1629) )
  { pHalData->CustomerID = RT_CID_819x_HP; }
  
  switch (pHalData->EEPROMCustomerID)
  {
  case EEPROM_CID_DEFAULT:
    if ( (pHalData->EEPROMVID == 0x2001) && (pHalData->EEPROMPID == 0x3308) )
    { pHalData->CustomerID = RT_CID_DLINK; }
    else
      if ( (pHalData->EEPROMVID == 0x2001) && (pHalData->EEPROMPID == 0x3309) )
      { pHalData->CustomerID = RT_CID_DLINK; }
      else
        if ( (pHalData->EEPROMVID == 0x2001) && (pHalData->EEPROMPID == 0x330a) )
        { pHalData->CustomerID = RT_CID_DLINK; }
    break;
  case EEPROM_CID_WHQL:
    /*
          Adapter->bInHctTest = TRUE;
    
          pMgntInfo->bSupportTurboMode = FALSE;
          pMgntInfo->bAutoTurboBy8186 = FALSE;
    
          pMgntInfo->PowerSaveControl.bInactivePs = FALSE;
          pMgntInfo->PowerSaveControl.bIPSModeBackup = FALSE;
          pMgntInfo->PowerSaveControl.bLeisurePs = FALSE;
    
          pMgntInfo->keepAliveLevel = 0;
    
          Adapter->bUnloadDriverwhenS3S4 = FALSE;
    */
    break;
  default:
    pHalData->CustomerID = RT_CID_DEFAULT;
    break;
    
  }
  
  MSG_8192C ("EEPROMVID = 0x%04x\n", pHalData->EEPROMVID);
  MSG_8192C ("EEPROMPID = 0x%04x\n", pHalData->EEPROMPID);
  MSG_8192C ("EEPROMCustomerID : 0x%02x\n", pHalData->EEPROMCustomerID);
  MSG_8192C ("EEPROMSubCustomerID: 0x%02x\n", pHalData->EEPROMSubCustomerID);
  
  MSG_8192C ("RT_CustomerID: 0x%02x\n", pHalData->CustomerID);
  #endif
}


static VOID
_ReadMACAddress (
  IN  PADAPTER  Adapter,
  IN  u8  * PROMContent,
  IN  BOOLEAN   AutoloadFail
)
{
  #if 0

  EEPROM_EFUSE_PRIV * pEEPROM = GET_EEPROM_EFUSE_PRIV (Adapter);
  
  if (_FALSE == AutoloadFail) {
    _rtw_memcpy (pEEPROM->mac_addr, &PROMContent[EEPROM_MAC_ADDR], ETH_ALEN);
  }
  else {
    u8 sMacAddr[MAC_ADDR_LEN] = {0x00, 0xE0, 0x4C, 0x81, 0x92, 0x00};
    _rtw_memcpy (pEEPROM->mac_addr, sMacAddr, ETH_ALEN);
  }
  DBG_8192C ("%s MAC Address from EFUSE = "MAC_FMT"\n", __FUNCTION__, MAC_ARG (pEEPROM->mac_addr) );
  #endif
}

static VOID
_ReadBoardType (
  IN  PADAPTER  Adapter,
  IN  u8  * PROMContent,
  IN  BOOLEAN   AutoloadFail
)
{

}


static VOID
_ReadLEDSetting (
  IN  PADAPTER  Adapter,
  IN  u8  * PROMContent,
  IN  BOOLEAN   AutoloadFail
)
{
  struct led_priv * pledpriv = & (Adapter->ledpriv);
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  #ifdef CONFIG_SW_LED
  pledpriv->bRegUseLed = _TRUE;
  
  switch (pHalData->CustomerID)
  {
  default:
    pledpriv->LedStrategy = SW_LED_MODE1;
    break;
  }
  pHalData->bLedOpenDrain = _TRUE;// Support Open-drain arrangement for controlling the LED. Added by Roger, 2009.10.16.
  #else
  pledpriv->LedStrategy = HW_LED;
  #endif
}

static VOID
_ReadThermalMeter (
  IN  PADAPTER  Adapter,
  IN  u8  * PROMContent,
  IN  BOOLEAN   AutoloadFail
)
{
  #if 0
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  struct dm_priv * pdmpriv = &pHalData->dmpriv;
  u8  tempval;
  
  if (!AutoloadFail)
  { tempval = PROMContent[EEPROM_THERMAL_METER]; }
  else
  { tempval = EEPROM_Default_ThermalMeter; }
  
  pHalData->EEPROMThermalMeter = (tempval & 0x1f);
  
  if (pHalData->EEPROMThermalMeter == 0x1f || AutoloadFail)
  { pdmpriv->bAPKThermalMeterIgnore = _TRUE; }
  
  #if 0
  if (pHalData->EEPROMThermalMeter < 0x06 || pHalData->EEPROMThermalMeter > 0x1c)
  { pHalData->EEPROMThermalMeter = 0x12; }
  #endif
  
  pdmpriv->ThermalMeter[0] = pHalData->EEPROMThermalMeter;
  
  #endif
}

static VOID
_ReadRFSetting (
  IN  PADAPTER  Adapter,
  IN  u8  * PROMContent,
  IN  BOOLEAN   AutoloadFail
)
{
}

static void
_ReadPROMVersion (
  IN  PADAPTER  Adapter,
  IN  u8  * PROMContent,
  IN  BOOLEAN   AutoloadFail
)
{
  #if 0
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
  if (AutoloadFail) {
    pHalData->EEPROMVersion = EEPROM_Default_Version;
  }
  else {
    pHalData->EEPROMVersion = * (u8 *) &PROMContent[EEPROM_VERSION];
  }
  #endif
}

static VOID
readAntennaDiversity (
  IN  PADAPTER  pAdapter,
  IN  u8   *   hwinfo,
  IN  BOOLEAN   AutoLoadFail
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (pAdapter);
  struct registry_priv * registry_par = &pAdapter->registrypriv;
  
  pHalData->AntDivCfg = registry_par->antdiv_cfg ; 
  #if 0
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (pAdapter);
  struct registry_priv * registry_par = &pAdapter->registrypriv;
  
  if (!AutoLoadFail)
  {
    if (registry_par->antdiv_cfg == 2)
    { pHalData->AntDivCfg = (hwinfo[EEPROM_RF_OPT1] & 0x18) >> 3; }
    else
    { pHalData->AntDivCfg = registry_par->antdiv_cfg ; } 
    
    DBG_8192C ("### AntDivCfg(%x)\n", pHalData->AntDivCfg);
    
  }
  else
  {
    pHalData->AntDivCfg = 0;
  }
  #endif
}

static VOID
hal_InitPGData (
  IN  PADAPTER  pAdapter,
  IN  OUT u8  *  PROMContent
)
{
  #if 0
  EEPROM_EFUSE_PRIV * pEEPROM = GET_EEPROM_EFUSE_PRIV (pAdapter);
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (pAdapter);
  u32 i;
  u16 value16;
  
  if (_FALSE == pEEPROM->bautoload_fail_flag)
  {
    if (_TRUE == pEEPROM->EepromOrEfuse)
    {
      for (i = 0; i < HWSET_MAX_SIZE_88E; i += 2)
      {
      }
    }
    else
    {
      EFUSE_ShadowMapUpdate (pAdapter, EFUSE_WIFI, _FALSE);
      _rtw_memcpy ( (void *) PROMContent, (void *) pEEPROM->efuse_eeprom_data, HWSET_MAX_SIZE_88E);
    }
  }
  else
  {
    pEEPROM->bautoload_fail_flag = _TRUE;
    if (_FALSE == pEEPROM->EepromOrEfuse)
    { EFUSE_ShadowMapUpdate (pAdapter, EFUSE_WIFI, _FALSE); }
  }
  #endif
}
static void
Hal_EfuseParsePIDVID_8188EU (
  IN  PADAPTER    pAdapter,
  IN  u8    *   hwinfo,
  IN  BOOLEAN     AutoLoadFail
)
{

  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (pAdapter);
  
  if ( !AutoLoadFail )
  {
    pHalData->EEPROMVID = EF2Byte ( * (u16 *) &hwinfo[EEPROM_VID_88EU] );
    pHalData->EEPROMPID = EF2Byte ( * (u16 *) &hwinfo[EEPROM_PID_88EU] );
    
    pHalData->EEPROMCustomerID = * (u8 *) &hwinfo[EEPROM_CUSTOMERID_88E];
    pHalData->EEPROMSubCustomerID = EEPROM_Default_SubCustomerID;
    
  }
  else
  {
    pHalData->EEPROMVID       = EEPROM_Default_VID;
    pHalData->EEPROMPID       = EEPROM_Default_PID;
    
    pHalData->EEPROMCustomerID    = EEPROM_Default_CustomerID;
    pHalData->EEPROMSubCustomerID = EEPROM_Default_SubCustomerID;
    
  }
  
  DBG_871X ("VID = 0x%04X, PID = 0x%04X\n", pHalData->EEPROMVID, pHalData->EEPROMPID);
  DBG_871X ("Customer ID: 0x%02X, SubCustomer ID: 0x%02X\n", pHalData->EEPROMCustomerID, pHalData->EEPROMSubCustomerID);
}

static void
Hal_EfuseParseMACAddr_8188EU (
  IN  PADAPTER    padapter,
  IN  u8   *  hwinfo,
  IN  BOOLEAN     AutoLoadFail
)
{
  u16     i, usValue;
  u8      sMacAddr[6] = {0x00, 0xE0, 0x4C, 0x81, 0x88, 0x02};
  EEPROM_EFUSE_PRIV * pEEPROM = GET_EEPROM_EFUSE_PRIV (padapter);
  
  if (AutoLoadFail)
  {
    for (i = 0; i < 6; i++)
    { pEEPROM->mac_addr[i] = sMacAddr[i]; }
  }
  else
  {
    _rtw_memcpy (pEEPROM->mac_addr, &hwinfo[EEPROM_MAC_ADDR_88EU], ETH_ALEN);
    
  }

  RT_TRACE (_module_hci_hal_init_c_, _drv_notice_,
            ("Hal_EfuseParseMACAddr_8188EU: Permanent Address = %02x-%02x-%02x-%02x-%02x-%02x\n",
             pEEPROM->mac_addr[0], pEEPROM->mac_addr[1],
             pEEPROM->mac_addr[2], pEEPROM->mac_addr[3],
             pEEPROM->mac_addr[4], pEEPROM->mac_addr[5]) );
}


static void
Hal_CustomizeByCustomerID_8188EU (
  IN  PADAPTER    padapter
)
{
  #if 0
  PMGNT_INFO    pMgntInfo = & (padapter->MgntInfo);
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  
  if ( (pHalData->EEPROMVID == 0x103C) && (pHalData->EEPROMVID == 0x1629) )
  { pMgntInfo->CustomerID = RT_CID_819x_HP; }
  
  switch (pHalData->EEPROMCustomerID)
  {
  case EEPROM_CID_DEFAULT:
    if ( (pHalData->EEPROMVID == 0x2001) && (pHalData->EEPROMPID == 0x3308) )
    { pMgntInfo->CustomerID = RT_CID_DLINK; }
    else
      if ( (pHalData->EEPROMVID == 0x2001) && (pHalData->EEPROMPID == 0x3309) )
      { pMgntInfo->CustomerID = RT_CID_DLINK; }
      else
        if ( (pHalData->EEPROMVID == 0x2001) && (pHalData->EEPROMPID == 0x330a) )
        { pMgntInfo->CustomerID = RT_CID_DLINK; }
    break;
  case EEPROM_CID_WHQL:
    padapter->bInHctTest = TRUE;
    
    pMgntInfo->bSupportTurboMode = FALSE;
    pMgntInfo->bAutoTurboBy8186 = FALSE;
    
    pMgntInfo->PowerSaveControl.bInactivePs = FALSE;
    pMgntInfo->PowerSaveControl.bIPSModeBackup = FALSE;
    pMgntInfo->PowerSaveControl.bLeisurePs = FALSE;
    pMgntInfo->PowerSaveControl.bLeisurePsModeBackup = FALSE;
    pMgntInfo->keepAliveLevel = 0;
    
    padapter->bUnloadDriverwhenS3S4 = FALSE;
    break;
  default:
    pMgntInfo->CustomerID = RT_CID_DEFAULT;
    break;
    
  }
  
  RT_TRACE (_module_hci_hal_init_c_, _drv_info_, ("Mgnt Customer ID: 0x%02x\n", pMgntInfo->CustomerID) );
  
  hal_CustomizedBehavior_8723U (padapter);
  #endif
}

static void _ReadPSSetting (IN PADAPTER Adapter, IN u8 * PROMContent, IN u8  AutoloadFail)
{
  #if 0
  if (AutoloadFail) {
    Adapter->pwrctrlpriv.bHWPowerdown = _FALSE;
    Adapter->pwrctrlpriv.bSupportRemoteWakeup = _FALSE;
  }
  else  {
    Adapter->pwrctrlpriv.bHWPwrPindetect = Adapter->registrypriv.hwpwrp_detect;
    
    
    
    if (Adapter->registrypriv.hwpdn_mode == 2)
    { Adapter->pwrctrlpriv.bHWPowerdown = (PROMContent[EEPROM_RF_OPT3] & BIT4); }
    else
    { Adapter->pwrctrlpriv.bHWPowerdown = Adapter->registrypriv.hwpdn_mode; }
    
    Adapter->pwrctrlpriv.bSupportRemoteWakeup = (PROMContent[EEPROM_TEST_USB_OPT] & BIT1) ? _TRUE : _FALSE;
    
    
    DBG_8192C ("%s...bHWPwrPindetect(%x)-bHWPowerdown(%x) ,bSupportRemoteWakeup(%x)\n", __FUNCTION__,
               Adapter->pwrctrlpriv.bHWPwrPindetect, Adapter->pwrctrlpriv.bHWPowerdown , Adapter->pwrctrlpriv.bSupportRemoteWakeup);
               
    DBG_8192C ("### PS params=>  power_mgnt(%x),usbss_enable(%x) ###\n", Adapter->registrypriv.power_mgnt, Adapter->registrypriv.usbss_enable);
    
  }
  #endif
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
Hal_ReadMACAddrFromFile_8188EU (
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
readAdapterInfo_8188EU (
  IN  PADAPTER  padapter
)
{
  #if 1
  EEPROM_EFUSE_PRIV * pEEPROM = GET_EEPROM_EFUSE_PRIV (padapter);
  
  /* parse the eeprom/efuse content */
  Hal_EfuseParseIDCode88E (padapter, pEEPROM->efuse_eeprom_data);
  Hal_EfuseParsePIDVID_8188EU (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  #ifdef CONFIG_EFUSE_CONFIG_FILE
  Hal_ReadMACAddrFromFile_8188EU (padapter);
  #else
  Hal_EfuseParseMACAddr_8188EU (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  #endif
  
  Hal_ReadPowerSavingMode88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_ReadTxPowerInfo88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_EfuseParseEEPROMVer88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  rtl8188e_EfuseParseChnlPlan (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_EfuseParseXtal_8188E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_EfuseParseCustomerID88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_ReadAntennaDiversity88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_EfuseParseBoardType88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  Hal_ReadThermalMeter_88E (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  
  Hal_InitChannelPlan (padapter);
  #if defined(CONFIG_WOWLAN) && defined(CONFIG_SDIO_HCI)
  Hal_DetectWoWMode (padapter);
  #endif
  Hal_CustomizeByCustomerID_8188EU (padapter);
  
  _ReadLEDSetting (padapter, pEEPROM->efuse_eeprom_data, pEEPROM->bautoload_fail_flag);
  
  #else
  
  #ifdef CONFIG_INTEL_PROXIM
  /* for intel proximity */
  if (pHalData->rf_type == RF_1T1R) {
    Adapter->proximity.proxim_support = _TRUE;
  }
  else
    if (pHalData->rf_type == RF_2T2R) {
      if ( (pHalData->EEPROMPID == 0x8186) &&
           (pHalData->EEPROMVID == 0x0bda) )
      { Adapter->proximity.proxim_support = _TRUE; }
    }
    else {
      Adapter->proximity.proxim_support = _FALSE;
    }
  #endif
  #endif
}

static void _ReadPROMContent (
  IN PADAPTER     Adapter
)
{
  EEPROM_EFUSE_PRIV * pEEPROM = GET_EEPROM_EFUSE_PRIV (Adapter);
  u8      eeValue;
  
  /* check system boot selection */
  eeValue = rtw_read8 (Adapter, REG_9346CR);
  pEEPROM->EepromOrEfuse    = (eeValue & BOOT_FROM_EEPROM) ? _TRUE : _FALSE;
  pEEPROM->bautoload_fail_flag  = (eeValue & EEPROM_EN) ? _FALSE : _TRUE;
  
  
  DBG_8192C ("Boot from %s, Autoload %s !\n", (pEEPROM->EepromOrEfuse ? "EEPROM" : "EFUSE"),
             (pEEPROM->bautoload_fail_flag ? "Fail" : "OK") );
             
  #ifdef CONFIG_EFUSE_CONFIG_FILE
  Hal_readPGDataFromConfigFile (Adapter);
  #else
  Hal_InitPGData88E (Adapter);
  #endif 
  readAdapterInfo_8188EU (Adapter);
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

static int _ReadAdapterInfo8188EU (PADAPTER  Adapter)
{
  u32 start = rtw_get_current_time();
  
  MSG_8192C ("====> %s\n", __FUNCTION__);
  
  
  
  _ReadRFType (Adapter);
  _ReadPROMContent (Adapter);
  
  
  MSG_8192C ("<==== %s in %d ms\n", __FUNCTION__, rtw_get_passing_time_ms (start) );
  
  return _SUCCESS;
}


static void ReadAdapterInfo8188EU (PADAPTER Adapter)
{
  Adapter->EepromAddressSize = GetEEPROMSize8188E (Adapter);
  
  _ReadAdapterInfo8188EU (Adapter);
}


#define GPIO_DEBUG_PORT_NUM 0
static void rtl8192cu_trigger_gpio_0 (_adapter * padapter)
{
  #ifdef CONFIG_USB_SUPPORT_ASYNC_VDN_REQ
  u32 gpioctrl;
  DBG_8192C ("==> trigger_gpio_0...\n");
  rtw_write16_async (padapter, REG_GPIO_PIN_CTRL, 0);
  rtw_write8_async (padapter, REG_GPIO_PIN_CTRL + 2, 0xFF);
  gpioctrl = (BIT (GPIO_DEBUG_PORT_NUM) << 24 ) | (BIT (GPIO_DEBUG_PORT_NUM) << 16);
  rtw_write32_async (padapter, REG_GPIO_PIN_CTRL, gpioctrl);
  gpioctrl |= (BIT (GPIO_DEBUG_PORT_NUM) << 8);
  rtw_write32_async (padapter, REG_GPIO_PIN_CTRL, gpioctrl);
  DBG_8192C ("<=== trigger_gpio_0...\n");
  #endif
}

static void ResumeTxBeacon (_adapter * padapter)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  
  
  rtw_write8 (padapter, REG_FWHW_TXQ_CTRL + 2, (pHalData->RegFwHwTxQCtrl) | BIT6);
  pHalData->RegFwHwTxQCtrl |= BIT6;
  rtw_write8 (padapter, REG_TBTT_PROHIBIT + 1, 0xff);
  pHalData->RegReg542 |= BIT0;
  rtw_write8 (padapter, REG_TBTT_PROHIBIT + 2, pHalData->RegReg542);
}
void UpdateInterruptMask8188EU (PADAPTER padapter, u8 bHIMR0 , u32 AddMSR, u32 RemoveMSR)
{
  HAL_DATA_TYPE * pHalData;
  
  u32 * himr;
  pHalData = GET_HAL_DATA (padapter);
  
  if (bHIMR0)
  { himr = & (pHalData->IntrMask[0]); }
  else
  { himr = & (pHalData->IntrMask[1]); }
  
  if (AddMSR)
  { *himr |= AddMSR; }
  
  if (RemoveMSR)
  { *himr &= (~RemoveMSR); }
  
  if (bHIMR0)
  { rtw_write32 (padapter, REG_HIMR_88E, *himr); }
  else
  { rtw_write32 (padapter, REG_HIMRE_88E, *himr); }
  
}

static void StopTxBeacon (_adapter * padapter)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (padapter);
  
  
  rtw_write8 (padapter, REG_FWHW_TXQ_CTRL + 2, (pHalData->RegFwHwTxQCtrl) & (~BIT6) );
  pHalData->RegFwHwTxQCtrl &= (~BIT6);
  rtw_write8 (padapter, REG_TBTT_PROHIBIT + 1, 0x64);
  pHalData->RegReg542 &= ~ (BIT0);
  rtw_write8 (padapter, REG_TBTT_PROHIBIT + 2, pHalData->RegReg542);
  
  
}


static void hw_var_set_opmode (PADAPTER Adapter, u8 variable, u8 * val)
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
        UpdateInterruptMask8188EU (Adapter, _TRUE, 0, IMR_BCNDMAINT0_88E);
        #endif
        
        #ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
        UpdateInterruptMask8188EU (Adapter, _TRUE , 0, (IMR_TBDER_88E | IMR_TBDOK_88E) );
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
          UpdateInterruptMask8188EU (Adapter, _TRUE , IMR_BCNDMAINT0_88E, 0);
          #endif//CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
          
          #ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
          UpdateInterruptMask8188EU (Adapter, _TRUE , (IMR_TBDER_88E | IMR_TBDOK_88E), 0);
          #endif//CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
          
          #endif
          
          ResumeTxBeacon (Adapter);
          
          rtw_write8 (Adapter, REG_BCN_CTRL_1, 0x12);
          
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
        UpdateInterruptMask8188EU (Adapter, _TRUE, 0, IMR_BCNDMAINT0_88E);
        #endif//CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
        
        #ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
        UpdateInterruptMask8188EU (Adapter, _TRUE , 0, (IMR_TBDER_88E | IMR_TBDOK_88E) );
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
          UpdateInterruptMask8188EU (Adapter, _TRUE , IMR_BCNDMAINT0_88E, 0);
          #endif//CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
          
          #ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
          UpdateInterruptMask8188EU (Adapter, _TRUE , (IMR_TBDER_88E | IMR_TBDOK_88E), 0);
          #endif//CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
          
          #endif
          
          
          ResumeTxBeacon (Adapter);
          
          rtw_write8 (Adapter, REG_BCN_CTRL, 0x12);
          
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
          
          #if defined(CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR)
          rtw_write8 (Adapter, REG_BCN_CTRL, (DIS_TSF_UDT0_NORMAL_CHIP | EN_BCN_FUNCTION | EN_TXBCN_RPT | BIT (1) ) );
          #else
          rtw_write8 (Adapter, REG_BCN_CTRL, (DIS_TSF_UDT0_NORMAL_CHIP | EN_BCN_FUNCTION | BIT (1) ) );
          #endif
          
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
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  
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

void SetHwReg8188EU (PADAPTER Adapter, u8 variable, u8 * val)
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
    
      if ( ( (pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) || ( (pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) )
      {
        StopTxBeacon (Adapter);
      }
    
      rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) & (~BIT (3) ) );
    
      rtw_write32 (Adapter, REG_TSFTR, tsf);
      rtw_write32 (Adapter, REG_TSFTR + 4, tsf >> 32);
    
      rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) | BIT (3) );
    
    
      if ( ( (pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) || ( (pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) )
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
    
      if ( (pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE)
      { rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | RCR_CBSSID_BCN); }
      else
      {
        if (Adapter->in_cta_test)
        {
          u32 v = rtw_read32 (Adapter, REG_RCR);
          v &= ~ (RCR_CBSSID_DATA | RCR_CBSSID_BCN );
          rtw_write32 (Adapter, REG_RCR, v);
        }
        else
        {
          rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | RCR_CBSSID_BCN);
        }
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
  case HW_VAR_ACM_CTRL:
    {
      u8  acm_ctrl = * ( (u8 *) val);
      u8  AcmCtrl = rtw_read8 ( Adapter, REG_ACMHWCTRL);
      
      if (acm_ctrl > 1)
      { AcmCtrl = AcmCtrl | 0x1; }
      
      if (acm_ctrl & BIT (3) )
      { AcmCtrl |= AcmHw_VoqEn; }
      else
      { AcmCtrl &= (~AcmHw_VoqEn); }
      
      if (acm_ctrl & BIT (2) )
      { AcmCtrl |= AcmHw_ViqEn; }
      else
      { AcmCtrl &= (~AcmHw_ViqEn); }
      
      if (acm_ctrl & BIT (1) )
      { AcmCtrl |= AcmHw_BeqEn; }
      else
      { AcmCtrl &= (~AcmHw_BeqEn); }
      
      DBG_871X ("[HW_VAR_ACM_CTRL] Write 0x%X\n", AcmCtrl );
      rtw_write8 (Adapter, REG_ACMHWCTRL, AcmCtrl );
    }
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
    #ifdef CONFIG_USB_RX_AGGREGATION
    {
      u8  threshold = * ( (u8 *) val);
      if ( threshold == 0)
      {
        threshold = pHalData->UsbRxAggPageCount;
      }
      rtw_write8 (Adapter, REG_RXDMA_AGG_PG_TH, threshold);
    }
    #endif
    break;
  case HW_VAR_SET_RPWM:
    #ifdef CONFIG_LPS_LCLK
    {
      u8  ps_state = * ( (u8 *) val);
      ps_state = ps_state & 0xC1;
      rtw_write8 (Adapter, REG_USB_HRPWM, ps_state);
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
    #ifdef CONFIG_TDLS
  case HW_VAR_TDLS_WRCR:
    rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) & (~RCR_CBSSID_DATA ) );
    break;
  case HW_VAR_TDLS_INIT_CH_SEN:
    {
      rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) & (~ RCR_CBSSID_DATA ) & (~RCR_CBSSID_BCN ) );
      rtw_write16 (Adapter, REG_RXFLTMAP2, 0xffff);
      
      rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) | BIT (4) );
    }
    break;
  case HW_VAR_TDLS_DONE_CH_SEN:
    {
      rtw_write8 (Adapter, REG_BCN_CTRL, rtw_read8 (Adapter, REG_BCN_CTRL) & (~ BIT (4) ) );
      rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | (RCR_CBSSID_BCN ) );
    }
    break;
  case HW_VAR_TDLS_RS_RCR:
    rtw_write32 (Adapter, REG_RCR, rtw_read32 (Adapter, REG_RCR) | (RCR_CBSSID_DATA) );
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
    rtl8192cu_trigger_gpio_0 (Adapter);
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
      
      if (pwrpriv->bkeepfwalive != _TRUE)
      {
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
    
  case HW_VAR_APFM_ON_MAC:
    pHalData->bMacPwrCtrlOn = *val;
    DBG_871X ("%s: bMacPwrCtrlOn=%d\n", __func__, pHalData->bMacPwrCtrlOn);
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
            DBG_871X_LEVEL (_drv_always_, "RX_DMA_IDLE is not true\n");
          }
        }
        while (trycnt--);
        if (trycnt == 0)
        { DBG_871X_LEVEL (_drv_always_, "Stop RX DMA failed...... \n"); }
        
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
                        
        /* Invoid SE0 reset signal during suspending*/
        rtw_write8 (Adapter, REG_RSV_CTRL, 0x20);
        rtw_write8 (Adapter, REG_RSV_CTRL, 0x60);
        
        break;
      case WOWLAN_DISABLE:
        DBG_871X_LEVEL (_drv_always_, "WOWLAN_DISABLE\n");
        trycnt = 10;
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
        
        rtw_msleep_os (2);
        if (! (Adapter->pwrctrlpriv.wowlan_wake_reason & FWDecisionDisconnect) )
        { rtl8188e_set_FwJoinBssReport_cmd (Adapter, 1); }
        break;
      default:
        break;
      }
    }
    break;
    #endif
    
    
    #if (RATE_ADAPTIVE_SUPPORT == 1)
  case HW_VAR_TX_RPT_MAX_MACID:
    {
      u8 maxMacid = *val;
      DBG_871X ("### MacID(%d),Set Max Tx RPT MID(%d)\n", maxMacid, maxMacid + 1);
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

void GetHwReg8188EU (PADAPTER Adapter, u8 variable, u8 * val)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  DM_ODM_T  *  podmpriv = &pHalData->odmpriv;
  _func_enter_;
  
  switch (variable)
  {
  case HW_VAR_BASIC_RATE:
    * ( (u16 *) (val) ) = pHalData->BasicRateSet;
  case HW_VAR_TXPAUSE:
    val[0] = rtw_read8 (Adapter, REG_TXPAUSE);
    break;
  case HW_VAR_BCN_VALID:
    val[0] = (BIT0 & rtw_read8 (Adapter, REG_TDECTRL + 2) ) ? _TRUE : _FALSE;
    break;
  case HW_VAR_DM_FLAG:
    val[0] = podmpriv->SupportAbility;
    break;
  case HW_VAR_RF_TYPE:
    val[0] = pHalData->rf_type;
    break;
  case HW_VAR_FWLPS_RF_ON:
    {
      if (Adapter->pwrctrlpriv.rf_pwrstate == rf_off)
      {
        val[0] = _TRUE;
      }
      else
      {
        u32 valRCR;
        valRCR = rtw_read32 (Adapter, REG_RCR);
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
    * ( (u16 *) (val) ) = pHalData->EfuseUsedBytes;
    break;
  case HW_VAR_APFM_ON_MAC:
    *val = pHalData->bMacPwrCtrlOn;
    break;
  case HW_VAR_CHK_HI_QUEUE_EMPTY:
    *val = ( (rtw_read32 (Adapter, REG_HGQ_INFORMATION) & 0x0000ff00) == 0) ? _TRUE : _FALSE;
    break;
    
  case HW_VAR_READ_LLT_TAB:
    {
      Read_LLT_Tab (Adapter);
    }
    break;
  default:
    break;
  }
  
  _func_exit_;
}

u8
GetHalDefVar8188EUsb (
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
    #if 1
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
    #else
    if (check_fwstate (&Adapter->mlmepriv, WIFI_STATION_STATE) == _TRUE) {
      * ( (int *) pValue) = pHalData->dmpriv.UndecoratedSmoothedPWDB;
    }
    else {
    
    }
    #endif
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
  case HAL_DEF_DRVINFO_SZ:
    * ( ( u32 *) pValue) = DRVINFO_SZ;
    break;
  case HAL_DEF_MAX_RECVBUF_SZ:
    * ( ( u32 *) pValue) = MAX_RECVBUF_SZ;
    break;
  case HAL_DEF_RX_PACKET_OFFSET:
    * ( ( u32 *) pValue) = RXDESC_SIZE + DRVINFO_SZ;
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
    #endif//(POWER_TRAINING_ACTIVE==1)
    break;
    
  case HW_VAR_MAX_RX_AMPDU_FACTOR:
    * ( ( u32 *) pValue) = MAX_AMPDU_FACTOR_64K;
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
  case HW_DEF_ODM_DBG_FLAG:
    {
      u8Byte  DebugComponents = * ( (u32 *) pValue);
      PDM_ODM_T pDM_Odm = & (pHalData->odmpriv);
      printk ("pDM_Odm->DebugComponents = 0x%llx \n", pDM_Odm->DebugComponents );
    }
    break;
    
  case HAL_DEF_DBG_DUMP_RXPKT:
    * ( ( u8 *) pValue) = pHalData->bDumpRxPkt;
    break;
  case HAL_DEF_DBG_DUMP_TXPKT:
    * ( ( u8 *) pValue) = pHalData->bDumpTxPkt;
    break;
    
  default:
    bResult = _FAIL;
    break;
  }
  
  return bResult;
}




u8
SetHalDefVar8188EUsb (
  IN  PADAPTER        Adapter,
  IN  HAL_DEF_VARIABLE    eVariable,
  IN  PVOID         pValue
)
{
  HAL_DATA_TYPE * pHalData = GET_HAL_DATA (Adapter);
  u8      bResult = _SUCCESS;
  
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
    bResult = _FAIL;
    break;
  }
  
  return bResult;
}
/*
u32  _update_92cu_basic_rate(_adapter *padapter, unsigned int mask)
{
  HAL_DATA_TYPE   *pHalData = GET_HAL_DATA(padapter);
#ifdef CONFIG_BT_COEXIST
  struct btcoexist_priv *pbtpriv = &(pHalData->bt_coexist);
#endif
  unsigned int BrateCfg = 0;

#ifdef CONFIG_BT_COEXIST
  if( (pbtpriv->BT_Coexist) &&  (pbtpriv->BT_CoexistType == BT_CSR_BC4) )
  {
    BrateCfg = mask  & 0x151;
  }
  else
#endif
  {
      BrateCfg = mask  & 0x15F;
  }

  BrateCfg |= 0x01;

  return BrateCfg;
}
*/
void _update_response_rate (_adapter * padapter, unsigned int mask)
{
  u8  RateIndex = 0;
  rtw_write8 (padapter, REG_RRSR, mask & 0xff);
  rtw_write8 (padapter, REG_RRSR + 1, (mask >> 8) & 0xff);
  
  while (mask > 0x1)
  {
    mask = (mask >> 1);
    RateIndex++;
  }
  rtw_write8 (padapter, REG_INIRTS_RATE_SEL, RateIndex);
}

void UpdateHalRAMask8188EUsb (PADAPTER padapter, u32 mac_id, u8 rssi_level)
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
    printk ("%s => mac_id:%d, networkType:0x%02x, mask:0x%08x\n\t ==> rssi_level:%d, rate_bitmap:0x%08x\n",
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
    mask |= ( (raid << 28) & 0xf0000000);
    
    /*
    *(pu4Byte)&RateMask=EF4Byte((ratr_bitmap&0x0fffffff) | (ratr_index<<28));
    RateMask[4] = macId | (bShortGI?0x20:0x00) | 0x80;
    */
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


void SetBeaconRelatedRegisters8188EUsb (PADAPTER padapter)
{
  u32 value32;
  struct mlme_ext_priv * pmlmeext = & (padapter->mlmeextpriv);
  struct mlme_ext_info * pmlmeinfo = & (pmlmeext->mlmext_info);
  u32 bcn_ctrl_reg      = REG_BCN_CTRL;
  
  
  #ifdef CONFIG_CONCURRENT_MODE
  if (padapter->iface_type == IFACE_PORT1) {
    bcn_ctrl_reg = REG_BCN_CTRL_1;
  }
  #endif
  rtw_write16 (padapter, REG_BCN_INTERVAL, pmlmeinfo->bcn_interval);
  rtw_write8 (padapter, REG_ATIMWND, 0x02);
  
  _InitBeaconParameters (padapter);
  
  rtw_write8 (padapter, REG_SLOT, 0x09);
  
  value32 = rtw_read32 (padapter, REG_TCR);
  value32 &= ~TSFRST;
  rtw_write32 (padapter,  REG_TCR, value32);
  
  value32 |= TSFRST;
  rtw_write32 (padapter, REG_TCR, value32);
  
  rtw_write8 (padapter,  REG_RXTSF_OFFSET_CCK, 0x50);
  rtw_write8 (padapter, REG_RXTSF_OFFSET_OFDM, 0x50);
  
  _BeaconFunctionEnable (padapter, _TRUE, _TRUE);
  
  ResumeTxBeacon (padapter);
  
  
  
  
  rtw_write8 (padapter, bcn_ctrl_reg, rtw_read8 (padapter, bcn_ctrl_reg) | BIT (1) );
  
}

static void rtl8188eu_init_default_value (_adapter * padapter)
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
}

static u8 rtl8188eu_ps_func (PADAPTER Adapter, HAL_INTF_PS_FUNC efunc_id, u8 * val)
{
  u8 bResult = _TRUE;
  switch (efunc_id) {
  
    #if defined(CONFIG_AUTOSUSPEND) && defined(SUPPORT_HW_RFOFF_DETECTED)
  case HAL_USB_SELECT_SUSPEND:
    {
      u8 bfwpoll = * ( ( u8 *) val);
    }
    break;
    #endif
    
  default:
    break;
  }
  return bResult;
}

void rtl8188eu_set_hal_ops (_adapter * padapter)
{
  struct hal_ops * pHalFunc = &padapter->HalFunc;
  
  _func_enter_;
  
  padapter->HalData = rtw_zmalloc (sizeof (HAL_DATA_TYPE) );
  if (padapter->HalData == NULL) {
    DBG_8192C ("cant not alloc memory for HAL DATA \n");
  }
  padapter->hal_data_sz = sizeof (HAL_DATA_TYPE);
  
  pHalFunc->hal_power_on = rtl8188eu_InitPowerOn;
  pHalFunc->hal_init = &rtl8188eu_hal_init;
  pHalFunc->hal_deinit = &rtl8188eu_hal_deinit;
  
  
  pHalFunc->inirp_init = &rtl8188eu_inirp_init;
  pHalFunc->inirp_deinit = &rtl8188eu_inirp_deinit;
  
  pHalFunc->init_xmit_priv = &rtl8188eu_init_xmit_priv;
  pHalFunc->free_xmit_priv = &rtl8188eu_free_xmit_priv;
  
  pHalFunc->init_recv_priv = &rtl8188eu_init_recv_priv;
  pHalFunc->free_recv_priv = &rtl8188eu_free_recv_priv;
  #ifdef CONFIG_SW_LED
  pHalFunc->InitSwLeds = &rtl8188eu_InitSwLeds;
  pHalFunc->DeInitSwLeds = &rtl8188eu_DeInitSwLeds;
  #else
  pHalFunc->InitSwLeds = NULL;
  pHalFunc->DeInitSwLeds = NULL;
  #endif//CONFIG_SW_LED
  
  pHalFunc->init_default_value = &rtl8188eu_init_default_value;
  pHalFunc->intf_chip_configure = &rtl8188eu_interface_configure;
  pHalFunc->read_adapter_info = &ReadAdapterInfo8188EU;
  
  
  
  
  pHalFunc->SetHwRegHandler = &SetHwReg8188EU;
  pHalFunc->GetHwRegHandler = &GetHwReg8188EU;
  pHalFunc->GetHalDefVarHandler = &GetHalDefVar8188EUsb;
  pHalFunc->SetHalDefVarHandler = &SetHalDefVar8188EUsb;
  
  pHalFunc->UpdateRAMaskHandler = &UpdateHalRAMask8188EUsb;
  pHalFunc->SetBeaconRelatedRegistersHandler = &SetBeaconRelatedRegisters8188EUsb;
  
  
  pHalFunc->hal_xmit = &rtl8188eu_hal_xmit;
  pHalFunc->mgnt_xmit = &rtl8188eu_mgnt_xmit;
  pHalFunc->hal_xmitframe_enqueue = &rtl8188eu_hal_xmitframe_enqueue;
  
  
  #ifdef CONFIG_HOSTAPD_MLME
  pHalFunc->hostap_mgnt_xmit_entry = &rtl8188eu_hostap_mgnt_xmit_entry;
  #endif
  pHalFunc->interface_ps_func = &rtl8188eu_ps_func;
  
  rtl8188e_set_hal_ops (pHalFunc);
  _func_exit_;
  
}

