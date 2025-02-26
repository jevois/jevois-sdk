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
#include "../include/nand_logic.h"

extern struct __NandDriverGlobal_t     NandDriverInfo;
extern struct __NandPartInfo_t         NandPartInfo[NAND_MAX_PART_CNT];
extern __u8 *lsb_page;

__u32 PMM_CalNextLogPage(__u32 current_page)
{
	__u32 next_page = 0xffff;
	__u32 page_index = current_page;

	if(SUPPORT_LOG_BLOCK_MANAGE)
	{
		if(current_page == 0xffff)
			next_page = 0;
		else
		{
			while(page_index <PAGE_CNT_OF_LOGIC_BLK)
			{
				if(lsb_page[page_index] == 1)
				{
					next_page = page_index;
					break;
				}
				else
				{
					page_index++;
				}
			}

			if(page_index == PAGE_CNT_OF_LOGIC_BLK)
				next_page = PAGE_CNT_OF_LOGIC_BLK;

			if((page_index<PAGE_CNT_OF_LOGIC_BLK)&&(lsb_page[page_index]!= 1))
				PRINT("PMM_CalNextLogPage error, current: %x, next: %x\n", current_page, next_page);
		}
	}
	else
		next_page = current_page;

	DBUG_MSG("[DBUG] PMM_CalNextLogPage, current: %x, next: %x\n", current_page, next_page);

	return next_page;
}
/*
************************************************************************************************************************
*                           CALCULATE THE ACCESS COUNT OF THE LOG BLOCK
*
*Description: Calculate the access count of the log block for make a rule to choose
*             the log for merge.
*
*Arguments  : nLogPst   the position of the log block in the log block table.
*
*Return     : none.
************************************************************************************************************************
*/
static void _CalLogAccessCnt(__u32 nLogPst)
{
    __s32   i;

    if(LOG_ACCESS_TIMER == 0xffff)
    {
        LOG_ACCESS_TIMER = 0;

        for(i=0; i<LOG_BLK_CNT_OF_ZONE; i++)
        {
            LOG_ACCESS_AGE[i] = 0;

        }
    }

    LOG_ACCESS_TIMER++;

    LOG_ACCESS_AGE[nLogPst] = LOG_ACCESS_TIMER;
}


/*
************************************************************************************************************************
*                       GET THE PARAMETER OF DATA BLOCK
*
*Description: Get the parameter of the data block from the data block mapping table.
*
*Arguments  : nBlk      the number of the logical block whose data block need be got;
*             pDataBlk  the pointer to the super block parameter.
*
*Return     : get data block result;
*               = 0    get data block successful;
*               =-1    get data block failed.
************************************************************************************************************************
*/
__s32 BMM_GetDataBlk(__u32 nBlk, struct __SuperPhyBlkType_t *pDataBlk)
{
    if(nBlk > DATA_BLK_CNT_OF_ZONE)
    {
        MAPPING_ERR("[MAPPING_ERR] Logical block number(0x%x) is invalid when get data block!\n", nBlk);
        pDataBlk->BlkEraseCnt = 0xffff;
        pDataBlk->PhyBlkNum = 0xffff;
        return -1;
    }
    else
    {
        pDataBlk->BlkEraseCnt = DATA_BLK_TBL[nBlk].BlkEraseCnt;
        pDataBlk->PhyBlkNum = DATA_BLK_TBL[nBlk].PhyBlkNum;
        return 0;
    }
}


/*
************************************************************************************************************************
*                       SET THE PARAMETER OF DATA BLOCK
*
*Description: set the parameter of the data block to the data block mapping table.
*
*Arguments  : nBlk      the number of the logical block whose data block need be set;
*             pDataBlk  the pointer to the usper block parameter.
*
*Return     : set data block result;
*               = 0    set data block successful;
*               =-1    set data block failed.
************************************************************************************************************************
*/
__s32 BMM_SetDataBlk(__u32 nBlk, struct __SuperPhyBlkType_t *pDataBlk)
{
    if(nBlk > DATA_BLK_CNT_OF_ZONE)
    {
        MAPPING_ERR("[MAPPING_ERR] Logical block number(0x%x) is invalid when set data block!\n", nBlk);
        DATA_BLK_TBL[nBlk].BlkEraseCnt = 0xffff;
        DATA_BLK_TBL[nBlk].PhyBlkNum = 0xffff;

        return -1;
    }
    else
    {
        DATA_BLK_TBL[nBlk].BlkEraseCnt = pDataBlk->BlkEraseCnt;
        DATA_BLK_TBL[nBlk].PhyBlkNum = pDataBlk->PhyBlkNum;
        return 0;
    }
}


/*
************************************************************************************************************************
*                       GET FREE BLOCK FROM FREE BLOCK TABLE
*
*Description: Get a free block from the free block table with highest erase counter or lowest
*             erase counter.
*
*Arguments  : nType     the type of the free block which need be got;
*             pFreeBlk  the pointer to the free block pointer for return.
*
*Return     : get free block result;
*               = 0     get free block successful;
*               =-1     get free block failed.
************************************************************************************************************************
*/
__s32 BMM_GetFreeBlk(__u32 nType, struct __SuperPhyBlkType_t *pFreeBlk)
{
    __s32   i, tmpFreePst = -1;
    __u16   tmpItem = LAST_FREE_BLK_PST + 1;
    __u32   tmpEraseCnt;

    if(nType == LOWEST_EC_TYPE)
    {
        tmpEraseCnt = 0xffff;
    }
    else
    {
        tmpEraseCnt = 0x0000;
    }

    for(i=0; i<FREE_BLK_CNT_OF_ZONE; i++, tmpItem++)
    {
        if(tmpItem >= FREE_BLK_CNT_OF_ZONE)
        {
            tmpItem = 0;
        }

        if(FREE_BLK_TBL[tmpItem].PhyBlkNum != 0xffff)
        {
            if(((nType == LOWEST_EC_TYPE) && (FREE_BLK_TBL[tmpItem].BlkEraseCnt <= tmpEraseCnt))
                || ((nType != LOWEST_EC_TYPE) && (FREE_BLK_TBL[tmpItem].BlkEraseCnt >= tmpEraseCnt)))
            {
                tmpEraseCnt = FREE_BLK_TBL[tmpItem].BlkEraseCnt;

                tmpFreePst = tmpItem;
            }
        }
    }

    if(tmpFreePst < 0)
    {
        MAPPING_ERR("[MAPPING_ERR] There is none free block in the free block table!\n");
        pFreeBlk->PhyBlkNum = 0xffff;
        pFreeBlk->BlkEraseCnt = 0xffff;

        return -1;
    }

    pFreeBlk->PhyBlkNum = FREE_BLK_TBL[tmpFreePst].PhyBlkNum;
    pFreeBlk->BlkEraseCnt = FREE_BLK_TBL[tmpFreePst].BlkEraseCnt;
    LAST_FREE_BLK_PST = tmpFreePst;

    FREE_BLK_TBL[tmpFreePst].PhyBlkNum = 0xffff;
    FREE_BLK_TBL[tmpFreePst].BlkEraseCnt = 0xffff;

	DBUG_MSG("[DBUG] BMM_GetFreeBlk, pos: %x\n", LAST_FREE_BLK_PST);

    return 0;
}


/*
************************************************************************************************************************
*                       SET FREE BLOCK TO FREE BLOCK TABLE
*
*Description: Fill a free block to the free block table.
*
*Arguments  : pFreeBlk      the pointer to the free block which need be fill free block table.
*
*Return     : set free block result;
*               = 0     set free block successful;
*               =-1     set free block failed.
************************************************************************************************************************
*/
__s32 BMM_SetFreeBlk(struct __SuperPhyBlkType_t *pFreeBlk)
{
    __s32   i;

    for(i=0; i<FREE_BLK_CNT_OF_ZONE; i++)
    {
        if(FREE_BLK_TBL[i].PhyBlkNum == 0xffff)
        {
            FREE_BLK_TBL[i].PhyBlkNum = pFreeBlk->PhyBlkNum;
            FREE_BLK_TBL[i].BlkEraseCnt = pFreeBlk->BlkEraseCnt;

			DBUG_MSG("[DBUG] BMM_SetFreeBlk, pos: %x\n", i);
            return 0;
        }
    }

    return -1;
}


/*
************************************************************************************************************************
*                       GET POSITION OF LOG BLOCK IN LOG BLOCK TABLE
*
*Description: Get the position of the log block in the log block table.
*
*Arguments  : nBlk      the logical block number which the log block is belonged to;
*
*Return     : Log block position;
*               >= 0    the position of the log block in log block table.
*               = -1    there is no such log block;
************************************************************************************************************************
*/
static __s32 _GetLogBlkPst(__u32 nBlk)
{
    __s32   i, tmpPst = -1;

    for(i=0; i<LOG_BLK_CNT_OF_ZONE; i++)
    {
        if(LOG_BLK_TBL[i].LogicBlkNum == nBlk)
        {
            tmpPst = i;
            break;
        }
    }

    return tmpPst;
}


/*
************************************************************************************************************************
*                       GET PARAMETER OF LOG BLOCK
*
*Description: Get parameter of log block.
*
*Arguments  : nLogicBlk     the logical block number which the log block is belonged to;
*             pLogBlk       the pointer to the log block item for return;
*
*Return     : get log block result;
*               = 0     get log block successful;
*               =-1     get log block failed.
*
*Note       : Scan the log block table which is accessing in the buffer currently,
*             to look for the log block, if the log block is exsit, return 0,
*             else, return -1
************************************************************************************************************************
*/
__s32 BMM_GetLogBlk(__u32 nLogicBlk, struct __LogBlkType_t *pLogBlk)
{
    __s32   tmpLogPst, result;
	struct __SuperPhyBlkType_t tmpFreeBlk1;
	__u32   LogBlkType;

    tmpLogPst = _GetLogBlkPst(nLogicBlk);

    if(tmpLogPst < 0)
    {
        if(nLogicBlk > DATA_BLK_CNT_OF_ZONE)
        {
            MAPPING_ERR("[MAPPING_ERR] Logical block number(0x%x) is invalid when get log block!\n", nLogicBlk);
        }

        if(pLogBlk != NULL)
        {
            pLogBlk->LogicBlkNum = 0xffff;
            pLogBlk->LastUsedPage = 0xffff;
			pLogBlk->LogBlkType = 0;
			pLogBlk->ReadBlkIndex = 0;
			pLogBlk->WriteBlkIndex = 0;
            pLogBlk->PhyBlk.PhyBlkNum = 0xffff;
            pLogBlk->PhyBlk.BlkEraseCnt = 0xffff;
			pLogBlk->PhyBlk1.PhyBlkNum = 0xffff;
            pLogBlk->PhyBlk1.BlkEraseCnt = 0xffff;
			pLogBlk->PhyBlk2.PhyBlkNum = 0xffff;
            pLogBlk->PhyBlk2.BlkEraseCnt = 0xffff;
        }

        return -1;
    }
    else
    {
        if(pLogBlk != NULL)
        {

			if(SUPPORT_LOG_BLOCK_MANAGE)
			{
				LogBlkType = BMM_CalLogBlkType(nLogicBlk);
				if(LOG_BLK_TBL[tmpLogPst].LogBlkType == 0xffff)
				{
					PRINT("[DBUG] find a log table item with valid log block type!\n");
					LOG_BLK_TBL[tmpLogPst].LogBlkType = 0;
					LOG_BLK_TBL[tmpLogPst].WriteBlkIndex = 0;
					LOG_BLK_TBL[tmpLogPst].ReadBlkIndex = 0;
				}

				if(LOG_BLK_TBL[tmpLogPst].LogBlkType != LogBlkType)
				{
					PRINT("[DBUG] LogBlkTye mismatch: 0x%x, 0x%x\n",LOG_BLK_TBL[tmpLogPst].LogBlkType, LogBlkType);
					if((LOG_BLK_TBL[tmpLogPst].LogBlkType == NORMAL_TYPE)&&(LogBlkType == LSB_TYPE))
					{
					    result = BMM_GetFreeBlk(LOWEST_EC_TYPE, &tmpFreeBlk1);
					    if(result < 0)
					    {
					        MAPPING_ERR("[MAPPING_ERR] Get free block failed when create new log block!\n");
					        return -1;
					    }

						LOG_BLK_TBL[tmpLogPst].LogBlkType = LogBlkType;
						LOG_BLK_TBL[tmpLogPst].WriteBlkIndex = 0;
						LOG_BLK_TBL[tmpLogPst].ReadBlkIndex = 0;
						LOG_BLK_TBL[tmpLogPst].PhyBlk1.PhyBlkNum = tmpFreeBlk1.PhyBlkNum;
						LOG_BLK_TBL[tmpLogPst].PhyBlk1.BlkEraseCnt = tmpFreeBlk1.BlkEraseCnt;

						*pLogBlk = LOG_BLK_TBL[tmpLogPst];

					}

				}
				else
				{
                	*pLogBlk = LOG_BLK_TBL[tmpLogPst];
				}
			}
			else
			{
            	*pLogBlk = LOG_BLK_TBL[tmpLogPst];
			}
        }
    }

    return 0;
}


/*
************************************************************************************************************************
*                   SET LOG BLOCK PARAMETER IN THE LOG BLOCK TABLE
*
*Description: Set the parameter for log block in the log block table.
*
*Arguments  : nLogicBlk     the logical block number which the log block is belonged to;
*             pLogBlk       the pointer to log block parameter which need be set to log block table.
*
*Return     : set log block result;
*               = 0     set log block successful;
*               < 0     set log block failed.
************************************************************************************************************************
*/
__s32 BMM_SetLogBlk(__u32 nLogicBlk, struct __LogBlkType_t *pLogBlk)
{
    __s32   tmpLogPst;

    tmpLogPst = _GetLogBlkPst(nLogicBlk);
    if(tmpLogPst < 0)
    {
        tmpLogPst = _GetLogBlkPst(0xffff);
        if(tmpLogPst < 0)
        {
            MAPPING_ERR("[MAPPING_ERR] Set log block table item failed!\n");

            return -1;
        }
    }

    LOG_BLK_TBL[tmpLogPst] = *pLogBlk;

    return 0;
}

__s32 BMM_CalLogBlkType(__u32 nBlk)
{
	__u32 i;
	__u32 log_block_type = 0;

	if(!SUPPORT_LOG_BLOCK_MANAGE)
		return NORMAL_TYPE;
	else
	{

		for(i=0;i<NAND_MAX_PART_CNT;i++)
		{
			if(NandPartInfo[i].PartType == 0xffff)
				break;

			if((NandPartInfo[i].PartStartLogicBlk<= nBlk)&&(NandPartInfo[i].PartEndLogicBlk> nBlk))
			{
				log_block_type = NandPartInfo[i].PartType;
			}
		}

		if((log_block_type != NORMAL_TYPE)&&(log_block_type != LSB_TYPE))
			log_block_type = NORMAL_TYPE;

		return log_block_type;
	}

}

/*
************************************************************************************************************************
*                       CREATE A NEW LOG BLOCK
*
*Description: Create a new log block.
*
*Arguments  : nBlk      the logical block number of the log block;
*             pLogPst   the pointer to the log block position in the log block table.
*
*Return     : create new log block result.
*               = 0     create new log block successful;
*               =-1     create new log block failed.
************************************************************************************************************************
*/
static __s32 _CreateNewLogBlk(__u32 nBlk, __u32 *pLogPst)
{
    __s32   i, result, LogBlkType,tmpPst=-1;
    __u16   tmpLogAccessAge = 0xffff;
    struct __SuperPhyBlkType_t tmpFreeBlk, tmpFreeBlk1;
    struct __PhysicOpPara_t tmpPhyPage;
    struct __NandUserData_t tmpSpare[2];

    #if CFG_SUPPORT_WEAR_LEVELLING

    if(BLK_ERASE_CNTER >= WEAR_LEVELLING_FREQUENCY)
    {
        LML_WearLevelling();
    }

    #endif
    for(i=0; i<LOG_BLK_CNT_OF_ZONE; i++)
    {
        if(LOG_BLK_TBL[i].LogicBlkNum == 0xffff)
        {
            tmpPst = i;
            break;
        }
    }

    if(tmpPst == -1)
    {
        for(i=0; i<LOG_BLK_CNT_OF_ZONE; i++)
        {
            if(LOG_BLK_TBL[i].LastUsedPage == PAGE_CNT_OF_SUPER_BLK-1)
            {
                tmpPst = i;
                break;
            }
        }

        if(tmpPst == -1)
        {
            for(i=0; i<LOG_BLK_CNT_OF_ZONE; i++)
            {
                if(LOG_ACCESS_AGE[i] < tmpLogAccessAge)
                {
                    tmpLogAccessAge = LOG_ACCESS_AGE[i];
                    tmpPst = i;
                }
            }
        }

        result = PMM_SwitchMapTbl(tmpPst);
        if(result < 0)
        {
            MAPPING_ERR("[MAPPING_ERR] Switch page mapping table failed when create new log block! Err:0x%x\n", result);
            return -1;
        }

        result = LML_MergeLogBlk(NORMAL_MERGE_MODE, LOG_BLK_TBL[tmpPst].LogicBlkNum);
        if(result < 0)
        {
            MAPPING_ERR("[MAPPING_ERR] Merge log block failed when create new log block! Err:0x%x\n", result);
            return -1;
        }
    }

	LogBlkType = BMM_CalLogBlkType(nBlk);

	if((SUPPORT_LOG_BLOCK_MANAGE)&&(LogBlkType == LSB_TYPE))
	{
		DBUG_MSG("[DBUG_MSG] _CreateNewLogBlk, select bak log block\n");

	    result = BMM_GetFreeBlk(LOWEST_EC_TYPE, &tmpFreeBlk);
	    if(result < 0)
	    {
	        MAPPING_ERR("[MAPPING_ERR] Get free block failed when create new log block!\n");
	        return -1;
	    }

	    result = BMM_GetFreeBlk(LOWEST_EC_TYPE, &tmpFreeBlk1);
	    if(result < 0)
	    {
	        MAPPING_ERR("[MAPPING_ERR] Get free block failed when create new log block!\n");
	        return -1;
	    }


	    LOG_BLK_TBL[tmpPst].LogicBlkNum = nBlk;
	    LOG_BLK_TBL[tmpPst].LastUsedPage = 0xffff;
		LOG_BLK_TBL[tmpPst].LogBlkType = LogBlkType;
		LOG_BLK_TBL[tmpPst].WriteBlkIndex = 0;
		LOG_BLK_TBL[tmpPst].ReadBlkIndex = 0;
	    LOG_BLK_TBL[tmpPst].PhyBlk = tmpFreeBlk;
		LOG_BLK_TBL[tmpPst].PhyBlk1 = tmpFreeBlk1;
	    *pLogPst = tmpPst;
	}
	else
	{
	    result = BMM_GetFreeBlk(LOWEST_EC_TYPE, &tmpFreeBlk);
	    if(result < 0)
	    {
	        MAPPING_ERR("[MAPPING_ERR] Get free block failed when create new log block!\n");
	        return -1;
	    }

	    LOG_BLK_TBL[tmpPst].LogicBlkNum = nBlk;
	    LOG_BLK_TBL[tmpPst].LastUsedPage = 0xffff;
		LOG_BLK_TBL[tmpPst].LogBlkType = LogBlkType;
		LOG_BLK_TBL[tmpPst].WriteBlkIndex = 0;
		LOG_BLK_TBL[tmpPst].ReadBlkIndex = 0;
	    LOG_BLK_TBL[tmpPst].PhyBlk = tmpFreeBlk;
	    *pLogPst = tmpPst;
	}


__CHECK_LOGICAL_INFO_OF_DATA_BLOCK:
    LML_CalculatePhyOpPar(&tmpPhyPage, CUR_MAP_ZONE, DATA_BLK_TBL[nBlk].PhyBlkNum, 0);
    tmpPhyPage.SectBitmap = 0x03;
    tmpPhyPage.MDataPtr = LML_TEMP_BUF;
    tmpPhyPage.SDataPtr = (void *)tmpSpare;
    LML_VirtualPageRead(&tmpPhyPage);

    if(tmpSpare[0].LogicInfo == 0xffff)
    {
        tmpSpare[0].BadBlkFlag = 0xff;
        tmpSpare[1].BadBlkFlag = 0xff;
        tmpSpare[0].LogicInfo = ((CUR_MAP_ZONE % ZONE_CNT_OF_DIE)<<10) | nBlk;
        tmpSpare[1].LogicInfo = ((CUR_MAP_ZONE % ZONE_CNT_OF_DIE)<<10) | nBlk;
        tmpSpare[0].LogicPageNum = 0xffff;
        tmpSpare[1].LogicPageNum = 0xffff;
        tmpSpare[0].PageStatus = 0xff;
        tmpSpare[1].PageStatus = 0xff;

        tmpPhyPage.SectBitmap = FULL_BITMAP_OF_SUPER_PAGE;
        result = LML_VirtualPageWrite(&tmpPhyPage);
        if(result < 0)
        {
            LOGICCTL_ERR("[MAPPING_ERR] Physical write module failed when write logical information, Err:0x%x!\n", result);
            return -1;
        }

        result = PHY_SynchBank(tmpPhyPage.BankNum, SYNC_CHIP_MODE);
        if(result < 0)
        {
            LOGICCTL_DBG("[LOGICCTL_DBG] Find a bad block when write logical page! bank:0x%x, block:0x%x, page:0x%x\n",
                    tmpPhyPage.BankNum, tmpPhyPage.BlkNum, tmpPhyPage.PageNum);

            result = LML_BadBlkManage(&DATA_BLK_TBL[nBlk], CUR_MAP_ZONE, 0, &tmpFreeBlk);
            if(result < 0)
            {
                LOGICCTL_ERR("[MAPPING_ERR] Bad block process failed when create new log block, Err:0x%x!\n", result);
                return -1;
            }
            DATA_BLK_TBL[nBlk] = tmpFreeBlk;

            goto __CHECK_LOGICAL_INFO_OF_DATA_BLOCK;
        }
    }

    return 0;
}


/*
************************************************************************************************************************
*                       GET LOG PAGE FOR WRITE
*
*Description: Get a log page for write.
*
*Arguments  : nBlk      the logical block number of the log block;
*             nPage     the number of the logical page, which page need log page;
*             pLogPage  the pointer to the log page number, for return value;
*             pLogPst   the pointer to the position of the log block in the log block table.
*
*Return     : get log page result.
*               = 0     get log page for write successful;
*               =-1     get log page for write failed.
************************************************************************************************************************
*/
static __s32 _GetLogPageForWrite(__u32 nBlk, __u32 nPage, __u16 *pLogPage, __u32 *pLogPst)
{
    __s32   result, tmpLogPst;
    __u16   tmpPage, tempBank;
    struct __PhysicOpPara_t tmpPhyPage;
    struct __NandUserData_t tmpSpare[2];

    tmpLogPst = _GetLogBlkPst(nBlk);
    if(tmpLogPst < 0)
    {
        result = _CreateNewLogBlk(nBlk, (__u32 *)&tmpLogPst);
        if(result < 0)
        {
            MAPPING_ERR("[MAPPING_ERR] Create new log block failed!\n");
            return -1;
        }
    }

    result = PMM_SwitchMapTbl(tmpLogPst);
    if(result < 0)
    {
        MAPPING_ERR("[MAPPING_ERR] Switch page mapping table failed when get log page! Err:0x%x\n", result);
        return -1;
    }

    tmpPage = LOG_BLK_TBL[tmpLogPst].LastUsedPage;

	if(SUPPORT_ALIGN_NAND_BNK)
    {
        if(tmpPage == 0xffff)
        {
            tmpPage = nPage % INTERLEAVE_BANK_CNT;
        }
        else
        {
            if((nPage % INTERLEAVE_BANK_CNT) > (tmpPage % INTERLEAVE_BANK_CNT))
            {
                tmpPage = tmpPage + ((nPage % INTERLEAVE_BANK_CNT) - (tmpPage % INTERLEAVE_BANK_CNT));
            }
            else
            {
                tmpPage = tmpPage + (nPage % INTERLEAVE_BANK_CNT) + (INTERLEAVE_BANK_CNT - (tmpPage % INTERLEAVE_BANK_CNT));
            }
        }
    }
    else
    {

        tmpPage = tmpPage + 1;
    }

	if((SUPPORT_LOG_BLOCK_MANAGE)&&(LOG_BLK_TBL[tmpLogPst].LogBlkType == LSB_TYPE))
	{
		DBUG_MSG("[DBUG_MSG] _GetLogPageForWrite, select bak log block\n");

		if(SUPPORT_ALIGN_NAND_BNK)
		{
			tempBank = tmpPage%INTERLEAVE_BANK_CNT;
			tmpPage =PMM_CalNextLogPage(tmpPage);
			while(tmpPage%INTERLEAVE_BANK_CNT != tempBank)
			{
				tmpPage++;
				tmpPage =PMM_CalNextLogPage(tmpPage);
				if(tmpPage>=PAGE_CNT_OF_SUPER_BLK)
					break;
			}
		}
		else
		{
			tmpPage =PMM_CalNextLogPage(tmpPage);
		}

		if((tmpPage >= PAGE_CNT_OF_SUPER_BLK)&&(LOG_BLK_TBL[tmpLogPst].WriteBlkIndex == 0))
		{
			LOG_BLK_TBL[tmpLogPst].WriteBlkIndex = 1;
			tmpPage = tmpPage - PAGE_CNT_OF_SUPER_BLK;
		}
		if(LOG_BLK_TBL[tmpLogPst].WriteBlkIndex == 1)
			DBUG_MSG("[DBUG_MSG] _GetLogPageForWrite, log block index: %x, log block num: %x, page: %x \n", LOG_BLK_TBL[tmpLogPst].WriteBlkIndex, LOG_BLK_TBL[tmpLogPst].PhyBlk1.PhyBlkNum, tmpPage);
		else
			DBUG_MSG("[DBUG_MSG] _GetLogPageForWrite, log block index: %x, log block num: %x, page: %x \n", LOG_BLK_TBL[tmpLogPst].WriteBlkIndex, LOG_BLK_TBL[tmpLogPst].PhyBlk.PhyBlkNum, tmpPage);
	}

__CHECK_WRITE_LOGICAL_INFO_OF_LOG_BLOCK:
    if((LOG_BLK_TBL[tmpLogPst].LastUsedPage == 0xffff) && (tmpPage != 0))
    {
        LML_CalculatePhyOpPar(&tmpPhyPage, CUR_MAP_ZONE, DATA_BLK_TBL[nBlk].PhyBlkNum, 0);
        tmpPhyPage.SectBitmap = 0x03;
        tmpPhyPage.MDataPtr = LML_TEMP_BUF;
        tmpPhyPage.SDataPtr = (void *)tmpSpare;
        LML_VirtualPageRead(&tmpPhyPage);


        tmpSpare[0].BadBlkFlag = 0xff;
        tmpSpare[1].BadBlkFlag = 0xff;
        tmpSpare[0].LogicInfo = ((CUR_MAP_ZONE % ZONE_CNT_OF_DIE)<<10) | nBlk;
        tmpSpare[1].LogicInfo = ((CUR_MAP_ZONE % ZONE_CNT_OF_DIE)<<10) | nBlk;
        tmpSpare[0].LogicPageNum = 0xffff;
        tmpSpare[1].LogicPageNum = 0xffff;
        tmpSpare[0].PageStatus =  tmpSpare[0].PageStatus + 1;
        tmpSpare[1].PageStatus = tmpSpare[0].PageStatus;
		if((SUPPORT_LOG_BLOCK_MANAGE)&&(LOG_BLK_TBL[tmpLogPst].LogBlkType == LSB_TYPE))
		{
			tmpSpare[0].LogType = LSB_TYPE|(LOG_BLK_TBL[tmpLogPst].WriteBlkIndex<<4);
			tmpSpare[1].LogType = LSB_TYPE|(LOG_BLK_TBL[tmpLogPst].WriteBlkIndex<<4);
		}
		else
		{
			tmpSpare[0].LogType = 0xff;
			tmpSpare[1].LogType = 0xff;
		}

       	if((SUPPORT_LOG_BLOCK_MANAGE)&&(LOG_BLK_TBL[tmpLogPst].LogBlkType == LSB_TYPE))
       	{
       		DBUG_MSG("[DBUG] _GetLogPageForWrite, write the logical information to log page 0, writeblkindex: %x\n", LOG_BLK_TBL[tmpLogPst].WriteBlkIndex);
       		LML_CalculatePhyOpPar(&tmpPhyPage, CUR_MAP_ZONE, LOG_BLK_TBL[tmpLogPst].PhyBlk.PhyBlkNum, 0);
       	}
		else
        	LML_CalculatePhyOpPar(&tmpPhyPage, CUR_MAP_ZONE, LOG_BLK_TBL[tmpLogPst].PhyBlk.PhyBlkNum, 0);

		tmpPhyPage.SectBitmap = FULL_BITMAP_OF_SUPER_PAGE;
        result = LML_VirtualPageWrite(&tmpPhyPage);
        if(result < 0)
        {
            LOGICCTL_ERR("[MAPPING_ERR] Physical write module failed when write logical information, Err:0x%x!\n", result);
            return -1;
        }

        result = PHY_SynchBank(tmpPhyPage.BankNum, SYNC_CHIP_MODE);
        if(result < 0)
        {
            LOGICCTL_DBG("[LOGICCTL_DBG] Find a bad block when write logical page! bank:0x%x, block:0x%x, page:0x%x\n",
                    tmpPhyPage.BankNum, tmpPhyPage.BlkNum, tmpPhyPage.PageNum);

            result = LML_BadBlkManage(&LOG_BLK_TBL[tmpLogPst].PhyBlk, CUR_MAP_ZONE, 0, &LOG_BLK_TBL[tmpLogPst].PhyBlk);
            if(result < 0)
            {
                LOGICCTL_ERR("[MAPPING_ERR] Bad block process failed when get log page for write, Err:0x%x!\n", result);
                return -1;
            }

           goto __CHECK_WRITE_LOGICAL_INFO_OF_LOG_BLOCK;
        }
    }

    *pLogPage = tmpPage;
    *pLogPst = tmpLogPst;

    return 0;
}


/*
************************************************************************************************************************
*                       GET LOG PAGE PARAMETER
*
*Description: Get a page from log block for read or write.
*
*Arguments  : nBlk      the logical block number of the log block;
*             nPage     the number of the logical page, which page need log page;
*             nMode     the type of get log page, 'r' or 'w', others is invalid.
*
*Return     : the number of the log page;
*               != 0xffff   get log page successful, return page number;
*                = 0xffff   get log page failed.
*
*Note       : Scan the log block table to try to get the log block.
*             when the get type is 'r', if the log block is exsit and the logical
*             page contain a log page, return the number of the log page, else,
*             return 0xffff;
*             when the get type is 'w', if the log block is not exsit, need create
*             log block, then, if get log page failed, need merge the log block, and
*             try to get log page again, this mode should return a value page number
*             except there is no enough valid blocks.
************************************************************************************************************************
*/
__u32 PMM_GetLogPage(__u32 nBlk, __u32 nPage, __u8 nMode)
{
    __s32   result, tmpLogPst;
    __u16   tmpPage, PhyPageNum;

    if(nMode == 'r')
    {
        tmpLogPst = _GetLogBlkPst(nBlk);
        if(tmpLogPst < 0)
        {
            return INVALID_PAGE_NUM;
        }

        result = PMM_SwitchMapTbl(tmpLogPst);
        if(result < 0)
        {
            MAPPING_ERR("[MAPPING_ERR] Switch page mapping table failed when get log page! Err:0x%x\n", result);
            return INVALID_PAGE_NUM;
        }

        _CalLogAccessCnt(tmpLogPst);

		if((SUPPORT_LOG_BLOCK_MANAGE)&&(LOG_BLK_TBL[tmpLogPst].LogBlkType == LSB_TYPE))
		{
			PhyPageNum = PAGE_MAP_TBL[nPage].PhyPageNum;
			if((PhyPageNum&(0x1<<15))&&(PhyPageNum!= 0xffff))
			{
				LOG_BLK_TBL[tmpLogPst].ReadBlkIndex = 1;
				PhyPageNum &= 0x7fff;
			}
			else
				LOG_BLK_TBL[tmpLogPst].ReadBlkIndex = 0;

	        return (PhyPageNum|LOG_BLK_TBL[tmpLogPst].ReadBlkIndex<<16);
		}
		else
		{
			LOG_BLK_TBL[tmpLogPst].ReadBlkIndex = 0;
			return PAGE_MAP_TBL[nPage].PhyPageNum;
		}
    }

    result = _GetLogPageForWrite(nBlk, nPage, &tmpPage, (__u32 *)&tmpLogPst);
    if(result < 0)
    {
        MAPPING_ERR("[MAPPING_ERR] Get log page for write failed!\n");
        return INVALID_PAGE_NUM;
    }

    if(!(tmpPage < PAGE_CNT_OF_SUPER_BLK))
    {
        result = LML_MergeLogBlk(SPECIAL_MERGE_MODE, nBlk);
        if(result < 0)
        {
            MAPPING_ERR("[MAPPING_ERR] Merge log block failed when get log page! Err:0x%x\n", result);
            return INVALID_PAGE_NUM;
        }

        result = _GetLogPageForWrite(nBlk, nPage, &tmpPage, (__u32 *)&tmpLogPst);
        if(result < 0)
        {
            MAPPING_ERR("[MAPPING_ERR] Get log page for write failed!\n");
            return INVALID_PAGE_NUM;
        }
    }

    if(!(tmpPage < PAGE_CNT_OF_SUPER_BLK))
    {
        MAPPING_ERR("[MAPPING_ERR] Get log page for write failed!\n");
        return INVALID_PAGE_NUM;
    }
    else
    {
        LOG_BLK_TBL[tmpLogPst].LastUsedPage = tmpPage;
    }

    if((SUPPORT_LOG_BLOCK_MANAGE)&&(LOG_BLK_TBL[tmpLogPst].LogBlkType == LSB_TYPE))
    {
    	DBUG_MSG("[DBUG_MSG] PMM_GetLogPage 2, select bak log block\n");
    	PAGE_MAP_TBL[nPage].PhyPageNum = tmpPage|((LOG_BLK_TBL[tmpLogPst].WriteBlkIndex&0x1)<<15);
    }
	else
		PAGE_MAP_TBL[nPage].PhyPageNum = tmpPage;

    PAGE_MAP_CACHE->DirtyFlag = 1;

    _CalLogAccessCnt(tmpLogPst);

	if((SUPPORT_LOG_BLOCK_MANAGE)&&(LOG_BLK_TBL[tmpLogPst].LogBlkType == LSB_TYPE))
		return (tmpPage|LOG_BLK_TBL[tmpLogPst].WriteBlkIndex<<16);
	else
    	return tmpPage;
}


void PMM_ClearCurMapTbl(void)
{
	PAGE_MAP_CACHE->ZoneNum = 0xff;
	PAGE_MAP_CACHE->LogBlkPst = 0xff;
	PAGE_MAP_CACHE->DirtyFlag = 0x0;
}

__u32 PMM_GetCurMapPage(__u16 nLogicalPage)
{
	return PAGE_MAP_TBL[nLogicalPage].PhyPageNum;
}

void PMM_SetCurMapPage(__u16 nLogicalPage,__u16 nPhysicPage)
{
	PAGE_MAP_TBL[nLogicalPage].PhyPageNum = nPhysicPage;
}

