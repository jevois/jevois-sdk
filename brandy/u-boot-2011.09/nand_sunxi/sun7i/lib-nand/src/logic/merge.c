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

__s32 _copy_page0(__u32 SrcBlk,__u16 SrcDataPage,__u32 DstBlk,__u8 SeqPlus)
{
    __u8 seq;
    __u16 LogicInfo;
    struct __NandUserData_t UserData[2];
    struct __PhysicOpPara_t SrcParam,DstParam;

    SrcParam.MDataPtr = DstParam.MDataPtr = LML_TEMP_BUF;
    SrcParam.SDataPtr = DstParam.SDataPtr = (void *)&UserData;
    MEMSET((void *)&UserData,0xff,sizeof(struct __NandUserData_t) * 2);

    /*get seq and logicinfo*/
    SrcParam.SectBitmap = 0x3;
    LML_CalculatePhyOpPar(&SrcParam,CUR_MAP_ZONE, SrcBlk, 0);
    if (LML_VirtualPageRead(&SrcParam) < 0){
        LOGICCTL_ERR("_copy_page0 : read user data err\n");
        return NAND_OP_FALSE;
    }
    seq = UserData[0].PageStatus;
    LogicInfo = UserData[0].LogicInfo;

    /*copy main data */
    SrcParam.SectBitmap = DstParam.SectBitmap = FULL_BITMAP_OF_SUPER_PAGE;
    LML_CalculatePhyOpPar(&SrcParam, CUR_MAP_ZONE, SrcBlk, SrcDataPage);
    LML_CalculatePhyOpPar(&DstParam, CUR_MAP_ZONE, DstBlk, 0);

    if (LML_VirtualPageRead(&SrcParam) < 0){
        LOGICCTL_ERR("_copy_page0 : read main data err\n");
        return NAND_OP_FALSE;
    }

	if((SeqPlus!=0)&&(SeqPlus!=1)&&(SeqPlus!=2))
	{
		PRINT("Error, invalid SeqPlus: %x \n", SeqPlus);
		PRINT("copypage0, SeqPlus, old_seq: %x, new_seq: %x\n", SeqPlus, seq, SeqPlus+seq);
		PRINT("SrcBlk: %x, DstBlk: %x, SrcDataPage: %x \n", SrcBlk, DstBlk, SrcDataPage);
		while(1);
	}
	else
	{
	}

    UserData[0].LogicInfo = LogicInfo;
    UserData[0].PageStatus = seq + SeqPlus;
	if(SeqPlus)
	{
		UserData[0].LogType = 0xff;
		UserData[1].LogType = 0xff;
	}
	else
	{
		UserData[0].LogType &= 0xf;
		UserData[0].LogType &= 0xf;
	}

    if (NAND_OP_TRUE != LML_VirtualPageWrite(&DstParam)){
        LOGICCTL_ERR("_copy_page0 : write err\n");
        return NAND_OP_FALSE;
    }


    return NAND_OP_TRUE;
}

/*!
*
* \par  Description:
*       This function copy valuable data from datablk to logblk,then change datablk to freeblk ,change logblk to datablk.
*
* \param  [in]       LogNum,serial number within log block space
* \return      sucess or failed.
* \note         this function was called when log block is in order,that is to say physical
*             page number is same with logical page number.
**/
__s32  _log2data_swap_merge(__u32 nlogical)
{
    __u16 LastUsedPage,SuperPage;
    struct __SuperPhyBlkType_t DataBlk;
    struct __LogBlkType_t LogBlk;
    struct __PhysicOpPara_t SrcParam,DstParam;

    /* init info of data block and log block*/
    BMM_GetDataBlk(nlogical, &DataBlk);
    BMM_GetLogBlk(nlogical, &LogBlk);
    LastUsedPage = LogBlk.LastUsedPage;

	if((SUPPORT_LOG_BLOCK_MANAGE)&&(LogBlk.LogBlkType == LSB_TYPE))
	{
		 LOGICCTL_ERR("swap merge : LSB type not support swap merge\n");
         return NAND_OP_FALSE;
	}

    /*copy data from data block to log block*/
    for (SuperPage = LastUsedPage + 1; SuperPage < PAGE_CNT_OF_SUPER_BLK; SuperPage++){
        /*set source and destinate address*/
        LML_CalculatePhyOpPar(&SrcParam,CUR_MAP_ZONE, DataBlk.PhyBlkNum, SuperPage);
        LML_CalculatePhyOpPar(&DstParam,CUR_MAP_ZONE, LogBlk.PhyBlk.PhyBlkNum, SuperPage);
        if (NAND_OP_TRUE != PHY_PageCopyback(&SrcParam,&DstParam)){
            LOGICCTL_ERR("swap merge : copy back err\n");
            return NAND_OP_FALSE;
        }
        if (NAND_OP_TRUE !=  PHY_SynchBank(DstParam.BankNum, SYNC_BANK_MODE)){
            struct __SuperPhyBlkType_t SubBlk;
            if (NAND_OP_TRUE != LML_BadBlkManage(&LogBlk.PhyBlk,CUR_MAP_ZONE,SuperPage,&SubBlk)){
                LOGICCTL_ERR("swap merge : bad block manage err after copy back\n");
                return NAND_OP_FALSE;
            }
            LogBlk.PhyBlk = SubBlk;
            SuperPage -= 1;
        }
    }

    /*move log block to data block*/
    BMM_SetDataBlk(nlogical, &LogBlk.PhyBlk);
    /*clear log block item*/
    MEMSET(&LogBlk, 0xff, sizeof(struct __LogBlkType_t));
	LogBlk.LogBlkType = 0;
	LogBlk.WriteBlkIndex = 0;
	LogBlk.ReadBlkIndex = 0;
    BMM_SetLogBlk(nlogical, &LogBlk);

    /*erase data block*/
    if ( NAND_OP_TRUE != LML_VirtualBlkErase(CUR_MAP_ZONE, DataBlk.PhyBlkNum)){
        if (NAND_OP_TRUE != LML_BadBlkManage(&DataBlk,CUR_MAP_ZONE,0,NULL)){
            LOGICCTL_ERR("swap merge : bad block manage err erase data block\n");
            return NAND_OP_FALSE;
        }
    }
	else{
	    /*move erased data block to free block*/
	    if (DataBlk.BlkEraseCnt < 0xffff)
	        DataBlk.BlkEraseCnt ++;
	    BMM_SetFreeBlk(&DataBlk);
	}
    /*clear page map table*/
    PMM_ClearCurMapTbl();

    return  NAND_OP_TRUE;
}

/*!
*
* \par  Description:
*       This function move valuable data from log block to free block,then replace them.
*
* \param  [in]       LogNum,serial number within log block space
* \return      sucess or failed.
* \note         this function was called when log block is full, and valid pages is less than half of one block.
**/
__s32  _free2log_move_merge(__u32 nlogical)
{
    __u8 bank;
    __u16 LastUsedPage,SuperPage;
    __u16 SrcPage,DstPage, SrcBlock, DstBlock;
    struct __SuperPhyBlkType_t FreeBlk,FreeBlk1;
    struct __LogBlkType_t LogBlk;
    struct __PhysicOpPara_t SrcParam,DstParam;
	struct __NandUserData_t UserData[2];

    /*init info of log block , and get one free block */
    BMM_GetLogBlk(nlogical, &LogBlk);

    if (NAND_OP_TRUE != BMM_GetFreeBlk(LOWEST_EC_TYPE, &FreeBlk))
        return NAND_OP_FALSE;

	if((SUPPORT_LOG_BLOCK_MANAGE)&&(LogBlk.LogBlkType == LSB_TYPE))
	{
		if (NAND_OP_TRUE != BMM_GetFreeBlk(LOWEST_EC_TYPE, &FreeBlk1))
        	return NAND_OP_FALSE;
	}


    SrcParam.MDataPtr = DstParam.MDataPtr = NULL;
    SrcParam.SDataPtr = DstParam.SDataPtr = NULL;
    SrcParam.SectBitmap = DstParam.SectBitmap = FULL_BITMAP_OF_SUPER_PAGE;

    if(SUPPORT_ALIGN_NAND_BNK)
    {
        redo:
        /*copy data bank by bank, for copy-back using*/
        LastUsedPage = 0;
        for (bank = 0; bank < INTERLEAVE_BANK_CNT; bank++)
        {
            DstPage = bank;
            for (SuperPage  = bank; SuperPage < PAGE_CNT_OF_SUPER_BLK; SuperPage+= INTERLEAVE_BANK_CNT)
            {
                SrcPage = PMM_GetCurMapPage(SuperPage);
                if (SrcPage != 0xffff)
                {
                	  /*set source and destinate address*/
	                if((SUPPORT_LOG_BLOCK_MANAGE)&&(LogBlk.LogBlkType == LSB_TYPE))
					{
						DBUG_MSG("[DBUG_MSG] _free2log_move_merge 2, select bak log block\n");
						DstPage = PMM_CalNextLogPage(DstPage);
						while((DstPage%INTERLEAVE_BANK_CNT)!=bank)
						{
							DstPage++;
							DstPage = PMM_CalNextLogPage(DstPage);
							if(DstPage>=PAGE_CNT_OF_SUPER_BLK)
								break;
						}

						if(DstPage >= PAGE_CNT_OF_SUPER_BLK)
						{
							LOGICCTL_ERR("move merge : dst page cal error\n");
							return NAND_OP_FALSE;
						}

						if(SrcPage&(0x1<<15))
							SrcBlock = LogBlk.PhyBlk1.PhyBlkNum;
						else
							SrcBlock = LogBlk.PhyBlk.PhyBlkNum;
						DstBlock = FreeBlk.PhyBlkNum;
						SrcPage &= (~(0x1<<15));

					}
					else
					{
						SrcBlock = LogBlk.PhyBlk.PhyBlkNum;
						DstBlock = FreeBlk.PhyBlkNum;
					}

    		 		LML_CalculatePhyOpPar(&SrcParam,CUR_MAP_ZONE, SrcBlock, SrcPage);
                   	LML_CalculatePhyOpPar(&DstParam,CUR_MAP_ZONE, DstBlock, DstPage);
                    if (DstPage == 0)
                    {
                        if ( NAND_OP_FALSE == _copy_page0(SrcBlock,SrcPage,FreeBlk.PhyBlkNum,0))
                        {
                            LOGICCTL_ERR("move merge : copy page 0 err1\n");
                            return NAND_OP_FALSE;
                        }
                    }
                    else
                    {
                        if (NAND_OP_TRUE != PHY_PageCopyback(&SrcParam,&DstParam))
                        {
                            LOGICCTL_ERR("move merge : copy back err\n");
                            return NAND_OP_FALSE;
                        }
                    }

                    if (NAND_OP_TRUE !=  PHY_SynchBank(DstParam.BankNum, SYNC_BANK_MODE))
                    {
                        struct __SuperPhyBlkType_t SubBlk;
                        if (NAND_OP_TRUE != LML_BadBlkManage(&FreeBlk,CUR_MAP_ZONE,0,&SubBlk))
                        {
                            LOGICCTL_ERR("move merge : bad block manage err after copy back\n");
                            return NAND_OP_FALSE;
                        }
                        FreeBlk = SubBlk;
                        goto redo;
                    }

                    PMM_SetCurMapPage(SuperPage,DstPage);
                    DstPage += INTERLEAVE_BANK_CNT;
                }
            }

            /*if bank 0 is empty, need write mange info in page 0*/
            if ((bank == 0) && (DstPage == 0))
            {
                if ( NAND_OP_FALSE == _copy_page0(LogBlk.PhyBlk.PhyBlkNum,0,FreeBlk.PhyBlkNum,0))
                {
                    LOGICCTL_ERR("move merge : copy page 0 err2\n");
                    return NAND_OP_FALSE;
                }
    			LML_CalculatePhyOpPar(&DstParam, CUR_MAP_ZONE, FreeBlk.PhyBlkNum, 0);
                if (NAND_OP_TRUE !=  PHY_SynchBank(DstParam.BankNum, SYNC_BANK_MODE))
                {
                    struct __SuperPhyBlkType_t SubBlk;
                    if (NAND_OP_TRUE != LML_BadBlkManage(&FreeBlk,CUR_MAP_ZONE,0,&SubBlk))
                    {
                        LOGICCTL_ERR("move merge : bad block manage err after copy back\n");
                        return NAND_OP_FALSE;
                    }
                    FreeBlk = SubBlk;
                    goto redo;
                }
            }

            /*reset LastUsedPage*/
            if ((DstPage - INTERLEAVE_BANK_CNT) > LastUsedPage)
            {
                LastUsedPage = DstPage - INTERLEAVE_BANK_CNT;
            }
        }
    }
    else
    {
    	/*copy data page by page*/
    	DstPage = 0;
        LastUsedPage = 0;
    	for (SuperPage = 0; SuperPage < PAGE_CNT_OF_LOGIC_BLK; SuperPage++)
    	{
    		SrcPage = PMM_GetCurMapPage(SuperPage);
    		if (SrcPage != 0xffff)
    		{
    			/*set source and destinate address*/
	            if((SUPPORT_LOG_BLOCK_MANAGE)&&(LogBlk.LogBlkType == LSB_TYPE))
				{
					DBUG_MSG("[DBUG_MSG] _free2log_move_merge 3, select bak log block\n");
					DstPage = PMM_CalNextLogPage(DstPage);
					if(DstPage >= PAGE_CNT_OF_SUPER_BLK)
					{
						LOGICCTL_ERR("move merge : dst page cal error\n");
						return NAND_OP_FALSE;
					}

					if(SrcPage&(0x1<<15))
						SrcBlock = LogBlk.PhyBlk1.PhyBlkNum;
					else
						SrcBlock = LogBlk.PhyBlk.PhyBlkNum;
					DstBlock = FreeBlk.PhyBlkNum;
					SrcPage &= 0x7fff;

				}
				else
				{
					SrcBlock = LogBlk.PhyBlk.PhyBlkNum;
					DstBlock = FreeBlk.PhyBlkNum;
				}

    		 	LML_CalculatePhyOpPar(&SrcParam,CUR_MAP_ZONE, SrcBlock, SrcPage);
                LML_CalculatePhyOpPar(&DstParam,CUR_MAP_ZONE, DstBlock, DstPage);
    			if (0 == DstPage)
    			{
    				if ( NAND_OP_FALSE == _copy_page0(SrcBlock,SrcPage,FreeBlk.PhyBlkNum,0))
                    {
                         LOGICCTL_ERR("move merge : copy page 0 err1\n");
                         return NAND_OP_FALSE;
                    }
    			}
    			else
    			{
    				SrcParam.MDataPtr = DstParam.MDataPtr = LML_TEMP_BUF;
    				SrcParam.SDataPtr = DstParam.SDataPtr = (void *)&UserData;
        			MEMSET((void *)&UserData,0xff,sizeof(struct __NandUserData_t) * 2);
    				SrcParam.SectBitmap = DstParam.SectBitmap = FULL_BITMAP_OF_SUPER_PAGE;
    				if (LML_VirtualPageRead(&SrcParam) < 0){
           				 LOGICCTL_ERR("move merge : read main data err\n");
            			 return NAND_OP_FALSE;
        			}

       				if (NAND_OP_TRUE != LML_VirtualPageWrite(&DstParam)){
            			LOGICCTL_ERR("move merge : write err\n");
            			return NAND_OP_FALSE;
        			}
    			}
    			if (NAND_OP_TRUE !=  PHY_SynchBank(DstParam.BankNum, SYNC_BANK_MODE))
                {
                	struct __SuperPhyBlkType_t SubBlk;
                    if (NAND_OP_TRUE != LML_BadBlkManage(&FreeBlk,CUR_MAP_ZONE,LastUsedPage,&SubBlk))
                    {
                        LOGICCTL_ERR("move merge : bad block manage err after copy back\n");
                    	return NAND_OP_FALSE;
    				}
    				FreeBlk = SubBlk;
    				SuperPage -= 1;
    			}
    			PMM_SetCurMapPage(SuperPage,DstPage);
    			LastUsedPage = DstPage;
    			DstPage++;
    		}
    	}

    }

    /*erase log block*/
    if(NAND_OP_TRUE != LML_VirtualBlkErase(CUR_MAP_ZONE, LogBlk.PhyBlk.PhyBlkNum))
    {
        if(NAND_OP_TRUE != LML_BadBlkManage(&LogBlk.PhyBlk,CUR_MAP_ZONE,0,NULL))
        {
            LOGICCTL_ERR("move merge : bad block manage err after erase log block\n");
            return NAND_OP_FALSE;
        }
    }
	else{
		/*move erased log block to free block*/
	    if(LogBlk.PhyBlk.BlkEraseCnt < 0xffff)
	    {
	        LogBlk.PhyBlk.BlkEraseCnt ++;
	    }
	    BMM_SetFreeBlk(&LogBlk.PhyBlk);
	}
	if((SUPPORT_LOG_BLOCK_MANAGE)&&(LogBlk.LogBlkType == LSB_TYPE))
	{
		if(NAND_OP_TRUE != LML_VirtualBlkErase(CUR_MAP_ZONE, LogBlk.PhyBlk1.PhyBlkNum))
	    {
	        if(NAND_OP_TRUE != LML_BadBlkManage(&LogBlk.PhyBlk1,CUR_MAP_ZONE,0,NULL))
	        {
	            LOGICCTL_ERR("move merge : bad block manage err after erase log block\n");
	            return NAND_OP_FALSE;
	        }
	    }
		else{
			/*move erased log block to free block*/
		    if(LogBlk.PhyBlk1.BlkEraseCnt < 0xffff)
		    {
		        LogBlk.PhyBlk1.BlkEraseCnt ++;
		    }
		    BMM_SetFreeBlk(&LogBlk.PhyBlk1);
		}
	}


    /*move free block to log block*/
    LogBlk.PhyBlk.PhyBlkNum= FreeBlk.PhyBlkNum;
	LogBlk.PhyBlk.BlkEraseCnt= FreeBlk.BlkEraseCnt;
	if((SUPPORT_LOG_BLOCK_MANAGE)&&(LogBlk.LogBlkType == LSB_TYPE))
	{
		DBUG_MSG("[DBUG_MSG] _free2log_move_merge 4, select bak log block\n");
		LogBlk.PhyBlk1.PhyBlkNum= FreeBlk1.PhyBlkNum;
	    LogBlk.PhyBlk1.BlkEraseCnt= FreeBlk1.BlkEraseCnt;
	    LogBlk.WriteBlkIndex = 0;
	    LogBlk.ReadBlkIndex = 0;
		DBUG_MSG("[DBUG] move merge to new log block, logic block: %x, logblock0: %x, logblock1: %x\n", LogBlk.LogicBlkNum, LogBlk.PhyBlk.PhyBlkNum, LogBlk.PhyBlk1.PhyBlkNum);

	}
	else
	{
		LogBlk.LogBlkType = 0;
		LogBlk.WriteBlkIndex = 0;
	    LogBlk.ReadBlkIndex = 0;
	}
	LogBlk.LastUsedPage = LastUsedPage;
    BMM_SetLogBlk(nlogical, &LogBlk);


    return NAND_OP_TRUE;
}

/*!
*
* \par  Description:
*       This function copy valuable data from log block or dat block to free block, change free to data ,change
*       data and log to free.
*
* \param  [in]       LogNum,serial number within log block space
* \return      sucess or failed.
* \note         this function was called when log block is not suit for swap or move.
**/
__s32  _free2data_simple_merge(__u32 nlogical)
{
    __u8 InData;
    __u16 SuperPage;
    __u16 SrcPage,DstPage;
    __u32 SrcBlk,DstBlk;
    struct __SuperPhyBlkType_t DataBlk;
    struct __SuperPhyBlkType_t FreeBlk;
    struct __LogBlkType_t LogBlk;
    struct __PhysicOpPara_t SrcParam,DstParam;

    /*init block info*/
    BMM_GetDataBlk(nlogical,&DataBlk);
	BMM_GetLogBlk(nlogical,&LogBlk);
    if (NAND_OP_TRUE != BMM_GetFreeBlk(LOWEST_EC_TYPE, &FreeBlk))
        return NAND_OP_FALSE;


    /*copy data from data block or log block to free block*/
	if((SUPPORT_LOG_BLOCK_MANAGE)&&(LogBlk.LogBlkType == LSB_TYPE))
	{
		for (SuperPage = 0; SuperPage < PAGE_CNT_OF_LOGIC_BLK; SuperPage++)
	    {
	        /*set source address and destination address*/
	        DstPage = SuperPage;
	        DstBlk = FreeBlk.PhyBlkNum;
	        SrcPage = PMM_GetCurMapPage(SuperPage);
	        InData = (SrcPage == 0xffff)?1 : 0;
			if(InData)
			{
				SrcBlk = DataBlk.PhyBlkNum;
			}
			else
			{
				if(SrcPage&(0x1<<15))
					SrcBlk = LogBlk.PhyBlk1.PhyBlkNum;
				else
					SrcBlk = LogBlk.PhyBlk.PhyBlkNum;
			}
	        SrcPage = InData?SuperPage:(SrcPage&0x7fff);
			LML_CalculatePhyOpPar(&SrcParam, CUR_MAP_ZONE,SrcBlk, SrcPage);
			LML_CalculatePhyOpPar(&DstParam, CUR_MAP_ZONE,DstBlk, DstPage);

	        if (DstPage == 0)
	        {
	            __u8 SeqPlus;
	            SeqPlus = InData?2:1;
	            if(NAND_OP_FALSE == _copy_page0(SrcBlk, SrcPage, DstBlk,SeqPlus))
	            {
	                LOGICCTL_ERR("simple_merge : copy page 0 err\n");
	                return NAND_OP_FALSE;
	            }
	        }
	        else
	        {
	            if(NAND_OP_TRUE != PHY_PageCopyback(&SrcParam,&DstParam))
	            {
	                LOGICCTL_ERR("simple merge : copy back err\n");
	                return NAND_OP_FALSE;
	            }
	        }

	        if(NAND_OP_TRUE != PHY_SynchBank(DstParam.BankNum, SYNC_BANK_MODE))
	        {
	            struct __SuperPhyBlkType_t SubBlk;
	            if(NAND_OP_TRUE != LML_BadBlkManage(&FreeBlk,CUR_MAP_ZONE,DstPage, &SubBlk))
	            {
	                LOGICCTL_ERR("simgple merge : bad block manage err after copy back\n");
	                return NAND_OP_FALSE;
	            }
	            FreeBlk = SubBlk;
	            SuperPage -= 1;
	        }
	    }

	    /*move free block to data block*/
	    BMM_SetDataBlk(nlogical, &FreeBlk);


		/*move erased data block to free block*/
	    if ( NAND_OP_TRUE != LML_VirtualBlkErase(CUR_MAP_ZONE, DataBlk.PhyBlkNum)){
	        if (NAND_OP_TRUE != LML_BadBlkManage(&DataBlk,CUR_MAP_ZONE,0,NULL)){
	            LOGICCTL_ERR("swap merge : bad block manage err erase data block\n");
	            return NAND_OP_FALSE;
	        }
	    }
		else{
		    /*move erased data block to free block*/
		    if (DataBlk.BlkEraseCnt < 0xffff)
		        DataBlk.BlkEraseCnt ++;
		    BMM_SetFreeBlk(&DataBlk);
		}
	    /*move erased log block to free block*/
	    if ( NAND_OP_TRUE != LML_VirtualBlkErase(CUR_MAP_ZONE, LogBlk.PhyBlk.PhyBlkNum)){
	        if (NAND_OP_TRUE != LML_BadBlkManage(&LogBlk.PhyBlk,CUR_MAP_ZONE,0,NULL)){
	            LOGICCTL_ERR("simple merge : bad block manage err after erase log block\n");
	            return NAND_OP_FALSE;
	        }
	    }
		else{
		    if (LogBlk.PhyBlk.BlkEraseCnt < 0xffff)
		        LogBlk.PhyBlk.BlkEraseCnt ++;
		    BMM_SetFreeBlk(&LogBlk.PhyBlk);
		}
		 /*move erased log block to free block*/

		if ( NAND_OP_TRUE != LML_VirtualBlkErase(CUR_MAP_ZONE, LogBlk.PhyBlk1.PhyBlkNum)){
	        if (NAND_OP_TRUE != LML_BadBlkManage(&LogBlk.PhyBlk1,CUR_MAP_ZONE,0,NULL)){
	            LOGICCTL_ERR("simple merge : bad block manage err after erase log block\n");
	            return NAND_OP_FALSE;
	        }
	    }
		else{
		    if (LogBlk.PhyBlk1.BlkEraseCnt < 0xffff)
		        LogBlk.PhyBlk1.BlkEraseCnt ++;
		    BMM_SetFreeBlk(&LogBlk.PhyBlk1);
		}
	    MEMSET(&LogBlk, 0xff, sizeof(struct __LogBlkType_t));
		LogBlk.LogBlkType = 0;
		LogBlk.WriteBlkIndex = 0;
		LogBlk.ReadBlkIndex = 0;
	    BMM_SetLogBlk(nlogical, &LogBlk);

	    /*clear page map table*/
	    PMM_ClearCurMapTbl();
	}
	else
	{
		for (SuperPage = 0; SuperPage < PAGE_CNT_OF_LOGIC_BLK; SuperPage++)
	    {
	        /*set source address and destination address*/
	        DstPage = SuperPage;
	        DstBlk = FreeBlk.PhyBlkNum;
	        SrcPage = PMM_GetCurMapPage(SuperPage);
	        InData = (SrcPage == 0xffff)?1 : 0;
	        SrcBlk = InData?DataBlk.PhyBlkNum : LogBlk.PhyBlk.PhyBlkNum;
	        SrcPage = InData?SuperPage:SrcPage;
			LML_CalculatePhyOpPar(&SrcParam, CUR_MAP_ZONE,SrcBlk, SrcPage);
			LML_CalculatePhyOpPar(&DstParam, CUR_MAP_ZONE,DstBlk, DstPage);

	        if (DstPage == 0)
	        {
	            __u8 SeqPlus;
	            SeqPlus = InData?2:1;
	            if(NAND_OP_FALSE == _copy_page0(SrcBlk, SrcPage, DstBlk,SeqPlus))
	            {
	                LOGICCTL_ERR("simple_merge : copy page 0 err\n");
	                return NAND_OP_FALSE;
	            }
	        }
	        else
	        {
	            if(NAND_OP_TRUE != PHY_PageCopyback(&SrcParam,&DstParam))
	            {
	                LOGICCTL_ERR("simple merge : copy back err\n");
	                return NAND_OP_FALSE;
	            }
	        }

	        if(NAND_OP_TRUE != PHY_SynchBank(DstParam.BankNum, SYNC_BANK_MODE))
	        {
	            struct __SuperPhyBlkType_t SubBlk;
	            if(NAND_OP_TRUE != LML_BadBlkManage(&FreeBlk,CUR_MAP_ZONE,DstPage, &SubBlk))
	            {
	                LOGICCTL_ERR("simgple merge : bad block manage err after copy back\n");
	                return NAND_OP_FALSE;
	            }
	            FreeBlk = SubBlk;
	            SuperPage -= 1;
	        }
	    }

	    /*move free block to data block*/
	    BMM_SetDataBlk(nlogical, &FreeBlk);


		/*move erased data block to free block*/
	    if ( NAND_OP_TRUE != LML_VirtualBlkErase(CUR_MAP_ZONE, DataBlk.PhyBlkNum)){
	        if (NAND_OP_TRUE != LML_BadBlkManage(&DataBlk,CUR_MAP_ZONE,0,NULL)){
	            LOGICCTL_ERR("swap merge : bad block manage err erase data block\n");
	            return NAND_OP_FALSE;
	        }
	    }
		else{
		    /*move erased data block to free block*/
		    if (DataBlk.BlkEraseCnt < 0xffff)
		        DataBlk.BlkEraseCnt ++;
		    BMM_SetFreeBlk(&DataBlk);
		}

	    /*move erased log block to free block*/
	    if ( NAND_OP_TRUE != LML_VirtualBlkErase(CUR_MAP_ZONE, LogBlk.PhyBlk.PhyBlkNum)){
	        if (NAND_OP_TRUE != LML_BadBlkManage(&LogBlk.PhyBlk,CUR_MAP_ZONE,0,NULL)){
	            LOGICCTL_ERR("move merge : bad block manage err after erase log block\n");
	            return NAND_OP_FALSE;
	        }
	    }
		else{
		    if (LogBlk.PhyBlk.BlkEraseCnt < 0xffff)
		        LogBlk.PhyBlk.BlkEraseCnt ++;
		    BMM_SetFreeBlk(&LogBlk.PhyBlk);
		}
	    MEMSET(&LogBlk, 0xff, sizeof(struct __LogBlkType_t));
		LogBlk.LogBlkType = 0;
		LogBlk.WriteBlkIndex = 0;
		LogBlk.ReadBlkIndex = 0;
	    BMM_SetLogBlk(nlogical, &LogBlk);

	    /*clear page map table*/
	    PMM_ClearCurMapTbl();
	}


    return NAND_OP_TRUE;

}


void _get_page_map_tbl_info(__u32 nlogical,__u8 *InOrder, __u16 *nValidPage)
{
    __u16 LastUsedPage,PhysicPage;
    __u32 i;
    struct __LogBlkType_t  LogBlk;

    *InOrder = 1;
    *nValidPage = 0;
    BMM_GetLogBlk(nlogical, &LogBlk);
    LastUsedPage = LogBlk.LastUsedPage;

	if((SUPPORT_LOG_BLOCK_MANAGE)&&(LogBlk.LogBlkType == LSB_TYPE))
	{
		DBUG_MSG("[DBUG_MSG] _get_page_map_tbl_info, select bak log block\n");
		*InOrder = 0;
	}

    for (i = 0; i < PAGE_CNT_OF_SUPER_BLK; i++)
    {
        PhysicPage = PMM_GetCurMapPage(i);
        if (PhysicPage != 0xffff){
            *nValidPage = *nValidPage + 1;
            if (PhysicPage != i)
                *InOrder = 0;
        }
    }

    if (*nValidPage < LastUsedPage + 1)
        *InOrder = 0;
}

/*
************************************************************************************************************************
*                       NAND FLASH LOGIC MANAGE LAYER MERGE LOG BLOCK
*
*Description: Merge the log block whoes mapping table is active.
*
*Arguments  : nMode     the type of the merge;
*                       = 0     normal merge, the log block  table is not full;
*                       = 1     special merge, the log block table is full.
*
*Return     : merge result;
*               = 0     merge log successful;
*               = -1    do bad block manage failed.
************************************************************************************************************************
*/
__s32 LML_MergeLogBlk(__u32 nMode, __u32 nlogical)
{
    __u8 InOrder;
    __u16 nValidPage;
	__s32 ret;

    _get_page_map_tbl_info(nlogical,&InOrder,&nValidPage);

    if (InOrder)
    {
    	ret = (_log2data_swap_merge(nlogical));
    }
    else{
        if ( (nMode == SPECIAL_MERGE_MODE) && (nValidPage < PAGE_CNT_OF_SUPER_BLK/(INTERLEAVE_BANK_CNT+1)))
        {
        	ret =  (_free2log_move_merge(nlogical));
        }
        else
        {
			ret =  (_free2data_simple_merge(nlogical));
        }
    }

	return ret;

}


