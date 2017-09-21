/*
************************************************************************************************************************
*                                                      eNand
*                                     Nand flash driver logic control module define
*
*                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : bsp_nand.h
*
* Author : Kevin.z
*
* Version : v0.1
*
* Date : 2008.03.25
*
* Description : This file define the function interface and some data structure export
*               for the nand bsp.
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
#ifndef __BSP_FOR_BOOT1_H__
#define __BSP_FOR_BOOT1_H__





#define SUCCESS	0
#define FAIL	-1
#define BADBLOCK -2
#define  NAND_VERSION_0                 0x02
#define  NAND_VERSION_1                 0x11

typedef struct
{
	__u32		ChannelCnt;
	__u32        ChipCnt;                           
    __u32       ChipConnectInfo;                   
	__u32		RbCnt;
	__u32		RbConnectInfo;					
    __u32        RbConnectMode;					
	__u32        BankCntPerChip;                    
    __u32        DieCntPerChip;                     
    __u32        PlaneCntPerDie;                    
    __u32        SectorCntPerPage;                  
    __u32       PageCntPerPhyBlk;                  
    __u32       BlkCntPerDie;                      
    __u32       OperationOpt;                      
    __u32        FrequencePar;                      
    __u32        EccMode;                           
    __u8        NandChipId[8];                     
    __u32       ValidBlkRatio;                     
	__u32 		good_block_ratio;				
	__u32		ReadRetryType;					
	__u32       DDRType;
	__u32		Reserved[22];
}boot_nand_para_t;

typedef struct boot_flash_info{
	__u32 chip_cnt;
	__u32 blk_cnt_per_chip;
	__u32 blocksize;
	__u32 pagesize;
	__u32 pagewithbadflag; /*bad block flag was written at the first byte of spare area of this page*/
}boot_flash_info_t;


struct boot_physical_param{
	__u32   chip;
	__u32  block;
	__u32  page;
	__u32  sectorbitmap;
	void   *mainbuf;
	void   *oobbuf;
};

#define ND_MAX_PART_COUNT		15	 								

struct nand_disk{
	unsigned long size;
	unsigned long offset;
	unsigned char type;
};


extern __s32 LML_Init(void);
extern __s32 LML_Exit(void);
extern __s32 LML_Read(__u32 nLba, __u32 nLength, void* pBuf);
extern __s32 LML_Write(__u32 nLba, __u32 nLength, void* pBuf);
extern __s32 LML_FlushPageCache(void);

extern __s32 BMM_RleaseLogBlock(__u32 log_level);
extern __s32 BMM_WriteBackAllMapTbl(void);

extern __s32 NAND_CacheFlush(void);
extern __s32 NAND_CacheFlushDev(__u32 dev_num);
extern __s32 NAND_CacheRead(__u32 blk, __u32 nblk, void *buf);
extern __s32 NAND_CacheWrite(__u32 blk, __u32 nblk, void *buf);
extern __s32 NAND_CacheOpen(void);
extern __s32 NAND_CacheClose(void);

extern __s32 FMT_Init(void);
extern __s32 FMT_Exit(void);
extern __s32 FMT_FormatNand(void);
extern void  ClearNandStruct( void );

__s32  SCN_AnalyzeNandSystem(void);

extern __s32 PHY_Init(void);
extern __s32 PHY_Exit(void);
extern __s32 PHY_ChangeMode(__u8 serial_mode);
extern __s32 PHY_ScanDDRParam(void);

extern __s32 PHY_SimpleErase(struct boot_physical_param * eraseop);
extern __s32 PHY_SimpleErase_2CH(struct boot_physical_param * eraseop);
extern __s32 PHY_SimpleRead(struct boot_physical_param * readop);
extern __s32 PHY_SimpleRead_2CH(struct boot_physical_param * readop);
extern __s32 PHY_SimpleWrite(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleWrite_1K(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleWrite_Seq(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleRead_Seq(struct boot_physical_param * readop);
extern __s32 PHY_SimpleRead_1K(struct boot_physical_param * readop);
extern __s32 BOOT_AnalyzeNandSystem(void);

extern __u32 NAND_GetValidBlkRatio(void);
extern __s32 NAND_SetValidBlkRatio(__u32 ValidBlkRatio);
extern __u32 NAND_GetFrequencePar(void);
extern __s32 NAND_SetFrequencePar(__u32 FrequencePar);
extern __u32 NAND_GetNandVersion(void);
extern __s32 NAND_GetParam(boot_nand_para_t * nand_param);
extern __s32 NAND_GetFlashInfo(boot_flash_info_t *info);
extern __u32 NAND_GetDiskSize(void);
extern void  NAND_SetSrcClkName(__u32 pll_name);

extern __s32 NFC_LSBEnable(__u32 chip, __u32 read_retry_type);
extern __s32 NFC_LSBDisable(__u32 chip, __u32 read_retry_type);
extern __s32 NFC_LSBInit(__u32 read_retry_type);
extern __s32 NFC_LSBExit(__u32 read_retry_type);

extern void NFC_RbIntEnable(void);
extern void NFC_RbIntDisable(void);
extern void NFC_RbIntClearStatus(void);
extern __u32 NFC_RbIntGetStatus(void);
extern __u32 NFC_GetRbSelect(void);
extern __u32 NFC_GetRbStatus(__u32 rb);
extern __u32 NFC_RbIntOccur(void);

extern void NFC_DmaIntEnable(void);
extern void NFC_DmaIntDisable(void);
extern void NFC_DmaIntClearStatus(void);
extern __u32 NFC_DmaIntGetStatus(void);
extern __u32 NFC_DmaIntOccur(void);

extern int mbr2disks(struct nand_disk* disk_array);



/*
*   Description:
*   1. if u wanna set dma callback hanlder(sleep during dma transfer) to free cpu for other tasks,
*      one must call the interface before nand flash initialization.
      this option is also protected by dma poll method,wakup(succeed or timeout) then check
      dma transfer complete or still running.
*   2. if u use dma poll method,no need to call the interface.
*
*   3. default is unuse dma callback hanlder,that is,dma poll method.
*   4. input para  : 0:dma poll method;  1:dma callback isr,free cpu for other tasks.
*   5. return value: 0:set succeed; -1:set failed.
*/
extern __s32 NAND_SetDrqCbMethod(__u32 used);


#endif 

