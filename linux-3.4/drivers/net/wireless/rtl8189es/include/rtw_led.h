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
#ifndef __RTW_LED_H_
#define __RTW_LED_H_

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>

#define MSECS(t)        (HZ * ((t) / 1000) + (HZ * ((t) % 1000)) / 1000)

#define LED_BLINK_NORMAL_INTERVAL	100
#define LED_BLINK_SLOWLY_INTERVAL	200
#define LED_BLINK_LONG_INTERVAL	400

#define LED_BLINK_NO_LINK_INTERVAL_ALPHA		1000
#define LED_BLINK_LINK_INTERVAL_ALPHA			500	
#define LED_BLINK_SCAN_INTERVAL_ALPHA		180 
#define LED_BLINK_FASTER_INTERVAL_ALPHA		50
#define LED_BLINK_WPS_SUCESS_INTERVAL_ALPHA	5000

#define LED_BLINK_NORMAL_INTERVAL_NETTRONIX  100
#define LED_BLINK_SLOWLY_INTERVAL_NETTRONIX  2000

#define LED_BLINK_SLOWLY_INTERVAL_PORNET 1000
#define LED_BLINK_NORMAL_INTERVAL_PORNET 100

#define LED_BLINK_FAST_INTERVAL_BITLAND 30

#define LED_CM2_BLINK_ON_INTERVAL			250
#define LED_CM2_BLINK_OFF_INTERVAL		4750

#define LED_CM8_BLINK_INTERVAL			500	
#define LED_CM8_BLINK_OFF_INTERVAL	3750

#define LED_RunTop_BLINK_INTERVAL			300

#define LED_CM3_BLINK_INTERVAL				1500

typedef enum _LED_CTL_MODE{
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
}LED_CTL_MODE;

typedef enum _LED_STATE_871x{
	LED_UNKNOWN = 0,
	RTW_LED_ON = 1,
	RTW_LED_OFF = 2,
	LED_BLINK_NORMAL = 3,
	LED_BLINK_SLOWLY = 4,
	LED_BLINK_POWER_ON = 5,
	LED_BLINK_SCAN = 6,
	LED_BLINK_NO_LINK = 7,
	LED_BLINK_StartToBlink = 8,// Customzied for Sercomm Printer Server case
	LED_BLINK_TXRX = 9,
	LED_BLINK_WPS = 10,
	LED_BLINK_WPS_STOP = 11,
	LED_BLINK_WPS_STOP_OVERLAP = 12,
	LED_BLINK_RUNTOP = 13,
	LED_BLINK_CAMEO = 14,
	LED_BLINK_XAVI = 15,
	LED_BLINK_ALWAYS_ON = 16,	
}LED_STATE_871x;

typedef enum _LED_PIN_871x{
	LED_PIN_NULL = 0,
	LED_PIN_LED0 = 1,
	LED_PIN_LED1 = 2,
	LED_PIN_LED2 = 3,
	LED_PIN_GPIO0 = 4,
}LED_PIN_871x;

typedef struct _LED_871x{
	_adapter				*padapter;
	
	LED_PIN_871x		LedPin;
	LED_STATE_871x		CurrLedState;
	LED_STATE_871x		BlinkingLedState;
	
	u8					bLedOn;

	u8					bLedBlinkInProgress;

	u8					bLedWPSBlinkInProgress;

	u32					BlinkTimes;

	_timer				BlinkTimer;

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
	u8					bSWLedCtrl;

	u8					bLedNoLinkBlinkInProgress;
	u8					bLedLinkBlinkInProgress;
	u8					bLedStartToLinkBlinkInProgress;
	u8					bLedScanBlinkInProgress;
	
	#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)|| defined PLATFORM_FREEBSD
	_workitem			BlinkWorkItem;
	#endif
#endif
	
#if defined(CONFIG_PCI_HCI)
	u8					bLedSlowBlinkInProgress;//added by vivi, for led new mode
#endif

} LED_871x, *PLED_871x;

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)

#define IS_LED_WPS_BLINKING(_LED_871x)	(((PLED_871x)_LED_871x)->CurrLedState==LED_BLINK_WPS \
					|| ((PLED_871x)_LED_871x)->CurrLedState==LED_BLINK_WPS_STOP \
					|| ((PLED_871x)_LED_871x)->bLedWPSBlinkInProgress)

#define IS_LED_BLINKING(_LED_871x) 	(((PLED_871x)_LED_871x)->bLedWPSBlinkInProgress \
					||((PLED_871x)_LED_871x)->bLedScanBlinkInProgress)


typedef	enum _LED_STRATEGY_871x{
	SW_LED_MODE0 = 0,
	SW_LED_MODE1= 1,
	SW_LED_MODE2 = 2,
	SW_LED_MODE3 = 3,
	SW_LED_MODE4 = 4,
	SW_LED_MODE5 = 5,
	SW_LED_MODE6 = 6,
	HW_LED = 50,
	LED_ST_NONE = 99,
}LED_STRATEGY_871x, *PLED_STRATEGY_871x;

void
LedControl871x(
	_adapter				*padapter,
	LED_CTL_MODE		LedAction
	);
#endif

#if defined(CONFIG_PCI_HCI)

typedef	enum _LED_STRATEGY_871x{
	SW_LED_MODE0 = 0,
	SW_LED_MODE1 = 1,
	SW_LED_MODE2 = 2,
	SW_LED_MODE3 = 3,
	SW_LED_MODE4 = 4,
	SW_LED_MODE5 = 5,
	SW_LED_MODE6 = 6,
	SW_LED_MODE7 = 7,
	SW_LED_MODE8 = 8,
	SW_LED_MODE9 = 9,
	SW_LED_MODE10 = 10,
	HW_LED = 50,
	LED_ST_NONE = 99,
}LED_STRATEGY_871x, *PLED_STRATEGY_871x;
#endif

struct led_priv{
	/* add for led controll */
	LED_871x			SwLed0;
	LED_871x			SwLed1;
	LED_STRATEGY_871x	LedStrategy;
	u8					bRegUseLed;
	void (*LedControlHandler)(_adapter *padapter, LED_CTL_MODE LedAction);
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

void BlinkTimerCallback(void *data);
void BlinkWorkItemCallback(struct work_struct *work);

void ResetLedStatus(PLED_871x pLed);

void
InitLed871x(
	_adapter			*padapter,
	PLED_871x		pLed,
	LED_PIN_871x	LedPin
	);

void
DeInitLed871x(
	PLED_871x			pLed
	);

extern void BlinkHandler(PLED_871x	 pLed);

#endif

