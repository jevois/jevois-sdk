/******************************************************************************
 *
 * Copyright(c) 2010 - 2012 Realtek Corporation. All rights reserved.
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
/*
 * Automatically generated C config: don't edit
 */

#define CONFIG_ODM_REFRESH_RAMASK
#define CONFIG_PHY_SETTING_WITH_ODM


#define AUTOCONF_INCLUDED
#define RTL871X_MODULE_NAME "8189ES"
#define DRV_NAME "rtl8189es"

#define CONFIG_RTL8188E
#define CONFIG_SDIO_HCI
#define PLATFORM_LINUX


#ifdef CONFIG_PLATFORM_ARM_SUN8I
#ifndef CONFIG_IOCTL_CFG80211
#define CONFIG_IOCTL_CFG80211
#endif
#endif

#ifdef CONFIG_IOCTL_CFG80211
#define CONFIG_CFG80211_FORCE_COMPATIBLE_2_6_37_UNDER
#define CONFIG_SET_SCAN_DENY_TIMER
#endif

#define CONFIG_EMBEDDED_FWIMG

#define CONFIG_XMIT_ACK
#ifdef CONFIG_XMIT_ACK
#define CONFIG_ACTIVE_KEEP_ALIVE_CHECK
#endif
#define CONFIG_80211N_HT
#define CONFIG_RECV_REORDERING_CTRL

#ifdef CONFIG_CONCURRENT_MODE
#define CONFIG_TSF_RESET_OFFLOAD   
#endif

#define CONFIG_AP_MODE
#ifdef CONFIG_AP_MODE

#define CONFIG_INTERRUPT_BASED_TXBCN
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
#endif

#define CONFIG_TX_MCAST2UNI  
#define CONFIG_CHECK_AC_LIFETIME 

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

#define CONFIG_LAYER2_ROAMING
#define CONFIG_LAYER2_ROAMING_RESUME

#define CONFIG_LONG_DELAY_ISSUE
#define CONFIG_NEW_SIGNAL_STAT_PROCESS
#define RTW_NOTCH_FILTER 0 /* 0:Disable, 1:Enable, */

/*
 * Hardware Related Config
 */


#define CONFIG_REGULATORY_CTRL

/*
 * Interface Related Config
 */
#define CONFIG_SDIO_RX_COPY


/*
 * Others
 */


/*
 * Auto Config Section
 */
#if defined(CONFIG_RTL8188E) && defined(CONFIG_SDIO_HCI)
#define CONFIG_RTL8188E_SDIO
#define CONFIG_XMIT_THREAD_MODE
#endif

#define CONFIG_IPS
#define CONFIG_LPS
#if defined(CONFIG_LPS) && defined(CONFIG_SDIO_HCI)
#define CONFIG_LPS_LCLK
#endif

#ifdef CONFIG_MAC_LOOPBACK_DRIVER
#undef CONFIG_AP_MODE
#undef CONFIG_NATIVEAP_MLME
#undef CONFIG_POWER_SAVING
#undef SUPPORT_HW_RFOFF_DETECTED
#endif

#ifdef CONFIG_MP_INCLUDED

#define MP_DRIVER   1
#define CONFIG_MP_IWPRIV_SUPPORT


#else// #ifdef CONFIG_MP_INCLUDED

#define MP_DRIVER   0

#endif

#define CONFIG_IOL
#ifdef CONFIG_IOL
#define CONFIG_IOL_NEW_GENERATION
#define CONFIG_IOL_READ_EFUSE_MAP
#define CONFIG_IOL_EFUSE_PATCH
#endif


#define   CONFIG_TX_AGGREGATION


#ifdef CONFIG_PLATFORM_ACTIONS_ATV5201
#define CONFIG_SDIO_DISABLE_RXFIFO_POLLING_LOOP
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

#define   RTL8723_FPGA_VERIFICATION   0
#define   RTL8723AU_SUPPORT       0
#define   RTL8723AS_SUPPORT       0
#define   RTL8723AE_SUPPORT       0
#define   RTL8723A_SUPPORT        (RTL8723AU_SUPPORT|RTL8723AS_SUPPORT|RTL8723AE_SUPPORT)

#define RTL8188EE_SUPPORT       0
#define RTL8188EU_SUPPORT       0
#define RTL8188ES_SUPPORT       1
#define RTL8188E_SUPPORT        (RTL8188EE_SUPPORT|RTL8188EU_SUPPORT|RTL8188ES_SUPPORT)
#define RTL8188E_FOR_TEST_CHIP      0
#define RATE_ADAPTIVE_SUPPORT       1
#define POWER_TRAINING_ACTIVE     1

#ifdef CONFIG_TX_EARLY_MODE
#define RTL8188E_EARLY_MODE_PKT_NUM_10  0
#endif

#define CONFIG_ATTEMPT_TO_FIX_AP_BEACON_ERROR

/*
 * HAL  Related Config
 */

#define RTL8188E_FPGA_TRUE_PHY_VERIFICATION 0

#define DISABLE_BB_RF 0

#if DISABLE_BB_RF
#define HAL_MAC_ENABLE    1
#define HAL_BB_ENABLE   0
#define HAL_RF_ENABLE   0
#else
#define HAL_MAC_ENABLE    1
#define HAL_BB_ENABLE   1
#define HAL_RF_ENABLE   1
#endif

#define CONFIG_80211D

/*
 * Debug Related Config
 */
#define DBG 1

#define CONFIG_DEBUG /* DBG_871X, etc... */

#define CONFIG_PROC_DEBUG

#define DBG_CONFIG_ERROR_DETECT
#define DBG_CONFIG_ERROR_RESET









