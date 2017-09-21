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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef __NAND_PHYSIC_H__
#define __NAND_PHYSIC_H__

#include "nand_type.h"

extern struct __NandStorageInfo_t  NandStorageInfo;
extern struct __NandPageCachePool_t PageCachePool;




#define ECC_MODE           (NandStorageInfo.EccMode)

#define DDR_TYPE           (NandStorageInfo.DDRType)

#define SECTOR_CNT_OF_SINGLE_PAGE           (NandStorageInfo.SectorCntPerPage)

#define SECTOR_CNT_OF_SUPER_PAGE            (NandStorageInfo.SectorCntPerPage * NandStorageInfo.PlaneCntPerDie)

#define FULL_BITMAP_OF_SINGLE_PAGE           ((__u64)(((__u64)1<<(SECTOR_CNT_OF_SINGLE_PAGE - 1)) | (((__u64)1<<(SECTOR_CNT_OF_SINGLE_PAGE - 1)) - 1)))

#define FULL_BITMAP_OF_SUPER_PAGE           ((__u64)(((__u64)1<<(SECTOR_CNT_OF_SUPER_PAGE - 1)) | (((__u64)1<<(SECTOR_CNT_OF_SUPER_PAGE - 1)) - 1)))

#define MULTI_PLANE_BLOCK_OFFSET            (NandStorageInfo.OptPhyOpPar.MultiPlaneBlockOffset)

#define BAD_BLOCK_FLAG_PST                  (NandStorageInfo.OptPhyOpPar.BadBlockFlagPosition)

#define SUPPORT_CACHE_READ                  (NAND_CACHE_READ & NandStorageInfo.OperationOpt)

#define SUPPORT_CACHE_PROGRAM               (NAND_CACHE_PROGRAM & NandStorageInfo.OperationOpt)

#define SUPPORT_MULTI_READ                  (NAND_MULTI_READ & NandStorageInfo.OperationOpt)

#define SUPPORT_MULTI_PROGRAM               (NAND_MULTI_PROGRAM & NandStorageInfo.OperationOpt)

#define SUPPORT_PAGE_COPYBACK               (NAND_PAGE_COPYBACK & NandStorageInfo.OperationOpt)

#define SUPPORT_INT_INTERLEAVE              (NAND_INT_INTERLEAVE & NandStorageInfo.OperationOpt)

#define SUPPORT_EXT_INTERLEAVE              (NAND_EXT_INTERLEAVE & NandStorageInfo.OperationOpt)

#define SUPPORT_RANDOM                      (NAND_RANDOM & NandStorageInfo.OperationOpt)

#define SUPPORT_READ_RETRY                  (NAND_READ_RETRY & NandStorageInfo.OperationOpt)

#define SUPPORT_READ_UNIQUE_ID              (NAND_READ_UNIQUE_ID & NandStorageInfo.OperationOpt)

#define SUPPORT_ALIGN_NAND_BNK              (!(NAND_PAGE_ADR_NO_SKIP & NandStorageInfo.OperationOpt))

#define SUPPORT_DIE_SKIP                    (NAND_DIE_SKIP & NandStorageInfo.OperationOpt)

#define SUPPORT_LOG_BLOCK_MANAGE            (NAND_LOG_BLOCK_MANAGE & NandStorageInfo.OperationOpt)
#define LOG_BLOCK_LSB_PAGE_TYPE             ((NAND_LOG_BLOCK_LSB_TYPE & NandStorageInfo.OperationOpt)>>16)


#define DIE_CNT_OF_CHIP                     (NandStorageInfo.DieCntPerChip)

#define BNK_CNT_OF_CHIP                     (NandStorageInfo.BankCntPerChip)

#define RB_CONNECT_MODE                    (NandStorageInfo.RbConnectMode)

#define TOTAL_BANK_CNT                      (NandStorageInfo.BankCntPerChip * NandStorageInfo.ChipCnt)

#define BLOCK_CNT_OF_DIE                    (NandStorageInfo.BlkCntPerDie)

#define PLANE_CNT_OF_DIE                    (NandStorageInfo.PlaneCntPerDie)

#define PAGE_CNT_OF_PHY_BLK                 (NandStorageInfo.PageCntPerPhyBlk)

#define CHIP_CONNECT_INFO                   (NandStorageInfo.ChipConnectInfo)

#define READ_RETRY_TYPE                   (NandStorageInfo.ReadRetryType)

#define READ_RETRY_MODE                   ((READ_RETRY_TYPE>>16)&0xff)

#define READ_RETRY_CYCLE                   ((READ_RETRY_TYPE>>8)&0xff)

#define READ_RETRY_REG_CNT                   ((READ_RETRY_TYPE>>0)&0xff)

#define NAND_ACCESS_FREQUENCE               (NandStorageInfo.FrequencePar)

#define BAD_BLK_FLAG_PST                    (NandStorageInfo.OptPhyOpPar.BadBlockFlagPosition)

#define SYNC_CHIP_MODE                      0x00

#define SYNC_BANK_MODE                      0x01

#define PHY_TMP_PAGE_CACHE                  (PageCachePool.PageCache0)

#define PHY_TMP_SPARE_CACHE                 (PageCachePool.SpareCache)

/*
************************************************************************************************************************
*                       INIT NAND FLASH DRIVER PHYSICAL MODULE
*
* Description: init nand flash driver physical module.
*
* Aguments   : none
*
* Returns    : the resutl of initial.
*                   = 0     initiate successful;
*                   = -1    initiate failed.
************************************************************************************************************************
*/
__s32 PHY_Init(void);
__s32 PHY_ChangeMode(__u8 serial_mode);

/*
************************************************************************************************************************
*                       NAND FLASH DRIVER PHYSICAL MODULE EXIT
*
* Description: nand flash driver physical module exit.
*
* Aguments   : none
*
* Returns    : the resutl of exit.
*                   = 0     exit successful;
*                   = -1    exit failed.
************************************************************************************************************************
*/
__s32 PHY_Exit(void);


/*
************************************************************************************************************************
*                       RESET ONE NAND FLASH CHIP
*
*Description: Reset the given nand chip;
*
*Arguments  : nChip     the chip select number, which need be reset.
*
*Return     : the result of chip reset;
*               = 0     reset nand chip successful;
*               = -1    reset nand chip failed.
************************************************************************************************************************
*/
__s32 PHY_ResetChip(__u32 nChip);


/*
************************************************************************************************************************
*                       READ NAND FLASH ID
*
*Description: Read nand flash ID from the given nand chip.
*
*Arguments  : nChip         the chip number whoes ID need be read;
*             pChipID       the po__s32er to the chip ID buffer.
*
*Return     : read nand chip ID result;
*               = 0     read chip ID successful, the chip ID has been stored in given buffer;
*               = -1    read chip ID failed.
************************************************************************************************************************
*/
__s32 PHY_ReadNandId(__s32 nChip, void *pChipID);
__s32 PHY_ReadNandUniqueId(__s32 bank, void *pChipID);


/*
************************************************************************************************************************
*                       CHECK WRITE PROTECT STATUS
*
*Description: check the status of write protect.
*
*Arguments  : nChip     the number of chip, which nand chip need be checked.
*
*Return     : the result of status check;
*             = 0       the nand flash is not write proteced;
*             = 1       the nand flash is write proteced;
*             = -1      check status failed.
************************************************************************************************************************
*/
__s32 PHY_CheckWp(__u32 nChip);


/*
************************************************************************************************************************
*                           PHYSICAL BLOCK ERASE
*
*Description: Erase one nand flash physical block.
*
*Arguments  : pBlkAdr   the parameter of the physical block which need be erased.
*
*Return     : the result of the block erase;
*               = 0     erase physical block successful;
*               = -1    erase physical block failed.
************************************************************************************************************************
*/
__s32 PHY_BlockErase(struct __PhysicOpPara_t *pBlkAdr);


/*
************************************************************************************************************************
*                       READ NAND FLASH PHYSICAL PAGE DATA
*
*Description: Read a page from a nand flash physical page to buffer.
*
*Arguments  : pPageAdr      the po__s32er to the accessed page parameter.
*
*Return     : the result of physical page read;
*               = 0     read physical page successful;
*               > 0     read physical page successful, but need do some process;
*               < 0     read physical page failed.
************************************************************************************************************************
*/
__s32 PHY_PageRead(struct __PhysicOpPara_t *pPageAdr);


__s32  PHY_PageReadSpare(struct __PhysicOpPara_t *pPageAdr);

/*
************************************************************************************************************************
*                       WRITE NAND FLASH PHYSICAL PAGE DATA
*
*Description: Write a page from buffer to a nand flash physical page.
*
*Arguments  : pPageAdr      the po__s32er to the accessed page parameter.
*
*Return     : The result of the page write;
*               = 0     page write successful;
*               > 0     page write successful, but need do some process;
*               < 0     page write failed.
************************************************************************************************************************
*/
__s32 PHY_PageWrite(struct __PhysicOpPara_t  *pPageAdr);


/*
************************************************************************************************************************
*                           PHYSIC PAGE COPY-BACK
*
*Description: copy one physical page from one physical block to another physical block.
*
*Arguments  : pSrcPage      the parameter of the source page which need be copied;
*             pDstPage      the parameter of the destination page which copied to.
*
*Return     : the result of the page copy-back;
*               = 0         page copy-back successful;
*               = -1        page copy-back failed.
************************************************************************************************************************
*/
__s32 PHY_PageCopyback(struct __PhysicOpPara_t *pSrcPage, struct __PhysicOpPara_t *pDstPage);


/*
************************************************************************************************************************
*                       SYNCH NAND FLASH PHYSIC OPERATION
*
*Description: Synch nand flash operation, check nand flash program/erase operation status.
*
*Arguments  : nBank     the number of the bank which need be synchronized;
*             bMode     the type of synch,
*                       = 0     synch the chip which the bank belonged to, wait the whole chip
*                               to be ready, and report status. if the chip support cacheprogram,
*                               need check if the chip is true ready;
*                       = 1     only synch the the bank, wait the bank ready and report the status,
*                               if the chip support cache program, need not check if the cache is
*                               true ready.
*
*Return     : the result of synch;
*               = 0     synch nand flash successful, nand operation ok;
*               = -1    synch nand flash failed.
************************************************************************************************************************
*/
__s32 PHY_SynchBank(__u32 nBank, __u32 bMode);


__s32 PHY_GetDefaultParam(__u32 bank);
__s32 PHY_SetDefaultParam(__u32 bank);

__s32 PHY_ScanDDRParam(void);

#endif 




