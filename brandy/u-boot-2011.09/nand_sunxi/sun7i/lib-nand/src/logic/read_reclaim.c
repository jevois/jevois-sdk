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

/*
************************************************************************************************************************
*                       NAND FLASH LOGIC MANAGE LAYER READ-RECLAIM
*
*Description: Repair the logic block whose data has reach the limit of the ability of
*             the HW ECC module correct.
*
*Arguments  : nPage     the page address where need be repaired.
*
*Return     : read-reclaim result;
*               = 0     do read-reclaim successful;
*               = -1    do read-reclaim failed.
*
*Notes      : if read a physical page millions of times, there may be some bit error in
*             the page, and the error bit number will increase along with the read times,
*             so, if the number of the error bit reachs the ECC limit, the data should be
*             read out and write to another physical blcok.
************************************************************************************************************************
*/
__s32 LML_ReadReclaim(__u32 nPage)
{
    __s32   result;

    #if CFG_SUPPORT_READ_RECLAIM

	LOGICCTL_ERR("[LOGICCTL_ERR] read reclaim go\n");

    result = LML_FlushPageCache();
    if(result < 0)
    {
        LOGICCTL_ERR("[LOGICCTL_ERR] Flush page cache failed when do read reclaim! Error:0x%x\n", result);
        return -1;
    }

    result = LML_PageRead(nPage, FULL_BITMAP_OF_LOGIC_PAGE, LML_WRITE_PAGE_CACHE);
    if(result < 0)
    {
        return -1;
    }

    result = LML_PageWrite(nPage, FULL_BITMAP_OF_LOGIC_PAGE, LML_WRITE_PAGE_CACHE);
    if(result < 0)
    {
        return -1;
    }

    #endif
    return 0;
}


