/*
************************************************************************************************************************
*                                                      eNand
*                                   Nand flash driver data struct type define
*
*                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : nand_type.h
*
* Author : Kevin.z
*
* Version : v0.1
*
* Date : 2008.03.19
*
* Description : This file defines the data struct type and return value type for nand flash driver.
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Kevin.z         2008.03.19      0.1          build the file
*
************************************************************************************************************************
*/
#ifndef __NAND_TYPE_H
#define __NAND_TYPE_H

#include "nand_drv_cfg.h"


struct __OptionalPhyOpPar_t
{
    __u8        MultiPlaneReadCmd[2];              
    __u8        MultiPlaneWriteCmd[2];             
    __u8        MultiPlaneCopyReadCmd[3];          
    __u8        MultiPlaneCopyWriteCmd[3];         
    __u8        MultiPlaneStatusCmd;               
    __u8        InterBnk0StatusCmd;                
    __u8        InterBnk1StatusCmd;                
    __u8        BadBlockFlagPosition;              
    __u16       MultiPlaneBlockOffset;             
};

typedef struct
{
    __u8        ChipCnt;                           
    __u16       ChipConnectInfo;                   
	__u8		RbCnt;
	__u8		RbConnectInfo;					
    __u8        RbConnectMode;					
	__u8        BankCntPerChip;                    
    __u8        DieCntPerChip;                     
    __u8        PlaneCntPerDie;                    
    __u8        SectorCntPerPage;                  
    __u16       PageCntPerPhyBlk;                  
    __u16       BlkCntPerDie;                      
    __u32       OperationOpt;                      
    __u8        FrequencePar;                      
    __u8        EccMode;                           
    __u8        NandChipId[8];                     
    __u16       ValidBlkRatio;                     
	__u32 		good_block_ratio;				
	__u32		ReadRetryType;					
	__u32       DDRType;
	__u32		Reserved[32];
}boot_nand_para_t;

typedef struct boot_flash_info{
	__u32 chip_cnt;
	__u32 blk_cnt_per_chip;
	__u32 blocksize;
	__u32 pagesize;
	__u32 pagewithbadflag; /*bad block flag was written at the first byte of spare area of this page*/
}boot_flash_info_t;

struct __NandStorageInfo_t
{
    __u8        ChipCnt;                           
    __u16       ChipConnectInfo;                   
	__u8		RbCnt;
	__u8		RbConnectInfo;					
    __u8        RbConnectMode;					
	__u8        BankCntPerChip;                    
    __u8        DieCntPerChip;                     
    __u8        PlaneCntPerDie;                    
    __u8        SectorCntPerPage;                  
    __u16       PageCntPerPhyBlk;                  
    __u16       BlkCntPerDie;                      
    __u32       OperationOpt;                      
    __u8        FrequencePar;                      
    __u8        EccMode;                           
    __u8        NandChipId[8];                     
    __u16       ValidBlkRatio;                        
    __u32		ReadRetryType;					
    __u32       DDRType;
    struct __OptionalPhyOpPar_t OptPhyOpPar;       
};


struct __NandPageCachePool_t
{
    __u8        *PageCache0;                       
    __u8        *PageCache1;                       
    __u8        *PageCache2;                       
    __u8		*SpareCache;

	__u8		*TmpPageCache;
};

#ifdef __OS_LINUX_SYSTEM__
struct __NandUserData_t
{
    __u8        BadBlkFlag;                        
    __u16       LogicInfo;                         
    __u8        Reserved0;                         
    __u16       LogicPageNum;                      
    __u8        PageStatus;                        
    __u8        LogType;                         
} __attribute__ ((packed));
#else
__packed struct __NandUserData_t
{
    __u8        BadBlkFlag;                        
    __u16       LogicInfo;                         
    __u8        Reserved0;                         
    __u16       LogicPageNum;                      
    __u8        PageStatus;                        
    __u8        LogType;                         
} ;

#endif


struct __PhysicOpPara_t
{
    __u8        BankNum;                           
    __u8        PageNum;                           
    __u16       BlkNum;                            
    __u64       SectBitmap;                        
    void        *MDataPtr;                         
    void        *SDataPtr;                         
};



struct __LogicArchitecture_t
{
    __u16       LogicBlkCntPerZone;                
    __u16       PageCntPerLogicBlk;                
    __u8        SectCntPerLogicPage;               
    __u8        ZoneCntPerDie;                     
    __u16       Reserved;                          
};

struct __SuperPhyBlkType_t
{
    __u16       PhyBlkNum;                         
    __u16       BlkEraseCnt;                       
};


struct __LogBlkType_t
{
    __u16       LogicBlkNum;                       
    __u16       LogBlkType;
    __u16       LastUsedPage;                      
	__u16       Reserved0;
	__u16		WriteBlkIndex;
	__u16       ReadBlkIndex;
    struct __SuperPhyBlkType_t PhyBlk;             
    struct __SuperPhyBlkType_t PhyBlk1;
	struct __SuperPhyBlkType_t PhyBlk2;
};


struct __ZoneTblPstInfo_t
{
    __u16       PhyBlkNum;                         
    __u16       TablePst;                          
};


struct __BlkMapTblCache_t
{
    __u8        ZoneNum;                           
    __u8        DirtyFlag;                         
    __u16       AccessCnt;                         
    struct __SuperPhyBlkType_t *DataBlkTbl;        
    struct __LogBlkType_t *LogBlkTbl;              
    struct __SuperPhyBlkType_t *FreeBlkTbl;        
    __u16       LastFreeBlkPst;                    
    __u16       Reserved;                          
};

struct __BlkMapTblCachePool_t
{
    struct __BlkMapTblCache_t *ActBlkMapTbl;                               
    struct __BlkMapTblCache_t BlkMapTblCachePool[BLOCK_MAP_TBL_CACHE_CNT]; 
    __u16       LogBlkAccessAge[MAX_LOG_BLK_CNT];  
    __u16       LogBlkAccessTimer;                 
    __u16       SuperBlkEraseCnt;                  
};


struct __PageMapTblItem_t
{
    __u16       PhyPageNum;                        
};

struct __PageMapTblCache_t
{
    __u8        ZoneNum;                           
    __u8        LogBlkPst;                         
    __u16       AccessCnt;                         
	struct __PageMapTblItem_t *PageMapTbl;         
    __u8        DirtyFlag;                         
    __u8        Reserved[3];                       
};

struct __PageMapTblCachePool_t
{
    struct __PageMapTblCache_t *ActPageMapTbl;                             
    struct __PageMapTblCache_t PageMapTblCachePool[PAGE_MAP_TBL_CACHE_CNT];
};


struct __GlobalLogicPageType_t
{
    __u32       LogicPageNum;                      
    __u64       SectorBitmap;                      
};


struct __LogicPageType_t
{
    __u64       SectBitmap;                        
    __u16       BlockNum;                          
    __u16       PageNum;                           
    __u8        ZoneNum;                           
    __u8        Reserved[3];                       
};


struct __LogicCtlPar_t
{
    __u8        OpMode;                            
    __u8        ZoneNum;                           
    __u16       LogicBlkNum;                       
    __u16       LogicPageNum;                      
    __u16       LogPageNum;                        
    __u16       LogBlockType;
    __u16       WriteBlockIndex;
	__u16       ReadBlockIndex;
    struct __SuperPhyBlkType_t  DataBlkNum;        
    struct __SuperPhyBlkType_t  LogBlkNum;         
    struct __SuperPhyBlkType_t  LogBlkNum1;         
    struct __SuperPhyBlkType_t  LogBlkNum2;         
    __u32       DiskCap;                           
};


struct __NandPhyInfoPar_t
{
    __u8        NandID[8];                         
    __u8        DieCntPerChip;                     
    __u8        SectCntPerPage;                    
    __u16       PageCntPerBlk;                     
    __u16       BlkCntPerDie;                      
    __u32       OperationOpt;                      
    __u16       ValidBlkRatio;                     
    __u16       AccessFreq;                        
    __u16       EccMode;                           
    __u32 		ReadRetryType;
    __u32       DDRType;
    struct __OptionalPhyOpPar_t *OptionOp;         
};

struct __NandPartInfo_t
{
    __u32       PartType;                          
    __u32       PartStartLogicBlk;
	__u32       PartEndLogicBlk;
	__u32       Reserved;
};

struct __NandPartTable_t
{
	__u32 magic;
	__u32 part_cnt;
	__u32 part_type[NAND_MAX_PART_CNT];
	__u32 start_sec[NAND_MAX_PART_CNT];
	__u32 sec_cnt[NAND_MAX_PART_CNT];
};


struct __NandDriverGlobal_t
{
    struct __NandStorageInfo_t  *NandStorageInfo;              
    struct __ZoneTblPstInfo_t   *ZoneTblPstInfo;               
    struct __BlkMapTblCachePool_t   *BlkMapTblCachePool;       
    struct __PageMapTblCachePool_t  *PageMapTblCachePool;      
    struct __LogicArchitecture_t    *LogicalArchitecture;      
    struct __NandPageCachePool_t    *PageCachePool;            
    struct __NandPartInfo_t *NandPartInfo;
};



#define NAND_CACHE_READ         (1<<0)             
#define NAND_CACHE_PROGRAM      (1<<1)             
#define NAND_MULTI_READ         (1<<2)             
#define NAND_MULTI_PROGRAM      (1<<3)             
#define NAND_PAGE_COPYBACK      (1<<4)             
#define NAND_INT_INTERLEAVE     (1<<5)             
#define NAND_EXT_INTERLEAVE     (1<<6)             
#define NAND_RANDOM		        (1<<7)			   
#define NAND_READ_RETRY	        (1<<8)			   
#define NAND_READ_UNIQUE_ID	    (1<<9)			   
#define NAND_PAGE_ADR_NO_SKIP	(1<<10)			   
#define NAND_DIE_SKIP           (1<<11)            
#define NAND_LOG_BLOCK_MANAGE   (1<<12)            
#define NAND_LOG_BLOCK_LSB_TYPE (0xff<<16)         


#define NORMAL_TYPE    0
#define LSB_TYPE       1

#define NAND_OPERATE_FAIL       (1<<0)             
#define NAND_CACHE_READY        (1<<5)             
#define NAND_STATUS_READY       (1<<6)             
#define NAND_WRITE_PROTECT      (1<<7)             


#define FREE_PAGE_MARK          0xff               
#define DATA_PAGE_MARK          0x55               
#define TABLE_PAGE_MARK         0xaa               

#define TABLE_BLK_MARK          0xaa               
#define BOOT_BLK_MARK           0xbb               

#define BLOCK_CNT_OF_ZONE       1024               

#define SECTOR_SIZE             512                



#define NAND_OP_TRUE            (0)                    
#define NAND_OP_FALSE           (-1)                   


#define ECC_LIMIT               10                 
#define ERR_MALLOC              11                 
#define ERR_ECC                 12                 
#define ERR_NANDFAIL            13                 
#define ERR_TIMEOUT             14                 
#define ERR_PHYSIC              15                 
#define ERR_SCAN                16                 
#define ERR_FORMAT              17                 
#define ERR_MAPPING             18                 
#define ERR_LOGICCTL            19                 
#define ERR_ADDRBEYOND          20                 
#define ERR_INVALIDPHYADDR		  21

#endif

