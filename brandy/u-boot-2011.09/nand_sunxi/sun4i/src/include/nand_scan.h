/*
************************************************************************************************************************
*                                                      eNand
*                                     Nand flash driver scan module define
*
*                             Copyright(C), 2008-2009, SoftWinners Microelectronic Co., Ltd.
*											       All Rights Reserved
*
* File Name : nand_scan.h
*
* Author : Kevin.z
*
* Version : v0.1
*
* Date : 2008.03.25
*
* Description : This file define the function __s32erface and some data structure export
*               for the nand flash scan module.
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
#ifndef __NAND_SCAN_H__
#define __NAND_SCAN_H__

#include "nand_type.h"
#include "nand_physic.h"


#define TOSHIBA_NAND            0x98               
#define SAMSUNG_NAND            0xec               
#define HYNIX_NAND              0xad               
#define MICRON_NAND             0x2c               
#define ST_NAND                 0x20               
#define INTEL_NAND              0x89               
#define SPANSION_NAND           0x01               
#define POWER_NAND              0x92               
#define SANDISK                 0x45               



/*
************************************************************************************************************************
*                           ANALYZE NAND FLASH STORAGE SYSTEM
*
*Description: Analyze nand flash storage system, generate the nand flash physical
*             architecture parameter and connect information.
*
*Arguments  : none
*
*Return     : analyze result;
*               = 0     analyze successful;
*               < 0     analyze failed, can't recognize or some other error.
************************************************************************************************************************
*/
__s32  SCN_AnalyzeNandSystem(void);

__u32 NAND_GetValidBlkRatio(void);
__s32 NAND_SetValidBlkRatio(__u32 ValidBlkRatio);
__u32 NAND_GetFrequencePar(void);
__s32 NAND_SetFrequencePar(__u32 FrequencePar);
__u32 NAND_GetNandVersion(void);
__s32 NAND_GetParam(boot_nand_para_t * nand_param);

#endif 
