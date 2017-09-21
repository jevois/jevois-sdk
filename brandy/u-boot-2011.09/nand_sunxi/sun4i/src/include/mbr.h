/*
 *================================================================================================
 *
 *                                            Aone project ---- tools
 *
 *                             Copyright(C), 2006-2008, Microelectronic Co., Ltd.
 *											       All Rights Reserved
 *
 * File Name :  MBR.h
 *
 * Author : javen
 *
 * Version : 1.0
 *
 * Date : 2008.12.02
 *
 * Description :
 *
 * History :
 *================================================================================================
*/

#ifndef    __ND_MBR_H__
#define    __ND_MBR_H__

#include "nand_drv_cfg.h"

#define ND_MAX_PART_COUNT		15	 								
#define ND_MBR_COPY_NUM        4    								

#define ND_MBR_START_ADDRESS	0x0									
#define ND_MBR_SIZE			1024								
#define ND_MBR_RESERVED        (ND_MBR_SIZE - 20 - (ND_MAX_PART_COUNT * 64))

extern struct __NandDriverGlobal_t NandDriverInfo;

extern struct __NandStorageInfo_t  NandStorageInfo;

#define DiskSize  (SECTOR_CNT_OF_SINGLE_PAGE * PAGE_CNT_OF_PHY_BLK * BLOCK_CNT_OF_DIE * \
            DIE_CNT_OF_CHIP * NandStorageInfo.ChipCnt  / 1024 * DATA_BLK_CNT_OF_ZONE)


struct nand_disk{
	unsigned long size;
	unsigned long offset;
	unsigned char type;
};

/* part info */
typedef struct nand_tag_PARTITION{
	__u32 addrhi;			
	__u32 addrlo;			
	__u32 lenhi;			
	__u32 lenlo;			
	__u8  classname[12];	
	__u8  name[12];			
	unsigned  int       user_type;          //标志当前盘符所属于的用户
	unsigned  int       ro;                 //标志当前盘符的读写属性
	__u8  res[16];			
}ND_PARTITION;

/* mbr info */
typedef struct nand_tag_MBR{
	__u32 crc32;				
	__u32 version;				
	__u8  magic[8];				
	__u8  copy;					
	__u8  index;				
	__u16   PartCount;			
	ND_PARTITION array[ND_MAX_PART_COUNT];// part info
	__u8 res[ND_MBR_RESERVED];        
}ND_MBR;

int mbr2disks(struct nand_disk* disk_array);
int NAND_PartInit(void);

#endif   
