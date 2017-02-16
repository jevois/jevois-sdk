/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef __NAND_DRV_CFG_H
#define __NAND_DRV_CFG_H

#include "../../osal/nand_osal.h"

#define  PLATFORM           20
#define  NAND_VERSION_0                 0x02
#define  NAND_VERSION_1                 0x13
#define  TIME             2119
#define  NAND_DRV_DATE                  0x20140509
#define  NAND_PART_TABLE_MAGIC          0x0055ff00



#define MAX_CHIP_SELECT_CNT                 (8)

#define NAND_MAX_PART_CNT                   (20)

#define MAX_ZONE_CNT                        (32)

#define MAX_SUPER_PAGE_CNT                  (1024)

#define BLOCK_MAP_TBL_CACHE_CNT             (4)
#if (BLOCK_MAP_TBL_CACHE_CNT < 1)
#error BLOCK_MAP_TBL_CACHE_CNT config error, the value must be larger than 0!!!
#endif

#define PAGE_MAP_TBL_CACHE_CNT              (52)
#if (PAGE_MAP_TBL_CACHE_CNT < 1)
#error PAGE_MAP_TBL_CACHE_CNT config error, the value must be larger than 0!!!
#endif

#define MAX_LOG_BLK_CNT                     (52)

#define WEAR_LEVELLING_FREQUENCY            (10)

#define BOOT_CHIP_SELECT_NUM                (0)

#define DEFAUL_DATA_BLK_CNT_PER_ZONE        (1000)
#if (DEFAUL_DATA_BLK_CNT_PER_ZONE > 1000)
#error  DEFAUL_DATA_BLK_CNT_PER_ZONE config error, the value must not be larger than 1000!!!
#endif


#define CFG_SUPPORT_MULTI_PLANE_PROGRAM         (1)

#define CFG_SUPPORT_MULTI_PLANE_READ            (0)

#define CFG_SUPPORT_INT_INTERLEAVE              (0)

#define CFG_SUPPORT_EXT_INTERLEAVE              (1)

#define CFG_SUPPORT_CACHE_PROGRAM               (0)

#define CFG_SUPPORT_PAGE_COPYBACK               (1)

#define CFG_SUPPORT_WEAR_LEVELLING              (0)

#define CFG_SUPPORT_READ_RECLAIM                (1)

#define CFG_SUPPORT_CHECK_WRITE_SYNCH           (0)

#define CFG_SUPPORT_ALIGN_NAND_BNK              (1)

#define CFG_SUPPORT_RANDOM                      (1)

#define CFG_SUPPORT_READ_RETRY                  (1)


#define SUPPORT_DMA_IRQ             (0)
#define SUPPORT_RB_IRQ              (0)

#ifndef __OS_NAND_DBG__
#define PHY_DBG_MESSAGE_ON                  (0)
#else
#define PHY_DBG_MESSAGE_ON                  (1)
#endif

#define PHY_ERR_MESSAGE_ON                  (1)

#ifndef __OS_NAND_DBG__
#define SCAN_DBG_MESSAGE_ON                 (0)
#else
#define SCAN_DBG_MESSAGE_ON                 (1)
#endif

#define SCAN_ERR_MESSAGE_ON                 (1)

#define FORMAT_DBG_MESSAGE_ON               (0)

#define FORMAT_ERR_MESSAGE_ON               (1)

#define MAPPING_DBG_MESSAGE_ON              (0)

#define MAPPING_ERR_MESSAGE_ON              (1)

#define LOGICCTL_DBG_MESSAGE_ON             (0)

#define LOGICCTL_ERR_MESSAGE_ON             (1)

#define MBR_DBG_MESSAGE_ON             (0)



#if PHY_DBG_MESSAGE_ON
#define    PHY_DBG(...)             PRINT(__VA_ARGS__)
#else
#define     PHY_DBG(...)
#endif

#if PHY_ERR_MESSAGE_ON
#define     PHY_ERR(...)            PRINT(__VA_ARGS__)
#else
#define     PHY_ERR(...)
#endif


#if SCAN_DBG_MESSAGE_ON
#define     SCAN_DBG(...)             PRINT(__VA_ARGS__)
#else
#define     SCAN_DBG(...)
#endif

#if SCAN_ERR_MESSAGE_ON
#define     SCAN_ERR(...)             PRINT(__VA_ARGS__)
#else
#define     SCAN_ERR(...)
#endif


#if FORMAT_DBG_MESSAGE_ON
#define     FORMAT_DBG(...)           PRINT(__VA_ARGS__)
#else
#define     FORMAT_DBG(...)
#endif

#if FORMAT_ERR_MESSAGE_ON
#define     FORMAT_ERR(...)           PRINT(__VA_ARGS__)
#else
#define     FORMAT_ERR(...)
#endif


#if MAPPING_DBG_MESSAGE_ON
#define     MAPPING_DBG(...)          PRINT(__VA_ARGS__)
#else
#define     MAPPING_DBG(...)
#endif

#if MAPPING_ERR_MESSAGE_ON
#define     MAPPING_ERR(...)          PRINT(__VA_ARGS__)
#else
#define     MAPPING_ERR(...)
#endif


#if LOGICCTL_DBG_MESSAGE_ON
#define     LOGICCTL_DBG(...)         PRINT(__VA_ARGS__)
#else
#define     LOGICCTL_DBG(...)
#endif

#if LOGICCTL_ERR_MESSAGE_ON
#define     LOGICCTL_ERR(...)         PRINT(__VA_ARGS__)
#else
#define     LOGICCTL_ERR(...)
#endif

#if MBR_DBG_MESSAGE_ON
#define     MBR_DBG(...)          PRINT(__VA_ARGS__)
#else
#define     MBR_DBG(...)
#endif

#endif

