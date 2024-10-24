/*
************************************************************************************************************************
*                                                      eNand
*                                     Nand flash driver format module define
*
*                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : nand_format.h
*
* Author : Kevin.z
*
* Version : v0.1
*
* Date : 2008.03.25
*
* Description : This file define the function interface and some data structure export
*               for the format module.
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Kevin.z         2008.03.25      0.1          build the file
*
************************************************************************************************************************
*/
#ifndef __NAND_FORMAT_H__
#define __NAND_FORMAT_H__

#include "nand_type.h"
#include "nand_physic.h"


struct __ScanZoneInfo_t
{
    __u16   nDataBlkCnt;               
    __u16   nFreeBlkCnt;               
    __u16   nFreeBlkIndex;             
    __u16   Reserved;                  
    struct __SuperPhyBlkType_t ZoneTbl[BLOCK_CNT_OF_ZONE]; 
	struct __LogBlkType_t LogBlkTbl[MAX_LOG_BLK_CNT];      
};


struct __ScanDieInfo_t
{
    __u8    nDie;                      
    __u8    TblBitmap;                 
    __u16   nBadCnt;                   
    __u16   nFreeCnt;                  
    __u16   nFreeIndex;                
    __u16   *pPhyBlk;                  
    struct __ScanZoneInfo_t *ZoneInfo; 
};

typedef struct _blk_for_boot1_t
{
	__u32 blk_size;
	__u32 blks_boot0;
	__u32 blks_boot1;
}blk_for_boot1_t;

#define TBL_AREA_BLK_NUM    32

#define DATA_TABLE_BITMAP   0xf        
#define LOG_TABLE_BITMAP    0xf        
#define DIRTY_FLAG_BITMAP   0x1        

#define DATA_TBL_OFFSET     0          
#define LOG_TBL_OFFSET      2          
#define DIRTY_FLAG_OFFSET   3          
#define PAGE_CNT_OF_TBL_GROUP       4  

#define FORMAT_PAGE_BUF     (PageCachePool.PageCache0)

#define FORMAT_SPARE_BUF    (PageCachePool.SpareCache)

#define NULL_BLOCK_INFO     0xfffd
#define BAD_BLOCK_INFO      0xfffe
#define FREE_BLOCK_INFO     0xffff

#define ALLOC_BLK_MARK      ((0x1<<14) | 0xff)


#define COMPARE_AGE(a, b)           ((signed char)((signed char)(a) - (signed char)(b)))

#define GET_LOGIC_INFO_ZONE(a)      ((((unsigned int)(a))>>10) & 0x0f)
#define GET_LOGIC_INFO_TYPE(a)      ((((unsigned int)(a))>>14) & 0x01)
#define GET_LOGIC_INFO_BLK(a)       (((unsigned int)(a)) & 0x03ff)

#define RET_FORMAT_TRY_AGAIN        (-2)
	
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
__s32 FMT_Init(void);


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
__s32 FMT_Exit(void);


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
__s32 FMT_FormatNand(void);

void ClearNandStruct( void );


#endif 


