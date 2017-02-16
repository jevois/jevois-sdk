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
#ifndef __HAL_COMMON_LED_H_
#define __HAL_COMMON_LED_H_


#define MSECS(t)        (HZ * ((t) / 1000) + (HZ * ((t) % 1000)) / 1000)

#define LED_BLINK_NORMAL_INTERVAL 100
#define LED_BLINK_SLOWLY_INTERVAL 200
#define LED_BLINK_LONG_INTERVAL 400
#define LED_INITIAL_INTERVAL    1800


#define LED_BLINK_NORMAL_INTERVAL_NETTRONIX 100
#define LED_BLINK_SLOWLY_INTERVAL_NETTRONIX 2000

#define LED_BLINK_SLOWLY_INTERVAL_PORNET  1000
#define LED_BLINK_NORMAL_INTERVAL_PORNET  100
#define LED_BLINK_FAST_INTERVAL_BITLAND   30

#define LED_CM2_BLINK_ON_INTERVAL   250
#define LED_CM2_BLINK_OFF_INTERVAL    4750
#define LED_CM8_BLINK_OFF_INTERVAL    3750 

#define LED_RunTop_BLINK_INTERVAL   300

#define LED_BLINK_NO_LINK_INTERVAL_ALPHA  1000
#define LED_BLINK_NO_LINK_INTERVAL_ALPHA_500MS 500
#define LED_BLINK_LINK_INTERVAL_ALPHA   500
#define LED_BLINK_SCAN_INTERVAL_ALPHA   180  
#define LED_BLINK_FASTER_INTERVAL_ALPHA   50
#define LED_BLINK_WPS_SUCESS_INTERVAL_ALPHA 5000

#define LED_CM11_BLINK_INTERVAL     300
#define LED_CM11_LINK_ON_INTERVEL   3000

#define LED_BLINK_LINK_INTERVAL_NETGEAR   500
#define LED_BLINK_LINK_SLOWLY_INTERVAL_NETGEAR    1000

#define LED_WPS_BLINK_OFF_INTERVAL_NETGEAR    100
#define LED_WPS_BLINK_ON_INTERVAL_NETGEAR   500

#define LED_BLINK_LINK_INTERVAL_ON_BELKIN   200
#define LED_BLINK_LINK_INTERVAL_OFF_BELKIN    100
#define LED_BLINK_ERROR_INTERVAL_BELKIN   100

#define LED_CM12_BLINK_INTERVAL_5Mbps   160
#define LED_CM12_BLINK_INTERVAL_10Mbps    80
#define LED_CM12_BLINK_INTERVAL_20Mbps    50
#define LED_CM12_BLINK_INTERVAL_40Mbps    40
#define LED_CM12_BLINK_INTERVAL_80Mbps    30
#define LED_CM12_BLINK_INTERVAL_MAXMbps   25

#define LED_BLINK_NO_LINK_INTERVAL    1000
#define LED_BLINK_LINK_IDEL_INTERVAL    100

#define LED_BLINK_SCAN_ON_INTERVAL    30
#define LED_BLINK_SCAN_OFF_INTERVAL   300

#define LED_WPS_BLINK_ON_INTERVAL_DLINK   30
#define LED_WPS_BLINK_OFF_INTERVAL_DLINK      300
#define LED_WPS_BLINK_LINKED_ON_INTERVAL_DLINK      5000


typedef enum _LED_CTL_MODE {
  LED_CTL_POWER_ON = 1,
  LED_CTL_LINK = 2,
  LED_CTL_NO_LINK = 3,
  LED_CTL_TX = 4,
  LED_CTL_RX = 5,
  LED_CTL_SITE_SURVEY = 6,
  LED_CTL_POWER_OFF = 7,
  LED_CTL_START_TO_LINK = 8,
  LED_CTL_START_WPS = 9,
  LED_CTL_STOP_WPS = 10,
  LED_CTL_START_WPS_BOTTON = 11,
  LED_CTL_STOP_WPS_FAIL = 12,
  LED_CTL_STOP_WPS_FAIL_OVERLAP = 13,
  LED_CTL_CONNECTION_NO_TRANSFER = 14,
} LED_CTL_MODE;

typedef enum _LED_STATE {
  LED_UNKNOWN = 0,
  RTW_LED_ON = 1,
  RTW_LED_OFF = 2,
  LED_BLINK_NORMAL = 3,
  LED_BLINK_SLOWLY = 4,
  LED_BLINK_POWER_ON = 5,
  LED_BLINK_SCAN = 6,  
  LED_BLINK_NO_LINK = 7,
  LED_BLINK_StartToBlink = 8,
  LED_BLINK_TXRX = 9,
  LED_BLINK_WPS = 10,
  LED_BLINK_WPS_STOP = 11, 
  LED_BLINK_WPS_STOP_OVERLAP = 12, 
  LED_BLINK_RUNTOP = 13, 
  LED_BLINK_CAMEO = 14,
  LED_BLINK_XAVI = 15,
  LED_BLINK_ALWAYS_ON = 16,
  LED_BLINK_LINK_IN_PROCESS = 17, 
  LED_BLINK_AUTH_ERROR = 18, 
  LED_BLINK_Azurewave_5Mbps = 19,
  LED_BLINK_Azurewave_10Mbps = 20,
  LED_BLINK_Azurewave_20Mbps = 21,
  LED_BLINK_Azurewave_40Mbps = 22,
  LED_BLINK_Azurewave_80Mbps = 23,
  LED_BLINK_Azurewave_MAXMbps = 24,
  LED_BLINK_LINK_IDEL = 25,
  LED_BLINK_WPS_LINKED = 26,
} LED_STATE;

typedef enum _LED_PIN {
  LED_PIN_GPIO0,
  LED_PIN_LED0,
  LED_PIN_LED1,
  LED_PIN_LED2
} LED_PIN;


#ifdef CONFIG_PCI_HCI
typedef enum _LED_STRATEGY_PCIE {
  SW_LED_MODE0,
  SW_LED_MODE1,
  SW_LED_MODE2,
  SW_LED_MODE3,
  SW_LED_MODE4,
  SW_LED_MODE5,
  SW_LED_MODE6,
  SW_LED_MODE7,
  SW_LED_MODE8,
  SW_LED_MODE9,
  SW_LED_MODE10,
  SW_LED_MODE11, 
  SW_LED_MODE12, 
  HW_LED,
} LED_STRATEGY_PCIE, *PLED_STRATEGY_PCIE;

typedef struct _LED_PCIE {
  PADAPTER    padapter;
  
  LED_PIN     LedPin;
  
  LED_STATE   CurrLedState;
  BOOLEAN     bLedOn;
  
  BOOLEAN     bLedBlinkInProgress;
  BOOLEAN     bLedWPSBlinkInProgress;
  
  BOOLEAN     bLedSlowBlinkInProgress;//added by vivi, for led new mode
  u32       BlinkTimes;
  LED_STATE   BlinkingLedState;
  
  _timer      BlinkTimer;
} LED_PCIE, *PLED_PCIE;

typedef struct _LED_PCIE  LED_DATA, *PLED_DATA;
typedef enum _LED_STRATEGY_PCIE LED_STRATEGY, *PLED_STRATEGY;

VOID
LedControlPCIE (
  IN  PADAPTER    Adapter,
  IN  LED_CTL_MODE    LedAction
);

VOID
gen_RefreshLedState (
  IN  PADAPTER    Adapter);

#elif defined(CONFIG_USB_HCI)

#define IS_LED_WPS_BLINKING(_LED_USB) (((PLED_USB)_LED_USB)->CurrLedState==LED_BLINK_WPS \
                                       || ((PLED_USB)_LED_USB)->CurrLedState==LED_BLINK_WPS_STOP \
                                       || ((PLED_USB)_LED_USB)->bLedWPSBlinkInProgress)

#define IS_LED_BLINKING(_LED_USB)   (((PLED_USB)_LED_USB)->bLedWPSBlinkInProgress \
                                     ||((PLED_USB)_LED_USB)->bLedScanBlinkInProgress)


typedef enum _LED_STRATEGY_USB {
  SW_LED_MODE0,
  SW_LED_MODE1,
  SW_LED_MODE2,
  SW_LED_MODE3,
  SW_LED_MODE4,
  SW_LED_MODE5,
  SW_LED_MODE6,
  SW_LED_MODE7,
  SW_LED_MODE8,
  SW_LED_MODE9,
  SW_LED_MODE10,
  SW_LED_MODE11,
  SW_LED_MODE12,
  SW_LED_MODE13,
  SW_LED_MODE14,
  SW_LED_MODE15,
  HW_LED,
} LED_STRATEGY_USB, *PLED_STRATEGY_USB;


typedef struct _LED_USB {
  PADAPTER      padapter;

  LED_PIN       LedPin;

  LED_STATE     CurrLedState;
  BOOLEAN       bLedOn;

  BOOLEAN       bSWLedCtrl;

  BOOLEAN       bLedBlinkInProgress;
  BOOLEAN       bLedNoLinkBlinkInProgress;
  BOOLEAN       bLedLinkBlinkInProgress;
  BOOLEAN       bLedStartToLinkBlinkInProgress;
  BOOLEAN       bLedScanBlinkInProgress;
  BOOLEAN       bLedWPSBlinkInProgress;

  u32         BlinkTimes;
  u8          BlinkCounter;
  LED_STATE     BlinkingLedState;

  _timer        BlinkTimer;

  _workitem     BlinkWorkItem;
} LED_USB, *PLED_USB;

typedef struct _LED_USB LED_DATA, *PLED_DATA;
typedef enum _LED_STRATEGY_USB  LED_STRATEGY, *PLED_STRATEGY;

VOID
LedControlUSB (
  IN  PADAPTER    Adapter,
  IN  LED_CTL_MODE    LedAction
);


#elif defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)

#define IS_LED_WPS_BLINKING(_LED_SDIO)  (((PLED_SDIO)_LED_SDIO)->CurrLedState==LED_BLINK_WPS \
    || ((PLED_SDIO)_LED_SDIO)->CurrLedState==LED_BLINK_WPS_STOP \
    || ((PLED_SDIO)_LED_SDIO)->bLedWPSBlinkInProgress)

#define IS_LED_BLINKING(_LED_SDIO)  (((PLED_SDIO)_LED_SDIO)->bLedWPSBlinkInProgress \
                                     ||((PLED_SDIO)_LED_SDIO)->bLedScanBlinkInProgress)


typedef enum _LED_STRATEGY_SDIO {
  SW_LED_MODE0,
  SW_LED_MODE1,
  SW_LED_MODE2,
  SW_LED_MODE3,
  SW_LED_MODE4,
  SW_LED_MODE5,
  SW_LED_MODE6,
  HW_LED,
} LED_STRATEGY_SDIO, *PLED_STRATEGY_SDIO;

typedef struct _LED_SDIO {
  PADAPTER      padapter;

  LED_PIN       LedPin;

  LED_STATE     CurrLedState;
  BOOLEAN       bLedOn;

  BOOLEAN       bSWLedCtrl;

  BOOLEAN       bLedBlinkInProgress;
  BOOLEAN       bLedNoLinkBlinkInProgress;
  BOOLEAN       bLedLinkBlinkInProgress;
  BOOLEAN       bLedStartToLinkBlinkInProgress;
  BOOLEAN       bLedScanBlinkInProgress;
  BOOLEAN       bLedWPSBlinkInProgress;

  u32         BlinkTimes;
  LED_STATE     BlinkingLedState;

  _timer        BlinkTimer;

  _workitem     BlinkWorkItem;
} LED_SDIO, *PLED_SDIO;

typedef struct _LED_SDIO  LED_DATA, *PLED_DATA;
typedef enum _LED_STRATEGY_SDIO LED_STRATEGY, *PLED_STRATEGY;

VOID
LedControlSDIO (
  IN  PADAPTER    Adapter,
  IN  LED_CTL_MODE    LedAction
);

#endif

struct led_priv {
  /* add for led controll */
  LED_DATA      SwLed0;
  LED_DATA      SwLed1;
  LED_DATA      SwLed2;
  LED_STRATEGY    LedStrategy;
  u8          bRegUseLed;
  void (*LedControlHandler) (_adapter * padapter, LED_CTL_MODE LedAction);
  void (*SwLedOn) (_adapter * padapter, PLED_DATA pLed);
  void (*SwLedOff) (_adapter * padapter, PLED_DATA pLed);
  /* add for led controll */
};

#ifdef CONFIG_SW_LED
#define rtw_led_control(adapter, LedAction) \
  do { \
    if((adapter)->ledpriv.LedControlHandler) \
      (adapter)->ledpriv.LedControlHandler((adapter), (LedAction)); \
  } while(0)
#else
#define rtw_led_control(adapter, LedAction)
#endif

#define SwLedOn(adapter, pLed) \
  do { \
    if((adapter)->ledpriv.SwLedOn) \
      (adapter)->ledpriv.SwLedOn((adapter), (pLed)); \
  } while(0)

#define SwLedOff(adapter, pLed) \
  do { \
    if((adapter)->ledpriv.SwLedOff) \
      (adapter)->ledpriv.SwLedOff((adapter), (pLed)); \
  } while(0)

void BlinkTimerCallback (void * data);
void BlinkWorkItemCallback (_workitem * work);

void ResetLedStatus (PLED_DATA pLed);

void
InitLed (
  _adapter   *   padapter,
  PLED_DATA   pLed,
  LED_PIN     LedPin
);

void
DeInitLed (
  PLED_DATA   pLed
);

extern void BlinkHandler (PLED_DATA  pLed);

#endif

