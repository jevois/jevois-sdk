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
#include "../include/nand_type.h"
#include "../include/nand_physic.h"
#include "../include/nand_simple.h"
#include "../include/nfc.h"
#include "../include/nand_drv_cfg.h"

__u32 dumy_read(__u32 bank);


struct __NandStorageInfo_t  NandStorageInfo={0};
struct __NandPageCachePool_t PageCachePool={0};
__u32 RetryCount[8]={0};
const __u16 random_seed[128] = {
	0x2b75, 0x0bd0, 0x5ca3, 0x62d1, 0x1c93, 0x07e9, 0x2162, 0x3a72, 0x0d67, 0x67f9,
    0x1be7, 0x077d, 0x032f, 0x0dac, 0x2716, 0x2436, 0x7922, 0x1510, 0x3860, 0x5287,
    0x480f, 0x4252, 0x1789, 0x5a2d, 0x2a49, 0x5e10, 0x437f, 0x4b4e, 0x2f45, 0x216e,
    0x5cb7, 0x7130, 0x2a3f, 0x60e4, 0x4dc9, 0x0ef0, 0x0f52, 0x1bb9, 0x6211, 0x7a56,
    0x226d, 0x4ea7, 0x6f36, 0x3692, 0x38bf, 0x0c62, 0x05eb, 0x4c55, 0x60f4, 0x728c,
    0x3b6f, 0x2037, 0x7f69, 0x0936, 0x651a, 0x4ceb, 0x6218, 0x79f3, 0x383f, 0x18d9,
    0x4f05, 0x5c82, 0x2912, 0x6f17, 0x6856, 0x5938, 0x1007, 0x61ab, 0x3e7f, 0x57c2,
    0x542f, 0x4f62, 0x7454, 0x2eac, 0x7739, 0x42d4, 0x2f90, 0x435a, 0x2e52, 0x2064,
    0x637c, 0x66ad, 0x2c90, 0x0bad, 0x759c, 0x0029, 0x0986, 0x7126, 0x1ca7, 0x1605,
    0x386a, 0x27f5, 0x1380, 0x6d75, 0x24c3, 0x0f8e, 0x2b7a, 0x1418, 0x1fd1, 0x7dc1,
    0x2d8e, 0x43af, 0x2267, 0x7da3, 0x4e3d, 0x1338, 0x50db, 0x454d, 0x764d, 0x40a3,
    0x42e6, 0x262b, 0x2d2e, 0x1aea, 0x2e17, 0x173d, 0x3a6e, 0x71bf, 0x25f9, 0x0a5d,
    0x7c57, 0x0fbe, 0x46ce, 0x4939, 0x6b17, 0x37bb, 0x3e91, 0x76db
};

static void dumphex32(char* name, char* base, int len)
{
	__u32 i;

	PRINT("dump %s registers:", name);
	for (i=0; i<len*4; i+=4) {
		if (!(i&0xf))
			PRINT("\n0x%p : ", base + i);
		PRINT("0x%x ", *((volatile unsigned int *)(base + i)));
	}
	PRINT("\n");
}


static void dumpreg()
{

	dumphex32("nand0 reg", (char*)0x01c03000, 60);

	dumphex32("gpio reg", (char*)0x01c20848, 50);

	dumphex32("dma reg", (char*)0x01c02300, 50);

	dumphex32("clk reg", (char*)0x01c00080, 10);

}




/**************************************************************************
************************* add one cmd to cmd list******************************
****************************************************************************/
void _add_cmd_list(NFC_CMD_LIST *cmd,__u32 value,__u32 addr_cycle,__u8 *addr,__u8 data_fetch_flag,
					__u8 main_data_fetch,__u32 bytecnt,__u8 wait_rb_flag)
{
	cmd->addr = addr;
	cmd->addr_cycle = addr_cycle;
	cmd->data_fetch_flag = data_fetch_flag;
	cmd->main_data_fetch = main_data_fetch;
	cmd->bytecnt = bytecnt;
	cmd->value = value;
	cmd->wait_rb_flag = wait_rb_flag;
	cmd->next = NULL;
}

/****************************************************************************
*********************translate (block + page+ sector) into 5 bytes addr***************
*****************************************************************************/
void _cal_addr_in_chip(__u32 block, __u32 page, __u32 sector,__u8 *addr, __u8 cycle)
{
	__u32 row;
	__u32 column;

	column = 512 * sector;
	row = block * PAGE_CNT_OF_PHY_BLK + page;

	switch(cycle){
		case 1:
			addr[0] = 0x00;
			break;
		case 2:
			addr[0] = column & 0xff;
			addr[1] = (column >> 8) & 0xff;
			break;
		case 3:
			addr[0] = row & 0xff;
			addr[1] = (row >> 8) & 0xff;
			addr[2] = (row >> 16) & 0xff;
			break;
		case 4:
			addr[0] = column && 0xff;
			addr[1] = (column >> 8) & 0xff;
			addr[2] = row & 0xff;
			addr[3] = (row >> 8) & 0xff;
			break;
		case 5:
			addr[0] = column & 0xff;
			addr[1] = (column >> 8) & 0xff;
			addr[2] = row & 0xff;
			addr[3] = (row >> 8) & 0xff;
			addr[4] = (row >> 16) & 0xff;
			break;
		default:
			break;
	}

}




__u8 _cal_real_chip(__u32 global_bank)
{
	__u8 chip = 0;

	if((RB_CONNECT_MODE == 0)&&(global_bank<=2))
	{
	    if(global_bank)
		chip = 7;
	    else
		chip = 0;

	    return chip;
	}
	if((RB_CONNECT_MODE == 1)&&(global_bank<=1))
      	{
      	    chip = global_bank;
	    return chip;
      	}
	if((RB_CONNECT_MODE == 2)&&(global_bank<=2))
      	{
      	    chip = global_bank;
	    return chip;
      	}
	if((RB_CONNECT_MODE == 3)&&(global_bank<=2))
      	{
      	    chip = global_bank*2;
	    return chip;
      	}
	if((RB_CONNECT_MODE == 4)&&(global_bank<=4))
      	{
      	    switch(global_bank){
		  case 0:
		  	chip = 0;
			break;
		  case 1:
		  	chip = 2;
			break;
		  case 2:
		  	chip = 1;
			break;
		  case 3:
		  	chip = 3;
			break;
		  default :
		  	chip =0;
	    }

	    return chip;
      	}
	if((RB_CONNECT_MODE == 5)&&(global_bank<=4))
      	{
      	    chip = global_bank*2;

	    return chip;
      	}
	if((RB_CONNECT_MODE == 8)&&(global_bank<=8))
      	{
      	    switch(global_bank){
		  case 0:
		  	chip = 0;
			break;
		  case 1:
		  	chip = 2;
			break;
		  case 2:
		  	chip = 1;
			break;
		  case 3:
		  	chip = 3;
			break;
		  case 4:
		  	chip = 4;
			break;
		  case 5:
		  	chip = 6;
			break;
		  case 6:
		  	chip = 5;
			break;
		  case 7:
		  	chip = 7;
			break;
		  default : chip =0;

	    }

	    return chip;
      	}



	PHY_ERR("wrong chip number ,rb_mode = %d, bank = %d, chip = %d, chip info = %x\n",RB_CONNECT_MODE, global_bank, chip, CHIP_CONNECT_INFO);

	return 0xff;
}

__u8 _cal_real_rb(__u32 chip)
{
	__u8 rb;


	rb = 0;

	if(RB_CONNECT_MODE == 0)
      	{
      	    rb = 0;
      	}
      if(RB_CONNECT_MODE == 1)
      	{
      	    rb = chip;
      	}
	if(RB_CONNECT_MODE == 2)
      	{
      	    rb = chip;
      	}
	if(RB_CONNECT_MODE == 3)
      	{
      	    rb = chip/2;
      	}
	if(RB_CONNECT_MODE == 4)
      	{
      	    rb = chip/2;
      	}
	if(RB_CONNECT_MODE == 5)
      	{
      	    rb = (chip/2)%2;
      	}
	if(RB_CONNECT_MODE == 8)
      	{
      	    rb = (chip/2)%2;
      	}

	if((rb!=0)&&(rb!=1))
	{
	    PHY_ERR("wrong Rb connect Mode, chip = %d ,RbConnectMode = %d \n",chip,RB_CONNECT_MODE);
	    return 0xff;
	}

	return rb;
}

/*******************************************************************
**********************get status**************************************
********************************************************************/
__s32 _read_status(__u32 cmd_value, __u32 nBank)
{
	/*get status*/
	__u8 addr[5];
	__u32 addr_cycle;
	NFC_CMD_LIST cmd_list;

	addr_cycle = 0;

	if(!(cmd_value == 0x70 || cmd_value == 0x71))
	{
        	/* not 0x70 or 0x71, need send some address cycle */
       	 if(cmd_value == 0x78)
	 		addr_cycle = 3;
       	 else
       		addr_cycle = 1;
      		 _cal_addr_in_chip(nBank*BLOCK_CNT_OF_DIE,0,0,addr,addr_cycle);
	}
	_add_cmd_list(&cmd_list, cmd_value, addr_cycle, addr, 1,NFC_IGNORE,1,NFC_IGNORE);
	return (NFC_GetStatus(&cmd_list));

}

/********************************************************************
***************************wait rb ready*******************************
*********************************************************************/
__s32 _wait_rb_ready(__u32 chip)
{
	__s32 timeout = 0xffff;
	__u32 rb;


      rb = _cal_real_rb(chip);


	/*wait rb ready*/
	while((timeout--) && (NFC_CheckRbReady(rb)));
	if (timeout < 0)
	{
		dumpreg();
		return -ERR_TIMEOUT;
	}
	return 0;
}

#ifdef __OS_SUPPORT_RB_INT__
__s32 _wait_rb_ready_int(__u32 chip)
{
	__u32 rb;

    rb = _cal_real_rb(chip);
    NFC_SelectRb(rb);

    if(NFC_CheckRbReady(rb))
    {
       NAND_WaitRbReady();
    }
    else
    {
    }

    while(NFC_CheckRbReady(rb))
    {
        PRINT("rb int error!\n");
    }


	return 0;
}
#else
__s32 _wait_rb_ready_int(__u32 chip)
{
	__s32 timeout = 0xffff;
	__u32 rb;


      rb = _cal_real_rb(chip);


	/*wait rb ready*/
	while((timeout--) && (NFC_CheckRbReady(rb)));
	if (timeout < 0)
	{
		dumpreg();
		return -ERR_TIMEOUT;
	}
	return 0;
}
#endif

void _pending_dma_irq_sem(void)
{
	return;
}

void _random_seed_init(void)
{

}

__u32 _cal_random_seed(__u32 page)
{
	__u32 randomseed;

	randomseed = random_seed[page%128];

	return randomseed;
}

__s32 _read_single_page(struct boot_physical_param *readop,__u8 dma_wait_mode)
{
	__s32 ret;
	__u32 k = 0;
	__u32 rb,n;
	__u32 random_seed;
	__u8 sparebuf[4*16];
	__u8 default_value[16];
	__u8 addr[5];
	NFC_CMD_LIST cmd_list[4];
	__u32 list_len,i;

	/*create cmd list*/
	/*samll block*/
	if (SECTOR_CNT_OF_SINGLE_PAGE == 1){
		_cal_addr_in_chip(readop->block,readop->page,0,addr,4);
		_add_cmd_list(cmd_list,0x00,4,addr,NFC_DATA_FETCH,NFC_IGNORE,NFC_IGNORE,NFC_WAIT_RB);
	}
	/*large block*/
	else{
		/*the cammand have no corresponding feature if IGNORE was set, */
		_cal_addr_in_chip(readop->block,readop->page,0,addr,5);
		_add_cmd_list(cmd_list,0x00,5,addr,NFC_NO_DATA_FETCH,NFC_IGNORE,NFC_IGNORE,NFC_NO_WAIT_RB);

	}
	_add_cmd_list(cmd_list + 1,0x05,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE);
	_add_cmd_list(cmd_list + 2,0xe0,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE);
	_add_cmd_list(cmd_list + 3,0x30,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE,NFC_IGNORE);
	list_len = 4;
	for(i = 0; i < list_len - 1; i++){
		cmd_list[i].next = &(cmd_list[i+1]);
	}

	/*wait rb ready*/
	ret = _wait_rb_ready(readop->chip);
	if (ret)
		return ret;

	rb = _cal_real_rb(readop->chip);
	NFC_SelectChip(readop->chip);
	NFC_SelectRb(rb);

    if(SUPPORT_READ_RETRY)
    {
        if((READ_RETRY_MODE>=0x10)&&(READ_RETRY_MODE<0x60)) 
        {
			RetryCount[readop->chip] = 0;
			if((READ_RETRY_MODE>=0x30)&&(READ_RETRY_MODE<0x40)) 
			{
				if(READ_RETRY_MODE==0x30)//for sandisk 19nm flash
				{
					if((readop->page!=255)&&((readop->page==0)||((readop->page)%2)))
					{
						READ_RETRY_TYPE = 0x301009;
						NFC_ReadRetryInit(READ_RETRY_TYPE);
					}
					else
					{
						READ_RETRY_TYPE = 0x301409;
						NFC_ReadRetryInit(READ_RETRY_TYPE);
					}
				}
			}
        }
        for( k = 0; k<READ_RETRY_CYCLE+1;k++)
		{
			if(RetryCount[readop->chip]==(READ_RETRY_CYCLE+1))
				RetryCount[readop->chip] = 0;

			if(k>0)
			{
			    if(NFC_ReadRetry(readop->chip,RetryCount[readop->chip],READ_RETRY_TYPE))
			    {
			        PHY_ERR("[Read_single_page] NFC_ReadRetry fail \n");
			        return -1;
			    }
			}

			if(SUPPORT_RANDOM)
			{
				random_seed = _cal_random_seed(readop->page);
				NFC_SetRandomSeed(random_seed);
				NFC_RandomEnable();
				ret = NFC_Read(cmd_list, readop->mainbuf, sparebuf, dma_wait_mode , NFC_PAGE_MODE);
				NFC_RandomDisable();
				if(ret == -ERR_ECC)
					ret = NFC_Read(cmd_list, readop->mainbuf, sparebuf, dma_wait_mode , NFC_PAGE_MODE);

				/**************************************************************************************
				* 1. add by Neil, from v2.09
				* 2. if spare area is all 0xff in random disable mode, it means the page is a clear page
				* 3. because in toshiba 24nm nand, too many clear pages are not all 0xff
				***************************************************************************************/
				if((ret == -ERR_ECC)&&(sparebuf[0]==0xff)&&(sparebuf[1]==0xff)&&(sparebuf[2]==0xff)&&(sparebuf[3]==0xff)&&(sparebuf[4]==0xff)&&(sparebuf[5]==0xff)&&(sparebuf[6]==0xff)&&(sparebuf[7]==0xff))
				{
					ret = 0;
				}

			}
			else
			{
				ret = NFC_Read(cmd_list, readop->mainbuf, sparebuf, dma_wait_mode , NFC_PAGE_MODE);
			}

			if((ret != -ERR_ECC)||(k==READ_RETRY_CYCLE))
			{
				if(k==0)
				{
					break;
				}
				else
				{
					if((READ_RETRY_MODE>=0x10)&&(READ_RETRY_MODE<0x20)) 
				    {
	    			    if(0x10 == READ_RETRY_MODE)
	    			    {
		    				PHY_ResetChip(readop->chip);
	    			    }
	    			    else if(0x11 == READ_RETRY_MODE)
	    			    {
	    			    	NFC_SetDefaultParam(readop->chip, default_value, READ_RETRY_TYPE);
	    			    }
				    }
					else if((READ_RETRY_MODE>=0x30)&&(READ_RETRY_MODE<0x40)) 
					{
						NFC_ReadRetry_off(readop->chip);

					}
				    else if((READ_RETRY_MODE>=0x20)&&(READ_RETRY_MODE<0x30))  
				    {
				        NFC_SetDefaultParam(readop->chip, default_value, READ_RETRY_TYPE);
				    }
					else if((READ_RETRY_MODE>=0x40)&&(READ_RETRY_MODE<0x50))  
				    {
				        NFC_SetDefaultParam(readop->chip, default_value, READ_RETRY_TYPE);
				    }
					else if((READ_RETRY_MODE>=0x50)&&(READ_RETRY_MODE<0x60))  
				    {
				        NFC_SetDefaultParam(readop->chip, default_value, READ_RETRY_TYPE);
				    }

					break;
				}
			}

			RetryCount[readop->chip]++;
		}

    	if(k>0)
    	{
    		PHY_DBG("[Read_single_page] NFC_ReadRetry %d cycles, chip = %d, block = %d, page = %d, RetryCount = %d  \n", k ,readop->chip,readop->block, readop->page, RetryCount[readop->chip]);
    		if(ret == -ERR_ECC)
    		{
				if((READ_RETRY_MODE==0x2)||(READ_RETRY_MODE==0x3))
				{
					NFC_SetDefaultParam(readop->chip, default_value, READ_RETRY_TYPE);
					RetryCount[readop->chip] = 0;
					PHY_DBG("ecc error!\n");
				}

				else if(0x32 == READ_RETRY_MODE)
				{
					if((255 == readop->page) || ((0 == (readop->page)%2)&&(readop->page != 0)))
					{
						RetryCount[readop->chip] = 1;
						for( n = 1; n<(READ_RETRY_CYCLE+1);n++)
						{
							if(RetryCount[readop->chip]==(READ_RETRY_CYCLE+1))
								RetryCount[readop->chip] = 1;

							NFC_ReadRetry_0x32_UpperPage();

							if(NFC_ReadRetry(readop->chip,RetryCount[readop->chip],READ_RETRY_TYPE))
							{
								PHY_ERR("[Read_single_page] NFC_ReadRetry fail \n");
								return -1;
							}


							if(SUPPORT_RANDOM)
							{
								random_seed = _cal_random_seed(readop->page);
								NFC_SetRandomSeed(random_seed);
								NFC_RandomEnable();
								ret = NFC_Read(cmd_list, readop->mainbuf, sparebuf, dma_wait_mode , NFC_PAGE_MODE);
								NFC_RandomDisable();
								if(ret == -ERR_ECC)
								{
									ret = NFC_Read(cmd_list, readop->mainbuf, sparebuf, dma_wait_mode , NFC_PAGE_MODE);
								}

							}
							else
							{
								ret = NFC_Read(cmd_list, readop->mainbuf, sparebuf, dma_wait_mode , NFC_PAGE_MODE);
							}
							if((ret != -ERR_ECC)||(n==READ_RETRY_CYCLE))
							{
								NFC_ReadRetry_off(readop->chip);
								break;
							}

							RetryCount[readop->chip]++;
						}
						PHY_DBG("[Read_single_page] ReadRetry again , count: %d\n", (__u32)RetryCount[readop->chip]);
						if(ret == -ERR_ECC)
						{
							PHY_DBG("ecc error!\n");
						}
					}
					else
						PHY_DBG("ecc error!\n");
				}
				else
					PHY_DBG("ecc error!\n");

			}
		}
    	if(ret == ECC_LIMIT)
    		ret = 0;


    }
    else
    {
		if(SUPPORT_RANDOM)
        {
			random_seed = _cal_random_seed(readop->page);
			NFC_SetRandomSeed(random_seed);
			NFC_RandomEnable();
			ret = NFC_Read(cmd_list, readop->mainbuf, sparebuf, dma_wait_mode , NFC_PAGE_MODE);
			NFC_RandomDisable();
			if(ret == -ERR_ECC)
				ret = NFC_Read(cmd_list, readop->mainbuf, sparebuf, dma_wait_mode , NFC_PAGE_MODE);
		}
		else
		{
			ret = NFC_Read(cmd_list, readop->mainbuf, sparebuf, dma_wait_mode , NFC_PAGE_MODE);
		}

    }

	if (dma_wait_mode)
		_pending_dma_irq_sem();

	if (readop->oobbuf){
		MEMCPY(readop->oobbuf,sparebuf, 2 * 4);
	}

	NFC_DeSelectChip(readop->chip);
	NFC_DeSelectRb(rb);

	return ret;
}



/*
************************************************************************************************************************
*                       INIT NAND FLASH DRIVER PHYSICAL MODULE
*
* Description: init nand flash driver physical module.
*
* Aguments   : none
*
* Returns    : the resutl of initial.
*                   = 0     initial successful;
*                   = -1    initial failed.
************************************************************************************************************************
*/
__s32 PHY_Init(void)
{
    __s32 ret;
    __u32 i;
	NFC_INIT_INFO nand_info;
    for(i=0; i<8; i++)
        RetryCount[i] = 0;
	nand_info.bus_width = 0x0;
	nand_info.ce_ctl = 0x0;
	nand_info.ce_ctl1 = 0x0;
	nand_info.debug = 0x0;
	nand_info.pagesize = 4;
	nand_info.rb_sel = 1;
	nand_info.serial_access_mode = 1;
	nand_info.ddr_type = 0;
	ret = NFC_Init(&nand_info);

    PHY_DBG("NFC Randomizer start. \n");
	_random_seed_init();
	NFC_RandomDisable();

	return ret;
}

__s32 PHY_GetDefaultParam(__u32 bank)
{
    return 0;
}

__s32 PHY_SetDefaultParam(__u32 bank)
{
    return 0;
}

__s32 PHY_ChangeMode(__u8 serial_mode)
{

	NFC_INIT_INFO nand_info;

	NAND_SetClk(NandStorageInfo.FrequencePar);

	/*memory allocate*/
	if (!PageCachePool.PageCache0){
		PageCachePool.PageCache0 = (__u8 *)MALLOC(SECTOR_CNT_OF_SUPER_PAGE * 512);
		if (!PageCachePool.PageCache0)
			return -1;
	}

	if (!PageCachePool.SpareCache){
		PageCachePool.SpareCache = (__u8 *)MALLOC(SECTOR_CNT_OF_SUPER_PAGE * 4);
		if (!PageCachePool.SpareCache)
			return -1;
	}

	if (!PageCachePool.TmpPageCache){
		PageCachePool.TmpPageCache = (__u8 *)MALLOC(SECTOR_CNT_OF_SUPER_PAGE * 512);
		if (!PageCachePool.TmpPageCache)
			return -1;
	}

    NFC_SetEccMode(ECC_MODE);

	nand_info.bus_width = 0x0;
	nand_info.ce_ctl = 0x0;
	nand_info.ce_ctl1 = 0x0;
	nand_info.debug = 0x0;
	nand_info.pagesize = SECTOR_CNT_OF_SINGLE_PAGE;
	nand_info.serial_access_mode = serial_mode;
	nand_info.ddr_type = DDR_TYPE;
	return (NFC_ChangMode(&nand_info));
}


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
__s32 PHY_Exit(void)
{
    __u32 i = 0;

	if (PageCachePool.PageCache0){
		FREE(PageCachePool.PageCache0,SECTOR_CNT_OF_SUPER_PAGE * 512);
		PageCachePool.PageCache0 = NULL;
	}
	if (PageCachePool.SpareCache){
		FREE(PageCachePool.SpareCache,SECTOR_CNT_OF_SUPER_PAGE * 4);
		PageCachePool.SpareCache = NULL;
	}
	if (PageCachePool.TmpPageCache){
		FREE(PageCachePool.TmpPageCache,SECTOR_CNT_OF_SUPER_PAGE * 512);
		PageCachePool.TmpPageCache = NULL;
	}

	if(SUPPORT_READ_RETRY)
	{
	    for(i=0; i<NandStorageInfo.ChipCnt;i++)
        {
            PHY_SetDefaultParam(i);
			dumy_read(i);
        }
        NFC_ReadRetryExit(READ_RETRY_TYPE);
	}


	NFC_RandomDisable();

	NFC_Exit();
	return 0;
}


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
__s32  PHY_ResetChip(__u32 nChip)
{
	__s32 ret;
	__s32 timeout = 0xffff;


	NFC_CMD_LIST cmd;

	NFC_SelectChip(nChip);

	_add_cmd_list(&cmd, 0xff, 0 , NFC_IGNORE, NFC_NO_DATA_FETCH, NFC_IGNORE, NFC_IGNORE, NFC_NO_WAIT_RB);
	ret = NFC_ResetChip(&cmd);

      	/*wait rb0 ready*/
	NFC_SelectRb(0);
	while((timeout--) && (NFC_CheckRbReady(0)));
	if (timeout < 0)
		return -ERR_TIMEOUT;

      /*wait rb0 ready*/
	NFC_SelectRb(1);
	while((timeout--) && (NFC_CheckRbReady(1)));
	if (timeout < 0)
		return -ERR_TIMEOUT;

	NFC_DeSelectChip(nChip);



	return ret;
}


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

__s32  PHY_ReadNandId(__s32 nChip, void *pChipID)
{
	__s32 ret;


	NFC_CMD_LIST cmd;
	__u8 addr = 0;

	NFC_SelectChip(nChip);

	_add_cmd_list(&cmd, 0x90,1 , &addr, NFC_DATA_FETCH, NFC_IGNORE, 6, NFC_NO_WAIT_RB);
	ret = NFC_GetId(&cmd, pChipID);

	NFC_DeSelectChip(nChip);


	return ret;
}

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
__s32  PHY_CheckWp(__u32 nChip)
{
	__s32 status;
	__u32 rb;


	rb = _cal_real_rb(nChip);
	NFC_SelectChip(nChip);
	NFC_SelectRb(rb);


	status = _read_status(0x70,nChip);
	NFC_DeSelectChip(nChip);
	NFC_DeSelectRb(rb);

	if (status < 0)
		return status;

	if (status & NAND_WRITE_PROTECT){
		return 1;
	}
	else
		return 0;
}

void _pending_rb_irq_sem(void)
{
	return;
}

void _do_irq(void)
{

}


__s32 PHY_SimpleRead (struct boot_physical_param *readop)
{
	return (_read_single_page(readop,0));
}



/*
************************************************************************************************************************
*                       SYNC NAND FLASH PHYSIC OPERATION
*
*Description: Sync nand flash operation, check nand flash program/erase operation status.
*
*Arguments  : nBank     the number of the bank which need be synchronized;
*             bMode     the type of synch,
*                       = 0     sync the chip which the bank belonged to, wait the whole chip
*                               to be ready, and report status. if the chip support cacheprogram,
*                               need check if the chip is true ready;
*                       = 1     only sync the the bank, wait the bank ready and report the status,
*                               if the chip support cache program, need not check if the cache is
*                               true ready.
*
*Return     : the result of synch;
*               = 0     synch nand flash successful, nand operation ok;
*               = -1    synch nand flash failed.
************************************************************************************************************************
*/
__s32 PHY_SynchBank(__u32 nBank, __u32 bMode)
{
	__s32 ret,status;
	__u32 chip;
	__u32 rb;
	__u32 cmd_value;
	__s32 timeout = 0xffff;

	ret = 0;
	/*get chip no*/
	chip = _cal_real_chip(nBank);
	rb = _cal_real_rb(chip);

	if (0xff == chip){
		PHY_ERR("PHY_SynchBank : beyond chip count\n");
		return -ERR_INVALIDPHYADDR;
	}

	if ( (bMode == 1) && SUPPORT_INT_INTERLEAVE){
		if (nBank%BNK_CNT_OF_CHIP == 0)
			cmd_value = NandStorageInfo.OptPhyOpPar.InterBnk0StatusCmd;
		else
			cmd_value = NandStorageInfo.OptPhyOpPar.InterBnk1StatusCmd;
	}
	else{
		if (SUPPORT_MULTI_PROGRAM)
			cmd_value = NandStorageInfo.OptPhyOpPar.MultiPlaneStatusCmd;
		else
			cmd_value = 0x70;
	}

	/*if support rb irq , last op is erase or write*/
	if (SUPPORT_RB_IRQ)
		_pending_rb_irq_sem();

	_wait_rb_ready_int(chip);

	NFC_SelectChip(chip);
	NFC_SelectRb(rb);

	while(1){
		status = _read_status(cmd_value,nBank%BNK_CNT_OF_CHIP);
		if (status < 0)
		{
		    PHY_ERR("PHY_SynchBank : read status invalid ,chip = %x, bank = %x\n",chip,nBank);
			PHY_ERR("PHY_SynchBank : read status invalid ,cmd value = %x, status = %x\n",cmd_value,status);
		    return status;
		}
		if (status & NAND_STATUS_READY)
			break;

		if (timeout-- < 0){
			PHY_ERR("PHY_SynchBank : wait nand ready timeout,chip = %x, bank = %x, cmd value = %x, status = %x\n",chip,nBank,cmd_value,status);
			return -ERR_TIMEOUT;
		}
	}
	if(status & NAND_OPERATE_FAIL)
	{
	    PHY_ERR("PHY_SynchBank : last W/E operation fail,chip = %x, bank = %x, cmd value = %x, status = %x\n",chip,nBank,cmd_value,status);
		PHY_ERR("PHY_SynchBank : last W/E operation fail ,cmd value = %x, status = %x\n",cmd_value,status);
	    ret = NAND_OP_FALSE;
	}

	NFC_DeSelectChip(chip);
	NFC_DeSelectRb(rb);


	return ret;
}

__u32 dumy_read(__u32 bank)
{
	__u32 i, j, k,chip;
	__u8* main_buf = (__u8 *)MALLOC(8192);
	__u8  oob_buf[32];
	struct boot_physical_param readop;
	__s32 ret;
	PHY_ERR("[NAND]dumy read,chip %x\n",bank);

    chip = _cal_real_chip(bank);
    readop.chip = chip;
    readop.block = 0;
    readop.page = 0;
    readop.sectorbitmap = 0x3;
    readop.mainbuf = main_buf;
    readop.oobbuf = oob_buf;
    ret = PHY_SimpleRead_1K(&readop);

    FREE(main_buf, 8192);
    return 0;


}

__s32 PHY_Readretry_reset(void)
{
	return 0;

}
