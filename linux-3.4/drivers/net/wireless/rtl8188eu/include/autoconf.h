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

#define CONFIG_ODM_REFRESH_RAMASK
#define CONFIG_PHY_SETTING_WITH_ODM
#define RTL8188E_FPGA_TRUE_PHY_VERIFICATION 0

/*
 * Public  General Config
 */
#define AUTOCONF_INCLUDED
#define RTL871X_MODULE_NAME "88EU"
#define DRV_NAME "rtl8188eu"

#define CONFIG_USB_HCI

#define CONFIG_RTL8188E

#define PLATFORM_LINUX


#if defined( CONFIG_PLATFORM_ARM_SUNxI) || defined(CONFIG_PLATFORM_ACTIONS_ATM702X)
#ifndef CONFIG_IOCTL_CFG80211
#define CONFIG_IOCTL_CFG80211
#endif
#endif

#ifdef CONFIG_IOCTL_CFG80211
#define CONFIG_CFG80211_FORCE_COMPATIBLE_2_6_37_UNDER
#define CONFIG_SET_SCAN_DENY_TIMER

#endif

/*
 * Internal  General Config
 */


#define CONFIG_EMBEDDED_FWIMG

#define CONFIG_XMIT_ACK
#ifdef CONFIG_XMIT_ACK
#define CONFIG_ACTIVE_KEEP_ALIVE_CHECK
#endif
#define CONFIG_80211N_HT

#define CONFIG_RECV_REORDERING_CTRL



#define CONFIG_SUPPORT_USB_INT
#ifdef CONFIG_SUPPORT_USB_INT
#endif

#define CONFIG_IPS
#ifdef CONFIG_IPS
#endif
#define SUPPORT_HW_RFOFF_DETECTED

#define CONFIG_LPS
#if defined(CONFIG_LPS) && defined(CONFIG_SUPPORT_USB_INT)


#endif

#ifdef CONFIG_LPS_LCLK
#define CONFIG_XMIT_THREAD_MODE
#endif

#define CONFIG_ANTENNA_DIVERSITY

#ifdef CONFIG_ANTENNA_DIVERSITY
#define CONFIG_HW_ANTENNA_DIVERSITY
#endif


#ifdef CONFIG_CONCURRENT_MODE
#define CONFIG_TSF_RESET_OFFLOAD     
#endif

#define CONFIG_IOL


#define CONFIG_AP_MODE
#ifdef CONFIG_AP_MODE
#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_INTERRUPT_BASED_TXBCN)
#undef CONFIG_INTERRUPT_BASED_TXBCN
#endif
#ifdef CONFIG_INTERRUPT_BASED_TXBCN
#define CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
#endif

#define CONFIG_NATIVEAP_MLME
#ifndef CONFIG_NATIVEAP_MLME
#define CONFIG_HOSTAPD_MLME
#endif
#define CONFIG_FIND_BEST_CHANNEL
#endif

#define CONFIG_P2P
#ifdef CONFIG_P2P
#define CONFIG_WFD

#ifndef CONFIG_WIFI_TEST
#define CONFIG_P2P_REMOVE_GROUP_INFO
#endif

#define CONFIG_P2P_PS
#endif

#ifdef CONFIG_TDLS
#endif


#define CONFIG_SKB_COPY

#ifdef CONFIG_LED
#define CONFIG_SW_LED
#ifdef CONFIG_SW_LED
#endif
#endif

#ifdef CONFIG_IOL
#define CONFIG_IOL_NEW_GENERATION
#define CONFIG_IOL_READ_EFUSE_MAP
#define CONFIG_IOL_EFUSE_PATCH
#endif


#define USB_INTERFERENCE_ISSUE
#define CONFIG_GLOBAL_UI_PID

#define CONFIG_LAYER2_ROAMING
#define CONFIG_LAYER2_ROAMING_RESUME
#define CONFIG_LONG_DELAY_ISSUE
#define CONFIG_NEW_SIGNAL_STAT_PROCESS
#define RTW_NOTCH_FILTER 0 /* 0:Disable, 1:Enable, */

#define CONFIG_BR_EXT  
#ifdef CONFIG_BR_EXT
#define CONFIG_BR_EXT_BRNAME  "br0"
#endif 

#define CONFIG_TX_MCAST2UNI  
#define CONFIG_CHECK_AC_LIFETIME 


/*
 * Interface  Related Config
 */

#ifndef CONFIG_MINIMAL_MEMORY_USAGE
#define CONFIG_USB_TX_AGGREGATION
#define CONFIG_USB_RX_AGGREGATION
#endif

#define CONFIG_PREALLOC_RECV_SKB

/*
 * CONFIG_USE_USB_BUFFER_ALLOC_XX uses Linux USB Buffer alloc API and is for Linux platform only now!
 */
#ifdef CONFIG_USE_USB_BUFFER_ALLOC_RX
#undef CONFIG_PREALLOC_RECV_SKB
#endif

#ifdef CONFIG_PLATFORM_ARM_SUNxI
#ifndef   CONFIG_USE_USB_BUFFER_ALLOC_TX
#define CONFIG_USE_USB_BUFFER_ALLOC_TX
#endif
#endif

/*
 * USB VENDOR REQ BUFFER ALLOCATION METHOD
 * if not set we'll use function local variable (stack memory)
 */
#define CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC

#define CONFIG_USB_VENDOR_REQ_MUTEX
#define CONFIG_VENDOR_REQ_RETRY



/*
 * HAL  Related Config
 */

#define RTL8188E_RX_PACKET_INCLUDE_CRC  0

#define SUPPORTED_BLOCK_IO



#define CONFIG_OUT_EP_WIFI_MODE 0

#define ENABLE_USB_DROP_INCORRECT_OUT 0



#define DISABLE_BB_RF 0


#ifdef CONFIG_MP_INCLUDED
#define MP_DRIVER 1
#define CONFIG_MP_IWPRIV_SUPPORT
#else
#define MP_DRIVER 0
#endif


/*
 * Platform  Related Config
 */
#ifdef CONFIG_PLATFORM_MN10300
#define CONFIG_SPECIAL_SETTING_FOR_FUNAI_TV
#define CONFIG_USE_USB_BUFFER_ALLOC_RX

#if defined (CONFIG_SW_ANTENNA_DIVERSITY)
#undef CONFIG_SW_ANTENNA_DIVERSITY
#define CONFIG_HW_ANTENNA_DIVERSITY
#endif

#if defined (CONFIG_POWER_SAVING)
#undef CONFIG_POWER_SAVING
#endif

#endif//CONFIG_PLATFORM_MN10300



#ifdef CONFIG_PLATFORM_TI_DM365
#define CONFIG_USE_USB_BUFFER_ALLOC_RX
#endif


#if defined(CONFIG_PLATFORM_ACTIONS_ATM702X)
#ifdef CONFIG_USB_TX_AGGREGATION
#undef CONFIG_USB_TX_AGGREGATION
#endif
#ifndef CONFIG_USE_USB_BUFFER_ALLOC_TX
#define CONFIG_USE_USB_BUFFER_ALLOC_TX
#endif
#ifndef CONFIG_USE_USB_BUFFER_ALLOC_RX
#define CONFIG_USE_USB_BUFFER_ALLOC_RX
#endif
#endif


/*
 * Outsource  Related Config
 */

#define   RTL8192CE_SUPPORT         0
#define   RTL8192CU_SUPPORT       0
#define   RTL8192C_SUPPORT        (RTL8192CE_SUPPORT|RTL8192CU_SUPPORT)

#define   RTL8192DE_SUPPORT         0
#define   RTL8192DU_SUPPORT       0
#define   RTL8192D_SUPPORT        (RTL8192DE_SUPPORT|RTL8192DU_SUPPORT)

#define   RTL8723AU_SUPPORT       0
#define   RTL8723AS_SUPPORT       0
#define   RTL8723AE_SUPPORT       0
#define   RTL8723A_SUPPORT        (RTL8723AU_SUPPORT|RTL8723AS_SUPPORT|RTL8723AE_SUPPORT)

#define   RTL8723_FPGA_VERIFICATION   0

#define RTL8188EE_SUPPORT       0
#define RTL8188EU_SUPPORT       1
#define RTL8188ES_SUPPORT       0
#define RTL8188E_SUPPORT        (RTL8188EE_SUPPORT|RTL8188EU_SUPPORT|RTL8188ES_SUPPORT)
#define RTL8188E_FOR_TEST_CHIP      0
#define RATE_ADAPTIVE_SUPPORT       1
#define POWER_TRAINING_ACTIVE     1


#ifdef CONFIG_USB_TX_AGGREGATION
#endif

#ifdef CONFIG_TX_EARLY_MODE
#define RTL8188E_EARLY_MODE_PKT_NUM_10  0
#endif

#define CONFIG_80211D

#define CONFIG_ATTEMPT_TO_FIX_AP_BEACON_ERROR

/*
 * Debug Related Config
 */
#define DBG 1


#define CONFIG_PROC_DEBUG

#define DBG_CONFIG_ERROR_DETECT
#define DBG_CONFIG_ERROR_RESET










