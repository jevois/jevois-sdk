/*
 * Copyright (C) 2013 Allwinnertech, kevin.z.m <kevin@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "../include/nand_format.h"
#include "../include/nand_logic.h"

#define DBG_DUMP_DIE_INFO       (1)
#define LOG_BLOCK_ECC_CHECK
#define LOG_AGE_SEQ_CHECK

extern  struct __NandDriverGlobal_t     NandDriverInfo;
extern  struct __LogicArchitecture_t    LogicArchiPar;
extern  struct __ZoneTblPstInfo_t       ZoneTblPstInfo[MAX_ZONE_CNT];

extern  struct __NandStorageInfo_t      NandStorageInfo;
extern  struct __NandPageCachePool_t    PageCachePool;
extern  struct __NandPartInfo_t         NandPartInfo[NAND_MAX_PART_CNT];


__u32 DieCntOfNand = 0;         
static __u32 SuperBlkCntOfDie = 0;     

blk_for_boot1_t blks_array[ ] = {
	{ 16,  4, 16 },
	{ 32,  4, 8 },
	{ 128, 4, 3 },
	{ 256, 4, 3 },
	{ 512, 4, 3 },
	{ 0xffffffff,   4, 3 },
};
__u32 DIE0_FIRST_BLK_NUM=0;

__u8 *lsb_page=NULL;

static void _LSBPageTypeTabInit(void)
{
	__s32 i;

	FORMAT_DBG("Request memory for lsb page table \n");
	lsb_page = (__u8 *)MALLOC(MAX_SUPER_PAGE_CNT);
	if(!lsb_page)
    {
        PRINT("[FORMAT_ERR] Request memory for lsb page table failed, size: %x !!", (MAX_SUPER_PAGE_CNT*2));
    }

	if(LOG_BLOCK_LSB_PAGE_TYPE == 0x0)
	{
		FORMAT_DBG("page type: %x\n", LOG_BLOCK_LSB_PAGE_TYPE);
		if((SUPPORT_EXT_INTERLEAVE)&&(NandStorageInfo.ChipCnt >=2))
		{
			for(i=0; i<PAGE_CNT_OF_PHY_BLK;i++)
			{
				if((i%4 == 2)||(i%4 == 3))
				{
					lsb_page[2*i] = 1;
					lsb_page[2*i+1] = 1;
				}
				else
				{
					lsb_page[2*i] = 0;
					lsb_page[2*i+1] = 0;
				}
			}
			lsb_page[0] = 1;
			lsb_page[1] = 1;
			lsb_page[2] = 1;
			lsb_page[3] = 1;
			lsb_page[2*(PAGE_CNT_OF_PHY_BLK-2)] = 0;
			lsb_page[2*(PAGE_CNT_OF_PHY_BLK-2)+1] = 0;
			lsb_page[2*(PAGE_CNT_OF_PHY_BLK-1)] = 0;
			lsb_page[2*(PAGE_CNT_OF_PHY_BLK-1)+1] = 0;
		}
		else
		{
			for(i=0; i<PAGE_CNT_OF_PHY_BLK;i++)
			{
				if((i%4 == 2)||(i%4 == 3))
					lsb_page[i] = 1;
				else
					lsb_page[i] = 0;
			}
			lsb_page[0] = 1;
			lsb_page[1] = 1;
			lsb_page[PAGE_CNT_OF_PHY_BLK-2] = 0;
			lsb_page[PAGE_CNT_OF_PHY_BLK-1] = 0;
		}

	}
	else if(LOG_BLOCK_LSB_PAGE_TYPE == 0x1)
	{
		FORMAT_DBG("page type: %x\n", LOG_BLOCK_LSB_PAGE_TYPE);
		if((SUPPORT_EXT_INTERLEAVE)&&(NandStorageInfo.ChipCnt >=2))
		{
			for(i=0; i<PAGE_CNT_OF_PHY_BLK;i++)
			{
				if(i%2 == 1)
				{
					lsb_page[2*i] = 1;
					lsb_page[2*i+1] = 1;
				}
				else
				{
					lsb_page[2*i] = 0;
					lsb_page[2*i+1] = 0;
				}
			}
			lsb_page[0] = 1;
			lsb_page[1] = 1;
			lsb_page[2*(PAGE_CNT_OF_PHY_BLK-1)] = 0;
			lsb_page[2*(PAGE_CNT_OF_PHY_BLK-1)+1] = 0;
		}
		else
		{
			for(i=0; i<PAGE_CNT_OF_PHY_BLK;i++)
			{
				if(i%2 == 1)
					lsb_page[i] = 1;
				else
					lsb_page[i] = 0;
			}
			lsb_page[0] = 1;
			lsb_page[PAGE_CNT_OF_PHY_BLK-1] = 0;
		}

	}
	else if(LOG_BLOCK_LSB_PAGE_TYPE == 0x2)
	{
		FORMAT_DBG("page type: %x\n", LOG_BLOCK_LSB_PAGE_TYPE);
		if((SUPPORT_EXT_INTERLEAVE)&&(NandStorageInfo.ChipCnt >=2))
		{
			for(i=0; i<PAGE_CNT_OF_PHY_BLK;i++)
			{
				if((i%4 == 0)||(i%4 == 1))
				{
					lsb_page[2*i] = 1;
					lsb_page[2*i+1] = 1;
				}
				else
				{
					lsb_page[2*i] = 0;
					lsb_page[2*i+1] = 0;
				}
			}

			for(i=0;i<6;i++)
			{
				lsb_page[2*i] = 1;
				lsb_page[2*i+1] = 1;
				lsb_page[2*(PAGE_CNT_OF_PHY_BLK-1-i)] = 0;
				lsb_page[2*(PAGE_CNT_OF_PHY_BLK-1-i)+1] = 0;
			}
		}
		else
		{
			for(i=0; i<PAGE_CNT_OF_PHY_BLK;i++)
			{
				if((i%4 == 0)||(i%4 == 1))
					lsb_page[i] = 1;
				else
					lsb_page[i] = 0;
			}

			for(i=0;i<6;i++)
			{
				lsb_page[i] = 1;
				lsb_page[PAGE_CNT_OF_PHY_BLK-1-i] = 0;
			}

		}

	}

	DBUG_MSG("[NAND] LSB table:");
	for(i=0; i<PAGE_CNT_OF_LOGIC_BLK;i++)
	{
		if(lsb_page[i] == 1)
			DBUG_MSG(" 0x%x \n", i);
	}

	FORMAT_DBG("Init lsb page table ok\n");
}

/*
************************************************************************************************************************
*                       CALCULATE PHYSICAL OPERATION PARAMETER
*
*Description: Calculate the paramter for physical operation with the number of zone, number of
*             super block and number of page in the super block.
*
*Arguments  : pPhyPar   the pointer to the physical operation parameter;
*             nZone     the number of the zone which the super block blonged to;
*             nBlock    the number of the super block;
*             nPage     the number of the super page in the super block.
*
*Return     : calculate parameter result;
*               = 0     calculate parameter successful;
*               < 0     calcualte parameter failed.
************************************************************************************************************************
*/
#if(0)
__s32 _CalculatePhyOpPar(struct __PhysicOpPara_t *pPhyPar, __u32 nZone, __u32 nBlock, __u32 nPage)
{


    __u32   tmpDieNum, tmpBnkNum, tmpBlkNum, tmpPageNum;


    tmpDieNum = nZone / ZONE_CNT_OF_DIE;

    if(SUPPORT_INT_INTERLEAVE && SUPPORT_EXT_INTERLEAVE)
    {
        tmpBnkNum = nPage % INTERLEAVE_BANK_CNT;
        tmpBlkNum = nBlock;
        tmpPageNum = nPage / INTERLEAVE_BANK_CNT;
    }

    else if(SUPPORT_INT_INTERLEAVE && !SUPPORT_EXT_INTERLEAVE)
    {
        tmpBnkNum = (nPage % INTERLEAVE_BANK_CNT) + (tmpDieNum * INTERLEAVE_BANK_CNT);
        tmpBlkNum = nBlock;
        tmpPageNum = nPage / INTERLEAVE_BANK_CNT;
    }

    else if(!SUPPORT_INT_INTERLEAVE && SUPPORT_EXT_INTERLEAVE)
    {
        tmpBnkNum = nPage % INTERLEAVE_BANK_CNT;
        tmpBlkNum = nBlock + (tmpDieNum * (BLOCK_CNT_OF_DIE / PLANE_CNT_OF_DIE));
        tmpPageNum = nPage / INTERLEAVE_BANK_CNT;
    }

    else//if(!SUPPORT_INT_INTERLEAVE && !SUPPORT_EXT_INTERLEAVE)
    {
        tmpBnkNum = tmpDieNum / DIE_CNT_OF_CHIP;
        tmpBlkNum = nBlock + (tmpDieNum % DIE_CNT_OF_CHIP) * (BLOCK_CNT_OF_DIE / PLANE_CNT_OF_DIE);
        tmpPageNum = nPage;
    }

    pPhyPar->BankNum = tmpBnkNum;
    pPhyPar->PageNum = tmpPageNum;
    pPhyPar->BlkNum = tmpBlkNum;


    return 0;
}
#elif(1)
__s32 _CalculatePhyOpPar(struct __PhysicOpPara_t *pPhyPar, __u32 nZone, __u32 nBlock, __u32 nPage)
{


    __u32   tmpDieNum, tmpBnkNum, tmpBlkNum, tmpPageNum;


    tmpDieNum = nZone / ZONE_CNT_OF_DIE;

    if(SUPPORT_INT_INTERLEAVE && SUPPORT_EXT_INTERLEAVE)
    {
        tmpBnkNum = (nPage % INTERLEAVE_BANK_CNT) + (tmpDieNum * INTERLEAVE_BANK_CNT);
        tmpBlkNum = nBlock ;
        tmpPageNum = nPage / INTERLEAVE_BANK_CNT;
    }

    else if(SUPPORT_INT_INTERLEAVE && !SUPPORT_EXT_INTERLEAVE)
    {
        tmpBnkNum = (nPage % INTERLEAVE_BANK_CNT) + (tmpDieNum * INTERLEAVE_BANK_CNT);
        tmpBlkNum = nBlock;
        tmpPageNum = nPage / INTERLEAVE_BANK_CNT;
    }

    else if(!SUPPORT_INT_INTERLEAVE && SUPPORT_EXT_INTERLEAVE)
    {
        tmpBnkNum =  (nPage % INTERLEAVE_BANK_CNT) + (tmpDieNum/DIE_CNT_OF_CHIP )*INTERLEAVE_BANK_CNT;
        tmpBlkNum = nBlock + ((tmpDieNum % DIE_CNT_OF_CHIP) * (BLOCK_CNT_OF_DIE / PLANE_CNT_OF_DIE));
        tmpPageNum = nPage / INTERLEAVE_BANK_CNT;
    }

    else//if(!SUPPORT_INT_INTERLEAVE && !SUPPORT_EXT_INTERLEAVE)
    {
        tmpBnkNum = tmpDieNum / DIE_CNT_OF_CHIP;
        tmpBlkNum = nBlock + (tmpDieNum % DIE_CNT_OF_CHIP) * (BLOCK_CNT_OF_DIE / PLANE_CNT_OF_DIE);
        tmpPageNum = nPage;
    }

    pPhyPar->BankNum = tmpBnkNum;
    pPhyPar->PageNum = tmpPageNum;
    pPhyPar->BlkNum = tmpBlkNum;


    return 0;
}

#endif


/*
************************************************************************************************************************
*                       READ PAGE DATA FROM VIRTUAL BLOCK
*
*Description: Read page data from virtual block, the block is composed by several physical block.
*             It is named super block too.
*
*Arguments  : nDieNum   the number of the DIE, which the page is belonged to;
*             nBlkNum   the number of the virtual block in the die;
*             nPage     the number of the page in the virtual block;
*             Bitmap    the bitmap of the sectors need access in the page;
*             pBuf      the pointer to the page data buffer;
*             pSpare    the pointer to the spare data buffer.
*
*Return     : read result;
*               = 0     read page data successful;
*               < 0     read page data failed.
************************************************************************************************************************
*/
static __s32 _VirtualPageRead(__u32 nDieNum, __u32 nBlkNum, __u32 nPage, __u64 SectBitmap, void *pBuf, void *pSpare)
{
    __s32 i, result;
    __u8  *tmpSrcData, *tmpDstData, *tmpSrcPtr[4], *tmpDstPtr[4];
    struct __PhysicOpPara_t tmpPhyPage;

    _CalculatePhyOpPar(&tmpPhyPage, nDieNum * ZONE_CNT_OF_DIE, nBlkNum, nPage);

    tmpPhyPage.SectBitmap = SectBitmap;
    tmpPhyPage.MDataPtr = pBuf;
    if(pSpare)
    {
        tmpPhyPage.SDataPtr = FORMAT_SPARE_BUF;

        for(i=0; i<2; i++)
        {
            if(SectBitmap & ((__u64)1<<i))
            {
                tmpSrcPtr[i] = FORMAT_SPARE_BUF + 4 * i;
                tmpDstPtr[i] = (__u8 *)pSpare + 4 * i;
            }
            else
            {
                tmpDstPtr[i] = NULL;
            }
        }

        for(i=0; i<2; i++)
        {
            if(SectBitmap & ((__u64)1<<(i + SECTOR_CNT_OF_SINGLE_PAGE)))
            {
                tmpSrcPtr[i+2] = LML_SPARE_BUF + 4 * (i + SECTOR_CNT_OF_SINGLE_PAGE);
                tmpDstPtr[i+2] = (__u8 *)pSpare + 8 + 4 * i;
            }
            else
            {
                tmpDstPtr[i+2] = NULL;
            }
        }
#if(1)
		result = PHY_PageReadSpare(&tmpPhyPage);
#else
		result = PHY_PageRead(&tmpPhyPage);
#endif
    }
    else
    {
        tmpPhyPage.SDataPtr = NULL;
		result = PHY_PageRead(&tmpPhyPage);
    }



    if(pSpare)
    {
        for(i=0; i<4; i++)
        {
            if(tmpDstPtr[i] != NULL)
            {
                tmpSrcData = tmpSrcPtr[i];
                tmpDstData = tmpDstPtr[i];

                *tmpDstData++ = *tmpSrcData++;
                *tmpDstData++ = *tmpSrcData++;
                *tmpDstData++ = *tmpSrcData++;
                *tmpDstData++ = *tmpSrcData++;
            }
        }
    }

	return result;

    #if 0
    if(result < 0)
    {
        return result;
    }
    else
    {
        return 0;
    }
	#endif
}


/*
************************************************************************************************************************
*                       WRITE PAGE DATA TO VIRTUAL BLOCK
*
*Description: Write page data to virtual block, the block is composed by several physical block.
*             It is named super block too.
*
*Arguments  : nDieNum   the number of the DIE, which the page is belonged to;
*             nBlkNum   the number of the virtual block in the die;
*             nPage     the number of the page in the virtual block;
*             Bitmap    the bitmap of the sectors need access in the page;
*             pBuf      the pointer to the page data buffer;
*             pSpare    the pointer to the spare data buffer.
*
*Return     : write result;
*               = 0     write page data successful;
*               < 0     write page data failed.
************************************************************************************************************************
*/
static __s32 _VirtualPageWrite(__u32 nDieNum, __u32 nBlkNum, __u32 nPage, __u64 SectBitmap, void *pBuf, void *pSpare)
{
    __s32 i, result;
    __u8  *tmpSrcData, *tmpDstData, *tmpSrcPtr[4], *tmpDstPtr[4];
    struct __PhysicOpPara_t tmpPhyPage;

    _CalculatePhyOpPar(&tmpPhyPage, nDieNum * ZONE_CNT_OF_DIE, nBlkNum, nPage);

    tmpPhyPage.SectBitmap = SectBitmap;
    tmpPhyPage.MDataPtr = pBuf;
    tmpPhyPage.SDataPtr = FORMAT_SPARE_BUF;

    if(pSpare)
    {
        for(i=0; i<2; i++)
        {
            if(SectBitmap & ((__u64)1<<i))
            {
                tmpSrcPtr[i] = (__u8 *)pSpare + 4 * i;
                tmpDstPtr[i] = FORMAT_SPARE_BUF + 4 * i;
            }
            else
            {
                tmpDstPtr[i] = NULL;
            }
        }

        for(i=0; i<2; i++)
        {
            if(SectBitmap & ((__u64)1<<(i + SECTOR_CNT_OF_SINGLE_PAGE)))
            {
                tmpSrcPtr[i+2] = (__u8 *)pSpare + 8 + 4 * i;
                tmpDstPtr[i+2] = LML_SPARE_BUF + 4 * (i + SECTOR_CNT_OF_SINGLE_PAGE);
            }
            else
            {
                tmpDstPtr[i+2] = NULL;
            }
        }

        MEMSET(FORMAT_SPARE_BUF, 0xff, SECTOR_CNT_OF_SUPER_PAGE * 4);

        for(i=0; i<4; i++)
        {
            tmpSrcData = tmpSrcPtr[i];
            tmpDstData = tmpDstPtr[i];

            if(tmpDstData != NULL)
            {
                *tmpDstData++ = *tmpSrcData++;
                *tmpDstData++ = *tmpSrcData++;
                *tmpDstData++ = *tmpSrcData++;
                *tmpDstData++ = *tmpSrcData++;
            }
        }
    }
    else
    {
        MEMSET(FORMAT_SPARE_BUF, 0xff, SECTOR_CNT_OF_SUPER_PAGE * 4);
    }

    PHY_PageWrite(&tmpPhyPage);
    result = PHY_SynchBank(tmpPhyPage.BankNum, SYNC_CHIP_MODE);
    if(result < 0)
    {
        return result;
    }
    else
    {
        return 0;
    }
}


/*
************************************************************************************************************************
*                       ERASE VIRTUAL BLOCK
*
*Description: Erase a virtual blcok.
*
*Arguments  : nDieNum   the number of the DIE, which the block is belonged to;
*             nBlkNum   the number of the virtual block in the die.
*
*Return     : erase result;
*               = 0     virtual block erase successful;
*               < 0     virtual block erase failed.
************************************************************************************************************************
*/
static __s32 _VirtualBlockErase(__u32 nDieNum, __u32 nBlkNum)
{
    __s32 i, result = 0;
    struct __PhysicOpPara_t tmpPhyBlk;

    for(i=0; i<INTERLEAVE_BANK_CNT; i++)
    {
        _CalculatePhyOpPar(&tmpPhyBlk, nDieNum * ZONE_CNT_OF_DIE, nBlkNum, i);

        PHY_BlockErase(&tmpPhyBlk);
    }

    for(i=0; i<INTERLEAVE_BANK_CNT; i++)
    {
        result = PHY_SynchBank(i, SYNC_CHIP_MODE);
        if(result < 0)
        {
            return -1;
        }
    }

    return 0;
}


/*
************************************************************************************************************************
*                       COPY ONE VIRTUAL BLOCK TO ANOTERH BLOCK
*
*Description: Copy the whole block data from one virtual block to another virtual, the
*             source block and the destination block should be in the same DIE.
*
*Arguments  : nDieNum   the number of the DIE, which the blocks belonged to;
*             nSrcBlk   the number of the source virtual block;
*             nDstBlk   the number of the destination virtual block.
*
*Return     : block copy result;
*               = 0     copy block data successful;
*               < 0     copy block data failed.
************************************************************************************************************************
*/
static __s32 _VirtualBlockCopy(__u32 nDieNum, __u32 nSrcBlk, __u32 nDstBlk)
{
    __s32 i, result = 0;
    struct __PhysicOpPara_t tmpSrcBlk, tmpDstBlk;

    for(i=0; i<PAGE_CNT_OF_SUPER_BLK; i++)
    {
        _CalculatePhyOpPar(&tmpSrcBlk, nDieNum * ZONE_CNT_OF_DIE, nSrcBlk, i);
        _CalculatePhyOpPar(&tmpDstBlk, nDieNum * ZONE_CNT_OF_DIE, nDstBlk, i);

        PHY_PageCopyback(&tmpSrcBlk, &tmpDstBlk);

        result = PHY_SynchBank(tmpDstBlk.BankNum, SYNC_CHIP_MODE);
        if(result < 0)
        {
            FORMAT_ERR("[FORMAT_ERR] Copy page write failed!\n");

            return -1;
        }
    }

    return 0;
}

static __s32 _VirtualLogBlkRepair(__u32 nDieNum, __u32 nSrcBlk, __u32 nDstBlk, __u32 last_page)
{
    __s32 i, ret, result = 0, page_cnt;
    struct __PhysicOpPara_t tmpSrcBlk, tmpDstBlk;
	struct __NandUserData_t *tmpSpare;

	tmpSpare = (struct __NandUserData_t *)PHY_TMP_SPARE_CACHE;

	PRINT(" _VirtualLogBlkRepair, srcblock: %x, dstblock: %x, lastpaqe: %x\n", nSrcBlk, nDstBlk, last_page);

    if(last_page<PAGE_CNT_OF_SUPER_BLK)
        page_cnt = last_page;
    else
        page_cnt = PAGE_CNT_OF_SUPER_BLK -1;

    for(i=0; i<=last_page; i++)
    {
		if(i>=PAGE_CNT_OF_SUPER_BLK)
			break;

        _CalculatePhyOpPar(&tmpSrcBlk, nDieNum * ZONE_CNT_OF_DIE, nSrcBlk, i);
        _CalculatePhyOpPar(&tmpDstBlk, nDieNum * ZONE_CNT_OF_DIE, nDstBlk, i);
		tmpSrcBlk.SectBitmap = tmpDstBlk.SectBitmap = FULL_BITMAP_OF_SUPER_PAGE;
		tmpSrcBlk.MDataPtr = tmpDstBlk.MDataPtr = PHY_TMP_PAGE_CACHE;
		tmpSrcBlk.SDataPtr = tmpDstBlk.SDataPtr = PHY_TMP_SPARE_CACHE;

		ret = PHY_PageRead(&tmpSrcBlk);
		if(ret == -ERR_ECC)
		{
			if(i == 0)
			{
				PRINT(" _VirtualLogBlkRepair, page 0 ecc error, write back page 0\n");
				tmpSpare->LogicPageNum = 0xffff;
			}
			else
			{
				PRINT(" _VirtualLogBlkRepair, skip page %x as ecc error page\n", i);
				continue;
			}

		}
		else if((tmpSpare->BadBlkFlag == 0xff)&&(tmpSpare->LogicInfo == 0xffff)\
			&&(tmpSpare->LogicPageNum == 0xffff)&&(tmpSpare->LogType == 0xff)\
			&&(tmpSpare->PageStatus== 0xff))
		{
			PRINT(" _VirtualLogBlkRepair, skip page %x as clear page\n", i);
			continue;
		}


		PHY_PageWrite(&tmpDstBlk);
        result = PHY_SynchBank(tmpDstBlk.BankNum, SYNC_CHIP_MODE);
        if(result < 0)
        {
            PRINT("_VirtualLogBlkRepair, write page %x error\n", i);
            FORMAT_ERR("[FORMAT_ERR] Copy page write failed!\n");

            return -1;
        }
    }

    return 0;
}

static __s32 _VirtualLogBlkEccCheck(__u32 nDieNum, __u32 nSrcBlk, __u32 logtype)
{
    __s32 i, ret, ecc_error_cnt = 0;
    struct __PhysicOpPara_t tmpSrcBlk;
	struct __NandUserData_t *tmpSpare;

	tmpSpare = (struct __NandUserData_t *)PHY_TMP_SPARE_CACHE;

	tmpSrcBlk.SectBitmap = 0x3;
	tmpSrcBlk.MDataPtr  = FORMAT_PAGE_BUF;
	tmpSrcBlk.SDataPtr  = (void *)(tmpSpare);

	if(SUPPORT_LOG_BLOCK_MANAGE&&(logtype == LSB_TYPE))
	{
	    for(i=PAGE_CNT_OF_SUPER_BLK - 1; i>=0; i--)
	    {
	    	if(lsb_page[i] == LSB_TYPE)
	    	{
		        _CalculatePhyOpPar(&tmpSrcBlk, nDieNum * ZONE_CNT_OF_DIE, nSrcBlk, i);

				ret = PHY_PageReadSpare(&tmpSrcBlk);
				if(ret == -ERR_ECC)
				{
					PRINT("[NAND_ECC_CHECK] page %x ecc error, checking block %x\n", i, nSrcBlk);
					ecc_error_cnt++;
					break;

				}
				else if((tmpSpare->LogicInfo!= 0xffff)||(tmpSpare->LogicPageNum != 0xffff)||(tmpSpare->PageStatus != 0xff))
				{
				}
				else if(ret<0)
				{
					PRINT("[NAND_ECC_CHECK] page %x read error, checking block %x\n", i, nSrcBlk);
				}

	    	}
	    #if 0
			else
			{
		        _CalculatePhyOpPar(&tmpSrcBlk, nDieNum * ZONE_CNT_OF_DIE, nSrcBlk, i);

				ret = PHY_PageRead(&tmpSrcBlk);
				if(ret == -ERR_ECC)
				{
					PRINT("[NAND_ECC_CHECK] page %x ecc error, checking block %x\n", i, nSrcBlk);
					ecc_error_cnt++;
					break;

				}
				else if((tmpSpare->LogicInfo!= 0xffff)||(tmpSpare->LogicPageNum != 0xffff)||(tmpSpare->PageStatus != 0xff))
				{
					PRINT("[NAND_ERR] msb page %x in block %x is not clear!\n", i, nSrcBlk);
					PRINT("LogicInfo: 0x%x, LogicPageNum: 0x%x, PageStatus: %x\n", tmpSpare->LogicInfo, tmpSpare->LogicPageNum, tmpSpare->PageStatus);
				}
				else if(ret<0)
				{
					PRINT("[NAND_ECC_CHECK] page %x read error, checking block %x\n", i, nSrcBlk);
				}
			}
			#endif

		}
    }
	else
	{
	    for(i=0; i<PAGE_CNT_OF_SUPER_BLK; i++)
	    {
	        _CalculatePhyOpPar(&tmpSrcBlk, nDieNum * ZONE_CNT_OF_DIE, nSrcBlk, i);

			ret = PHY_PageRead(&tmpSrcBlk);
			if(ret == -ERR_ECC)
			{
				PRINT("[NAND_ECC_CHECK] page %x ecc error, checking block %x\n", i, nSrcBlk);
				ecc_error_cnt++;

			}
			else if(ret<0)
			{
				PRINT("[NAND_ECC_CHECK] page %x read error, checking block %x\n", i, nSrcBlk);
			}
	    }
	}

    return ecc_error_cnt;
}

static __s32 _VirtualFreeBlockCheck(__u32 nDieNum, __u32 nSrcBlk)
{
    __s32 ret;
    struct __PhysicOpPara_t tmpSrcBlk;
	struct __NandUserData_t *tmpSpare;

	tmpSrcBlk.SectBitmap = FULL_BITMAP_OF_SUPER_PAGE;
	tmpSrcBlk.MDataPtr  = FORMAT_PAGE_BUF;
	tmpSrcBlk.SDataPtr  = (void *)(PHY_TMP_SPARE_CACHE);
	tmpSpare = (struct __NandUserData_t *)(PHY_TMP_SPARE_CACHE);

    _CalculatePhyOpPar(&tmpSrcBlk, nDieNum * ZONE_CNT_OF_DIE, nSrcBlk, 0);

	ret = PHY_PageReadSpare(&tmpSrcBlk);

	if((tmpSpare->BadBlkFlag!=0xff)||(tmpSpare->LogicInfo !=0xffff)||(tmpSpare->LogicPageNum!=0xffff)||(tmpSpare->LogType!=0xff)||(tmpSpare->PageStatus!=0xff))
	{
		PRINT("[NAND] Error, check free block %x fail!\n", nSrcBlk);
		PRINT("Error, block %x is not clear!\n", nSrcBlk);
		PRINT("bad flag: %x, logicinfo: %x, logicpagenum: %x\n", tmpSpare->BadBlkFlag, tmpSpare->LogicInfo, tmpSpare->LogicPageNum);
		PRINT("pagestatus: %x, logtype: %x\n", tmpSpare->PageStatus, tmpSpare->LogType);
		return -1;
	}
	else
	{
		PRINT("[DUBG], check free block %x ok!\n", nSrcBlk);
	}

    return 0;
}





/*
************************************************************************************************************************
*                       WRITE THE BAD BLOCK FLAG TO A VIRTUAL BLOCK
*
*Description: Write bad block flag to a virtual block, because there is some error happen when write
*             erase the block, because we don't know which single physical block in the virtual block
*             is bad, so, should write the bad block flag to every single physical block in the virtual
*             block, the bad block will be kicked out anyway.
*
*Arguments  : nDieNum   the number of the DIE, which the virtual block belonged to;
*             nBlock    the number of the virtual block in the die;
*
*Return     : write result;
*             return 0 always.
************************************************************************************************************************
*/
static __s32 _WriteBadBlkFlag(__u32 nDieNum, __u32 nBlock)
{
    __s32   i;
    struct __NandUserData_t tmpSpare[2];

    tmpSpare[0].BadBlkFlag = 0x00;
    tmpSpare[1].BadBlkFlag = 0x00;
    tmpSpare[0].LogicInfo = 0x00;
    tmpSpare[1].LogicInfo = 0x00;
    tmpSpare[0].LogicPageNum = 0x00;
    tmpSpare[1].LogicPageNum = 0x00;
    tmpSpare[0].PageStatus = 0x00;
    tmpSpare[1].PageStatus = 0x00;

    for(i=0; i<INTERLEAVE_BANK_CNT; i++)
    {
        _VirtualPageWrite(nDieNum, nBlock, i, FULL_BITMAP_OF_SUPER_PAGE, FORMAT_PAGE_BUF, (void *)&tmpSpare);

        _VirtualPageWrite(nDieNum, nBlock, PAGE_CNT_OF_SUPER_BLK - INTERLEAVE_BANK_CNT + i, \
                        FULL_BITMAP_OF_SUPER_PAGE, FORMAT_PAGE_BUF, (void *)&tmpSpare);
    }

    return 0;
}


#if DBG_DUMP_DIE_INFO

/*
************************************************************************************************************************
*                       DUMP DIE INFORMATION FOR DEBUG
*
*Description: Dump die information for debug nand format.
*
*Arguments  : pDieInfo   the pointer to the die information.
*
*Return     : none
************************************************************************************************************************
*/
static void _DumpDieInfo(struct __ScanDieInfo_t *pDieInfo)
{
    int tmpZone, tmpLog;
	struct __ScanZoneInfo_t *tmpZoneInfo;
	struct __LogBlkType_t   *tmpLogBlk;

    FORMAT_DBG("\n");
    FORMAT_DBG("[FORMAT_DBG] ================== Die information ================\n");
    FORMAT_DBG("[FORMAT_DBG]    Die number:         0x%x\n", pDieInfo->nDie);
    FORMAT_DBG("[FORMAT_DBG]    Super block count:  0x%x\n", SuperBlkCntOfDie);
    FORMAT_DBG("[FORMAT_DBG]    Free block count:   0x%x\n", pDieInfo->nFreeCnt);
    FORMAT_DBG("[FORMAT_DBG]    Bad block count:    0x%x\n", pDieInfo->nBadCnt);

    for(tmpZone=0; tmpZone<ZONE_CNT_OF_DIE; tmpZone++)
    {
        tmpZoneInfo = &pDieInfo->ZoneInfo[tmpZone];
        FORMAT_DBG("[FORMAT_DBG] ---------------------------------------------------\n");
        FORMAT_DBG("[FORMAT_DBG] ZoneNum:    0x%x\n", tmpZone);
        FORMAT_DBG("[FORMAT_DBG]    Data block Count:    0x%x\n", tmpZoneInfo->nDataBlkCnt);
        FORMAT_DBG("[FORMAT_DBG]    Free block Count:    0x%x\n", tmpZoneInfo->nFreeBlkCnt);
        FORMAT_DBG("[FORMAT_DBG]    Log block table: \n");
		FORMAT_DBG("       [Index]             [LogicalBlk]         [LogBlk]        [DataBlk]\n");
        for(tmpLog=0; tmpLog<MAX_LOG_BLK_CNT; tmpLog++)
        {
            tmpLogBlk = tmpLogBlk;
            tmpLogBlk = &tmpZoneInfo->LogBlkTbl[tmpLog];
            FORMAT_DBG("      %x           %x          %x        %x\n", tmpLog, tmpLogBlk->LogicBlkNum,
    		    tmpLogBlk->PhyBlk.PhyBlkNum,
    		    (tmpLogBlk->LogicBlkNum == 0xffff)? 0xffff : tmpZoneInfo->ZoneTbl[tmpLogBlk->LogicBlkNum].PhyBlkNum);
        }
    }

    FORMAT_DBG("[FORMAT_DBG] ===================================================\n");
}

#endif


/*
************************************************************************************************************************
*                           GET FREE BLOCK FROM DIE INFORMATION
*
*Description: Get free block from the die information.
*
*Arguments  : pDieInfo  the pointer to the die information;
*
*Return     : the free block number;
*               >= 0    get free block successful, return the block number;
*               < 0     get free block failed.
************************************************************************************************************************
*/
static __s32 _GetFreeBlkFromDieInfo(struct __ScanDieInfo_t *pDieInfo)
{
    __s32   i, tmpFreeBlk;

    tmpFreeBlk = -1;

    for(i=pDieInfo->nFreeIndex; i<SuperBlkCntOfDie; i++)
    {
        if(pDieInfo->pPhyBlk[i] == FREE_BLOCK_INFO)
        {
            tmpFreeBlk = i;

            pDieInfo->pPhyBlk[i] = NULL_BLOCK_INFO;

            pDieInfo->nFreeIndex = i;

            pDieInfo->nFreeCnt--;

            break;
        }
    }

    return tmpFreeBlk;
}


/*
************************************************************************************************************************
*                           GET THE ZONE NUMBEB WHICH HAS LEAST BLOCK
*
*Description: Get the number of zone, which has the least count blocks, include data block, free
*             block and log block; assure the blocks is proportioned in every block mapping table.
*
*Arguments  : pDieInfo  the pointer to the die information;
*
*Return     : the number of the zone which has least blocks;
************************************************************************************************************************
*/
static __s32 _LeastBlkCntZone(struct __ScanDieInfo_t *pDieInfo)
{
    __s32 i, tmpZone, tmpBlkCnt, tmpLeastCntBlkZone = 0, tmpLeastBlkCnt = 0xffff;
    struct __LogBlkType_t *tmpLogBlkTbl;

    for(tmpZone=0; tmpZone<ZONE_CNT_OF_DIE; tmpZone++)
    {
        if(pDieInfo->TblBitmap & (1 << tmpZone))
        {
            continue;
        }

        tmpBlkCnt = DATA_BLK_CNT_OF_ZONE + pDieInfo->ZoneInfo[tmpZone].nFreeBlkCnt;

        tmpLogBlkTbl = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl;
        for(i=0; i<MAX_LOG_BLK_CNT; i++)
        {
            if(tmpLogBlkTbl[i].LogicBlkNum != 0xffff)
            {
            	if(tmpLogBlkTbl[i].LogBlkType == LSB_TYPE)
					tmpBlkCnt += 2;
				else
                    tmpBlkCnt++;
            }
        }

        if(tmpBlkCnt < tmpLeastBlkCnt)
        {
            tmpLeastBlkCnt = tmpBlkCnt;
            tmpLeastCntBlkZone = tmpZone;
        }
    }

    return tmpLeastCntBlkZone;
}


/*
************************************************************************************************************************
*                           MERGE DATA BLOCK TO FREE BLOCK
*
*Description: Merge a data block to a free block, for block replace.
*
*Arguments  : pDieInfo  the pointer to the die information;
*             nDataBlk  the number of the data block;
*             nFreeBlk  the number of the free block.
*
*Return     : merge result;
*               = 0     merge successful;
*               < 0     merge failed.
************************************************************************************************************************
*/
static __s32 _MergeDataBlkToFreeBlk(struct __ScanDieInfo_t *pDieInfo, __u32 nDataBlk, __u32 nFreeBlk)
{
    __s32 result;

    result = _VirtualBlockCopy(pDieInfo->nDie, nDataBlk, nFreeBlk);
    if(result < 0)
    {
        _WriteBadBlkFlag(pDieInfo->nDie, nFreeBlk);

        return -1;
    }

    pDieInfo->pPhyBlk[nFreeBlk] = pDieInfo->pPhyBlk[nDataBlk];
    pDieInfo->ZoneInfo[GET_LOGIC_INFO_ZONE(pDieInfo-> \
            pPhyBlk[nDataBlk])].ZoneTbl[GET_LOGIC_INFO_BLK(pDieInfo->pPhyBlk[nDataBlk])].PhyBlkNum = nFreeBlk;

    result = _VirtualBlockErase(pDieInfo->nDie, nDataBlk);
    if(result < 0)
    {
        _WriteBadBlkFlag(pDieInfo->nDie, nDataBlk);

        return -1;
    }

    return 0;
}


/*
************************************************************************************************************************
*                       LOOK FOR THE BLOCK MAPPING TABLE POSITION
*
*Description: Look for the block-mapping-table position in the virtual block.
*
*Arguments  : nDieNum   the number of the DIE, which the virtual block belonged to;
*             nBlock    the number of the virtual block in the die;
*
*Return     : the page number which is the block-mapping-table position;
*               >=0     find the last group;
*               < 0     look for the last group failed.
************************************************************************************************************************
*/
static __u32 _SearchBlkTblPst(__u32 nDieNum, __u32 nBlock)
{
    struct __NandUserData_t tmpSpare;

    __s32   tmpLowPage, tmpHighPage, tmpMidPage;
    __u32   tmpPage;

    tmpLowPage = 0;
    tmpHighPage = PAGE_CNT_OF_SUPER_BLK / PAGE_CNT_OF_TBL_GROUP - 1;
    while(tmpLowPage <= tmpHighPage)
    {
        tmpMidPage = (tmpLowPage + tmpHighPage) / 2;
        tmpPage = tmpMidPage * PAGE_CNT_OF_TBL_GROUP;

        _VirtualPageRead(nDieNum, nBlock, tmpPage, 0x3, FORMAT_PAGE_BUF, (void *)&tmpSpare);

        if(tmpSpare.PageStatus == FREE_PAGE_MARK)
        {
            tmpHighPage = tmpMidPage - 1;
        }
        else
        {
            tmpLowPage = tmpMidPage + 1;
        }
    }

    tmpPage = ((tmpLowPage + tmpHighPage) / 2) * PAGE_CNT_OF_TBL_GROUP;

    return tmpPage;
}


/*
************************************************************************************************************************
*                       LOOK FOR THE LAST USED PAGE IN A SUPER BLOCK
*
*Description: Look for the last used page, which is the last page page in the used group.
*
*Arguments  : nDieNum   the number of the DIE, which the virtual block belonged to;
*             nBlock    the number of the virtual block in the die;
*
*Return     : the page number of the last page in the used group;
*               >= 0    find the last used page;
*               <  0    look for the last used page failed.
************************************************************************************************************************
*/
static __s32 _GetLastUsedPage(__u32 nDieNum, __u32 nBlock, __u32 log_type)
{
    __s32   tmpLowPage, tmpHighPage, tmpMidPage, tmpPage, tmpUsedPage = 0;
    struct __NandUserData_t tmpSpare;

    if(SUPPORT_LOG_BLOCK_MANAGE&&(log_type==LSB_TYPE))
    {
    	tmpPage = PAGE_CNT_OF_SUPER_BLK - 1;
		while(tmpPage)
		{
			if(lsb_page[tmpPage] == LSB_TYPE)
			{
				_VirtualPageRead(nDieNum, nBlock, tmpPage, 0x3, FORMAT_PAGE_BUF, (void *)&tmpSpare);
				if((tmpSpare.LogicInfo != 0xffff) || (tmpSpare.LogicPageNum != 0xffff)||(tmpSpare.PageStatus!=0xff))
	            {
	                break;
	            }
			}

			tmpPage--;
		}

		#if 1
		{
			__u32 index;
			__u32 test_err_flag = 0;

			for(index = 1; index<=2; index++)
			{
				if((tmpPage+index)<PAGE_CNT_OF_SUPER_BLK)
				{
					_VirtualPageRead(nDieNum, nBlock, tmpPage+index, 0x3, FORMAT_PAGE_BUF, (void *)&tmpSpare);
					if((tmpSpare.LogicInfo != 0xffff) || (tmpSpare.LogicPageNum != 0xffff)||(tmpSpare.PageStatus!=0xff))
		            {
		                PRINT("[NAND_ERR] lastpage + %d is not a clear page, lastpage: %x\n",index, tmpPage);
						PRINT("LogicInfo: 0x%x, LogicPageNum: 0x%x, PageStatus: %x\n", tmpSpare.LogicInfo, tmpSpare.LogicPageNum, tmpSpare.PageStatus);
						test_err_flag = 1;
		            }
			    }
			}

			if(test_err_flag)
			{
				tmpPage = PAGE_CNT_OF_SUPER_BLK - 1;
				while(tmpPage)
				{
					_VirtualPageRead(nDieNum, nBlock, tmpPage, 0x3, FORMAT_PAGE_BUF, (void *)&tmpSpare);
					if((tmpSpare.LogicInfo != 0xffff) || (tmpSpare.LogicPageNum != 0xffff)||(tmpSpare.PageStatus!=0xff))
		            {
		                break;
		            }

					tmpPage--;
				}
			}

		}
		#endif


		tmpUsedPage = tmpPage;
    }
	else
	{
		if(SUPPORT_ALIGN_NAND_BNK)
	    {
	        __u32   tmpBnkNum;
	        __u8    tmpPageStatus;

	        tmpLowPage = 0;
	        tmpHighPage = PAGE_CNT_OF_PHY_BLK - 1;

	        while(tmpLowPage <= tmpHighPage)
	        {
	            tmpPageStatus = FREE_PAGE_MARK;
	            tmpMidPage = (tmpLowPage + tmpHighPage) / 2;

	            for(tmpBnkNum=0; tmpBnkNum<INTERLEAVE_BANK_CNT; tmpBnkNum++)
	            {
	                tmpPage = tmpMidPage * INTERLEAVE_BANK_CNT + tmpBnkNum;
	                _VirtualPageRead(nDieNum, nBlock, tmpPage, 0x3, FORMAT_PAGE_BUF, (void *)&tmpSpare);

	                if((tmpSpare.PageStatus == FREE_PAGE_MARK) && (tmpSpare.LogicPageNum == 0xffff))
	                {
	                    continue;
	                }

	                tmpPageStatus &= DATA_PAGE_MARK;
	                tmpUsedPage = tmpPage;
	            }

	            if(tmpPageStatus == FREE_PAGE_MARK)
	            {
	                tmpHighPage = tmpMidPage - 1;
	            }
	            else
	            {
	                tmpLowPage = tmpMidPage + 1;
	            }
	        }

	    }
	    else
	    {

	        tmpLowPage = 0;
	        tmpHighPage = PAGE_CNT_OF_SUPER_BLK - 1;

	        while(tmpLowPage <= tmpHighPage)
	        {
	            tmpMidPage = (tmpLowPage + tmpHighPage) / 2;

	            _VirtualPageRead(nDieNum, nBlock, tmpMidPage, 0x3, FORMAT_PAGE_BUF, (void *)&tmpSpare);

	            if((tmpSpare.PageStatus == FREE_PAGE_MARK) && (tmpSpare.LogicPageNum == 0xffff))
	            {
	                tmpHighPage = tmpMidPage - 1;
	            }
	            else
	            {
	                tmpLowPage = tmpMidPage + 1;
					tmpUsedPage = tmpMidPage;
	            }
	        }

	    }
	}



    return tmpUsedPage;
}


/*
************************************************************************************************************************
*                       CALCULATE THE CHECKSUM FOR A MAPPING TABLE
*
*Description: Calculate the checksum for a mapping table, based on word.
*
*Arguments  : pTblBuf   the pointer to the table data buffer;
*             nLength   the size of the table data, based on word.
*
*Return     : table checksum;
************************************************************************************************************************
*/
static __u32 _CalCheckSum(__u32 *pTblBuf, __u32 nLength)
{
    __s32 i;
    __u32 tmpCheckSum = 0;
    __u32 *tmpItem = pTblBuf;

    for(i=0; i<nLength; i++)
    {
        tmpCheckSum += *tmpItem;
        tmpItem++;
    }

    return tmpCheckSum;
}


/*
************************************************************************************************************************
*                       GET THE LOGICAL INFORMATION OF PHYSICAL BLOCKS
*
*Description: Get the logical information of every physial block of the die.
*
*Arguments  : pDieInfo   the pointer to the die information whose logical block information need be got.
*
*Return     : get logical information result;
*               = 0     get logical information successful;
*               < 0     get logical information failed.
************************************************************************************************************************
*/
static __s32 _GetBlkLogicInfo(struct __ScanDieInfo_t *pDieInfo)
{
    __u32   tmpBlkNum, tmpBnkNum, tmpPage, tmpBadFlag, result, tmpStartBlk;
    __s32   i;
    __s16   tmpPageNum[4];
    __u16   tmpLogicInfo;
    __u64   spare_bitmap;
    struct  __NandUserData_t tmpSpare[2];

	if(pDieInfo->nDie == 0)
    {
        tmpStartBlk = DIE0_FIRST_BLK_NUM;
    }
    else
    {
        tmpStartBlk = 0;
    }

    tmpPageNum[0] = 0;
    tmpPageNum[1] = -1;
    tmpPageNum[2] = -1;
    tmpPageNum[3] = -1;

	tmpLogicInfo = 0xffff;

    switch(BAD_BLK_FLAG_PST & 0x03)
    {
        case 0x00:
            break;

        case 0x01:
            tmpPageNum[1] = 1;
            break;

        case 0x02:
            tmpPageNum[1] = PAGE_CNT_OF_PHY_BLK - 1;
            break;

        case 0x03:
            tmpPageNum[1] = PAGE_CNT_OF_PHY_BLK - 1;
            tmpPageNum[2] = PAGE_CNT_OF_PHY_BLK - 2;
            break;
    }

    for(tmpBlkNum=0; tmpBlkNum<SuperBlkCntOfDie; tmpBlkNum++)
    {
        tmpBadFlag = 0;

		if(tmpBlkNum<tmpStartBlk)
		{
			tmpBadFlag = 1;
			continue;
		}

        for(tmpBnkNum=0; tmpBnkNum<INTERLEAVE_BANK_CNT; tmpBnkNum++)
        {
            for(i=3; i>=0; i--)
            {
                if(tmpPageNum[i] == -1)
                {
                    continue;
                }

                tmpPage = tmpPageNum[i] * INTERLEAVE_BANK_CNT + tmpBnkNum;
                spare_bitmap = (SUPPORT_MULTI_PROGRAM ? ((__u64)0x3 | ((__u64)0x3 << SECTOR_CNT_OF_SINGLE_PAGE)) : (__u64)0x3);
                result = _VirtualPageRead(pDieInfo->nDie, tmpBlkNum, tmpPage, spare_bitmap, FORMAT_PAGE_BUF, (void *)&tmpSpare);

				if(NandStorageInfo.NandChipId[0]==0x45)
				{
					if(result==-ERR_ECC)
					{
						__u8* temp_mdata_ptr0 = (__u8*)FORMAT_PAGE_BUF;
						__u8* temp_mdata_ptr1 = (__u8*)((__u32)FORMAT_PAGE_BUF + SECTOR_CNT_OF_SINGLE_PAGE*512);

						if(((*temp_mdata_ptr0==0x00) && (*(temp_mdata_ptr0+1)==0x00) && (*(temp_mdata_ptr0+2)==0x00) && (*(temp_mdata_ptr0+3)==0x00) && (*(temp_mdata_ptr0+4)==0x00) && (*(temp_mdata_ptr0+5)==0x00))
							||(SUPPORT_MULTI_PROGRAM && ((*temp_mdata_ptr1==0x00) && (*(temp_mdata_ptr1+1)==0x00) && (*(temp_mdata_ptr1+2)==0x00) && (*(temp_mdata_ptr1+3)==0x00) && (*(temp_mdata_ptr1+4)==0x00) && (*(temp_mdata_ptr1+5)==0x00))))
						{
							tmpBadFlag = 1;

						}
						else
						{
							if((tmpSpare[0].BadBlkFlag != 0xff) || (SUPPORT_MULTI_PROGRAM && (tmpSpare[1].BadBlkFlag != 0xff)))
							{
								tmpBadFlag = 1;

							}
						}


					}
					else
					{
						if((tmpSpare[0].BadBlkFlag != 0xff) || (SUPPORT_MULTI_PROGRAM && (tmpSpare[1].BadBlkFlag != 0xff)))
						{
							tmpBadFlag = 1;

						}
					}
				}
				else
				{
					if((tmpSpare[0].BadBlkFlag != 0xff) || (SUPPORT_MULTI_PROGRAM && (tmpSpare[1].BadBlkFlag != 0xff)))
	                {
	                    tmpBadFlag = 1;

	                }

				}
                if(tmpPage == 0)
                {
                    tmpLogicInfo = tmpSpare[0].LogicInfo;
                }
            }
        }

        if(tmpBadFlag == 1)
        {
            FORMAT_DBG("[FORMAT_DBG] Find a bad block (NO. 0x%x) in the Die 0x%x\n", tmpBlkNum, pDieInfo->nDie);
            pDieInfo->pPhyBlk[tmpBlkNum] = BAD_BLOCK_INFO;

            continue;
        }

        pDieInfo->pPhyBlk[tmpBlkNum] = tmpLogicInfo;
    }

    return 0;
}


/*
************************************************************************************************************************
*                       GET LOG AGE FROM PHYSICAL BLOCK
*
*Description: Get log age from physical block. the log age is stored in the spare area of the physical block.
*
*Arguments  : nDie      the number of the die which the physical block is belonged to;
*             nPhyBlk   the number of the physical block whose log age need be get.
*
*Return     : the log age of the physical block;
************************************************************************************************************************
*/
static __u8 _GetLogAge(__u32 nDie, __u16 nPhyBlk)
{
    __u8    tmpLogAge;
    struct __NandUserData_t tmpSpareData;

    _VirtualPageRead(nDie, nPhyBlk, 0, 0x3, FORMAT_PAGE_BUF, (void *)&tmpSpareData);

    tmpLogAge = tmpSpareData.PageStatus;

    return tmpLogAge;
}

static __u8 _GetLogType(__u32 nDie, __u16 nPhyBlk)
{
    __u8    tmpLogAge;
    struct __NandUserData_t tmpSpareData;

    _VirtualPageRead(nDie, nPhyBlk, 0, 0x3, FORMAT_PAGE_BUF, (void *)&tmpSpareData);

    tmpLogAge = tmpSpareData.LogType;

    return tmpLogAge;
}

static __s32 _GetEccFlag(__u32 nDie, __u16 nPhyBlk)
{
    __s32    ecc_flag;
    struct __NandUserData_t tmpSpareData;

    ecc_flag = _VirtualPageRead(nDie, nPhyBlk, 0, 0x3, FORMAT_PAGE_BUF, (void *)&tmpSpareData);

    return ecc_flag;
}


/*
************************************************************************************************************************
*               FILL A PHYSCIAL BLOCK TO THE BLOCK MAPPING TABLE
*
*Description: Fill a physical block to the block mapping table, one logical block may be related with
*             one data block and one log block at most. if the logical block contain a data and a log
*             block, check the age value to decide which block is the data block and which is the log block.
*
*Arguments  : pDieInfo      the pointer to the die information the physical block is belonged to;
*             pLogicInfo    the logical information of the super block;
*             nPhyBlk       the number of the physical block in the die;
*             pEraseBlk     the pointer to the block which need be erased;
*
*Return     : fill result;
*               = 0     fill block successful;
*               < 0     fill block failed.
************************************************************************************************************************
*/
static __s32 _FillBlkToZoneTbl(struct __ScanDieInfo_t *pDieInfo, __u16 nLogicInfo, __u16 nPhyBlk, __u32 *pEraseBlk)
{
    __s32   i, tmpLogPst;
    __u32   tmpZone, tmpLogicBlk;
    __u16   tmpDataBlk, tmpLogBlk, tmpNewBlk, tmpLogBlk0, tmpLogBlk2;
    __u8    tmpLogTypeData, tmpLogTypeNew;
    __u8    tmpAgeData, tmpAgeNew, tmpAge0, tmpAge2;
    __s32   tmpLastPageOfData, tmpLastPageOfLog, tmpLastPageOfNew, tmpLastPage0, tmpLastPage2;
	  __s32   tmpDatalogflag = 0, tmpnewlogflag = 0;
	  __s32   tmpDataEcc = 0, tmpNewEcc=0;
    struct __SuperPhyBlkType_t *tmpSuperBlk;
    __u32   eraseblockcnt = 0;

    tmpZone = GET_LOGIC_INFO_ZONE(nLogicInfo);
    tmpLogicBlk = GET_LOGIC_INFO_BLK(nLogicInfo);
    tmpSuperBlk = (struct __SuperPhyBlkType_t *)&pDieInfo->ZoneInfo[tmpZone].ZoneTbl[tmpLogicBlk];

		eraseblockcnt = 0;
    pEraseBlk[0] = 0xffff;
    pEraseBlk[1] = 0xffff;
    pEraseBlk[2] = 0xffff;
    pEraseBlk[3] = 0xffff;


    if(tmpSuperBlk->PhyBlkNum == 0xffff)
    {
        tmpSuperBlk->PhyBlkNum = nPhyBlk;
        tmpSuperBlk->BlkEraseCnt = 0;
        pDieInfo->ZoneInfo[tmpZone].nDataBlkCnt++;

        return 0;
    }

    tmpDataBlk = tmpSuperBlk->PhyBlkNum;
    tmpNewBlk = nPhyBlk;
    tmpLogBlk = 0xffff;
    tmpAgeData = _GetLogAge(pDieInfo->nDie, tmpDataBlk);
    tmpAgeNew = _GetLogAge(pDieInfo->nDie, tmpNewBlk);
    tmpLogPst = -1;

    for(i=0; i<MAX_LOG_BLK_CNT; i++)
    {
        if(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[i].LogicBlkNum == tmpLogicBlk)
        {
            tmpLogBlk = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[i].PhyBlk.PhyBlkNum;
            tmpLogPst = i;
            break;
        }
    }

	if(SUPPORT_LOG_BLOCK_MANAGE)
	{
		  tmpLogTypeData = _GetLogType(pDieInfo->nDie, tmpDataBlk);
    	tmpLogTypeNew = _GetLogType(pDieInfo->nDie, tmpNewBlk);
	}
	else
	{
		tmpLogTypeData = 0xff;
		tmpLogTypeNew = 0xff;
	}


	if(((tmpLogTypeData&0xf) == LSB_TYPE)||((tmpLogTypeNew&0xf) == LSB_TYPE)||((tmpLogPst!= -1)&&(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].LogBlkType == LSB_TYPE)))
	{
    	if(tmpLogPst == -1)
    	{
            for(i=0; i<MAX_LOG_BLK_CNT; i++)
            {
                if(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[i].LogicBlkNum == 0xffff)
                {
                    tmpLogPst = i;
    				pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].LogicBlkNum = GET_LOGIC_INFO_BLK(nLogicInfo);
                    break;
                }
            }

            if(tmpLogPst == -1)
            {
                PRINT("[NNAD] Error, log table full!\n");
                return -1;
            }
    	}

    	pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].LogBlkType = LSB_TYPE;

		if(((tmpLogTypeNew&0xf) == LSB_TYPE)&&((tmpLogTypeData&0xf) != LSB_TYPE))
		{
			/*the new block is log block */
			tmpnewlogflag = LSB_TYPE;
		}
		else if(((tmpLogTypeNew&0xf) != LSB_TYPE)&&((tmpLogTypeData&0xf) == LSB_TYPE))
		{
			/*the new block is data block, the data block is log block */
			tmpSuperBlk->PhyBlkNum = tmpNewBlk;
			tmpDatalogflag = LSB_TYPE;
		}
		else if(((tmpLogTypeNew&0xf) == LSB_TYPE)&&((tmpLogTypeData&0xf) == LSB_TYPE))
		{
			/*the new block & data block are both log block */
			tmpSuperBlk->PhyBlkNum = 0xffff;
	        tmpSuperBlk->BlkEraseCnt = 0xffff;
	        pDieInfo->ZoneInfo[tmpZone].nDataBlkCnt--;

			tmpnewlogflag = LSB_TYPE;
			tmpDatalogflag = LSB_TYPE;
		}
		else
		{
			tmpDataEcc = _GetEccFlag(pDieInfo->nDie, tmpDataBlk);
			tmpNewEcc = _GetEccFlag(pDieInfo->nDie, tmpNewBlk);

			if((tmpDataEcc>=0)&&(tmpNewEcc<0))
			{
				*pEraseBlk = tmpNewBlk;
				PRINT(" two data block, age: tmpData: %x, tmpAgeNew: %x \n", tmpAgeData, tmpAgeNew);
				PRINT(" new block ecc error, Erase new block %x \n", tmpNewBlk);
			}
			else if((tmpNewEcc>=0)&&(tmpDataEcc<0))
			{
				tmpSuperBlk->PhyBlkNum = tmpNewBlk;
	            *pEraseBlk = tmpDataBlk;
				PRINT(" two data block, age: tmpData: %x, tmpAgeNew: %x \n", tmpAgeData, tmpAgeNew);
				PRINT(" data block ecc error, Erase data block  \n", tmpDataBlk);
			}
			else
			{
				if((tmpAgeData&0xff) == ((tmpAgeNew+2)&0xff))
				{
					tmpSuperBlk->PhyBlkNum = tmpNewBlk;
		            *pEraseBlk = tmpDataBlk;
					PRINT(" two data block 0, age: tmpData: %x, tmpAgeNew: %x \n", tmpAgeData, tmpAgeNew);
					PRINT(" two data block 0, Erase data block %x with big age \n", tmpDataBlk);
				}
				else if(((tmpAgeData+2)&0xff) ==(tmpAgeNew&0xff))
				{
		            *pEraseBlk = tmpNewBlk;
					PRINT(" two data block 1, age: tmpData: %x, tmpAgeNew: %x \n", tmpAgeData, tmpAgeNew);
					PRINT(" two data block 1, Erase new block %x with big age \n", tmpNewBlk);
				}
				else
				{
					PRINT("[NAND] Error, two data block with diffrent age: tmpData: %x, tmpAgeNew: %x \n", tmpAgeData, tmpAgeNew);
					PRINT("logicnum: %x, data block: %x, new block: %x\n", pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].LogicBlkNum, tmpDataBlk, tmpNewBlk);


					if(tmpAgeData>tmpAgeNew)
					{
						tmpSuperBlk->PhyBlkNum = tmpNewBlk;
			            *pEraseBlk = tmpDataBlk;
						PRINT(" Error, two data block, age not seq: tmpData: %x, tmpAgeNew: %x \n", tmpAgeData, tmpAgeNew);
						PRINT(" Error, two data block, Erase data block %x with big age \n", tmpDataBlk);
					}
					else if(tmpAgeData<tmpAgeNew)
					{
						*pEraseBlk = tmpNewBlk;
						PRINT(" Error, two data block, age: tmpData: %x, tmpAgeNew: %x \n", tmpAgeData, tmpAgeNew);
						PRINT(" Error, two data block, Erase new block %x with big age \n", tmpNewBlk);
					}
					else
					{
						/*the new block & data block are both data block */
						tmpLastPageOfData = _GetLastUsedPage(pDieInfo->nDie,tmpDataBlk, NORMAL_TYPE);
						tmpLastPageOfNew = _GetLastUsedPage(pDieInfo->nDie,tmpNewBlk, NORMAL_TYPE);
						if(tmpLastPageOfNew>tmpLastPageOfData)
						{
							tmpSuperBlk->PhyBlkNum = tmpNewBlk;
				            *pEraseBlk = tmpDataBlk;
							PRINT(" Error, two data block, Erase data block with less used page \n");
						}
						else
						{
							*pEraseBlk = tmpNewBlk;
							PRINT(" Error, two data block, Erase new block with less used page \n");
						}
					}

				}

			}

		}

		if(*pEraseBlk!=0xffff)
			eraseblockcnt++;

		if(tmpnewlogflag == LSB_TYPE)
		{
			if(((tmpLogTypeNew>>4)&0xf)==0)
			{
				if(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum == 0xffff)
				{
					pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum = tmpNewBlk;
				}
				else if(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum == 0xffff)
				{
					pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum = tmpNewBlk;
				}
				else
				{
					PRINT("[NAND] Error, tmpnewlogflag, log block0 full!\n");
					if(_GetEccFlag(pDieInfo->nDie, pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum)<0)
					{
						*(pEraseBlk + eraseblockcnt) = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum;
				  	eraseblockcnt++;
				  	pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum = tmpNewBlk;
					}
					else if(_GetEccFlag(pDieInfo->nDie, pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum)<0)
					{
						*(pEraseBlk + eraseblockcnt) = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum;
				  	eraseblockcnt++;
				  	pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum = tmpNewBlk;
					}
					else
					{
						*(pEraseBlk + eraseblockcnt) = tmpNewBlk;
				  	eraseblockcnt++;
					}

				}

			}
			else 
			{
				if(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk1.PhyBlkNum == 0xffff)
				{
					pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk1.PhyBlkNum = tmpNewBlk;
				}
				else
				{
					PRINT("[NAND] Error, tmpnewlogflag, log block1 full! tmpLogTypeNew: %x\n", tmpLogTypeNew);
					PRINT("[NAND] Error, logicblock: %x, old logblk1:%x, new log1: %x! tmpLogTypeNew: %x\n", pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk1.PhyBlkNum, tmpNewBlk);

					if(_GetEccFlag(pDieInfo->nDie, pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk1.PhyBlkNum)<0)
					{
						*(pEraseBlk + eraseblockcnt) = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk1.PhyBlkNum;
				  	eraseblockcnt++;
				  	pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk1.PhyBlkNum = tmpNewBlk;
					}
					else
					{
						*(pEraseBlk + eraseblockcnt) = tmpNewBlk;
				  	eraseblockcnt++;
					}

				}
			}
		}

		if(tmpDatalogflag == LSB_TYPE)
		{
			if(((tmpLogTypeData>>4)&0xf)==0)
			{
				if(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum == 0xffff)
					pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum = tmpDataBlk;
				else if(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum == 0xffff)
					pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum = tmpDataBlk;
				else
				{
					PRINT("[NAND] Error, tmpDatalogflag, log block0 full!\n");

					if(_GetEccFlag(pDieInfo->nDie, pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum)<0)
					{
						*(pEraseBlk + eraseblockcnt) = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum;
				  	eraseblockcnt++;
				  	pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum = tmpDataBlk;
					}
					else if(_GetEccFlag(pDieInfo->nDie, pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum)<0)
					{
						*(pEraseBlk + eraseblockcnt) = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum;
				  	eraseblockcnt++;
				  	pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum = tmpDataBlk;
					}
					else
					{
						*(pEraseBlk + eraseblockcnt) = tmpDataBlk;
				  	eraseblockcnt++;
					}

				}

			}
			else 
			{
				if(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk1.PhyBlkNum == 0xffff)
					pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk1.PhyBlkNum = tmpDataBlk;
				else
				{
					PRINT("[NAND] Error, tmpDatalogflag, log block1 full! tmpLogTypeData: 0x%x\n", tmpLogTypeData);
					if(_GetEccFlag(pDieInfo->nDie, pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk1.PhyBlkNum)<0)
					{
						*(pEraseBlk + eraseblockcnt) = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk1.PhyBlkNum;
				  	eraseblockcnt++;
				  	pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk1.PhyBlkNum = tmpDataBlk;
					}
					else
					{
						*(pEraseBlk + eraseblockcnt) = tmpDataBlk;
				  	eraseblockcnt++;
					}

				}
			}
		}

		if(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum != 0xffff)
		{
			tmpLogBlk0 = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum;
			tmpLogBlk2 = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum;
			PRINT("kick valid log block, tmpLogBlk0 %x, tmpLogBlk2 %x\n", tmpLogBlk0, tmpLogBlk2);

			if(_GetEccFlag(pDieInfo->nDie, tmpLogBlk2)<0)
			{
				pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum = 0xffff;
		    *(pEraseBlk + eraseblockcnt) = tmpLogBlk2;
				 eraseblockcnt++;
			}
			else if(_GetEccFlag(pDieInfo->nDie, tmpLogBlk0)<0)
			{
				pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum = tmpLogBlk2;
				pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum = 0xffff;
	      *(pEraseBlk + eraseblockcnt) = tmpLogBlk0;
			  eraseblockcnt++;
			}
			else
			{
				tmpAge0 = _GetLogAge(pDieInfo->nDie, tmpLogBlk0);
				tmpAge2 = _GetLogAge(pDieInfo->nDie, tmpLogBlk2);

				PRINT("kick valid log block, tmpAge0 %x, tmpAge2 %x\n", tmpAge0, tmpAge2);

				if(COMPARE_AGE(tmpAge0, tmpAge2) == 0) 
				{
					tmpLastPage0 = _GetLastUsedPage(pDieInfo->nDie, tmpLogBlk0, SUPPORT_LOG_BLOCK_MANAGE&LSB_TYPE);
					tmpLastPage2 = _GetLastUsedPage(pDieInfo->nDie, tmpLogBlk2, SUPPORT_LOG_BLOCK_MANAGE&LSB_TYPE);

					if(tmpLastPage0 < tmpLastPage2)
			        {
			            pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum = tmpLogBlk2;
						pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum = 0xffff;
			      *(pEraseBlk + eraseblockcnt) = tmpLogBlk0;
					  eraseblockcnt++;

						PRINT(" log age the same, Erase tmpLogBlk0 \n");
			        }
			        else
			        {
			            pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum = 0xffff;
			            *(pEraseBlk + eraseblockcnt) = tmpLogBlk2;
					  			eraseblockcnt++;
						PRINT(" log age the same, Erase tmpLogBlk0 \n");
			        }
				}
				else
				{
					pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk2.PhyBlkNum = 0xffff;
			    *(pEraseBlk + eraseblockcnt) = tmpLogBlk2;
					  			eraseblockcnt++;
					PRINT(" Error, log age diff, Erase tmpLogBlk2 \n");
				}

			}



		}
		return 0;
	}

    if(COMPARE_AGE(tmpAgeNew, tmpAgeData) == 0)
    {
        tmpLastPageOfData = _GetLastUsedPage(pDieInfo->nDie, tmpDataBlk, SUPPORT_LOG_BLOCK_MANAGE&LSB_TYPE);
        tmpLastPageOfNew = _GetLastUsedPage(pDieInfo->nDie, tmpNewBlk, SUPPORT_LOG_BLOCK_MANAGE&LSB_TYPE);

        if(tmpLastPageOfNew > tmpLastPageOfData)
        {
            tmpSuperBlk->PhyBlkNum = tmpNewBlk;
            *pEraseBlk = tmpDataBlk;
			PRINT(" two data block, erase data block %x with less pages\n", tmpDataBlk);

            return 0;
        }
        else
        {
            *pEraseBlk = tmpNewBlk;
			PRINT(" two data block, erase new block %x with less pages\n", tmpNewBlk);

            return 0;
        }
    }
    else if(COMPARE_AGE(tmpAgeNew, tmpAgeData) > 0)
    {
        if(tmpAgeNew == ((tmpAgeData+1) & 0xff))
        {
            if(tmpLogPst != -1)
            {
                tmpLastPageOfNew = _GetLastUsedPage(pDieInfo->nDie, tmpNewBlk, SUPPORT_LOG_BLOCK_MANAGE&LSB_TYPE);
                tmpLastPageOfLog = _GetLastUsedPage(pDieInfo->nDie, tmpLogBlk, SUPPORT_LOG_BLOCK_MANAGE&LSB_TYPE);

				if(tmpLastPageOfNew > tmpLastPageOfLog)
                {
                    pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[i].PhyBlk.PhyBlkNum = tmpNewBlk;
                    *pEraseBlk = tmpLogBlk;
					PRINT(" two log block, erase log block %x with less pages\n", tmpDataBlk);

                    return 0;
                }
                else
                {
                    *pEraseBlk = tmpNewBlk;
					PRINT(" two log block, erase new block %x with less pages\n", tmpNewBlk);

                    return 0;
                }

            }
            else
            {
                for(i=0; i<MAX_LOG_BLK_CNT; i++)
                {
                    if(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[i].LogicBlkNum == 0xffff)
                    {
                        tmpLogPst = i;
                        break;
                    }
                }

                if(tmpLogPst != -1)
                {
                    pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].LogicBlkNum = GET_LOGIC_INFO_BLK(nLogicInfo);
                    pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum = tmpNewBlk;

                    return 0;
                }
                else
                {
                    *pEraseBlk = tmpNewBlk;
					PRINT(" log block table full, erase new block %x \n", tmpNewBlk);
                    return 0;
                }
            }
        }
        else
        {
            FORMAT_DBG("[FORMAT_DBG] The log age of block(logicInfo:0x%x) are not sequential, "
                "age is:0x%x, 0x%x\n", nLogicInfo, tmpAgeData, tmpAgeNew);

            *pEraseBlk = tmpNewBlk;
			PRINT(" log age not sequential, erase new block %x \n", tmpNewBlk);
            return 0;
        }
    }
    else
    {
        if(tmpAgeData == ((tmpAgeNew+1) & 0xff))
        {
            tmpSuperBlk->PhyBlkNum = tmpNewBlk;
            if(tmpLogPst != -1)
            {
                *pEraseBlk = tmpLogBlk;
				PRINT(" log age too large, erase log block %x \n", tmpLogBlk);
            }
            else
            {
                for(i=0; i<MAX_LOG_BLK_CNT; i++)
                {
                    if(pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[i].LogicBlkNum == 0xffff)
                    {
                        tmpLogPst = i;
                        break;
                    }
                }

                if(tmpLogPst == -1)
                {
                    *pEraseBlk = tmpDataBlk;
					PRINT(" log block table full, erase data block %x \n", tmpDataBlk);
                    return 0;
                }

                pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].LogicBlkNum = GET_LOGIC_INFO_BLK(nLogicInfo);
            }

            pDieInfo->ZoneInfo[tmpZone].LogBlkTbl[tmpLogPst].PhyBlk.PhyBlkNum = tmpDataBlk;

            return 0;
        }
        else
        {
            FORMAT_DBG("[FORMAT_DBG] The log age of block(logicInfo:0x%x) are not sequential, "
                "age is:0x%x, 0x%x\n", nLogicInfo, tmpAgeNew, tmpAgeData);

            tmpSuperBlk->PhyBlkNum = tmpNewBlk;
            *pEraseBlk = tmpDataBlk;
			PRINT(" log age not sequential, erase data block %x \n", tmpDataBlk);
            return 0;
        }
    }

}


/*
************************************************************************************************************************
*                       GET A NEW PHYSICAL BLOCK FOR SAVING TABLE
*
*Description: Get a new physical block for saving block mapping table, the block should be in the
*             table block area, if can't find block in the block table area anyway, then, get a
*             free block not in the block area is ok, in this case, it will need rebuild the block
*             mapping table when installed the nand flash driver every time.
*
*Arguments  : pDieInfo  the pointer to the die information;
*             nZone     the number of the zone in the die.
*
*Return     : the block number.
*               >=0     get block successful, the return value is the number of the block.
*               < 0     get block failed.
************************************************************************************************************************
*/
static __s32 _GetNewTableBlk(struct __ScanDieInfo_t *pDieInfo, __u32 nZone)
{
    __s32   i, tmpBlk, tmpFreeBlk, tmpStartBlk, result;
    __s32   tmpValidTblBlk, tmpInvalidTblBlk, tmpValidTblFreeBlk;

    if(pDieInfo->nDie == 0)
    {
        tmpStartBlk = DIE0_FIRST_BLK_NUM;
    }
    else
    {
        tmpStartBlk = 0;
    }

    for(tmpBlk=tmpStartBlk; tmpBlk<TBL_AREA_BLK_NUM; tmpBlk++)
    {
        if(pDieInfo->pPhyBlk[tmpBlk] == FREE_BLOCK_INFO)
        {
            pDieInfo->pPhyBlk[tmpBlk] = NULL_BLOCK_INFO;
            pDieInfo->nFreeCnt--;

            return tmpBlk;
        }
    }


    tmpValidTblBlk = -1;
    tmpInvalidTblBlk = -1;
    tmpValidTblFreeBlk = -1;

    for(tmpBlk=tmpStartBlk; tmpBlk<TBL_AREA_BLK_NUM; tmpBlk++)
    {
        if((pDieInfo->pPhyBlk[tmpBlk] & ALLOC_BLK_MARK) == ALLOC_BLK_MARK)
        {
            if(tmpValidTblFreeBlk == -1)
            {
                tmpValidTblFreeBlk = tmpBlk;
            }

            continue;
        }

        if(!GET_LOGIC_INFO_TYPE(pDieInfo->pPhyBlk[tmpBlk]))
        {
            if((tmpValidTblBlk == -1) && (pDieInfo->ZoneInfo[GET_LOGIC_INFO_ZONE(pDieInfo-> \
                pPhyBlk[tmpBlk])].ZoneTbl[GET_LOGIC_INFO_BLK(pDieInfo->pPhyBlk[tmpBlk])].PhyBlkNum == tmpBlk))
            {
                tmpValidTblBlk = tmpBlk;
            }

            if((tmpInvalidTblBlk == -1) && (pDieInfo->ZoneInfo[GET_LOGIC_INFO_ZONE(pDieInfo-> \
                pPhyBlk[tmpBlk])].ZoneTbl[GET_LOGIC_INFO_BLK(pDieInfo->pPhyBlk[tmpBlk])].PhyBlkNum == tmpBlk))
            {
                if(!(pDieInfo->TblBitmap & (1 << GET_LOGIC_INFO_ZONE(pDieInfo->pPhyBlk[tmpBlk]))))
                {
                    tmpInvalidTblBlk = tmpBlk;

                    break;
                }
            }
        }
    }

    tmpFreeBlk = _GetFreeBlkFromDieInfo(pDieInfo);
    if(tmpFreeBlk < 0)
    {
        FORMAT_ERR("[FORMAT_ERR] Look for a free block  failed, not enough valid blocks\n");

        return -1;
    }

    pDieInfo->nFreeCnt--;

    if(tmpInvalidTblBlk != -1)
    {
        result = _MergeDataBlkToFreeBlk(pDieInfo, tmpInvalidTblBlk, tmpFreeBlk);
        if(result < 0)
        {
            return -1;
        }

        return tmpInvalidTblBlk;
    }
    else if (tmpValidTblFreeBlk != -1)
    {
        struct __SuperPhyBlkType_t *tmpMapTbl = pDieInfo->ZoneInfo[GET_LOGIC_INFO_ZONE(pDieInfo-> \
                                pPhyBlk[tmpValidTblFreeBlk])].ZoneTbl;

        pDieInfo->TblBitmap &= ~(1 << GET_LOGIC_INFO_ZONE(pDieInfo->pPhyBlk[tmpValidTblFreeBlk]));

        for(i=0; i<DATA_BLK_CNT_OF_ZONE - 1; i++)
        {
            if(tmpMapTbl[i].PhyBlkNum == tmpValidTblFreeBlk)
            {
                pDieInfo->pPhyBlk[tmpFreeBlk] = pDieInfo->pPhyBlk[tmpValidTblFreeBlk];

                tmpMapTbl[i].PhyBlkNum = tmpFreeBlk;
            }
        }

        for(i=DATA_BLK_CNT_OF_ZONE; i<BLOCK_CNT_OF_ZONE - 1; i++)
        {
            if(tmpMapTbl[i].PhyBlkNum == tmpValidTblFreeBlk)
            {
                pDieInfo->pPhyBlk[tmpFreeBlk] = pDieInfo->pPhyBlk[tmpValidTblFreeBlk];

                tmpMapTbl[i].PhyBlkNum = tmpFreeBlk;
            }

            if(tmpMapTbl[i].PhyBlkNum != 0xffff)
            {
                pDieInfo->pPhyBlk[tmpMapTbl[i].PhyBlkNum] = FREE_BLOCK_INFO;
                pDieInfo->nFreeCnt++;

                tmpMapTbl[i].PhyBlkNum = 0xffff;
            }
        }

        pDieInfo->ZoneInfo[GET_LOGIC_INFO_ZONE(pDieInfo-> \
                                pPhyBlk[tmpValidTblFreeBlk])].nDataBlkCnt = DATA_BLK_CNT_OF_ZONE;

        pDieInfo->ZoneInfo[GET_LOGIC_INFO_ZONE(pDieInfo-> \
                                pPhyBlk[tmpValidTblFreeBlk])].nFreeBlkCnt = 0;

        return tmpValidTblFreeBlk;
    }
    else if(tmpValidTblBlk != -1)
    {
        struct __SuperPhyBlkType_t *tmpMapTbl = pDieInfo->ZoneInfo[GET_LOGIC_INFO_ZONE(pDieInfo-> \
                                pPhyBlk[tmpValidTblBlk])].ZoneTbl;

        pDieInfo->TblBitmap &= ~(1 << GET_LOGIC_INFO_ZONE(pDieInfo->pPhyBlk[tmpBlk]));
        pDieInfo->nFreeCnt--;

        result = _MergeDataBlkToFreeBlk(pDieInfo, tmpValidTblBlk, tmpFreeBlk);

        for(i=DATA_BLK_CNT_OF_ZONE; i<BLOCK_CNT_OF_ZONE - 1; i++)
        {
            if(tmpMapTbl[i].PhyBlkNum != 0xffff)
            {
                pDieInfo->pPhyBlk[tmpMapTbl[i].PhyBlkNum] = FREE_BLOCK_INFO;
                pDieInfo->nFreeCnt++;

                tmpMapTbl[i].PhyBlkNum = 0xffff;
            }
        }

        pDieInfo->ZoneInfo[GET_LOGIC_INFO_ZONE(pDieInfo-> \
                                pPhyBlk[tmpValidTblFreeBlk])].nDataBlkCnt = DATA_BLK_CNT_OF_ZONE;

        pDieInfo->ZoneInfo[GET_LOGIC_INFO_ZONE(pDieInfo-> \
                                pPhyBlk[tmpValidTblFreeBlk])].nFreeBlkCnt = 0;

        if(result < 0)
        {
            return -1;
        }

        return tmpValidTblBlk;
    }
    else
    {
        FORMAT_DBG("[FORMAT_DBG] Look for a block in table block area for table failed\n");

        return tmpFreeBlk;
    }
}


/*
************************************************************************************************************************
*                       GET PHYSICAL BLOCK FOR SAVING BLOCK MAPPING TABLE
*
*Description: Get physical block for saving block mapping table, the physical block should be in
*             the block mapping table block area, which is in front of the die.
*
*Arguments  : pDieInfo   the pointer to the die information.
*
*Return     : get mapping table block result;
*               = 0     get mapping table block successful;
*               < 0     get mapping table block failed.
************************************************************************************************************************
*/
static __s32 _GetMapTblBlock(struct __ScanDieInfo_t *pDieInfo)
{
    __s32   i, result, tmpTryGet;

    for(i=0; i<ZONE_CNT_OF_DIE; i++)
    {
        if(pDieInfo->TblBitmap & (1 << i))
        {
            continue;
        }

        if(ZoneTblPstInfo[i + pDieInfo->nDie * ZONE_CNT_OF_DIE].PhyBlkNum < TBL_AREA_BLK_NUM)
        {
            continue;
        }

        tmpTryGet = 0;
        while(tmpTryGet < 5)
        {
            tmpTryGet++;

            result = _GetNewTableBlk(pDieInfo, i);
            if(!(result < 0))
            {
                break;
            }
        }

        if(result < 0)
        {
            FORMAT_ERR("[FORMAT_ERR] Get new physical block for mapping table failed in die 0x%x!\n", pDieInfo->nDie);
            return -1;
        }


        ZoneTblPstInfo[i + pDieInfo->nDie * ZONE_CNT_OF_DIE].PhyBlkNum = result;
        ZoneTblPstInfo[i + pDieInfo->nDie * ZONE_CNT_OF_DIE].TablePst = 0xffff;

        pDieInfo->pPhyBlk[result] = TABLE_BLK_MARK | (i<<10) | (1<<14);
    }

    return 0;
}


/*
************************************************************************************************************************
*                       KICK FREE BLOCKS FROM VALID BLOCK TABLE
*
*Description: Kick free blocks from valid block table, the other free blocks is used for allocating to
*             the block tables which need be rebuiled.
*
*Arguments  : pDieInfo   the pointer to the die information.
*
*Return     : kick free block result;
*               = 0     kick free block successful;
*               < 0     kick free block failed.
************************************************************************************************************************
*/
static __s32 _KickValidTblBlk(struct __ScanDieInfo_t *pDieInfo)
{
    __s32   i, j;
    __u32   tmpTblBlk, tmpTblPage;
    __u16   tmpPhyBlk;

    for(i=0; i<ZONE_CNT_OF_DIE; i++)
    {
        if(!(pDieInfo->TblBitmap & (1 << i)))
        {
            continue;
        }

        tmpTblBlk = ZoneTblPstInfo[i + pDieInfo->nDie * ZONE_CNT_OF_DIE].PhyBlkNum;
        tmpTblPage = ZoneTblPstInfo[i + pDieInfo->nDie * ZONE_CNT_OF_DIE].TablePst;
        _VirtualPageRead(pDieInfo->nDie, tmpTblBlk, tmpTblPage+DATA_TBL_OFFSET, DATA_TABLE_BITMAP, FORMAT_PAGE_BUF, NULL);
        MEMCPY(pDieInfo->ZoneInfo[i].ZoneTbl, FORMAT_PAGE_BUF, SECTOR_SIZE * 4);
        _VirtualPageRead(pDieInfo->nDie, tmpTblBlk, tmpTblPage+DATA_TBL_OFFSET+1, DATA_TABLE_BITMAP, FORMAT_PAGE_BUF, NULL);
        MEMCPY(&pDieInfo->ZoneInfo[i].ZoneTbl[BLOCK_CNT_OF_ZONE / 2], FORMAT_PAGE_BUF, SECTOR_SIZE * 4);
        _VirtualPageRead(pDieInfo->nDie, tmpTblBlk, tmpTblPage+LOG_TBL_OFFSET, LOG_TABLE_BITMAP, FORMAT_PAGE_BUF, NULL);
        MEMCPY(&pDieInfo->ZoneInfo[i].LogBlkTbl, FORMAT_PAGE_BUF, MAX_LOG_BLK_CNT * sizeof(struct __LogBlkType_t));

        for(j=0; j<BLOCK_CNT_OF_ZONE-1; j++)
        {
            tmpPhyBlk = pDieInfo->ZoneInfo[i].ZoneTbl[j].PhyBlkNum;

            if(tmpPhyBlk == 0xffff)
            {
                continue;
            }

            if(pDieInfo->pPhyBlk[tmpPhyBlk] == FREE_BLOCK_INFO)
            {
                pDieInfo->pPhyBlk[tmpPhyBlk] = (ALLOC_BLK_MARK | (i<<10));
                pDieInfo->nFreeCnt--;
            }
        }
    }

    return 0;
}


/*
************************************************************************************************************************
*                       REPAIR THE LOG BLOCK TABLE FOR A DIE
*
*Description: Repair the log block table for a die.
*
*Arguments  : pDieInfo   the pointer to the die information.
*
*Return     : repair log block table result;
*               = 0     repair log block table successful;
*               < 0     repair log block table failed.
************************************************************************************************************************
*/
static __s32 _RepairLogBlkTbl(struct __ScanDieInfo_t *pDieInfo)
{
    __s32 i, tmpZone, tmpLastUsedPage, result, ecc_flag = 0;
    struct __LogBlkType_t *tmpLogTbl;
	__u32 tmpLogBlock;
	__s32 tmpStartBlk, tmpFreeBlock;

    for(tmpZone=0; tmpZone<ZONE_CNT_OF_DIE; tmpZone++)
    {
        if(pDieInfo->TblBitmap & (1 << tmpZone))
        {
            continue;
        }

        tmpLogTbl = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl;

        for(i=0; i<MAX_LOG_BLK_CNT; i++)
        {
        	ecc_flag = 0;
            if(tmpLogTbl->LogicBlkNum != 0xffff)
            {
            	if((SUPPORT_LOG_BLOCK_MANAGE)&&(tmpLogTbl->LogBlkType == LSB_TYPE))
            	{
            		if(tmpLogTbl->PhyBlk1.PhyBlkNum != 0xffff )
						tmpLogTbl->WriteBlkIndex = 1;
					else
					{
					    if(pDieInfo->nDie == 0)
					    {
					        tmpStartBlk = DIE0_FIRST_BLK_NUM;
					    }
					    else
					    {
					        tmpStartBlk = 0;
					    }

					    for(tmpFreeBlock=SuperBlkCntOfDie-1; tmpFreeBlock>=0; tmpFreeBlock--)
					    {
					        if(pDieInfo->pPhyBlk[tmpFreeBlock] == FREE_BLOCK_INFO)
					        {
					            if(_VirtualFreeBlockCheck(pDieInfo->nDie, tmpFreeBlock)==0)
					            {
						            pDieInfo->pPhyBlk[tmpFreeBlock] = NULL_BLOCK_INFO;
						            pDieInfo->nFreeCnt--;

												break;
											}
					        }
					    }

						tmpLogTbl->PhyBlk1.PhyBlkNum = tmpFreeBlock;
						tmpLogTbl->WriteBlkIndex = 0;

					}
				}

			#ifdef LOG_BLOCK_ECC_CHECK
				#ifndef NORMAL_LOG_BLOCK_ECC_CHECK
					if((SUPPORT_LOG_BLOCK_MANAGE)&&(tmpLogTbl->LogBlkType == LSB_TYPE))
				#else
					if(1)
				#endif
					{
						if((SUPPORT_LOG_BLOCK_MANAGE)&&(tmpLogTbl->LogBlkType == LSB_TYPE)&&(tmpLogTbl->WriteBlkIndex == 1))
							tmpLogBlock = tmpLogTbl->PhyBlk1.PhyBlkNum;
						else
							tmpLogBlock = tmpLogTbl->PhyBlk.PhyBlkNum;

						result = _VirtualLogBlkEccCheck(pDieInfo->nDie, tmpLogBlock, tmpLogTbl->LogBlkType);
						if(result)
						{
							ecc_flag = 1;
							PRINT("Logic Block %x, log blk: %x, %x page ecc check error\n", tmpLogTbl->LogicBlkNum, tmpLogBlock, result);
						}

						if(ecc_flag)
						{
							PRINT("[NAND ECC Repair] logrepair, find a ecc error in log block, need to repair the block\n");
							if(pDieInfo->nDie == 0)
						    {
						        tmpStartBlk = DIE0_FIRST_BLK_NUM;
						    }
						    else
						    {
						        tmpStartBlk = 0;
						    }

							for(tmpFreeBlock=SuperBlkCntOfDie-1; tmpFreeBlock>=0; tmpFreeBlock--)
						    {
						        if(pDieInfo->pPhyBlk[tmpFreeBlock] == FREE_BLOCK_INFO)
						        {
						            if(_VirtualFreeBlockCheck(pDieInfo->nDie, tmpFreeBlock)==0)
						            {
										pDieInfo->pPhyBlk[tmpFreeBlock] = NULL_BLOCK_INFO;
						            	pDieInfo->nFreeCnt--;

													break;
						            }

						        }
						    }

							_VirtualLogBlkRepair(pDieInfo->nDie,tmpLogBlock,tmpFreeBlock,PAGE_CNT_OF_SUPER_BLK -1);

							_VirtualBlockErase(pDieInfo->nDie,tmpLogBlock);
							pDieInfo->pPhyBlk[tmpLogBlock] = FREE_BLOCK_INFO;
							pDieInfo->nFreeCnt++;
							if(tmpLogTbl->WriteBlkIndex == 1)
								tmpLogTbl->PhyBlk1.PhyBlkNum = tmpFreeBlock;
							else
								tmpLogTbl->PhyBlk.PhyBlkNum = tmpFreeBlock;
							tmpLogBlock = tmpFreeBlock;
						}
					}
			#endif

			#ifdef LOG_AGE_SEQ_CHECK
			if((SUPPORT_LOG_BLOCK_MANAGE)&&(tmpLogTbl->LogBlkType == LSB_TYPE))
			{
				__u32 tmpDataBlk, tmpLog0, tmpLog1, tmpDataAge, tmpLogAge0, tmpLogAge1;

				tmpDataBlk = pDieInfo->ZoneInfo[tmpZone].ZoneTbl[tmpLogTbl->LogicBlkNum].PhyBlkNum;
			    tmpLog0 = tmpLogTbl->PhyBlk.PhyBlkNum;
				tmpLog1 = tmpLogTbl->PhyBlk1.PhyBlkNum;

				if(tmpDataBlk!=0xffff)
					tmpDataAge = _GetLogAge(pDieInfo->nDie,tmpDataBlk);
				else
				{
					PRINT("[NAND] Error,LogicBlockNum: %x, Data block lost\n", tmpLogTbl->LogicBlkNum);
				}

				if(tmpLog0!=0xffff)
					tmpLogAge0 = _GetLogAge(pDieInfo->nDie,tmpLog0);
				else
				{
					PRINT("[NAND] Error, LogicBlockNum: %x, log block 0 lost\n", tmpLogTbl->LogicBlkNum);

					if(tmpLog1 != 0xffff)
					{
						_VirtualBlockErase(pDieInfo->nDie,tmpLog1);
						pDieInfo->pPhyBlk[tmpLog1] = FREE_BLOCK_INFO;
						pDieInfo->nFreeCnt++;

						MEMSET(tmpLogTbl, 0xff, sizeof(struct __LogBlkType_t));

						tmpLogTbl->LogBlkType = 0;
						tmpLogTbl->WriteBlkIndex= 0;
						tmpLogTbl->ReadBlkIndex= 0;
						PRINT(" log table repair,LogicBlockNum: %x, log0 lost, log0: %x, log1: %x, erase log1\n", tmpLogTbl->LogicBlkNum, tmpLog0, tmpLog1);
					}
				}

				if((tmpLog0!=0xffff)&&((tmpDataAge+1)&0xff)!=(tmpLogAge0&0xff))
				{
					if(((tmpLogAge0+1)&0xff)==(tmpDataAge&0xff))
					{
						_VirtualBlockErase(pDieInfo->nDie,tmpLog0);
						pDieInfo->pPhyBlk[tmpLog0] = FREE_BLOCK_INFO;
						pDieInfo->nFreeCnt++;

						if(tmpLog1 != 0xffff)
						{
							_VirtualBlockErase(pDieInfo->nDie,tmpLog1);
							pDieInfo->pPhyBlk[tmpLog1] = FREE_BLOCK_INFO;
							pDieInfo->nFreeCnt++;
						}

						MEMSET(tmpLogTbl, 0xff, sizeof(struct __LogBlkType_t));

						tmpLogTbl->LogBlkType = 0;
						tmpLogTbl->WriteBlkIndex= 0;
						tmpLogTbl->ReadBlkIndex= 0;
						PRINT(" log table repair,LogicBlockNum: %x, log0 age not seq, data: %x, log0: %x, erase log blocks\n", tmpLogTbl->LogicBlkNum, tmpDataAge, tmpLogAge0);

					}
					else
					{
						PRINT("[NAND] Error, log table repair,LogicBlockNum: %x, log0 age not seq, data: %x, log0: %x\n", tmpLogTbl->LogicBlkNum, tmpDataAge, tmpLogAge0);
					}


				}

				if((tmpLog0!=0xffff)&&(tmpLogTbl->WriteBlkIndex == 1))
				{
					tmpLogAge1 = _GetLogAge(pDieInfo->nDie,tmpLog1);
					if((tmpLogAge1!=tmpLogAge0)&&(tmpLogAge1!=0xaa))
					{
						PRINT("[NAND] Error, log table repair,LogicBlockNum: %x, log age diff, log0: %x, log1: %x\n", tmpLogTbl->LogicBlkNum, tmpLogAge0, tmpLogAge1);
					}

				}

			}
			#endif

				if((SUPPORT_LOG_BLOCK_MANAGE)&&(tmpLogTbl->LogBlkType == LSB_TYPE)&&(tmpLogTbl->WriteBlkIndex == 1))
					tmpLogBlock = tmpLogTbl->PhyBlk1.PhyBlkNum;
				else
					tmpLogBlock = tmpLogTbl->PhyBlk.PhyBlkNum;

				tmpLastUsedPage = _GetLastUsedPage(pDieInfo->nDie, tmpLogBlock, tmpLogTbl->LogBlkType);
	        	tmpLogTbl->LastUsedPage = tmpLastUsedPage;

				DBUG_INF("Log Block Index %x, LogicBlockNum: %x, LogBlockType: %x\n", i, tmpLogTbl->LogicBlkNum, tmpLogTbl->LogBlkType);
				DBUG_INF("log0: %x, Log1: %x, WriteIndex: %x\n",tmpLogTbl->PhyBlk.PhyBlkNum, tmpLogTbl->PhyBlk1.PhyBlkNum, tmpLogTbl->WriteBlkIndex);
				DBUG_INF("datablock: %x, lastusedpage: %x\n", pDieInfo->ZoneInfo[tmpZone].ZoneTbl[tmpLogTbl->LogicBlkNum].PhyBlkNum ,tmpLogTbl->LastUsedPage);
				if((tmpLogTbl->PhyBlk1.PhyBlkNum != 0xffff)&&(tmpLogTbl->LogBlkType != LSB_TYPE))
				{
					PRINT("[NAND] Error, log type error!\n");
				}

            }
            tmpLogTbl++;
        }
    }

    return 0;
}


/*
************************************************************************************************************************
*                       DISTRIBUTE FREE BLOCKS TO BLOCK MAPPING TABLES
*
*Description: Destribute the free blocks to the block mapping tables, distribute free block to fill
*             the empty items in the data block table at first, assure every data block table item
*             has a block; then distribute the free blocks to the free block table items.
*
*Arguments  : pDieInfo   the pointer to the die information.
*
*Return     : distribute free block result;
*               = 0     distribute free block successful;
*               < 0     distribute free block failed.
************************************************************************************************************************
*/
static __s32 _DistributeFreeBlk(struct __ScanDieInfo_t *pDieInfo)
{
    __s32   i, tmpZone, tmpFreeBlk;

    struct __SuperPhyBlkType_t *tmpZoneTbl;


    if(pDieInfo->nDie == 0)
    {
        pDieInfo->nFreeIndex = DIE0_FIRST_BLK_NUM;
    }
    else
    {
        pDieInfo->nFreeIndex = 0;
    }


    for(tmpZone=0; tmpZone<ZONE_CNT_OF_DIE; tmpZone++)
    {
        if(pDieInfo->TblBitmap & (1 << tmpZone))
        {
            continue;
        }

        if(pDieInfo->nFreeCnt < (DATA_BLK_CNT_OF_ZONE - pDieInfo->ZoneInfo[tmpZone].nDataBlkCnt))
        {
            FORMAT_ERR("[FORMAT_ERR] There is not enough valid block for using!\n");

            return -1;
        }

        tmpZoneTbl = pDieInfo->ZoneInfo[tmpZone].ZoneTbl;

        pDieInfo->ZoneInfo[tmpZone].nFreeBlkIndex = DATA_BLK_CNT_OF_ZONE;

        for(i=0; i<DATA_BLK_CNT_OF_ZONE; i++)
        {
            if(tmpZoneTbl[i].PhyBlkNum == 0xffff)
            {
                tmpFreeBlk = _GetFreeBlkFromDieInfo(pDieInfo);
                if(tmpFreeBlk < 0)
                {
                    FORMAT_ERR("[FORMAT_ERR] There is not enough valid block for using!\n");

                    return -1;
                }

                tmpZoneTbl[i].PhyBlkNum = tmpFreeBlk;
                pDieInfo->ZoneInfo[tmpZone].nDataBlkCnt++;
                pDieInfo->pPhyBlk[tmpFreeBlk] = (tmpZone<<10) | ALLOC_BLK_MARK;
            }
        }
    }


	while(pDieInfo->nFreeCnt > 0)
    {
        tmpZone = _LeastBlkCntZone(pDieInfo);

        tmpZoneTbl =  pDieInfo->ZoneInfo[tmpZone].ZoneTbl;

        if(pDieInfo->ZoneInfo[tmpZone].nFreeBlkIndex < BLOCK_CNT_OF_ZONE)
        {
            tmpFreeBlk = _GetFreeBlkFromDieInfo(pDieInfo);
            if(tmpFreeBlk < 0)
            {
                FORMAT_DBG("[FORMAT_WARNNING] Get free block failed when it should be successful!\n");
                continue;
            }

            tmpZoneTbl[pDieInfo->ZoneInfo[tmpZone].nFreeBlkIndex].PhyBlkNum = tmpFreeBlk;
            pDieInfo->ZoneInfo[tmpZone].nFreeBlkIndex++;
            pDieInfo->ZoneInfo[tmpZone].nFreeBlkCnt++;

            pDieInfo->pPhyBlk[tmpFreeBlk] = (tmpZone<<10) | ALLOC_BLK_MARK;
        }
        else
        {
            FORMAT_DBG("[FORMAT_WARNNING] There is some blocks more than we used!\n");
			break;
        }
    }


    return 0;

}


/*
************************************************************************************************************************
*                       FILL BLOCK MAPPING TABLE INFORMATION
*
*Description: Fill the block mapping table information with the logical informaton buffer of the die.
*
*Arguments  : pDieInfo   the pointer to the die information whose mapping table information need be filled.
*
*Return     : fill block mapping table result;
*               = 0     fill block mapping table successful;
*               < 0     fill block mapping table failed.
************************************************************************************************************************
*/
static __s32 _FillZoneTblInfo(struct __ScanDieInfo_t *pDieInfo)
{
    __u16   tmpLogicInfo;
    __u32   i, tmpPhyBlk, tmpBlkErase[8];
    __s32   result;

    if(pDieInfo->nDie == 0)
    {
        tmpPhyBlk = DIE0_FIRST_BLK_NUM;
    }
    else
    {
        tmpPhyBlk = 0;
    }

    for( ; tmpPhyBlk<SuperBlkCntOfDie; tmpPhyBlk++)
    {
        tmpLogicInfo = pDieInfo->pPhyBlk[tmpPhyBlk];

        if(tmpPhyBlk == SuperBlkCntOfDie-1)
        {
            if(tmpLogicInfo == FREE_BLOCK_INFO)
            {
                FORMAT_DBG("[FORMAT_DBG] mark the last block as bad block \n");
                pDieInfo->pPhyBlk[tmpPhyBlk] = BAD_BLOCK_INFO;
                _WriteBadBlkFlag(pDieInfo->nDie, tmpPhyBlk);

            }
        }

        tmpLogicInfo = pDieInfo->pPhyBlk[tmpPhyBlk];


        if(tmpLogicInfo == BAD_BLOCK_INFO)
        {
            pDieInfo->nBadCnt++;

            continue;
        }

        if(tmpLogicInfo == FREE_BLOCK_INFO)
        {
            pDieInfo->nFreeCnt++;
            continue;
        }

        if(GET_LOGIC_INFO_TYPE(tmpLogicInfo) == 1)
        {
            if(GET_LOGIC_INFO_BLK(tmpLogicInfo) == BOOT_BLK_MARK)
            {
                if(pDieInfo->nDie == 0)
                {
                    continue;
                }
                else
                {
                    FORMAT_DBG("[FORMAT_DBG] Find a boot type block(0x%x) not in die0!\n", tmpPhyBlk);
                    result = _VirtualBlockErase(pDieInfo->nDie, tmpPhyBlk);
                    if(result < 0)
                    {
                        pDieInfo->pPhyBlk[tmpPhyBlk] = BAD_BLOCK_INFO;
                        _WriteBadBlkFlag(pDieInfo->nDie, tmpPhyBlk);

                        continue;
                    }

                    pDieInfo->pPhyBlk[tmpPhyBlk] = FREE_BLOCK_INFO;
                    pDieInfo->nFreeCnt++;

                    continue;
                }
            }
            else if(GET_LOGIC_INFO_BLK(tmpLogicInfo) == TABLE_BLK_MARK)
            {
                if((GET_LOGIC_INFO_ZONE(tmpLogicInfo) < ZONE_CNT_OF_DIE)
                    && (tmpPhyBlk < TBL_AREA_BLK_NUM))
                {
                    continue;
                }
                else
                {
                    FORMAT_DBG("[FORMAT_DBG] Find an invalid block mapping table in Die 0x%x !\n", pDieInfo->nDie);

                    result = _VirtualBlockErase(pDieInfo->nDie, tmpPhyBlk);
                    if(result < 0)
                    {
                        pDieInfo->pPhyBlk[tmpPhyBlk] = BAD_BLOCK_INFO;
                        _WriteBadBlkFlag(pDieInfo->nDie, tmpPhyBlk);

                        continue;
                    }

                    pDieInfo->pPhyBlk[tmpPhyBlk] = FREE_BLOCK_INFO;
                    pDieInfo->nFreeCnt++;

                    continue;
                }
            }
            else
            {
                FORMAT_DBG("[FORMAT_DBG] Find an recgnized special type block in die 0x%x!\n", pDieInfo->nDie);

                result = _VirtualBlockErase(pDieInfo->nDie, tmpPhyBlk);
                if(result < 0)
                {
                    pDieInfo->pPhyBlk[tmpPhyBlk] = BAD_BLOCK_INFO;
                    _WriteBadBlkFlag(pDieInfo->nDie, tmpPhyBlk);

                    continue;
                }

                pDieInfo->pPhyBlk[tmpPhyBlk] = FREE_BLOCK_INFO;
                pDieInfo->nFreeCnt++;

                continue;
            }
        }

        if((GET_LOGIC_INFO_ZONE(tmpLogicInfo) >= ZONE_CNT_OF_DIE) || \
                    (GET_LOGIC_INFO_BLK(tmpLogicInfo) >= DATA_BLK_CNT_OF_ZONE))
        {
            result = _VirtualBlockErase(pDieInfo->nDie, tmpPhyBlk);
            if(result < 0)
            {
                pDieInfo->pPhyBlk[tmpPhyBlk] = BAD_BLOCK_INFO;
                _WriteBadBlkFlag(pDieInfo->nDie, tmpPhyBlk);

                continue;
            }

            pDieInfo->pPhyBlk[tmpPhyBlk] = FREE_BLOCK_INFO;
            pDieInfo->nFreeCnt++;

            continue;
        }

        if((1 << GET_LOGIC_INFO_ZONE(tmpLogicInfo)) & (pDieInfo->TblBitmap))
        {
            continue;
        }

        result = _FillBlkToZoneTbl(pDieInfo, tmpLogicInfo, tmpPhyBlk, &tmpBlkErase[0]);
        for(i=0;i<4;i++)
        {
        	if(tmpBlkErase[i] != 0xffff)
	        {
	        	PRINT("Erase block %x after _FillBlkToZoneTbl \n", tmpBlkErase[i]);
	            result = _VirtualBlockErase(pDieInfo->nDie, tmpBlkErase[i]);
	            if(result < 0)
	            {
	                pDieInfo->pPhyBlk[tmpBlkErase[i]] = BAD_BLOCK_INFO;
	                _WriteBadBlkFlag(pDieInfo->nDie, tmpBlkErase[i]);

	            }

	            pDieInfo->pPhyBlk[tmpBlkErase[i]] = FREE_BLOCK_INFO;
	            pDieInfo->nFreeCnt++;

	        }
	        else
	        	break;

        }

        continue;


    }

#if DBG_DUMP_DIE_INFO

    _DumpDieInfo(pDieInfo);

#endif

    return 0;
}


/*
************************************************************************************************************************
*                       WRITE BLOCK MAPPING TABLE TO NAND FLASH
*
*Description: Write the block mapping table to nand flash, the block mapping table is stored
*             in the buffer indexed by the die information.
*
*Arguments  : pDieInfo   the pointer to the die information
*
*Return     : write block mapping table result;
*               = 0     write block mapping table successful;
*               < 0     write block mapping table failed.
************************************************************************************************************************
*/
static __s32 _WriteBlkMapTbl(struct __ScanDieInfo_t *pDieInfo)
{
    __s32   i, tmpZone, tmpTblBlk, tmpTblPage, tmpGlobzone, result;
    struct __NandUserData_t tmpSpare[2];
    struct __SuperPhyBlkType_t *tmpDataBlk;
    struct __LogBlkType_t *tmpLogBlk;

    for(tmpZone=0; tmpZone<ZONE_CNT_OF_DIE; tmpZone++)
    {
        if(pDieInfo->TblBitmap & (1 << tmpZone))
        {
            continue;
        }

        tmpGlobzone = (pDieInfo->nDie) * ZONE_CNT_OF_DIE + tmpZone;

        tmpTblBlk = ZoneTblPstInfo[tmpGlobzone].PhyBlkNum;
        tmpTblPage = ZoneTblPstInfo[tmpGlobzone].TablePst;

        if(tmpTblPage == 0xffff)
        {
            tmpTblPage = 0;
        }
        else
        {
            tmpTblPage += PAGE_CNT_OF_TBL_GROUP;
        }

        if(!(tmpTblPage < PAGE_CNT_OF_SUPER_BLK))
        {
            result = _VirtualBlockErase(pDieInfo->nDie, tmpTblBlk);
            if(result < 0)
            {
                FORMAT_DBG("[FORMAT_DBG] Erase block failed when write block mapping table!\n");
                _WriteBadBlkFlag(pDieInfo->nDie, tmpTblBlk);

                return -1;
            }

            tmpTblPage = 0;
        }

        tmpDataBlk = pDieInfo->ZoneInfo[tmpZone].ZoneTbl;
        tmpLogBlk = pDieInfo->ZoneInfo[tmpZone].LogBlkTbl;
        for(i=0; i<BLOCK_CNT_OF_ZONE; i++)
        {
            if(tmpDataBlk[i].PhyBlkNum != 0xffff)
            {
                tmpDataBlk[i].BlkEraseCnt = 0;
            }
        }

        for(i=0; i<MAX_LOG_BLK_CNT; i++)
        {
            if(tmpLogBlk[i].LogicBlkNum != 0xffff)
            {
                tmpLogBlk[i].PhyBlk.BlkEraseCnt = 0;
            }
        }

        tmpSpare[0].BadBlkFlag = 0xff;
        tmpSpare[1].BadBlkFlag = 0xff;
        tmpSpare[0].LogicInfo = (1<<14) | (tmpZone<<10) | TABLE_BLK_MARK;
        tmpSpare[1].LogicInfo = (1<<14) | (tmpZone<<10) | TABLE_BLK_MARK;
        tmpSpare[0].LogicPageNum = 0xffff;
        tmpSpare[1].LogicPageNum = 0xffff;
        tmpSpare[0].PageStatus = 0x55;
        tmpSpare[1].PageStatus = 0x55;

        result = _CalCheckSum((__u32 *)tmpDataBlk, BLOCK_CNT_OF_ZONE - 1);
        *(__u32 *)&tmpDataBlk[BLOCK_CNT_OF_ZONE - 1] = (__u32)result;

        MEMSET(FORMAT_PAGE_BUF, 0xff, SECTOR_CNT_OF_SUPER_PAGE * SECTOR_SIZE);
        MEMCPY(FORMAT_PAGE_BUF, (__u32 *)tmpDataBlk, SECTOR_SIZE * 4);


        result = _VirtualPageWrite(pDieInfo->nDie, tmpTblBlk, tmpTblPage + DATA_TBL_OFFSET, \
                            FULL_BITMAP_OF_SUPER_PAGE, FORMAT_PAGE_BUF, (void *)tmpSpare);
        if(result < 0)
        {
            FORMAT_DBG("[FORMAT_DBG] Write page failed when write block mapping table!\n");
            _WriteBadBlkFlag(pDieInfo->nDie, tmpTblBlk);

            return -1;
        }


        MEMCPY(FORMAT_PAGE_BUF, (__u32 *)&tmpDataBlk[BLOCK_CNT_OF_ZONE / 2], SECTOR_SIZE * 4);
        result = _VirtualPageWrite(pDieInfo->nDie, tmpTblBlk, tmpTblPage + DATA_TBL_OFFSET + 1, \
                            FULL_BITMAP_OF_SUPER_PAGE, FORMAT_PAGE_BUF, (void *)tmpSpare);
        if(result < 0)
        {
            FORMAT_DBG("[FORMAT_DBG] Write page failed when write block mapping table!\n");
            _WriteBadBlkFlag(pDieInfo->nDie, tmpTblBlk);

            return -1;
        }

        MEMSET(FORMAT_PAGE_BUF, 0xff, SECTOR_CNT_OF_SUPER_PAGE * SECTOR_SIZE);
        MEMCPY(FORMAT_PAGE_BUF, (__u32 *)tmpLogBlk, LOG_BLK_CNT_OF_ZONE*sizeof(struct __LogBlkType_t));
        result = _CalCheckSum((__u32 *)FORMAT_PAGE_BUF, LOG_BLK_CNT_OF_ZONE*sizeof(struct __LogBlkType_t)/sizeof(__u32));
        ((__u32*)FORMAT_PAGE_BUF)[511] = (__u32)result;
        result = _VirtualPageWrite(pDieInfo->nDie, tmpTblBlk, tmpTblPage + LOG_TBL_OFFSET, \
                            FULL_BITMAP_OF_SUPER_PAGE, FORMAT_PAGE_BUF, (void *)tmpSpare);
        if(result < 0)
        {
            FORMAT_DBG("[FORMAT_DBG] Write page failed when write block mapping table!\n");
            _WriteBadBlkFlag(pDieInfo->nDie, tmpTblBlk);

            return -1;
        }

        ZoneTblPstInfo[tmpGlobzone].TablePst = tmpTblPage;
    }

    return 0;
}


/*
************************************************************************************************************************
*                       SEARCH ZONE TABLES FROM ONE NAND DIE
*
*Description: Search zone tables from one nand flash die.
*
*Arguments  : pDieInfo   the pointer to the die information whose block mapping table need be searched
*
*Return     : search result;
*               = 0     search zone table successful;
*               < 0     search zone table failed.
************************************************************************************************************************
*/
static __s32 _SearchZoneTbls(struct __ScanDieInfo_t *pDieInfo)
{
    __u32   tmpSuperBlk, tmpZoneInDie, tmpPage, tmpVar;
    __s32   result;
    struct  __NandUserData_t tmpSpareData[2];
    struct  __SuperPhyBlkType_t *tmpDataBlkTbl;

    FORMAT_DBG("[FORMAT_DBG] Search the block mapping table on DIE 0x%x\n", pDieInfo->nDie);

    if(pDieInfo->nDie == 0)
    {
        tmpSuperBlk = DIE0_FIRST_BLK_NUM;
    }
    else
    {
        tmpSuperBlk = 0;
    }

    for( ; tmpSuperBlk<TBL_AREA_BLK_NUM; tmpSuperBlk++)
    {
        tmpSpareData[0].BadBlkFlag = tmpSpareData[1].BadBlkFlag = 0xff;

        _VirtualPageRead(pDieInfo->nDie, tmpSuperBlk, 0, SPARE_DATA_BITMAP, FORMAT_PAGE_BUF, (void *)&tmpSpareData);

        if((tmpSpareData[0].BadBlkFlag != 0xff) || (tmpSpareData[1].BadBlkFlag != 0xff) || \
                (GET_LOGIC_INFO_TYPE(tmpSpareData[0].LogicInfo) != 1) || \
                        (GET_LOGIC_INFO_BLK(tmpSpareData[0].LogicInfo) != 0xaa))
        {
            continue;
        }

        tmpZoneInDie = GET_LOGIC_INFO_ZONE(tmpSpareData[0].LogicInfo);
        if(!(tmpZoneInDie < ZONE_CNT_OF_DIE))
        {
            result = _VirtualBlockErase(pDieInfo->nDie, tmpSuperBlk);
            if(result < 0)
            {
                _WriteBadBlkFlag(pDieInfo->nDie, tmpSuperBlk);
            }

            continue;
        }

        if(ZoneTblPstInfo[tmpZoneInDie + pDieInfo->nDie * ZONE_CNT_OF_DIE].PhyBlkNum != 0xffff)
        {
            result = _VirtualBlockErase(pDieInfo->nDie, tmpSuperBlk);
            if(result < 0)
            {
                _WriteBadBlkFlag(pDieInfo->nDie, tmpSuperBlk);
            }

            continue;
        }

        tmpPage = _SearchBlkTblPst(pDieInfo->nDie, tmpSuperBlk);

        ZoneTblPstInfo[tmpZoneInDie + pDieInfo->nDie * ZONE_CNT_OF_DIE].PhyBlkNum = tmpSuperBlk;
        ZoneTblPstInfo[tmpZoneInDie + pDieInfo->nDie * ZONE_CNT_OF_DIE].TablePst = tmpPage;

        _VirtualPageRead(pDieInfo->nDie, tmpSuperBlk, tmpPage+DIRTY_FLAG_OFFSET, DIRTY_FLAG_BITMAP, FORMAT_PAGE_BUF, NULL);
        if(FORMAT_PAGE_BUF[0] != 0xff)
        {
            FORMAT_DBG("[FORMAT_DBG] Find the table block %d for zone 0x%x of die 0x%x, but the table is invalid!\n",
                        tmpSuperBlk,tmpZoneInDie, pDieInfo->nDie);
            continue;
        }

        tmpDataBlkTbl = (struct  __SuperPhyBlkType_t *)MALLOC(BLOCK_CNT_OF_ZONE * sizeof(struct __SuperPhyBlkType_t));
        _VirtualPageRead(pDieInfo->nDie, tmpSuperBlk, tmpPage+DATA_TBL_OFFSET, DATA_TABLE_BITMAP, FORMAT_PAGE_BUF, NULL);
        MEMCPY(&tmpDataBlkTbl[0], FORMAT_PAGE_BUF, SECTOR_SIZE * 4);
        _VirtualPageRead(pDieInfo->nDie, tmpSuperBlk, tmpPage+DATA_TBL_OFFSET+1, DATA_TABLE_BITMAP, FORMAT_PAGE_BUF, NULL);
        MEMCPY(&tmpDataBlkTbl[BLOCK_CNT_OF_ZONE / 2], FORMAT_PAGE_BUF, SECTOR_SIZE * 4);
        tmpVar = _CalCheckSum((__u32 *)tmpDataBlkTbl, BLOCK_CNT_OF_ZONE - 1);
        if(tmpVar != ((__u32 *)tmpDataBlkTbl)[BLOCK_CNT_OF_ZONE - 1])
        {
            FORMAT_DBG("[FORMAT_DBG] Find the table block %d for zone 0x%x of die 0x%x,"
                       " but the data block table is invalid!\n",tmpSuperBlk,tmpZoneInDie, pDieInfo->nDie);

            FREE(tmpDataBlkTbl,BLOCK_CNT_OF_ZONE * sizeof(struct __SuperPhyBlkType_t));
            continue;
        }
        FREE(tmpDataBlkTbl,BLOCK_CNT_OF_ZONE * sizeof(struct __SuperPhyBlkType_t));

        _VirtualPageRead(pDieInfo->nDie, tmpSuperBlk, tmpPage+LOG_TBL_OFFSET, LOG_TABLE_BITMAP, FORMAT_PAGE_BUF, NULL);
        tmpVar = _CalCheckSum((__u32 *)FORMAT_PAGE_BUF, LOG_BLK_CNT_OF_ZONE*sizeof(struct __LogBlkType_t)/sizeof(__u32));
        if(tmpVar != ((__u32 *)FORMAT_PAGE_BUF)[511])
        {
            FORMAT_DBG("[FORMAT_DBG] Find the table block for zone 0x%x of die 0x%x,"
                       "but the log block tabl is invalid!\n",tmpZoneInDie, pDieInfo->nDie);
            continue;
        }

        pDieInfo->TblBitmap |= (1 << tmpZoneInDie);

        FORMAT_DBG("[FORMAT_DBG] Search block mapping table for zone 0x%x of die 0x%x successfully!\n",
                    tmpZoneInDie, pDieInfo->nDie);
    }

    for(tmpVar=0; tmpVar<ZONE_CNT_OF_DIE; tmpVar++)
    {
        if(!(pDieInfo->TblBitmap & (1 << tmpVar)))
        {
            FORMAT_DBG("[FORMAT_DBG] Search block mapping table for die 0x%x failed!\n", pDieInfo->nDie);
            return -1;
        }
    }

    return 0;
}


/*
************************************************************************************************************************
*                       REBUILD ZONE TABLES FOR ONE NAND FLASH DIE
*
*Description: Rebuild zone tables for one nand flash die.
*
*Arguments  : pDieInfo   the pointer to the die information whose block mapping table need be rebuiled.
*
*Return     :  rebuild result;
*               = 0     rebuild zone tables successful;
*               < 0     rebuild zone tables failed.
************************************************************************************************************************
*/
static __s32 _RebuildZoneTbls(struct __ScanDieInfo_t *pDieInfo)
{
    __s32 i, j,result;

    pDieInfo->pPhyBlk = (__u16 *)MALLOC(SuperBlkCntOfDie * sizeof(__u16));
    if(!pDieInfo->pPhyBlk)
    {
        FORMAT_ERR("[FORMAT_ERR] Malloc buffer for logical information of physical block failed!\n");

        return -1;
    }
    pDieInfo->ZoneInfo = (struct __ScanZoneInfo_t *) MALLOC(ZONE_CNT_OF_DIE * sizeof(struct __ScanZoneInfo_t));
    if(!pDieInfo->ZoneInfo)
    {
        FORMAT_ERR("[FORMAT_ERR] Malloc buffer for proccess the block mapping table data failed!\n");
        FREE(pDieInfo->pPhyBlk,SuperBlkCntOfDie * sizeof(__u16));

        return -1;
    }

    MEMSET(pDieInfo->pPhyBlk, 0xff, SuperBlkCntOfDie * sizeof(__u16));
    for(i=0; i<ZONE_CNT_OF_DIE; i++)
    {
        pDieInfo->ZoneInfo[i].nDataBlkCnt = 0;
        pDieInfo->ZoneInfo[i].nFreeBlkCnt = 0;
        pDieInfo->ZoneInfo[i].nFreeBlkIndex = 0;
        MEMSET(pDieInfo->ZoneInfo[i].ZoneTbl, 0xff, BLOCK_CNT_OF_ZONE * sizeof(__u32));
        MEMSET(pDieInfo->ZoneInfo[i].LogBlkTbl, 0xff, MAX_LOG_BLK_CNT * sizeof(struct __LogBlkType_t));
		for(j=0;j<MAX_LOG_BLK_CNT;j++)
		{
			pDieInfo->ZoneInfo[i].LogBlkTbl[j].LogBlkType = 0;
			pDieInfo->ZoneInfo[i].LogBlkTbl[j].WriteBlkIndex= 0;
			pDieInfo->ZoneInfo[i].LogBlkTbl[j].ReadBlkIndex= 0;
		}
    }

    if(pDieInfo->nDie == 0)
    {
        pDieInfo->nFreeIndex = DIE0_FIRST_BLK_NUM;
    }
    else
    {
        pDieInfo->nFreeIndex = 0;
    }

    result = _GetBlkLogicInfo(pDieInfo);

    result = _FillZoneTblInfo(pDieInfo);

    result = _KickValidTblBlk(pDieInfo);

    result = _GetMapTblBlock(pDieInfo);
    if(result < 0)
    {
        FORMAT_ERR("[FORMAT_ERR] Get block for saving block mapping table failed in die 0x%x!\n", pDieInfo->nDie);
        FREE(pDieInfo->pPhyBlk,SuperBlkCntOfDie * sizeof(__u16));
        FREE(pDieInfo->ZoneInfo,ZONE_CNT_OF_DIE * sizeof(struct __ScanZoneInfo_t));

        return -1;
    }
	PRINT("_RepairLogBlkTbl start\n");
    result = _RepairLogBlkTbl(pDieInfo);
	PRINT("_RepairLogBlkTbl end\n");

    result = _DistributeFreeBlk(pDieInfo);
    if(result < 0)
    {
        FORMAT_ERR("[FORMAT_ERR] There is not enough free blocks for distribute!\n");

        FREE(pDieInfo->pPhyBlk,SuperBlkCntOfDie * sizeof(__u16));
        FREE(pDieInfo->ZoneInfo,ZONE_CNT_OF_DIE * sizeof(struct __ScanZoneInfo_t));

        return -1;
    }

    result = _WriteBlkMapTbl(pDieInfo);
    if(result < 0)
    {
        FORMAT_ERR("[FORMAT_DBG] Write block mapping table failed!\n");

        FREE(pDieInfo->pPhyBlk,SuperBlkCntOfDie * sizeof(__u16));
        FREE(pDieInfo->ZoneInfo,ZONE_CNT_OF_DIE * sizeof(struct __ScanZoneInfo_t));

        return RET_FORMAT_TRY_AGAIN;
    }

#if DBG_DUMP_DIE_INFO

    _DumpDieInfo(pDieInfo);

#endif

    FREE(pDieInfo->pPhyBlk,SuperBlkCntOfDie * sizeof(__u16));
    FREE(pDieInfo->ZoneInfo,ZONE_CNT_OF_DIE * sizeof(struct __ScanZoneInfo_t));

    return 0;
}


/*
************************************************************************************************************************
*                                   FORMAT NAND FLASH DISK MODULE INIT
*
*Description: Init the nand disk format module, initiate some variables and request resource.
*
*Arguments  : none
*
*Return     : init result;
*               = 0     format module init successful;
*               < 0     format module init failed.
************************************************************************************************************************
*/
#if(0)
__s32 FMT_Init(void)
{
    __s32 i;
	__u32 TmpBlkSize;

	DIE0_FIRST_BLK_NUM = 0;
	/*get block number for boot*/
	for (i = 0; ; i++)
	{
		TmpBlkSize = blks_array[i].blk_size;
		if ( (TmpBlkSize == 0xffffffff) || (TmpBlkSize == SECTOR_CNT_OF_SINGLE_PAGE*PAGE_CNT_OF_PHY_BLK/2))
		{
			DIE0_FIRST_BLK_NUM = blks_array[i].blks_boot0 + blks_array[i].blks_boot1;
			break;
		}

	}

    NandDriverInfo.NandStorageInfo = &NandStorageInfo;
    NandDriverInfo.ZoneTblPstInfo = ZoneTblPstInfo;
    NandDriverInfo.LogicalArchitecture = &LogicArchiPar;
    NandDriverInfo.PageCachePool = &PageCachePool;

    LogicArchiPar.LogicBlkCntPerZone = NandStorageInfo.ValidBlkRatio;
    LogicArchiPar.SectCntPerLogicPage = NandStorageInfo.SectorCntPerPage * NandStorageInfo.PlaneCntPerDie;
    LogicArchiPar.PageCntPerLogicBlk = NandStorageInfo.PageCntPerPhyBlk * NandStorageInfo.BankCntPerChip;
    if(SUPPORT_EXT_INTERLEAVE)
    {
        LogicArchiPar.PageCntPerLogicBlk *= NandStorageInfo.ChipCnt;
    }
    LogicArchiPar.ZoneCntPerDie = (NandStorageInfo.BlkCntPerDie / NandStorageInfo.PlaneCntPerDie) / BLOCK_CNT_OF_ZONE;

    for(i=0; i<MAX_ZONE_CNT; i++)
    {
        ZoneTblPstInfo[i].PhyBlkNum = 0xffff;
        ZoneTblPstInfo[i].TablePst = 0xffff;
    }

    DieCntOfNand = NandStorageInfo.DieCntPerChip / NandStorageInfo.BankCntPerChip;
    if(!SUPPORT_EXT_INTERLEAVE)
    {
        DieCntOfNand *= NandStorageInfo.ChipCnt;
    }
    SuperBlkCntOfDie = NandStorageInfo.BlkCntPerDie / NandStorageInfo.PlaneCntPerDie;

    MEMSET(FORMAT_SPARE_BUF, 0xff, SECTOR_CNT_OF_SUPER_PAGE * 4);

    FORMAT_DBG("\n");
    FORMAT_DBG("[FORMAT_DBG] ===========Logical Architecture Paramter===========\n");
    FORMAT_DBG("[FORMAT_DBG]    Logic Block Count of Zone:  0x%x\n", LogicArchiPar.LogicBlkCntPerZone);
    FORMAT_DBG("[FORMAT_DBG]    Page Count of Logic Block:  0x%x\n", LogicArchiPar.PageCntPerLogicBlk);
    FORMAT_DBG("[FORMAT_DBG]    Sector Count of Logic Page: 0x%x\n", LogicArchiPar.SectCntPerLogicPage);
    FORMAT_DBG("[FORMAT_DBG]    Zone Count of Die:          0x%x\n", LogicArchiPar.ZoneCntPerDie);
    FORMAT_DBG("[FORMAT_DBG] ===================================================\n");

    return 0;
}
#elif(1)
__s32 FMT_Init(void)
{
    __s32 i;
	__u32 TmpBlkSize;

	DIE0_FIRST_BLK_NUM = 0;
	/*get block number for boot*/
	for (i = 0; ; i++)
	{
		TmpBlkSize = blks_array[i].blk_size;
		if ( (TmpBlkSize == 0xffffffff) || (TmpBlkSize == SECTOR_CNT_OF_SINGLE_PAGE*PAGE_CNT_OF_PHY_BLK/2))
		{
			DIE0_FIRST_BLK_NUM = blks_array[i].blks_boot0 + blks_array[i].blks_boot1;
			break;
		}

	}

    NandDriverInfo.NandStorageInfo = &NandStorageInfo;
    NandDriverInfo.ZoneTblPstInfo = ZoneTblPstInfo;
    NandDriverInfo.LogicalArchitecture = &LogicArchiPar;
    NandDriverInfo.PageCachePool = &PageCachePool;

    LogicArchiPar.LogicBlkCntPerZone = NandStorageInfo.ValidBlkRatio;
    LogicArchiPar.SectCntPerLogicPage = NandStorageInfo.SectorCntPerPage * NandStorageInfo.PlaneCntPerDie;
    LogicArchiPar.PageCntPerLogicBlk = NandStorageInfo.PageCntPerPhyBlk * NandStorageInfo.BankCntPerChip;
    if(SUPPORT_EXT_INTERLEAVE)
    {
       if(NandStorageInfo.ChipCnt >=2)
          LogicArchiPar.PageCntPerLogicBlk *= 2;
    }
    LogicArchiPar.ZoneCntPerDie = (NandStorageInfo.BlkCntPerDie / NandStorageInfo.PlaneCntPerDie) / BLOCK_CNT_OF_ZONE;

    for(i=0; i<MAX_ZONE_CNT; i++)
    {
        ZoneTblPstInfo[i].PhyBlkNum = 0xffff;
        ZoneTblPstInfo[i].TablePst = 0xffff;
    }

    DieCntOfNand = NandStorageInfo.DieCntPerChip / NandStorageInfo.BankCntPerChip;
    if(!SUPPORT_EXT_INTERLEAVE)
    {
        DieCntOfNand *= NandStorageInfo.ChipCnt;
    }
    if(SUPPORT_EXT_INTERLEAVE)
    {
       if(NandStorageInfo.ChipCnt >=2)
	   DieCntOfNand *= (NandStorageInfo.ChipCnt/2);
    }
    SuperBlkCntOfDie = NandStorageInfo.BlkCntPerDie / NandStorageInfo.PlaneCntPerDie;

    MEMSET(FORMAT_SPARE_BUF, 0xff, SECTOR_CNT_OF_SUPER_PAGE * 4);

    FORMAT_DBG("\n");
    FORMAT_DBG("[FORMAT_DBG] ===========Logical Architecture Paramter===========\n");
    FORMAT_DBG("[FORMAT_DBG]    Logic Block Count of Zone:  0x%x\n", LogicArchiPar.LogicBlkCntPerZone);
    FORMAT_DBG("[FORMAT_DBG]    Page Count of Logic Block:  0x%x\n", LogicArchiPar.PageCntPerLogicBlk);
    FORMAT_DBG("[FORMAT_DBG]    Sector Count of Logic Page: 0x%x\n", LogicArchiPar.SectCntPerLogicPage);
    FORMAT_DBG("[FORMAT_DBG]    Zone Count of Die:          0x%x\n", LogicArchiPar.ZoneCntPerDie);
    FORMAT_DBG("[FORMAT_DBG] ===================================================\n");

	_LSBPageTypeTabInit();

    return 0;
}
#endif

/*
************************************************************************************************************************
*                                   FORMAT NAND FLASH DISK MODULE EXIT
*
*Description: Exit the nand disk format module, release some resource.
*
*Arguments  : none
*
*Return     : exit result;
*               = 0     format module exit successful;
*               < 0     format module exit failed.
************************************************************************************************************************
*/
__s32 FMT_Exit(void)
{

    return 0;
}


/*
************************************************************************************************************************
*                                   FORMAT NAND FLASH DISK
*
*Description: Format the nand flash disk, create a logical disk area.
*
*Arguments  : none
*
*Return     : format result;
*               = 0     format nand successful;
*               < 0     format nand failed.
*
*Note       : This function look for the mapping information on the nand flash first, if the find all
*             mapping information and check successful, format nand disk successful; if the mapping
*             information has some error, need repair it. If find none mapping information, create it!
************************************************************************************************************************
*/
__s32 FMT_FormatNand(void)
{
    __s32 tmpDie, result, tmpTryAgain;
    struct __ScanDieInfo_t tmpDieInfo;

	result = 0;

    for(tmpDie=0; tmpDie<DieCntOfNand; tmpDie++)
    {
        MEMSET(&tmpDieInfo, 0, sizeof(struct __ScanDieInfo_t));
        tmpDieInfo.nDie = tmpDie;

        result = _SearchZoneTbls(&tmpDieInfo);
#if 0
		{
			PRINT("for debug, force to rebuild nand block table\n");
		    tmpDieInfo.TblBitmap = 0;
		    result = -1;
		}
#endif
		if(result < 0)
        {
            tmpTryAgain = 5;
            while(tmpTryAgain > 0)
            {
                result = _RebuildZoneTbls(&tmpDieInfo);

                if(result != RET_FORMAT_TRY_AGAIN)
                {
                    break;
                }

                tmpTryAgain--;
            }
        }
    }

    if(result < 0)
    {
        FORMAT_ERR("[FORMAT_ERR] Format nand disk failed!\n");
        return -1;
    }

    return 0;
}

void ClearNandStruct( void )
{
    MEMSET(&PageCachePool, 0x00, sizeof(struct __NandPageCachePool_t));
}



