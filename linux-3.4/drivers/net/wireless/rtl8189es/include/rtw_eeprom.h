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
#ifndef __RTW_EEPROM_H__
#define __RTW_EEPROM_H__

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>

#define RTL8712_EEPROM_ID     0x8712

#define HWSET_MAX_SIZE_512    512
#define EEPROM_MAX_SIZE     HWSET_MAX_SIZE_512

#define CLOCK_RATE          50     

#define EEPROM_READ_OPCODE    06
#define EEPROM_WRITE_OPCODE   05
#define EEPROM_ERASE_OPCODE   07
#define EEPROM_EWEN_OPCODE    19     
#define EEPROM_EWDS_OPCODE    16     

#define USA             0x555320
#define EUROPE            0x1
#define JAPAN           0x2

#ifdef CONFIG_SDIO_HCI
#define eeprom_cis0_sz  17
#define eeprom_cis1_sz  50
#endif

#define EEPROM_CID_DEFAULT      0x0
#define EEPROM_CID_ALPHA        0x1
#define EEPROM_CID_Senao        0x3
#define EEPROM_CID_NetCore        0x5
#define EEPROM_CID_CAMEO        0X8
#define EEPROM_CID_SITECOM        0x9
#define EEPROM_CID_COREGA       0xB
#define EEPROM_CID_EDIMAX_BELKIN    0xC
#define EEPROM_CID_SERCOMM_BELKIN   0xE
#define EEPROM_CID_CAMEO1       0xF
#define EEPROM_CID_WNC_COREGA   0x12
#define EEPROM_CID_CLEVO        0x13
#define EEPROM_CID_WHQL       0xFE

typedef enum _RT_CUSTOMER_ID
{
  RT_CID_DEFAULT = 0,
  RT_CID_8187_ALPHA0 = 1,
  RT_CID_8187_SERCOMM_PS = 2,
  RT_CID_8187_HW_LED = 3,
  RT_CID_8187_NETGEAR = 4,
  RT_CID_WHQL = 5,
  RT_CID_819x_CAMEO  = 6,
  RT_CID_819x_RUNTOP = 7,
  RT_CID_819x_Senao = 8,
  RT_CID_TOSHIBA = 9,
  RT_CID_819x_Netcore = 10,
  RT_CID_Nettronix = 11,
  RT_CID_DLINK = 12,
  RT_CID_PRONET = 13,
  RT_CID_COREGA = 14,
  RT_CID_CHINA_MOBILE = 15,
  RT_CID_819x_ALPHA = 16,
  RT_CID_819x_Sitecom = 17,
  RT_CID_CCX = 18,
  RT_CID_819x_Lenovo = 19,
  RT_CID_819x_QMI = 20,
  RT_CID_819x_Edimax_Belkin = 21,
  RT_CID_819x_Sercomm_Belkin = 22,
  RT_CID_819x_CAMEO1 = 23,
  RT_CID_819x_MSI = 24,
  RT_CID_819x_Acer = 25,
  RT_CID_819x_AzWave_ASUS = 26,
  RT_CID_819x_AzWave = 27,
  RT_CID_819x_HP = 28,
  RT_CID_819x_WNC_COREGA = 29,
  RT_CID_819x_Arcadyan_Belkin = 30,
  RT_CID_819x_SAMSUNG = 31,
  RT_CID_819x_CLEVO = 32,
  RT_CID_819x_DELL = 33,
  RT_CID_819x_PRONETS = 34,
  RT_CID_819x_Edimax_ASUS = 35,
  RT_CID_819x_CAMEO_NETGEAR = 36,
  RT_CID_PLANEX = 37,
  RT_CID_CC_C = 38,
  RT_CID_819x_Xavi = 39,
  RT_CID_819x_FUNAI_TV = 40,
  RT_CID_819x_ALPHA_WD = 41,
} RT_CUSTOMER_ID, *PRT_CUSTOMER_ID;

struct eeprom_priv
{
  u8    bautoload_fail_flag;
  u8    bloadfile_fail_flag;
  u8    bloadmac_fail_flag;
  u8    mac_addr[6]; 
  u16   channel_plan;
  
  u8    EepromOrEfuse;
  
  u8    efuse_eeprom_data[HWSET_MAX_SIZE_512];
  
  #ifdef CONFIG_RF_GAIN_OFFSET
  u8    EEPROMRFGainOffset;
  u8    EEPROMRFGainVal;
  #endif
  
  #ifdef CONFIG_SDIO_HCI
  u8    sdio_setting;
  u32   ocr;
  u8    cis0[eeprom_cis0_sz];
  u8    cis1[eeprom_cis1_sz];
  #endif
};


extern void eeprom_write16 (_adapter * padapter, u16 reg, u16 data);
extern u16 eeprom_read16 (_adapter * padapter, u16 reg);
extern void read_eeprom_content (_adapter * padapter);
extern void eeprom_read_sz (_adapter * padapter, u16 reg, u8 * data, u32 sz);

extern void read_eeprom_content_by_attrib (_adapter  * padapter  );

#ifdef PLATFORM_LINUX
#ifdef CONFIG_ADAPTOR_INFO_CACHING_FILE
extern int isAdaptorInfoFileValid (void);
extern int storeAdaptorInfoFile (char * path, struct eeprom_priv * eeprom_priv);
extern int retriveAdaptorInfoFile (char * path, struct eeprom_priv * eeprom_priv);
#endif
#endif

#endif 

