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
#include <config.h>
#include <common.h>
#include <malloc.h>

#include "imgdecode.h"
#include "imagefile_new.h"
#include "../sprite_card.h"

#define HEAD_ID				0		//头加密接口索引
#define TABLE_ID			1		//表加密接口索引
#define DATA_ID				2		//数据加密接口索引
#define IF_CNT				3		//加密接口个数	现在只有头加密，表加密，数据加密3种
#define	MAX_KEY_SIZE 		32		//密码长度

#pragma pack(push, 1)
typedef struct tag_IMAGE_HANDLE
{


	ImageHead_t  ImageHead;		//img头信息

	ImageItem_t *ItemTable;		//item信息表


}IMAGE_HANDLE;

#define INVALID_INDEX		0xFFFFFFFF


typedef struct tag_ITEM_HANDLE{
	uint	index;					//在ItemTable中的索引
	uint    reserved[3];
}ITEM_HANDLE;

#define ITEM_PHOENIX_TOOLS 	  "PXTOOLS "

uint img_file_start;			//固件的起始位置


HIMAGE 	Img_Open	(char * ImageFile)
{
	IMAGE_HANDLE * pImage = NULL;
	uint ItemTableSize;					//固件索引表的大小

	img_file_start = sprite_card_firmware_start();
	if(!img_file_start)
	{
		printf("sunxi sprite error: unable to get firmware start position\n");

		return NULL;
	}
	debug("img start = 0x%x\n", img_file_start);
	pImage = (IMAGE_HANDLE *)malloc(sizeof(IMAGE_HANDLE));
	if (NULL == pImage)
	{
		printf("sunxi sprite error: fail to malloc memory for img head\n");

		return NULL;
	}
	memset(pImage, 0, sizeof(IMAGE_HANDLE));
	if(!sunxi_flash_read(img_file_start, IMAGE_HEAD_SIZE/512, &pImage->ImageHead))
	{
		printf("sunxi sprite error: read iamge head fail\n");

		goto _img_open_fail_;
	}
	debug("read mmc ok\n");
	if (memcmp(pImage->ImageHead.magic, IMAGE_MAGIC, 8) != 0)
	{
		printf("sunxi sprite error: iamge magic is bad\n");

		goto _img_open_fail_;
	}
	ItemTableSize = pImage->ImageHead.itemcount * sizeof(ImageItem_t);
	pImage->ItemTable = (ImageItem_t*)malloc(ItemTableSize);
	if (NULL == pImage->ItemTable)
	{
		printf("sunxi sprite error: fail to malloc memory for item table\n");

		goto _img_open_fail_;
	}
	if(!sunxi_flash_read(img_file_start + (IMAGE_HEAD_SIZE/512), ItemTableSize/512, pImage->ItemTable))
	{
		printf("sunxi sprite error: read iamge item table fail\n");

		goto _img_open_fail_;
	}

	return pImage;

_img_open_fail_:
	if(pImage->ItemTable)
	{
		free(pImage->ItemTable);
	}
	if(pImage)
	{
		free(pImage);
	}

	return NULL;
}


long long Img_GetSize	(HIMAGE hImage)
{
	IMAGE_HANDLE* pImage = (IMAGE_HANDLE *)hImage;
	long long       size;

	if (NULL == hImage)
	{
		printf("sunxi sprite error : hImage is NULL\n");

		return 0;
	}

	size = pImage->ImageHead.lenHi;
	size <<= 32;
	size |= pImage->ImageHead.lenLo;

	return size;
}
HIMAGEITEM 	Img_OpenItem	(HIMAGE hImage, char * MainType, char * subType)
{
	IMAGE_HANDLE* pImage = (IMAGE_HANDLE *)hImage;
	ITEM_HANDLE * pItem  = NULL;
	uint          i;

	if (NULL == pImage || NULL == MainType || NULL == subType)
	{
		return NULL;
	}

	pItem = (ITEM_HANDLE *) malloc(sizeof(ITEM_HANDLE));
	if (NULL == pItem)
	{
		printf("sunxi sprite error : cannot malloc memory for item\n");

		return NULL;
	}
	pItem->index = INVALID_INDEX;

	for (i = 0; i < pImage->ImageHead.itemcount ; i++)
	{
		if(!memcmp(subType,  pImage->ItemTable[i].subType,  SUBTYPE_LEN))
		{
			pItem->index = i;

			return pItem;
		}
	}

	printf("sunxi sprite error : cannot find item %s %s\n", MainType, subType);

	free(pItem);
	pItem = NULL;

	return NULL;
}



long long Img_GetItemSize	(HIMAGE hImage, HIMAGEITEM hItem)
{
	IMAGE_HANDLE* pImage = (IMAGE_HANDLE *)hImage;
	ITEM_HANDLE * pItem  = (ITEM_HANDLE  *)hItem;
	long long       size;

	if (NULL == pItem)
	{
		printf("sunxi sprite error : item is NULL\n");

		return 0;
	}

	size = pImage->ItemTable[pItem->index].filelenHi;
	size <<= 32;
	size |= pImage->ItemTable[pItem->index].filelenLo;

	return size;
}

uint Img_GetItemStart	(HIMAGE hImage, HIMAGEITEM hItem)
{
	IMAGE_HANDLE* pImage = (IMAGE_HANDLE *)hImage;
	ITEM_HANDLE * pItem  = (ITEM_HANDLE  *)hItem;
	long long       start;
	long long		offset;

	if (NULL == pItem)
	{
		printf("sunxi sprite error : item is NULL\n");

		return 0;
	}
	offset = pImage->ItemTable[pItem->index].offsetHi;
	offset <<= 32;
	offset |= pImage->ItemTable[pItem->index].offsetLo;
	start = offset/512;

	return ((uint)start + img_file_start);
}
#if 0
uint Img_ReadItem(HIMAGE hImage, HIMAGEITEM hItem, void *buffer, uint buffer_size)
{
	IMAGE_HANDLE* pImage = (IMAGE_HANDLE *)hImage;
	ITEM_HANDLE * pItem  = (ITEM_HANDLE  *)hItem;
	long long     start;
	long long	  offset;
	uint	      file_size;
	void          *tmp;

	if (NULL == pItem)
	{
		printf("sunxi sprite error : item is NULL\n");

		return 0;
	}
	if(pImage->ItemTable[pItem->index].filelenHi)
	{
		printf("sunxi sprite error : the item too big\n");

		return 0;
	}
	file_size = pImage->ItemTable[pItem->index].filelenLo;
	debug("file size=%d, buffer size=%d\n", file_size, buffer_size);
	if(file_size > buffer_size)
	{
		printf("sunxi sprite error : buffer is smaller than data size\n");

		return 0;
	}
	if(file_size > 2 * 1024 * 1024)
	{
		printf("sunxi sprite error : this function cant be used to read data over 2M bytes\n");

		return 0;
	}
	file_size = (file_size + 1023) & (~(1024 - 1));
	offset = pImage->ItemTable[pItem->index].offsetHi;
	offset <<= 32;
	offset |= pImage->ItemTable[pItem->index].offsetLo;
	start = offset/512;

	debug("malloc size = %d\n", file_size);
	tmp = malloc(file_size);
	if(!tmp)
	{
		printf("sunxi sprite error : fail to get memory for temp data\n");

		return 0;
	}
	if(!sunxi_flash_read((uint)start + img_file_start, file_size/512, tmp))
	{
		printf("sunxi sprite error : read item data failed\n");
		free(tmp);

		return 0;
	}
	memcpy(buffer, tmp, buffer_size);
	free(tmp);

	return buffer_size;
}
#else
uint Img_ReadItem(HIMAGE hImage, HIMAGEITEM hItem, void *buffer, uint buffer_size)
{
	IMAGE_HANDLE* pImage = (IMAGE_HANDLE *)hImage;
	ITEM_HANDLE * pItem  = (ITEM_HANDLE  *)hItem;
	long long     start;
	long long	  offset;
	uint	      file_size;

	if (NULL == pItem)
	{
		printf("sunxi sprite error : item is NULL\n");

		return 0;
	}
	if(pImage->ItemTable[pItem->index].filelenHi)
	{
		printf("sunxi sprite error : the item too big\n");

		return 0;
	}
	file_size = pImage->ItemTable[pItem->index].filelenLo;
	file_size = (file_size + 1023) & (~(1024 - 1));
	debug("file size=%d, buffer size=%d\n", file_size, buffer_size);
	if(file_size > buffer_size)
	{
		printf("sunxi sprite error : buffer is smaller than data size\n");

		return 0;
	}
	offset = pImage->ItemTable[pItem->index].offsetHi;
	offset <<= 32;
	offset |= pImage->ItemTable[pItem->index].offsetLo;
	start = offset/512;

	if(!sunxi_flash_read((uint)start + img_file_start, file_size/512, buffer))
	{
		printf("sunxi sprite error : read item data failed\n");

		return 0;
	}

	return file_size;
}
#endif




int Img_CloseItem	(HIMAGE hImage, HIMAGEITEM hItem)
{
	ITEM_HANDLE * pItem = (ITEM_HANDLE *)hItem;
	if (NULL == pItem)
	{
		printf("sunxi sprite error : item is null when close it\n");

		return -1;
	}
	free(pItem);
	pItem = NULL;

	return 0;
}



void  Img_Close	(HIMAGE hImage)
{
	IMAGE_HANDLE * pImage = (IMAGE_HANDLE *)hImage;

	if (NULL == pImage)
	{
		printf("sunxi sprite error : imghead is null when close it\n");

		return ;
	}

	if (NULL != pImage->ItemTable)
	{
		free(pImage->ItemTable);
		pImage->ItemTable = NULL;
	}

	memset(pImage, 0, sizeof(IMAGE_HANDLE));
	free(pImage);
	pImage = NULL;

	return ;
}



