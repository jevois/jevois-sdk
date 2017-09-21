/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/


#include "odm_precomp.h"


const u2Byte dB_Invert_Table[8][12] = {
	{	1,		1,		1,		2,		2,		2,		2,		3,		3,		3,		4,		4},
	{	4,		5,		6,		6,		7,		8,		9,		10,		11,		13,		14,		16},
	{	18,		20,		22,		25,		28,		32,		35,		40,		45,		50,		56,		63},
	{	71,		79,		89,		100,	112,	126,	141,	158,	178,	200,	224,	251},
	{	282,	316,	355,	398,	447,	501,	562,	631,	708,	794,	891,	1000},
	{	1122,	1259,	1413,	1585,	1778,	1995,	2239,	2512,	2818,	3162,	3548,	3981},
	{	4467,	5012,	5623,	6310,	7079,	7943,	8913,	10000,	11220,	12589,	14125,	15849},
	{	17783,	19953,	22387,	25119,	28184,	31623,	35481,	39811,	44668,	50119,	56234,	65535}};


#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
static u4Byte edca_setting_UL[HT_IOT_PEER_MAX] = 
{ 0x5e4322, 		0xa44f, 		0x5e4322,		0x5ea32b,  		0x5ea422, 	0x5ea322,	0x3ea430,	0x5ea42b, 0x5ea44f,	0x5e4322,	0x5e4322};


static u4Byte edca_setting_DL[HT_IOT_PEER_MAX] = 
{ 0xa44f, 		0x5ea44f, 	0x5e4322, 		0x5ea42b, 		0xa44f, 		0xa630, 		0x5ea630,	0x5ea42b, 0xa44f,		0xa42b,		0xa42b};

static u4Byte edca_setting_DL_GMode[HT_IOT_PEER_MAX] = 
{ 0x4322, 		0xa44f, 		0x5e4322,		0xa42b, 			0x5e4322, 	0x4322, 		0xa42b,		0x5ea42b, 0xa44f,		0x5e4322,	0x5ea42b};


#elif (DM_ODM_SUPPORT_TYPE &ODM_ADSL)
enum qos_prio { BK, BE, VI, VO, VI_AG, VO_AG };

static const struct ParaRecord rtl_ap_EDCA[] =
{
     {0,     7,      4,      10,     0},           
     {0,     3,      4,      6,      0},            
     {0,     1,      3,      4,      188},        
     {0,     1,      2,      3,      102},        
     {0,     1,      3,      4,      94},         
     {0,     1,      2,      3,      47},         
};

static const struct ParaRecord rtl_sta_EDCA[] =
{
     {0,     7,      4,      10,     0},
     {0,     3,      4,      10,     0},
     {0,     2,      3,      4,      188},
     {0,     2,      2,      3,      102},
     {0,     2,      3,      4,      94},
     {0,     2,      2,      3,      47},
};
#endif


u4Byte	OFDMSwingTable[OFDM_TABLE_SIZE] = {
	0x7f8001fe,
	0x788001e2,
	0x71c001c7,
	0x6b8001ae,
	0x65400195,
	0x5fc0017f,
	0x5a400169,
	0x55400155,
	0x50800142,
	0x4c000130,
	0x47c0011f,
	0x43c0010f,
	0x40000100,
	0x3c8000f2,
	0x390000e4,
	0x35c000d7,
	0x32c000cb,
	0x300000c0,
	0x2d4000b5,
	0x2ac000ab,
	0x288000a2,
	0x26000098,
	0x24000090,
	0x22000088,
	0x20000080,
	0x1e400079,
	0x1c800072,
	0x1b00006c,
	0x19800066,
	0x18000060,
	0x16c0005b,
	0x15800056,
	0x14400051,
	0x1300004c,
	0x12000048,
	0x11000044,
	0x10000040,
};

u1Byte	CCKSwingTable_Ch1_Ch13[CCK_TABLE_SIZE][8] = {
	{0x36, 0x35, 0x2e, 0x25, 0x1c, 0x12, 0x09, 0x04},
	{0x33, 0x32, 0x2b, 0x23, 0x1a, 0x11, 0x08, 0x04},
	{0x30, 0x2f, 0x29, 0x21, 0x19, 0x10, 0x08, 0x03},
	{0x2d, 0x2d, 0x27, 0x1f, 0x18, 0x0f, 0x08, 0x03},
	{0x2b, 0x2a, 0x25, 0x1e, 0x16, 0x0e, 0x07, 0x03},
	{0x28, 0x28, 0x22, 0x1c, 0x15, 0x0d, 0x07, 0x03},
	{0x26, 0x25, 0x21, 0x1b, 0x14, 0x0d, 0x06, 0x03},
	{0x24, 0x23, 0x1f, 0x19, 0x13, 0x0c, 0x06, 0x03},
	{0x22, 0x21, 0x1d, 0x18, 0x11, 0x0b, 0x06, 0x02},
	{0x20, 0x20, 0x1b, 0x16, 0x11, 0x08, 0x05, 0x02},
	{0x1f, 0x1e, 0x1a, 0x15, 0x10, 0x0a, 0x05, 0x02},
	{0x1d, 0x1c, 0x18, 0x14, 0x0f, 0x0a, 0x05, 0x02},
	{0x1b, 0x1a, 0x17, 0x13, 0x0e, 0x09, 0x04, 0x02},
	{0x1a, 0x19, 0x16, 0x12, 0x0d, 0x09, 0x04, 0x02},
	{0x18, 0x17, 0x15, 0x11, 0x0c, 0x08, 0x04, 0x02},
	{0x17, 0x16, 0x13, 0x10, 0x0c, 0x08, 0x04, 0x02},
	{0x16, 0x15, 0x12, 0x0f, 0x0b, 0x07, 0x04, 0x01},
	{0x14, 0x14, 0x11, 0x0e, 0x0b, 0x07, 0x03, 0x02},
	{0x13, 0x13, 0x10, 0x0d, 0x0a, 0x06, 0x03, 0x01},
	{0x12, 0x12, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},
	{0x11, 0x11, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},
	{0x10, 0x10, 0x0e, 0x0b, 0x08, 0x05, 0x03, 0x01},
	{0x0f, 0x0f, 0x0d, 0x0b, 0x08, 0x05, 0x03, 0x01},
	{0x0e, 0x0e, 0x0c, 0x0a, 0x08, 0x05, 0x02, 0x01},
	{0x0d, 0x0d, 0x0c, 0x0a, 0x07, 0x05, 0x02, 0x01},
	{0x0d, 0x0c, 0x0b, 0x09, 0x07, 0x04, 0x02, 0x01},
	{0x0c, 0x0c, 0x0a, 0x09, 0x06, 0x04, 0x02, 0x01},
	{0x0b, 0x0b, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x01},
	{0x0b, 0x0a, 0x09, 0x08, 0x06, 0x04, 0x02, 0x01},
	{0x0a, 0x0a, 0x09, 0x07, 0x05, 0x03, 0x02, 0x01},
	{0x0a, 0x09, 0x08, 0x07, 0x05, 0x03, 0x02, 0x01},
	{0x09, 0x09, 0x08, 0x06, 0x05, 0x03, 0x01, 0x01},
	{0x09, 0x08, 0x07, 0x06, 0x04, 0x03, 0x01, 0x01}
};


u1Byte	CCKSwingTable_Ch14[CCK_TABLE_SIZE][8] = {
	{0x36, 0x35, 0x2e, 0x1b, 0x00, 0x00, 0x00, 0x00},
	{0x33, 0x32, 0x2b, 0x19, 0x00, 0x00, 0x00, 0x00},
	{0x30, 0x2f, 0x29, 0x18, 0x00, 0x00, 0x00, 0x00},
	{0x2d, 0x2d, 0x17, 0x17, 0x00, 0x00, 0x00, 0x00},
	{0x2b, 0x2a, 0x25, 0x15, 0x00, 0x00, 0x00, 0x00},
	{0x28, 0x28, 0x24, 0x14, 0x00, 0x00, 0x00, 0x00},
	{0x26, 0x25, 0x21, 0x13, 0x00, 0x00, 0x00, 0x00},
	{0x24, 0x23, 0x1f, 0x12, 0x00, 0x00, 0x00, 0x00},
	{0x22, 0x21, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x00},
	{0x20, 0x20, 0x1b, 0x10, 0x00, 0x00, 0x00, 0x00},
	{0x1f, 0x1e, 0x1a, 0x0f, 0x00, 0x00, 0x00, 0x00},
	{0x1d, 0x1c, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00},
	{0x1b, 0x1a, 0x17, 0x0e, 0x00, 0x00, 0x00, 0x00},
	{0x1a, 0x19, 0x16, 0x0d, 0x00, 0x00, 0x00, 0x00},
	{0x18, 0x17, 0x15, 0x0c, 0x00, 0x00, 0x00, 0x00},
	{0x17, 0x16, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00},
	{0x16, 0x15, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00},
	{0x14, 0x14, 0x11, 0x0a, 0x00, 0x00, 0x00, 0x00},
	{0x13, 0x13, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00},
	{0x12, 0x12, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},
	{0x11, 0x11, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},
	{0x10, 0x10, 0x0e, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x0f, 0x0f, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x0e, 0x0e, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},
	{0x0d, 0x0d, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},
	{0x0d, 0x0c, 0x0b, 0x06, 0x00, 0x00, 0x00, 0x00},
	{0x0c, 0x0c, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},
	{0x0b, 0x0b, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},
	{0x0b, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x0a, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x0a, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x09, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x09, 0x08, 0x07, 0x04, 0x00, 0x00, 0x00, 0x00}
};


u4Byte OFDMSwingTable_New[OFDM_TABLE_SIZE] = {
	0x0b40002d,
	0x0c000030,
	0x0cc00033,
	0x0d800036,
	0x0e400039,
	0x0f00003c,
	0x10000040,
	0x11000044,
	0x12000048,
	0x1300004c,
	0x14400051,
	0x15800056,
	0x16c0005b,
	0x18000060,
	0x19800066,
	0x1b00006c,
	0x1c800072,
	0x1e400079,
	0x20000080,
	0x22000088,
	0x24000090,
	0x26000098,
	0x288000a2,
	0x2ac000ab,
	0x2d4000b5,
	0x300000c0,
	0x32c000cb,
	0x35c000d7,
	0x390000e4,
	0x3c8000f2,
	0x40000100,
	0x43c0010f,
	0x47c0011f,
	0x4c000130,
	0x50800142,
	0x55400155,
	0x5a400169,
	0x5fc0017f,
	0x65400195,
	0x6b8001ae,
	0x71c001c7,
	0x788001e2,
	0x7f8001fe 
};               


u1Byte CCKSwingTable_Ch1_Ch13_New[CCK_TABLE_SIZE][8] = {
	{0x09, 0x08, 0x07, 0x06, 0x04, 0x03, 0x01, 0x01},
	{0x09, 0x09, 0x08, 0x06, 0x05, 0x03, 0x01, 0x01},
	{0x0a, 0x09, 0x08, 0x07, 0x05, 0x03, 0x02, 0x01},
	{0x0a, 0x0a, 0x09, 0x07, 0x05, 0x03, 0x02, 0x01},
	{0x0b, 0x0a, 0x09, 0x08, 0x06, 0x04, 0x02, 0x01},
	{0x0b, 0x0b, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x01},
	{0x0c, 0x0c, 0x0a, 0x09, 0x06, 0x04, 0x02, 0x01},
	{0x0d, 0x0c, 0x0b, 0x09, 0x07, 0x04, 0x02, 0x01},
	{0x0d, 0x0d, 0x0c, 0x0a, 0x07, 0x05, 0x02, 0x01},
	{0x0e, 0x0e, 0x0c, 0x0a, 0x08, 0x05, 0x02, 0x01},
	{0x0f, 0x0f, 0x0d, 0x0b, 0x08, 0x05, 0x03, 0x01},
	{0x10, 0x10, 0x0e, 0x0b, 0x08, 0x05, 0x03, 0x01},
	{0x11, 0x11, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},
	{0x12, 0x12, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},
	{0x13, 0x13, 0x10, 0x0d, 0x0a, 0x06, 0x03, 0x01},
	{0x14, 0x14, 0x11, 0x0e, 0x0b, 0x07, 0x03, 0x02},
	{0x16, 0x15, 0x12, 0x0f, 0x0b, 0x07, 0x04, 0x01},
	{0x17, 0x16, 0x13, 0x10, 0x0c, 0x08, 0x04, 0x02},
	{0x18, 0x17, 0x15, 0x11, 0x0c, 0x08, 0x04, 0x02},
	{0x1a, 0x19, 0x16, 0x12, 0x0d, 0x09, 0x04, 0x02},
    {0x1b, 0x1a, 0x17, 0x13, 0x0e, 0x09, 0x04, 0x02},
	{0x1d, 0x1c, 0x18, 0x14, 0x0f, 0x0a, 0x05, 0x02},
	{0x1f, 0x1e, 0x1a, 0x15, 0x10, 0x0a, 0x05, 0x02},
	{0x20, 0x20, 0x1b, 0x16, 0x11, 0x08, 0x05, 0x02},
	{0x22, 0x21, 0x1d, 0x18, 0x11, 0x0b, 0x06, 0x02},
	{0x24, 0x23, 0x1f, 0x19, 0x13, 0x0c, 0x06, 0x03},
	{0x26, 0x25, 0x21, 0x1b, 0x14, 0x0d, 0x06, 0x03},
	{0x28, 0x28, 0x22, 0x1c, 0x15, 0x0d, 0x07, 0x03},
	{0x2b, 0x2a, 0x25, 0x1e, 0x16, 0x0e, 0x07, 0x03},
	{0x2d, 0x2d, 0x27, 0x1f, 0x18, 0x0f, 0x08, 0x03},
	{0x30, 0x2f, 0x29, 0x21, 0x19, 0x10, 0x08, 0x03},
	{0x33, 0x32, 0x2b, 0x23, 0x1a, 0x11, 0x08, 0x04},
	{0x36, 0x35, 0x2e, 0x25, 0x1c, 0x12, 0x09, 0x04} 
};                                                                  


u1Byte CCKSwingTable_Ch14_New[CCK_TABLE_SIZE][8]= {
	{0x09, 0x08, 0x07, 0x04, 0x00, 0x00, 0x00, 0x00},
	{0x09, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x0a, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x0a, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x0b, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x0b, 0x0b, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},
	{0x0c, 0x0c, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},
	{0x0d, 0x0c, 0x0b, 0x06, 0x00, 0x00, 0x00, 0x00},
	{0x0d, 0x0d, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},
	{0x0e, 0x0e, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},
	{0x0f, 0x0f, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x10, 0x10, 0x0e, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x11, 0x11, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},
	{0x12, 0x12, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},
	{0x13, 0x13, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00},
	{0x14, 0x14, 0x11, 0x0a, 0x00, 0x00, 0x00, 0x00},
	{0x16, 0x15, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00},
	{0x17, 0x16, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00},
	{0x18, 0x17, 0x15, 0x0c, 0x00, 0x00, 0x00, 0x00},
	{0x1a, 0x19, 0x16, 0x0d, 0x00, 0x00, 0x00, 0x00},
	{0x1b, 0x1a, 0x17, 0x0e, 0x00, 0x00, 0x00, 0x00},
	{0x1d, 0x1c, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00},
	{0x1f, 0x1e, 0x1a, 0x0f, 0x00, 0x00, 0x00, 0x00},
	{0x20, 0x20, 0x1b, 0x10, 0x00, 0x00, 0x00, 0x00},
	{0x22, 0x21, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x00},
	{0x24, 0x23, 0x1f, 0x12, 0x00, 0x00, 0x00, 0x00},
	{0x26, 0x25, 0x21, 0x13, 0x00, 0x00, 0x00, 0x00},
	{0x28, 0x28, 0x24, 0x14, 0x00, 0x00, 0x00, 0x00},
	{0x2b, 0x2a, 0x25, 0x15, 0x00, 0x00, 0x00, 0x00},
	{0x2d, 0x2d, 0x17, 0x17, 0x00, 0x00, 0x00, 0x00},
	{0x30, 0x2f, 0x29, 0x18, 0x00, 0x00, 0x00, 0x00},
	{0x33, 0x32, 0x2b, 0x19, 0x00, 0x00, 0x00, 0x00},
	{0x36, 0x35, 0x2e, 0x1b, 0x00, 0x00, 0x00, 0x00} 
};

u4Byte TxScalingTable_Jaguar[TXSCALE_TABLE_SIZE] =
{
	0x081,
	0x088,
	0x090,
	0x099,
	0x0A2,
	0x0AC,
	0x0B6,
	0x0C0,
	0x0CC,
	0x0D8,
	0x0E5,
	0x0F2,
	0x101,
	0x110,
	0x120,
	0x131,
	0x143,
	0x156,
	0x16A,
	0x180,
	0x197,
	0x1AF,
	0x1C8,
	0x1E3,
	0x200,
	0x21E,
	0x23E,
	0x261,
	0x285,
	0x2AB,
	0x2D3,
	0x2FE,
	0x32B,
	0x35C,
	0x38E,
	0x3C4,
	0x3FE 
};

#ifdef AP_BUILD_WORKAROUND

unsigned int TxPwrTrk_OFDM_SwingTbl[TxPwrTrk_OFDM_SwingTbl_Len] = {
	/*  +6.0dB */ 0x7f8001fe,
	/*  +5.5dB */ 0x788001e2,
	/*  +5.0dB */ 0x71c001c7,
	/*  +4.5dB */ 0x6b8001ae,
	/*  +4.0dB */ 0x65400195,
	/*  +3.5dB */ 0x5fc0017f,
	/*  +3.0dB */ 0x5a400169,
	/*  +2.5dB */ 0x55400155,
	/*  +2.0dB */ 0x50800142,
	/*  +1.5dB */ 0x4c000130,
	/*  +1.0dB */ 0x47c0011f,
	/*  +0.5dB */ 0x43c0010f,
	/*   0.0dB */ 0x40000100,
	/*  -0.5dB */ 0x3c8000f2,
	/*  -1.0dB */ 0x390000e4,
	/*  -1.5dB */ 0x35c000d7,
	/*  -2.0dB */ 0x32c000cb,
	/*  -2.5dB */ 0x300000c0,
	/*  -3.0dB */ 0x2d4000b5,
	/*  -3.5dB */ 0x2ac000ab,
	/*  -4.0dB */ 0x288000a2,
	/*  -4.5dB */ 0x26000098,
	/*  -5.0dB */ 0x24000090,
	/*  -5.5dB */ 0x22000088,
	/*  -6.0dB */ 0x20000080,
	/*  -6.5dB */ 0x1a00006c,
	/*  -7.0dB */ 0x1c800072,
	/*  -7.5dB */ 0x18000060,
	/*  -8.0dB */ 0x19800066,
	/*  -8.5dB */ 0x15800056,
	/*  -9.0dB */ 0x26c0005b,
	/*  -9.5dB */ 0x14400051,
	/* -10.0dB */ 0x24400051,
	/* -10.5dB */ 0x1300004c,
	/* -11.0dB */ 0x12000048,
	/* -11.5dB */ 0x11000044,
	/* -12.0dB */ 0x10000040
};
#endif


VOID
odm_CommonInfoSelfInit(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_CommonInfoSelfUpdate(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_CmnInfoInit_Debug(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_CmnInfoHook_Debug(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_CmnInfoUpdate_Debug(
	IN		PDM_ODM_T		pDM_Odm
	);
VOID
odm_BasicDbgMessage
(
	IN		PDM_ODM_T		pDM_Odm
	);


VOID 
odm_FalseAlarmCounterStatistics(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_DIGInit(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID	
odm_DIG(
	IN		PDM_ODM_T		pDM_Odm
	);

BOOLEAN 
odm_DigAbort(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID 
odm_CCKPacketDetectionThresh(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_AdaptivityInit(
	IN		PDM_ODM_T		pDM_Odm
);

VOID
odm_Adaptivity(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			IGI
);

VOID 
odm_DynamicBBPowerSavingInit(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID 
odm_DynamicBBPowerSaving(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_1R_CCA(
	IN		PDM_ODM_T		pDM_Odm
	);

#if(DM_ODM_SUPPORT_TYPE & (ODM_WIN)) 
VOID	odm_PathDiversityInit_92C(	IN	PADAPTER	Adapter);
VOID	odm_2TPathDiversityInit_92C(	IN	PADAPTER	Adapter);
VOID	odm_1TPathDiversityInit_92C(	IN	PADAPTER	Adapter);
BOOLEAN	odm_IsConnected_92C(IN	PADAPTER	Adapter);
VOID	odm_PathDiversityAfterLink_92C(	IN	PADAPTER	Adapter);

VOID
odm_CCKTXPathDiversityCallback(
	PRT_TIMER		pTimer
	);

VOID
odm_CCKTXPathDiversityWorkItemCallback(
    IN PVOID            pContext
    );

VOID
odm_PathDivChkAntSwitchCallback(
	PRT_TIMER		pTimer
	);

VOID
odm_PathDivChkAntSwitchWorkitemCallback(
    IN PVOID            pContext
    );

VOID	odm_SetRespPath_92C(		IN	PADAPTER	Adapter, 	IN	u1Byte	DefaultRespPath);
VOID	odm_OFDMTXPathDiversity_92C(	IN	PADAPTER	Adapter);
VOID	odm_CCKTXPathDiversity_92C(	IN	PADAPTER	Adapter);
VOID	odm_ResetPathDiversity_92C(		IN	PADAPTER	Adapter);

VOID	odm_RXHPInit(	IN		PDM_ODM_T		pDM_Odm);
VOID	odm_RXHP(	IN		PDM_ODM_T		pDM_Odm);
VOID	odm_Write_RXHP(	IN	PDM_ODM_T	pDM_Odm);

VOID	odm_PSD_RXHP(		IN	PDM_ODM_T	pDM_Odm);
VOID	odm_PSD_RXHPCallback(	PRT_TIMER		pTimer);
VOID	odm_PSD_RXHPWorkitemCallback(	IN PVOID            pContext);

VOID	odm_PathDivInit_92D(	IN	PDM_ODM_T 	pDM_Odm);

VOID
odm_SetRespPath_92C(
	IN	PADAPTER	Adapter,
	IN	u1Byte	DefaultRespPath 
	);

#endif

VOID
odm_RefreshRateAdaptiveMaskMP(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_RefreshRateAdaptiveMaskCE(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_RefreshRateAdaptiveMaskAPADSL(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
ODM_DynamicATCSwitch_init(
	IN 		PDM_ODM_T 		pDM_Odm
	);

VOID
ODM_DynamicATCSwitch(
	IN 		PDM_ODM_T 		pDM_Odm
	);

VOID
odm_Write_CrystalCap(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			CrystalCap
);

VOID 
odm_DynamicTxPowerInit(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_DynamicTxPowerRestorePowerIndex(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID 
odm_DynamicTxPowerNIC(
	IN	PDM_ODM_T	pDM_Odm
	);

#if(DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
VOID
odm_DynamicTxPowerSavePowerIndex(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_DynamicTxPowerWritePowerIndex(
	IN	PDM_ODM_T	pDM_Odm, 
	IN 	u1Byte		Value);

VOID 
odm_DynamicTxPower_92C(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID 
odm_DynamicTxPower_92D(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID
odm_MPT_DIGCallback(
	PRT_TIMER		pTimer
);

VOID
odm_MPT_DIGWorkItemCallback(
    IN PVOID            pContext
    );
#endif


VOID
odm_RSSIMonitorInit(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID
odm_RSSIMonitorCheckMP(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID 
odm_RSSIMonitorCheckCE(
	IN		PDM_ODM_T		pDM_Odm
	);
VOID 
odm_RSSIMonitorCheckAP(
	IN		PDM_ODM_T		pDM_Odm
	);



VOID
odm_RSSIMonitorCheck(
	IN		PDM_ODM_T		pDM_Odm
	);
VOID 
odm_DynamicTxPower(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID 
odm_DynamicTxPowerAP(
	IN		PDM_ODM_T		pDM_Odm
	);


VOID
odm_SwAntDivInit(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_SwAntDivInit_NIC(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_SwAntDetectInit(
	IN 		PDM_ODM_T 		pDM_Odm
	);

VOID
odm_SwAntDivChkAntSwitch(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			Step
	);

VOID
odm_SwAntDivChkAntSwitchNIC(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte		Step
	);


#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID
odm_SwAntDivChkAntSwitchCallback(
	PRT_TIMER		pTimer
);
VOID
odm_SwAntDivChkAntSwitchWorkitemCallback(
    IN PVOID            pContext
    );
VOID
ODM_UpdateInitRateWorkItemCallback(
    IN PVOID            pContext
    );
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext);
#elif (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext);
#endif



VOID
odm_GlobalAdapterCheck(
	IN		VOID
	);

VOID
odm_RefreshBasicRateMask(
	IN		PDM_ODM_T		pDM_Odm	
	);

VOID
odm_RefreshRateAdaptiveMask(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
ODM_TXPowerTrackingCheck(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_TXPowerTrackingCheckAP(
	IN		PDM_ODM_T		pDM_Odm
	);







VOID
odm_RateAdaptiveMaskInit(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID
odm_TXPowerTrackingThermalMeterInit(
	IN	PDM_ODM_T	pDM_Odm
	);


VOID
odm_IQCalibrate(
		IN	PDM_ODM_T	pDM_Odm 
		);

VOID
odm_TXPowerTrackingInit(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID
odm_TXPowerTrackingCheckMP(
	IN	PDM_ODM_T	pDM_Odm
	);


VOID
odm_TXPowerTrackingCheckCE(
	IN	PDM_ODM_T	pDM_Odm
	);

#if(DM_ODM_SUPPORT_TYPE & (ODM_WIN)) 

VOID
ODM_RateAdaptiveStateApInit(
	IN	PADAPTER	Adapter	,
	IN	PRT_WLAN_STA  pEntry
	);

VOID 
odm_TXPowerTrackingCallbackThermalMeter92C(
            IN PADAPTER	Adapter
            );

VOID
odm_TXPowerTrackingCallbackRXGainThermalMeter92D(
	IN PADAPTER 	Adapter
	);

VOID
odm_TXPowerTrackingCallbackThermalMeter92D(
            IN PADAPTER	Adapter
            );

VOID
odm_TXPowerTrackingDirectCall92C(
            IN	PADAPTER		Adapter
            );

VOID
odm_TXPowerTrackingThermalMeterCheck(
	IN	PADAPTER		Adapter
	);

#endif

VOID
odm_EdcaTurboCheck(
	IN		PDM_ODM_T		pDM_Odm
	);
VOID
ODM_EdcaTurboInit(
	IN	PDM_ODM_T		pDM_Odm
);

#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
VOID
odm_EdcaTurboCheckMP(
	IN		PDM_ODM_T		pDM_Odm
	);

BOOLEAN
odm_IsEdcaTurboDisable(
	IN 	PDM_ODM_T 	pDM_Odm
);
VOID 
ODM_EdcaParaSelByIot(
	IN 	PDM_ODM_T 	pDM_Odm,
	OUT	u4Byte		*EDCA_BE_UL,
	OUT u4Byte		*EDCA_BE_DL
	);
VOID
odm_EdcaChooseTrafficIdx( 
	IN	PDM_ODM_T		pDM_Odm,
	IN	u8Byte  			cur_tx_bytes,  
	IN	u8Byte  			cur_rx_bytes, 
	IN	BOOLEAN 		bBiasOnRx,
	OUT BOOLEAN 		*pbIsCurRDLState
	);

#elif (DM_ODM_SUPPORT_TYPE==ODM_CE)
VOID
odm_EdcaTurboCheckCE(
	IN		PDM_ODM_T		pDM_Odm
	);
#else
VOID 
odm_IotEngine(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID
odm_EdcaParaInit(
	IN	PDM_ODM_T	pDM_Odm
	);
#endif



#define 	RxDefaultAnt1		0x65a9
#define	RxDefaultAnt2		0x569a

VOID
odm_InitHybridAntDiv(
	IN PDM_ODM_T	pDM_Odm 
	);

BOOLEAN
odm_StaDefAntSel(
	IN PDM_ODM_T	pDM_Odm,
	IN u4Byte		OFDM_Ant1_Cnt,
	IN u4Byte		OFDM_Ant2_Cnt,
	IN u4Byte		CCK_Ant1_Cnt,
	IN u4Byte		CCK_Ant2_Cnt,
	OUT u1Byte		*pDefAnt 
	);

VOID
odm_SetRxIdleAnt(
	IN	PDM_ODM_T	pDM_Odm,
	IN	u1Byte	Ant,
	IN   BOOLEAN   bDualPath                     
);



VOID
odm_HwAntDiv(
	IN	PDM_ODM_T	pDM_Odm
);

VOID	odm_PathDiversityInit(IN	PDM_ODM_T	pDM_Odm);
VOID	odm_PathDiversity(	IN	PDM_ODM_T	pDM_Odm);




VOID
ODM_DMInit(
	IN		PDM_ODM_T		pDM_Odm
	)
{

	odm_CommonInfoSelfInit(pDM_Odm);
	odm_CmnInfoInit_Debug(pDM_Odm);
	odm_DIGInit(pDM_Odm);	
	odm_AdaptivityInit(pDM_Odm);
	odm_RateAdaptiveMaskInit(pDM_Odm);
	odm_RSSIMonitorInit(pDM_Odm);
	
#if (RTL8192E_SUPPORT == 1)
	if(pDM_Odm->SupportICType==ODM_RTL8192E)
	{
		odm_PrimaryCCA_Check_Init(pDM_Odm);
	}
#endif

	if ( *(pDM_Odm->mp_mode) != 1)
	    odm_PathDiversityInit(pDM_Odm);
	ODM_EdcaTurboInit(pDM_Odm);

	if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
	{
		odm_TXPowerTrackingInit(pDM_Odm);
		if ( *(pDM_Odm->mp_mode) != 1)
			ODM_AntDivInit(pDM_Odm);
	}
	else if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
	{
		odm_DynamicBBPowerSavingInit(pDM_Odm);
		odm_DynamicTxPowerInit(pDM_Odm);
		odm_TXPowerTrackingInit(pDM_Odm);
		if ( *(pDM_Odm->mp_mode) != 1) {
		if(pDM_Odm->SupportICType==ODM_RTL8723A)
			odm_SwAntDivInit(pDM_Odm);	
		else if(pDM_Odm->SupportICType & (ODM_RTL8192C|ODM_RTL8192D))
		{
			if(pDM_Odm->AntDivType == HW_ANTDIV)
			odm_InitHybridAntDiv(pDM_Odm);
			else
			odm_SwAntDivInit(pDM_Odm);
		}
		else
			ODM_AntDivInit(pDM_Odm);
	
		if(pDM_Odm->SupportICType == ODM_RTL8723B)
			odm_SwAntDetectInit(pDM_Odm);
		}

#if (RTL8188E_SUPPORT == 1)
		if(pDM_Odm->SupportICType==ODM_RTL8188E)
		{
			odm_PrimaryCCA_Init(pDM_Odm);   
			ODM_RAInfo_Init_all(pDM_Odm);
		}	
#endif		

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		if(pDM_Odm->SupportICType&ODM_RTL8723A)
			odm_PSDMonitorInit(pDM_Odm);
		
		if(!(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8188E)))
		{
			odm_RXHPInit(pDM_Odm);	
		}
		if(pDM_Odm->SupportICType==ODM_RTL8192D)
		{
			odm_PathDivInit_92D(pDM_Odm);
		}	
#endif
	}

	ODM_DynamicATCSwitch_init(pDM_Odm);
	ODM_ClearTxPowerTrackingState(pDM_Odm);

}

VOID
ODM_DMWatchdog(
	IN		PDM_ODM_T		pDM_Odm
	)
{	
	if((pDM_Odm->SupportICType == ODM_RTL8821) && (pDM_Odm->SupportInterface == ODM_ITRF_USB))
	{
		if(pDM_Odm->RSSI_Min > 25)
			ODM_Write1Byte(pDM_Odm, 0x4CF, 0x02);
		else if(pDM_Odm->RSSI_Min < 20)
			ODM_Write1Byte(pDM_Odm, 0x4CF, 0x00);
	}


	odm_CommonInfoSelfUpdate(pDM_Odm);
	odm_BasicDbgMessage(pDM_Odm);
	odm_FalseAlarmCounterStatistics(pDM_Odm);
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): RSSI=0x%x\n",pDM_Odm->RSSI_Min));

	odm_RSSIMonitorCheck(pDM_Odm);

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	if( 	(adapter_to_pwrctl(pDM_Odm->Adapter)->pwr_mode != PS_MODE_ACTIVE)
	)
	{
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("----Step1: odm_DIG is in LPS mode\n"));				
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("---Step2: 8723AS is in LPS mode\n"));
			odm_DIGbyRSSI_LPS(pDM_Odm);
	}		
	else				
#endif
	{
		odm_DIG(pDM_Odm);
	}

	{
		pDIG_T	pDM_DigTable = &pDM_Odm->DM_DigTable;
		odm_Adaptivity(pDM_Odm, pDM_DigTable->CurIGValue);
	}
	odm_CCKPacketDetectionThresh(pDM_Odm);

	if(*(pDM_Odm->pbPowerSaving)==TRUE)
		return;

	
	odm_RefreshRateAdaptiveMask(pDM_Odm);
	odm_RefreshBasicRateMask(pDM_Odm);
	odm_DynamicBBPowerSaving(pDM_Odm);	
	odm_EdcaTurboCheck(pDM_Odm);
	odm_PathDiversity(pDM_Odm);
	ODM_DynamicATCSwitch(pDM_Odm);
	odm_DynamicTxPower(pDM_Odm);	

#if (RTL8192E_SUPPORT == 1)
        if(pDM_Odm->SupportICType==ODM_RTL8192E)
                odm_DynamicPrimaryCCA_Check(pDM_Odm); 
#endif

	
if ( *(pDM_Odm->mp_mode) != 1) {
	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
		odm_SwAntDivChkAntSwitch(pDM_Odm, SWAW_STEP_PEAK);
	}
	else if(pDM_Odm->SupportICType & (ODM_RTL8192C|ODM_RTL8192D))
	{
		if(pDM_Odm->AntDivType == HW_ANTDIV)
			odm_HwAntDiv(pDM_Odm);
		else
			odm_SwAntDivChkAntSwitch(pDM_Odm, SWAW_STEP_PEAK);
	}
	else
		ODM_AntDiv(pDM_Odm);
}

	if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
	{
		ODM_TXPowerTrackingCheck(pDM_Odm);

		odm_IQCalibrate(pDM_Odm);
	}
	else if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
	{
		ODM_TXPowerTrackingCheck(pDM_Odm);


		#if( DM_ODM_SUPPORT_TYPE & (ODM_WIN))	
		if(!(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8188E)))
		        odm_RXHP(pDM_Odm);	
		#endif

#if (RTL8192D_SUPPORT == 1)
	        if(pDM_Odm->SupportICType==ODM_RTL8192D)
	                ODM_DynamicEarlyMode(pDM_Odm);
#endif
	        odm_DynamicBBPowerSaving(pDM_Odm);
#if (RTL8188E_SUPPORT == 1)
	        if(pDM_Odm->SupportICType==ODM_RTL8188E)
	                odm_DynamicPrimaryCCA(pDM_Odm);	
#endif

	}
	pDM_Odm->PhyDbgInfo.NumQryBeaconPkt = 0;

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	odm_dtc(pDM_Odm);
#endif
}


VOID
ODM_CmnInfoInit(
	IN		PDM_ODM_T		pDM_Odm,
	IN		ODM_CMNINFO_E	CmnInfo,
	IN		u4Byte			Value	
	)
{
	switch	(CmnInfo)
	{
		case	ODM_CMNINFO_ABILITY:
			pDM_Odm->SupportAbility = (u4Byte)Value;
			break;

		case	ODM_CMNINFO_RF_TYPE:
			pDM_Odm->RFType = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_PLATFORM:
			pDM_Odm->SupportPlatform = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_INTERFACE:
			pDM_Odm->SupportInterface = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_MP_TEST_CHIP:
			pDM_Odm->bIsMPChip= (u1Byte)Value;
			break;
            
		case	ODM_CMNINFO_IC_TYPE:
			pDM_Odm->SupportICType = Value;
			break;

		case	ODM_CMNINFO_CUT_VER:
			pDM_Odm->CutVersion = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_FAB_VER:
			pDM_Odm->FabVersion = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_RFE_TYPE:
			pDM_Odm->RFEType = (u1Byte)Value;
			break;

		case    ODM_CMNINFO_RF_ANTENNA_TYPE:
			pDM_Odm->AntDivType= (u1Byte)Value;
			break;

		case	ODM_CMNINFO_BOARD_TYPE:
			pDM_Odm->BoardType = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_PACKAGE_TYPE:
			pDM_Odm->PackageType = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_EXT_LNA:
			pDM_Odm->ExtLNA = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_5G_EXT_LNA:
			pDM_Odm->ExtLNA5G = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_EXT_PA:
			pDM_Odm->ExtPA = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_5G_EXT_PA:
			pDM_Odm->ExtPA5G = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_GPA:
			pDM_Odm->TypeGPA= (ODM_TYPE_GPA_E)Value;
			break;
		case	ODM_CMNINFO_APA:
			pDM_Odm->TypeAPA= (ODM_TYPE_APA_E)Value;
			break;
		case	ODM_CMNINFO_GLNA:
			pDM_Odm->TypeGLNA= (ODM_TYPE_GLNA_E)Value;
			break;
		case	ODM_CMNINFO_ALNA:
			pDM_Odm->TypeALNA= (ODM_TYPE_ALNA_E)Value;
			break;

		case	ODM_CMNINFO_EXT_TRSW:
			pDM_Odm->ExtTRSW = (u1Byte)Value;
			break;
		case 	ODM_CMNINFO_PATCH_ID:
			pDM_Odm->PatchID = (u1Byte)Value;
			break;
		case 	ODM_CMNINFO_BINHCT_TEST:
			pDM_Odm->bInHctTest = (BOOLEAN)Value;
			break;
		case 	ODM_CMNINFO_BWIFI_TEST:
			pDM_Odm->bWIFITest = (BOOLEAN)Value;
			break;	

		case	ODM_CMNINFO_SMART_CONCURRENT:
			pDM_Odm->bDualMacSmartConcurrent = (BOOLEAN )Value;
			break;
		
		default:
			break;	
		
	}

}


VOID
ODM_CmnInfoHook(
	IN		PDM_ODM_T		pDM_Odm,
	IN		ODM_CMNINFO_E	CmnInfo,
	IN		PVOID			pValue	
	)
{
	switch	(CmnInfo)
	{
		case	ODM_CMNINFO_MAC_PHY_MODE:
			pDM_Odm->pMacPhyMode = (u1Byte *)pValue;
			break;
		
		case	ODM_CMNINFO_TX_UNI:
			pDM_Odm->pNumTxBytesUnicast = (u8Byte *)pValue;
			break;

		case	ODM_CMNINFO_RX_UNI:
			pDM_Odm->pNumRxBytesUnicast = (u8Byte *)pValue;
			break;

		case	ODM_CMNINFO_WM_MODE:
			pDM_Odm->pWirelessMode = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_BAND:
			pDM_Odm->pBandType = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_SEC_CHNL_OFFSET:
			pDM_Odm->pSecChOffset = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_SEC_MODE:
			pDM_Odm->pSecurity = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_BW:
			pDM_Odm->pBandWidth = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_CHNL:
			pDM_Odm->pChannel = (u1Byte *)pValue;
			break;
		
		case	ODM_CMNINFO_DMSP_GET_VALUE:
			pDM_Odm->pbGetValueFromOtherMac = (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_BUDDY_ADAPTOR:
			pDM_Odm->pBuddyAdapter = (PADAPTER *)pValue;
			break;

		case	ODM_CMNINFO_DMSP_IS_MASTER:
			pDM_Odm->pbMasterOfDMSP = (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_SCAN:
			pDM_Odm->pbScanInProcess = (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_POWER_SAVING:
			pDM_Odm->pbPowerSaving = (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_ONE_PATH_CCA:
			pDM_Odm->pOnePathCCA = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_DRV_STOP:
			pDM_Odm->pbDriverStopped =  (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_PNP_IN:
			pDM_Odm->pbDriverIsGoingToPnpSetPowerSleep =  (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_INIT_ON:
			pDM_Odm->pinit_adpt_in_progress =  (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_ANT_TEST:
			pDM_Odm->pAntennaTest =  (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_NET_CLOSED:
			pDM_Odm->pbNet_closed = (BOOLEAN *)pValue;
			break;

		case 	ODM_CMNINFO_FORCED_RATE:
			pDM_Odm->pForcedDataRate = (pu2Byte)pValue;
			break;

		case  ODM_CMNINFO_FORCED_IGI_LB:
			pDM_Odm->pu1ForcedIgiLb = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_MP_MODE:
			pDM_Odm->mp_mode = (u1Byte *)pValue;
			break;





		default:
			break;

	}

}


VOID
ODM_CmnInfoPtrArrayHook(
	IN		PDM_ODM_T		pDM_Odm,
	IN		ODM_CMNINFO_E	CmnInfo,
	IN		u2Byte			Index,
	IN		PVOID			pValue	
	)
{
	switch	(CmnInfo)
	{
		case	ODM_CMNINFO_STA_STATUS:
			pDM_Odm->pODM_StaInfo[Index] = (PSTA_INFO_T)pValue;
			break;		
		default:
			break;
	}
	
}


VOID
ODM_CmnInfoUpdate(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u4Byte			CmnInfo,
	IN		u8Byte			Value	
	)
{
	switch	(CmnInfo)
	{
		case ODM_CMNINFO_LINK_IN_PROGRESS:
			pDM_Odm->bLinkInProcess = (BOOLEAN)Value;
			break;
		
		case	ODM_CMNINFO_ABILITY:
			pDM_Odm->SupportAbility = (u4Byte)Value;
			break;

		case	ODM_CMNINFO_RF_TYPE:
			pDM_Odm->RFType = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_WIFI_DIRECT:
			pDM_Odm->bWIFI_Direct = (BOOLEAN)Value;
			break;

		case	ODM_CMNINFO_WIFI_DISPLAY:
			pDM_Odm->bWIFI_Display = (BOOLEAN)Value;
			break;

		case	ODM_CMNINFO_LINK:
			pDM_Odm->bLinked = (BOOLEAN)Value;
			break;
			
		case	ODM_CMNINFO_STATION_STATE:
			pDM_Odm->bsta_state = (BOOLEAN)Value;
			break;
			
		case	ODM_CMNINFO_RSSI_MIN:
			pDM_Odm->RSSI_Min= (u1Byte)Value;
			break;

		case	ODM_CMNINFO_DBG_COMP:
			pDM_Odm->DebugComponents = Value;
			break;

		case	ODM_CMNINFO_DBG_LEVEL:
			pDM_Odm->DebugLevel = (u4Byte)Value;
			break;
		case	ODM_CMNINFO_RA_THRESHOLD_HIGH:
			pDM_Odm->RateAdaptive.HighRSSIThresh = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_RA_THRESHOLD_LOW:
			pDM_Odm->RateAdaptive.LowRSSIThresh = (u1Byte)Value;
			break;
		case ODM_CMNINFO_BT_DISABLED:
			pDM_Odm->bBtDisabled = (BOOLEAN)Value;
			break;
			
		case ODM_CMNINFO_BT_HS_CONNECT_PROCESS:
			pDM_Odm->bBtConnectProcess = (BOOLEAN)Value;
			break;
		
		case ODM_CMNINFO_BT_HS_RSSI:
			pDM_Odm->btHsRssi = (u1Byte)Value;
			break;
			
		case	ODM_CMNINFO_BT_OPERATION:
			pDM_Odm->bBtHsOperation = (BOOLEAN)Value;
			break;

		case	ODM_CMNINFO_BT_LIMITED_DIG:
			pDM_Odm->bBtLimitedDig = (BOOLEAN)Value;
			break;	

		case	ODM_CMNINFO_BT_DISABLE_EDCA:
			pDM_Odm->bBtDisableEdcaTurbo = (BOOLEAN)Value;
			break;
			
/*
		case	ODM_CMNINFO_OP_MODE:
			pDM_Odm->OPMode = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_WM_MODE:
			pDM_Odm->WirelessMode = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_BAND:
			pDM_Odm->BandType = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_SEC_CHNL_OFFSET:
			pDM_Odm->SecChOffset = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_SEC_MODE:
			pDM_Odm->Security = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_BW:
			pDM_Odm->BandWidth = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_CHNL:
			pDM_Odm->Channel = (u1Byte)Value;
			break;			
*/	
                default:
			break;
	}

	
}

VOID
odm_CommonInfoSelfInit(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pFAT_T			pDM_FatTable = &pDM_Odm->DM_FatTable;
	pDM_Odm->bCckHighPower = (BOOLEAN) ODM_GetBBReg(pDM_Odm, ODM_REG(CCK_RPT_FORMAT,pDM_Odm), ODM_BIT(CCK_RPT_FORMAT,pDM_Odm));		
	pDM_Odm->RFPathRxEnable = (u1Byte) ODM_GetBBReg(pDM_Odm, ODM_REG(BB_RX_PATH,pDM_Odm), ODM_BIT(BB_RX_PATH,pDM_Odm));
#if (DM_ODM_SUPPORT_TYPE != ODM_CE)	
	pDM_Odm->pbNet_closed = &pDM_Odm->BOOLEAN_temp;
#endif

	ODM_InitDebugSetting(pDM_Odm);

	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
		pDM_Odm->AntDivType = SW_ANTDIV;
	}
	else if(pDM_Odm->SupportICType & (ODM_RTL8192C|ODM_RTL8192D))
	{
           #if(defined(CONFIG_HW_ANTENNA_DIVERSITY))	
		pDM_Odm->AntDivType = HW_ANTDIV;
           #elif (defined(CONFIG_SW_ANTENNA_DIVERSITY))
		pDM_Odm->AntDivType = SW_ANTDIV;
           #endif
	}
	pDM_Odm->TxRate = 0xFF;
#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))

	if(pDM_Odm->SupportICType==ODM_RTL8723B)
	{
		if((!pDM_Odm->DM_SWAT_Table.ANTA_ON || !pDM_Odm->DM_SWAT_Table.ANTB_ON))
			pDM_Odm->SupportAbility &= ~(ODM_BB_ANT_DIV);
	}

#elif (DM_ODM_SUPPORT_TYPE == ODM_AP)

	#if(defined(CONFIG_NOT_SUPPORT_ANTDIV)) 
		pDM_Odm->SupportAbility &= ~(ODM_BB_ANT_DIV);
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ Disable AntDiv function] : Not Support 2.4G & 5G Antenna Diversity\n"));
	#elif(defined(CONFIG_2G5G_SUPPORT_ANTDIV)) 
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ Enable AntDiv function] : 2.4G & 5G Support Antenna Diversity Simultaneously \n"));
		pDM_FatTable->AntDiv_2G_5G = (ODM_ANTDIV_2G|ODM_ANTDIV_5G);
		if(pDM_Odm->SupportICType & ODM_ANTDIV_SUPPORT)
			pDM_Odm->SupportAbility |= ODM_BB_ANT_DIV;
		if(*pDM_Odm->pBandType == ODM_BAND_5G )
		{
			#if ( defined(CONFIG_5G_CGCS_RX_DIVERSITY) )
				pDM_Odm->AntDivType = CGCS_RX_HW_ANTDIV; 
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 5G] : AntDiv Type = CGCS_RX_HW_ANTDIV\n"));
			#elif( defined(CONFIG_5G_CG_TRX_DIVERSITY) )
				pDM_Odm->AntDivType = CG_TRX_HW_ANTDIV;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 5G] : AntDiv Type = CG_TRX_HW_ANTDIV\n"));
			#elif( defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY) )
				pDM_Odm->AntDivType = CG_TRX_SMART_ANTDIV;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 5G] : AntDiv Type = CG_SMART_ANTDIV\n"));
			#endif
		}		
		else 	if(*pDM_Odm->pBandType == ODM_BAND_2_4G )
	        {
			#if ( defined(CONFIG_2G_CGCS_RX_DIVERSITY) )
				pDM_Odm->AntDivType = CGCS_RX_HW_ANTDIV;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 2.4G] : AntDiv Type = CGCS_RX_HW_ANTDIV\n"));
			#elif( defined(CONFIG_2G_CG_TRX_DIVERSITY) )
				pDM_Odm->AntDivType = CG_TRX_HW_ANTDIV;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 2.4G] : AntDiv Type = CG_TRX_HW_ANTDIV\n"));
			#elif( defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY) )
				pDM_Odm->AntDivType = CG_TRX_SMART_ANTDIV;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 2.4G] : AntDiv Type = CG_SMART_ANTDIV\n"));
			#endif
		}
        #elif(defined(CONFIG_5G_SUPPORT_ANTDIV))
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ Enable AntDiv function] : Only 5G Support Antenna Diversity\n"));
		pDM_FatTable->AntDiv_2G_5G = (ODM_ANTDIV_5G);
		if(*pDM_Odm->pBandType == ODM_BAND_5G )
		{
			if(pDM_Odm->SupportICType & ODM_ANTDIV_5G_SUPPORT_IC)
				pDM_Odm->SupportAbility |= ODM_BB_ANT_DIV;	
			#if ( defined(CONFIG_5G_CGCS_RX_DIVERSITY) )
				pDM_Odm->AntDivType = CGCS_RX_HW_ANTDIV;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 5G] : AntDiv Type = CGCS_RX_HW_ANTDIV\n"));
			#elif( defined(CONFIG_5G_CG_TRX_DIVERSITY) )
				pDM_Odm->AntDivType = CG_TRX_HW_ANTDIV;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 5G] : AntDiv Type = CG_TRX_HW_ANTDIV\n"));
			#elif( defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY) )
				pDM_Odm->AntDivType = CG_TRX_SMART_ANTDIV;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 5G] : AntDiv Type = CG_SMART_ANTDIV\n"));
			#endif
	        }
		else if(*pDM_Odm->pBandType == ODM_BAND_2_4G )
		{
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("Not Support 2G AntDivType\n"));
			pDM_Odm->SupportAbility &= ~(ODM_BB_ANT_DIV);
		}
	#elif(defined(CONFIG_2G_SUPPORT_ANTDIV)) 
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ Enable AntDiv function] : Only 2.4G Support Antenna Diversity\n"));
		pDM_FatTable->AntDiv_2G_5G = (ODM_ANTDIV_2G);
		if(*pDM_Odm->pBandType == ODM_BAND_2_4G )
		{
			if(pDM_Odm->SupportICType & ODM_ANTDIV_2G_SUPPORT_IC)
				pDM_Odm->SupportAbility |= ODM_BB_ANT_DIV;
			#if ( defined(CONFIG_2G_CGCS_RX_DIVERSITY) )
				pDM_Odm->AntDivType = CGCS_RX_HW_ANTDIV;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 2.4G] : AntDiv Type = CGCS_RX_HW_ANTDIV\n"));
			#elif( defined(CONFIG_2G_CG_TRX_DIVERSITY) )
				pDM_Odm->AntDivType = CG_TRX_HW_ANTDIV;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 2.4G] : AntDiv Type = CG_TRX_HW_ANTDIV\n"));
			#elif( defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY) )
				pDM_Odm->AntDivType = CG_TRX_SMART_ANTDIV;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("[ 2.4G] : AntDiv Type = CG_SMART_ANTDIV\n"));
                        #endif
	        }
		else if(*pDM_Odm->pBandType == ODM_BAND_5G )
		{
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("Not Support 5G AntDivType\n"));
			pDM_Odm->SupportAbility &= ~(ODM_BB_ANT_DIV);
		}
	#endif
#endif
#endif

}

VOID
odm_CommonInfoSelfUpdate(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	u1Byte	EntryCnt=0;
	u1Byte	i;
	PSTA_INFO_T   	pEntry;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

	PADAPTER	Adapter =  pDM_Odm->Adapter;
	PMGNT_INFO	pMgntInfo = &Adapter->MgntInfo;

	pEntry = pDM_Odm->pODM_StaInfo[0];
	if(pMgntInfo->mAssoc)
	{
		pEntry->bUsed=TRUE;
		for (i=0; i<6; i++)
			pEntry->MacAddr[i] = pMgntInfo->Bssid[i];
	}
	else
	{
		pEntry->bUsed=FALSE;
		for (i=0; i<6; i++)
			pEntry->MacAddr[i] = 0;
	}
#endif


	if(*(pDM_Odm->pBandWidth) == ODM_BW40M)
	{
		if(*(pDM_Odm->pSecChOffset) == 1)
			pDM_Odm->ControlChannel = *(pDM_Odm->pChannel) -2;
		else if(*(pDM_Odm->pSecChOffset) == 2)
			pDM_Odm->ControlChannel = *(pDM_Odm->pChannel) +2;
	}
	else
		pDM_Odm->ControlChannel = *(pDM_Odm->pChannel);

	for (i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		pEntry = pDM_Odm->pODM_StaInfo[i];
		if(IS_STA_VALID(pEntry))
			EntryCnt++;
	}
	if(EntryCnt == 1)
		pDM_Odm->bOneEntryOnly = TRUE;
	else
		pDM_Odm->bOneEntryOnly = FALSE;
}

VOID
odm_CmnInfoInit_Debug(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("odm_CmnInfoInit_Debug==>\n"));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("SupportPlatform=%d\n",pDM_Odm->SupportPlatform) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("SupportAbility=0x%x\n",pDM_Odm->SupportAbility) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("SupportInterface=%d\n",pDM_Odm->SupportInterface) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("SupportICType=0x%x\n",pDM_Odm->SupportICType) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("CutVersion=%d\n",pDM_Odm->CutVersion) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("FabVersion=%d\n",pDM_Odm->FabVersion) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("RFType=%d\n",pDM_Odm->RFType) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("BoardType=%d\n",pDM_Odm->BoardType) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("ExtLNA=%d\n",pDM_Odm->ExtLNA) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("ExtPA=%d\n",pDM_Odm->ExtPA) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("ExtTRSW=%d\n",pDM_Odm->ExtTRSW) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("PatchID=%d\n",pDM_Odm->PatchID) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bInHctTest=%d\n",pDM_Odm->bInHctTest) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bWIFITest=%d\n",pDM_Odm->bWIFITest) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bDualMacSmartConcurrent=%d\n",pDM_Odm->bDualMacSmartConcurrent) );

}

VOID
odm_CmnInfoHook_Debug(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("odm_CmnInfoHook_Debug==>\n"));	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pNumTxBytesUnicast=%llu\n",*(pDM_Odm->pNumTxBytesUnicast)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pNumRxBytesUnicast=%llu\n",*(pDM_Odm->pNumRxBytesUnicast)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pWirelessMode=0x%x\n",*(pDM_Odm->pWirelessMode)) );	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pSecChOffset=%d\n",*(pDM_Odm->pSecChOffset)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pSecurity=%d\n",*(pDM_Odm->pSecurity)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pBandWidth=%d\n",*(pDM_Odm->pBandWidth)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pChannel=%d\n",*(pDM_Odm->pChannel)) );

	if(pDM_Odm->SupportICType==ODM_RTL8192D)
	{
		if(pDM_Odm->pBandType)
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pBandType=%d\n",*(pDM_Odm->pBandType)) );
		if(pDM_Odm->pMacPhyMode)
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pMacPhyMode=%d\n",*(pDM_Odm->pMacPhyMode)) );
		if(pDM_Odm->pBuddyAdapter)
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pbGetValueFromOtherMac=%d\n",*(pDM_Odm->pbGetValueFromOtherMac)) );
		if(pDM_Odm->pBuddyAdapter)
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pBuddyAdapter=%p\n",*(pDM_Odm->pBuddyAdapter)) );
		if(pDM_Odm->pbMasterOfDMSP)
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pbMasterOfDMSP=%d\n",*(pDM_Odm->pbMasterOfDMSP)) );
	}
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pbScanInProcess=%d\n",*(pDM_Odm->pbScanInProcess)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pbPowerSaving=%d\n",*(pDM_Odm->pbPowerSaving)) );

	if(pDM_Odm->SupportPlatform & (ODM_AP|ODM_ADSL))
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pOnePathCCA=%d\n",*(pDM_Odm->pOnePathCCA)) );
}

VOID
odm_CmnInfoUpdate_Debug(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("odm_CmnInfoUpdate_Debug==>\n"));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bWIFI_Direct=%d\n",pDM_Odm->bWIFI_Direct) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bWIFI_Display=%d\n",pDM_Odm->bWIFI_Display) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bLinked=%d\n",pDM_Odm->bLinked) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("RSSI_Min=%d\n",pDM_Odm->RSSI_Min) );
}

VOID
odm_BasicDbgMessage
(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	PFALSE_ALARM_STATISTICS FalseAlmCnt = &(pDM_Odm->FalseAlmCnt);
	pDIG_T	pDM_DigTable = &pDM_Odm->DM_DigTable;
	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("odm_BasicDbgMsg==>\n"));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bLinked = %d, RSSI_Min = %d, CurrentIGI = 0x%x \n",
		pDM_Odm->bLinked, pDM_Odm->RSSI_Min, pDM_DigTable->CurIGValue) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("Cnt_Cck_fail = %d, Cnt_Ofdm_fail = %d, Total False Alarm = %d\n",	
		FalseAlmCnt->Cnt_Cck_fail, FalseAlmCnt->Cnt_Ofdm_fail, FalseAlmCnt->Cnt_all));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("RxRate = 0x%x, RSSI_A = %d, RSSI_B = %d\n", 
		pDM_Odm->RxRate, pDM_Odm->RSSI_A, pDM_Odm->RSSI_B));

}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID
ODM_InitAllWorkItems(IN PDM_ODM_T	pDM_Odm )
{
#if USE_WORKITEM
	PADAPTER		pAdapter = pDM_Odm->Adapter;

	ODM_InitializeWorkItem(	pDM_Odm, 
							&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchWorkitem_8723B, 
							(RT_WORKITEM_CALL_BACK)ODM_SW_AntDiv_WorkitemCallback,
							(PVOID)pAdapter,
							"AntennaSwitchWorkitem");
	
	ODM_InitializeWorkItem(	pDM_Odm, 
							&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchWorkitem, 
							(RT_WORKITEM_CALL_BACK)odm_SwAntDivChkAntSwitchWorkitemCallback,
							(PVOID)pAdapter,
							"AntennaSwitchWorkitem");
	

	ODM_InitializeWorkItem(
		pDM_Odm,
		&(pDM_Odm->PathDivSwitchWorkitem), 
		(RT_WORKITEM_CALL_BACK)odm_PathDivChkAntSwitchWorkitemCallback, 
		(PVOID)pAdapter,
		"SWAS_WorkItem");

	ODM_InitializeWorkItem(
		pDM_Odm,
		&(pDM_Odm->CCKPathDiversityWorkitem), 
		(RT_WORKITEM_CALL_BACK)odm_CCKTXPathDiversityWorkItemCallback, 
		(PVOID)pAdapter,
		"CCKTXPathDiversityWorkItem");

	ODM_InitializeWorkItem(
		pDM_Odm,
		&(pDM_Odm->MPT_DIGWorkitem), 
		(RT_WORKITEM_CALL_BACK)odm_MPT_DIGWorkItemCallback, 
		(PVOID)pAdapter,
		"MPT_DIGWorkitem");

	ODM_InitializeWorkItem(
		pDM_Odm,
		&(pDM_Odm->RaRptWorkitem), 
		(RT_WORKITEM_CALL_BACK)ODM_UpdateInitRateWorkItemCallback, 
		(PVOID)pAdapter,
		"RaRptWorkitem");
	
#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))
#if (RTL8188E_SUPPORT == 1)
	ODM_InitializeWorkItem(
		pDM_Odm,
		&(pDM_Odm->FastAntTrainingWorkitem), 
		(RT_WORKITEM_CALL_BACK)odm_FastAntTrainingWorkItemCallback, 
		(PVOID)pAdapter,
		"FastAntTrainingWorkitem");
#endif
#endif
	ODM_InitializeWorkItem(
		pDM_Odm,
		&(pDM_Odm->DM_RXHP_Table.PSDTimeWorkitem), 
		(RT_WORKITEM_CALL_BACK)odm_PSD_RXHPWorkitemCallback, 
		(PVOID)pAdapter,
		"PSDRXHP_WorkItem");  
#endif
}

VOID
ODM_FreeAllWorkItems(IN PDM_ODM_T	pDM_Odm )
{
#if USE_WORKITEM
	ODM_FreeWorkItem(	&(pDM_Odm->DM_SWAT_Table.SwAntennaSwitchWorkitem_8723B));
	
	ODM_FreeWorkItem(	&(pDM_Odm->DM_SWAT_Table.SwAntennaSwitchWorkitem));

	ODM_FreeWorkItem(&(pDM_Odm->PathDivSwitchWorkitem));      

	ODM_FreeWorkItem(&(pDM_Odm->CCKPathDiversityWorkitem));
	
	ODM_FreeWorkItem(&(pDM_Odm->FastAntTrainingWorkitem));

	ODM_FreeWorkItem(&(pDM_Odm->MPT_DIGWorkitem));

	ODM_FreeWorkItem(&(pDM_Odm->RaRptWorkitem));

	ODM_FreeWorkItem((&pDM_Odm->DM_RXHP_Table.PSDTimeWorkitem));
#endif

}
#endif

/*
VOID
odm_FindMinimumRSSI(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	u4Byte	i;
	u1Byte	RSSI_Min = 0xFF;

	for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		if(IS_STA_VALID(pDM_Odm->pODM_StaInfo[i]) )
		{
			if(pDM_Odm->pODM_StaInfo[i]->RSSI_Ave < RSSI_Min)
			{
				RSSI_Min = pDM_Odm->pODM_StaInfo[i]->RSSI_Ave;
			}
		}
	}

	pDM_Odm->RSSI_Min = RSSI_Min;

}

VOID
odm_IsLinked(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	u4Byte i;
	BOOLEAN Linked = FALSE;
	
	for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
			if(IS_STA_VALID(pDM_Odm->pODM_StaInfo[i]) )
			{			
				Linked = TRUE;
				break;
			}
		
	}

	pDM_Odm->bLinked = Linked;
}
*/


/*-----------------------------------------------------------------------------
 * Function:	odm_DIGInit()
 *
 * Overview:	Set DIG scheme init value.
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *
 *---------------------------------------------------------------------------*/
VOID
ODM_ChangeDynamicInitGainThresh(
	IN	PDM_ODM_T	pDM_Odm,
	IN	u4Byte		DM_Type,
	IN	u4Byte		DM_Value
	)
{
	pDIG_T	pDM_DigTable = &pDM_Odm->DM_DigTable;

	if (DM_Type == DIG_TYPE_THRESH_HIGH)
	{
		pDM_DigTable->RssiHighThresh = DM_Value;		
	}
	else if (DM_Type == DIG_TYPE_THRESH_LOW)
	{
		pDM_DigTable->RssiLowThresh = DM_Value;
	}
	else if (DM_Type == DIG_TYPE_ENABLE)
	{
		pDM_DigTable->Dig_Enable_Flag	= TRUE;
	}	
	else if (DM_Type == DIG_TYPE_DISABLE)
	{
		pDM_DigTable->Dig_Enable_Flag = FALSE;
	}	
	else if (DM_Type == DIG_TYPE_BACKOFF)
	{
		if(DM_Value > 30)
			DM_Value = 30;
		pDM_DigTable->BackoffVal = (u1Byte)DM_Value;
	}
	else if(DM_Type == DIG_TYPE_RX_GAIN_MIN)
	{
		if(DM_Value == 0)
			DM_Value = 0x1;
		pDM_DigTable->rx_gain_range_min = (u1Byte)DM_Value;
	}
	else if(DM_Type == DIG_TYPE_RX_GAIN_MAX)
	{
		if(DM_Value > 0x50)
			DM_Value = 0x50;
		pDM_DigTable->rx_gain_range_max = (u1Byte)DM_Value;
	}
}	/* DM_ChangeDynamicInitGainThresh */

int getIGIForDiff(int value_IGI)
{
	#define ONERCCA_LOW_TH		0x30
	#define ONERCCA_LOW_DIFF	8

	if (value_IGI < ONERCCA_LOW_TH) {
		if ((ONERCCA_LOW_TH - value_IGI) < ONERCCA_LOW_DIFF)
			return ONERCCA_LOW_TH;
		else
			return value_IGI + ONERCCA_LOW_DIFF;
	} else {
		return value_IGI;
	}
}


VOID
odm_AdaptivityInit(
IN PDM_ODM_T pDM_Odm
)
{
	if(pDM_Odm->SupportICType == ODM_RTL8723B)
	{
		pDM_Odm->TH_L2H_ini = 0xf8;
	}
	if((pDM_Odm->SupportICType == ODM_RTL8192E)&&(pDM_Odm->SupportInterface == ODM_ITRF_PCIE))
	{
		pDM_Odm->TH_L2H_ini = 0xf0;
	}
	else
	{
		pDM_Odm->TH_L2H_ini = 0xf9;
	}
	
	pDM_Odm->TH_EDCCA_HL_diff = 7;
	pDM_Odm->IGI_Base = 0x32;
	pDM_Odm->IGI_target = 0x1c;
	pDM_Odm->ForceEDCCA = 0;
	pDM_Odm->AdapEn_RSSI = 20;


}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

VOID
odm_EnableEDCCA(
	IN		PDM_ODM_T		pDM_Odm
)
{

	PADAPTER		pAdapter	= pDM_Odm->Adapter;

	if((pDM_Odm->SupportICType == ODM_RTL8723A)&&(IS_WIRELESS_MODE_G(pAdapter)))
	{
		ODM_Write1Byte(pDM_Odm,rOFDM0_ECCAThreshold,0x00);
		ODM_Write1Byte(pDM_Odm,rOFDM0_ECCAThreshold+2,0xFD);
		
	}	
	else
	{
		ODM_Write1Byte(pDM_Odm,rOFDM0_ECCAThreshold,0x03);
		ODM_Write1Byte(pDM_Odm,rOFDM0_ECCAThreshold+2,0x00);
	}	
	
}

VOID
odm_DisableEDCCA(
	IN		PDM_ODM_T		pDM_Odm
)
{	
	ODM_Write1Byte(pDM_Odm, rOFDM0_ECCAThreshold, 0x7f);
	ODM_Write1Byte(pDM_Odm, rOFDM0_ECCAThreshold+2, 0x7f);
}

VOID
odm_DynamicEDCCA(
	IN		PDM_ODM_T		pDM_Odm
)
{
	PADAPTER		pAdapter	= pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	u1Byte	RegC50, RegC58;
	BOOLEAN		bFwCurrentInPSMode=FALSE;	

	pAdapter->HalFunc.GetHwRegHandler(pAdapter, HW_VAR_FW_PSMODE_STATUS, (pu1Byte)(&bFwCurrentInPSMode));	

	if(bFwCurrentInPSMode)
		return;
	
	RegC50 = (u1Byte)ODM_GetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskByte0);
	RegC58 = (u1Byte)ODM_GetBBReg(pDM_Odm, rOFDM0_XBAGCCore1, bMaskByte0);


 	if((RegC50 > 0x28 && RegC58 > 0x28) ||
  		((pDM_Odm->SupportICType == ODM_RTL8723A && IS_WIRELESS_MODE_G(pAdapter) && RegC50>0x26)) ||
  		(pDM_Odm->SupportICType == ODM_RTL8188E && RegC50 > 0x28))
	{
		if(!pHalData->bPreEdccaEnable)
		{
			odm_EnableEDCCA(pDM_Odm);
			pHalData->bPreEdccaEnable = TRUE;
		}
		
	}
	else if((RegC50 < 0x25 && RegC58 < 0x25) || (pDM_Odm->SupportICType == ODM_RTL8188E && RegC50 < 0x25))
	{
		if(pHalData->bPreEdccaEnable)
		{
			odm_DisableEDCCA(pDM_Odm);
			pHalData->bPreEdccaEnable = FALSE;
		}
	}
}


#endif   

VOID
odm_Adaptivity(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			IGI
)
{
	s1Byte TH_L2H_dmc, TH_H2L_dmc;
	s1Byte Diff, IGI_target;
	BOOLEAN EDCCA_State = 0;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER		pAdapter	= pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	BOOLEAN		bFwCurrentInPSMode=FALSE;	
	PMGNT_INFO				pMgntInfo = &(pAdapter->MgntInfo);
		
	pAdapter->HalFunc.GetHwRegHandler(pAdapter, HW_VAR_FW_PSMODE_STATUS, (pu1Byte)(&bFwCurrentInPSMode));	

	if(bFwCurrentInPSMode)
		return;
#endif

	if(!(pDM_Odm->SupportAbility & ODM_BB_ADAPTIVITY))
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("Go to odm_DynamicEDCCA() \n"));
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
			odm_DynamicEDCCA(pDM_Odm);
#endif
		return;
	}
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_Adaptivity() =====> \n"));

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("ForceEDCCA=%d, IGI_Base=0x%x, TH_L2H_ini = %d, TH_EDCCA_HL_diff = %d, AdapEn_RSSI = %d\n", 
		pDM_Odm->ForceEDCCA, pDM_Odm->IGI_Base, pDM_Odm->TH_L2H_ini, pDM_Odm->TH_EDCCA_HL_diff, pDM_Odm->AdapEn_RSSI));

	if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
		ODM_SetBBReg(pDM_Odm, 0x800, BIT10, 0);
	
	if((!pDM_Odm->bLinked)||(*pDM_Odm->pChannel > 149))
	{
		if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
		{
			ODM_SetBBReg(pDM_Odm,rOFDM0_ECCAThreshold, bMaskByte0, 0x7f);
			ODM_SetBBReg(pDM_Odm,rOFDM0_ECCAThreshold, bMaskByte2, 0x7f);
		}
		else
			ODM_SetBBReg(pDM_Odm, rFPGA0_XB_LSSIReadBack, 0xFFFF, (0x7f<<8) | 0x7f);
		return;
	}

#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)	
		if(pMgntInfo->IOTPeer == HT_IOT_PEER_BROADCOM)
			ODM_Write1Byte(pDM_Odm, REG_TRX_SIFS_OFDM, 0x0a); 
		else
			ODM_Write1Byte(pDM_Odm, REG_TRX_SIFS_OFDM, 0x0e);
#endif
	if(!pDM_Odm->ForceEDCCA)
	{
		if(pDM_Odm->RSSI_Min > pDM_Odm->AdapEn_RSSI)
			EDCCA_State = 1;
		else if(pDM_Odm->RSSI_Min < (pDM_Odm->AdapEn_RSSI - 5))
			EDCCA_State = 0;
	}
	else
		EDCCA_State = 1;
	{

		if(*pDM_Odm->pBandWidth == ODM_BW20M)
			IGI_target = pDM_Odm->IGI_Base;
		else if(*pDM_Odm->pBandWidth == ODM_BW40M)
			IGI_target = pDM_Odm->IGI_Base + 2;
		else if(*pDM_Odm->pBandWidth == ODM_BW80M)
			IGI_target = pDM_Odm->IGI_Base + 6;
		else
			IGI_target = pDM_Odm->IGI_Base;
	}

	pDM_Odm->IGI_target = (u1Byte) IGI_target;

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("BandWidth=%s, IGI_target=0x%x, EDCCA_State=%d\n",
		(*pDM_Odm->pBandWidth==ODM_BW80M)?"80M":((*pDM_Odm->pBandWidth==ODM_BW40M)?"40M":"20M"), IGI_target, EDCCA_State));

	if(EDCCA_State == 1)
	{
		Diff = IGI_target -(s1Byte)IGI;
		TH_L2H_dmc = pDM_Odm->TH_L2H_ini + Diff;
		if(TH_L2H_dmc > 10) 	TH_L2H_dmc = 10;
		TH_H2L_dmc = TH_L2H_dmc - pDM_Odm->TH_EDCCA_HL_diff;
	}
	else
	{
		TH_L2H_dmc = 0x7f;
		TH_H2L_dmc = 0x7f;
	}
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("IGI=0x%x, TH_L2H_dmc = %d, TH_H2L_dmc = %d\n", 
		IGI, TH_L2H_dmc, TH_H2L_dmc));

	if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
	{
		ODM_SetBBReg(pDM_Odm,rOFDM0_ECCAThreshold, bMaskByte0, (u1Byte)TH_L2H_dmc);
		ODM_SetBBReg(pDM_Odm,rOFDM0_ECCAThreshold, bMaskByte2, (u1Byte)TH_H2L_dmc);
	}
	else
		ODM_SetBBReg(pDM_Odm, rFPGA0_XB_LSSIReadBack, 0xFFFF, ((u1Byte)TH_H2L_dmc<<8) | (u1Byte)TH_L2H_dmc);
}

VOID
ODM_DynamicATCSwitch_init(
	IN 		PDM_ODM_T 		pDM_Odm
)
{
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN )

	pDM_Odm->CrystalCap = pHalData->CrystalCap;
	pDM_Odm->bATCStatus = (u1Byte)ODM_GetBBReg(pDM_Odm, rOFDM1_CFOTracking, BIT11);
	pDM_Odm->CFOThreshold = CFO_Threshold_Xtal;

#endif
}

VOID
ODM_DynamicATCSwitch(
	IN 		PDM_ODM_T 		pDM_Odm
)
{
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u1Byte 			CrystalCap,ATC_status_temp = 0;
	u4Byte			packet_count;
	int				CFO_kHz_A,CFO_kHz_B,CFO_ave = 0, Adjust_Xtal = 0;
	int				CFO_ave_diff;

#if (MP_DRIVER == 1)
	if ( *(pDM_Odm->mp_mode) == 1)
		pDM_Odm->bLinked = TRUE;
#endif 

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN )

	if(!(pDM_Odm->SupportAbility & ODM_BB_DYNAMIC_ATC))
		return;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("=========> ODM_DynamicATCSwitch()\n"));

	if(!pDM_Odm->bLinked)
	{	
		if(pDM_Odm->bATCStatus == ATC_Status_Off)
		{
			if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
				ODM_SetBBReg(pDM_Odm, rOFDM1_CFOTracking, BIT11, ATC_Status_On);
			
			if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
				ODM_SetBBReg(pDM_Odm, rFc_area_Jaguar, BIT14, ATC_Status_On);
			
			pDM_Odm->bATCStatus = ATC_Status_On;
		}

		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): No link!!\n"));
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): ATCStatus = %d\n", pDM_Odm->bATCStatus));

		if(!pDM_Odm->bBtDisabled)
		{
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): Disable CFO tracking for BT!!\n"));
			return;
		}

		if(pDM_Odm->CrystalCap != pHalData->CrystalCap)
		{
			pDM_Odm->CrystalCap = pHalData->CrystalCap;
			CrystalCap = pDM_Odm->CrystalCap & 0x3f;
			odm_Write_CrystalCap(pDM_Odm,CrystalCap);
		}
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): CrystalCap = 0x%x\n", pDM_Odm->CrystalCap));
		
	}
	else
	{

		CFO_kHz_A =  (int)(pDM_Odm->CFO_tail[0] * 3125)  / 1280;
		CFO_kHz_B =  (int)(pDM_Odm->CFO_tail[1] * 3125)  / 1280;
		packet_count = pDM_Odm->packetCount;
		
		if(packet_count == pDM_Odm->packetCount_pre)
		{
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): packet counter doesn't change\n"));
			return;
		}
		pDM_Odm->packetCount_pre = packet_count;
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): packet counter = %d\n", pDM_Odm->packetCount));
		
		if(pDM_Odm->RFType == ODM_1T1R)
			CFO_ave = CFO_kHz_A;
		else
			CFO_ave = (int)(CFO_kHz_A + CFO_kHz_B) >> 1;

		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): CFO_kHz_A = %dkHz, CFO_kHz_B = %dkHz, CFO_ave = %dkHz\n", 
						CFO_kHz_A, CFO_kHz_B, CFO_ave));

		CFO_ave_diff = (pDM_Odm->CFO_ave_pre >= CFO_ave)?(pDM_Odm->CFO_ave_pre - CFO_ave):(CFO_ave - pDM_Odm->CFO_ave_pre);
		if(CFO_ave_diff > 20 && pDM_Odm->largeCFOHit == 0)
		{
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): first large CFO hit\n"));
			pDM_Odm->largeCFOHit = 1;
			return;
		}
		else
			pDM_Odm->largeCFOHit = 0;
		pDM_Odm->CFO_ave_pre = CFO_ave;

		if (pDM_Odm->bBtDisabled)
		{
			if(CFO_ave >= -pDM_Odm->CFOThreshold && CFO_ave <= pDM_Odm->CFOThreshold && pDM_Odm->bIsfreeze == 0)
			{
				if (pDM_Odm->CFOThreshold == CFO_Threshold_Xtal)
				{
					pDM_Odm->CFOThreshold = CFO_Threshold_Xtal + 10;
					pDM_Odm->bIsfreeze = 1;
				}
				else
					pDM_Odm->CFOThreshold = CFO_Threshold_Xtal;
			}
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): Dynamic threshold = %d\n", pDM_Odm->CFOThreshold));
			
			if(CFO_ave > pDM_Odm->CFOThreshold && pDM_Odm->CrystalCap < 0x3f)
				Adjust_Xtal =  ((CFO_ave - CFO_Threshold_Xtal) >> 2) + 1;
			else if(CFO_ave < (-pDM_Odm->CFOThreshold) && pDM_Odm->CrystalCap > 0)
				Adjust_Xtal =  ((CFO_ave + CFO_Threshold_Xtal) >> 2) - 1;
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): Crystal cap = 0x%x, Crystal cap offset = %d\n", pDM_Odm->CrystalCap, Adjust_Xtal));

			if(Adjust_Xtal != 0)
			{
				pDM_Odm->bIsfreeze = 0;
				pDM_Odm->CrystalCap = pDM_Odm->CrystalCap + Adjust_Xtal;

				if(pDM_Odm->CrystalCap > 0x3f)
					pDM_Odm->CrystalCap = 0x3f;
				else if (pDM_Odm->CrystalCap < 0)
					pDM_Odm->CrystalCap = 0;

				CrystalCap = pDM_Odm->CrystalCap & 0x3f;
				odm_Write_CrystalCap(pDM_Odm,CrystalCap);
	
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): New crystal cap = 0x%x \n", pDM_Odm->CrystalCap));
			}
		}
		else if(pDM_Odm->CrystalCap != pHalData->CrystalCap)
		{
			pDM_Odm->CrystalCap = pHalData->CrystalCap;
			CrystalCap = pDM_Odm->CrystalCap & 0x3f;
			odm_Write_CrystalCap(pDM_Odm,CrystalCap);
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): Disable CFO tracking for BT!! (CrystalCap is reset)\n"));
		}
		else
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): Disable CFO tracking for BT!! (CrystalCap is unchanged)\n"));
		if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES){
				if(CFO_ave < CFO_Threshold_ATC && CFO_ave > -CFO_Threshold_ATC)
				{
					if(pDM_Odm->bATCStatus == ATC_Status_On)
					{
					ODM_SetBBReg(pDM_Odm, rOFDM1_CFOTracking, BIT11, ATC_Status_Off);
					pDM_Odm->bATCStatus = ATC_Status_Off;
					}
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): Disable ATC!!\n"));
				}
				else
				{
					if(pDM_Odm->bATCStatus == ATC_Status_Off)
					{
						ODM_SetBBReg(pDM_Odm, rOFDM1_CFOTracking, BIT11, ATC_Status_On);
						pDM_Odm->bATCStatus = ATC_Status_On;
					}
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_DYNAMIC_ATC, ODM_DBG_LOUD, ("ODM_DynamicATCSwitch(): Enable ATC!!\n"));
				}
		}
	}
#endif
}

VOID
odm_Write_CrystalCap(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			CrystalCap
)
{
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	if(IS_HARDWARE_TYPE_8192D(Adapter))
	{
		PHY_SetBBReg(Adapter, 0x24, 0xF0, CrystalCap & 0x0F);
		PHY_SetBBReg(Adapter, 0x28, 0xF0000000, ((CrystalCap & 0xF0) >> 4));
	}

	if(IS_HARDWARE_TYPE_8188E(Adapter))
	{
		PHY_SetBBReg(Adapter, REG_AFE_XTAL_CTRL, 0x7ff800, (CrystalCap | (CrystalCap << 6)));
	}
	
	if(IS_HARDWARE_TYPE_8812(Adapter))
	{
		CrystalCap = CrystalCap & 0x3F;
		PHY_SetBBReg(Adapter, REG_MAC_PHY_CTRL, 0x7FF80000, (CrystalCap | (CrystalCap << 6)));
	}	
	
	if ((IS_HARDWARE_TYPE_8723A(Adapter) && pHalData->EEPROMVersion >= 0x01) ||
		IS_HARDWARE_TYPE_8723B(Adapter) ||IS_HARDWARE_TYPE_8192E(Adapter) || IS_HARDWARE_TYPE_8821(Adapter))
	{
		CrystalCap = CrystalCap & 0x3F;
		PHY_SetBBReg(Adapter, REG_MAC_PHY_CTRL, 0xFFF000, (CrystalCap | (CrystalCap << 6)));	
	}
	
	if(IS_HARDWARE_TYPE_8723AE(Adapter))
		PHY_SetBBReg(Adapter, REG_LDOA15_CTRL, bMaskDWord, 0x01572505);				

}


VOID
ODM_Write_DIG(
	IN	PDM_ODM_T		pDM_Odm,
	IN	u1Byte			CurrentIGI
	)
{
	pDIG_T	pDM_DigTable = &pDM_Odm->DM_DigTable;

	if(pDM_Odm->StopDIG)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("Stop Writing IGI\n"));
		return;
	}

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("ODM_REG(IGI_A,pDM_Odm)=0x%x, ODM_BIT(IGI,pDM_Odm)=0x%x \n",
		ODM_REG(IGI_A,pDM_Odm),ODM_BIT(IGI,pDM_Odm)));

	if(pDM_DigTable->CurIGValue != CurrentIGI)//if(pDM_DigTable->PreIGValue != CurrentIGI)
	{
		if(pDM_Odm->SupportPlatform & (ODM_CE|ODM_WIN))
		{ 
				ODM_SetBBReg(pDM_Odm, ODM_REG(IGI_A,pDM_Odm), ODM_BIT(IGI,pDM_Odm), CurrentIGI);
			if(pDM_Odm->RFType != ODM_1T1R)
				ODM_SetBBReg(pDM_Odm, ODM_REG(IGI_B,pDM_Odm), ODM_BIT(IGI,pDM_Odm), CurrentIGI);
			}
		else if(pDM_Odm->SupportPlatform & (ODM_AP|ODM_ADSL))
		{
			switch(*(pDM_Odm->pOnePathCCA))
			{
			case ODM_CCA_2R:
			ODM_SetBBReg(pDM_Odm, ODM_REG(IGI_A,pDM_Odm), ODM_BIT(IGI,pDM_Odm), CurrentIGI);
					if(pDM_Odm->RFType != ODM_1T1R)
					ODM_SetBBReg(pDM_Odm, ODM_REG(IGI_B,pDM_Odm), ODM_BIT(IGI,pDM_Odm), CurrentIGI);
				break;
			case ODM_CCA_1R_A:
				ODM_SetBBReg(pDM_Odm, ODM_REG(IGI_A,pDM_Odm), ODM_BIT(IGI,pDM_Odm), CurrentIGI);
					if(pDM_Odm->RFType != ODM_1T1R)
					ODM_SetBBReg(pDM_Odm, ODM_REG(IGI_B,pDM_Odm), ODM_BIT(IGI,pDM_Odm), getIGIForDiff(CurrentIGI));
				break;
			case ODM_CCA_1R_B:
				ODM_SetBBReg(pDM_Odm, ODM_REG(IGI_A,pDM_Odm), ODM_BIT(IGI,pDM_Odm), getIGIForDiff(CurrentIGI));
					if(pDM_Odm->RFType != ODM_1T1R)
					ODM_SetBBReg(pDM_Odm, ODM_REG(IGI_B,pDM_Odm), ODM_BIT(IGI,pDM_Odm), CurrentIGI);
					break;
				}
		}
		
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("CurrentIGI(0x%02x). \n",CurrentIGI));
		pDM_DigTable->CurIGValue = CurrentIGI;
	}	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("ODM_Write_DIG():CurrentIGI=0x%x \n",CurrentIGI));
	
}

VOID
odm_DIGbyRSSI_LPS(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	PFALSE_ALARM_STATISTICS		pFalseAlmCnt = &pDM_Odm->FalseAlmCnt;

#if 0	
	struct mlme_priv	*pmlmepriv = &(pAdapter->mlmepriv);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);	
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
#endif 

	u1Byte	RSSI_Lower=DM_DIG_MIN_NIC;  
	u1Byte	CurrentIGI=pDM_Odm->RSSI_Min;

	CurrentIGI=CurrentIGI+RSSI_OFFSET_DIG;




	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("---Neil---odm_DIG is in LPS mode\n"));
	if(pFalseAlmCnt->Cnt_all> DM_DIG_FA_TH2_LPS)
		CurrentIGI = CurrentIGI+2;
	else if (pFalseAlmCnt->Cnt_all > DM_DIG_FA_TH1_LPS)
		CurrentIGI = CurrentIGI+1;
	else if(pFalseAlmCnt->Cnt_all < DM_DIG_FA_TH0_LPS)
		CurrentIGI = CurrentIGI-1;	



	if((pDM_Odm->RSSI_Min-10) > DM_DIG_MIN_NIC)
		RSSI_Lower =(pDM_Odm->RSSI_Min-10);
	else
		RSSI_Lower =DM_DIG_MIN_NIC;

	 if(CurrentIGI > DM_DIG_MAX_NIC)
	 	CurrentIGI=DM_DIG_MAX_NIC;
	 else if(CurrentIGI < RSSI_Lower)
		CurrentIGI =RSSI_Lower;

	ODM_Write_DIG(pDM_Odm, CurrentIGI);//ODM_Write_DIG(pDM_Odm, pDM_DigTable->CurIGValue);

}

VOID
odm_DIGInit(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pDIG_T	pDM_DigTable = &pDM_Odm->DM_DigTable;

	pDM_DigTable->CurIGValue = (u1Byte) ODM_GetBBReg(pDM_Odm, ODM_REG(IGI_A,pDM_Odm), ODM_BIT(IGI,pDM_Odm));
	pDM_DigTable->RssiLowThresh 	= DM_DIG_THRESH_LOW;
	pDM_DigTable->RssiHighThresh 	= DM_DIG_THRESH_HIGH;
	pDM_DigTable->FALowThresh	= DM_FALSEALARM_THRESH_LOW;
	pDM_DigTable->FAHighThresh	= DM_FALSEALARM_THRESH_HIGH;
	if(pDM_Odm->BoardType & (ODM_BOARD_EXT_PA|ODM_BOARD_EXT_LNA))
	{
		pDM_DigTable->rx_gain_range_max = DM_DIG_MAX_NIC;
		pDM_DigTable->rx_gain_range_min = DM_DIG_MIN_NIC;
	}
	else
	{
		pDM_DigTable->rx_gain_range_max = DM_DIG_MAX_NIC;
		pDM_DigTable->rx_gain_range_min = DM_DIG_MIN_NIC;
	}
	pDM_DigTable->BackoffVal = DM_DIG_BACKOFF_DEFAULT;
	pDM_DigTable->BackoffVal_range_max = DM_DIG_BACKOFF_MAX;
	pDM_DigTable->BackoffVal_range_min = DM_DIG_BACKOFF_MIN;
	pDM_DigTable->PreCCK_CCAThres = 0xFF;
	pDM_DigTable->CurCCK_CCAThres = 0x83;
	pDM_DigTable->ForbiddenIGI = DM_DIG_MIN_NIC;
	pDM_DigTable->LargeFAHit = 0;
	pDM_DigTable->Recover_cnt = 0;
	pDM_DigTable->DIG_Dynamic_MIN_0 = DM_DIG_MIN_NIC;
	pDM_DigTable->DIG_Dynamic_MIN_1 = DM_DIG_MIN_NIC;
	pDM_DigTable->bMediaConnect_0 = FALSE;
	pDM_DigTable->bMediaConnect_1 = FALSE;
	
	pDM_Odm->bDMInitialGainEnable = TRUE;

	pDM_DigTable->BT30_CurIGI=0x32;

}

VOID
odm_DigForBtHsMode(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	pDIG_T					pDM_DigTable=&pDM_Odm->DM_DigTable;
	u1Byte					digForBtHs=0;
	u1Byte					digUpBound=0x5a;
	
	if(pDM_Odm->bBtConnectProcess)
	{
		if(pDM_Odm->SupportICType&(ODM_RTL8723A))
			digForBtHs = 0x28;
		else
			digForBtHs = 0x22;
	}
	else
	{
		digForBtHs = pDM_Odm->btHsRssi+4;
		
		if(pDM_Odm->SupportICType&(ODM_RTL8723A))
			digUpBound = 0x3e;
		
		if(digForBtHs > digUpBound)
			digForBtHs = digUpBound;
		if(digForBtHs < 0x1c)
			digForBtHs = 0x1c;

		pDM_DigTable->BT30_CurIGI = digForBtHs;
	}
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DigForBtHsMode() : set DigValue=0x%x\n", digForBtHs));
#endif
}

VOID 
odm_DIG(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pDIG_T						pDM_DigTable = &pDM_Odm->DM_DigTable;
	PFALSE_ALARM_STATISTICS		pFalseAlmCnt = &pDM_Odm->FalseAlmCnt;
	pRXHP_T						pRX_HP_Table  = &pDM_Odm->DM_RXHP_Table;
	u1Byte						DIG_Dynamic_MIN;
	u1Byte						DIG_MaxOfMin;
	BOOLEAN						FirstConnect, FirstDisConnect;
	u1Byte						dm_dig_max, dm_dig_min, offset;
	u1Byte						CurrentIGI = pDM_DigTable->CurIGValue;
	u1Byte						Adap_IGI_Upper = pDM_Odm->IGI_target + 30 + (u1Byte) pDM_Odm->TH_L2H_ini -(u1Byte) pDM_Odm->TH_EDCCA_HL_diff;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER		pAdapter	= pDM_Odm->Adapter;
#if OS_WIN_FROM_WIN7(OS_VERSION)
	if(IsAPModeExist( pAdapter) && pAdapter->bInHctTest)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG() Return: Is AP mode or In HCT Test \n"));
		return;
	}
#endif
/*
	if (pDM_Odm->SupportICType==ODM_RTL8723B)
		return;
*/

	if(pDM_Odm->bBtHsOperation)
	{
		odm_DigForBtHsMode(pDM_Odm);
	}
	
	if(!(pDM_Odm->SupportICType &(ODM_RTL8723A|ODM_RTL8188E)))
	{
		if(pRX_HP_Table->RXHP_flag == 1)
		{
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG() Return: In RXHP Operation \n"));
			return;	
		}
	}	
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
#ifdef CONFIG_SPECIAL_SETTING_FOR_FUNAI_TV	
	if((pDM_Odm->bLinked) && (pDM_Odm->Adapter->registrypriv.force_igi !=0))
	{	
		printk("pDM_Odm->RSSI_Min=%d \n",pDM_Odm->RSSI_Min);
		ODM_Write_DIG(pDM_Odm,pDM_Odm->Adapter->registrypriv.force_igi);
		return;
	}
#endif
#endif
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
	prtl8192cd_priv	priv			= pDM_Odm->priv;	
	if (!((priv->up_time > 5) && (priv->up_time % 2)) )
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG() Return: Not In DIG Operation Period \n"));
		return;
	}
#endif

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG()==>\n"));
	if((!(pDM_Odm->SupportAbility&ODM_BB_DIG)) ||(!(pDM_Odm->SupportAbility&ODM_BB_FA_CNT)))
	{
#if 0	
		if(pDM_Odm->SupportPlatform & (ODM_AP|ODM_ADSL))
		{
			if ((pDM_Odm->SupportICType == ODM_RTL8192C) && (pDM_Odm->ExtLNA == 1))
				CurrentIGI = 0x30;
			else
				CurrentIGI = 0x20;
			ODM_Write_DIG(pDM_Odm, CurrentIGI);//ODM_Write_DIG(pDM_Odm, pDM_DigTable->CurIGValue);
		}
#endif		
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG() Return: SupportAbility ODM_BB_DIG or ODM_BB_FA_CNT is disabled\n"));
		return;
	}
		
	if(*(pDM_Odm->pbScanInProcess))
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG() Return: In Scan Progress \n"));
	    	return;
	}

	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
	        if(pDM_Odm->bDMInitialGainEnable == FALSE)
	        {
		        ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG() Return: PSD is Processing \n"));
		        return;
	        }
	}
		
	if(pDM_Odm->SupportICType == ODM_RTL8192D)
	{
		if(*(pDM_Odm->pMacPhyMode) == ODM_DMSP)
		{
			if(*(pDM_Odm->pbMasterOfDMSP))
			{
				DIG_Dynamic_MIN = pDM_DigTable->DIG_Dynamic_MIN_0;
				FirstConnect = (pDM_Odm->bLinked) && (pDM_DigTable->bMediaConnect_0 == FALSE);	
				FirstDisConnect = (!pDM_Odm->bLinked) && (pDM_DigTable->bMediaConnect_0 == TRUE);
			}
			else
			{
				DIG_Dynamic_MIN = pDM_DigTable->DIG_Dynamic_MIN_1;
				FirstConnect = (pDM_Odm->bLinked) && (pDM_DigTable->bMediaConnect_1 == FALSE);	
				FirstDisConnect = (!pDM_Odm->bLinked) && (pDM_DigTable->bMediaConnect_1 == TRUE);
			}
		}
		else
		{
			if(*(pDM_Odm->pBandType) == ODM_BAND_5G)
			{
				DIG_Dynamic_MIN = pDM_DigTable->DIG_Dynamic_MIN_0;
				FirstConnect = (pDM_Odm->bLinked) && (pDM_DigTable->bMediaConnect_0 == FALSE);
				FirstDisConnect = (!pDM_Odm->bLinked) && (pDM_DigTable->bMediaConnect_0 == TRUE);
			}
			else
			{
				DIG_Dynamic_MIN = pDM_DigTable->DIG_Dynamic_MIN_1;
				FirstConnect = (pDM_Odm->bLinked) && (pDM_DigTable->bMediaConnect_1 == FALSE);
				FirstDisConnect = (!pDM_Odm->bLinked) && (pDM_DigTable->bMediaConnect_1 == TRUE);
			}
		}
	}
	else
	{	
		DIG_Dynamic_MIN = pDM_DigTable->DIG_Dynamic_MIN_0;
		FirstConnect = (pDM_Odm->bLinked) && (pDM_DigTable->bMediaConnect_0 == FALSE);
		FirstDisConnect = (!pDM_Odm->bLinked) && (pDM_DigTable->bMediaConnect_0 == TRUE);
	}
	
	if(pDM_Odm->SupportICType & (ODM_RTL8192C) &&(pDM_Odm->BoardType & (ODM_BOARD_EXT_LNA | ODM_BOARD_EXT_PA)))
	{
		if(pDM_Odm->SupportPlatform & (ODM_AP|ODM_ADSL))
		{

			dm_dig_max = DM_DIG_MAX_AP_HP;
			dm_dig_min = DM_DIG_MIN_AP_HP;
		}
		else
		{
			dm_dig_max = DM_DIG_MAX_NIC_HP;
			dm_dig_min = DM_DIG_MIN_NIC_HP;
		}
		DIG_MaxOfMin = DM_DIG_MAX_AP_HP;
	}
	else
	{
		if(pDM_Odm->SupportPlatform & (ODM_AP|ODM_ADSL))
		{
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
#ifdef DFS
			if (!priv->pmib->dot11DFSEntry.disable_DFS &&
				(OPMODE & WIFI_AP_STATE) &&
				(((pDM_Odm->ControlChannel >= 52) &&
				(pDM_Odm->ControlChannel <= 64)) ||
				((pDM_Odm->ControlChannel >= 100) &&
				(pDM_Odm->ControlChannel <= 140))))
				dm_dig_max = 0x24;
			else
#endif
			if (priv->pmib->dot11RFEntry.tx2path) {
				if (*(pDM_Odm->pWirelessMode) == ODM_WM_B)//(priv->pmib->dot11BssType.net_work_type == WIRELESS_11B)
					dm_dig_max = 0x2A;
				else
					dm_dig_max = 0x32;
			}
			else
#endif				
			dm_dig_max = DM_DIG_MAX_AP;
			dm_dig_min = DM_DIG_MIN_AP;
			DIG_MaxOfMin = dm_dig_max;
		}
		else
		{
			if((pDM_Odm->SupportICType >= ODM_RTL8188E) && (pDM_Odm->SupportPlatform & (ODM_WIN|ODM_CE)))
				dm_dig_max = 0x5A;
			else
				dm_dig_max = DM_DIG_MAX_NIC;
			
			if(pDM_Odm->SupportICType != ODM_RTL8821)
				dm_dig_min = DM_DIG_MIN_NIC;
			else
				dm_dig_min = 0x1C;

			DIG_MaxOfMin = DM_DIG_MAX_AP;
		}
	}

	if(0 < *pDM_Odm->pu1ForcedIgiLb)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): force IGI lb to: %u\n", *pDM_Odm->pu1ForcedIgiLb));
		dm_dig_min = *pDM_Odm->pu1ForcedIgiLb;
		dm_dig_max = (dm_dig_min <= dm_dig_max) ? (dm_dig_max) : (dm_dig_min + 1);
	}
		
	if(pDM_Odm->bLinked)
	{
		if(pDM_Odm->SupportICType&(ODM_RTL8723A/*|ODM_RTL8821*/))
		{
			if(( pDM_Odm->RSSI_Min + 10) > DM_DIG_MAX_NIC )
				pDM_DigTable->rx_gain_range_max = DM_DIG_MAX_NIC;
			else if(( pDM_Odm->RSSI_Min + 10) < DM_DIG_MIN_NIC )
				pDM_DigTable->rx_gain_range_max = DM_DIG_MIN_NIC;
			else
				pDM_DigTable->rx_gain_range_max = pDM_Odm->RSSI_Min + 10;


			if(pDM_Odm->bBtLimitedDig)
			{
				if(pDM_Odm->RSSI_Min>10)
				{
					if((pDM_Odm->RSSI_Min - 10) > DM_DIG_MAX_NIC)
						DIG_Dynamic_MIN = DM_DIG_MAX_NIC;
					else if((pDM_Odm->RSSI_Min - 10) < DM_DIG_MIN_NIC)
						DIG_Dynamic_MIN = DM_DIG_MIN_NIC;
					else
						DIG_Dynamic_MIN = pDM_Odm->RSSI_Min - 10;
				}
				else
					DIG_Dynamic_MIN=DM_DIG_MIN_NIC;
			}
			else
			{
				if((pDM_Odm->RSSI_Min + 20) > dm_dig_max )
					pDM_DigTable->rx_gain_range_max = dm_dig_max;
				else if((pDM_Odm->RSSI_Min + 20) < dm_dig_min )
					pDM_DigTable->rx_gain_range_max = dm_dig_min;
				else
					pDM_DigTable->rx_gain_range_max = pDM_Odm->RSSI_Min + 20;
				
			}
		}
		else
		{
			if((pDM_Odm->SupportICType & (ODM_RTL8192E|ODM_RTL8723B|ODM_RTL8812|ODM_RTL8821)) && (pDM_Odm->bBtLimitedDig==1)){				
				if((pDM_Odm->RSSI_Min + 10) > dm_dig_max )
					pDM_DigTable->rx_gain_range_max = dm_dig_max;
				else if((pDM_Odm->RSSI_Min + 10) < dm_dig_min )
					pDM_DigTable->rx_gain_range_max = dm_dig_min;
				else
					pDM_DigTable->rx_gain_range_max = pDM_Odm->RSSI_Min + 10;
			}
			else{
		
			if(pDM_Odm->SupportICType != ODM_RTL8821 && pDM_Odm->SupportICType != ODM_RTL8192E)
				offset = 20;
			else
				offset = 10;
			
			if((pDM_Odm->RSSI_Min + offset) > dm_dig_max )
				pDM_DigTable->rx_gain_range_max = dm_dig_max;
				else if((pDM_Odm->RSSI_Min + offset) < dm_dig_min )
					pDM_DigTable->rx_gain_range_max = dm_dig_min;
			else
				pDM_DigTable->rx_gain_range_max = pDM_Odm->RSSI_Min + offset;
			
			}

		/*
			if((pFalseAlmCnt->Cnt_all > 500)&&(DIG_Dynamic_MIN < 0x25))
				DIG_Dynamic_MIN++;
			else if(((pFalseAlmCnt->Cnt_all < 500)||(pDM_Odm->RSSI_Min < 8))&&(DIG_Dynamic_MIN > dm_dig_min))
				DIG_Dynamic_MIN--;
		*/
			if(pDM_Odm->bOneEntryOnly)
			{	
				if(pDM_Odm->SupportICType != ODM_RTL8723B)
					offset = 0;
				else
					offset = 12;
				
				if(pDM_Odm->RSSI_Min - offset < dm_dig_min)
					DIG_Dynamic_MIN = dm_dig_min;
				else if (pDM_Odm->RSSI_Min - offset > DIG_MaxOfMin)
					DIG_Dynamic_MIN = DIG_MaxOfMin;
				else
					DIG_Dynamic_MIN = pDM_Odm->RSSI_Min - offset;

				ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG() : bOneEntryOnly=TRUE,  DIG_Dynamic_MIN=0x%x\n",DIG_Dynamic_MIN));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG() : pDM_Odm->RSSI_Min=%d",pDM_Odm->RSSI_Min));
			}
#if (defined(CONFIG_HW_ANTENNA_DIVERSITY))
			else if( (pDM_Odm->SupportICType & ODM_ANTDIV_SUPPORT) &&(pDM_Odm->SupportAbility & ODM_BB_ANT_DIV) )
			{
				if((pDM_Odm->AntDivType == CG_TRX_HW_ANTDIV)||(pDM_Odm->AntDivType == CGCS_RX_HW_ANTDIV) ||pDM_Odm->AntDivType == S0S1_SW_ANTDIV)
				{
					DIG_Dynamic_MIN = (u1Byte) pDM_DigTable->AntDiv_RSSI_max;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("odm_DIG(): pDM_DigTable->AntDiv_RSSI_max=%d \n",pDM_DigTable->AntDiv_RSSI_max));
				}
			}
#endif
			else
			{
				DIG_Dynamic_MIN=dm_dig_min;
			}
		}
	}
	else
	{
		pDM_DigTable->rx_gain_range_max = dm_dig_max;
		DIG_Dynamic_MIN = dm_dig_min;
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG() : No Link\n"));
	}
	
	if(pFalseAlmCnt->Cnt_all > 10000)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("dm_DIG(): Abnornally false alarm case. \n"));

		if(pDM_DigTable->LargeFAHit != 3)
		        pDM_DigTable->LargeFAHit++;
		if(pDM_DigTable->ForbiddenIGI < CurrentIGI)//if(pDM_DigTable->ForbiddenIGI < pDM_DigTable->CurIGValue)
		{
			pDM_DigTable->ForbiddenIGI = (u1Byte)CurrentIGI;//pDM_DigTable->ForbiddenIGI = pDM_DigTable->CurIGValue;
			pDM_DigTable->LargeFAHit = 1;
		}

		if(pDM_DigTable->LargeFAHit >= 3)
		{
			if((pDM_DigTable->ForbiddenIGI+1) >pDM_DigTable->rx_gain_range_max)
				pDM_DigTable->rx_gain_range_min = pDM_DigTable->rx_gain_range_max;
			else
				pDM_DigTable->rx_gain_range_min = (pDM_DigTable->ForbiddenIGI + 1);
			pDM_DigTable->Recover_cnt = 3600;
		}

	}
	else
	{
		if(pDM_DigTable->Recover_cnt != 0)
			pDM_DigTable->Recover_cnt --;
		else
		{
			if(pDM_DigTable->LargeFAHit < 3)
			{
				if((pDM_DigTable->ForbiddenIGI -1) < DIG_Dynamic_MIN)
				{
					pDM_DigTable->ForbiddenIGI = DIG_Dynamic_MIN;
					pDM_DigTable->rx_gain_range_min = DIG_Dynamic_MIN;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): Normal Case: At Lower Bound\n"));
				}
				else
				{
					pDM_DigTable->ForbiddenIGI --;
					pDM_DigTable->rx_gain_range_min = (pDM_DigTable->ForbiddenIGI + 1);
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): Normal Case: Approach Lower Bound\n"));
				}
			}
			else
			{
				pDM_DigTable->LargeFAHit = 0;
			}
		}
	}
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): pDM_DigTable->LargeFAHit=%d\n",pDM_DigTable->LargeFAHit));

	#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	if(IS_STA_VALID(pDM_Odm->pODM_StaInfo[0]))
		pDM_Odm->bsta_state = _TRUE;
	#endif	
	
	if((pDM_Odm->SupportPlatform&(ODM_WIN|ODM_CE))&&(pDM_Odm->PhyDbgInfo.NumQryBeaconPkt < 2) && (pDM_Odm->bsta_state) )
	{		
			pDM_DigTable->rx_gain_range_min = dm_dig_min;
	}
	
	if(pDM_DigTable->rx_gain_range_min > pDM_DigTable->rx_gain_range_max)
		pDM_DigTable->rx_gain_range_min = pDM_DigTable->rx_gain_range_max;

	if(pDM_Odm->bLinked)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): DIG AfterLink\n"));
		if(FirstConnect)
		{
			if(pDM_Odm->RSSI_Min <= DIG_MaxOfMin)
				CurrentIGI = pDM_Odm->RSSI_Min;
			else
				CurrentIGI = DIG_MaxOfMin;
			ODM_RT_TRACE(pDM_Odm,	ODM_COMP_DIG, ODM_DBG_LOUD, ("DIG: First Connect\n"));

			ODM_ConfigBBWithHeaderFile(pDM_Odm, CONFIG_BB_AGC_TAB_DIFF);		
		}
		else
		{
			if(pDM_Odm->SupportICType == ODM_RTL8192D)
			{
				if(pFalseAlmCnt->Cnt_all > DM_DIG_FA_TH2_92D)
					CurrentIGI = CurrentIGI + 4;//pDM_DigTable->CurIGValue = pDM_DigTable->PreIGValue+2;
				else if (pFalseAlmCnt->Cnt_all > DM_DIG_FA_TH1_92D)
					CurrentIGI = CurrentIGI + 2;
				else if(pFalseAlmCnt->Cnt_all < DM_DIG_FA_TH0_92D)
					CurrentIGI = CurrentIGI - 2;//pDM_DigTable->CurIGValue =pDM_DigTable->PreIGValue-1;	
			}
			else
			{
				if(pDM_Odm->SupportICType == ODM_RTL8723A)
				{
					if(pDM_Odm->bBtLimitedDig)
					{
						if(pFalseAlmCnt->Cnt_all > 0x300)
							CurrentIGI = CurrentIGI + 4;
						else if (pFalseAlmCnt->Cnt_all > 0x250)
							CurrentIGI = CurrentIGI + 2;
						else if(pFalseAlmCnt->Cnt_all < DM_DIG_FA_TH0)
							CurrentIGI = CurrentIGI -2;
					}
					else
					{
						if(pFalseAlmCnt->Cnt_all > DM_DIG_FA_TH2)
							CurrentIGI = CurrentIGI + 4;//pDM_DigTable->CurIGValue = pDM_DigTable->PreIGValue+2;
						else if (pFalseAlmCnt->Cnt_all > DM_DIG_FA_TH1)
							CurrentIGI = CurrentIGI + 2;//pDM_DigTable->CurIGValue = pDM_DigTable->PreIGValue+1;
						else if(pFalseAlmCnt->Cnt_all < DM_DIG_FA_TH0)
							CurrentIGI = CurrentIGI - 2;//pDM_DigTable->CurIGValue =pDM_DigTable->PreIGValue-1;	
					}
				}
				else
				{
					if(pFalseAlmCnt->Cnt_all > DM_DIG_FA_TH2)
						CurrentIGI = CurrentIGI + 4;//pDM_DigTable->CurIGValue = pDM_DigTable->PreIGValue+2;
					else if (pFalseAlmCnt->Cnt_all > DM_DIG_FA_TH1)
						CurrentIGI = CurrentIGI + 2;//pDM_DigTable->CurIGValue = pDM_DigTable->PreIGValue+1;
					else if(pFalseAlmCnt->Cnt_all < DM_DIG_FA_TH0)
						CurrentIGI = CurrentIGI - 2;//pDM_DigTable->CurIGValue =pDM_DigTable->PreIGValue-1;	

					#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
					if(IS_STA_VALID(pDM_Odm->pODM_StaInfo[0]))
						pDM_Odm->bsta_state = _TRUE;
					#endif

					if((pDM_Odm->SupportPlatform&(ODM_WIN|ODM_CE))&&(pDM_Odm->PhyDbgInfo.NumQryBeaconPkt < 2)
						&&(pFalseAlmCnt->Cnt_all < DM_DIG_FA_TH1) && (pDM_Odm->bsta_state))
					{						
						CurrentIGI = pDM_DigTable->rx_gain_range_min;
						ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): Beacon is less than 10 and FA is less than 768, IGI GOES TO 0x1E!!!!!!!!!!!!\n"));
					}
					/*{
						u2Byte value16;
						value16 = (u2Byte) ODM_GetBBReg(pDM_Odm, 0x664, bMaskLWord);
						ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): NumQryBeaconPkt = %d, OFDM_OK_Cnt = %d\n", 
							pDM_Odm->PhyDbgInfo.NumQryBeaconPkt, value16));
					}*/
				}
			}
		}
	}	
	else
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): DIG BeforeLink\n"));
		if(FirstDisConnect)
		{
				CurrentIGI = pDM_DigTable->rx_gain_range_min;
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): First DisConnect \n"));
		}
		else
		{
	             if(pFalseAlmCnt->Cnt_all > 10000)
				CurrentIGI = CurrentIGI + 4;
			else if (pFalseAlmCnt->Cnt_all > 8000)
				CurrentIGI = CurrentIGI + 2;
			else if(pFalseAlmCnt->Cnt_all < 500)
				CurrentIGI = CurrentIGI - 2;
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): England DIG \n"));
		}
	}
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): DIG End Adjust IGI\n"));

	if(CurrentIGI > pDM_DigTable->rx_gain_range_max)
		CurrentIGI = pDM_DigTable->rx_gain_range_max;
	if(CurrentIGI < pDM_DigTable->rx_gain_range_min)
		CurrentIGI = pDM_DigTable->rx_gain_range_min;

	if(pDM_Odm->SupportAbility & ODM_BB_ADAPTIVITY)
	{
		if(CurrentIGI > Adap_IGI_Upper)
			CurrentIGI = Adap_IGI_Upper;

		if(pDM_Odm->IGI_LowerBound != 0)
		{
			if(CurrentIGI < pDM_Odm->IGI_LowerBound)
				CurrentIGI = pDM_Odm->IGI_LowerBound;
		}
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): pDM_Odm->IGI_LowerBound = %d\n", pDM_Odm->IGI_LowerBound));
	}
	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): rx_gain_range_max=0x%x, rx_gain_range_min=0x%x\n", 
		pDM_DigTable->rx_gain_range_max, pDM_DigTable->rx_gain_range_min));
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): TotalFA=%d\n", pFalseAlmCnt->Cnt_all));
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): CurIGValue=0x%x\n", CurrentIGI));

#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)	
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pDM_Odm->Adapter);
	u8Byte			curTxOkCnt=0, curRxOkCnt=0;
	static u8Byte		lastTxOkCnt=0, lastRxOkCnt=0;

	
	curTxOkCnt = pAdapter->TxStats.NumTxBytesUnicast - lastTxOkCnt;
	curRxOkCnt =pAdapter->RxStats.NumRxBytesUnicast - lastRxOkCnt;
	lastTxOkCnt = pAdapter->TxStats.NumTxBytesUnicast;
	lastRxOkCnt = pAdapter->RxStats.NumRxBytesUnicast;
	if((pDM_Odm->SupportICType == ODM_RTL8723A)&& (pHalData->UndecoratedSmoothedPWDB > DM_DIG_HIGH_PWR_THRESHOLD))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): UndecoratedSmoothedPWDB(%#x)\n", pHalData->UndecoratedSmoothedPWDB));
		if(CurrentIGI < DM_DIG_HIGH_PWR_IGI_LOWER_BOUND)
		{
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): CurIGValue(%#x)\n", pDM_DigTable->CurIGValue));
			CurrentIGI=DM_DIG_HIGH_PWR_IGI_LOWER_BOUND;
		}
	}
	if((pDM_Odm->SupportICType & ODM_RTL8723A) && 
			IS_WIRELESS_MODE_G(pAdapter))
		{
			if(pHalData->UndecoratedSmoothedPWDB > 0x28)
			{
				if(CurrentIGI < DM_DIG_Gmode_HIGH_PWR_IGI_LOWER_BOUND)
				{
					CurrentIGI = DM_DIG_Gmode_HIGH_PWR_IGI_LOWER_BOUND;
				}	
			} 
		}	
#if 0
	if((pDM_Odm->SupportICType & ODM_RTL8723A)&&(pMgntInfo->CustomerID = RT_CID_LENOVO_CHINA))
	{
		OKCntAll = (curTxOkCnt+curRxOkCnt);
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): CurIGValue(%#x)\n", CurrentIGI));
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): UndecoratedSmoothedPWDB(%#x)\n", pHalData->UndecoratedSmoothedPWDB));
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG(): OKCntAll(%#x)\n", OKCntAll));
		if(pDM_Odm->SupportInterface==ODM_ITRF_USB)
		{
			if(pHalData->UndecoratedSmoothedPWDB < 12)
			{
				if(CurrentIGI > DM_DIG_MIN_NIC)
				{
					if(OKCntAll >= 1500000) 		
						CurrentIGI=0x1B;
					else if(OKCntAll >= 1000000) 	
						CurrentIGI=0x1A;
					else if(OKCntAll >= 500000)		
						CurrentIGI=0x19;
					else if(OKCntAll >= 250000)	
						CurrentIGI=0x18;
					else
					{
						CurrentIGI=0x17;	
					}
				}
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, ODM_DBG_LOUD, ("Modify---->CurIGValue(%#x)\n", CurrentIGI));	
			}
		}
	}	
#endif	
}
#endif
		
#if (RTL8192D_SUPPORT==1) 
	if(pDM_Odm->SupportICType == ODM_RTL8192D)
	{
		if(*(pDM_Odm->pMacPhyMode) == ODM_DMSP)
		{
			ODM_Write_DIG_DMSP(pDM_Odm, (u1Byte)CurrentIGI);//ODM_Write_DIG_DMSP(pDM_Odm, pDM_DigTable->CurIGValue);
			if(*(pDM_Odm->pbMasterOfDMSP))
			{
				pDM_DigTable->bMediaConnect_0 = pDM_Odm->bLinked;
				pDM_DigTable->DIG_Dynamic_MIN_0 = DIG_Dynamic_MIN;
			}
			else
			{
				pDM_DigTable->bMediaConnect_1 = pDM_Odm->bLinked;
				pDM_DigTable->DIG_Dynamic_MIN_1 = DIG_Dynamic_MIN;
			}
		}
		else
		{
			ODM_Write_DIG(pDM_Odm, CurrentIGI);//ODM_Write_DIG(pDM_Odm, pDM_DigTable->CurIGValue);
			if(*(pDM_Odm->pBandType) == ODM_BAND_5G)
			{
				pDM_DigTable->bMediaConnect_0 = pDM_Odm->bLinked;
				pDM_DigTable->DIG_Dynamic_MIN_0 = DIG_Dynamic_MIN;
			}
			else
			{
				pDM_DigTable->bMediaConnect_1 = pDM_Odm->bLinked;
				pDM_DigTable->DIG_Dynamic_MIN_1 = DIG_Dynamic_MIN;
			}
		}
	}
	else
#endif
	{
		if(pDM_Odm->bBtHsOperation)
		{
			if(pDM_Odm->bLinked)
			{
				if(pDM_DigTable->BT30_CurIGI > (CurrentIGI))
				{
					ODM_Write_DIG(pDM_Odm, CurrentIGI);
					
				}	
				else
				{
					ODM_Write_DIG(pDM_Odm, pDM_DigTable->BT30_CurIGI);
				}
				pDM_DigTable->bMediaConnect_0 = pDM_Odm->bLinked;
				pDM_DigTable->DIG_Dynamic_MIN_0 = DIG_Dynamic_MIN;
			}
			else
			{
				if(pDM_Odm->bLinkInProcess)
				{
					ODM_Write_DIG(pDM_Odm, 0x1c);
				}
				else if(pDM_Odm->bBtConnectProcess)
				{
					ODM_Write_DIG(pDM_Odm, 0x28);
				}
				else
				{
					ODM_Write_DIG(pDM_Odm, pDM_DigTable->BT30_CurIGI);//ODM_Write_DIG(pDM_Odm, pDM_DigTable->CurIGValue);	
				}
			}
		}	
		else	
		{
			ODM_Write_DIG(pDM_Odm, CurrentIGI);//ODM_Write_DIG(pDM_Odm, pDM_DigTable->CurIGValue);
			pDM_DigTable->bMediaConnect_0 = pDM_Odm->bLinked;
			pDM_DigTable->DIG_Dynamic_MIN_0 = DIG_Dynamic_MIN;
		}
	}
}


BOOLEAN 
odm_DigAbort(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER		pAdapter	= pDM_Odm->Adapter;
	pRXHP_T			pRX_HP_Table  = &pDM_Odm->DM_RXHP_Table;
	
#if OS_WIN_FROM_WIN7(OS_VERSION)
	if(IsAPModeExist( pAdapter) && pAdapter->bInHctTest)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG() Return: Is AP mode or In HCT Test \n"));
	    	return	TRUE;
	}
#endif

	if(pRX_HP_Table->RXHP_flag == 1)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_DIG() Return: In RXHP Operation \n"));
		return	TRUE;	
	}

	return	FALSE;
#else
	return	FALSE;
#endif
	

}


VOID 
odm_FalseAlarmCounterStatistics(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	u4Byte ret_value;
	PFALSE_ALARM_STATISTICS FalseAlmCnt = &(pDM_Odm->FalseAlmCnt);

#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	prtl8192cd_priv priv		= pDM_Odm->priv;
	if( (priv->auto_channel != 0) && (priv->auto_channel != 2) )
		return;
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	if((pDM_Odm->SupportICType == ODM_RTL8192D) &&
		(*(pDM_Odm->pMacPhyMode)==ODM_DMSP)&&   
		(!(*(pDM_Odm->pbMasterOfDMSP))))
	{
		odm_FalseAlarmCounterStatistics_ForSlaveOfDMSP(pDM_Odm);
		return;
	}
#endif		

	if(!(pDM_Odm->SupportAbility & ODM_BB_FA_CNT))
		return;

	if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
	{

		ODM_SetBBReg(pDM_Odm, ODM_REG_OFDM_FA_HOLDC_11N, BIT31, 1);
		ODM_SetBBReg(pDM_Odm, ODM_REG_OFDM_FA_RSTD_11N, BIT31, 1);
	
		ret_value = ODM_GetBBReg(pDM_Odm, ODM_REG_OFDM_FA_TYPE1_11N, bMaskDWord);
		FalseAlmCnt->Cnt_Fast_Fsync = (ret_value&0xffff);
		FalseAlmCnt->Cnt_SB_Search_fail = ((ret_value&0xffff0000)>>16);		
		ret_value = ODM_GetBBReg(pDM_Odm, ODM_REG_OFDM_FA_TYPE2_11N, bMaskDWord);
		FalseAlmCnt->Cnt_OFDM_CCA = (ret_value&0xffff); 
		FalseAlmCnt->Cnt_Parity_Fail = ((ret_value&0xffff0000)>>16);	
		ret_value = ODM_GetBBReg(pDM_Odm, ODM_REG_OFDM_FA_TYPE3_11N, bMaskDWord);
		FalseAlmCnt->Cnt_Rate_Illegal = (ret_value&0xffff);
		FalseAlmCnt->Cnt_Crc8_fail = ((ret_value&0xffff0000)>>16);
		ret_value = ODM_GetBBReg(pDM_Odm, ODM_REG_OFDM_FA_TYPE4_11N, bMaskDWord);
		FalseAlmCnt->Cnt_Mcs_fail = (ret_value&0xffff);

		FalseAlmCnt->Cnt_Ofdm_fail = 	FalseAlmCnt->Cnt_Parity_Fail + FalseAlmCnt->Cnt_Rate_Illegal +
									FalseAlmCnt->Cnt_Crc8_fail + FalseAlmCnt->Cnt_Mcs_fail +
									FalseAlmCnt->Cnt_Fast_Fsync + FalseAlmCnt->Cnt_SB_Search_fail;

#if (RTL8188E_SUPPORT==1)
		if((pDM_Odm->SupportICType == ODM_RTL8188E)||(pDM_Odm->SupportICType == ODM_RTL8192E))
		{
				ret_value = ODM_GetBBReg(pDM_Odm, ODM_REG_SC_CNT_11N, bMaskDWord);
			FalseAlmCnt->Cnt_BW_LSC = (ret_value&0xffff);
			FalseAlmCnt->Cnt_BW_USC = ((ret_value&0xffff0000)>>16);
		}
#endif

#if (RTL8192D_SUPPORT==1) 
		if(pDM_Odm->SupportICType == ODM_RTL8192D)
		{
			odm_GetCCKFalseAlarm_92D(pDM_Odm);
		}
		else
#endif
		{
				ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_FA_RST_11N, BIT12, 1); 
				ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_FA_RST_11N, BIT14, 1); 
		
				ret_value = ODM_GetBBReg(pDM_Odm, ODM_REG_CCK_FA_LSB_11N, bMaskByte0);
			FalseAlmCnt->Cnt_Cck_fail = ret_value;
				ret_value = ODM_GetBBReg(pDM_Odm, ODM_REG_CCK_FA_MSB_11N, bMaskByte3);
			FalseAlmCnt->Cnt_Cck_fail +=  (ret_value& 0xff)<<8;

				ret_value = ODM_GetBBReg(pDM_Odm, ODM_REG_CCK_CCA_CNT_11N, bMaskDWord);
			FalseAlmCnt->Cnt_CCK_CCA = ((ret_value&0xFF)<<8) |((ret_value&0xFF00)>>8);
		}
		
		FalseAlmCnt->Cnt_all = (	FalseAlmCnt->Cnt_Fast_Fsync + 
							FalseAlmCnt->Cnt_SB_Search_fail +
							FalseAlmCnt->Cnt_Parity_Fail +
							FalseAlmCnt->Cnt_Rate_Illegal +
							FalseAlmCnt->Cnt_Crc8_fail +
							FalseAlmCnt->Cnt_Mcs_fail +
							FalseAlmCnt->Cnt_Cck_fail);	

		FalseAlmCnt->Cnt_CCA_all = FalseAlmCnt->Cnt_OFDM_CCA + FalseAlmCnt->Cnt_CCK_CCA;

#if (RTL8192C_SUPPORT==1)
		if(pDM_Odm->SupportICType == ODM_RTL8192C)
			odm_ResetFACounter_92C(pDM_Odm);
#endif

#if (RTL8192D_SUPPORT==1)
		if(pDM_Odm->SupportICType == ODM_RTL8192D)
			odm_ResetFACounter_92D(pDM_Odm);
#endif

		if(pDM_Odm->SupportICType >=ODM_RTL8723A)
		{
				ODM_SetBBReg(pDM_Odm, ODM_REG_OFDM_FA_RSTC_11N, BIT31, 1);
				ODM_SetBBReg(pDM_Odm, ODM_REG_OFDM_FA_RSTC_11N, BIT31, 0);
				ODM_SetBBReg(pDM_Odm, ODM_REG_OFDM_FA_RSTD_11N, BIT27, 1);
				ODM_SetBBReg(pDM_Odm, ODM_REG_OFDM_FA_RSTD_11N, BIT27, 0);
				ODM_SetBBReg(pDM_Odm, ODM_REG_OFDM_FA_HOLDC_11N, BIT31, 0);
				ODM_SetBBReg(pDM_Odm, ODM_REG_OFDM_FA_RSTD_11N, BIT31, 0);

				ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_FA_RST_11N, BIT13|BIT12, 0); 
				ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_FA_RST_11N, BIT13|BIT12, 2); 
				ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_FA_RST_11N, BIT15|BIT14, 0); 
				ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_FA_RST_11N, BIT15|BIT14, 2); 
		}
			
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_FA_CNT, ODM_DBG_LOUD, ("Enter odm_FalseAlarmCounterStatistics\n"));
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_FA_CNT, ODM_DBG_LOUD, ("Cnt_Fast_Fsync=%d, Cnt_SB_Search_fail=%d\n",
			FalseAlmCnt->Cnt_Fast_Fsync, FalseAlmCnt->Cnt_SB_Search_fail));
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_FA_CNT, ODM_DBG_LOUD, ("Cnt_Parity_Fail=%d, Cnt_Rate_Illegal=%d\n",
			FalseAlmCnt->Cnt_Parity_Fail, FalseAlmCnt->Cnt_Rate_Illegal));
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_FA_CNT, ODM_DBG_LOUD, ("Cnt_Crc8_fail=%d, Cnt_Mcs_fail=%d\n",
			FalseAlmCnt->Cnt_Crc8_fail, FalseAlmCnt->Cnt_Mcs_fail));
	}
	else if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
	{
		u4Byte CCKenable;
		FalseAlmCnt->Cnt_Ofdm_fail = ODM_GetBBReg(pDM_Odm, ODM_REG_OFDM_FA_11AC, bMaskLWord);
		FalseAlmCnt->Cnt_Cck_fail = ODM_GetBBReg(pDM_Odm, ODM_REG_CCK_FA_11AC, bMaskLWord);
		
		CCKenable =  ODM_GetBBReg(pDM_Odm, ODM_REG_BB_RX_PATH_11AC, BIT28);
		if(CCKenable)//if(*pDM_Odm->pBandType == ODM_BAND_2_4G)
			FalseAlmCnt->Cnt_all = FalseAlmCnt->Cnt_Ofdm_fail + FalseAlmCnt->Cnt_Cck_fail;
		else
			FalseAlmCnt->Cnt_all = FalseAlmCnt->Cnt_Ofdm_fail;

		ODM_SetBBReg(pDM_Odm, ODM_REG_OFDM_FA_RST_11AC, BIT17, 1);
		ODM_SetBBReg(pDM_Odm, ODM_REG_OFDM_FA_RST_11AC, BIT17, 0);
		ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_FA_RST_11AC, BIT15, 0);
		ODM_SetBBReg(pDM_Odm, ODM_REG_CCK_FA_RST_11AC, BIT15, 1);
	}
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_FA_CNT, ODM_DBG_LOUD, ("Cnt_Cck_fail=%d\n",	FalseAlmCnt->Cnt_Cck_fail));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_FA_CNT, ODM_DBG_LOUD, ("Cnt_Ofdm_fail=%d\n",	FalseAlmCnt->Cnt_Ofdm_fail));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_FA_CNT, ODM_DBG_LOUD, ("Total False Alarm=%d\n",	FalseAlmCnt->Cnt_all));
}


VOID 
odm_CCKPacketDetectionThresh(
	IN		PDM_ODM_T		pDM_Odm
	)
{

	u1Byte	CurCCK_CCAThres;
	PFALSE_ALARM_STATISTICS FalseAlmCnt = &(pDM_Odm->FalseAlmCnt);

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	if (pDM_Odm->bDualMacSmartConcurrent == TRUE)
		return;

	if(pDM_Odm->bBtHsOperation)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("odm_CCKPacketDetectionThresh() write 0xcd for BT HS mode!!\n"));
		ODM_Write_CCK_CCA_Thres(pDM_Odm, 0xcd);
		return;
	}

#endif

	if(!(pDM_Odm->SupportAbility & (ODM_BB_CCK_PD|ODM_BB_FA_CNT)))
		return;

	if(pDM_Odm->ExtLNA)
		return;

	if(pDM_Odm->bLinked)
	{
		if(pDM_Odm->RSSI_Min > 25)
			CurCCK_CCAThres = 0xcd;
		else if((pDM_Odm->RSSI_Min <= 25) && (pDM_Odm->RSSI_Min > 10))
			CurCCK_CCAThres = 0x83;
		else
		{
			if(FalseAlmCnt->Cnt_Cck_fail > 1000)
				CurCCK_CCAThres = 0x83;
			else
				CurCCK_CCAThres = 0x40;
		}
	}
	else
	{
		if(FalseAlmCnt->Cnt_Cck_fail > 1000)
			CurCCK_CCAThres = 0x83;
		else
			CurCCK_CCAThres = 0x40;
	}
	
#if (RTL8192D_SUPPORT==1) 
	if((pDM_Odm->SupportICType == ODM_RTL8192D)&&(*pDM_Odm->pBandType == ODM_BAND_2_4G))
		ODM_Write_CCK_CCA_Thres_92D(pDM_Odm, CurCCK_CCAThres);
	else
#endif
		ODM_Write_CCK_CCA_Thres(pDM_Odm, CurCCK_CCAThres);
}

VOID
ODM_Write_CCK_CCA_Thres(
	IN	PDM_ODM_T		pDM_Odm,
	IN	u1Byte			CurCCK_CCAThres
	)
{
	pDIG_T	pDM_DigTable = &pDM_Odm->DM_DigTable;

	if(pDM_DigTable->CurCCK_CCAThres!=CurCCK_CCAThres)	
	{
		ODM_Write1Byte(pDM_Odm, ODM_REG(CCK_CCA,pDM_Odm), CurCCK_CCAThres);
	}
	pDM_DigTable->PreCCK_CCAThres = pDM_DigTable->CurCCK_CCAThres;
	pDM_DigTable->CurCCK_CCAThres = CurCCK_CCAThres;
	
}

VOID 
odm_DynamicBBPowerSavingInit(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pPS_T	pDM_PSTable = &pDM_Odm->DM_PSTable;

	pDM_PSTable->PreCCAState = CCA_MAX;
	pDM_PSTable->CurCCAState = CCA_MAX;
	pDM_PSTable->PreRFState = RF_MAX;
	pDM_PSTable->CurRFState = RF_MAX;
	pDM_PSTable->Rssi_val_min = 0;
	pDM_PSTable->initialize = 0;
}


VOID
odm_DynamicBBPowerSaving(
	IN		PDM_ODM_T		pDM_Odm
	)
{	
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))

	if (pDM_Odm->SupportICType != ODM_RTL8723A)
		return;
	if(!(pDM_Odm->SupportAbility & ODM_BB_PWR_SAVE))
		return;
	if(!(pDM_Odm->SupportPlatform & (ODM_WIN|ODM_CE)))
		return;
	
	if((pDM_Odm->SupportICType == ODM_RTL8192C) &&(pDM_Odm->RFType == ODM_2T2R))
	{
		odm_1R_CCA(pDM_Odm);
	}
	
	else
	{
		ODM_RF_Saving(pDM_Odm, FALSE);
	}
#endif
	
}

VOID
odm_1R_CCA(
	IN 	PDM_ODM_T	pDM_Odm
	)
{
	pPS_T	pDM_PSTable = &pDM_Odm->DM_PSTable;

	if(pDM_Odm->RSSI_Min!= 0xFF)
	{
		 
		if(pDM_PSTable->PreCCAState == CCA_2R)
		{
			if(pDM_Odm->RSSI_Min >= 35)
				pDM_PSTable->CurCCAState = CCA_1R;
			else
				pDM_PSTable->CurCCAState = CCA_2R;
			
		}
		else{
			if(pDM_Odm->RSSI_Min <= 30)
				pDM_PSTable->CurCCAState = CCA_2R;
			else
				pDM_PSTable->CurCCAState = CCA_1R;
		}
	}
	else{
		pDM_PSTable->CurCCAState=CCA_MAX;
	}
	
	if(pDM_PSTable->PreCCAState != pDM_PSTable->CurCCAState)
	{
		if(pDM_PSTable->CurCCAState == CCA_1R)
		{
			if(  pDM_Odm->RFType ==ODM_2T2R )
			{
				ODM_SetBBReg(pDM_Odm, 0xc04  , bMaskByte0, 0x13);
			}
			else
			{
				ODM_SetBBReg(pDM_Odm, 0xc04  , bMaskByte0, 0x23);
			}
		}
		else
		{
			ODM_SetBBReg(pDM_Odm, 0xc04  , bMaskByte0, 0x33);
		}
		pDM_PSTable->PreCCAState = pDM_PSTable->CurCCAState;
	}
}

void
ODM_RF_Saving(
	IN 	PDM_ODM_T	pDM_Odm,
	IN	u1Byte		bForceInNormal 
	)
{
#if (DM_ODM_SUPPORT_TYPE != ODM_AP)
	pPS_T	pDM_PSTable = &pDM_Odm->DM_PSTable;
	u1Byte	Rssi_Up_bound = 30 ;
	u1Byte	Rssi_Low_bound = 25;
	#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	if(pDM_Odm->PatchID == 40 )
	{
		Rssi_Up_bound = 50 ;
		Rssi_Low_bound = 45;
	}
	#endif
	if(pDM_PSTable->initialize == 0){
		
		pDM_PSTable->Reg874 = (ODM_GetBBReg(pDM_Odm, 0x874, bMaskDWord)&0x1CC000)>>14;
		pDM_PSTable->RegC70 = (ODM_GetBBReg(pDM_Odm, 0xc70, bMaskDWord)&BIT3)>>3;
		pDM_PSTable->Reg85C = (ODM_GetBBReg(pDM_Odm, 0x85c, bMaskDWord)&0xFF000000)>>24;
		pDM_PSTable->RegA74 = (ODM_GetBBReg(pDM_Odm, 0xa74, bMaskDWord)&0xF000)>>12;
		pDM_PSTable->initialize = 1;
	}

	if(!bForceInNormal)
	{
		if(pDM_Odm->RSSI_Min != 0xFF)
		{			 
			if(pDM_PSTable->PreRFState == RF_Normal)
			{
				if(pDM_Odm->RSSI_Min >= Rssi_Up_bound)
					pDM_PSTable->CurRFState = RF_Save;
				else
					pDM_PSTable->CurRFState = RF_Normal;
			}
			else{
				if(pDM_Odm->RSSI_Min <= Rssi_Low_bound)
					pDM_PSTable->CurRFState = RF_Normal;
				else
					pDM_PSTable->CurRFState = RF_Save;
			}
		}
		else
			pDM_PSTable->CurRFState=RF_MAX;
	}
	else
	{
		pDM_PSTable->CurRFState = RF_Normal;
	}
	
	if(pDM_PSTable->PreRFState != pDM_PSTable->CurRFState)
	{
		if(pDM_PSTable->CurRFState == RF_Save)
		{
			if(pDM_Odm->SupportICType == ODM_RTL8723A)
			{
				ODM_SetBBReg(pDM_Odm, 0x874  , BIT5, 0x1);
			}
			ODM_SetBBReg(pDM_Odm, 0x874  , 0x1C0000, 0x2);
			ODM_SetBBReg(pDM_Odm, 0xc70, BIT3, 0);
			ODM_SetBBReg(pDM_Odm, 0x85c, 0xFF000000, 0x63);
			ODM_SetBBReg(pDM_Odm, 0x874, 0xC000, 0x2);
			ODM_SetBBReg(pDM_Odm, 0xa74, 0xF000, 0x3);
			ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 0x0);
			ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 0x1);
		}
		else
		{
			ODM_SetBBReg(pDM_Odm, 0x874  , 0x1CC000, pDM_PSTable->Reg874); 
			ODM_SetBBReg(pDM_Odm, 0xc70, BIT3, pDM_PSTable->RegC70); 
			ODM_SetBBReg(pDM_Odm, 0x85c, 0xFF000000, pDM_PSTable->Reg85C);
			ODM_SetBBReg(pDM_Odm, 0xa74, 0xF000, pDM_PSTable->RegA74); 
			ODM_SetBBReg(pDM_Odm,0x818, BIT28, 0x0);  

			if(pDM_Odm->SupportICType == ODM_RTL8723A)
			{
				ODM_SetBBReg(pDM_Odm,0x874  , BIT5, 0x0);
			}
		}
		pDM_PSTable->PreRFState =pDM_PSTable->CurRFState;
	}
#endif	
}



VOID
odm_RateAdaptiveMaskInit(
	IN 	PDM_ODM_T	pDM_Odm
	)
{
	PODM_RATE_ADAPTIVE	pOdmRA = &pDM_Odm->RateAdaptive;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PMGNT_INFO		pMgntInfo = &pDM_Odm->Adapter->MgntInfo;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pDM_Odm->Adapter);

	pMgntInfo->Ratr_State = DM_RATR_STA_INIT;

	if (pMgntInfo->DM_Type == DM_Type_ByDriver)
		pHalData->bUseRAMask = TRUE;
	else
		pHalData->bUseRAMask = FALSE;	

#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	pOdmRA->Type = DM_Type_ByDriver;
	if (pOdmRA->Type == DM_Type_ByDriver)
		pDM_Odm->bUseRAMask = _TRUE;
	else
		pDM_Odm->bUseRAMask = _FALSE;	
#endif

	pOdmRA->RATRState = DM_RATR_STA_INIT;
	pOdmRA->LdpcThres = 35;
	pOdmRA->bUseLdpc = FALSE;
	pOdmRA->HighRSSIThresh = 50;
	pOdmRA->LowRSSIThresh = 20;
}

#if (DM_ODM_SUPPORT_TYPE & ODM_WIN) 
VOID
ODM_RateAdaptiveStateApInit(	
	IN	PADAPTER		Adapter	,
	IN	PRT_WLAN_STA  	pEntry
	)
{
	pEntry->Ratr_State = DM_RATR_STA_INIT;
}
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
u4Byte ODM_Get_Rate_Bitmap(
	IN	PDM_ODM_T	pDM_Odm,	
	IN	u4Byte		macid,
	IN	u4Byte 		ra_mask,	
	IN	u1Byte 		rssi_level)
{
	PSTA_INFO_T   	pEntry;
	u4Byte 	rate_bitmap = 0;
	u1Byte 	WirelessMode;
	
	
	pEntry = pDM_Odm->pODM_StaInfo[macid];
	if(!IS_STA_VALID(pEntry))
		return ra_mask;

	WirelessMode = pEntry->wireless_mode;
	
	switch(WirelessMode)
	{
		case ODM_WM_B:
			if(ra_mask & 0x0000000c)	
				rate_bitmap = 0x0000000d;
			else
				rate_bitmap = 0x0000000f;
			break;
			
		case (ODM_WM_G):
		case (ODM_WM_A):
			if(rssi_level == DM_RATR_STA_HIGH)
				rate_bitmap = 0x00000f00;
			else
				rate_bitmap = 0x00000ff0;
			break;
			
		case (ODM_WM_B|ODM_WM_G):
			if(rssi_level == DM_RATR_STA_HIGH)
				rate_bitmap = 0x00000f00;
			else if(rssi_level == DM_RATR_STA_MIDDLE)
				rate_bitmap = 0x00000ff0;
			else
				rate_bitmap = 0x00000ff5;
			break;		

		case (ODM_WM_B|ODM_WM_G|ODM_WM_N24G)	:
		case (ODM_WM_B|ODM_WM_N24G)	:
		case (ODM_WM_G|ODM_WM_N24G)	:
		case (ODM_WM_A|ODM_WM_N5G)	:
			{					
				if (	pDM_Odm->RFType == ODM_1T2R ||pDM_Odm->RFType == ODM_1T1R)
				{
					if(rssi_level == DM_RATR_STA_HIGH)
					{
						rate_bitmap = 0x000f0000;
					}
					else if(rssi_level == DM_RATR_STA_MIDDLE)
					{
						rate_bitmap = 0x000ff000;
					}
					else{
						if (*(pDM_Odm->pBandWidth) == ODM_BW40M)
							rate_bitmap = 0x000ff015;
						else
							rate_bitmap = 0x000ff005;
					}				
				}
				else
				{
					if(rssi_level == DM_RATR_STA_HIGH)
					{		
						rate_bitmap = 0x0f8f0000;
					}
					else if(rssi_level == DM_RATR_STA_MIDDLE)
					{
						rate_bitmap = 0x0f8ff000;
					}
					else
					{
						if (*(pDM_Odm->pBandWidth) == ODM_BW40M)
							rate_bitmap = 0x0f8ff015;
						else
							rate_bitmap = 0x0f8ff005;
					}					
				}
			}
			break;

		case (ODM_WM_AC|ODM_WM_G):
			if(rssi_level == 1)
				rate_bitmap = 0xfc3f0000;
			else if(rssi_level == 2)
				rate_bitmap = 0xfffff000;
			else
				rate_bitmap = 0xffffffff;
			break;

		case (ODM_WM_AC|ODM_WM_A):

			if (pDM_Odm->RFType == RF_1T1R)
			{
				if(rssi_level == 1)			
					rate_bitmap = 0x003f8000;
				else if (rssi_level == 2)
					rate_bitmap = 0x003ff000;
				else
					rate_bitmap = 0x003ff010;
			}
			else
			{
				if(rssi_level == 1)			
					rate_bitmap = 0xfe3f8000;      
				else if (rssi_level == 2)
					rate_bitmap = 0xfffff000;      
				else
					rate_bitmap = 0xfffff010;      
			}
			break;
			
		default:
			if(pDM_Odm->RFType == RF_1T2R)
				rate_bitmap = 0x000fffff;
			else
				rate_bitmap = 0x0fffffff;
			break;	

	}

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, (" ==> rssi_level:0x%02x, WirelessMode:0x%02x, rate_bitmap:0x%08x \n",rssi_level,WirelessMode,rate_bitmap));

	return (ra_mask&rate_bitmap);
	
}	
#endif


VOID
odm_RefreshBasicRateMask(
	IN		PDM_ODM_T		pDM_Odm	
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER		Adapter	 =  pDM_Odm->Adapter;
	static u1Byte		Stage = 0;
	u1Byte			CurStage = 0;
	OCTET_STRING 	osRateSet;
	PMGNT_INFO		pMgntInfo = GetDefaultMgntInfo(Adapter);
	u1Byte 			RateSet[5] = {MGN_1M, MGN_2M, MGN_5_5M, MGN_11M, MGN_6M};

	if(pDM_Odm->SupportICType != ODM_RTL8812 && pDM_Odm->SupportICType != ODM_RTL8821 )
		return;

	if(pDM_Odm->bLinked == FALSE)
		CurStage = 0;	
	else if(pDM_Odm->RSSI_Min < 40)
		CurStage = 1;
	else if(pDM_Odm->RSSI_Min > 45)
		CurStage = 3;	
	else
		CurStage = 2;				

	if(CurStage != Stage)
	{
		if(CurStage == 1)
		{
			FillOctetString(osRateSet, RateSet, 5);
			FilterSupportRate(pMgntInfo->mBrates, &osRateSet, FALSE);
			Adapter->HalFunc.SetHwRegHandler(Adapter, HW_VAR_BASIC_RATE, (pu1Byte)&osRateSet);
		}
		else if(CurStage == 3 && (Stage == 1 || Stage == 2))
		{
			Adapter->HalFunc.SetHwRegHandler( Adapter, HW_VAR_BASIC_RATE, (pu1Byte)(&pMgntInfo->mBrates) );
		}
	}
	
	Stage = CurStage;
#endif
}

/*-----------------------------------------------------------------------------
 * Function:	odm_RefreshRateAdaptiveMask()
 *
 * Overview:	Update rate table mask according to rssi
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	05/27/2009	hpfan	Create Version 0.  
 *
 *---------------------------------------------------------------------------*/
VOID
odm_RefreshRateAdaptiveMask(
	IN		PDM_ODM_T		pDM_Odm
	)
{

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_TRACE, ("odm_RefreshRateAdaptiveMask()---------->\n"));	
	if (!(pDM_Odm->SupportAbility & ODM_BB_RA_MASK))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_TRACE, ("odm_RefreshRateAdaptiveMask(): Return cos not supported\n"));
		return;	
	}
	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:
			odm_RefreshRateAdaptiveMaskMP(pDM_Odm);
			break;

		case	ODM_CE:
			odm_RefreshRateAdaptiveMaskCE(pDM_Odm);
			break;

		case	ODM_AP:
		case	ODM_ADSL:
			odm_RefreshRateAdaptiveMaskAPADSL(pDM_Odm);
			break;
	}
	
}

VOID
odm_RefreshRateAdaptiveMaskMP(
	IN		PDM_ODM_T		pDM_Odm	
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER				pAdapter	 =  pDM_Odm->Adapter;
	PADAPTER 				pTargetAdapter = NULL;
	HAL_DATA_TYPE			*pHalData = GET_HAL_DATA(pAdapter);
	PMGNT_INFO				pMgntInfo = GetDefaultMgntInfo(pAdapter);
	PODM_RATE_ADAPTIVE		pRA = &pDM_Odm->RateAdaptive;

	if(pAdapter->bDriverStopped)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_TRACE, ("<---- odm_RefreshRateAdaptiveMask(): driver is going to unload\n"));
		return;
	}

	if(!pHalData->bUseRAMask)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("<---- odm_RefreshRateAdaptiveMask(): driver does not control rate adaptive mask\n"));
		return;
	}

	if(pMgntInfo->mAssoc && (!ACTING_AS_AP(pAdapter)))
	{
	
		if(pHalData->UndecoratedSmoothedPWDB < pRA->LdpcThres)
		{
			pRA->bUseLdpc = TRUE;
			pRA->bLowerRtsRate = TRUE;
			if((pDM_Odm->SupportICType == ODM_RTL8821) && (pDM_Odm->CutVersion == ODM_CUT_A))
				MgntSet_TX_LDPC(pAdapter,0,TRUE);
		}
		else if(pHalData->UndecoratedSmoothedPWDB > (pRA->LdpcThres-5))
		{
			pRA->bUseLdpc = FALSE;
			pRA->bLowerRtsRate = FALSE;
			if((pDM_Odm->SupportICType == ODM_RTL8821) && (pDM_Odm->CutVersion == ODM_CUT_A))
				MgntSet_TX_LDPC(pAdapter,0,FALSE);
		}
	
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("odm_RefreshRateAdaptiveMask(): Infrasture Mode\n"));
		if( ODM_RAStateCheck(pDM_Odm, pHalData->UndecoratedSmoothedPWDB, pMgntInfo->bSetTXPowerTrainingByOid, &pMgntInfo->Ratr_State) )
		{
			ODM_PRINT_ADDR(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Target AP addr : "), pMgntInfo->Bssid);
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pHalData->UndecoratedSmoothedPWDB, pMgntInfo->Ratr_State));
			pAdapter->HalFunc.UpdateHalRAMaskHandler(pAdapter, pMgntInfo->mMacId, NULL, pMgntInfo->Ratr_State);
		}
	}


	if(pMgntInfo->mIbss) 
		pTargetAdapter = GetDefaultAdapter(pAdapter);
	else
		pTargetAdapter = GetFirstAPAdapter(pAdapter);

	if(pTargetAdapter != NULL)
	{
		int	i;
		PRT_WLAN_STA	pEntry;

		for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
		{
			pEntry = AsocEntry_EnumStation(pTargetAdapter, i);
			if(NULL != pEntry)
			{
				if(pEntry->bAssociated)
				{
					if(ODM_RAStateCheck(pDM_Odm, pEntry->rssi_stat.UndecoratedSmoothedPWDB, pMgntInfo->bSetTXPowerTrainingByOid, &pEntry->Ratr_State) )
					{
						ODM_PRINT_ADDR(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Target STA addr : "), pEntry->MacAddr);
						ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pEntry->rssi_stat.UndecoratedSmoothedPWDB, pEntry->Ratr_State));
						pAdapter->HalFunc.UpdateHalRAMaskHandler(pTargetAdapter, pEntry->AssociatedMacId, pEntry, pEntry->Ratr_State);
					}
				}
			}
		}
	}

	if(pMgntInfo->bSetTXPowerTrainingByOid)
		pMgntInfo->bSetTXPowerTrainingByOid = FALSE;	
#endif
}


VOID
odm_RefreshRateAdaptiveMaskCE(
	IN		PDM_ODM_T		pDM_Odm	
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	u1Byte	i;
	PADAPTER	pAdapter	 =  pDM_Odm->Adapter;
	PODM_RATE_ADAPTIVE		pRA = &pDM_Odm->RateAdaptive;

	if(pAdapter->bDriverStopped)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_TRACE, ("<---- odm_RefreshRateAdaptiveMask(): driver is going to unload\n"));
		return;
	}

	if(!pDM_Odm->bUseRAMask)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("<---- odm_RefreshRateAdaptiveMask(): driver does not control rate adaptive mask\n"));
		return;
	}


	for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++){
		PSTA_INFO_T pstat = pDM_Odm->pODM_StaInfo[i];
		if(IS_STA_VALID(pstat) ) {
			if(IS_MCAST( pstat->hwaddr)) 
				 continue;
			if(IS_MCAST( pstat->hwaddr))
				continue;

			#if((RTL8812A_SUPPORT==1)||(RTL8821A_SUPPORT==1))
			if((pDM_Odm->SupportICType == ODM_RTL8812)||(pDM_Odm->SupportICType == ODM_RTL8821))
			{
				if(pstat->rssi_stat.UndecoratedSmoothedPWDB < pRA->LdpcThres)
				{
					pRA->bUseLdpc = TRUE;
					pRA->bLowerRtsRate = TRUE;
					if((pDM_Odm->SupportICType == ODM_RTL8821) && (pDM_Odm->CutVersion == ODM_CUT_A))
						Set_RA_LDPC_8812(pstat, TRUE);
				}
				else if(pstat->rssi_stat.UndecoratedSmoothedPWDB > (pRA->LdpcThres-5))
				{
					pRA->bUseLdpc = FALSE;
					pRA->bLowerRtsRate = FALSE;
					if((pDM_Odm->SupportICType == ODM_RTL8821) && (pDM_Odm->CutVersion == ODM_CUT_A))
						Set_RA_LDPC_8812(pstat, FALSE);
				}
			}
			#endif

			if( TRUE == ODM_RAStateCheck(pDM_Odm, pstat->rssi_stat.UndecoratedSmoothedPWDB, FALSE , &pstat->rssi_level) )
			{
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi_stat.UndecoratedSmoothedPWDB, pstat->rssi_level));
				rtw_hal_update_ra_mask(pstat, pstat->rssi_level);
			}
		
		}
	}			
	
#endif
}

VOID
odm_RefreshRateAdaptiveMaskAPADSL(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
	struct rtl8192cd_priv *priv = pDM_Odm->priv;
	struct stat_info	*pstat;

	if (!priv->pmib->dot11StationConfigEntry.autoRate) 
		return;

	if (list_empty(&priv->asoc_list))
		return;

	list_for_each_entry(pstat, &priv->asoc_list, asoc_list) {
		if(ODM_RAStateCheck(pDM_Odm, (s4Byte)pstat->rssi, FALSE, &pstat->rssi_level) ) {
			ODM_PRINT_ADDR(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Target STA addr : "), pstat->hwaddr);
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi, pstat->rssi_level));

#ifdef CONFIG_RTL_88E_SUPPORT
			if (GET_CHIP_VER(priv)==VERSION_8188E) {
#ifdef TXREPORT
				add_RATid(priv, pstat);
#endif
			} else
#endif
			{
#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_92C_SUPPORT)			
			add_update_RATid(priv, pstat);
#endif
		        }
	        }
	}
#endif
}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID
ODM_DynamicARFBSelect(
	IN		PDM_ODM_T		pDM_Odm,
	IN 		u1Byte			rate,
	IN  		BOOLEAN			Collision_State	
)
{

	if(pDM_Odm->SupportICType != ODM_RTL8192E)
		return;

	if (rate >= DESC_RATEMCS8  && rate <= DESC_RATEMCS12){
		if (Collision_State == 1){
			if(rate == DESC_RATEMCS12){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x0);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x07060501);	
			}
			else if(rate == DESC_RATEMCS11){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x0);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x07070605);	
			}
			else if(rate == DESC_RATEMCS10){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x0);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x08080706);	
			}
			else if(rate == DESC_RATEMCS9){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x0);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x08080707);	
			}
			else{

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x0);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x09090808);	
			}
		}
		else{  
			if(rate == DESC_RATEMCS12){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x05010000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x09080706);	
			}
			else if(rate == DESC_RATEMCS11){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x06050000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x09080807);	
			}
			else if(rate == DESC_RATEMCS10){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x07060000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x0a090908);	
			}
			else if(rate == DESC_RATEMCS9){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x07070000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x0a090808);	
			}
			else{

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x08080000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x0b0a0909);	
			}
		}
	}
	else{ 
		if (Collision_State == 1){
			if(rate == DESC_RATEMCS15){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x00000000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x05040302);	
			}
			else if(rate == DESC_RATEMCS14){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x00000000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x06050302);	
			}
			else if(rate == DESC_RATEMCS13){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x00000000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x07060502);	
			}
			else{

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x00000000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x06050402);	
			}
		}
		else{  
  			if(rate == DESC_RATEMCS15){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x03020000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x07060504);	
			}
			else if(rate == DESC_RATEMCS14){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x03020000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x08070605);	
			}
			else if(rate == DESC_RATEMCS13){

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x05020000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x09080706);	
			}
			else{

				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E, 0x04020000);
				ODM_Write4Byte(pDM_Odm, REG_DARFRC_8192E+4, 0x08070605);	
			}


		}

	}	

}

#endif

BOOLEAN 
ODM_RAStateCheck(
	IN		PDM_ODM_T		pDM_Odm,
	IN		s4Byte			RSSI,
	IN		BOOLEAN			bForceUpdate,
	OUT		pu1Byte			pRATRState
	)
{
	PODM_RATE_ADAPTIVE pRA = &pDM_Odm->RateAdaptive;
	const u1Byte GoUpGap = 5;
	u1Byte HighRSSIThreshForRA = pRA->HighRSSIThresh;
	u1Byte LowRSSIThreshForRA = pRA->LowRSSIThresh;
	u1Byte RATRState;

	switch (*pRATRState)
	{
		case DM_RATR_STA_INIT:
		case DM_RATR_STA_HIGH:
			break;

		case DM_RATR_STA_MIDDLE:
			HighRSSIThreshForRA += GoUpGap;
			break;

		case DM_RATR_STA_LOW:
			HighRSSIThreshForRA += GoUpGap;
			LowRSSIThreshForRA += GoUpGap;
			break;

		default: 
			ODM_RT_ASSERT(pDM_Odm, FALSE, ("wrong rssi level setting %d !", *pRATRState) );
			break;
	}

	if(RSSI > HighRSSIThreshForRA)
		RATRState = DM_RATR_STA_HIGH;
	else if(RSSI > LowRSSIThreshForRA)
		RATRState = DM_RATR_STA_MIDDLE;
	else
		RATRState = DM_RATR_STA_LOW;

	if( *pRATRState!=RATRState || bForceUpdate)
	{
		ODM_RT_TRACE( pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI Level %d -> %d\n", *pRATRState, RATRState) );
		*pRATRState = RATRState;
		return TRUE;
	}

	return FALSE;
}




VOID 
odm_DynamicTxPowerInit(
	IN		PDM_ODM_T		pDM_Odm	
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER	Adapter = pDM_Odm->Adapter;
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);

	#if DEV_BUS_TYPE==RT_USB_INTERFACE					
	if(RT_GetInterfaceSelection(Adapter) == INTF_SEL1_USB_High_Power)
	{
		odm_DynamicTxPowerSavePowerIndex(pDM_Odm);
		pMgntInfo->bDynamicTxPowerEnable = TRUE;
	}		
	else	
	#else
	if(IS_HARDWARE_TYPE_8192D(Adapter))
		pMgntInfo->bDynamicTxPowerEnable = TRUE;
	else
		pMgntInfo->bDynamicTxPowerEnable = FALSE;
	#endif
	

	pHalData->LastDTPLvl = TxHighPwrLevel_Normal;
	pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	PADAPTER	Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	pdmpriv->bDynamicTxPowerEnable = _FALSE;

	#if (RTL8192C_SUPPORT==1) 
	#ifdef CONFIG_USB_HCI

	#ifdef CONFIG_INTEL_PROXIM
	if((pHalData->BoardType == BOARD_USB_High_PA)||(Adapter->proximity.proxim_support==_TRUE))
	#else
	if(pHalData->BoardType == BOARD_USB_High_PA)
	#endif

	{
		odm_DynamicTxPowerSavePowerIndex(pDM_Odm);
		pdmpriv->bDynamicTxPowerEnable = _TRUE;
	}		
	else	
	#else
		pdmpriv->bDynamicTxPowerEnable = _FALSE;
	#endif
	#endif
	
	pdmpriv->LastDTPLvl = TxHighPwrLevel_Normal;
	pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;	
	
#endif
	
}

VOID
odm_DynamicTxPowerSavePowerIndex(
	IN		PDM_ODM_T		pDM_Odm	
	)
{	
	u1Byte		index;
	u4Byte		Power_Index_REG[6] = {0xc90, 0xc91, 0xc92, 0xc98, 0xc99, 0xc9a};
	
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)	
	PADAPTER	Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);	
	for(index = 0; index< 6; index++)
		pHalData->PowerIndex_backup[index] = PlatformEFIORead1Byte(Adapter, Power_Index_REG[index]);
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)	
	PADAPTER	Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	for(index = 0; index< 6; index++)
		pdmpriv->PowerIndex_backup[index] = rtw_read8(Adapter, Power_Index_REG[index]);
#endif
}

VOID
odm_DynamicTxPowerRestorePowerIndex(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	u1Byte			index;
	PADAPTER		Adapter = pDM_Odm->Adapter;

#if (DM_ODM_SUPPORT_TYPE & (ODM_CE|ODM_WIN))
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u4Byte			Power_Index_REG[6] = {0xc90, 0xc91, 0xc92, 0xc98, 0xc99, 0xc9a};
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	for(index = 0; index< 6; index++)
		PlatformEFIOWrite1Byte(Adapter, Power_Index_REG[index], pHalData->PowerIndex_backup[index]);
#elif(DM_ODM_SUPPORT_TYPE == ODM_CE)	
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	for(index = 0; index< 6; index++)
		rtw_write8(Adapter, Power_Index_REG[index], pdmpriv->PowerIndex_backup[index]);
#endif
#endif
}

VOID
odm_DynamicTxPowerWritePowerIndex(
	IN	PDM_ODM_T	pDM_Odm, 
	IN 	u1Byte		Value)
{

	u1Byte			index;
	u4Byte			Power_Index_REG[6] = {0xc90, 0xc91, 0xc92, 0xc98, 0xc99, 0xc9a};
	
	for(index = 0; index< 6; index++)
		ODM_Write1Byte(pDM_Odm, Power_Index_REG[index], Value);

}


VOID 
odm_DynamicTxPower(
	IN		PDM_ODM_T		pDM_Odm
	)
{

	if (!(pDM_Odm->SupportAbility & ODM_BB_DYNAMIC_TXPWR))
		return;

	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:
		case	ODM_CE:
			odm_DynamicTxPowerNIC(pDM_Odm);
			break;	
		case	ODM_AP:
			odm_DynamicTxPowerAP(pDM_Odm);
			break;		

		case	ODM_ADSL:
			break;	
	}

	
}


VOID 
odm_DynamicTxPowerNIC(
	IN		PDM_ODM_T		pDM_Odm
	)
{	
	if (!(pDM_Odm->SupportAbility & ODM_BB_DYNAMIC_TXPWR))
		return;
	
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))

	if(pDM_Odm->SupportICType == ODM_RTL8192C)	
	{
		odm_DynamicTxPower_92C(pDM_Odm);
	}
	else if(pDM_Odm->SupportICType == ODM_RTL8192D)
	{
		odm_DynamicTxPower_92D(pDM_Odm);
	}
	else if (pDM_Odm->SupportICType == ODM_RTL8821)
	{
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN))
		PADAPTER		Adapter	 =  pDM_Odm->Adapter;
		PMGNT_INFO		pMgntInfo = GetDefaultMgntInfo(Adapter);

		if (pMgntInfo->RegRspPwr == 1)
		{
			if(pDM_Odm->RSSI_Min > 60)
			{
				ODM_SetMACReg(pDM_Odm, ODM_REG_RESP_TX_11AC, BIT20|BIT19|BIT18, 1);

			}
			else if(pDM_Odm->RSSI_Min < 55)
			{
				ODM_SetMACReg(pDM_Odm, ODM_REG_RESP_TX_11AC, BIT20|BIT19|BIT18, 0);
			}
		}
#endif
	}
#endif	
}

VOID 
odm_DynamicTxPowerAP(
	IN		PDM_ODM_T		pDM_Odm

	)
{	
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	prtl8192cd_priv	priv		= pDM_Odm->priv;
	s4Byte i;

	if(!priv->pshare->rf_ft_var.tx_pwr_ctrl)
		return;
	
#ifdef HIGH_POWER_EXT_PA
	if(pDM_Odm->ExtPA)
		tx_power_control(priv);
#endif		

	/*
	 *	Check if station is near by to use lower tx power
	 */

	if ((priv->up_time % 3) == 0 )  {
		for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++){
			PSTA_INFO_T pstat = pDM_Odm->pODM_StaInfo[i];
			if(IS_STA_VALID(pstat) ) {
				if ((pstat->hp_level == 0) && (pstat->rssi > TX_POWER_NEAR_FIELD_THRESH_AP+4))
					pstat->hp_level = 1;
				else if ((pstat->hp_level == 1) && (pstat->rssi < TX_POWER_NEAR_FIELD_THRESH_AP))
					pstat->hp_level = 0;
			}
		}
	}

#endif	
}


VOID 
odm_DynamicTxPower_92C(
	IN	PDM_ODM_T	pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER Adapter = pDM_Odm->Adapter;
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	s4Byte				UndecoratedSmoothedPWDB;

	if (pDM_Odm->ExtPA == FALSE)
		return;

	if((!pMgntInfo->bMediaConnect) &&	
		(pHalData->EntryMinUndecoratedSmoothedPWDB == 0))
	{
		ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("Not connected to any \n"));
		pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;

		 pHalData->LastDTPLvl=TxHighPwrLevel_Normal;
		return;
	}

#if (INTEL_PROXIMITY_SUPPORT == 1)
	if(pMgntInfo->IntelProximityModeInfo.PowerOutput > 0)
	{
		switch(pMgntInfo->IntelProximityModeInfo.PowerOutput){
			case 1:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_100;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_100\n"));
				break;
			case 2:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_70;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_70\n"));
				break;
			case 3:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_50;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_50\n"));
				break;
			case 4:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_35;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_35\n"));
				break;
			case 5:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_15;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_15\n"));
				break;
			default:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_100;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_100\n"));
				break;
		}		
	}
	else
#endif		
	{ 
		if(	(pMgntInfo->bDynamicTxPowerEnable != TRUE) ||
			(pHalData->DMFlag & HAL_DM_HIPWR_DISABLE) ||
			pMgntInfo->IOTAction & HT_IOT_ACT_DISABLE_HIGH_POWER)
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		}
		else
		{
			if(pMgntInfo->bMediaConnect)
			{
				if(ACTING_AS_AP(Adapter) || ACTING_AS_IBSS(Adapter))
				{
					UndecoratedSmoothedPWDB = pHalData->EntryMinUndecoratedSmoothedPWDB;
					ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Client PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
				}
				else
				{
					UndecoratedSmoothedPWDB = pHalData->UndecoratedSmoothedPWDB;
					ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("STA Default Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
				}
			}
			else
			{	
				UndecoratedSmoothedPWDB = pHalData->EntryMinUndecoratedSmoothedPWDB;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Ext Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
			}
				
			if(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL2)
			{
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level2;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x0)\n"));
			}
			else if((UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL2-3)) &&
				(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL1) )
			{
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x10)\n"));
			}
			else if(UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL1-5))
			{
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Normal\n"));
			}
		}
	}
	if( pHalData->DynamicTxHighPowerLvl != pHalData->LastDTPLvl )
	{
		ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("PHY_SetTxPowerLevel8192C() Channel = %d \n" , pHalData->CurrentChannel));
		PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
		if(	(pHalData->DynamicTxHighPowerLvl == TxHighPwrLevel_Normal) &&
			(pHalData->LastDTPLvl == TxHighPwrLevel_Level1 || pHalData->LastDTPLvl == TxHighPwrLevel_Level2))
			odm_DynamicTxPowerRestorePowerIndex(pDM_Odm);
		else if(pHalData->DynamicTxHighPowerLvl == TxHighPwrLevel_Level1)
			odm_DynamicTxPowerWritePowerIndex(pDM_Odm, 0x14);
		else if(pHalData->DynamicTxHighPowerLvl == TxHighPwrLevel_Level2)
			odm_DynamicTxPowerWritePowerIndex(pDM_Odm, 0x10);
	}
	pHalData->LastDTPLvl = pHalData->DynamicTxHighPowerLvl;

	
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)

	#if (RTL8192C_SUPPORT==1) 
	PADAPTER Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	struct mlme_priv	*pmlmepriv = &(Adapter->mlmepriv);
	struct mlme_ext_priv	*pmlmeext = &Adapter->mlmeextpriv;
	int	UndecoratedSmoothedPWDB;

	if(!pdmpriv->bDynamicTxPowerEnable)
		return;

#ifdef CONFIG_INTEL_PROXIM
	if(Adapter->proximity.proxim_on== _TRUE){
		struct proximity_priv *prox_priv=Adapter->proximity.proximity_priv;
		printk("\n %s  Adapter->proximity.proxim_on=%d prox_priv->proxim_modeinfo->power_output=%d \n",__FUNCTION__,Adapter->proximity.proxim_on,prox_priv->proxim_modeinfo->power_output);
		if(prox_priv!=NULL){
			if(prox_priv->proxim_modeinfo->power_output> 0)	
			{
				switch(prox_priv->proxim_modeinfo->power_output)
				{
					case 1:
						pdmpriv->DynamicTxHighPowerLvl  = TxHighPwrLevel_100;
						printk("TxHighPwrLevel_100\n");
						break;
					case 2:
						pdmpriv->DynamicTxHighPowerLvl  = TxHighPwrLevel_70;
						printk("TxHighPwrLevel_70\n");
						break;
					case 3:
						pdmpriv->DynamicTxHighPowerLvl  = TxHighPwrLevel_50;
						printk("TxHighPwrLevel_50\n");
						break;
					case 4:
						pdmpriv->DynamicTxHighPowerLvl  = TxHighPwrLevel_35;
						printk("TxHighPwrLevel_35\n");
						break;
					case 5:
						pdmpriv->DynamicTxHighPowerLvl  = TxHighPwrLevel_15;
						printk("TxHighPwrLevel_15\n");
						break;
					default:
						pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_100;
						printk("TxHighPwrLevel_100\n");
						break;
				}		
			}
		}
	}
	else
#endif	
	{
		if((check_fwstate(pmlmepriv, _FW_LINKED) != _TRUE) &&	
			(pdmpriv->EntryMinUndecoratedSmoothedPWDB == 0))
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;

			pdmpriv->LastDTPLvl=TxHighPwrLevel_Normal;
			return;
		}
		
		if(check_fwstate(pmlmepriv, _FW_LINKED) == _TRUE)
		{
		#if 0
			if ((check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE) ||
			       (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE))
			{
				UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;
			}
			else
			{
				UndecoratedSmoothedPWDB = pdmpriv->UndecoratedSmoothedPWDB;
			}
		#else
		UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;	
		#endif
		}
		else
		{	
			UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;
		}
			
		if(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL2)
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level2;
		}
		else if((UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL2-3)) &&
			(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL1) )
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
		}
		else if(UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL1-5))
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		}
	}
	if( (pdmpriv->DynamicTxHighPowerLvl != pdmpriv->LastDTPLvl) )
	{
		PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
		if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Normal)
			odm_DynamicTxPowerRestorePowerIndex(pDM_Odm);
		else if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level1)
			odm_DynamicTxPowerWritePowerIndex(pDM_Odm, 0x14);
		else if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level2)
			odm_DynamicTxPowerWritePowerIndex(pDM_Odm, 0x10);
	}
	pdmpriv->LastDTPLvl = pdmpriv->DynamicTxHighPowerLvl;
	#endif
#endif

}


VOID 
odm_DynamicTxPower_92D(
	IN	PDM_ODM_T	pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER Adapter = pDM_Odm->Adapter;
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	s4Byte				UndecoratedSmoothedPWDB;

	PADAPTER	BuddyAdapter = Adapter->BuddyAdapter;
	BOOLEAN		bGetValueFromBuddyAdapter = dm_DualMacGetParameterFromBuddyAdapter(Adapter);
	u1Byte		HighPowerLvlBackForMac0 = TxHighPwrLevel_Level1;

	if (pDM_Odm->ExtPA == FALSE)
		return;

	if( (pMgntInfo->bDynamicTxPowerEnable != TRUE) ||
		(pHalData->DMFlag & HAL_DM_HIPWR_DISABLE) ||
		pMgntInfo->IOTAction & HT_IOT_ACT_DISABLE_HIGH_POWER)
	{
		pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		return;
	}

	if((!pMgntInfo->bMediaConnect) &&	
		(pHalData->EntryMinUndecoratedSmoothedPWDB == 0))
	{
		ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("Not connected to any \n"));
		pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;

		 pHalData->LastDTPLvl=TxHighPwrLevel_Normal;
		return;
	}
	
	if(pMgntInfo->bMediaConnect)
	{
		if(ACTING_AS_AP(Adapter) || pMgntInfo->mIbss)
		{
			UndecoratedSmoothedPWDB = pHalData->EntryMinUndecoratedSmoothedPWDB;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Client PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
		}
		else
		{
			UndecoratedSmoothedPWDB = pHalData->UndecoratedSmoothedPWDB;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("STA Default Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
		}
	}
	else
	{	
		UndecoratedSmoothedPWDB = pHalData->EntryMinUndecoratedSmoothedPWDB;
		ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Ext Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
	}
	
	if(IS_HARDWARE_TYPE_8192D(Adapter) && GET_HAL_DATA(Adapter)->CurrentBandType == 1){
		if(UndecoratedSmoothedPWDB >= 0x33)
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level2;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("5G:TxHighPwrLevel_Level2 (TxPwr=0x0)\n"));
		}
		else if((UndecoratedSmoothedPWDB <0x33) &&
			(UndecoratedSmoothedPWDB >= 0x2b) )
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("5G:TxHighPwrLevel_Level1 (TxPwr=0x10)\n"));
		}
		else if(UndecoratedSmoothedPWDB < 0x2b)
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("5G:TxHighPwrLevel_Normal\n"));
		}

	}
	else
	
	{
		if(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL2)
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x0)\n"));
		}
		else if((UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL2-3)) &&
			(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL1) )
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x10)\n"));
		}
		else if(UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL1-5))
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Normal\n"));
		}

	}

	if(bGetValueFromBuddyAdapter)
	{
		ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() mac 0 for mac 1 \n"));
		if(Adapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP)
		{
			ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() change value \n"));
			HighPowerLvlBackForMac0 = pHalData->DynamicTxHighPowerLvl;
			pHalData->DynamicTxHighPowerLvl = Adapter->DualMacDMSPControl.CurTxHighLvlForAnotherMacOfDMSP;
			PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
			pHalData->DynamicTxHighPowerLvl = HighPowerLvlBackForMac0;
			Adapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP = FALSE;
		}						
	}

	if( (pHalData->DynamicTxHighPowerLvl != pHalData->LastDTPLvl) )
	{
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("PHY_SetTxPowerLevel8192S() Channel = %d \n" , pHalData->CurrentChannel));
			if(Adapter->DualMacSmartConcurrent == TRUE)
			{
				if(BuddyAdapter == NULL)
				{
					ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() BuddyAdapter == NULL case \n"));
					if(!Adapter->bSlaveOfDMSP)
					{
						PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
					}
				}
				else
				{
					if(pHalData->MacPhyMode92D == DUALMAC_SINGLEPHY)
					{
						ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() BuddyAdapter DMSP \n"));
						if(Adapter->bSlaveOfDMSP)
						{
							ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() bslave case  \n"));
							BuddyAdapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP = TRUE;
							BuddyAdapter->DualMacDMSPControl.CurTxHighLvlForAnotherMacOfDMSP = pHalData->DynamicTxHighPowerLvl;
						}
						else
						{
							ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() master case  \n"));					
							if(!bGetValueFromBuddyAdapter)
							{
								ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() mac 0 for mac 0 \n"));
								PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
							}
						}
					}
					else
					{
						ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() BuddyAdapter DMDP\n"));
						PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
					}
				}
			}
			else
			{
				PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
			}

		}
	pHalData->LastDTPLvl = pHalData->DynamicTxHighPowerLvl;
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
#if (RTL8192D_SUPPORT==1) 
	PADAPTER Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct mlme_priv	*pmlmepriv = &(Adapter->mlmepriv);

	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	DM_ODM_T 		*podmpriv = &pHalData->odmpriv;
	int	UndecoratedSmoothedPWDB;
	#if (RTL8192D_EASY_SMART_CONCURRENT == 1)
	PADAPTER	BuddyAdapter = Adapter->BuddyAdapter;
	BOOLEAN		bGetValueFromBuddyAdapter = DualMacGetParameterFromBuddyAdapter(Adapter);
	u8		HighPowerLvlBackForMac0 = TxHighPwrLevel_Level1;
	#endif

	if( (pdmpriv->bDynamicTxPowerEnable != _TRUE) ||
		(!(podmpriv->SupportAbility& ODM_BB_DYNAMIC_TXPWR)) )
	{
		pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		return;
	}

	if((check_fwstate(pmlmepriv, _FW_LINKED) != _TRUE) &&	
		(pdmpriv->EntryMinUndecoratedSmoothedPWDB == 0))
	{
		pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		pdmpriv->LastDTPLvl=TxHighPwrLevel_Normal;
		return;
	}
		
	if(check_fwstate(pmlmepriv, _FW_LINKED) == _TRUE)
	{
	#if 0
		if ((check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE) ||
	       (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE))
		{
			UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;
		}
		else
		{
			UndecoratedSmoothedPWDB = pdmpriv->UndecoratedSmoothedPWDB;
		}
	#else
	UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;
	#endif
	}
	else
	{	
		UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;
	}
#if TX_POWER_FOR_5G_BAND == 1
	if(pHalData->CurrentBandType92D == BAND_ON_5G){
		if(UndecoratedSmoothedPWDB >= 0x33)
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level2;
		}
		else if((UndecoratedSmoothedPWDB <0x33) &&
			(UndecoratedSmoothedPWDB >= 0x2b) )
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
		}
		else if(UndecoratedSmoothedPWDB < 0x2b)
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		}
	}
	else
#endif
	{
		if(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL2)
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level2;
		}
		else if((UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL2-3)) &&
			(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL1) )
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
		}
		else if(UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL1-5))
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		}
	}
#if (RTL8192D_EASY_SMART_CONCURRENT == 1)
	if(bGetValueFromBuddyAdapter)
	{
		if(Adapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP)
		{
			HighPowerLvlBackForMac0 = pHalData->DynamicTxHighPowerLvl;
			pHalData->DynamicTxHighPowerLvl = Adapter->DualMacDMSPControl.CurTxHighLvlForAnotherMacOfDMSP;
			PHY_SetTxPowerLevel8192D(Adapter, pHalData->CurrentChannel);
			pHalData->DynamicTxHighPowerLvl = HighPowerLvlBackForMac0;
			Adapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP = _FALSE;
		}						
	}
#endif

	if( (pdmpriv->DynamicTxHighPowerLvl != pdmpriv->LastDTPLvl) )
	{
#if (RTL8192D_EASY_SMART_CONCURRENT == 1)
		if(BuddyAdapter == NULL)
		{
			if(!Adapter->bSlaveOfDMSP)
			{
				PHY_SetTxPowerLevel8192D(Adapter, pHalData->CurrentChannel);
			}
		}
		else
		{
			if(pHalData->MacPhyMode92D == DUALMAC_SINGLEPHY)
			{
				if(Adapter->bSlaveOfDMSP)
				{
					BuddyAdapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP = _TRUE;
					BuddyAdapter->DualMacDMSPControl.CurTxHighLvlForAnotherMacOfDMSP = pHalData->DynamicTxHighPowerLvl;
				}
				else
				{
					if(!bGetValueFromBuddyAdapter)
					{
						PHY_SetTxPowerLevel8192D(Adapter, pHalData->CurrentChannel);
					}
				}
			}
			else
			{
				PHY_SetTxPowerLevel8192D(Adapter, pHalData->CurrentChannel);
			}
		}
#else
		PHY_SetTxPowerLevel8192D(Adapter, pHalData->CurrentChannel);
#endif
	}
	pdmpriv->LastDTPLvl = pdmpriv->DynamicTxHighPowerLvl;
#endif	
#endif

}



VOID
odm_RSSIDumpToRegister(
	IN	PDM_ODM_T	pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER		Adapter = pDM_Odm->Adapter;

	if(pDM_Odm->SupportICType == ODM_RTL8812)
	{
		PlatformEFIOWrite1Byte(Adapter, rA_RSSIDump_Jaguar, Adapter->RxStats.RxRSSIPercentage[0]);
		PlatformEFIOWrite1Byte(Adapter, rB_RSSIDump_Jaguar, Adapter->RxStats.RxRSSIPercentage[1]);

		PlatformEFIOWrite1Byte(Adapter, rS1_RXevmDump_Jaguar, Adapter->RxStats.RxEVMdbm[0]);
		PlatformEFIOWrite1Byte(Adapter, rS2_RXevmDump_Jaguar, Adapter->RxStats.RxEVMdbm[1]);

		PlatformEFIOWrite1Byte(Adapter, rA_RXsnrDump_Jaguar, (u1Byte)(Adapter->RxStats.RxSNRdB[0]));
		PlatformEFIOWrite1Byte(Adapter, rB_RXsnrDump_Jaguar, (u1Byte)(Adapter->RxStats.RxSNRdB[1]));

		PlatformEFIOWrite2Byte(Adapter, rA_CfoShortDump_Jaguar, Adapter->RxStats.RxCfoShort[0]);
		PlatformEFIOWrite2Byte(Adapter, rB_CfoShortDump_Jaguar, Adapter->RxStats.RxCfoShort[1]);

		PlatformEFIOWrite2Byte(Adapter, rA_CfoLongDump_Jaguar, Adapter->RxStats.RxCfoTail[0]);
		PlatformEFIOWrite2Byte(Adapter, rB_CfoLongDump_Jaguar, Adapter->RxStats.RxCfoTail[1]);
	}
	else if(pDM_Odm->SupportICType == ODM_RTL8192E)
	{
		PlatformEFIOWrite1Byte(Adapter, rA_RSSIDump_92E, Adapter->RxStats.RxRSSIPercentage[0]);
		PlatformEFIOWrite1Byte(Adapter, rB_RSSIDump_92E, Adapter->RxStats.RxRSSIPercentage[1]);
		PlatformEFIOWrite1Byte(Adapter, rS1_RXevmDump_92E, Adapter->RxStats.RxEVMdbm[0]);
		PlatformEFIOWrite1Byte(Adapter, rS2_RXevmDump_92E, Adapter->RxStats.RxEVMdbm[1]);
		PlatformEFIOWrite1Byte(Adapter, rA_RXsnrDump_92E, (u1Byte)(Adapter->RxStats.RxSNRdB[0]));
		PlatformEFIOWrite1Byte(Adapter, rB_RXsnrDump_92E, (u1Byte)(Adapter->RxStats.RxSNRdB[1]));
		PlatformEFIOWrite2Byte(Adapter, rA_CfoShortDump_92E, Adapter->RxStats.RxCfoShort[0]);
		PlatformEFIOWrite2Byte(Adapter, rB_CfoShortDump_92E, Adapter->RxStats.RxCfoShort[1]);
		PlatformEFIOWrite2Byte(Adapter, rA_CfoLongDump_92E, Adapter->RxStats.RxCfoTail[0]);
		PlatformEFIOWrite2Byte(Adapter, rB_CfoLongDump_92E, Adapter->RxStats.RxCfoTail[1]);
	 }
#endif
}


VOID
odm_RSSIMonitorInit(
	IN	PDM_ODM_T	pDM_Odm
	)
{
	pRA_T		pRA_Table = &pDM_Odm->DM_RA_Table;

   	pRA_Table->firstconnect = FALSE;

}

VOID
odm_RSSIMonitorCheck(
	IN		PDM_ODM_T		pDM_Odm
	)
{

	if (!(pDM_Odm->SupportAbility & ODM_BB_RSSI_MONITOR))
		return;
	
	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:
			odm_RSSIMonitorCheckMP(pDM_Odm);
			break;

		case	ODM_CE:
			odm_RSSIMonitorCheckCE(pDM_Odm);
			break;

		case	ODM_AP:
			odm_RSSIMonitorCheckAP(pDM_Odm);
			break;		

		case	ODM_ADSL:
			break;	
	}
	
}


VOID
odm_RSSIMonitorCheckMP(
	IN	PDM_ODM_T	pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PRT_WLAN_STA	pEntry;
	u1Byte			i;
	s4Byte			tmpEntryMaxPWDB=0, tmpEntryMinPWDB=0xff;
	u1Byte			H2C_Parameter[4] ={0};
	PMGNT_INFO		pMgntInfo = &Adapter->MgntInfo;
	u8Byte			curTxOkCnt = 0, curRxOkCnt = 0;	
	u1Byte			STBC_TX = 0;
	BOOLEAN			FirstConnect;                                                    
	pRA_T			pRA_Table = &pDM_Odm->DM_RA_Table;      
#if (BEAMFORMING_SUPPORT == 1)	
	BEAMFORMING_CAP Beamform_cap = BEAMFORMING_CAP_NONE;
	u1Byte			TxBF_EN = 0;
#endif

	RT_DISP(FDM, DM_PWDB, ("pHalData->UndecoratedSmoothedPWDB = 0x%x( %d)\n", 
		pHalData->UndecoratedSmoothedPWDB,
		pHalData->UndecoratedSmoothedPWDB));

	curTxOkCnt = Adapter->TxStats.NumTxBytesUnicast - pMgntInfo->lastTxOkCnt;
	curRxOkCnt = Adapter->RxStats.NumRxBytesUnicast - pMgntInfo->lastRxOkCnt;
	pMgntInfo->lastTxOkCnt = curTxOkCnt;
	pMgntInfo->lastRxOkCnt = curRxOkCnt;

	RT_DISP(FDM, DM_PWDB, ("Tx = %d Rx = %d\n", curTxOkCnt, curRxOkCnt));

       FirstConnect = (pHalData->bLinked) && (pRA_Table->firstconnect == FALSE);    
	pRA_Table->firstconnect = pHalData->bLinked;                                               
       H2C_Parameter[3] |= FirstConnect << 5;

	if(pDM_Odm->SupportICType == ODM_RTL8188E && (pMgntInfo->CustomerID==RT_CID_819x_HP))
	{
		if(curRxOkCnt >(curTxOkCnt*6))
			PlatformEFIOWrite4Byte(Adapter, REG_ARFR0, 0x8f015);
		else
			PlatformEFIOWrite4Byte(Adapter, REG_ARFR0, 0xff015);
	}	

	if(pDM_Odm->SupportICType == ODM_RTL8812 || pDM_Odm->SupportICType == ODM_RTL8821)
	{
		if(curRxOkCnt >(curTxOkCnt*6))
			H2C_Parameter[3]=0x01;
		else
			H2C_Parameter[3]=0x00;
	}

	for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
		{
			pEntry = AsocEntry_EnumStation(GetFirstExtAdapter(Adapter), i);
		}
		else
		{
			pEntry = AsocEntry_EnumStation(GetDefaultAdapter(Adapter), i);
		}

		if(pEntry != NULL)
		{
			if(pEntry->bAssociated)
			{
			
				RT_DISP_ADDR(FDM, DM_PWDB, ("pEntry->MacAddr ="), pEntry->MacAddr);
				RT_DISP(FDM, DM_PWDB, ("pEntry->rssi = 0x%x(%d)\n", 
					pEntry->rssi_stat.UndecoratedSmoothedPWDB, pEntry->rssi_stat.UndecoratedSmoothedPWDB));

				if(pDM_Odm->SupportICType == ODM_RTL8192E || pDM_Odm->SupportICType == ODM_RTL8812)
				{

#if (BEAMFORMING_SUPPORT == 1)
					Beamform_cap = Beamforming_GetEntryBeamCapByMacId(pMgntInfo, pEntry->AssociatedMacId);
					if(Beamform_cap & (BEAMFORMER_CAP_HT_EXPLICIT |BEAMFORMER_CAP_VHT_SU))
						TxBF_EN = 1;
					else
						TxBF_EN = 0;
	
					H2C_Parameter[3] |= TxBF_EN << 6; 
					
					if(TxBF_EN)
						STBC_TX = 0;
					else
#endif
					{
						if(IS_WIRELESS_MODE_AC(Adapter))
							STBC_TX = TEST_FLAG(pEntry->VHTInfo.STBC, STBC_VHT_ENABLE_TX);
						else
							STBC_TX = TEST_FLAG(pEntry->HTInfo.STBC, STBC_HT_ENABLE_TX);
					}

					H2C_Parameter[3] |= STBC_TX << 1;
				}

				if(pEntry->rssi_stat.UndecoratedSmoothedPWDB < tmpEntryMinPWDB)
					tmpEntryMinPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;
				if(pEntry->rssi_stat.UndecoratedSmoothedPWDB > tmpEntryMaxPWDB)
					tmpEntryMaxPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;

				H2C_Parameter[2] = (u1Byte)(pEntry->rssi_stat.UndecoratedSmoothedPWDB & 0xFF);
				H2C_Parameter[1] = 0x20;  
				H2C_Parameter[0] = (pEntry->AssociatedMacId);
				if(pDM_Odm->SupportICType == ODM_RTL8812)
					ODM_FillH2CCmd(Adapter, ODM_H2C_RSSI_REPORT, 4, H2C_Parameter);
				else if(pDM_Odm->SupportICType == ODM_RTL8192E)
					ODM_FillH2CCmd(Adapter, ODM_H2C_RSSI_REPORT, 4, H2C_Parameter);
				else	
					ODM_FillH2CCmd(Adapter, ODM_H2C_RSSI_REPORT, 3, H2C_Parameter);
			}
		}
		else
		{
			break;
		}
	}

	if(tmpEntryMaxPWDB != 0)
	{
		pHalData->EntryMaxUndecoratedSmoothedPWDB = tmpEntryMaxPWDB;
		RT_DISP(FDM, DM_PWDB, ("EntryMaxPWDB = 0x%x(%d)\n",	tmpEntryMaxPWDB, tmpEntryMaxPWDB));
	}
	else
	{
		pHalData->EntryMaxUndecoratedSmoothedPWDB = 0;
	}
	
	if(tmpEntryMinPWDB != 0xff)
	{
		pHalData->EntryMinUndecoratedSmoothedPWDB = tmpEntryMinPWDB;
		RT_DISP(FDM, DM_PWDB, ("EntryMinPWDB = 0x%x(%d)\n", tmpEntryMinPWDB, tmpEntryMinPWDB));

	}
	else
	{
		pHalData->EntryMinUndecoratedSmoothedPWDB = 0;
	}

	if(pHalData->bUseRAMask)
	{
		if(pDM_Odm->SupportICType == ODM_RTL8192E || pDM_Odm->SupportICType == ODM_RTL8812)
		{
			PRT_HIGH_THROUGHPUT 		pHTInfo = GET_HT_INFO(pMgntInfo);
			PRT_VERY_HIGH_THROUGHPUT	pVHTInfo = GET_VHT_INFO(pMgntInfo);

#if (BEAMFORMING_SUPPORT == 1)
			
			Beamform_cap = Beamforming_GetEntryBeamCapByMacId(pMgntInfo, pMgntInfo->mMacId);

			if(Beamform_cap & (BEAMFORMER_CAP_HT_EXPLICIT |BEAMFORMER_CAP_VHT_SU))
				TxBF_EN = 1;
			else
				TxBF_EN = 0;

			H2C_Parameter[3] |= TxBF_EN << 6; 

			if(TxBF_EN)
				STBC_TX = 0;
			else
#endif
			{
				if(IS_WIRELESS_MODE_AC(Adapter))
					STBC_TX = TEST_FLAG(pVHTInfo->VhtCurStbc, STBC_VHT_ENABLE_TX);
				else
					STBC_TX = TEST_FLAG(pHTInfo->HtCurStbc, STBC_HT_ENABLE_TX);
			}

			H2C_Parameter[3] |= STBC_TX << 1;
		}
		
		H2C_Parameter[2] = (u1Byte)(pHalData->UndecoratedSmoothedPWDB & 0xFF);
		H2C_Parameter[1] = 0x20;
		H2C_Parameter[0] = 0;	
		if(pDM_Odm->SupportICType == ODM_RTL8812)
			ODM_FillH2CCmd(Adapter, ODM_H2C_RSSI_REPORT, 4, H2C_Parameter);
		else  if(pDM_Odm->SupportICType == ODM_RTL8192E)
			ODM_FillH2CCmd(Adapter, ODM_H2C_RSSI_REPORT, 4, H2C_Parameter);	
		else	
			ODM_FillH2CCmd(Adapter, ODM_H2C_RSSI_REPORT, 3, H2C_Parameter);
	}
	else
	{
		PlatformEFIOWrite1Byte(Adapter, 0x4fe, (u1Byte)pHalData->UndecoratedSmoothedPWDB);
	}

	if((pDM_Odm->SupportICType == ODM_RTL8812)||(pDM_Odm->SupportICType == ODM_RTL8192E))
		odm_RSSIDumpToRegister(pDM_Odm);

	odm_FindMinimumRSSI(Adapter);
	ODM_CmnInfoUpdate(&pHalData->DM_OutSrc ,ODM_CMNINFO_LINK, (u8Byte)pHalData->bLinked);
	ODM_CmnInfoUpdate(&pHalData->DM_OutSrc ,ODM_CMNINFO_RSSI_MIN, (u8Byte)pHalData->MinUndecoratedPWDBForDM);
#endif
}

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
static VOID
FindMinimumRSSI_Dmsp(
	IN	PADAPTER	pAdapter
)
{
#if 0
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	s32	Rssi_val_min_back_for_mac0;
	BOOLEAN		bGetValueFromBuddyAdapter = dm_DualMacGetParameterFromBuddyAdapter(pAdapter);
	BOOLEAN		bRestoreRssi = _FALSE;
	PADAPTER	BuddyAdapter = pAdapter->BuddyAdapter;

	if(pHalData->MacPhyMode92D == DUALMAC_SINGLEPHY)
	{
		if(BuddyAdapter!= NULL)
		{
			if(pHalData->bSlaveOfDMSP)
			{
				BuddyAdapter->DualMacDMSPControl.RssiValMinForAnotherMacOfDMSP = pdmpriv->MinUndecoratedPWDBForDM;
			}
			else
			{
				if(bGetValueFromBuddyAdapter)
				{
					bRestoreRssi = _TRUE;
					Rssi_val_min_back_for_mac0 = pdmpriv->MinUndecoratedPWDBForDM;
					pdmpriv->MinUndecoratedPWDBForDM = pAdapter->DualMacDMSPControl.RssiValMinForAnotherMacOfDMSP;
				}
			}
		}
		
	}

	if(bRestoreRssi)
	{
		bRestoreRssi = _FALSE;
		pdmpriv->MinUndecoratedPWDBForDM = Rssi_val_min_back_for_mac0;
	}
#endif
}

static void
FindMinimumRSSI(
IN	PADAPTER	pAdapter
	)
{	
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;	
	PDM_ODM_T		pDM_Odm = &(pHalData->odmpriv);


	if((pDM_Odm->bLinked != _TRUE) &&
		(pdmpriv->EntryMinUndecoratedSmoothedPWDB == 0))
	{
		pdmpriv->MinUndecoratedPWDBForDM = 0;
	}
	else
	{
		pdmpriv->MinUndecoratedPWDBForDM = pdmpriv->EntryMinUndecoratedSmoothedPWDB;
	}

}
#endif

VOID
odm_RSSIMonitorCheckCE(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	PADAPTER	Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(Adapter);
	int	i;
	int	tmpEntryMaxPWDB=0, tmpEntryMinPWDB=0xff;
	u8 	sta_cnt=0;
	u32	UL_DL_STATE = 0, STBC_TX = 0, TxBF_EN = 0;
	u32	PWDB_rssi[NUM_STA]={0};//[0~15]:MACID, [16~31]:PWDB_rssi
	BOOLEAN			FirstConnect = FALSE;
	pRA_T			pRA_Table = &pDM_Odm->DM_RA_Table;

	if(pDM_Odm->bLinked != _TRUE)
		return;

	#if((RTL8812A_SUPPORT==1)||(RTL8821A_SUPPORT==1))
	if((pDM_Odm->SupportICType == ODM_RTL8812)||(pDM_Odm->SupportICType == ODM_RTL8821))
	{
		u64	curTxOkCnt = pdvobjpriv->traffic_stat.cur_tx_bytes;
		u64	curRxOkCnt = pdvobjpriv->traffic_stat.cur_rx_bytes;

		if(curRxOkCnt >(curTxOkCnt*6))
			UL_DL_STATE = 1;
		else
			UL_DL_STATE = 0;
	}
	#endif

       FirstConnect = (pDM_Odm->bLinked) && (pRA_Table->firstconnect == FALSE);    
	pRA_Table->firstconnect = pDM_Odm->bLinked;

	{
		#if 1
		struct sta_info *psta;
		
		for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++) {
			if (IS_STA_VALID(psta = pDM_Odm->pODM_StaInfo[i]))
			{
                        		if(IS_MCAST( psta->hwaddr)) 
						 continue;
								
					if(psta->rssi_stat.UndecoratedSmoothedPWDB == (-1))
						 continue;
								
					if(psta->rssi_stat.UndecoratedSmoothedPWDB < tmpEntryMinPWDB)
						tmpEntryMinPWDB = psta->rssi_stat.UndecoratedSmoothedPWDB;

					if(psta->rssi_stat.UndecoratedSmoothedPWDB > tmpEntryMaxPWDB)
						tmpEntryMaxPWDB = psta->rssi_stat.UndecoratedSmoothedPWDB;

					#if 0
					DBG_871X("%s mac_id:%u, mac:"MAC_FMT", rssi:%d\n", __func__,
						psta->mac_id, MAC_ARG(psta->hwaddr), psta->rssi_stat.UndecoratedSmoothedPWDB);
					#endif

					if(psta->rssi_stat.UndecoratedSmoothedPWDB != (-1)) {

#ifdef CONFIG_80211N_HT
						if(pDM_Odm->SupportICType == ODM_RTL8192E || pDM_Odm->SupportICType == ODM_RTL8812)
						{
#ifdef CONFIG_BEAMFORMING
							BEAMFORMING_CAP Beamform_cap = beamforming_get_entry_beam_cap_by_mac_id(&Adapter->mlmepriv, psta->mac_id);

							if(Beamform_cap & (BEAMFORMER_CAP_HT_EXPLICIT |BEAMFORMER_CAP_VHT_SU))
								TxBF_EN = 1;
							else
								TxBF_EN = 0;

							if (TxBF_EN) {
								STBC_TX = 0;
							}
							else
#endif
							{
#ifdef CONFIG_80211AC_VHT
								if(IsSupportedVHT(psta->wireless_mode))
									STBC_TX = TEST_FLAG(psta->vhtpriv.stbc_cap, STBC_VHT_ENABLE_TX);
								else	
#endif
									STBC_TX = TEST_FLAG(psta->htpriv.stbc_cap, STBC_HT_ENABLE_TX);
							}
						}
#endif

						if(pDM_Odm->SupportICType == ODM_RTL8192D)
							PWDB_rssi[sta_cnt++] = (psta->mac_id | (psta->rssi_stat.UndecoratedSmoothedPWDB<<16) | ((Adapter->stapriv.asoc_sta_count+1) << 8));
						else if ((pDM_Odm->SupportICType == ODM_RTL8192E)||(pDM_Odm->SupportICType == ODM_RTL8812)||(pDM_Odm->SupportICType == ODM_RTL8821))
							PWDB_rssi[sta_cnt++] = (((u8)(psta->mac_id&0xFF)) | ((psta->rssi_stat.UndecoratedSmoothedPWDB&0x7F)<<16) |(STBC_TX << 25) | (FirstConnect << 29) | (TxBF_EN << 30));
						else
							PWDB_rssi[sta_cnt++] = (psta->mac_id | (psta->rssi_stat.UndecoratedSmoothedPWDB<<16) );
					}
			}
		}
		#else
		_irqL irqL;
		_list	*plist, *phead;
		struct sta_info *psta;
		struct sta_priv *pstapriv = &Adapter->stapriv;
		u8 bcast_addr[ETH_ALEN]= {0xff,0xff,0xff,0xff,0xff,0xff};

		_enter_critical_bh(&pstapriv->sta_hash_lock, &irqL);

		for(i=0; i< NUM_STA; i++)
		{
			phead = &(pstapriv->sta_hash[i]);
			plist = get_next(phead);
		
			while ((rtw_end_of_queue_search(phead, plist)) == _FALSE)
			{
				psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);

				plist = get_next(plist);

				if(_rtw_memcmp(psta->hwaddr, bcast_addr, ETH_ALEN) || 
					_rtw_memcmp(psta->hwaddr, myid(&Adapter->eeprompriv), ETH_ALEN))
					continue;

				if(psta->state & WIFI_ASOC_STATE)
				{
					
					if(psta->rssi_stat.UndecoratedSmoothedPWDB < tmpEntryMinPWDB)
						tmpEntryMinPWDB = psta->rssi_stat.UndecoratedSmoothedPWDB;

					if(psta->rssi_stat.UndecoratedSmoothedPWDB > tmpEntryMaxPWDB)
						tmpEntryMaxPWDB = psta->rssi_stat.UndecoratedSmoothedPWDB;

					if(psta->rssi_stat.UndecoratedSmoothedPWDB != (-1)){
						#if(RTL8192D_SUPPORT==1)
						PWDB_rssi[sta_cnt++] = (psta->mac_id | (psta->rssi_stat.UndecoratedSmoothedPWDB<<16) | ((Adapter->stapriv.asoc_sta_count+1) << 8));
						#else
						PWDB_rssi[sta_cnt++] = (psta->mac_id | (psta->rssi_stat.UndecoratedSmoothedPWDB<<16) );
						#endif
					}
				}
			
			}

		}
	
		_exit_critical_bh(&pstapriv->sta_hash_lock, &irqL);
		#endif


		for(i=0; i< sta_cnt; i++)
		{
			if(PWDB_rssi[i] != (0)){
				if(pHalData->fw_ractrl == _TRUE)// Report every sta's RSSI to FW
				{
					#if(RTL8192D_SUPPORT==1)
					if(pDM_Odm->SupportICType == ODM_RTL8192D){
						FillH2CCmd92D(Adapter, H2C_RSSI_REPORT, 3, (u8 *)(&PWDB_rssi[i]));		
					}
					#endif
					
					#if((RTL8192C_SUPPORT==1)||(RTL8723A_SUPPORT==1))
					if((pDM_Odm->SupportICType == ODM_RTL8192C)||(pDM_Odm->SupportICType == ODM_RTL8723A)){
						rtl8192c_set_rssi_cmd(Adapter, (u8*)&PWDB_rssi[i]);
					}
					#endif
					
					#if((RTL8812A_SUPPORT==1)||(RTL8821A_SUPPORT==1))
					if((pDM_Odm->SupportICType == ODM_RTL8812)||(pDM_Odm->SupportICType == ODM_RTL8821)){	
						PWDB_rssi[i] |= (UL_DL_STATE << 24);
						rtl8812_set_rssi_cmd(Adapter, (u8 *)(&PWDB_rssi[i]));
					}
					#endif
					#if(RTL8192E_SUPPORT==1)
					if(pDM_Odm->SupportICType == ODM_RTL8192E){
						rtl8192e_set_rssi_cmd(Adapter, (u8 *)(&PWDB_rssi[i]));
					}
					#endif
					#if(RTL8723B_SUPPORT==1)
					if(pDM_Odm->SupportICType == ODM_RTL8723B){
						rtl8723b_set_rssi_cmd(Adapter, (u8 *)(&PWDB_rssi[i]));
					}
					#endif
				}
				else{
					#if((RTL8188E_SUPPORT==1)&&(RATE_ADAPTIVE_SUPPORT == 1))
					if(pDM_Odm->SupportICType == ODM_RTL8188E){
						ODM_RA_SetRSSI_8188E(
						&(pHalData->odmpriv), (PWDB_rssi[i]&0xFF), (u8)((PWDB_rssi[i]>>16) & 0xFF));
					}
					#endif
				}
			}
		}		
	}



	if(tmpEntryMaxPWDB != 0)
	{
		pdmpriv->EntryMaxUndecoratedSmoothedPWDB = tmpEntryMaxPWDB;		
	}
	else
	{
		pdmpriv->EntryMaxUndecoratedSmoothedPWDB = 0;
	}

	if(tmpEntryMinPWDB != 0xff)
	{
		pdmpriv->EntryMinUndecoratedSmoothedPWDB = tmpEntryMinPWDB;		
	}
	else
	{
		pdmpriv->EntryMinUndecoratedSmoothedPWDB = 0;
	}

	FindMinimumRSSI(Adapter);//get pdmpriv->MinUndecoratedPWDBForDM

	#if(RTL8192D_SUPPORT==1)
	FindMinimumRSSI_Dmsp(Adapter);
	#endif
	pDM_Odm->RSSI_Min = pdmpriv->MinUndecoratedPWDBForDM;
#endif//if (DM_ODM_SUPPORT_TYPE == ODM_CE)
}
VOID
odm_RSSIMonitorCheckAP(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
#ifdef CONFIG_RTL_92C_SUPPORT || defined(CONFIG_RTL_92D_SUPPORT)

	u4Byte i;
	PSTA_INFO_T pstat;

	for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		pstat = pDM_Odm->pODM_StaInfo[i];
		if(IS_STA_VALID(pstat) )
		{			
#ifdef STA_EXT
			if (REMAP_AID(pstat) < (FW_NUM_STAT - 1))
#endif
				add_update_rssi(pDM_Odm->priv, pstat);

		}		
	}
#endif
#endif

}



VOID
ODM_InitAllTimers(
	IN PDM_ODM_T	pDM_Odm 
	)
{
#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))
#if (RTL8723B_SUPPORT == 1)||(RTL8821A_SUPPORT == 1)
	ODM_InitializeTimer(pDM_Odm,&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchTimer_8723B,
	(RT_TIMER_CALL_BACK)ODM_SW_AntDiv_Callback, NULL, "SwAntennaSwitchTimer_8723B");
#endif
#endif

#if(defined(CONFIG_SW_ANTENNA_DIVERSITY))
	ODM_InitializeTimer(pDM_Odm,&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchTimer,
		(RT_TIMER_CALL_BACK)odm_SwAntDivChkAntSwitchCallback, NULL, "SwAntennaSwitchTimer");
#endif
	
#if (!(DM_ODM_SUPPORT_TYPE == ODM_CE))
#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))
#if (RTL8188E_SUPPORT == 1)
	ODM_InitializeTimer(pDM_Odm,&pDM_Odm->FastAntTrainingTimer,
		(RT_TIMER_CALL_BACK)odm_FastAntTrainingCallback, NULL, "FastAntTrainingTimer");
#endif
#endif
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	ODM_InitializeTimer(pDM_Odm, &pDM_Odm->PSDTimer, 
		(RT_TIMER_CALL_BACK)dm_PSDMonitorCallback, NULL, "PSDTimer");
	ODM_InitializeTimer(pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 
		(RT_TIMER_CALL_BACK)odm_PathDivChkAntSwitchCallback, NULL, "PathDivTimer");

	ODM_InitializeTimer(pDM_Odm, &pDM_Odm->CCKPathDiversityTimer, 
		(RT_TIMER_CALL_BACK)odm_CCKTXPathDiversityCallback, NULL, "CCKPathDiversityTimer");

	ODM_InitializeTimer(pDM_Odm, &pDM_Odm->MPT_DIGTimer, 
		(RT_TIMER_CALL_BACK)odm_MPT_DIGCallback, NULL, "MPT_DIGTimer");

	ODM_InitializeTimer(pDM_Odm, &pDM_Odm->DM_RXHP_Table.PSDTimer,
		(RT_TIMER_CALL_BACK)odm_PSD_RXHPCallback, NULL, "PSDRXHPTimer");  
#endif	
}

VOID
ODM_CancelAllTimers(
	IN PDM_ODM_T	pDM_Odm 
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	HAL_ADAPTER_STS_CHK(pDM_Odm)
#endif	
#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))
#if (RTL8723B_SUPPORT == 1)||(RTL8821A_SUPPORT == 1)
	ODM_CancelTimer(pDM_Odm,&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchTimer_8723B);
#endif
#endif

#if(defined(CONFIG_SW_ANTENNA_DIVERSITY))
	ODM_CancelTimer(pDM_Odm,&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchTimer);
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

#if (RTL8188E_SUPPORT == 1)
	ODM_CancelTimer(pDM_Odm,&pDM_Odm->FastAntTrainingTimer);
#endif
	ODM_CancelTimer(pDM_Odm, &pDM_Odm->PSDTimer);	
	ODM_CancelTimer(pDM_Odm, &pDM_Odm->PathDivSwitchTimer);

	ODM_CancelTimer(pDM_Odm, &pDM_Odm->CCKPathDiversityTimer);

	ODM_CancelTimer(pDM_Odm, &pDM_Odm->MPT_DIGTimer);

	ODM_CancelTimer(pDM_Odm, &pDM_Odm->DM_RXHP_Table.PSDTimer);
#endif	
}


VOID
ODM_ReleaseAllTimers(
	IN PDM_ODM_T	pDM_Odm 
	)
{
#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))
#if (RTL8723B_SUPPORT == 1)||(RTL8821A_SUPPORT == 1)
	ODM_ReleaseTimer(pDM_Odm,&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchTimer_8723B);
#endif
#endif

#if(defined(CONFIG_SW_ANTENNA_DIVERSITY))
	ODM_ReleaseTimer(pDM_Odm,&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchTimer);
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

#if (RTL8188E_SUPPORT == 1)
	ODM_ReleaseTimer(pDM_Odm,&pDM_Odm->FastAntTrainingTimer);
#endif

	ODM_ReleaseTimer(pDM_Odm, &pDM_Odm->PSDTimer);
	ODM_ReleaseTimer(pDM_Odm, &pDM_Odm->PathDivSwitchTimer);

	ODM_ReleaseTimer(pDM_Odm, &pDM_Odm->CCKPathDiversityTimer);

	ODM_ReleaseTimer(pDM_Odm, &pDM_Odm->MPT_DIGTimer);

	ODM_ReleaseTimer(pDM_Odm, &pDM_Odm->DM_RXHP_Table.PSDTimer); 
#endif	
}



VOID
odm_IQCalibrate(
		IN	PDM_ODM_T	pDM_Odm 
		)
{
	PADAPTER	Adapter = pDM_Odm->Adapter;
	
	if(!IS_HARDWARE_TYPE_JAGUAR(Adapter))
		return;
	else if(IS_HARDWARE_TYPE_8812AU(Adapter))
		return;
#if (RTL8821A_SUPPORT == 1)
	if(pDM_Odm->bLinked)
	{
		if((*pDM_Odm->pChannel != pDM_Odm->preChannel) && (!*pDM_Odm->pbScanInProcess))
		{
			pDM_Odm->preChannel = *pDM_Odm->pChannel;
			pDM_Odm->LinkedInterval = 0;
		}

		if(pDM_Odm->LinkedInterval < 3)
			pDM_Odm->LinkedInterval++;
		
		if(pDM_Odm->LinkedInterval == 2)
		{
			PHY_IQCalibrate_8821A(Adapter, FALSE);
		}
	}
	else
		pDM_Odm->LinkedInterval = 0;
#endif
}


VOID
odm_TXPowerTrackingInit(
	IN	PDM_ODM_T	pDM_Odm 
	)
{
	odm_TXPowerTrackingThermalMeterInit(pDM_Odm);
}	

u1Byte 
getSwingIndex(
	IN	PDM_ODM_T	pDM_Odm 
	)
{
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u1Byte 			i = 0;
	u4Byte 			bbSwing;
	u4Byte 			swingTableSize;
	pu4Byte 			pSwingTable;

	if (pDM_Odm->SupportICType == ODM_RTL8188E || pDM_Odm->SupportICType == ODM_RTL8723B ||
		pDM_Odm->SupportICType == ODM_RTL8192E) 
	{
		bbSwing = PHY_QueryBBReg(Adapter, rOFDM0_XATxIQImbalance, 0xFFC00000);

		pSwingTable = OFDMSwingTable_New;
		swingTableSize = OFDM_TABLE_SIZE;
	} else {
#if ((RTL8812A_SUPPORT==1)||(RTL8821A_SUPPORT==1))
		if (pDM_Odm->SupportICType == ODM_RTL8812 || pDM_Odm->SupportICType == ODM_RTL8821)
		{
			bbSwing = PHY_GetTxBBSwing_8812A(Adapter, pHalData->CurrentBandType, ODM_RF_PATH_A);
			pSwingTable = TxScalingTable_Jaguar;
			swingTableSize = TXSCALE_TABLE_SIZE;
		}
		else
#endif
		{
			bbSwing = 0;
			pSwingTable = OFDMSwingTable;
			swingTableSize = OFDM_TABLE_SIZE;
		}
	}

	for (i = 0; i < swingTableSize; ++i) {
		u4Byte tableValue = pSwingTable[i];
		
		if (tableValue >= 0x100000 )
			tableValue >>= 22;
		if (bbSwing == tableValue)
			break;
	}
	return i;
}

VOID
odm_TXPowerTrackingThermalMeterInit(
	IN	PDM_ODM_T	pDM_Odm 
	)
{
	u1Byte defaultSwingIndex = getSwingIndex(pDM_Odm);
	u1Byte 			p = 0;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	#if	MP_DRIVER != 1				
	pHalData->TxPowerTrackControl = TRUE;		
	#endif
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	PADAPTER			Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);


	if (pDM_Odm->SupportICType >= ODM_RTL8188E) 
	{
		pDM_Odm->RFCalibrateInfo.bTXPowerTracking = _TRUE;
		pDM_Odm->RFCalibrateInfo.TXPowercount = 0;
		pDM_Odm->RFCalibrateInfo.bTXPowerTrackingInit = _FALSE;
		if ( *(pDM_Odm->mp_mode) != 1)
			pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = _TRUE;
		MSG_8192C("pDM_Odm TxPowerTrackControl = %d\n", pDM_Odm->RFCalibrateInfo.TxPowerTrackControl);
	}
	else
	{
		struct dm_priv	*pdmpriv = &pHalData->dmpriv;

		pdmpriv->bTXPowerTracking = _TRUE;
		pdmpriv->TXPowercount = 0;
		pdmpriv->bTXPowerTrackingInit = _FALSE;

		if (*(pDM_Odm->mp_mode) != 1)
			pdmpriv->TxPowerTrackControl = _TRUE;

	}
	
#elif (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
	#ifdef RTL8188E_SUPPORT
	{
		pDM_Odm->RFCalibrateInfo.bTXPowerTracking = _TRUE;
		pDM_Odm->RFCalibrateInfo.TXPowercount = 0;
		pDM_Odm->RFCalibrateInfo.bTXPowerTrackingInit = _FALSE;
		pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = _TRUE;
	}
	#endif
#endif

	pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = TRUE;
	pDM_Odm->RFCalibrateInfo.ThermalValue = pHalData->EEPROMThermalMeter;
	pDM_Odm->RFCalibrateInfo.ThermalValue_IQK = pHalData->EEPROMThermalMeter;
	pDM_Odm->RFCalibrateInfo.ThermalValue_LCK = pHalData->EEPROMThermalMeter;	

	if (pDM_Odm->SupportICType == ODM_RTL8188E || pDM_Odm->SupportICType == ODM_RTL8723B ||
		pDM_Odm->SupportICType == ODM_RTL8192E) 
	{
		pDM_Odm->DefaultOfdmIndex = (defaultSwingIndex >= OFDM_TABLE_SIZE) ? 30 : defaultSwingIndex;
		pDM_Odm->DefaultCckIndex = 20;	
	}
	else
	{
		pDM_Odm->DefaultOfdmIndex = (defaultSwingIndex >= TXSCALE_TABLE_SIZE) ? 24 : defaultSwingIndex;
		pDM_Odm->DefaultCckIndex = 24;	
	}

	pDM_Odm->BbSwingIdxCckBase = pDM_Odm->DefaultCckIndex;
	pDM_Odm->RFCalibrateInfo.CCK_index = pDM_Odm->DefaultCckIndex;
	
	for (p = ODM_RF_PATH_A; p < MAX_RF_PATH; ++p)
	{
		pDM_Odm->BbSwingIdxOfdmBase[p] = pDM_Odm->DefaultOfdmIndex;		
	   	pDM_Odm->RFCalibrateInfo.OFDM_index[p] = pDM_Odm->DefaultOfdmIndex;		
		pDM_Odm->RFCalibrateInfo.DeltaPowerIndex[p] = 0;
		pDM_Odm->RFCalibrateInfo.DeltaPowerIndexLast[p] = 0;
		pDM_Odm->RFCalibrateInfo.PowerIndexOffset[p] = 0;
	}

}


VOID
ODM_TXPowerTrackingCheck(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:
			odm_TXPowerTrackingCheckMP(pDM_Odm);
			break;

		case	ODM_CE:
			odm_TXPowerTrackingCheckCE(pDM_Odm);
			break;

		case	ODM_AP:
			odm_TXPowerTrackingCheckAP(pDM_Odm);		
			break;		

		case	ODM_ADSL:
			break;	
	}

}

VOID
odm_TXPowerTrackingCheckCE(
	IN		PDM_ODM_T		pDM_Odm 
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	PADAPTER	Adapter = pDM_Odm->Adapter;
	#if( (RTL8192C_SUPPORT==1) ||  (RTL8723A_SUPPORT==1) )
	if(IS_HARDWARE_TYPE_8192C(Adapter)){
		rtl8192c_odm_CheckTXPowerTracking(Adapter);
		return;
	}
	#endif

	#if (RTL8192D_SUPPORT==1) 
	if(IS_HARDWARE_TYPE_8192D(Adapter)){	
		#if (RTL8192D_EASY_SMART_CONCURRENT == 1)
		if(!Adapter->bSlaveOfDMSP)
		#endif
			rtl8192d_odm_CheckTXPowerTracking(Adapter);
		return;	
	}
	#endif

	#if(((RTL8188E_SUPPORT==1) ||  (RTL8812A_SUPPORT==1) ||  (RTL8821A_SUPPORT==1) ||  (RTL8192E_SUPPORT==1)  ||  (RTL8723B_SUPPORT==1)  ))
	if(!(pDM_Odm->SupportAbility & ODM_RF_TX_PWR_TRACK))
	{
		return;
	}

	if(!pDM_Odm->RFCalibrateInfo.TM_Trigger)	
	{
		if(IS_HARDWARE_TYPE_8188E(Adapter) || IS_HARDWARE_TYPE_JAGUAR(Adapter) || IS_HARDWARE_TYPE_8192E(Adapter)||IS_HARDWARE_TYPE_8723B(Adapter))
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_T_METER_NEW, (BIT17 | BIT16), 0x03);
		else
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_T_METER_OLD, bRFRegOffsetMask, 0x60);
		
		
		pDM_Odm->RFCalibrateInfo.TM_Trigger = 1;
		return;
	}
	else
	{
		ODM_TXPowerTrackingCallback_ThermalMeter(Adapter);
		pDM_Odm->RFCalibrateInfo.TM_Trigger = 0;
	}
	#endif
#endif	
}

VOID
odm_TXPowerTrackingCheckMP(
	IN		PDM_ODM_T		pDM_Odm 
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER	Adapter = pDM_Odm->Adapter;

	if (ODM_CheckPowerStatus(Adapter) == FALSE) 
	{
		RT_TRACE(COMP_POWER_TRACKING, DBG_LOUD, ("===>ODM_CheckPowerStatus() return FALSE\n"));
		return;
	}

	if(IS_HARDWARE_TYPE_8723A(Adapter))
		return;

	if(!Adapter->bSlaveOfDMSP || Adapter->DualMacSmartConcurrent == FALSE)
		odm_TXPowerTrackingThermalMeterCheck(Adapter);
	else {
		RT_TRACE(COMP_POWER_TRACKING, DBG_LOUD, ("!Adapter->bSlaveOfDMSP || Adapter->DualMacSmartConcurrent == FALSE\n"));
	}
#endif
	
}


VOID
odm_TXPowerTrackingCheckAP(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	prtl8192cd_priv	priv		= pDM_Odm->priv;

	if ( (priv->pmib->dot11RFEntry.ther) && ((priv->up_time % priv->pshare->rf_ft_var.tpt_period) == 0)){
#ifdef CONFIG_RTL_92D_SUPPORT
		if (GET_CHIP_VER(priv)==VERSION_8192D){
			tx_power_tracking_92D(priv);
		} else 
#endif
		{
#ifdef CONFIG_RTL_92C_SUPPORT			
			tx_power_tracking(priv);
#endif
		}
	}
#endif	

}




#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

VOID
odm_TXPowerTrackingThermalMeterCheck(
	IN	PADAPTER		Adapter
	)
{
#ifndef AP_BUILD_WORKAROUND
	static u1Byte			TM_Trigger = 0;

	if(!(GET_HAL_DATA(Adapter)->DM_OutSrc.SupportAbility & ODM_RF_TX_PWR_TRACK))
	{
		RT_TRACE(COMP_POWER_TRACKING, DBG_LOUD,
			("===>odm_TXPowerTrackingThermalMeterCheck(),pMgntInfo->bTXPowerTracking is FALSE, return!!\n"));
		return;
	}

	if(!TM_Trigger)	
	{
		if(IS_HARDWARE_TYPE_8192D(Adapter))
			PHY_SetRFReg(Adapter, ODM_RF_PATH_A, RF_T_METER_92D, BIT17 | BIT16, 0x03);
		else if(IS_HARDWARE_TYPE_8188E(Adapter) || IS_HARDWARE_TYPE_JAGUAR(Adapter) || IS_HARDWARE_TYPE_8192E(Adapter) ||
			    IS_HARDWARE_TYPE_8723B(Adapter))
			PHY_SetRFReg(Adapter, ODM_RF_PATH_A, RF_T_METER_88E, BIT17 | BIT16, 0x03);
		else
			PHY_SetRFReg(Adapter, ODM_RF_PATH_A, RF_T_METER, bRFRegOffsetMask, 0x60);
		
		RT_TRACE(COMP_POWER_TRACKING, DBG_LOUD,("Trigger Thermal Meter!!\n"));
		
		TM_Trigger = 1;
		return;
	}
	else
	{
		RT_TRACE(COMP_POWER_TRACKING, DBG_LOUD,("Schedule TxPowerTracking direct call!!\n"));		
		odm_TXPowerTrackingDirectCall(Adapter);
		TM_Trigger = 0;
	}
#endif
}

VOID
odm_SwAntDivInit_NIC_8723A(
	IN	PDM_ODM_T		pDM_Odm)
{
	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	PADAPTER		Adapter = pDM_Odm->Adapter;
	
	u1Byte 			btAntNum=BT_GetPgAntNum(Adapter);

	if(IS_HARDWARE_TYPE_8723A(Adapter))
	{
		pDM_SWAT_Table->ANTA_ON =TRUE;
		
		if(btAntNum == 2)
			pDM_SWAT_Table->ANTB_ON = TRUE;
		else if(btAntNum == 1)
			pDM_SWAT_Table->ANTB_ON = FALSE;
		else
			pDM_SWAT_Table->ANTB_ON = TRUE;
	}	
	
}

#endif



#if(defined(CONFIG_SW_ANTENNA_DIVERSITY))
VOID
odm_SwAntDivInit(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
	odm_SwAntDivInit_NIC(pDM_Odm);
#elif(DM_ODM_SUPPORT_TYPE == ODM_AP)
	dm_SW_AntennaSwitchInit(pDM_Odm->priv);
#endif
}

VOID
odm_SwAntDivInit_NIC(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;


#if (RTL8723A_SUPPORT==1) 
	if(pDM_Odm->SupportICType == ODM_RTL8723A)
	{
		odm_SwAntDivInit_NIC_8723A(pDM_Odm);	
	}	
#endif
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS:Init SW Antenna Switch\n"));
	pDM_SWAT_Table->RSSI_sum_A = 0;
	pDM_SWAT_Table->RSSI_cnt_A = 0;
	pDM_SWAT_Table->RSSI_sum_B = 0;
	pDM_SWAT_Table->RSSI_cnt_B = 0;
	pDM_SWAT_Table->CurAntenna = MAIN_ANT;
	pDM_SWAT_Table->PreAntenna = MAIN_ANT;
	pDM_SWAT_Table->try_flag = 0xff;
	pDM_SWAT_Table->PreRSSI = 0;
	pDM_SWAT_Table->SWAS_NoLink_State = 0;
	pDM_SWAT_Table->bTriggerAntennaSwitch = 0;
	pDM_SWAT_Table->SelectAntennaMap=0xAA;
	pDM_SWAT_Table->lastTxOkCnt = 0;
	pDM_SWAT_Table->lastRxOkCnt = 0;
	pDM_SWAT_Table->TXByteCnt_A = 0;
	pDM_SWAT_Table->TXByteCnt_B = 0;
	pDM_SWAT_Table->RXByteCnt_A = 0;
	pDM_SWAT_Table->RXByteCnt_B = 0;
	pDM_SWAT_Table->TrafficLoad = TRAFFIC_LOW;
	pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 = ODM_Read4Byte(pDM_Odm, 0x860);
	
}

VOID
ODM_SwAntDivResetBeforeLink(
	IN		PDM_ODM_T		pDM_Odm
	)
{

	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;

	pDM_SWAT_Table->SWAS_NoLink_State = 0;

}

VOID
ODM_SwAntDivRestAfterLink(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	pFAT_T		pDM_FatTable = &pDM_Odm->DM_FatTable;
	u4Byte             i;

	if(pDM_Odm->SupportICType == ODM_RTL8723A)
	{
	    pDM_SWAT_Table->RSSI_cnt_A = 0;
	    pDM_SWAT_Table->RSSI_cnt_B = 0;
	    pDM_Odm->RSSI_test = FALSE;
	    pDM_SWAT_Table->try_flag = 0xff;
	    pDM_SWAT_Table->RSSI_Trying = 0;
	    pDM_SWAT_Table->SelectAntennaMap=0xAA;
	
	}
	else if(pDM_Odm->SupportICType & (ODM_RTL8723B|ODM_RTL8821))
	{
		pDM_Odm->RSSI_test = FALSE;
		pDM_SWAT_Table->try_flag = 0xff;
		pDM_SWAT_Table->RSSI_Trying = 0;
		pDM_SWAT_Table->Double_chk_flag= 0;
		
		pDM_FatTable->RxIdleAnt=MAIN_ANT;
		
		for (i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
		{
			pDM_FatTable->MainAnt_Sum[i] = 0;
			pDM_FatTable->AuxAnt_Sum[i] = 0;
			pDM_FatTable->MainAnt_Cnt[i] = 0;
			pDM_FatTable->AuxAnt_Cnt[i] = 0;
		}

	}
}

void
odm_SwAntDetectInit(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
#if (RTL8723B_SUPPORT == 1)
	pDM_SWAT_Table->SWAS_NoLink_BK_Reg92c = ODM_Read4Byte(pDM_Odm, rDPDT_control);
#endif
	pDM_SWAT_Table->PreAntenna = MAIN_ANT;
	pDM_SWAT_Table->CurAntenna = MAIN_ANT;
	pDM_SWAT_Table->SWAS_NoLink_State = 0;
}

VOID
ODM_SwAntDivChkPerPktRssi(
	IN PDM_ODM_T	pDM_Odm,
	IN u1Byte		StationID,
	IN PODM_PHY_INFO_T pPhyInfo
	)
{	
	SWAT_T		*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	
	if(!(pDM_Odm->SupportAbility & (ODM_BB_ANT_DIV)))
		return;

#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
	if(pDM_Odm->SupportICType == ODM_RTL8723A)
	{
		if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
		{
			pDM_SWAT_Table->RSSI_sum_A += pPhyInfo->RxPWDBAll;
			pDM_SWAT_Table->RSSI_cnt_A++;
		}
		else
		{
			pDM_SWAT_Table->RSSI_sum_B += pPhyInfo->RxPWDBAll;
			pDM_SWAT_Table->RSSI_cnt_B++;

		}
	}
	else
	{
		if(StationID == pDM_SWAT_Table->RSSI_target)
		{
			if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
			{
				pDM_SWAT_Table->RSSI_sum_A += pPhyInfo->RxPWDBAll;
				pDM_SWAT_Table->RSSI_cnt_A++;
			}
			else
			{
				pDM_SWAT_Table->RSSI_sum_B += pPhyInfo->RxPWDBAll;
				pDM_SWAT_Table->RSSI_cnt_B++;

			}
		}
	}
#else	
	if(StationID == pDM_SWAT_Table->RSSI_target)
	{
		if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
		{
			pDM_SWAT_Table->RSSI_sum_A += pPhyInfo->RxPWDBAll;
			pDM_SWAT_Table->RSSI_cnt_A++;
		}
		else
		{
			pDM_SWAT_Table->RSSI_sum_B += pPhyInfo->RxPWDBAll;
			pDM_SWAT_Table->RSSI_cnt_B++;

		}
	}
#endif
}

VOID
odm_SwAntDivChkAntSwitch(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			Step
	)
{
	prtl8192cd_priv	priv		= pDM_Odm->priv;

	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:	
			odm_SwAntDivChkAntSwitchNIC(pDM_Odm, Step);
			break;
		case	ODM_CE:
			odm_SwAntDivChkAntSwitchNIC(pDM_Odm, Step);
			break;

		case	ODM_AP:
		case	ODM_ADSL:
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP |ODM_ADSL))
			if (priv->pshare->rf_ft_var.antSw_enable && (priv->up_time % 4==1))
				dm_SW_AntennaSwitch(priv, SWAW_STEP_PEAK);
#endif		
			break;			
	}

}



VOID
ODM_SetAntenna(
	IN 	PDM_ODM_T	pDM_Odm,
	IN	u1Byte		Antenna)
{
	ODM_SetBBReg(pDM_Odm, 0x860, BIT8|BIT9, Antenna); 
}

VOID
odm_SwAntDivChkAntSwitchNIC(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte		Step
	)
{
#if ((RTL8192C_SUPPORT==1)||(RTL8723A_SUPPORT==1))

#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
	PADAPTER		Adapter=pDM_Odm->Adapter;
#endif

	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	s4Byte			curRSSI=100, RSSI_A, RSSI_B;
	u1Byte			nextAntenna=AUX_ANT;
	u8Byte			curTxOkCnt=0, curRxOkCnt=0;
	u8Byte			CurByteCnt=0, PreByteCnt=0;
	u1Byte			Score_A=0, Score_B=0;      
	u1Byte			i;

	if(!(pDM_Odm->SupportAbility & ODM_BB_ANT_DIV))
		return;

	if (pDM_Odm->SupportICType & (ODM_RTL8192D|ODM_RTL8188E))
		return;

	if((pDM_Odm->SupportICType == ODM_RTL8192C) &&(pDM_Odm->RFType == ODM_2T2R))
		return;

	if(pDM_Odm->SupportPlatform & ODM_WIN)
	{
		if(*(pDM_Odm->pAntennaTest))
			return;
	}

	if((pDM_SWAT_Table->ANTA_ON == FALSE) ||(pDM_SWAT_Table->ANTB_ON == FALSE))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,
				("odm_SwAntDivChkAntSwitch(): No AntDiv Mechanism, Antenna A or B is off\n"));
		return;
	}

	if(*(pDM_Odm->pbPowerSaving)==TRUE)
	{
		ODM_SwAntDivRestAfterLink(pDM_Odm);
		return;
	}


	if(	Step != pDM_SWAT_Table->try_flag	)
	{
		ODM_SwAntDivRestAfterLink(pDM_Odm);
	}

#if  (DM_ODM_SUPPORT_TYPE &( ODM_WIN| ODM_CE ))

	if(pDM_SWAT_Table->try_flag == 0xff)
	{
		pDM_SWAT_Table->RSSI_target = 0xff;
		
		#if(DM_ODM_SUPPORT_TYPE & ODM_CE)
		{
			u1Byte			index = 0;
			PSTA_INFO_T		pEntry = NULL;
			
			
			for(index=0; index<ODM_ASSOCIATE_ENTRY_NUM; index++)
			{					
				pEntry =  pDM_Odm->pODM_StaInfo[index];
				if(IS_STA_VALID(pEntry) ) {
					break;
				}
			}
			if(pEntry == NULL)
			{
				ODM_SwAntDivRestAfterLink(pDM_Odm);
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): No Link.\n"));
				return;
			}
			else
			{
				pDM_SWAT_Table->RSSI_target = index;
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): RSSI_target is PEER STA\n"));
			}
                }
		#elif (DM_ODM_SUPPORT_TYPE & ODM_WIN) 
		{
			PADAPTER	pAdapter	 =  pDM_Odm->Adapter;
			PMGNT_INFO	pMgntInfo=&pAdapter->MgntInfo;
			
			if(pMgntInfo->mAssoc && !ACTING_AS_AP(pAdapter))
			{
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("odm_SwAntDivChkAntSwitch(): RSSI_target is DEF AP!\n"));
			}
			else
			{
				u1Byte			index = 0;
				PSTA_INFO_T		pEntry = NULL;
				PADAPTER		pTargetAdapter = NULL;
			
				if(pMgntInfo->mIbss )
				{
					pTargetAdapter = pAdapter;
				}
				else
				{
					pTargetAdapter = GetFirstAPAdapter(pAdapter);
				}

				if(pTargetAdapter != NULL)
				{			
					for(index=0; index<ODM_ASSOCIATE_ENTRY_NUM; index++)
					{					
						
						pEntry = AsocEntry_EnumStation(pTargetAdapter, index);
						if(pEntry != NULL)
						{
							if(pEntry->bAssociated)
								break;			
						}
						
					}
					
				}

				if(pEntry == NULL)
				{
					ODM_SwAntDivRestAfterLink(pDM_Odm);
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): No Link.\n"));
					return;
				}
				else
				{
					pDM_SWAT_Table->RSSI_target = index;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): RSSI_target is PEER STA\n"));
				}
			}//end if(pMgntInfo->mAssoc && !ACTING_AS_AP(Adapter))

		}
		#endif

		pDM_SWAT_Table->RSSI_cnt_A = 0;
		pDM_SWAT_Table->RSSI_cnt_B = 0;
		pDM_SWAT_Table->try_flag = 0;
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("odm_SwAntDivChkAntSwitch(): Set try_flag to 0 prepare for peak!\n"));
		return;
	}
	else
	{

#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
		if (pDM_Odm->SupportICType & ODM_RTL8723A)
		{
			curTxOkCnt = Adapter->TxStats.NumTxBytesUnicast - pDM_SWAT_Table->lastTxOkCnt;
			curRxOkCnt =Adapter->RxStats.NumRxBytesUnicast - pDM_SWAT_Table->lastRxOkCnt;
			pDM_SWAT_Table->lastTxOkCnt = Adapter->TxStats.NumTxBytesUnicast;
			pDM_SWAT_Table->lastRxOkCnt = Adapter->RxStats.NumRxBytesUnicast;
		}
#else	
		curTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast) - pDM_SWAT_Table->lastTxOkCnt;
		curRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast) - pDM_SWAT_Table->lastRxOkCnt;
		pDM_SWAT_Table->lastTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast);
		pDM_SWAT_Table->lastRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast);
#endif	
		if(pDM_SWAT_Table->try_flag == 1)
		{
			if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
			{
				pDM_SWAT_Table->TXByteCnt_A += curTxOkCnt;
				pDM_SWAT_Table->RXByteCnt_A += curRxOkCnt;
			}
			else
			{
				pDM_SWAT_Table->TXByteCnt_B += curTxOkCnt;
				pDM_SWAT_Table->RXByteCnt_B += curRxOkCnt;
			}
		
			nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? AUX_ANT : MAIN_ANT;
			pDM_SWAT_Table->RSSI_Trying--;
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("RSSI_Trying = %d\n",pDM_SWAT_Table->RSSI_Trying));
			if(pDM_SWAT_Table->RSSI_Trying == 0)
			{
				CurByteCnt = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? (pDM_SWAT_Table->TXByteCnt_A+pDM_SWAT_Table->RXByteCnt_A) : (pDM_SWAT_Table->TXByteCnt_B+pDM_SWAT_Table->RXByteCnt_B);
				PreByteCnt = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? (pDM_SWAT_Table->TXByteCnt_B+pDM_SWAT_Table->RXByteCnt_B) : (pDM_SWAT_Table->TXByteCnt_A+pDM_SWAT_Table->RXByteCnt_A);
				
				if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_HIGH)
					PreByteCnt = PreByteCnt*9;
				else if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_LOW)
					PreByteCnt = PreByteCnt*2;

				if(pDM_SWAT_Table->RSSI_cnt_A > 0)
					RSSI_A = pDM_SWAT_Table->RSSI_sum_A/pDM_SWAT_Table->RSSI_cnt_A; 
				else
					RSSI_A = 0;
				if(pDM_SWAT_Table->RSSI_cnt_B > 0)
					RSSI_B = pDM_SWAT_Table->RSSI_sum_B/pDM_SWAT_Table->RSSI_cnt_B; 
				else
					RSSI_B = 0;
				curRSSI = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
				pDM_SWAT_Table->PreRSSI =  (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_B : RSSI_A;
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Luke:PreRSSI = %d, CurRSSI = %d\n",pDM_SWAT_Table->PreRSSI, curRSSI));
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: preAntenna= %s, curAntenna= %s \n", 
				(pDM_SWAT_Table->PreAntenna == MAIN_ANT?"MAIN":"AUX"), (pDM_SWAT_Table->CurAntenna == MAIN_ANT?"MAIN":"AUX")));
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Luke:RSSI_A= %d, RSSI_cnt_A = %d, RSSI_B= %d, RSSI_cnt_B = %d\n",
					RSSI_A, pDM_SWAT_Table->RSSI_cnt_A, RSSI_B, pDM_SWAT_Table->RSSI_cnt_B));
			}

		}
		else
		{
		
			if(pDM_SWAT_Table->RSSI_cnt_A > 0)
				RSSI_A = pDM_SWAT_Table->RSSI_sum_A/pDM_SWAT_Table->RSSI_cnt_A; 
			else
				RSSI_A = 0;
			if(pDM_SWAT_Table->RSSI_cnt_B > 0)
				RSSI_B = pDM_SWAT_Table->RSSI_sum_B/pDM_SWAT_Table->RSSI_cnt_B; 
			else
				RSSI_B = 0;
			curRSSI = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
			pDM_SWAT_Table->PreRSSI =  (pDM_SWAT_Table->PreAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Ekul:PreRSSI = %d, CurRSSI = %d\n", pDM_SWAT_Table->PreRSSI, curRSSI));
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: preAntenna= %s, curAntenna= %s \n", 
			(pDM_SWAT_Table->PreAntenna == MAIN_ANT?"MAIN":"AUX"), (pDM_SWAT_Table->CurAntenna == MAIN_ANT?"MAIN":"AUX")));

			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Ekul:RSSI_A= %d, RSSI_cnt_A = %d, RSSI_B= %d, RSSI_cnt_B = %d\n",
				RSSI_A, pDM_SWAT_Table->RSSI_cnt_A, RSSI_B, pDM_SWAT_Table->RSSI_cnt_B));
		}

		if((pDM_SWAT_Table->try_flag == 1)&&(pDM_SWAT_Table->RSSI_Trying == 0))
		{

			if(pDM_SWAT_Table->TestMode == TP_MODE)
			{
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: TestMode = TP_MODE"));
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("TRY:CurByteCnt = %lld,", CurByteCnt));
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("TRY:PreByteCnt = %lld\n",PreByteCnt));		
				if(CurByteCnt < PreByteCnt)
				{
					if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
						pDM_SWAT_Table->SelectAntennaMap=pDM_SWAT_Table->SelectAntennaMap<<1;
					else
						pDM_SWAT_Table->SelectAntennaMap=(pDM_SWAT_Table->SelectAntennaMap<<1)+1;
				}
				else
				{
					if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
						pDM_SWAT_Table->SelectAntennaMap=(pDM_SWAT_Table->SelectAntennaMap<<1)+1;
					else
						pDM_SWAT_Table->SelectAntennaMap=pDM_SWAT_Table->SelectAntennaMap<<1;
				}
				for (i= 0; i<8; i++)
				{
					if(((pDM_SWAT_Table->SelectAntennaMap>>i)&BIT0) == 1)
						Score_A++;
					else
						Score_B++;
				}
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SelectAntennaMap=%x\n ",pDM_SWAT_Table->SelectAntennaMap));
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Score_A=%d, Score_B=%d\n", Score_A, Score_B));
			
				if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
				{
					nextAntenna = (Score_A > Score_B)?MAIN_ANT:AUX_ANT;
				}
				else
				{
					nextAntenna = (Score_B > Score_A)?AUX_ANT:MAIN_ANT;
				}

				if(nextAntenna != pDM_SWAT_Table->CurAntenna)
				{
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: Switch back to another antenna"));
				}
				else
				{
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: current anntena is good\n"));
				}	
			}

			if(pDM_SWAT_Table->TestMode == RSSI_MODE)
			{	
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: TestMode = RSSI_MODE"));
				pDM_SWAT_Table->SelectAntennaMap=0xAA;
				if(curRSSI < pDM_SWAT_Table->PreRSSI)
				{
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: Switch back to another antenna"));
					nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? AUX_ANT : MAIN_ANT;
				}
				else
				{
					nextAntenna =pDM_SWAT_Table->CurAntenna;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: current anntena is good\n"));
				}
			}
			pDM_SWAT_Table->try_flag = 0;
			pDM_Odm->RSSI_test = FALSE;
			pDM_SWAT_Table->RSSI_sum_A = 0;
			pDM_SWAT_Table->RSSI_cnt_A = 0;
			pDM_SWAT_Table->RSSI_sum_B = 0;
			pDM_SWAT_Table->RSSI_cnt_B = 0;
			pDM_SWAT_Table->TXByteCnt_A = 0;
			pDM_SWAT_Table->TXByteCnt_B = 0;
			pDM_SWAT_Table->RXByteCnt_A = 0;
			pDM_SWAT_Table->RXByteCnt_B = 0;
			
		}

		else if(pDM_SWAT_Table->try_flag == 0)
		{
			if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_HIGH)
			{
				if ((curTxOkCnt+curRxOkCnt) > 3750000)//if(PlatformDivision64(curTxOkCnt+curRxOkCnt, 2) > 1875000)
					pDM_SWAT_Table->TrafficLoad = TRAFFIC_HIGH;
				else
					pDM_SWAT_Table->TrafficLoad = TRAFFIC_LOW;
			}
			else if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_LOW)
				{
				if ((curTxOkCnt+curRxOkCnt) > 3750000)
					pDM_SWAT_Table->TrafficLoad = TRAFFIC_HIGH;
				else
					pDM_SWAT_Table->TrafficLoad = TRAFFIC_LOW;
			}
			if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_HIGH)
				pDM_SWAT_Table->bTriggerAntennaSwitch = 0;

					nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? AUX_ANT : MAIN_ANT;
					pDM_SWAT_Table->try_flag = 1;
					pDM_Odm->RSSI_test = TRUE;
			if((curRxOkCnt+curTxOkCnt) > 1000)
			{
				pDM_SWAT_Table->RSSI_Trying = 4;
				pDM_SWAT_Table->TestMode = TP_MODE;
				}
				else
				{
				pDM_SWAT_Table->RSSI_Trying = 2;
				pDM_SWAT_Table->TestMode = RSSI_MODE;

			}
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: Normal State -> Begin Trying!\n"));
			
			
			pDM_SWAT_Table->RSSI_sum_A = 0;
			pDM_SWAT_Table->RSSI_cnt_A = 0;
			pDM_SWAT_Table->RSSI_sum_B = 0;
			pDM_SWAT_Table->RSSI_cnt_B = 0;
		}
	}

	if(nextAntenna != pDM_SWAT_Table->CurAntenna)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: Change TX Antenna!\n "));
		#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)			
		ODM_SetAntenna(pDM_Odm,nextAntenna);		
		#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
		{
			BOOLEAN bEnqueue;			
			bEnqueue = (pDM_Odm->SupportInterface ==  ODM_ITRF_PCIE)?FALSE :TRUE;			
			rtw_antenna_select_cmd(pDM_Odm->Adapter, nextAntenna, bEnqueue);
		}
		#endif
		
	}

	pDM_SWAT_Table->PreAntenna = pDM_SWAT_Table->CurAntenna;
	pDM_SWAT_Table->CurAntenna = nextAntenna;
	pDM_SWAT_Table->PreRSSI = curRSSI;

	{
	

	if(pDM_SWAT_Table->RSSI_Trying == 0)
		return;

	if(pDM_SWAT_Table->RSSI_Trying%2 == 0)
	{
		if(pDM_SWAT_Table->TestMode == TP_MODE)
		{
			if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_HIGH)
			{
				ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 10 );
				
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("dm_SW_AntennaSwitch(): Test another antenna for 10 ms\n"));
			}
			else if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_LOW)
			{
				ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 50 );
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("dm_SW_AntennaSwitch(): Test another antenna for 50 ms\n"));
			}
		}
		else
		{
			ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 500 );
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("dm_SW_AntennaSwitch(): Test another antenna for 500 ms\n"));
		}
	}
	else
	{
		if(pDM_SWAT_Table->TestMode == TP_MODE)
		{
			if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_HIGH)
				ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 90 );
			else if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_LOW)
				ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 100 );
		}
		else
			ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 500 );
	}
	}
#endif
#endif
}


#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

u1Byte
odm_SwAntDivSelectScanChnl(
	IN	PADAPTER	Adapter
	)
{
#if (RT_MEM_SIZE_LEVEL != RT_MEM_SIZE_MINIMUM)
	PHAL_DATA_TYPE		pHalData = GET_HAL_DATA(Adapter);
	PMGNT_INFO			pMgntInfo = &(Adapter->MgntInfo);
	PDM_ODM_T			pDM_Odm = &pHalData->DM_OutSrc;
	u1Byte 				i, j, ScanChannel = 0, ChannelNum = 0;
	PRT_CHANNEL_LIST	pChannelList = GET_RT_CHANNEL_LIST(pMgntInfo);
	u1Byte 				EachChannelSTAs[MAX_SCAN_CHANNEL_NUM] = {0};

	if(pMgntInfo->tmpNumBssDesc == 0)
		return 0;

	for(i = 0; i < pMgntInfo->tmpNumBssDesc; i++)
	{		
		ChannelNum = pMgntInfo->tmpbssDesc[i].ChannelNumber;
		for(j = 0; j < pChannelList->ChannelLen; j++)
		{
			if(pChannelList->ChnlListEntry[j].ChannelNum == ChannelNum)
			{
				EachChannelSTAs[j]++;
				break;
			}
		}
	}
	
	for(i = 0; i < MAX_SCAN_CHANNEL_NUM; i++)
		{
		if(EachChannelSTAs[i] > EachChannelSTAs[ScanChannel])
			ScanChannel = i;
		}

	if(EachChannelSTAs[ScanChannel] == 0)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, DBG_LOUD, ("odm_SwAntDivSelectScanChnl(): Scan List is empty.\n"));
		return 0;
	}
	
	ScanChannel = pChannelList->ChnlListEntry[ScanChannel].ChannelNum;

	

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, DBG_LOUD, 
		("odm_SwAntDivSelectScanChnl(): Channel %d is select as scan channel.\n", ScanChannel));

	return ScanChannel;
#else
	return	0;
#endif	
}


VOID
odm_SwAntDivConstructScanChnl(
	IN	PADAPTER	Adapter,
	IN	u1Byte		ScanChnl
	)
{

	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;

	if(ScanChnl == 0)
	{
		u1Byte				i;		
		PRT_CHANNEL_LIST	pChannelList = GET_RT_CHANNEL_LIST(pMgntInfo);
	
		RtActChannelList(Adapter, RT_CHNL_LIST_ACTION_CONSTRUCT_SCAN_LIST, NULL, NULL);
		for(i = 0; i < pChannelList->ChannelLen; i++)
			pChannelList->ChnlListEntry[i].ScanPeriod /= 2;
	}
	else
	{
		CUSTOMIZED_SCAN_REQUEST CustomScanReq;

		CustomScanReq.bEnabled = TRUE;
		CustomScanReq.Channels[0] = ScanChnl;
		CustomScanReq.Channels[1] = pMgntInfo->dot11CurrentChannelNumber;
		CustomScanReq.nChannels = 2;
		CustomScanReq.ScanType = SCAN_ACTIVE;
		CustomScanReq.Duration = DEFAULT_PASSIVE_SCAN_PERIOD;

		RtActChannelList(Adapter, RT_CHNL_LIST_ACTION_CONSTRUCT_SCAN_LIST, &CustomScanReq, NULL);
	}

}
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID
odm_SwAntDivChkAntSwitchCallback(
	PRT_TIMER		pTimer
)
{
	PADAPTER		Adapter = (PADAPTER)pTimer->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	pSWAT_T		pDM_SWAT_Table = &pHalData->DM_OutSrc.DM_SWAT_Table;

	#if DEV_BUS_TYPE==RT_PCI_INTERFACE
	#if USE_WORKITEM
	ODM_ScheduleWorkItem(&pDM_SWAT_Table->SwAntennaSwitchWorkitem);
	#else
	odm_SwAntDivChkAntSwitch(&pHalData->DM_OutSrc, SWAW_STEP_DETERMINE);
	#endif
	#else
	ODM_ScheduleWorkItem(&pDM_SWAT_Table->SwAntennaSwitchWorkitem);
	#endif
	
}
VOID
odm_SwAntDivChkAntSwitchWorkitemCallback(
    IN PVOID            pContext
    )
{

	PADAPTER		pAdapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	odm_SwAntDivChkAntSwitch(&pHalData->DM_OutSrc, SWAW_STEP_DETERMINE);

}
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext)
{
	PDM_ODM_T	pDM_Odm= (PDM_ODM_T)FunctionContext;
	PADAPTER	padapter = pDM_Odm->Adapter;
	if(padapter->net_closed == _TRUE)
	    return;
	odm_SwAntDivChkAntSwitch(pDM_Odm, SWAW_STEP_DETERMINE);	
}
#elif (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext)
{
	PDM_ODM_T	pDM_Odm= (PDM_ODM_T)FunctionContext;
	odm_SwAntDivChkAntSwitch(pDM_Odm, SWAW_STEP_DETERMINE);
}
#endif

#else

VOID odm_SwAntDivInit(	IN		PDM_ODM_T		pDM_Odm	) {}
VOID ODM_SwAntDivChkPerPktRssi(
	IN PDM_ODM_T	pDM_Odm,
	IN u1Byte		StationID,
	IN PODM_PHY_INFO_T pPhyInfo
	) {}
VOID odm_SwAntDivChkAntSwitch(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			Step
	) {}
VOID ODM_SwAntDivResetBeforeLink(	IN		PDM_ODM_T		pDM_Odm	){}
VOID ODM_SwAntDivRestAfterLink(	IN		PDM_ODM_T		pDM_Odm	){}
VOID odm_SwAntDetectInit(	IN		PDM_ODM_T		pDM_Odm){}
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID odm_SwAntDivChkAntSwitchCallback(	PRT_TIMER		pTimer){}
VOID odm_SwAntDivChkAntSwitchWorkitemCallback(    IN PVOID            pContext    ){}
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext){}
#elif (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext){}
#endif

#endif



#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
BOOLEAN
ODM_SwAntDivCheckBeforeLink(
	IN		PDM_ODM_T		pDM_Odm
	)
{

#if (RT_MEM_SIZE_LEVEL != RT_MEM_SIZE_MINIMUM)

	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE*	pHalData = GET_HAL_DATA(Adapter);
	PMGNT_INFO		pMgntInfo = &Adapter->MgntInfo;
	pSWAT_T			pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	pFAT_T	pDM_FatTable = &pDM_Odm->DM_FatTable;
	s1Byte			Score = 0;
	PRT_WLAN_BSS	pTmpBssDesc, pTestBssDesc;
	s4Byte 			power_diff = 0, power_target = 10;
	u1Byte			index, counter = 0;
	static u1Byte		ScanChannel;
	u8Byte			tStamp_diff = 0;		


	if (pDM_Odm->Adapter == NULL) 
	{
		return FALSE;
	}

	if(!IS_ANT_DETECT_SUPPORT_RSSI(Adapter))
			return FALSE;

	PlatformAcquireSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	if(pHalData->eRFPowerState!=eRfOn || pMgntInfo->RFChangeInProgress || pMgntInfo->bMediaConnect)
	{
		PlatformReleaseSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
				("ODM_SwAntDivCheckBeforeLink(): RFChangeInProgress(%x), eRFPowerState(%x)\n", 
				pMgntInfo->RFChangeInProgress, pHalData->eRFPowerState));
	
		pDM_SWAT_Table->SWAS_NoLink_State = 0;
		
		return FALSE;
	}
	else
	{
		PlatformReleaseSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	}
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("pDM_SWAT_Table->SWAS_NoLink_State = %d\n", pDM_SWAT_Table->SWAS_NoLink_State));
	if(pDM_SWAT_Table->SWAS_NoLink_State == 0)
	{

		pDM_SWAT_Table->SWAS_NoLink_State = 1;
	
		pMgntInfo->tmpNumBssDesc = pMgntInfo->NumBssDesc;
		PlatformMoveMemory((PVOID)Adapter->MgntInfo.tmpbssDesc, (PVOID)pMgntInfo->bssDesc, sizeof(RT_WLAN_BSS)*MAX_BSS_DESC);
		
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SwAntDivCheckBeforeLink: Scan one more time\n"));
		pMgntInfo->ScanStep=0;
		pMgntInfo->bScanAntDetect = TRUE;
		ScanChannel = odm_SwAntDivSelectScanChnl(Adapter);

		
		if(pDM_Odm->SupportICType & (ODM_RTL8188E|ODM_RTL8821))
		{
			if(pDM_FatTable->RxIdleAnt == MAIN_ANT)
				ODM_UpdateRxIdleAnt(pDM_Odm, AUX_ANT);
			else
				ODM_UpdateRxIdleAnt(pDM_Odm, MAIN_ANT);
			if(ScanChannel == 0)
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
					("ODM_SwAntDivCheckBeforeLink(): No AP List Avaiable, Using Ant(%s)\n", (pDM_FatTable->RxIdleAnt==MAIN_ANT)?"AUX_ANT":"MAIN_ANT"));

				if(IS_5G_WIRELESS_MODE(pMgntInfo->dot11CurrentWirelessMode))
				{
					pDM_SWAT_Table->Ant5G = pDM_FatTable->RxIdleAnt;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("pDM_SWAT_Table->Ant5G=%s\n", (pDM_FatTable->RxIdleAnt==MAIN_ANT)?"MAIN_ANT":"AUX_ANT"));
				}
				else
				{
					pDM_SWAT_Table->Ant2G = pDM_FatTable->RxIdleAnt;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("pDM_SWAT_Table->Ant2G=%s\n", (pDM_FatTable->RxIdleAnt==MAIN_ANT)?"MAIN_ANT":"AUX_ANT"));
				}
				return FALSE;
			}

			ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
				("ODM_SwAntDivCheckBeforeLink: Change to %s for testing.\n", ((pDM_FatTable->RxIdleAnt == MAIN_ANT)?"MAIN_ANT":"AUX_ANT")));
		}
		else if(pDM_Odm->SupportICType & (ODM_RTL8192C|ODM_RTL8723B))
		{
			pDM_SWAT_Table->PreAntenna = pDM_SWAT_Table->CurAntenna;
			pDM_SWAT_Table->CurAntenna = (pDM_SWAT_Table->CurAntenna==MAIN_ANT)?AUX_ANT:MAIN_ANT;
			
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
				("ODM_SwAntDivCheckBeforeLink: Change to Ant(%s) for testing.\n", (pDM_SWAT_Table->CurAntenna==MAIN_ANT)?"MAIN":"AUX"));
			if(pDM_Odm->SupportICType == ODM_RTL8192C)
			{
				pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 = ((pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 & 0xfffffcff) | (pDM_SWAT_Table->CurAntenna<<8));
				ODM_SetBBReg(pDM_Odm,  rFPGA0_XA_RFInterfaceOE, bMaskDWord, pDM_SWAT_Table->SWAS_NoLink_BK_Reg860);
			}
			else if(pDM_Odm->SupportICType == ODM_RTL8723B)
			{
				pDM_SWAT_Table->SWAS_NoLink_BK_Reg92c = ((pDM_SWAT_Table->SWAS_NoLink_BK_Reg92c & 0xfffffffc) | (pDM_SWAT_Table->CurAntenna));
				ODM_SetBBReg(pDM_Odm,  rfe_ctrl_anta_src, 0xff, 0x77);
				ODM_SetBBReg(pDM_Odm,  rDPDT_control, bMaskDWord, pDM_SWAT_Table->SWAS_NoLink_BK_Reg92c);
			}
		}
		
		odm_SwAntDivConstructScanChnl(Adapter, ScanChannel);
		PlatformSetTimer(Adapter, &pMgntInfo->ScanTimer, 5);

		return TRUE;
	}
	else
	{

		for(index = 0; index < pMgntInfo->tmpNumBssDesc; index++)
		{
			pTmpBssDesc = &(pMgntInfo->tmpbssDesc[index]);
			pTestBssDesc = &(pMgntInfo->bssDesc[index]);

			if(PlatformCompareMemory(pTestBssDesc->bdBssIdBuf, pTmpBssDesc->bdBssIdBuf, 6)!=0)
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SwAntDivCheckBeforeLink(): ERROR!! This shall not happen.\n"));
				continue;
			}

			if(pDM_Odm->SupportICType != ODM_RTL8723B)
			{
				if(pTmpBssDesc->ChannelNumber == ScanChannel)
				{
			if(pTmpBssDesc->RecvSignalPower > pTestBssDesc->RecvSignalPower)
			{
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SwAntDivCheckBeforeLink: Compare scan entry: Score++\n"));
						RT_PRINT_STR(COMP_SCAN, DBG_WARNING, "GetScanInfo(): new Bss SSID:", pTmpBssDesc->bdSsIdBuf, pTmpBssDesc->bdSsIdLen);
						ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("at ch %d, Original: %d, Test: %d\n\n", pTmpBssDesc->ChannelNumber, pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
			
				Score++;
				PlatformMoveMemory(pTestBssDesc, pTmpBssDesc, sizeof(RT_WLAN_BSS));
			}
			else if(pTmpBssDesc->RecvSignalPower < pTestBssDesc->RecvSignalPower)
			{
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SwAntDivCheckBeforeLink: Compare scan entry: Score--\n"));
						RT_PRINT_STR(COMP_SCAN, DBG_WARNING, "GetScanInfo(): new Bss SSID:", pTmpBssDesc->bdSsIdBuf, pTmpBssDesc->bdSsIdLen);
						ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("at ch %d, Original: %d, Test: %d\n\n", pTmpBssDesc->ChannelNumber, pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
				Score--;
			}
					else
					{
						if(pTestBssDesc->bdTstamp - pTmpBssDesc->bdTstamp < 5000)
						{
							RT_PRINT_STR(COMP_SCAN, DBG_WARNING, "GetScanInfo(): new Bss SSID:", pTmpBssDesc->bdSsIdBuf, pTmpBssDesc->bdSsIdLen);
							ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("at ch %d, Original: %d, Test: %d\n", pTmpBssDesc->ChannelNumber, pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
							ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("The 2nd Antenna didn't get this AP\n\n"));
						}
					}
				}
			}
			else
			{ 
				if(pTmpBssDesc->ChannelNumber == ScanChannel)
				{
					if(pTmpBssDesc->RecvSignalPower > pTestBssDesc->RecvSignalPower)
					{
						counter++;
						power_diff = power_diff + (pTmpBssDesc->RecvSignalPower - pTestBssDesc->RecvSignalPower); 
						ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Original: %d, Test: %d\n", pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
						ODM_PRINT_ADDR(pDM_Odm,ODM_COMP_ANT_DIV, DBG_LOUD, ("SSID:"), pTmpBssDesc->bdSsIdBuf);
						ODM_PRINT_ADDR(pDM_Odm,ODM_COMP_ANT_DIV, DBG_LOUD, ("BSSID:"), pTmpBssDesc->bdBssIdBuf);
						PlatformMoveMemory(pTestBssDesc, pTmpBssDesc, sizeof(RT_WLAN_BSS));
					}
					else if(pTestBssDesc->RecvSignalPower > pTmpBssDesc->RecvSignalPower)
					{
						counter++;
						power_diff = power_diff + (pTestBssDesc->RecvSignalPower - pTmpBssDesc->RecvSignalPower);
						ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Original: %d, Test: %d\n", pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
						ODM_PRINT_ADDR(pDM_Odm,ODM_COMP_ANT_DIV, DBG_LOUD, ("SSID:"), pTmpBssDesc->bdSsIdBuf);
						ODM_PRINT_ADDR(pDM_Odm,ODM_COMP_ANT_DIV, DBG_LOUD, ("BSSID:"), pTmpBssDesc->bdBssIdBuf)
					}
					else if(pTestBssDesc->bdTstamp > pTmpBssDesc->bdTstamp)
					{
						ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("time_diff: %lld\n", (pTestBssDesc->bdTstamp-pTmpBssDesc->bdTstamp)/1000));
						if(pTestBssDesc->bdTstamp - pTmpBssDesc->bdTstamp > 5000)
						{
							counter++;
							ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Original: %d, Test: %d\n", pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
							ODM_PRINT_ADDR(pDM_Odm,ODM_COMP_ANT_DIV, DBG_LOUD, ("SSID:"), pTmpBssDesc->bdSsIdBuf);
							ODM_PRINT_ADDR(pDM_Odm,ODM_COMP_ANT_DIV, DBG_LOUD, ("BSSID:"), pTmpBssDesc->bdBssIdBuf)
						}
					}
				}
			}
		}

		if(pDM_Odm->SupportICType == ODM_RTL8723B)
		{ 
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("counter: %d power_diff: %d\n", counter, power_diff));

			if(counter != 0)
				power_diff = power_diff / counter;

			if(power_diff <= power_target && counter != 0) 
				Score++;
		}

		if(pDM_Odm->SupportICType & (ODM_RTL8188E|ODM_RTL8821))
		{
			if(pMgntInfo->NumBssDesc!=0 && Score<0)
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,
							("ODM_SwAntDivCheckBeforeLink(): Using Ant(%s)\n", (pDM_FatTable->RxIdleAnt==MAIN_ANT)?"MAIN_ANT":"AUX_ANT"));
			}
			else
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
						("ODM_SwAntDivCheckBeforeLink(): Remain Ant(%s)\n", (pDM_FatTable->RxIdleAnt==MAIN_ANT)?"AUX_ANT":"MAIN_ANT"));

				if(pDM_FatTable->RxIdleAnt == MAIN_ANT)
					ODM_UpdateRxIdleAnt(pDM_Odm, AUX_ANT);
				else
					ODM_UpdateRxIdleAnt(pDM_Odm, MAIN_ANT);
			}
			
			if(IS_5G_WIRELESS_MODE(pMgntInfo->dot11CurrentWirelessMode))
			{
				pDM_SWAT_Table->Ant5G = pDM_FatTable->RxIdleAnt;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("pDM_SWAT_Table->Ant5G=%s\n", (pDM_FatTable->RxIdleAnt==MAIN_ANT)?"MAIN_ANT":"AUX_ANT"));
			}
			else
			{
				pDM_SWAT_Table->Ant2G = pDM_FatTable->RxIdleAnt;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("pDM_SWAT_Table->Ant2G=%s\n", (pDM_FatTable->RxIdleAnt==MAIN_ANT)?"MAIN_ANT":"AUX_ANT"));
			}
		}
		else if(pDM_Odm->SupportICType == ODM_RTL8723B)
		{
			pDM_SWAT_Table->CurAntenna = pDM_SWAT_Table->PreAntenna;
			pDM_SWAT_Table->SWAS_NoLink_BK_Reg92c = ((pDM_SWAT_Table->SWAS_NoLink_BK_Reg92c & 0xfffffffc) | (pDM_SWAT_Table->CurAntenna));
			ODM_SetBBReg(pDM_Odm,  rfe_ctrl_anta_src, 0xff, 0x77);
			ODM_SetBBReg(pDM_Odm,  rDPDT_control,bMaskDWord, pDM_SWAT_Table->SWAS_NoLink_BK_Reg92c);

			if(counter != 0)
			{
				if(pMgntInfo->NumBssDesc != 0 && Score > 0)
				{
					if(pDM_Odm->DM_SWAT_Table.ANTB_ON == FALSE)
					{
						pDM_Odm->DM_SWAT_Table.ANTA_ON = TRUE;
						pDM_Odm->DM_SWAT_Table.ANTB_ON = TRUE;
					}
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("ODM_SwAntDivCheckBeforeLink(): Dual antenna\n"));
				}
				else
				{
					if(pDM_Odm->DM_SWAT_Table.ANTB_ON == TRUE)
					{
						pDM_Odm->DM_SWAT_Table.ANTA_ON = TRUE;
						pDM_Odm->DM_SWAT_Table.ANTB_ON = FALSE;
						BT_SetBtCoexAntNum(Adapter, BT_COEX_ANT_TYPE_DETECTED, 1);
					}
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SwAntDivCheckBeforeLink(): Single antenna\n"));
				}
			}
			else
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("ODM_SwAntDivCheckBeforeLink(): Igone result\n"));
		}
		else if(pDM_Odm->SupportICType == ODM_RTL8192C)
		{
			if(pMgntInfo->NumBssDesc!=0 && Score<=0)
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,
					("ODM_SwAntDivCheckBeforeLink(): Using Ant(%s)\n", (pDM_SWAT_Table->CurAntenna==MAIN_ANT)?"MAIN":"AUX"));

				pDM_SWAT_Table->PreAntenna = pDM_SWAT_Table->CurAntenna;
			}
			else
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
					("ODM_SwAntDivCheckBeforeLink(): Remain Ant(%s)\n", (pDM_SWAT_Table->CurAntenna==MAIN_ANT)?"AUX":"MAIN"));

				pDM_SWAT_Table->CurAntenna = pDM_SWAT_Table->PreAntenna;

				pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 = ((pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 & 0xfffffcff) | (pDM_SWAT_Table->CurAntenna<<8));
				PHY_SetBBReg(Adapter, rFPGA0_XA_RFInterfaceOE, bMaskDWord, pDM_SWAT_Table->SWAS_NoLink_BK_Reg860);
			}
		}
		
		pDM_SWAT_Table->SWAS_NoLink_State = 0;
		pMgntInfo->bScanAntDetect = FALSE;

		return FALSE;
	}

#else
		return	FALSE;
#endif

return FALSE;
}

#endif



#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))
VOID
odm_InitHybridAntDiv_88C_92D(
	IN PDM_ODM_T	pDM_Odm 
	)
{

#if((DM_ODM_SUPPORT_TYPE==ODM_AP)||(DM_ODM_SUPPORT_TYPE==ODM_ADSL))
	struct rtl8192cd_priv *priv=pDM_Odm->priv;
#endif
	SWAT_T			*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	u1Byte                  bTxPathSel=0;	       
	u1Byte			i;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_InitHybridAntDiv==============>\n"));

#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
	if(priv==NULL)	return;
	if(!priv->pshare->rf_ft_var.antHw_enable)
		return;	
	
	#ifdef SW_ANT_SWITCH
	priv->pshare->rf_ft_var.antSw_enable =0;
	#endif
#endif

	if((pDM_Odm->SupportICType != ODM_RTL8192C) && (pDM_Odm->SupportICType != ODM_RTL8192D))
		return;


	bTxPathSel=(pDM_Odm->RFType==ODM_1T1R)?FALSE:TRUE;

	ODM_SetBBReg(pDM_Odm,ODM_REG_BB_PWR_SAV1_11N, BIT23, 0);
	ODM_SetBBReg(pDM_Odm,ODM_REG_TX_ANT_CTRL_11N, BIT21, 1);
	ODM_SetBBReg(pDM_Odm,ODM_REG_ANTSEL_PIN_11N, BIT23, 1);
	ODM_SetBBReg(pDM_Odm,ODM_REG_ANTSEL_CTRL_11N, BIT8|BIT9, 0x01);
	#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
	ODM_Write2Byte(pDM_Odm,ODM_REG_RF_PIN_11N, (ODM_Read2Byte(pDM_Odm,0x804)&0xf0ff )| BIT(8) );
	#endif
	
	if(!bTxPathSel){                
		ODM_SetBBReg(pDM_Odm,ODM_REG_PIN_CTRL_11N, BIT8|BIT9, 0 );
		ODM_SetBBReg(pDM_Odm,ODM_REG_ANTSEL_PATH_11N, BIT13, 1);	
	}
	else	{
		ODM_SetBBReg(pDM_Odm,ODM_REG_PIN_CTRL_11N, BIT24|BIT25, 0 );
		ODM_SetBBReg(pDM_Odm,ODM_REG_ANTSEL_PATH_11N, BIT13, 0);		
	}

	ODM_SetBBReg(pDM_Odm,ODM_REG_ANTDIV_PARA1_11N, 0x7FF, 0x0c0);
	ODM_SetBBReg(pDM_Odm,ODM_REG_ANTDIV_PARA1_11N, BIT11, 0);
	ODM_SetBBReg(pDM_Odm,ODM_REG_ANTDIV_PARA3_11N, BIT23, 1);
	
	ODM_SetBBReg(pDM_Odm,ODM_REG_CCK_ANTDIV_PARA2_11N, BIT4, 0);
	ODM_SetBBReg(pDM_Odm,ODM_REG_CCK_ANTDIV_PARA2_11N, 0xf, 0xf);
	ODM_SetBBReg(pDM_Odm,ODM_REG_CCK_ANTDIV_PARA3_11N, BIT13, 1);
	ODM_SetBBReg(pDM_Odm,ODM_REG_CCK_ANTDIV_PARA4_11N, 0x1f, 0x8);


	if(!bTxPathSel)                
		ODM_SetBBReg(pDM_Odm,ODM_REG_IGI_A_11N, BIT7,1);
	else
		ODM_SetBBReg(pDM_Odm,ODM_REG_IGI_B_11N, BIT7,1);
	ODM_SetBBReg(pDM_Odm,ODM_REG_CCK_ANTDIV_PARA1_11N, BIT15, 1);//Enable antenna diversity

	pDM_SWAT_Table->CurAntenna=0;		
	pDM_SWAT_Table->PreAntenna=0;
	for(i=0; i<ASSOCIATE_ENTRY_NUM ; i++)
	{
		pDM_SWAT_Table->CCK_Ant1_Cnt[i] = 0;
		pDM_SWAT_Table->CCK_Ant2_Cnt[i] = 0;
		pDM_SWAT_Table->OFDM_Ant1_Cnt[i] = 0;
		pDM_SWAT_Table->OFDM_Ant2_Cnt[i] = 0;
		pDM_SWAT_Table->RSSI_Ant1_Sum[i] = 0;
		pDM_SWAT_Table->RSSI_Ant2_Sum[i] = 0;
	}
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("<==============odm_InitHybridAntDiv\n"));
}


VOID
odm_InitHybridAntDiv(
	IN PDM_ODM_T	pDM_Odm 
	)
{
	if(!(pDM_Odm->SupportAbility & ODM_BB_ANT_DIV))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("Return: Not Support HW AntDiv\n"));
		return;
	}
	
	if(pDM_Odm->SupportICType & (ODM_RTL8192C | ODM_RTL8192D))
	{
#if ((RTL8192C_SUPPORT == 1)||(RTL8192D_SUPPORT == 1))
		odm_InitHybridAntDiv_88C_92D(pDM_Odm);
#endif
	}
}


BOOLEAN
odm_StaDefAntSel(
	IN PDM_ODM_T	pDM_Odm,
	IN u4Byte		OFDM_Ant1_Cnt,
	IN u4Byte		OFDM_Ant2_Cnt,
	IN u4Byte		CCK_Ant1_Cnt,
	IN u4Byte		CCK_Ant2_Cnt,
	OUT u1Byte		*pDefAnt 

	)
{
#if 1
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_StaDefAntSelect==============>\n"));

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("OFDM_Ant1_Cnt:%d, OFDM_Ant2_Cnt:%d\n",OFDM_Ant1_Cnt,OFDM_Ant2_Cnt));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("CCK_Ant1_Cnt:%d, CCK_Ant2_Cnt:%d\n",CCK_Ant1_Cnt,CCK_Ant2_Cnt));

	
	if(((OFDM_Ant1_Cnt+OFDM_Ant2_Cnt)==0)&&((CCK_Ant1_Cnt + CCK_Ant2_Cnt) <10)){
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_StaDefAntSelect Fail: No enough packet info!\n"));
		return	FALSE;
	}

	if(OFDM_Ant1_Cnt || OFDM_Ant2_Cnt )	{
		if(OFDM_Ant1_Cnt > OFDM_Ant2_Cnt)
			(*pDefAnt)=1;
		else
			(*pDefAnt)=0;
	}
	else if((CCK_Ant1_Cnt + CCK_Ant2_Cnt) >=10 )
	{
		if(CCK_Ant1_Cnt > (5*CCK_Ant2_Cnt))
			(*pDefAnt)=1;
		else if(CCK_Ant2_Cnt > (5*CCK_Ant1_Cnt))
			(*pDefAnt)=0;
		else if(CCK_Ant1_Cnt > CCK_Ant2_Cnt)
			(*pDefAnt)=0;
		else
			(*pDefAnt)=1;

	}

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("TxAnt = %s\n",((*pDefAnt)==1)?"Ant1":"Ant2"));
	
#endif
	


	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("<==============odm_StaDefAntSelect\n"));

	return TRUE;

	
}


VOID
odm_SetRxIdleAnt(
	IN	PDM_ODM_T	pDM_Odm,
	IN	u1Byte	Ant,
	IN   BOOLEAN   bDualPath                     
)
{
	SWAT_T			*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;


	if(Ant != pDM_SWAT_Table->RxIdleAnt)
	{
	if(Ant==1) 
			ODM_SetBBReg(pDM_Odm,ODM_REG_RX_DEFUALT_A_11N, 0xFFFF, 0x65a9);  
	else
			ODM_SetBBReg(pDM_Odm,ODM_REG_RX_DEFUALT_A_11N, 0xFFFF, 0x569a);  

	if(bDualPath){
		if(Ant==0) 
				ODM_SetBBReg(pDM_Odm,ODM_REG_RX_DEFUALT_A_11N, 0xFFFF0000, 0x65a9);  
		else 
				ODM_SetBBReg(pDM_Odm,ODM_REG_RX_DEFUALT_A_11N, 0xFFFF0000, 0x569a); 
		}
	}
		pDM_SWAT_Table->RxIdleAnt = Ant;
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("RxIdleAnt: %s  Reg858=0x%x\n",(Ant==1)?"Ant1":"Ant2",(Ant==1)?0x65a9:0x569a));


	}
		
VOID
ODM_AntselStatistics_88C(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			MacId,
	IN		u4Byte			PWDBAll,
	IN		BOOLEAN			isCCKrate
)
{
	SWAT_T			*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;

	if(pDM_SWAT_Table->antsel == 1)
	{
		if(isCCKrate)
			pDM_SWAT_Table->CCK_Ant1_Cnt[MacId]++;
		else
		{
			pDM_SWAT_Table->OFDM_Ant1_Cnt[MacId]++;
			pDM_SWAT_Table->RSSI_Ant1_Sum[MacId] += PWDBAll;
		}
	}
	else
	{
		if(isCCKrate)
			pDM_SWAT_Table->CCK_Ant2_Cnt[MacId]++;
		else
		{
			pDM_SWAT_Table->OFDM_Ant2_Cnt[MacId]++;
			pDM_SWAT_Table->RSSI_Ant2_Sum[MacId] += PWDBAll;
		}
	}

}




#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
VOID
ODM_SetTxAntByTxInfo_88C_92D(
	IN		PDM_ODM_T		pDM_Odm,
	IN		pu1Byte			pDesc,
	IN		u1Byte			macId	
)
{
	SWAT_T			*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	u1Byte			antsel;

	if(!(pDM_Odm->SupportAbility&ODM_BB_ANT_DIV)) 
		return;

	if(pDM_SWAT_Table->RxIdleAnt == 1)
		antsel=(pDM_SWAT_Table->TxAnt[macId] == 1)?0:1;
	else
		antsel=(pDM_SWAT_Table->TxAnt[macId] == 1)?1:0;
	
	SET_TX_DESC_ANTSEL_A_92C(pDesc, antsel);
}
#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
VOID
ODM_SetTxAntByTxInfo_88C_92D(
	IN		PDM_ODM_T		pDM_Odm
)
{

}
#elif(DM_ODM_SUPPORT_TYPE==ODM_AP)
VOID
ODM_SetTxAntByTxInfo_88C_92D(
	IN		PDM_ODM_T		pDM_Odm
)
{

}
#endif

VOID
odm_HwAntDiv_92C_92D(
	IN	PDM_ODM_T	pDM_Odm
)
{
	SWAT_T			*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	u4Byte			RSSI_Min=0xFF, RSSI, RSSI_Ant1, RSSI_Ant2;
	u1Byte			RxIdleAnt, i;
	BOOLEAN		bRet=FALSE;
	PSTA_INFO_T   	pEntry;
	
#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
	struct rtl8192cd_priv *priv=pDM_Odm->priv;
	if(priv->pshare->rf_ft_var.CurAntenna & 0x80)
		return;	
#endif	

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_HwAntDiv==============>\n"));
	
	if(!(pDM_Odm->SupportAbility&ODM_BB_ANT_DIV))                                   
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_HwAntDiv: Not supported!\n"));
		return;
	}

	if((pDM_Odm->SupportICType != ODM_RTL8192C) && (pDM_Odm->SupportICType != ODM_RTL8192D))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("Return: IC Type is not 92C or 92D\n"));
		return;
	}
	
#if (DM_ODM_SUPPORT_TYPE&(ODM_WIN|ODM_CE))
	if(!pDM_Odm->bLinked)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("Return: bLinked is FALSE\n"));
		return;
	}
#endif

	for (i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		pEntry = pDM_Odm->pODM_StaInfo[i];
		if(IS_STA_VALID(pEntry))
		{

			RSSI_Ant1 = (pDM_SWAT_Table->OFDM_Ant1_Cnt[i] == 0)?0:(pDM_SWAT_Table->RSSI_Ant1_Sum[i]/pDM_SWAT_Table->OFDM_Ant1_Cnt[i]);
			RSSI_Ant2 = (pDM_SWAT_Table->OFDM_Ant2_Cnt[i] == 0)?0:(pDM_SWAT_Table->RSSI_Ant2_Sum[i]/pDM_SWAT_Table->OFDM_Ant2_Cnt[i]);

			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("RSSI_Ant1=%d,  RSSI_Ant2=%d\n", RSSI_Ant1, RSSI_Ant2));
		
			if(RSSI_Ant1 ||RSSI_Ant2) 
			{
#if (DM_ODM_SUPPORT_TYPE==ODM_AP)		
				if(pDM_Odm->pODM_StaInfo[i]->expire_to)
#endif
				{
					RSSI = (RSSI_Ant1 < RSSI_Ant2) ? RSSI_Ant1 : RSSI_Ant2;
					if((!RSSI) || ( RSSI < RSSI_Min) ) {
						pDM_SWAT_Table->TargetSTA = i;
						RSSI_Min = RSSI;
					}
				}
	}
			bRet=odm_StaDefAntSel(pDM_Odm, 
						 pDM_SWAT_Table->OFDM_Ant1_Cnt[i], 
						 pDM_SWAT_Table->OFDM_Ant2_Cnt[i], 
						 pDM_SWAT_Table->CCK_Ant1_Cnt[i], 
						 pDM_SWAT_Table->CCK_Ant2_Cnt[i], 
						 &pDM_SWAT_Table->TxAnt[i]);

			if(bRet){	
				pDM_SWAT_Table->RSSI_Ant1_Sum[i] = 0;
				pDM_SWAT_Table->RSSI_Ant2_Sum[i] = 0;
				pDM_SWAT_Table->OFDM_Ant1_Cnt[i] = 0;
				pDM_SWAT_Table->OFDM_Ant2_Cnt[i] = 0; 
				pDM_SWAT_Table->CCK_Ant1_Cnt[i] = 0; 
				pDM_SWAT_Table->CCK_Ant2_Cnt[i] = 0; 
			}
		}
	}
	
	RxIdleAnt = pDM_SWAT_Table->TxAnt[pDM_SWAT_Table->TargetSTA];
	odm_SetRxIdleAnt(pDM_Odm, RxIdleAnt, FALSE);

#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
#ifdef TX_SHORTCUT
	if (!priv->pmib->dot11OperationEntry.disable_txsc) {
		plist = phead->next;
		while(plist != phead)	{
			pstat = list_entry(plist, struct stat_info, asoc_list);
			if(pstat->expire_to) {
				for (i=0; i<TX_SC_ENTRY_NUM; i++) {
					struct tx_desc *pdesc= &(pstat->tx_sc_ent[i].hwdesc1);	
					pdesc->Dword2 &= set_desc(~ (BIT(24)|BIT(25)));
					if((pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)
						pdesc->Dword2 |= set_desc(BIT(24)|BIT(25));
					pdesc= &(pstat->tx_sc_ent[i].hwdesc2);	
					pdesc->Dword2 &= set_desc(~ (BIT(24)|BIT(25)));
					if((pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)
						pdesc->Dword2 |= set_desc(BIT(24)|BIT(25));					
				}
			}		

			if (plist == plist->next)
				break;
			plist = plist->next;
		};
	}
#endif	
#endif
	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("<==============odm_HwAntDiv\n"));
	
}

VOID
odm_HwAntDiv(
	IN	PDM_ODM_T	pDM_Odm
)
{	

	PADAPTER		pAdapter	= pDM_Odm->Adapter;

#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
	if(pAdapter->MgntInfo.AntennaTest)
		return;
#endif
	if(!(pDM_Odm->SupportAbility & ODM_BB_ANT_DIV))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("Return: Not Support HW AntDiv\n"));
		return;
	}
	
	if(pDM_Odm->SupportICType & (ODM_RTL8192C | ODM_RTL8192D))
	{
#if ((RTL8192C_SUPPORT == 1)||(RTL8192D_SUPPORT == 1))
		odm_HwAntDiv_92C_92D(pDM_Odm);
#endif
	}
}


#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
#if 0
VOID
odm_HwAntDiv(
	IN	PDM_ODM_T	pDM_Odm
)
{
	struct rtl8192cd_priv *priv=pDM_Odm->priv;
	struct stat_info	*pstat, *pstat_min=NULL;
	struct list_head	*phead, *plist;
	int rssi_min= 0xff, i;
	u1Byte	idleAnt=priv->pshare->rf_ft_var.CurAntenna;	
	u1Byte	nextAnt;
	BOOLEAN		bRet=FALSE;
	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_HwAntDiv==============>\n"));

	if((!priv->pshare->rf_ft_var.antHw_enable) ||(!(pDM_Odm->SupportAbility & ODM_BB_ANT_DIV)))
		return;
	
	if(priv->pshare->rf_ft_var.CurAntenna & 0x80)
		return;
	
	phead = &priv->asoc_list;
	plist = phead->next;
	while(plist != phead)	{
		pstat = list_entry(plist, struct stat_info, asoc_list);
		if((pstat->expire_to) && (pstat->AntRSSI[0] || pstat->AntRSSI[1])) {
			int rssi = (pstat->AntRSSI[0] < pstat->AntRSSI[1]) ? pstat->AntRSSI[0] : pstat->AntRSSI[1];
			if((!pstat_min) || ( rssi < rssi_min) ) {
				pstat_min = pstat;
				rssi_min = rssi;
			}
		}
		bRet=odm_StaDefAntSel(pDM_Odm,
						pstat->hwRxAntSel[1],
						pstat->hwRxAntSel[0],
						pstat->cckPktCount[1],
						pstat->cckPktCount[0],
						&nextAnt
						);
		
		if(bRet){	
			pstat->CurAntenna = nextAnt;
			for(i=0; i<2; i++) {
				if(pstat->cckPktCount[i]==0 && pstat->hwRxAntSel[i]==0)
					pstat->AntRSSI[i] = 0;
			}
			if(pstat->AntRSSI[idleAnt]==0)
				pstat->AntRSSI[idleAnt] = pstat->AntRSSI[idleAnt^1];
			pstat->hwRxAntSel[1] = pstat->hwRxAntSel[0] =0;
			pstat->cckPktCount[1]= pstat->cckPktCount[0] =0;
		}

		if (plist == plist->next)
			break;
		plist = plist->next;
		
	};
	if(pstat_min)	{
		if(priv->pshare->rf_ft_var.CurAntenna!=pstat_min->CurAntenna)
			odm_SetRxIdleAnt(pDM_Odm,pstat_min->CurAntenna,TRUE);
		priv->pshare->rf_ft_var.CurAntenna = pstat_min->CurAntenna;
	}


#ifdef TX_SHORTCUT
	if (!priv->pmib->dot11OperationEntry.disable_txsc) {
		plist = phead->next;
		while(plist != phead)	{
			pstat = list_entry(plist, struct stat_info, asoc_list);
			if(pstat->expire_to) {
				for (i=0; i<TX_SC_ENTRY_NUM; i++) {
					struct tx_desc *pdesc= &(pstat->tx_sc_ent[i].hwdesc1);	
					pdesc->Dword2 &= set_desc(~ (BIT(24)|BIT(25)));
					if((pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)
						pdesc->Dword2 |= set_desc(BIT(24)|BIT(25));
					pdesc= &(pstat->tx_sc_ent[i].hwdesc2);	
					pdesc->Dword2 &= set_desc(~ (BIT(24)|BIT(25)));
					if((pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)
						pdesc->Dword2 |= set_desc(BIT(24)|BIT(25));					
				}
			}		

			if (plist == plist->next)
				break;
			plist = plist->next;
		};
	}
#endif	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,"<==============odm_HwAntDiv\n");
}
#endif

u1Byte
ODM_Diversity_AntennaSelect(
	IN	PDM_ODM_T	pDM_Odm,
	IN	u1Byte	*data
)
{
	struct rtl8192cd_priv *priv=pDM_Odm->priv;

	int ant = _atoi(data, 16);

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("ODM_Diversity_AntennaSelect==============>\n"));

	#ifdef PCIE_POWER_SAVING
	PCIeWakeUp(priv, POWER_DOWN_T0);
	#endif

	if (ant==AUX_ANT || ant==MAIN_ANT) 
	{
		if ( !priv->pshare->rf_ft_var.antSw_select) {
			ODM_Write4Byte(pDM_Odm,0x870, ODM_Read4Byte(pDM_Odm,0x870) | BIT(8)| BIT(9) ); 
			ODM_Write1Byte(pDM_Odm,0xc50, ODM_Read1Byte(pDM_Odm,0xc50) & (~ BIT(7)));
			PHY_SetBBReg(priv, 0x860, 0x300, ant);
		} else {
			ODM_Write4Byte(pDM_Odm,0x870, ODM_Read4Byte(pDM_Odm,0x870) | BIT(24)| BIT(25) );
			PHY_SetBBReg(priv, 0x864, 0x300, ant);
			ODM_Write1Byte(pDM_Odm,0xc58, ODM_Read1Byte(pDM_Odm,0xc58) & (~ BIT(7)));	
		}

		ODM_Write1Byte(pDM_Odm,0xa01, ODM_Read1Byte(pDM_Odm,0xa01) & (~ BIT(7)));
		ODM_Write4Byte(pDM_Odm,0x80c, ODM_Read4Byte(pDM_Odm,0x80c) & (~ BIT(21)));
		ODM_Write4Byte(pDM_Odm,0x858, 0x569a569a);

		priv->pshare->rf_ft_var.antHw_enable = 0;
		priv->pshare->rf_ft_var.CurAntenna  = (ant%2);

		#ifdef SW_ANT_SWITCH
		priv->pshare->rf_ft_var.antSw_enable = 0;
		priv->pshare->DM_SWAT_Table.CurAntenna = ant;
		priv->pshare->RSSI_test =0;
		#endif
	}
	else if(ant==0){

		if ( !priv->pshare->rf_ft_var.antSw_select)  {
			ODM_Write4Byte(pDM_Odm,0x870, ODM_Read4Byte(pDM_Odm,0x870) & ~(BIT(8)| BIT(9)) );
			ODM_Write1Byte(pDM_Odm,0xc50, ODM_Read1Byte(pDM_Odm,0xc50) | BIT(7));
		} else {
			ODM_Write4Byte(pDM_Odm,0x870, ODM_Read4Byte(pDM_Odm,0x870) & ~(BIT(24)| BIT(25)) );
			ODM_Write1Byte(pDM_Odm,0xc58, ODM_Read1Byte(pDM_Odm,0xc58) | BIT(7));
		}

		ODM_Write1Byte(pDM_Odm,0xa01, ODM_Read1Byte(pDM_Odm,0xa01) | BIT(7));
		ODM_Write4Byte(pDM_Odm,0x80c, ODM_Read4Byte(pDM_Odm,0x80c) | BIT(21) );
		priv->pshare->rf_ft_var.CurAntenna = 0;
		ODM_Write4Byte(pDM_Odm,0x858, 0x569a569a);
		priv->pshare->rf_ft_var.antHw_enable = 1;
#ifdef SW_ANT_SWITCH
		priv->pshare->rf_ft_var.antSw_enable = 0;
		priv->pshare->RSSI_test =0;
#endif
	}
#ifdef SW_ANT_SWITCH
	else if(ant==3) {
		if(!priv->pshare->rf_ft_var.antSw_enable) {
			
			dm_SW_AntennaSwitchInit(priv);
			ODM_Write4Byte(pDM_Odm,0x858, 0x569a569a);
			priv->pshare->lastTxOkCnt = priv->net_stats.tx_bytes;
			priv->pshare->lastRxOkCnt = priv->net_stats.rx_bytes;
		}
		if ( !priv->pshare->rf_ft_var.antSw_select)
			ODM_Write1Byte(pDM_Odm,0xc50, ODM_Read1Byte(pDM_Odm,0xc50) & (~ BIT(7)));
		else
			ODM_Write1Byte(pDM_Odm,0xc58, ODM_Read1Byte(pDM_Odm,0xc58) & (~ BIT(7)));

		ODM_Write1Byte(pDM_Odm,0xa01, ODM_Read1Byte(pDM_Odm,0xa01) & (~ BIT(7)));	
		ODM_Write4Byte(pDM_Odm,0x80c, ODM_Read4Byte(pDM_Odm,0x80c) & (~ BIT(21))); 
		priv->pshare->rf_ft_var.antHw_enable = 0;
		priv->pshare->rf_ft_var.antSw_enable = 1;

	}
#endif
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("<==============ODM_Diversity_AntennaSelect\n"));

	return 1;
}
#endif

#else

VOID odm_InitHybridAntDiv(	IN PDM_ODM_T	pDM_Odm 	){}
VOID odm_HwAntDiv(	IN	PDM_ODM_T	pDM_Odm){}
#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
VOID ODM_SetTxAntByTxInfo_88C_92D(
	IN		PDM_ODM_T		pDM_Odm,
	IN		pu1Byte			pDesc,
	IN		u1Byte			macId	
){}
#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
VOID ODM_SetTxAntByTxInfo_88C_92D(	IN		PDM_ODM_T		pDM_Odm){ }
#elif(DM_ODM_SUPPORT_TYPE==ODM_AP)
VOID ODM_SetTxAntByTxInfo_88C_92D(	IN		PDM_ODM_T		pDM_Odm){ }
#endif

#endif



VOID
ODM_EdcaTurboInit(
	IN    PDM_ODM_T		pDM_Odm)
{

#if ((DM_ODM_SUPPORT_TYPE == ODM_AP)||(DM_ODM_SUPPORT_TYPE==ODM_ADSL))
	odm_EdcaParaInit(pDM_Odm);
#elif (DM_ODM_SUPPORT_TYPE==ODM_WIN)
	PADAPTER	Adapter = NULL;
	HAL_DATA_TYPE	*pHalData = NULL;

	if(pDM_Odm->Adapter==NULL)	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("EdcaTurboInit fail!!!\n"));
		return;
	}

	Adapter=pDM_Odm->Adapter;
	pHalData=GET_HAL_DATA(Adapter);

	pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = FALSE;	
	pDM_Odm->DM_EDCA_Table.bIsCurRDLState = FALSE;
	pHalData->bIsAnyNonBEPkts = FALSE;
	
#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
	PADAPTER	Adapter = pDM_Odm->Adapter;	
	pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = FALSE;	
	pDM_Odm->DM_EDCA_Table.bIsCurRDLState = FALSE;
	Adapter->recvpriv.bIsAnyNonBEPkts =FALSE;

#endif	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial VO PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_VO_PARAM)));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial VI PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_VI_PARAM)));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial BE PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_BE_PARAM)));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial BK PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_BK_PARAM)));

	
}

VOID
odm_EdcaTurboCheck(
	IN		PDM_ODM_T		pDM_Odm
	)
{

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("odm_EdcaTurboCheck========================>\n"));

	if(!(pDM_Odm->SupportAbility& ODM_MAC_EDCA_TURBO ))
		return;

	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:

#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
			odm_EdcaTurboCheckMP(pDM_Odm);
#endif
			break;

		case	ODM_CE:
#if(DM_ODM_SUPPORT_TYPE==ODM_CE)
			odm_EdcaTurboCheckCE(pDM_Odm);
#endif
			break;

		case	ODM_AP:
		case	ODM_ADSL:

#if ((DM_ODM_SUPPORT_TYPE == ODM_AP)||(DM_ODM_SUPPORT_TYPE==ODM_ADSL))
		odm_IotEngine(pDM_Odm);
#endif
			break;	
	}
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("<========================odm_EdcaTurboCheck\n"));

}

#if(DM_ODM_SUPPORT_TYPE==ODM_CE)


VOID
odm_EdcaTurboCheckCE(
	IN		PDM_ODM_T		pDM_Odm
	)
{

#if(DM_ODM_SUPPORT_TYPE==ODM_CE)

	PADAPTER		       Adapter = pDM_Odm->Adapter;
	u32	EDCA_BE_UL = 0x5ea42b;//Parameter suggested by Scott 
	u32	EDCA_BE_DL = 0x5ea42b;//Parameter suggested by Scott 
	u32	ICType=pDM_Odm->SupportICType;
	u32	IOTPeer=0;
	u8	WirelessMode=0xFF;                  
	u32 	trafficIndex;
	u32	edca_param;
	u64	cur_tx_bytes = 0;
	u64	cur_rx_bytes = 0;
	u8	bbtchange = _FALSE;
	u8	bBiasOnRx = _FALSE;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	struct dvobj_priv		*pdvobjpriv = adapter_to_dvobj(Adapter);
	struct xmit_priv		*pxmitpriv = &(Adapter->xmitpriv);
	struct recv_priv		*precvpriv = &(Adapter->recvpriv);
	struct registry_priv	*pregpriv = &Adapter->registrypriv;
	struct mlme_ext_priv	*pmlmeext = &(Adapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	if(pDM_Odm->bLinked != _TRUE)
		goto dm_CheckEdcaTurbo_EXIT;

	if ((pregpriv->wifi_spec == 1) )//|| (pmlmeinfo->HT_enable == 0))
	{
		goto dm_CheckEdcaTurbo_EXIT;
	}

	if(pDM_Odm->pWirelessMode!=NULL)
		WirelessMode=*(pDM_Odm->pWirelessMode);

	IOTPeer = pmlmeinfo->assoc_AP_vendor;

	if (IOTPeer >=  HT_IOT_PEER_MAX)
	{
		goto dm_CheckEdcaTurbo_EXIT;
	}

	if(	(pDM_Odm->SupportICType == ODM_RTL8192C) ||
		(pDM_Odm->SupportICType == ODM_RTL8723A) ||
		(pDM_Odm->SupportICType == ODM_RTL8188E))
	{
		if((IOTPeer == HT_IOT_PEER_RALINK)||(IOTPeer == HT_IOT_PEER_ATHEROS))
			bBiasOnRx = _TRUE;
	}

	if((bbtchange) || (!precvpriv->bIsAnyNonBEPkts) )
	{
		cur_tx_bytes = pdvobjpriv->traffic_stat.cur_tx_bytes;
		cur_rx_bytes = pdvobjpriv->traffic_stat.cur_rx_bytes;

		if(bBiasOnRx)
		{
			if (cur_tx_bytes > (cur_rx_bytes << 2))
			{
				trafficIndex = UP_LINK; 
			}
			else
			{
				trafficIndex = DOWN_LINK;
			}
		}
		else
		{
			if (cur_rx_bytes > (cur_tx_bytes << 2))
			{
				trafficIndex = DOWN_LINK;
			}
			else
			{
				trafficIndex = UP_LINK;
			}
		}

		{
			if(ICType==ODM_RTL8192D)
			{      
				if(pDM_Odm->RFType==ODM_2T2R)
				{
					EDCA_BE_UL = 0x60a42b;   
					EDCA_BE_DL = 0x60a42b;   
				}
				else
				{
					EDCA_BE_UL = 0x6ea42b;
					EDCA_BE_DL = 0x6ea42b;
				}
			}
			else
			{
				if(pDM_Odm->SupportInterface==ODM_ITRF_PCIE) {
					if((ICType==ODM_RTL8192C)&&(pDM_Odm->RFType==ODM_2T2R)) {
						EDCA_BE_UL = 0x60a42b;
						EDCA_BE_DL = 0x60a42b;
					}
					else
					{
						EDCA_BE_UL = 0x6ea42b;
						EDCA_BE_DL = 0x6ea42b;
					}
				}
			}
		
			if((ICType!=ODM_RTL8192D) && (IOTPeer== HT_IOT_PEER_CISCO) &&(WirelessMode==ODM_WM_N24G))
			{
				EDCA_BE_DL = edca_setting_DL[IOTPeer];
				EDCA_BE_UL = edca_setting_UL[IOTPeer];
			}
			else if((IOTPeer == HT_IOT_PEER_CISCO) &&((WirelessMode==ODM_WM_G)||(WirelessMode==(ODM_WM_B|ODM_WM_G))||(WirelessMode==ODM_WM_A)||(WirelessMode==ODM_WM_B)))
			{
				EDCA_BE_DL = edca_setting_DL_GMode[IOTPeer];
			}
			else if((IOTPeer== HT_IOT_PEER_AIRGO )&& ((WirelessMode==ODM_WM_G)||(WirelessMode==ODM_WM_A)))
			{
				EDCA_BE_DL = 0xa630;
			}
			else if(IOTPeer == HT_IOT_PEER_MARVELL)
			{
				EDCA_BE_DL = edca_setting_DL[IOTPeer];
				EDCA_BE_UL = edca_setting_UL[IOTPeer];
			}
			else if(IOTPeer == HT_IOT_PEER_ATHEROS)
			{
				EDCA_BE_DL = edca_setting_DL[IOTPeer];
			}

			if((ICType==ODM_RTL8812)||(ICType==ODM_RTL8821)||(ICType==ODM_RTL8192E))          
			{
				EDCA_BE_UL = 0x5ea42b;
				EDCA_BE_DL = 0x5ea42b;

				ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("8812A: EDCA_BE_UL=0x%x EDCA_BE_DL =0x%x",EDCA_BE_UL,EDCA_BE_DL));
			}

			if (trafficIndex == DOWN_LINK)
				edca_param = EDCA_BE_DL;
			else
				edca_param = EDCA_BE_UL;

			rtw_write32(Adapter, REG_EDCA_BE_PARAM, edca_param);

			pDM_Odm->DM_EDCA_Table.prv_traffic_idx = trafficIndex;
		}
		
		pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = _TRUE;
	}
	else
	{
		 if(pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA)
		{
			rtw_write32(Adapter, REG_EDCA_BE_PARAM, pHalData->AcParam_BE);
			pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = _FALSE;
		}
	}

dm_CheckEdcaTurbo_EXIT:
	precvpriv->bIsAnyNonBEPkts = _FALSE;
#endif	
}


#elif(DM_ODM_SUPPORT_TYPE==ODM_WIN)
VOID
odm_EdcaTurboCheckMP(
	IN		PDM_ODM_T		pDM_Odm
	)
{


	PADAPTER		       Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);

	PADAPTER 			pDefaultAdapter = GetDefaultAdapter(Adapter);
	PADAPTER 			pExtAdapter = GetFirstExtAdapter(Adapter);//NULL;
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	PSTA_QOS			pStaQos = Adapter->MgntInfo.pStaQos;
	u8Byte				Ext_curTxOkCnt = 0;
	u8Byte				Ext_curRxOkCnt = 0;	
	u1Byte TwoPortStatus = (u1Byte)TWO_PORT_STATUS__WITHOUT_ANY_ASSOCIATE;

	u8Byte				curTxOkCnt = 0;
	u8Byte				curRxOkCnt = 0;	
	u4Byte				EDCA_BE_UL = 0x5ea42b;//Parameter suggested by Scott 
	u4Byte				EDCA_BE_DL = 0x5ea42b;//Parameter suggested by Scott 
	u4Byte                         EDCA_BE = 0x5ea42b;
	u1Byte                         IOTPeer=0;
	BOOLEAN                      *pbIsCurRDLState=NULL;
	BOOLEAN                      bLastIsCurRDLState=FALSE;
	BOOLEAN				 bBiasOnRx=FALSE;
	BOOLEAN				bEdcaTurboOn=FALSE;
	u1Byte				TxRate = 0xFF;
	u8Byte				value64;	

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("odm_EdcaTurboCheckMP========================>"));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial BE PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_BE_PARAM)));

	bLastIsCurRDLState=pDM_Odm->DM_EDCA_Table.bIsCurRDLState;
	pbIsCurRDLState=&(pDM_Odm->DM_EDCA_Table.bIsCurRDLState);	

	if (pMgntInfo->NumNonBePkt > pMgntInfo->RegEdcaThresh && !Adapter->MgntInfo.bWiFiConfg)
		pHalData->bIsAnyNonBEPkts = TRUE;

	pMgntInfo->NumNonBePkt = 0;

	curTxOkCnt = Adapter->TxStats.NumTxBytesUnicast - pDM_Odm->lastTxOkCnt;
	curRxOkCnt = Adapter->RxStats.NumRxBytesUnicast - pDM_Odm->lastRxOkCnt;
	pDM_Odm->lastTxOkCnt = Adapter->TxStats.NumTxBytesUnicast;
	pDM_Odm->lastRxOkCnt = Adapter->RxStats.NumRxBytesUnicast;

	if(pExtAdapter == NULL) 
		pExtAdapter = pDefaultAdapter;

	Ext_curTxOkCnt = pExtAdapter->TxStats.NumTxBytesUnicast - pMgntInfo->Ext_lastTxOkCnt;
	Ext_curRxOkCnt = pExtAdapter->RxStats.NumRxBytesUnicast - pMgntInfo->Ext_lastRxOkCnt;
	GetTwoPortSharedResource(Adapter,TWO_PORT_SHARED_OBJECT__STATUS,NULL,&TwoPortStatus);
	if(TwoPortStatus == TWO_PORT_STATUS__EXTENSION_ONLY)
	{
		curTxOkCnt = Ext_curTxOkCnt ;
		curRxOkCnt = Ext_curRxOkCnt ;
	}
	IOTPeer=pMgntInfo->IOTPeer;
	bBiasOnRx=(pMgntInfo->IOTAction & HT_IOT_ACT_EDCA_BIAS_ON_RX)?TRUE:FALSE;
	bEdcaTurboOn=((!pHalData->bIsAnyNonBEPkts) && (!pMgntInfo->bDisableFrameBursting))?TRUE:FALSE;
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("bIsAnyNonBEPkts : 0x%lx  bDisableFrameBursting : 0x%lx  \n",pHalData->bIsAnyNonBEPkts,pMgntInfo->bDisableFrameBursting));


	if(odm_IsEdcaTurboDisable(pDM_Odm))
		goto dm_CheckEdcaTurbo_EXIT;


	ODM_EdcaParaSelByIot(pDM_Odm, &EDCA_BE_UL, &EDCA_BE_DL);


	if(bEdcaTurboOn)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("bEdcaTurboOn : 0x%x bBiasOnRx : 0x%x\n",bEdcaTurboOn,bBiasOnRx));
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("curTxOkCnt : 0x%lx \n",curTxOkCnt));
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("curRxOkCnt : 0x%lx \n",curRxOkCnt));
		if(bBiasOnRx)
			odm_EdcaChooseTrafficIdx(pDM_Odm,curTxOkCnt, curRxOkCnt,   TRUE,  pbIsCurRDLState);
		else
			odm_EdcaChooseTrafficIdx(pDM_Odm,curTxOkCnt, curRxOkCnt,   FALSE,  pbIsCurRDLState);

			EDCA_BE=((*pbIsCurRDLState)==TRUE)?EDCA_BE_DL:EDCA_BE_UL;
			if(IS_HARDWARE_TYPE_8821U(Adapter))
			{
				if(pMgntInfo->RegTxDutyEnable)
				{
					if(!pMgntInfo->ForcedDataRate)
					{
						if(pDM_Odm->TxRate != 0xFF)
							TxRate = Adapter->HalFunc.GetHwRateFromMRateHandler(pDM_Odm->TxRate); 
					}
					else
					{
						TxRate = (u1Byte) pMgntInfo->ForcedDataRate;
					}

					value64 = (curRxOkCnt<<2);
					if(curTxOkCnt < value64)
						ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,EDCA_BE);
					else
					{
						if((pDM_Odm->RFCalibrateInfo.ThermalValue < 0x2c) || (*pDM_Odm->pBandType == BAND_ON_2_4G))
							ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,EDCA_BE);
						else
						{
							switch (TxRate)
							{
								case MGN_VHT1SS_MCS6:
								case MGN_VHT1SS_MCS5:
								case MGN_MCS6:
								case MGN_MCS5:
								case MGN_48M:
								case MGN_54M:
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0x1ea42b);
								break;
								case MGN_VHT1SS_MCS4:
								case MGN_MCS4:
								case MGN_36M:
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0xa42b);
								break;
								case MGN_VHT1SS_MCS3:
								case MGN_MCS3:
								case MGN_24M:
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0xa47f);
								break;
								case MGN_VHT1SS_MCS2:
								case MGN_MCS2:
								case MGN_18M:
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0xa57f);
								break;
								case MGN_VHT1SS_MCS1:
								case MGN_MCS1:
								case MGN_9M:
								case MGN_12M:
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0xa77f);
								break;
								case MGN_VHT1SS_MCS0:
								case MGN_MCS0:
								case MGN_6M:
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0xa87f);
								break;
								default:
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,EDCA_BE);
								break;
							}
						}
					}				
				}
				else
				{
					ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,EDCA_BE);
				}

			}
			else if (IS_HARDWARE_TYPE_8812AU(Adapter)){
				if(pMgntInfo->RegTxDutyEnable)
				{
					if(!pMgntInfo->ForcedDataRate)
					{
						if(pDM_Odm->TxRate != 0xFF)
							TxRate = Adapter->HalFunc.GetHwRateFromMRateHandler(pDM_Odm->TxRate); 
					}
					else
					{
						TxRate = (u1Byte) pMgntInfo->ForcedDataRate;
					}

					value64 = (curRxOkCnt<<2);
					if(curTxOkCnt < value64)
						ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,EDCA_BE);
					else
					{
						if((pDM_Odm->RFCalibrateInfo.ThermalValue < 0x2c) || (*pDM_Odm->pBandType == BAND_ON_2_4G))
							ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,EDCA_BE);
						else
						{
							switch (TxRate)
							{
								case MGN_VHT2SS_MCS9:
								case MGN_VHT1SS_MCS9:									
								case MGN_VHT1SS_MCS8:
								case MGN_MCS15:
								case MGN_MCS7:									
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0x1ea44f);							
								case MGN_VHT2SS_MCS8:
								case MGN_VHT1SS_MCS7:
								case MGN_MCS14:
								case MGN_MCS6:
								case MGN_54M:									
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0xa44f);
								case MGN_VHT2SS_MCS7:
								case MGN_VHT2SS_MCS6:
								case MGN_VHT1SS_MCS6:
								case MGN_VHT1SS_MCS5:
								case MGN_MCS13:
								case MGN_MCS5:
								case MGN_48M:
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0xa630);
								break;
								case MGN_VHT2SS_MCS5:
								case MGN_VHT2SS_MCS4:
								case MGN_VHT1SS_MCS4:
								case MGN_VHT1SS_MCS3:	
								case MGN_MCS12:
								case MGN_MCS4:	
								case MGN_MCS3:	
								case MGN_36M:
								case MGN_24M:	
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0xa730);
								break;
								case MGN_VHT2SS_MCS3:
								case MGN_VHT2SS_MCS2:
								case MGN_VHT2SS_MCS1:
								case MGN_VHT1SS_MCS2:
								case MGN_VHT1SS_MCS1:	
								case MGN_MCS11:	
								case MGN_MCS10:	
								case MGN_MCS9:		
								case MGN_MCS2:	
								case MGN_MCS1:
								case MGN_18M:	
								case MGN_12M:
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0xa830);
								break;
								case MGN_VHT2SS_MCS0:
								case MGN_VHT1SS_MCS0:
								case MGN_MCS0:	
								case MGN_MCS8:
								case MGN_9M:	
								case MGN_6M:
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,0xa87f);
								break;
								default:
									ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,EDCA_BE);
								break;
							}
						}
					}				
				}
				else
				{
					ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,EDCA_BE);
				}
			}
			else
				ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,EDCA_BE);

		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("EDCA Turbo on: EDCA_BE:0x%lx\n",EDCA_BE));

		pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = TRUE;
		
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("EDCA_BE_DL : 0x%lx  EDCA_BE_UL : 0x%lx  EDCA_BE : 0x%lx  \n",EDCA_BE_DL,EDCA_BE_UL,EDCA_BE));

	}
	else
	{
		 if(pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA)
		{
			Adapter->HalFunc.SetHwRegHandler(Adapter, HW_VAR_AC_PARAM, GET_WMM_PARAM_ELE_SINGLE_AC_PARAM(pStaQos->WMMParamEle, AC0_BE) );

			pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = FALSE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Restore EDCA BE: 0x%lx  \n",pDM_Odm->WMMEDCA_BE));

		}
	}

dm_CheckEdcaTurbo_EXIT:
#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
	pHalData->bIsAnyNonBEPkts = FALSE;
	pMgntInfo->lastTxOkCnt = Adapter->TxStats.NumTxBytesUnicast;
	pMgntInfo->lastRxOkCnt = Adapter->RxStats.NumRxBytesUnicast;
	pMgntInfo->Ext_lastTxOkCnt = pExtAdapter->TxStats.NumTxBytesUnicast;
	pMgntInfo->Ext_lastRxOkCnt = pExtAdapter->RxStats.NumRxBytesUnicast;
#elif (DM_ODM_SUPPORT_TYPE==ODM_CE)
	precvpriv->bIsAnyNonBEPkts = FALSE;
	pxmitpriv->last_tx_bytes = pxmitpriv->tx_bytes;
	precvpriv->last_rx_bytes = precvpriv->rx_bytes;
#endif

}


BOOLEAN
odm_IsEdcaTurboDisable(
	IN 	PDM_ODM_T 	pDM_Odm
)
{
	PADAPTER		       Adapter = pDM_Odm->Adapter;

#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	u4Byte				IOTPeer=pMgntInfo->IOTPeer;
#elif (DM_ODM_SUPPORT_TYPE==ODM_CE)
	struct registry_priv	*pregpriv = &Adapter->registrypriv;
	struct mlme_ext_priv	*pmlmeext = &(Adapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u4Byte                         IOTPeer=pmlmeinfo->assoc_AP_vendor;
	u1Byte                         WirelessMode=0xFF;                  

	if(pDM_Odm->pWirelessMode!=NULL)
		WirelessMode=*(pDM_Odm->pWirelessMode);

#endif

	if(pDM_Odm->bBtDisableEdcaTurbo)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("EdcaTurboDisable for BT!!\n"));
		return TRUE;
	}

	if((!(pDM_Odm->SupportAbility& ODM_MAC_EDCA_TURBO ))||
		(pDM_Odm->bWIFITest)||
		(IOTPeer>= HT_IOT_PEER_MAX))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("EdcaTurboDisable\n"));
		return TRUE;
	}


#if (DM_ODM_SUPPORT_TYPE ==ODM_WIN)
	if(pMgntInfo->IOTAction & HT_IOT_ACT_DISABLE_EDCA_TURBO){
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("IOTAction:EdcaTurboDisable\n"));
		return	TRUE;
		}
		
#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
	if((WirelessMode==ODM_WM_B)||(WirelessMode==(ODM_WM_B|ODM_WM_G)||(WirelessMode==ODM_WM_G)||(WirelessMode=ODM_WM_A))
		ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, ODM_Read4Byte(pDM_Odm, ODM_EDCA_BE_PARAM)|0x5E0000);	
	
	if(pDM_Odm->SupportICType==ODM_RTL8192D)		{
		if ((pregpriv->wifi_spec == 1)  || (pmlmeext->cur_wireless_mode == WIRELESS_11B)) {
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("92D:EdcaTurboDisable\n"));
			return TRUE;
		}
	}	
	else
	{
		if((pregpriv->wifi_spec == 1) || (pmlmeinfo->HT_enable == 0)){
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("Others:EdcaTurboDisable\n"));
			return TRUE;
		}
	}

#endif

	return	FALSE;
	

}

VOID 
ODM_EdcaParaSelByIot(
	IN 	PDM_ODM_T 	pDM_Odm,
	OUT	u4Byte		*EDCA_BE_UL,
	OUT u4Byte		*EDCA_BE_DL
	)
{

	PADAPTER		       Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	u4Byte                         IOTPeer=0;
	u4Byte                         ICType=pDM_Odm->SupportICType;
	u1Byte                         WirelessMode=0xFF;                  
	u4Byte				RFType=pDM_Odm->RFType;
	  u4Byte                         IOTPeerSubType=0;

#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	u1Byte 				TwoPortStatus = (u1Byte)TWO_PORT_STATUS__WITHOUT_ANY_ASSOCIATE;

#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	#ifdef CONFIG_BT_COEXIST
	struct btcoexist_priv	*pbtpriv = &(pHalData->bt_coexist);	
	#endif
       u1Byte bbtchange =FALSE;
#endif

	if(pDM_Odm->pWirelessMode!=NULL)
		WirelessMode=*(pDM_Odm->pWirelessMode);
		
#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)	
	IOTPeer=pMgntInfo->IOTPeer;
	IOTPeerSubType=pMgntInfo->IOTPeerSubtype;
	GetTwoPortSharedResource(Adapter,TWO_PORT_SHARED_OBJECT__STATUS,NULL,&TwoPortStatus);

#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
	IOTPeer=pmlmeinfo->assoc_AP_vendor;
	#ifdef CONFIG_BT_COEXIST
	if(pbtpriv->BT_Coexist)
	{
		if( (pbtpriv->BT_EDCA[UP_LINK]!=0) ||  (pbtpriv->BT_EDCA[DOWN_LINK]!=0))
			bbtchange = TRUE;		
	}
	#endif

#endif

	if(ICType==ODM_RTL8192D)
	{      
		if(pDM_Odm->RFType==ODM_2T2R)
		{
			(*EDCA_BE_UL) = 0x60a42b;   
			(*EDCA_BE_DL) = 0x60a42b;   

		}
		else
		{
			(*EDCA_BE_UL) = 0x6ea42b;
			(*EDCA_BE_DL) = 0x6ea42b;
		}

	}
#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
	else
	{

		if(pDM_Odm->SupportInterface==ODM_ITRF_PCIE){
			if((ICType==ODM_RTL8192C)&&(pDM_Odm->RFType==ODM_2T2R))			{
				(*EDCA_BE_UL) = 0x60a42b;
				(*EDCA_BE_DL) = 0x60a42b;
			}
			else
			{
				(*EDCA_BE_UL) = 0x6ea42b;
				(*EDCA_BE_DL) = 0x6ea42b;
			}
		}
	}
 
	if(TwoPortStatus == TWO_PORT_STATUS__EXTENSION_ONLY)
	{
		(*EDCA_BE_UL) = 0x5ea42b;//Parameter suggested by Scott 
		(*EDCA_BE_DL) = 0x5ea42b;//Parameter suggested by Scott 
	}
     
	#if (INTEL_PROXIMITY_SUPPORT == 1)
	if(pMgntInfo->IntelClassModeInfo.bEnableCA == TRUE)
	{
		(*EDCA_BE_UL) = (*EDCA_BE_DL) = 0xa44f;
	}
	else
	#endif		
	{
		if((!pMgntInfo->bDisableFrameBursting) && 
			(pMgntInfo->IOTAction & (HT_IOT_ACT_FORCED_ENABLE_BE_TXOP|HT_IOT_ACT_AMSDU_ENABLE)))
		{// To check whether we shall force turn on TXOP configuration.
			if(!((*EDCA_BE_UL) & 0xffff0000))
				(*EDCA_BE_UL) |= 0x005e0000;
			if(!((*EDCA_BE_DL) & 0xffff0000))
				(*EDCA_BE_DL) |= 0x005e0000;
		}
		
		if((ICType!=ODM_RTL8192D) && (IOTPeer== HT_IOT_PEER_CISCO) &&(WirelessMode==ODM_WM_N24G))
		{
			(*EDCA_BE_DL) = edca_setting_DL[IOTPeer];
			(*EDCA_BE_UL) = edca_setting_UL[IOTPeer];
		}
		else if((IOTPeer == HT_IOT_PEER_CISCO) &&((WirelessMode==ODM_WM_G)||(WirelessMode==(ODM_WM_B|ODM_WM_G))||(WirelessMode==ODM_WM_A)||(WirelessMode==ODM_WM_B)))
		{
			(*EDCA_BE_DL) = edca_setting_DL_GMode[IOTPeer];
		}
		else if((IOTPeer== HT_IOT_PEER_AIRGO )&& ((WirelessMode==ODM_WM_G)||(WirelessMode==ODM_WM_A)))
		{
			(*EDCA_BE_DL) = 0xa630;
		}

		else if(IOTPeer == HT_IOT_PEER_MARVELL)
		{
			(*EDCA_BE_DL) = edca_setting_DL[IOTPeer];
			(*EDCA_BE_UL) = edca_setting_UL[IOTPeer];
		}
		else if(IOTPeer == HT_IOT_PEER_ATHEROS)
		{
			(*EDCA_BE_DL) = edca_setting_DL[IOTPeer];
			
			if(ICType == ODM_RTL8821)
				 (*EDCA_BE_DL) = 0x5ea630;
			
		}
	}

    	if((ICType == ODM_RTL8192D)&&(IOTPeerSubType == HT_IOT_PEER_LINKSYS_E4200_V1)&&((WirelessMode==ODM_WM_N5G)))
	{
		(*EDCA_BE_DL) = 0x432b;
		(*EDCA_BE_UL) = 0x432b;
	}		



	if((ICType==ODM_RTL8812)||(ICType==ODM_RTL8192E))          
	{
		(*EDCA_BE_UL) = 0x5ea42b;
		(*EDCA_BE_DL) = 0x5ea42b;

		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("8812A: EDCA_BE_UL=0x%lx EDCA_BE_DL =0x%lx",(*EDCA_BE_UL),(*EDCA_BE_DL)));
	}

	if((ICType == ODM_RTL8723A) && (IOTPeerSubType== HT_IOT_PEER_ATHEROS_DIR655) && 
		(pMgntInfo->dot11CurrentChannelNumber == 6))
	{
		(*EDCA_BE_DL) = 0xa92b;
	}

#elif (DM_ODM_SUPPORT_TYPE==ODM_CE)

	if(RFType==ODM_RTL8192D)
	{
		if((IOTPeer == HT_IOT_PEER_CISCO) &&(WirelessMode==ODM_WM_N24G))
		{
			(*EDCA_BE_UL) = EDCAParam[IOTPeer][UP_LINK];
			(*EDCA_BE_DL)=EDCAParam[IOTPeer][DOWN_LINK];
		}
		else if((IOTPeer == HT_IOT_PEER_AIRGO) &&
			((WirelessMode==ODM_WM_B)||(WirelessMode==(ODM_WM_B|ODM_WM_G)))) 
			(*EDCA_BE_DL)=0x00a630;
		
		else if((IOTPeer== HT_IOT_PEER_ATHEROS) && 
					(WirelessMode&ODM_WM_N5G) &&
					(Adapter->securitypriv.dot11PrivacyAlgrthm == _AES_ ))
			(*EDCA_BE_DL)=0xa42b;
			
	}
	else
	{
		#ifdef CONFIG_BT_COEXIST
		if(bbtchange)
		{
			(*EDCA_BE_UL) = pbtpriv->BT_EDCA[UP_LINK];
			(*EDCA_BE_DL) = pbtpriv->BT_EDCA[DOWN_LINK];		
		}
		else
		#endif
		{
			if((IOTPeer == HT_IOT_PEER_CISCO) &&(WirelessMode==ODM_WM_N24G))
			{
				(*EDCA_BE_UL) = EDCAParam[IOTPeer][UP_LINK];
				(*EDCA_BE_DL)=EDCAParam[IOTPeer][DOWN_LINK];
			}
			else
			{
				(*EDCA_BE_UL)=EDCAParam[HT_IOT_PEER_UNKNOWN][UP_LINK];
				(*EDCA_BE_DL)=EDCAParam[HT_IOT_PEER_UNKNOWN][DOWN_LINK];
			}
		}
		if(pDM_Odm->SupportInterface==ODM_ITRF_PCIE){
			if((ICType==ODM_RTL8192C)&&(pDM_Odm->RFType==ODM_2T2R))
			{
				(*EDCA_BE_UL) = 0x60a42b;
				(*EDCA_BE_DL) = 0x60a42b;
			}
			else
			{
				(*EDCA_BE_UL) = 0x6ea42b;
				(*EDCA_BE_DL) = 0x6ea42b;
			}
		}

	}
#endif

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Special: EDCA_BE_UL=0x%lx EDCA_BE_DL =0x%lx",(*EDCA_BE_UL),(*EDCA_BE_DL)));

}


VOID
odm_EdcaChooseTrafficIdx( 
	IN	PDM_ODM_T		pDM_Odm,
	IN	u8Byte  			cur_tx_bytes,  
	IN	u8Byte  			cur_rx_bytes, 
	IN	BOOLEAN 		bBiasOnRx,
	OUT BOOLEAN 		*pbIsCurRDLState
	)
{	
	
	
	if(bBiasOnRx)
	{
	  
		if(cur_tx_bytes>(cur_rx_bytes*4))
		{
			*pbIsCurRDLState=FALSE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Uplink Traffic\n "));

		}
		else
		{
			*pbIsCurRDLState=TRUE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Balance Traffic\n"));

		}
	}
	else
	{
		if(cur_rx_bytes>(cur_tx_bytes*4))
		{
			*pbIsCurRDLState=TRUE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Downlink	Traffic\n"));

		}
		else
		{
			*pbIsCurRDLState=FALSE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Balance Traffic\n"));
		}
	}

	return ;
}

#endif

#if((DM_ODM_SUPPORT_TYPE==ODM_AP)||(DM_ODM_SUPPORT_TYPE==ODM_ADSL))

void odm_EdcaParaInit(
	IN	PDM_ODM_T	pDM_Odm
	)
{
	prtl8192cd_priv	priv		= pDM_Odm->priv;
	int   mode=priv->pmib->dot11BssType.net_work_type;
	
	static unsigned int slot_time, VO_TXOP, VI_TXOP, sifs_time;
	struct ParaRecord EDCA[4];

	 memset(EDCA, 0, 4*sizeof(struct ParaRecord));

	sifs_time = 10;
	slot_time = 20;

	if (mode & (ODM_WM_N24G|ODM_WM_N5G))
		sifs_time = 16;

	if (mode & (ODM_WM_N24G|ODM_WM_N5G| ODM_WM_G|ODM_WM_A))
		slot_time = 9;


#if((defined(RTL_MANUAL_EDCA))&&(DM_ODM_SUPPORT_TYPE==ODM_AP))
	 if( priv->pmib->dot11QosEntry.ManualEDCA ) {
		 if( OPMODE & WIFI_AP_STATE )
			 memcpy(EDCA, priv->pmib->dot11QosEntry.AP_manualEDCA, 4*sizeof(struct ParaRecord));
		 else
			 memcpy(EDCA, priv->pmib->dot11QosEntry.STA_manualEDCA, 4*sizeof(struct ParaRecord));

		#ifdef WIFI_WMM
		if (QOS_ENABLE)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, (EDCA[VI].TXOPlimit<< 16) | (EDCA[VI].ECWmax<< 12) | (EDCA[VI].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));
		else
		#endif
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, (EDCA[BE].TXOPlimit<< 16) | (EDCA[BE].ECWmax<< 12) | (EDCA[BE].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));

	}else
	#endif
	{

		 if(OPMODE & WIFI_AP_STATE)
		 {
		 	memcpy(EDCA, rtl_ap_EDCA, 2*sizeof(struct ParaRecord));

			if(mode & (ODM_WM_A|ODM_WM_G|ODM_WM_N24G|ODM_WM_N5G))
				memcpy(&EDCA[VI], &rtl_ap_EDCA[VI_AG], 2*sizeof(struct ParaRecord));
			else
				memcpy(&EDCA[VI], &rtl_ap_EDCA[VI], 2*sizeof(struct ParaRecord));
		 }
		 else
		 {
		 	memcpy(EDCA, rtl_sta_EDCA, 2*sizeof(struct ParaRecord));

			if(mode & (ODM_WM_A|ODM_WM_G|ODM_WM_N24G|ODM_WM_N5G))
				memcpy(&EDCA[VI], &rtl_sta_EDCA[VI_AG], 2*sizeof(struct ParaRecord));
			else
				memcpy(&EDCA[VI], &rtl_sta_EDCA[VI], 2*sizeof(struct ParaRecord));
		 }
		 
	#ifdef WIFI_WMM
		if (QOS_ENABLE)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, (EDCA[VI].TXOPlimit<< 16) | (EDCA[VI].ECWmax<< 12) | (EDCA[VI].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));
		else
	#endif

#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM,  (EDCA[BK].ECWmax<< 12) | (EDCA[BK].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));
#elif(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM,  (EDCA[BK].ECWmax<< 12) | (EDCA[BK].ECWmin<< 8) | (sifs_time + 2* slot_time));
#endif
			

	}

	ODM_Write4Byte(pDM_Odm, ODM_EDCA_VO_PARAM, (EDCA[VO].TXOPlimit<< 16) | (EDCA[VO].ECWmax<< 12) | (EDCA[VO].ECWmin<< 8) | (sifs_time + EDCA[VO].AIFSN* slot_time));
	ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM,  (EDCA[BE].TXOPlimit<< 16) | (EDCA[BE].ECWmax<< 12) | (EDCA[BE].ECWmin<< 8) | (sifs_time + EDCA[BE].AIFSN* slot_time));
	ODM_Write4Byte(pDM_Odm, ODM_EDCA_BK_PARAM, (EDCA[BK].TXOPlimit<< 16) | (EDCA[BK].ECWmax<< 12) | (EDCA[BK].ECWmin<< 8) | (sifs_time + EDCA[BK].AIFSN* slot_time));

	priv->pshare->iot_mode_enable = 0;
#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
	if (priv->pshare->rf_ft_var.wifi_beq_iot)
		priv->pshare->iot_mode_VI_exist = 0;
	
	#ifdef WMM_VIBE_PRI
	priv->pshare->iot_mode_BE_exist = 0;
	#endif
	
	#ifdef LOW_TP_TXOP
	priv->pshare->BE_cwmax_enhance = 0;
	#endif

#elif (DM_ODM_SUPPORT_TYPE==ODM_ADSL)
      priv->pshare->iot_mode_BE_exist = 0;   
#endif
	priv->pshare->iot_mode_VO_exist = 0;
}

BOOLEAN
ODM_ChooseIotMainSTA(
	IN	PDM_ODM_T		pDM_Odm,
	IN	PSTA_INFO_T		pstat
	)
{
	prtl8192cd_priv	priv = pDM_Odm->priv;
	BOOLEAN		bhighTP_found_pstat=FALSE;
	
	if ((GET_ROOT(priv)->up_time % 2) == 0) {
		unsigned int tx_2s_avg = 0;
		unsigned int rx_2s_avg = 0;
		int i=0, aggReady=0;
		unsigned long total_sum = (priv->pshare->current_tx_bytes+priv->pshare->current_rx_bytes);

		pstat->current_tx_bytes += pstat->tx_byte_cnt;
		pstat->current_rx_bytes += pstat->rx_byte_cnt;

		if (total_sum != 0) {
			if (total_sum <= 100) {
			tx_2s_avg = (unsigned int)((pstat->current_tx_bytes*100) / total_sum);
			rx_2s_avg = (unsigned int)((pstat->current_rx_bytes*100) / total_sum);
			} else {
				tx_2s_avg = (unsigned int)(pstat->current_tx_bytes / (total_sum / 100));
				rx_2s_avg = (unsigned int)(pstat->current_rx_bytes / (total_sum / 100));
			}

		}

#if(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
		if (pstat->ht_cap_len) {
			if ((tx_2s_avg + rx_2s_avg) >=25 /*50*/) {

					priv->pshare->highTP_found_pstat = pstat;
					bhighTP_found_pstat=TRUE;
   				}
			}
#elif(DM_ODM_SUPPORT_TYPE==ODM_AP)
		for(i=0; i<8; i++)
			aggReady += (pstat->ADDBA_ready[i]);
		if (pstat->ht_cap_len && aggReady) 
		{
			if ((tx_2s_avg + rx_2s_avg >= 25)) {
				priv->pshare->highTP_found_pstat = pstat;
			}
			
		#ifdef CLIENT_MODE
			if (OPMODE & WIFI_STATION_STATE) {
#if (DM_ODM_SUPPORT_TYPE &ODM_AP) && defined(USE_OUT_SRC)
				if ((pstat->IOTPeer==HT_IOT_PEER_RALINK) && ((tx_2s_avg + rx_2s_avg) >= 45))
#else
				if(pstat->is_ralink_sta && ((tx_2s_avg + rx_2s_avg) >= 45))
#endif					
					priv->pshare->highTP_found_pstat = pstat;
		}
		#endif				
	}
#endif
	} else {
		pstat->current_tx_bytes = pstat->tx_byte_cnt;
		pstat->current_rx_bytes = pstat->rx_byte_cnt;
	}

	return bhighTP_found_pstat;
}


#ifdef WIFI_WMM
VOID
ODM_IotEdcaSwitch(
	IN	PDM_ODM_T		pDM_Odm,
	IN	unsigned char		enable
	)
{
	prtl8192cd_priv	priv	= pDM_Odm->priv;
	int   mode=priv->pmib->dot11BssType.net_work_type;
	unsigned int slot_time = 20, sifs_time = 10, BE_TXOP = 47, VI_TXOP = 94;
	unsigned int vi_cw_max = 4, vi_cw_min = 3, vi_aifs;

#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
	if (!(!priv->pmib->dot11OperationEntry.wifi_specific ||
		((OPMODE & WIFI_AP_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific == 2))
	#ifdef CLIENT_MODE
		|| ((OPMODE & WIFI_STATION_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific == 2))
	#endif
		))
		return;
#endif

	if ((mode & (ODM_WM_N24G|ODM_WM_N5G)) && (priv->pshare->ht_sta_num
	#ifdef WDS
		|| ((OPMODE & WIFI_AP_STATE) && priv->pmib->dot11WdsInfo.wdsEnabled && priv->pmib->dot11WdsInfo.wdsNum)
	#endif
		))
		sifs_time = 16;

	if (mode & (ODM_WM_N24G|ODM_WM_N5G|ODM_WM_G|ODM_WM_A)) {
		slot_time = 9;
	} 
	else
	{
		BE_TXOP = 94;
		VI_TXOP = 188;
	}

#if (DM_ODM_SUPPORT_TYPE==ODM_ADSL)
	if (priv->pshare->iot_mode_VO_exist) {
		if (priv->pshare->iot_mode_BE_exist) {
			vi_cw_max = 5;
			vi_cw_min = 3;
		} else {
			vi_cw_max = 6;
			vi_cw_min = 4;
		}
	}
	vi_aifs = (sifs_time + ((OPMODE & WIFI_AP_STATE)?1:2) * slot_time);

	ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, ((VI_TXOP*(1-priv->pshare->iot_mode_VO_exist)) << 16)| (vi_cw_max << 12) | (vi_cw_min << 8) | vi_aifs);

	
#elif (DM_ODM_SUPPORT_TYPE==ODM_AP)
	if ((OPMODE & WIFI_AP_STATE) && priv->pmib->dot11OperationEntry.wifi_specific) {
		if (priv->pshare->iot_mode_VO_exist) {
	#ifdef WMM_VIBE_PRI
			if (priv->pshare->iot_mode_BE_exist) 
			{
				vi_cw_max = 5;
				vi_cw_min = 3;
				vi_aifs = (sifs_time + ((OPMODE & WIFI_AP_STATE)?1:2) * slot_time);
			}
			else 
	#endif
			{
			vi_cw_max = 6;
			vi_cw_min = 4;
			vi_aifs = 0x2b;
			}
		} 
		else {
			vi_aifs = (sifs_time + ((OPMODE & WIFI_AP_STATE)?1:2) * slot_time);
		}

		ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, ((VI_TXOP*(1-priv->pshare->iot_mode_VO_exist)) << 16)
			| (vi_cw_max << 12) | (vi_cw_min << 8) | vi_aifs);
	}
#endif



#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
 	if (priv->pshare->rf_ft_var.wifi_beq_iot && priv->pshare->iot_mode_VI_exist) 
	  	ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (10 << 12) | (4 << 8) | 0x4f);
	else if(!enable)
#elif(DM_ODM_SUPPORT_TYPE==ODM_ADSL)      
	if(!enable)                                
#endif
		ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (((OPMODE & WIFI_AP_STATE)?6:10) << 12) | (4 << 8)
			| (sifs_time + 3 * slot_time));
	else
	{
		int txop_enlarge;
		int txop;
		unsigned int cw_max;
		unsigned int txop_close;
		
	#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined LOW_TP_TXOP))
			cw_max = ((priv->pshare->BE_cwmax_enhance) ? 10 : 6);
			txop_close = ((priv->pshare->rf_ft_var.low_tp_txop && priv->pshare->rf_ft_var.low_tp_txop_close) ? 1 : 0);

			if(priv->pshare->txop_enlarge == 0xe)  
				txop = (txop_close ? 0 : (BE_TXOP*2));
			else                                                       
				txop = (txop_close ? 0: (BE_TXOP*priv->pshare->txop_enlarge));
	#else
			cw_max=6;
			if((priv->pshare->txop_enlarge==0xe)||(priv->pshare->txop_enlarge==0xd))
				txop=BE_TXOP*2;
			else
				txop=BE_TXOP*priv->pshare->txop_enlarge;

	#endif
                           
		if (priv->pshare->ht_sta_num
	#ifdef WDS
			|| ((OPMODE & WIFI_AP_STATE) && (mode & (ODM_WM_N24G|ODM_WM_N5G)) &&
			priv->pmib->dot11WdsInfo.wdsEnabled && priv->pmib->dot11WdsInfo.wdsNum)
	#endif
			) 
			{

			if (priv->pshare->txop_enlarge == 0xe) {
				ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop<< 16) | (cw_max<< 12) | (4 << 8) | 0x1f);
				priv->pshare->txop_enlarge = 2;
			} 
#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
	#ifndef LOW_TP_TXOP
			 else if (priv->pshare->txop_enlarge == 0xd) {
				ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) | (4 << 12) | (3 << 8) | 0x19);
				priv->pshare->txop_enlarge = 2;
			} 
	#endif
#endif
			else 
			{
				if (pDM_Odm->RFType==ODM_2T2R)
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) |
						(cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time));
				else
				#if(DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined LOW_TP_TXOP)
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) |
						(((priv->pshare->BE_cwmax_enhance) ? 10 : 5) << 12) | (3 << 8) | (sifs_time + 2 * slot_time));
				#else
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) |
						(5 << 12) | (3 << 8) | (sifs_time + 2 * slot_time));

				#endif
			}
		}
              else 
              {
 #if((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined LOW_TP_TXOP))
			 ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (BE_TXOP << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time));
 #else
 		#if defined(CONFIG_RTL_8196D) || defined(CONFIG_RTL_8196E) || (defined(CONFIG_RTL_8197D) && !defined(CONFIG_PORT0_EXT_GIGA))
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM,  (BE_TXOP*2 << 16) | (cw_max << 12) | (5 << 8) | (sifs_time + 3 * slot_time));
		#else
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM,  (BE_TXOP*2 << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time));
		#endif
		
 #endif
              }

	}
}
#endif

VOID 
odm_IotEngine(
	IN	PDM_ODM_T	pDM_Odm
	)
{

	struct rtl8192cd_priv *priv=pDM_Odm->priv;
	PSTA_INFO_T pstat = NULL;
	u4Byte i;
	
#ifdef WIFI_WMM
	unsigned int switch_turbo = 0;
#endif	
	if(!(pDM_Odm->SupportAbility&ODM_MAC_EDCA_TURBO)){
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO, ODM_DBG_LOUD, ("ODM_MAC_EDCA_TURBO NOT SUPPORTED\n"));
		return;
	}
	
#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&& defined(RTL_MANUAL_EDCA) && defined(WIFI_WMM))
	if(priv->pmib->dot11QosEntry.ManualEDCA){
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO, ODM_DBG_LOUD, ("ODM_MAC_EDCA_TURBO OFF: MANUAL SETTING\n"));
		return ;
	}
#endif 

#if !(DM_ODM_SUPPORT_TYPE &ODM_AP)
	if ((GET_ROOT(priv)->up_time % 2) == 0) 
		priv->pshare->highTP_found_pstat==NULL;

#if 0
	phead = &priv->asoc_list;
	plist = phead->next;
	while(plist != phead)	{
		pstat = list_entry(plist, struct stat_info, asoc_list);

		if(ODM_ChooseIotMainSTA(pDM_Odm, pstat));             
			break;
		if (plist == plist->next)                                         
			break;
		plist = plist->next;
	};
#endif

	for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++) {
		pstat = pDM_Odm->pODM_StaInfo[i];
		if(IS_STA_VALID(pstat) && (ODM_ChooseIotMainSTA(pDM_Odm, pstat)))	
				break;
	}

	if(priv->pshare->highTP_found_pstat==NULL)	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO, ODM_DBG_LOUD, ("ODM_MAC_EDCA_TURBO OFF: NO HT STA FOUND\n"));
		return;
	}
#endif

	pstat=priv->pshare->highTP_found_pstat;


#ifdef WIFI_WMM
	if (QOS_ENABLE) {
		if (!priv->pmib->dot11OperationEntry.wifi_specific 
		#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
			||((OPMODE & WIFI_AP_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific == 2))
		#elif(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
			|| (priv->pmib->dot11OperationEntry.wifi_specific == 2)
		#endif
			) {
			if (priv->pshare->iot_mode_enable &&
				((priv->pshare->phw->VO_pkt_count > 50) ||
				 (priv->pshare->phw->VI_pkt_count > 50) ||
				 (priv->pshare->phw->BK_pkt_count > 50))) {
				priv->pshare->iot_mode_enable = 0;
				switch_turbo++;
			} else if ((!priv->pshare->iot_mode_enable) &&
				((priv->pshare->phw->VO_pkt_count < 50) &&
				 (priv->pshare->phw->VI_pkt_count < 50) &&
				 (priv->pshare->phw->BK_pkt_count < 50))) {
				priv->pshare->iot_mode_enable++;
				switch_turbo++;
			}
		}


		#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
		if ((OPMODE & WIFI_AP_STATE) && priv->pmib->dot11OperationEntry.wifi_specific)
		#elif (DM_ODM_SUPPORT_TYPE==ODM_ADSL)
		if (priv->pmib->dot11OperationEntry.wifi_specific) 
		#endif
		{
			if (!priv->pshare->iot_mode_VO_exist && (priv->pshare->phw->VO_pkt_count > 50)) {
				priv->pshare->iot_mode_VO_exist++;
				switch_turbo++;
			} else if (priv->pshare->iot_mode_VO_exist && (priv->pshare->phw->VO_pkt_count < 50)) {
				priv->pshare->iot_mode_VO_exist = 0;
				switch_turbo++;
			}
#if((DM_ODM_SUPPORT_TYPE==ODM_ADSL)||((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined WMM_VIBE_PRI)))
			if (priv->pshare->iot_mode_VO_exist) {
				if (!priv->pshare->iot_mode_BE_exist && (priv->pshare->phw->BE_pkt_count > 250)) {
					priv->pshare->iot_mode_BE_exist++;
					switch_turbo++;
				} else if (priv->pshare->iot_mode_BE_exist && (priv->pshare->phw->BE_pkt_count < 250)) {
					priv->pshare->iot_mode_BE_exist = 0;
					switch_turbo++;
				}
			}
#endif

#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
			if (priv->pshare->rf_ft_var.wifi_beq_iot) 
			{
				if (!priv->pshare->iot_mode_VI_exist && (priv->pshare->phw->VI_rx_pkt_count > 50)) {
					priv->pshare->iot_mode_VI_exist++;
					switch_turbo++;
				} else if (priv->pshare->iot_mode_VI_exist && (priv->pshare->phw->VI_rx_pkt_count < 50)) {
					priv->pshare->iot_mode_VI_exist = 0;
					switch_turbo++;
				}
			}
#endif

		}
		else if (!pstat || pstat->rssi < priv->pshare->rf_ft_var.txop_enlarge_lower) {
		   if (priv->pshare->txop_enlarge) {
			   priv->pshare->txop_enlarge = 0;
			   if (priv->pshare->iot_mode_enable)
					switch_turbo++;
				}
         	}

#if(defined(CLIENT_MODE) && (DM_ODM_SUPPORT_TYPE==ODM_AP))
        if ((OPMODE & WIFI_STATION_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific == 2))
        {
            if (priv->pshare->iot_mode_enable &&
                (((priv->pshare->phw->VO_pkt_count > 50) ||
                 (priv->pshare->phw->VI_pkt_count > 50) ||
                 (priv->pshare->phw->BK_pkt_count > 50)) ||
                 (pstat && (!pstat->ADDBA_ready[0]) & (!pstat->ADDBA_ready[3]))))
            {
                priv->pshare->iot_mode_enable = 0;
                switch_turbo++;
            }
            else if ((!priv->pshare->iot_mode_enable) &&
                (((priv->pshare->phw->VO_pkt_count < 50) &&
                 (priv->pshare->phw->VI_pkt_count < 50) &&
                 (priv->pshare->phw->BK_pkt_count < 50)) &&
                 (pstat && (pstat->ADDBA_ready[0] | pstat->ADDBA_ready[3]))))
            {
                priv->pshare->iot_mode_enable++;
                switch_turbo++;
            }
        }
#endif

		priv->pshare->phw->VO_pkt_count = 0;
		priv->pshare->phw->VI_pkt_count = 0;
		priv->pshare->phw->BK_pkt_count = 0;

	#if((DM_ODM_SUPPORT_TYPE==ODM_ADSL)||((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined WMM_VIBE_PRI)))
		priv->pshare->phw->BE_pkt_count = 0;
	#endif
		
	#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
		if (priv->pshare->rf_ft_var.wifi_beq_iot)
			priv->pshare->phw->VI_rx_pkt_count = 0;
		#endif

	}
#endif

	if ((priv->up_time % 2) == 0) {
		/*
		 * decide EDCA content for different chip vendor
		 */
#ifdef WIFI_WMM
	#if(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
		if (QOS_ENABLE && (!priv->pmib->dot11OperationEntry.wifi_specific || (priv->pmib->dot11OperationEntry.wifi_specific == 2)
	
	#elif(DM_ODM_SUPPORT_TYPE==ODM_AP)
		if (QOS_ENABLE && (!priv->pmib->dot11OperationEntry.wifi_specific || 
			((OPMODE & WIFI_AP_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific == 2))
		#ifdef CLIENT_MODE
            || ((OPMODE & WIFI_STATION_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific == 2))
		#endif
	#endif
		))
	
		{

			if (pstat && pstat->rssi >= priv->pshare->rf_ft_var.txop_enlarge_upper) {
#ifdef LOW_TP_TXOP
#if (DM_ODM_SUPPORT_TYPE &ODM_AP) && defined(USE_OUT_SRC)
				if (pstat->IOTPeer==HT_IOT_PEER_INTEL)
#else
				if (pstat->is_intel_sta)
#endif					
				{
					if (priv->pshare->txop_enlarge != 0xe)
					{
						priv->pshare->txop_enlarge = 0xe;

						if (priv->pshare->iot_mode_enable)
							switch_turbo++;
					}
				} 
				else if (priv->pshare->txop_enlarge != 2) 
				{
					priv->pshare->txop_enlarge = 2;
					if (priv->pshare->iot_mode_enable)
						switch_turbo++;
				}
#else
				if (priv->pshare->txop_enlarge != 2)
				{
#if (DM_ODM_SUPPORT_TYPE &ODM_AP) && defined(USE_OUT_SRC)
					if (pstat->IOTPeer==HT_IOT_PEER_INTEL)
#else				
					if (pstat->is_intel_sta)
#endif						
						priv->pshare->txop_enlarge = 0xe;						
#if (DM_ODM_SUPPORT_TYPE &ODM_AP) && defined(USE_OUT_SRC)
					else if (pstat->IOTPeer==HT_IOT_PEER_RALINK)
#else
					else if (pstat->is_ralink_sta)
#endif						
						priv->pshare->txop_enlarge = 0xd;						
					else
						priv->pshare->txop_enlarge = 2;

					if (priv->pshare->iot_mode_enable)
						switch_turbo++;
				}
#endif
#if 0
				if (priv->pshare->txop_enlarge != 2) 
				{
				#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
					if (pstat->IOTPeer==HT_IOT_PEER_INTEL)
				#else
					if (pstat->is_intel_sta)
				#endif					
						priv->pshare->txop_enlarge = 0xe;
				#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
					else if (pstat->IOTPeer==HT_IOT_PEER_RALINK)
						priv->pshare->txop_enlarge = 0xd;						
				#endif
					else
						priv->pshare->txop_enlarge = 2;
					if (priv->pshare->iot_mode_enable)
						switch_turbo++;
				}
#endif
			}
			else if (!pstat || pstat->rssi < priv->pshare->rf_ft_var.txop_enlarge_lower) 
			{
				if (priv->pshare->txop_enlarge) {
					priv->pshare->txop_enlarge = 0;
					if (priv->pshare->iot_mode_enable)
						switch_turbo++;
				}
			}

#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&&( defined LOW_TP_TXOP))
			if (pstat && pstat->is_intel_sta && (((pstat->tx_avarage+pstat->rx_avarage)>>10) < 
					priv->pshare->rf_ft_var.cwmax_enhance_thd)) 
			{
				if (!priv->pshare->BE_cwmax_enhance && priv->pshare->iot_mode_enable)
				{
					priv->pshare->BE_cwmax_enhance = 1;
					switch_turbo++;
				}
			} else {
				if (priv->pshare->BE_cwmax_enhance) {
					priv->pshare->BE_cwmax_enhance = 0;
					switch_turbo++;
				}
			}
#endif
		}
#endif
		priv->pshare->current_tx_bytes = 0;
		priv->pshare->current_rx_bytes = 0;
	}
	
#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&& defined( SW_TX_QUEUE))
	if ((priv->assoc_num > 1) && (AMPDU_ENABLE))
   	{
       	if (priv->swq_txmac_chg >= priv->pshare->rf_ft_var.swq_en_highthd){
			if ((priv->swq_en == 0)){
				switch_turbo++;
				if (priv->pshare->txop_enlarge == 0)
					priv->pshare->txop_enlarge = 2;
				priv->swq_en = 1;
				}
			else
			{
				if ((switch_turbo > 0) && (priv->pshare->txop_enlarge == 0) && (priv->pshare->iot_mode_enable != 0))
				{
					priv->pshare->txop_enlarge = 2;
					switch_turbo--;
				}
			}
		}
		else if(priv->swq_txmac_chg <= priv->pshare->rf_ft_var.swq_dis_lowthd){
			priv->swq_en = 0;
		}
		else if ((priv->swq_en == 1) && (switch_turbo > 0) && (priv->pshare->txop_enlarge == 0) && (priv->pshare->iot_mode_enable != 0))        {
			priv->pshare->txop_enlarge = 2;
			switch_turbo--;
		}
    }
#if ((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined CONFIG_RTL_819XD))
    else if( (priv->assoc_num == 1) && (AMPDU_ENABLE)) {		
        if (pstat) {
			int en_thd = 14417920>>(priv->up_time % 2);
            if ((priv->swq_en == 0) && (pstat->current_tx_bytes > en_thd) && (pstat->current_rx_bytes > en_thd) )  {
                priv->swq_en = 1;
				priv->swqen_keeptime = priv->up_time;
            }
            else if ((priv->swq_en == 1) && ((pstat->tx_avarage < 4587520) || (pstat->rx_avarage < 4587520))) {
                priv->swq_en = 0;
				priv->swqen_keeptime = 0;
            }
        }
        else {
            priv->swq_en = 0;
			priv->swqen_keeptime = 0;
        }
    }
#endif
#endif

#ifdef WIFI_WMM
#ifdef LOW_TP_TXOP
	if ((!priv->pmib->dot11OperationEntry.wifi_specific || (priv->pmib->dot11OperationEntry.wifi_specific == 2))
		&& QOS_ENABLE) {
		if (switch_turbo || priv->pshare->rf_ft_var.low_tp_txop) {
			unsigned int thd_tp;
			unsigned char under_thd;
			unsigned int curr_tp;

			if (priv->pmib->dot11BssType.net_work_type & (ODM_WM_N24G|ODM_WM_N5G| ODM_WM_G))
			{
				if (priv->pmib->dot11BssType.net_work_type & (ODM_WM_N24G|ODM_WM_N5G)) {
					if (priv->assoc_num && priv->assoc_num != priv->pshare->ht_sta_num)
						thd_tp = priv->pshare->rf_ft_var.low_tp_txop_thd_g;
					else
						thd_tp = priv->pshare->rf_ft_var.low_tp_txop_thd_n;
				}
				else
					thd_tp = priv->pshare->rf_ft_var.low_tp_txop_thd_g;

				curr_tp = (unsigned int)(priv->ext_stats.tx_avarage>>17) + (unsigned int)(priv->ext_stats.rx_avarage>>17);
				if (curr_tp <= thd_tp && curr_tp >= priv->pshare->rf_ft_var.low_tp_txop_thd_low)
					under_thd = 1;
				else
					under_thd = 0;
			}
			else
			{
				under_thd = 0;
			}

			if (switch_turbo) 
			{
				priv->pshare->rf_ft_var.low_tp_txop_close = under_thd;
				priv->pshare->rf_ft_var.low_tp_txop_count = 0;
			}
			else if (priv->pshare->iot_mode_enable && (priv->pshare->rf_ft_var.low_tp_txop_close != under_thd)) {
				priv->pshare->rf_ft_var.low_tp_txop_count++;
				if (priv->pshare->rf_ft_var.low_tp_txop_close) {
					priv->pshare->rf_ft_var.low_tp_txop_count = priv->pshare->rf_ft_var.low_tp_txop_delay;
				}
				if (priv->pshare->rf_ft_var.low_tp_txop_count ==priv->pshare->rf_ft_var.low_tp_txop_delay) 

				{					
					priv->pshare->rf_ft_var.low_tp_txop_count = 0;
					priv->pshare->rf_ft_var.low_tp_txop_close = under_thd;
					switch_turbo++;
				}
			} 
			else 
			{
				priv->pshare->rf_ft_var.low_tp_txop_count = 0;
			}
		}
	}
#endif		

	if (switch_turbo)
		ODM_IotEdcaSwitch( pDM_Odm, priv->pshare->iot_mode_enable );
#endif
}
#endif


#if( DM_ODM_SUPPORT_TYPE == ODM_WIN) 
BOOLEAN
ODM_CheckPowerStatus(
	IN	PADAPTER		Adapter)
{

	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T			pDM_Odm = &pHalData->DM_OutSrc;
	RT_RF_POWER_STATE 	rtState;
	PMGNT_INFO			pMgntInfo	= &(Adapter->MgntInfo);

	if (pMgntInfo->init_adpt_in_progress == TRUE)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ODM_CheckPowerStatus Return TRUE, due to initadapter"));
		return	TRUE;
	}
	
	Adapter->HalFunc.GetHwRegHandler(Adapter, HW_VAR_RF_STATE, (pu1Byte)(&rtState));	
	if(Adapter->bDriverStopped || Adapter->bDriverIsGoingToPnpSetPowerSleep || rtState == eRfOff)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ODM_CheckPowerStatus Return FALSE, due to %d/%d/%d\n", 
		Adapter->bDriverStopped, Adapter->bDriverIsGoingToPnpSetPowerSleep, rtState));
		return	FALSE;
	}
	return	TRUE;
}
#endif


#if ((DM_ODM_SUPPORT_TYPE == ODM_WIN)||(DM_ODM_SUPPORT_TYPE == ODM_CE))
u4Byte
GetPSDData(
	IN PDM_ODM_T	pDM_Odm,
	unsigned int 	point,
	u1Byte initial_gain_psd)
{
	u4Byte	psd_report;
	

	ODM_SetBBReg(pDM_Odm, 0x808, 0x3FF, point);

	ODM_SetBBReg(pDM_Odm, 0x808, BIT22, 1);
	ODM_StallExecution(1000);
	ODM_SetBBReg(pDM_Odm, 0x808, BIT22, 0);
	psd_report = ODM_GetBBReg(pDM_Odm,0x8B4, bMaskDWord) & 0x0000FFFF;
	
#if 1//(DEV_BUS_TYPE == RT_PCI_INTERFACE) && ( (RT_PLATFORM == PLATFORM_LINUX) || (RT_PLATFORM == PLATFORM_MACOSX))
	psd_report = (u4Byte) (ConvertTo_dB(psd_report))+(u4Byte)(initial_gain_psd-0x1c);
#else
	psd_report = (int) (20*log10((double)psd_report))+(int)(initial_gain_psd-0x1c);
#endif

	return psd_report;
	
}

u4Byte 
ConvertTo_dB(
	u4Byte 	Value)
{
	u1Byte i;
	u1Byte j;
	u4Byte dB;

	Value = Value & 0xFFFF;
	
	for (i=0;i<8;i++)
	{
		if (Value <= dB_Invert_Table[i][11])
		{
			break;
		}
	}

	if (i >= 8)
	{
		return (96);
	}

	for (j=0;j<12;j++)
	{
		if (Value <= dB_Invert_Table[i][j])
		{
			break;
		}
	}

	dB = i*12 + j + 1;

	return (dB);
}

#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

#define	AFH_PSD		1
#define	MODE_40M		0
#define	PSD_TH2		3  
#define	PSD_CHMIN		20  
#define	SIR_STEP_SIZE	3
#define   Smooth_Size_1 	5
#define	Smooth_TH_1	3
#define   Smooth_Size_2 	10
#define	Smooth_TH_2	4
#define   Smooth_Size_3 	20
#define	Smooth_TH_3	4
#define   Smooth_Step_Size 5
#define	Adaptive_SIR	1
#define	SCAN_INTERVAL	1500
#define	SYN_Length		5   
	
#define	LNA_Low_Gain_1                      0x64
#define	LNA_Low_Gain_2                      0x5A
#define	LNA_Low_Gain_3                      0x58

#define	pw_th_10dB					0x0
#define	pw_th_16dB					0x3

#define	FA_RXHP_TH1                           5000
#define	FA_RXHP_TH2                           1500
#define	FA_RXHP_TH3                             800
#define	FA_RXHP_TH4                             600
#define	FA_RXHP_TH5                             500

#define	Idle_Mode					0
#define	High_TP_Mode				1
#define	Low_TP_Mode				2


VOID
odm_PSDMonitorInit(
	IN PDM_ODM_T	pDM_Odm)
{
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)|(DEV_BUS_TYPE == RT_USB_INTERFACE)
	ODM_SetBBReg(pDM_Odm, ODM_PSDREG, BIT10|BIT11, 0x3);
	ODM_SetBBReg(pDM_Odm, ODM_PSDREG, BIT12|BIT13, 0x1);
	pDM_Odm->bPSDinProcess = FALSE;
	pDM_Odm->bUserAssignLevel = FALSE;
	pDM_Odm->bPSDactive = FALSE;

#endif
}

VOID
PatchDCTone(
	IN	PDM_ODM_T	pDM_Odm,
	pu4Byte		PSD_report,
	u1Byte 		initial_gain_psd
)
{
	
	u4Byte	psd_report;

	ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, 0x3FF, 11);
	
	if(pDM_Odm->SupportICType== ODM_RTL8192D)
	{
		if((*(pDM_Odm->pMacPhyMode) == ODM_SMSP)||(*(pDM_Odm->pMacPhyMode) == ODM_DMSP))
		{
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, RF_CHNLBW, 0x3FF, 11);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x25, 0xfffff, 0x643BC);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x26, 0xfffff, 0xFC038);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x27, 0xfffff, 0x77C1A);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x2B, 0xfffff, 0x41289);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x2C, 0xfffff, 0x01840);
		}
		else
		{
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x25, 0xfffff, 0x643BC);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x26, 0xfffff, 0xFC038);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x27, 0xfffff, 0x77C1A);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x2B, 0xfffff, 0x41289);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x2C, 0xfffff, 0x01840);
		}
	}
	
	psd_report = GetPSDData(pDM_Odm, 96, initial_gain_psd);
	PSD_report[50] = psd_report;
	psd_report = GetPSDData(pDM_Odm, 32, initial_gain_psd);
	PSD_report[70] = psd_report;
	
	ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, 0x3FF, 3);

	
	if(pDM_Odm->SupportICType==ODM_RTL8192D)
	{
		if((*(pDM_Odm->pMacPhyMode) == ODM_SMSP)||(*(pDM_Odm->pMacPhyMode) == ODM_DMSP))
		{
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, RF_CHNLBW, 0x3FF, 3);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x27, 0xfffff, 0x07C1A);
		}
		else
		{
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x27, 0xfffff, 0x07C1A);
		}
	}
	
	psd_report = GetPSDData(pDM_Odm, 96, initial_gain_psd);
	PSD_report[10] = psd_report;
	psd_report = GetPSDData(pDM_Odm, 32, initial_gain_psd);
	PSD_report[30] = psd_report;

}


VOID
GoodChannelDecision(
	PDM_ODM_T	pDM_Odm,
	pu4Byte		PSD_report,
	pu1Byte		PSD_bitmap,
	u1Byte 		RSSI_BT,
	pu1Byte		PSD_bitmap_memory)
{
	pRXHP_T			pRX_HP_Table = &pDM_Odm->DM_RXHP_Table;
	s4Byte	TH1= RSSI_BT+0x14;
	s4Byte	TH2 = RSSI_BT+85;
	u1Byte	bitmap, Smooth_size[3], Smooth_TH[3];
	u4Byte	i,n,j, byte_idx, bit_idx, good_cnt, good_cnt_smoothing, Smooth_Interval[3];
	int 		start_byte_idx,start_bit_idx,cur_byte_idx, cur_bit_idx,NOW_byte_idx ;
	

	if((pDM_Odm->SupportICType == ODM_RTL8192C)||(pDM_Odm->SupportICType == ODM_RTL8192D))
       {
            TH1 = RSSI_BT + 0x14;  
	}

	Smooth_size[0]=Smooth_Size_1;
	Smooth_size[1]=Smooth_Size_2;
	Smooth_size[2]=Smooth_Size_3;
	Smooth_TH[0]=Smooth_TH_1;
	Smooth_TH[1]=Smooth_TH_2;
	Smooth_TH[2]=Smooth_TH_3;
	Smooth_Interval[0]=16;
	Smooth_Interval[1]=15;
	Smooth_Interval[2]=13;
	good_cnt = 0;
	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{

		if(RSSI_BT >=41)
			TH1 = 113;	
		else if(RSSI_BT >=38)  
			TH1 = 105;                             
		else if((RSSI_BT >=33)&(RSSI_BT <38))
			TH1 = 99+(RSSI_BT-33);        
		else if((RSSI_BT >=26)&(RSSI_BT<33))
			TH1 = 99-(33-RSSI_BT)+2;    
	 	else if((RSSI_BT >=24)&(RSSI_BT<26))
			TH1 = 88-((RSSI_BT-24)*3);  
		else if((RSSI_BT >=18)&(RSSI_BT<24))
			TH1 = 77+((RSSI_BT-18)*2);
		else if((RSSI_BT >=14)&(RSSI_BT<18))
			TH1 = 63+((RSSI_BT-14)*2);
		else if((RSSI_BT >=8)&(RSSI_BT<14))
			TH1 = 58+((RSSI_BT-8)*2);
		else if((RSSI_BT >=3)&(RSSI_BT<8))
			TH1 = 52+(RSSI_BT-3);
		else
			TH1 = 51;
	}

	for (i = 0; i< 10; i++)
		PSD_bitmap[i] = 0;
	

       for (i=0; i<80; i++)
	   	pRX_HP_Table->PSD_bitmap_RXHP[i] = 0;



	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
		TH1 =TH1-SIR_STEP_SIZE;
	}
	while (good_cnt < PSD_CHMIN)
	{
		good_cnt = 0;
		if(pDM_Odm->SupportICType==ODM_RTL8723A)
		{
		if(TH1 ==TH2)
			break;
		if((TH1+SIR_STEP_SIZE) < TH2)
			TH1 += SIR_STEP_SIZE;
		else
			TH1 = TH2;
		}
		else
		{
			if(TH1==(RSSI_BT+0x1E))
             		     break;    
   			if((TH1+2) < (RSSI_BT+0x1E))
				TH1+=3;
		     	else
				TH1 = RSSI_BT+0x1E;	
             
		}
		ODM_RT_TRACE(pDM_Odm,COMP_PSD,DBG_LOUD,("PSD: decision threshold is: %d", TH1));
			 
		for (i = 0; i< 80; i++)
		{
			if((s4Byte)(PSD_report[i]) < TH1)
			{
				byte_idx = i / 8;
				bit_idx = i -8*byte_idx;
				bitmap = PSD_bitmap[byte_idx];
				PSD_bitmap[byte_idx] = bitmap | (u1Byte) (1 << bit_idx);
			}
		}

#if DBG
		ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: before smoothing\n"));
		for(n=0;n<10;n++)
		{
			for (i = 0; i<8; i++)
				ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD_bitmap[%u] =   %d\n", 2402+n*8+i, (PSD_bitmap[n]&BIT(i))>>i));
		}
#endif
	

		for (j=0;j<3;j++)
		{
			start_byte_idx=0;
			start_bit_idx=0;
			for(n=0; n<Smooth_Interval[j]; n++)
			{
				good_cnt_smoothing = 0;
				cur_bit_idx = start_bit_idx;
				cur_byte_idx = start_byte_idx;
				for ( i=0; i < Smooth_size[j]; i++)
				{
					NOW_byte_idx = cur_byte_idx + (i+cur_bit_idx)/8;
					if ( (PSD_bitmap[NOW_byte_idx]& BIT( (cur_bit_idx + i)%8)) != 0)
						good_cnt_smoothing++;

				}

				if( good_cnt_smoothing < Smooth_TH[j] )
				{
					cur_bit_idx = start_bit_idx;
					cur_byte_idx = start_byte_idx;
					for ( i=0; i< Smooth_size[j] ; i++)
					{	
						NOW_byte_idx = cur_byte_idx + (i+cur_bit_idx)/8;				
						PSD_bitmap[NOW_byte_idx] = PSD_bitmap[NOW_byte_idx] & (~BIT( (cur_bit_idx + i)%8));
					}
				}
				start_bit_idx =  start_bit_idx + Smooth_Step_Size;
				while ( (start_bit_idx)  > 7 )
				{
					start_byte_idx= start_byte_idx+start_bit_idx/8;
					start_bit_idx = start_bit_idx%8;
				}
			}

			ODM_RT_TRACE(	pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: after %u smoothing", j+1));
			for(n=0;n<10;n++)
			{
				for (i = 0; i<8; i++)
				{
					ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD_bitmap[%u] =   %d\n", 2402+n*8+i, (PSD_bitmap[n]&BIT(i))>>i));
					
					if ( ((PSD_bitmap[n]&BIT(i))>>i) ==1) 
					{
	                                   pRX_HP_Table->PSD_bitmap_RXHP[8*n+i] = 1;
					}                                                 
				}
			}

		}

	
		good_cnt = 0;
		for ( i = 0; i < 10; i++)
		{
			for (n = 0; n < 8; n++)
				if((PSD_bitmap[i]& BIT(n)) != 0)
					good_cnt++;
		}
		ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: good channel cnt = %u",good_cnt));
	}

	for (i = 0; i <10; i++)
		ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: PSD_bitmap[%u]=%x",i,PSD_bitmap[i]));
/*	
	for(i = 0; i < 80; i++)
	{
		byte_idx = i / 8;
		bit_idx = i -8*byte_idx;
		psd_bit = (PSD_bitmap[byte_idx] & BIT(bit_idx)) >> bit_idx;
		bitmap = PSD_bitmap_memory[i]; 
		PSD_bitmap_memory[i] = (bitmap << 1) |psd_bit;
	}
*/
}



VOID
odm_PSD_Monitor(
	PDM_ODM_T	pDM_Odm
)
{

	unsigned int 		pts, start_point, stop_point;
	u1Byte			initial_gain ;
	static u1Byte		PSD_bitmap_memory[80], init_memory = 0;
	static u1Byte 		psd_cnt=0;
	static u4Byte		PSD_report[80], PSD_report_tmp;
	static u8Byte		lastTxOkCnt=0, lastRxOkCnt=0;
	u1Byte 			H2C_PSD_DATA[5]={0,0,0,0,0};
	static u1Byte		H2C_PSD_DATA_last[5] ={0,0,0,0,0};
	u1Byte			idx[20]={96,99,102,106,109,112,115,118,122,125,
					0,3,6,10,13,16,19,22,26,29};
	u1Byte			n, i, channel, BBReset,tone_idx;
	u1Byte			PSD_bitmap[10], SSBT=0,initial_gain_psd=0, RSSI_BT=0, initialGainUpper;
	s4Byte    			PSD_skip_start, PSD_skip_stop;
	u4Byte			CurrentChannel, RXIQI, RxIdleLowPwr, wlan_channel;
	u4Byte			ReScan, Interval, Is40MHz;
	u8Byte			curTxOkCnt, curRxOkCnt;
	int 				cur_byte_idx, cur_bit_idx;
	PADAPTER		Adapter = pDM_Odm->Adapter;
	PMGNT_INFO      	pMgntInfo = &Adapter->MgntInfo;
	
	if( (*(pDM_Odm->pbScanInProcess)) ||
		pDM_Odm->bLinkInProcess)
	{
		if((pDM_Odm->SupportICType==ODM_RTL8723A)&(pDM_Odm->SupportInterface==ODM_ITRF_PCIE))
		{
			ODM_SetTimer( pDM_Odm, &pDM_Odm->PSDTimer, 1500);
		}
		return;
	}

	if(pDM_Odm->bBtHsOperation)
	{
		ReScan = 1;
		Interval = SCAN_INTERVAL;
	}
	else
	{
	ReScan = PSD_RESCAN;
	Interval = SCAN_INTERVAL;
	}

	if(init_memory == 0)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD, DBG_LOUD,("Init memory\n"));
		for(i = 0; i < 80; i++)
			PSD_bitmap_memory[i] = 0xFF;
		init_memory = 1;
	}
	if(psd_cnt == 0)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD, DBG_LOUD,("Enter dm_PSD_Monitor\n"));
		for(i = 0; i < 80; i++)
			PSD_report[i] = 0;
	}

	CurrentChannel = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask);
/*
	if(pDM_Odm->SupportICType==ODM_RTL8192D)
	{
		if((*pDM_Odm->MacPhyMode92D == SINGLEMAC_SINGLEPHY)||(*pDM_Odm->MacPhyMode92D == DUALMAC_SINGLEPHY))
		{
			SYN_RF25 = ODM_GetRFReg(Adapter, ODM_RF_PATH_B, 0x25, bMaskDWord);
			SYN_RF26 = ODM_GetRFReg(Adapter, ODM_RF_PATH_B, 0x26, bMaskDWord);
			SYN_RF27 = ODM_GetRFReg(Adapter, ODM_RF_PATH_B, 0x27, bMaskDWord);
			SYN_RF2B = ODM_GetRFReg(Adapter, ODM_RF_PATH_B, 0x2B, bMaskDWord);
			SYN_RF2C = ODM_GetRFReg(Adapter, ODM_RF_PATH_B, 0x2C, bMaskDWord);
       	}
		else    
		{
			SYN_RF25 = ODM_GetRFReg(Adapter, ODM_RF_PATH_A, 0x25, bMaskDWord);
			SYN_RF26 = ODM_GetRFReg(Adapter, ODM_RF_PATH_A, 0x26, bMaskDWord);
			SYN_RF27 = ODM_GetRFReg(Adapter, ODM_RF_PATH_A, 0x27, bMaskDWord);
			SYN_RF2B = ODM_GetRFReg(Adapter, ODM_RF_PATH_A, 0x2B, bMaskDWord);
			SYN_RF2C = ODM_GetRFReg(Adapter, ODM_RF_PATH_A, 0x2C, bMaskDWord);
		}
	}
*/
	RXIQI = ODM_GetBBReg(pDM_Odm, 0xC14, bMaskDWord);

	RxIdleLowPwr = (ODM_GetBBReg(pDM_Odm, 0x818, bMaskDWord)&BIT28)>>28;

	if(CHNL_RUN_ABOVE_40MHZ(pMgntInfo))
		Is40MHz = TRUE;
	else
		Is40MHz = FALSE;

	ODM_RT_TRACE(pDM_Odm,	ODM_COMP_PSD, DBG_LOUD,("PSD Scan Start\n"));
	ODM_SetBBReg(pDM_Odm, rFPGA0_RFMOD, BIT24, 0);
	ODM_Write1Byte(pDM_Odm,REG_TXPAUSE, 0xFF);
	

	ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_AC, bRFRegOffsetMask, 0x32E13);

	ODM_SetBBReg(pDM_Odm, 0xC70, BIT0, 0);
	ODM_SetBBReg(pDM_Odm, 0xC7C, BIT20, 0);

	
	ODM_SetBBReg(pDM_Odm, 0xC14, bMaskDWord, 0x0);
	
	BBReset = ODM_Read1Byte(pDM_Odm, 0x02);
	
	ODM_SetBBReg(pDM_Odm, 0x87C, BIT31, 1);
	ODM_Write1Byte(pDM_Odm,  0x02, BBReset&(~BIT0));
	ODM_Write1Byte(pDM_Odm,  0x02, BBReset|BIT0);
	ODM_SetBBReg(pDM_Odm, 0x87C, BIT31, 0);
	

	ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 0x0);

	if((pDM_Odm->SupportICType==ODM_RTL8723A)&(pDM_Odm->SupportInterface==ODM_ITRF_PCIE))
	RSSI_BT=pDM_Odm->RSSI_BT;	

	if(RSSI_BT>=47)
		RSSI_BT=47;
	   
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD, DBG_LOUD,("PSD: RSSI_BT= %d\n", RSSI_BT));
	
	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
		if(RSSI_BT >=35)  
			initial_gain_psd = RSSI_BT*2;
		else if((RSSI_BT >=33)&(RSSI_BT<35))
			initial_gain_psd = RSSI_BT*2+6;
		else if((RSSI_BT >=24)&(RSSI_BT<33))
			initial_gain_psd = 70-(33-RSSI_BT);
	 	else if((RSSI_BT >=19)&(RSSI_BT<24))
			initial_gain_psd = 64-((24-RSSI_BT)*4);
		else if((RSSI_BT >=14)&(RSSI_BT<19))
			initial_gain_psd = 44-((18-RSSI_BT)*2);
		else if((RSSI_BT >=8)&(RSSI_BT<14))
			initial_gain_psd = 35-(14-RSSI_BT);
		else
			initial_gain_psd = 0x1B;
	}
	else
	{
	
         	initial_gain_psd = pDM_Odm->RSSI_Min;   
	}
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD, DBG_LOUD,("PSD: RSSI_BT= %d\n", RSSI_BT));

	
	if(pDM_Odm->bUserAssignLevel)
	{
		pDM_Odm->bUserAssignLevel = FALSE;
		initialGainUpper = 0x7f;
	}
	else
	{
		initialGainUpper = 0x5E;
	}
	
	/*
	if (initial_gain_psd < 0x1a)
		initial_gain_psd = 0x1a;
	if (initial_gain_psd > initialGainUpper)
		initial_gain_psd = initialGainUpper;
	*/

	SSBT = RSSI_BT  * 2 +0x3E;
	
	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD, DBG_LOUD,("PSD: SSBT= %d\n", SSBT));
	ODM_RT_TRACE(	pDM_Odm,ODM_COMP_PSD, DBG_LOUD,("PSD: initial gain= 0x%x\n", initial_gain_psd));
	pDM_Odm->bDMInitialGainEnable = FALSE;
	initial_gain =(u1Byte) (ODM_GetBBReg(pDM_Odm, 0xc50, bMaskDWord) & 0x7F);
	
	ODM_Write_DIG(pDM_Odm, initial_gain_psd);
	ODM_SetBBReg(pDM_Odm, 0x88c, BIT20|BIT21|BIT22|BIT23, 0xF);

	pts = 128;

	if(pts == 128)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x0);
		start_point = 64;
		stop_point = 192;
	}
	else if(pts == 256)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x1);
		start_point = 128;
		stop_point = 384;
	}
	else if(pts == 512)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x2);
		start_point = 256;
		stop_point = 768;
	}
	else
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x3);
		start_point = 512;
		stop_point = 1536;
	}
	


	curTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast) - lastTxOkCnt;
	curRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast) - lastRxOkCnt;
	lastTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast);
	lastRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast);	

	PSD_skip_start=80;
	PSD_skip_stop = 0;
	wlan_channel = CurrentChannel & 0x0f;

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD,DBG_LOUD,("PSD: current channel: %x, BW:%d \n", wlan_channel, Is40MHz));
	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
		if(pDM_Odm->bBtHsOperation)
		{
			if(pDM_Odm->bLinked)
			{
				if(Is40MHz)
				{
					PSD_skip_start = ((wlan_channel-1)*5 -Is40MHz*10)-2; 
					PSD_skip_stop = (PSD_skip_start + (1+Is40MHz)*20)+4;
				}
				else
				{
					PSD_skip_start = ((wlan_channel-1)*5 -Is40MHz*10)-10; 
					PSD_skip_stop = (PSD_skip_start + (1+Is40MHz)*20)+18; 
				}
			}
			else
			{
				PSD_skip_start = ((wlan_channel-1)*5 -Is40MHz*10)-2; 
				PSD_skip_stop = (PSD_skip_start + (1+Is40MHz)*20)+4;
			}
			if(PSD_skip_start < 0)
				PSD_skip_start = 0;
			if(PSD_skip_stop >80)
				PSD_skip_stop = 80;
		}
		else
		{
			if((curRxOkCnt+curTxOkCnt) > 5)
			{
				if(Is40MHz)
				{
					PSD_skip_start = ((wlan_channel-1)*5 -Is40MHz*10)-2; 
					PSD_skip_stop = (PSD_skip_start + (1+Is40MHz)*20)+4;
				}
				else
				{
					PSD_skip_start = ((wlan_channel-1)*5 -Is40MHz*10)-10; 
					PSD_skip_stop = (PSD_skip_start + (1+Is40MHz)*20)+18; 
				}
				
				if(PSD_skip_start < 0)
					PSD_skip_start = 0;
				if(PSD_skip_stop >80)
					PSD_skip_stop = 80;
			}
		}
	}
#if 0	
	else
	{
		if((curRxOkCnt+curTxOkCnt) > 1000)
		{
			PSD_skip_start = (wlan_channel-1)*5 -Is40MHz*10;
			PSD_skip_stop = PSD_skip_start + (1+Is40MHz)*20;
		}
	}   
#endif 
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD,DBG_LOUD,("PSD: Skip tone from %d to %d \n", PSD_skip_start, PSD_skip_stop));

 	for (n=0;n<80;n++)
 	{
 		if((n%20)==0)
 		{
			channel = (n/20)*4 + 1;
					
					ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
				}
		tone_idx = n%20;
		if ((n>=PSD_skip_start) && (n<PSD_skip_stop))
		{	
			PSD_report[n] = SSBT;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD,DBG_LOUD,("PSD:Tone %d skipped \n", n));
		}
		else
		{
			PSD_report_tmp =  GetPSDData(pDM_Odm, idx[tone_idx], initial_gain_psd);

			if ( PSD_report_tmp > PSD_report[n])
				PSD_report[n] = PSD_report_tmp;
				
		}
	}

	PatchDCTone(pDM_Odm, PSD_report, initial_gain_psd);
      
	ODM_SetBBReg(pDM_Odm, 0xC70, BIT0, 1);
	ODM_SetBBReg(pDM_Odm, 0xC7C, BIT20, 1);
	ODM_SetBBReg(pDM_Odm, rFPGA0_RFMOD, BIT24, 1);
	
	ODM_Write1Byte(pDM_Odm,REG_TXPAUSE, 0x00);
	ODM_SetBBReg(pDM_Odm, 0x88c, BIT20|BIT21|BIT22|BIT23, 0x0);
	pDM_Odm->bDMInitialGainEnable = TRUE;
	
	ODM_Write_DIG(pDM_Odm, initial_gain);

	ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask, CurrentChannel);

	ODM_SetBBReg(pDM_Odm, 0xC14, bMaskDWord, RXIQI);
	if(RxIdleLowPwr == TRUE)
		ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 1);
	
	psd_cnt++;
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD, DBG_LOUD,("PSD:psd_cnt = %d \n",psd_cnt));
	if (psd_cnt < ReScan)
		ODM_SetTimer(pDM_Odm, &pDM_Odm->PSDTimer, Interval);		
	else
	{
		psd_cnt = 0;
		for(i=0;i<80;i++)
			RT_TRACE(	COMP_PSD, DBG_LOUD,("psd_report[%d]=     %d \n", 2402+i, PSD_report[i]));


		GoodChannelDecision(pDM_Odm, PSD_report, PSD_bitmap,RSSI_BT, PSD_bitmap_memory);

		if(pDM_Odm->SupportICType==ODM_RTL8723A)
		{
			cur_byte_idx=0;
			cur_bit_idx=0;

		  	H2C_PSD_DATA_last[0] = H2C_PSD_DATA[0];
			H2C_PSD_DATA_last[1] = H2C_PSD_DATA[1];
			H2C_PSD_DATA_last[2] = H2C_PSD_DATA[2];
			H2C_PSD_DATA_last[3] = H2C_PSD_DATA[3];
			H2C_PSD_DATA_last[4] = H2C_PSD_DATA[4];

	
			for ( i=0;i<5;i++)
			{
				for(n=0;n<8;n++)
				{
					cur_byte_idx = i*2 + n/4;
					cur_bit_idx = (n%4)*2;
					if ( ((PSD_bitmap[cur_byte_idx]& BIT(cur_bit_idx)) != 0) && ((PSD_bitmap[cur_byte_idx]& BIT(cur_bit_idx+1)) != 0))
						H2C_PSD_DATA[i] = H2C_PSD_DATA[i] | (u1Byte) (1 << n);
				}
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD, DBG_LOUD,("H2C_PSD_DATA[%d]=0x%x\n" ,i, H2C_PSD_DATA[i]));
			}
	
			for ( i=0;i<5;i++)
			{
				if(H2C_PSD_DATA[i] !=H2C_PSD_DATA_last[i])
				{
					FillH2CCmd(Adapter, H2C_92C_PSD_RESULT, 5, H2C_PSD_DATA);
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_PSD, DBG_LOUD,("Need to Update the AFH Map \n"));
					break;
				}
				else
				{
					if(i==5)
						ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD, DBG_LOUD,("Not need to Update\n"));	
				}
			}
			if(pDM_Odm->bBtHsOperation)
			{
				ODM_SetTimer(pDM_Odm, &pDM_Odm->PSDTimer, 10000);
				ODM_RT_TRACE(	pDM_Odm,ODM_COMP_PSD, DBG_LOUD,("Leave dm_PSD_Monitor\n"));		
			}
			else
			{
				ODM_SetTimer(pDM_Odm, &pDM_Odm->PSDTimer, 1500);
				ODM_RT_TRACE(	pDM_Odm,ODM_COMP_PSD, DBG_LOUD,("Leave dm_PSD_Monitor\n"));		
		}
	}
    }
}
/*
VOID
ODM_PSDGetRSSI(
	IN	u1Byte	RSSI_BT)
{


}

*/

VOID
ODM_PSDMonitor(
	IN	PDM_ODM_T	pDM_Odm
	)
{
	
	
	if(pDM_Odm->SupportICType == ODM_RTL8723A)  
	{
		if(pDM_Odm->SupportInterface==ODM_ITRF_PCIE)
		{
			if(pDM_Odm->bBtDisabled)
			{
				pDM_Odm->bPSDactive=FALSE;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD, DBG_LOUD, ("odm_PSDMonitor, return for BT is disabled!!!\n"));
		   		return; 
			}

			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PSD, DBG_LOUD, ("odm_PSDMonitor\n"));
			pDM_Odm->bPSDinProcess = TRUE;
	 		pDM_Odm->bPSDactive=TRUE;
			odm_PSD_Monitor(pDM_Odm);
			pDM_Odm->bPSDinProcess = FALSE;
		}	
	}	

}
VOID
odm_PSDMonitorCallback(
	PRT_TIMER		pTimer
)
{
	PADAPTER		Adapter = (PADAPTER)pTimer->Adapter;
       HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	PlatformScheduleWorkItem(&pHalData->PSDMonitorWorkitem);
}

VOID
odm_PSDMonitorWorkItemCallback(
    IN PVOID            pContext
    )
{
	PADAPTER	Adapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;

	ODM_PSDMonitor(pDM_Odm);
}

VOID
odm_RFEControl(
	IN	PDM_ODM_T	pDM_Odm,
	IN  u8Byte		RSSIVal
	)
{
	PADAPTER		Adapter = (PADAPTER)pDM_Odm->Adapter;
    HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	static u1Byte 	TRSW_HighPwr = 0;
	 
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, DBG_LOUD, ("===> odm_RFEControl, RSSI = %d, TRSW_HighPwr = 0x%X, pHalData->RFEType = %d\n",
		         RSSIVal, TRSW_HighPwr, pHalData->RFEType ));

    if (pHalData->RFEType == 3) {	   
		
        pDM_Odm->RSSI_TRSW = RSSIVal;

        if (pDM_Odm->RSSI_TRSW >= pDM_Odm->RSSI_TRSW_H) 
		{				 
            TRSW_HighPwr = 1;
            PHY_SetBBReg(Adapter, r_ANTSEL_SW_Jaguar, BIT1|BIT0, 0x1); 
            PHY_SetBBReg(Adapter, r_ANTSEL_SW_Jaguar, BIT9|BIT8, 0x3); 
            
        } 
		else if (pDM_Odm->RSSI_TRSW <= pDM_Odm->RSSI_TRSW_L) 
        {	  
            TRSW_HighPwr = 0;
            PHY_SetBBReg(Adapter, r_ANTSEL_SW_Jaguar, BIT1|BIT0, 0x1); 
            PHY_SetBBReg(Adapter, r_ANTSEL_SW_Jaguar, BIT9|BIT8, 0x0); 

        }
    }  

	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, DBG_LOUD, ("(pDM_Odm->RSSI_TRSW_H, pDM_Odm->RSSI_TRSW_L) = (%d, %d)\n", pDM_Odm->RSSI_TRSW_H, pDM_Odm->RSSI_TRSW_L));		
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, DBG_LOUD, ("(RSSIVal, RSSIVal, pDM_Odm->RSSI_TRSW_iso) = (%d, %d, %d)\n", 
				 RSSIVal, pDM_Odm->RSSI_TRSW_iso, pDM_Odm->RSSI_TRSW));
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_DIG, DBG_LOUD, ("<=== odm_RFEControl, RSSI = %d, TRSW_HighPwr = 0x%X\n", RSSIVal, TRSW_HighPwr));	
}

VOID
ODM_MPT_DIG(
	IN	PDM_ODM_T	pDM_Odm
	)
{
	pDIG_T						pDM_DigTable = &pDM_Odm->DM_DigTable;
	PFALSE_ALARM_STATISTICS 	pFalseAlmCnt = &pDM_Odm->FalseAlmCnt;
	u1Byte						CurrentIGI = (u1Byte)pDM_DigTable->CurIGValue;
	u1Byte						DIG_Upper = 0x40, DIG_Lower = 0x20, C50, E50;
	u8Byte						RXOK_cal;
	u1Byte						IGI_A = 0x20, IGI_B = 0x20;

#if ODM_FIX_2G_DIG
	IGI_A = 0x22;
	IGI_B = 0x24;		
#endif

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_MP, DBG_LOUD, ("===> ODM_MPT_DIG, pBandType = %d\n", *pDM_Odm->pBandType));

	odm_FalseAlarmCounterStatistics( pDM_Odm);
	pDM_Odm->LastNumQryPhyStatusAll = pDM_Odm->NumQryPhyStatusAll;
	pDM_Odm->NumQryPhyStatusAll = pDM_Odm->PhyDbgInfo.NumQryPhyStatusCCK + pDM_Odm->PhyDbgInfo.NumQryPhyStatusOFDM;
	RXOK_cal = pDM_Odm->NumQryPhyStatusAll - pDM_Odm->LastNumQryPhyStatusAll;
	
	if (RXOK_cal == 0)
		pDM_Odm->RxPWDBAve_final= 0;
	else
		pDM_Odm->RxPWDBAve_final= pDM_Odm->RxPWDBAve/RXOK_cal;

	pDM_Odm->RxPWDBAve = 0;
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, DBG_LOUD, ("RX OK = %d\n", RXOK_cal));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, DBG_LOUD, ("pDM_Odm->RxPWDBAve_final = %d\n", pDM_Odm->RxPWDBAve_final));
	
#if ODM_FIX_2G_DIG
	if (*pDM_Odm->pBandType == BAND_ON_5G){	
#else
	if (1){
#endif
		pDM_Odm->MPDIG_2G = FALSE;
		pDM_Odm->Times_2G = 0;
	
		if (RXOK_cal >= 70 && pDM_Odm->RxPWDBAve_final<= 30)
		{
			if (CurrentIGI > 0x24){
				ODM_Write1Byte( pDM_Odm, rA_IGI_Jaguar, 0x24);
				ODM_Write1Byte( pDM_Odm, rB_IGI_Jaguar, 0x24);
			}
		}
		else
		{
			if(pFalseAlmCnt->Cnt_all > 1000){
				CurrentIGI = CurrentIGI + 8;
			}
			else if(pFalseAlmCnt->Cnt_all > 200){
				CurrentIGI = CurrentIGI + 4;
			}
			else if (pFalseAlmCnt->Cnt_all > 50){
				CurrentIGI = CurrentIGI + 2;
			}
			else if (pFalseAlmCnt->Cnt_all < 2){
				CurrentIGI = CurrentIGI - 2;
			}
			
			if (CurrentIGI < DIG_Lower ){
				CurrentIGI = DIG_Lower;
			}
			else if(CurrentIGI > DIG_Upper){
				CurrentIGI = DIG_Upper;
			}
			
			pDM_DigTable->CurIGValue = CurrentIGI;
			
			ODM_Write1Byte( pDM_Odm, rA_IGI_Jaguar, (u1Byte)CurrentIGI);
			ODM_Write1Byte( pDM_Odm, rB_IGI_Jaguar, (u1Byte)CurrentIGI);

			C50 = ODM_Read1Byte( pDM_Odm, 0xc50);
			E50 = ODM_Read1Byte( pDM_Odm, 0xe50);
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_MP, DBG_LOUD, ("DIG = (%x, %x), Cnt_all = %d, Cnt_Ofdm_fail = %d, Cnt_Cck_fail = %d\n", C50, E50, pFalseAlmCnt->Cnt_all, pFalseAlmCnt->Cnt_Ofdm_fail, pFalseAlmCnt->Cnt_Cck_fail));
		}
			
	}
	else
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_MP, DBG_LOUD, ("MPDIG_2G = %d,\n", pDM_Odm->MPDIG_2G));
		
		if(pDM_Odm->MPDIG_2G == FALSE){
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_MP, DBG_LOUD, ("===> Fix IGI\n"));
			ODM_Write1Byte( pDM_Odm, rA_IGI_Jaguar, (u1Byte)IGI_A);
			ODM_Write1Byte( pDM_Odm, rB_IGI_Jaguar, (u1Byte)IGI_B);
		}
		if (pDM_Odm->Times_2G == 2)
			pDM_Odm->MPDIG_2G = TRUE;
		pDM_Odm->Times_2G++;
	}
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, DBG_LOUD, ("pDM_Odm->RxPWDBAve_final = %d\n", pDM_Odm->RxPWDBAve_final));

	if (pDM_Odm->SupportICType == ODM_RTL8812)
		odm_RFEControl(pDM_Odm, pDM_Odm->RxPWDBAve_final);
	
	ODM_SetTimer(pDM_Odm, &pDM_Odm->MPT_DIGTimer, 700);
	
}		

VOID
odm_MPT_DIGCallback(
	PRT_TIMER		pTimer
)
{
	PADAPTER		Adapter = (PADAPTER)pTimer->Adapter;
       HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	  PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;


	#if DEV_BUS_TYPE==RT_PCI_INTERFACE
		#if USE_WORKITEM
			PlatformScheduleWorkItem(&pDM_Odm->MPT_DIGWorkitem);
		#else
			ODM_MPT_DIG(pDM_Odm);
		#endif
	#else
		PlatformScheduleWorkItem(&pDM_Odm->MPT_DIGWorkitem);
	#endif

}

VOID
odm_MPT_DIGWorkItemCallback(
    IN PVOID            pContext
    )
{
	PADAPTER	Adapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;

	ODM_MPT_DIG(pDM_Odm);
}





VOID
ODM_PSDDbgControl(
	IN	PADAPTER	Adapter,
	IN	u4Byte		mode,
	IN	u4Byte		btRssi
	)
{
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;

	ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD, (" Monitor mode=%d, btRssi=%d\n", mode, btRssi));
	if(mode)
	{
		pDM_Odm->RSSI_BT = (u1Byte)btRssi;
		pDM_Odm->bUserAssignLevel = TRUE;
		ODM_SetTimer( pDM_Odm, &pDM_Odm->PSDTimer, 0);
	}
	else
	{
		ODM_CancelTimer(pDM_Odm, &pDM_Odm->PSDTimer);
	}
#endif
}



void	odm_RXHPInit(
	IN		PDM_ODM_T		pDM_Odm)
{
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)|(DEV_BUS_TYPE == RT_USB_INTERFACE)
	pRXHP_T			pRX_HP_Table  = &pDM_Odm->DM_RXHP_Table;
   	u1Byte			index;

	pRX_HP_Table->RXHP_enable = TRUE;
	pRX_HP_Table->RXHP_flag = 0;
	pRX_HP_Table->PSD_func_trigger = 0;
	pRX_HP_Table->Pre_IGI = 0x20;
	pRX_HP_Table->Cur_IGI = 0x20;
	pRX_HP_Table->Cur_pw_th = pw_th_10dB;
	pRX_HP_Table->Pre_pw_th = pw_th_10dB;
	for(index=0; index<80; index++)
		pRX_HP_Table->PSD_bitmap_RXHP[index] = 1;

#if(DEV_BUS_TYPE == RT_USB_INTERFACE)
	pRX_HP_Table->TP_Mode = Idle_Mode;
#endif
#endif
}

void odm_RXHP(
	IN		PDM_ODM_T		pDM_Odm)
{
#if( DM_ODM_SUPPORT_TYPE & (ODM_WIN))
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE) | (DEV_BUS_TYPE == RT_USB_INTERFACE)
	PADAPTER	Adapter =  pDM_Odm->Adapter;
	PMGNT_INFO	pMgntInfo = &(Adapter->MgntInfo);
	pDIG_T		pDM_DigTable = &pDM_Odm->DM_DigTable;
	pRXHP_T		pRX_HP_Table  = &pDM_Odm->DM_RXHP_Table;
       PFALSE_ALARM_STATISTICS		FalseAlmCnt = &(pDM_Odm->FalseAlmCnt);
	
	u1Byte              	i, j, sum;
	u1Byte			Is40MHz;
	s1Byte              	Intf_diff_idx, MIN_Intf_diff_idx = 16;   
       s4Byte              	cur_channel;    
       u1Byte              	ch_map_intf_5M[17] = {0};     
       static u4Byte		FA_TH = 0;	
	static u1Byte      	psd_intf_flag = 0;
	static s4Byte      	curRssi = 0;                
       static s4Byte  		preRssi = 0;                                                                
	static u1Byte		PSDTriggerCnt = 1;
	
	u1Byte			RX_HP_enable = (u1Byte)(ODM_GetBBReg(pDM_Odm, rOFDM0_XAAGCCore2, bMaskDWord)>>31);  

#if(DEV_BUS_TYPE == RT_USB_INTERFACE)	
	static s8Byte  		lastTxOkCnt = 0, lastRxOkCnt = 0;  
       s8Byte			curTxOkCnt, curRxOkCnt;
	s8Byte			curTPOkCnt;
	s8Byte			TP_Acc3, TP_Acc5;
	static s8Byte		TP_Buff[5] = {0};
	static u1Byte		pre_state = 0, pre_state_flag = 0;
	static u1Byte		Intf_HighTP_flag = 0, De_counter = 16; 
	static u1Byte		TP_Degrade_flag = 0;
#endif	   
	static u1Byte		LatchCnt = 0;
	
	if(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8188E))
		return;
	if(pDM_Odm->SupportICType == ODM_RTL8192D)
	{
		if(*(pDM_Odm->pBandType) != ODM_BAND_2_4G)
			return;
	}

	if(!(pDM_Odm->SupportAbility==ODM_BB_RXHP))
		return;


	if(RX_HP_enable == 1)
		pRX_HP_Table->RXHP_enable = FALSE;
	else
		pRX_HP_Table->RXHP_enable = TRUE;

	if(pRX_HP_Table->RXHP_enable == FALSE)
	{
		if(pRX_HP_Table->RXHP_flag == 1)
		{
			pRX_HP_Table->RXHP_flag = 0;
			psd_intf_flag = 0;
		}
		return;
	}

#if(DEV_BUS_TYPE == RT_USB_INTERFACE)	
	curTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast)-lastTxOkCnt;
	curRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast)-lastRxOkCnt;
	lastTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast);
	lastRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast);

	curTPOkCnt = curTxOkCnt+curRxOkCnt;
	TP_Buff[0] = curTPOkCnt;   
	TP_Acc3 = PlatformDivision64((TP_Buff[1]+TP_Buff[2]+TP_Buff[3]), 3);
	TP_Acc5 = PlatformDivision64((TP_Buff[0]+TP_Buff[1]+TP_Buff[2]+TP_Buff[3]+TP_Buff[4]), 5);
	
	if(TP_Acc5 < 1000)
		pRX_HP_Table->TP_Mode = Idle_Mode;
	else if((1000 < TP_Acc5)&&(TP_Acc5 < 3750000))
		pRX_HP_Table->TP_Mode = Low_TP_Mode;
	else
		pRX_HP_Table->TP_Mode = High_TP_Mode;

	ODM_RT_TRACE(pDM_Odm, 	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RX HP TP Mode = %d\n", pRX_HP_Table->TP_Mode));
	if(TP_Degrade_flag == 1)
	{
		LatchCnt--;
		if(LatchCnt == 0)
		{
			TP_Degrade_flag = 0;
		}
	}
	if(Intf_HighTP_flag == 1)
	{
		De_counter--;
		if(De_counter == 0)
		{
			Intf_HighTP_flag = 0;
			psd_intf_flag = 0;
		}
	}
#endif

	if ((pMgntInfo->mAssoc) && (!pMgntInfo->mIbss) && (!ACTING_AS_AP(Adapter)))
	{    
		Is40MHz = *(pDM_Odm->pBandWidth);
		curRssi = pDM_Odm->RSSI_Min;
		cur_channel = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, 0x0fff) & 0x0f;
		ODM_RT_TRACE(pDM_Odm, 	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RXHP RX HP flag = %d\n", pRX_HP_Table->RXHP_flag));
		ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RXHP FA = %d\n", FalseAlmCnt->Cnt_all));
		ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RXHP cur RSSI = %d, pre RSSI=%d\n", curRssi, preRssi));
		ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RXHP current CH = %d\n", cur_channel));
		ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RXHP Is 40MHz = %d\n", Is40MHz));
		if((curRssi > 68) && (pRX_HP_Table->RXHP_flag == 0))
		{
#if (DEV_BUS_TYPE == RT_USB_INTERFACE)
			if(pRX_HP_Table->TP_Mode == Idle_Mode)	
			{
#endif
				if(PSDTriggerCnt == 1)       
				{    	
					odm_PSD_RXHP(pDM_Odm);
					pRX_HP_Table->PSD_func_trigger = 1;
					PSDTriggerCnt = 0;
				}
				else
				{
             				PSDTriggerCnt++;
				}
#if(DEV_BUS_TYPE == RT_USB_INTERFACE)
			}	
			if(pRX_HP_Table->TP_Mode == High_TP_Mode)
			{
				if((pre_state_flag == 0)&&(LatchCnt == 0)) 
				{
					if((((curTPOkCnt-TP_Acc3)*20)<(TP_Acc3))&&(((curTPOkCnt-TP_Acc3)*20)>(-TP_Acc3)))
					{
						pre_state++;
						if(pre_state == 3)     
						{
							pre_state_flag = 1;
							pre_state = 0;
						}

					}
					else
					{
						pre_state = 0;
					}
				}
				if(pre_state_flag == 1)		
				{
					if(((TP_Acc3-curTPOkCnt)*5)>(TP_Acc3))     
					{
						odm_PSD_RXHP(pDM_Odm);
						pRX_HP_Table->PSD_func_trigger = 1;
						TP_Degrade_flag = 1;
						LatchCnt = 2;
						pre_state_flag = 0;
					}
					else if(((TP_Buff[2]-curTPOkCnt)*5)>TP_Buff[2])
					{
						odm_PSD_RXHP(pDM_Odm);
						pRX_HP_Table->PSD_func_trigger = 1;
						TP_Degrade_flag = 1;
						LatchCnt = 2;
						pre_state_flag = 0;
					}
					else if(((TP_Buff[3]-curTPOkCnt)*5)>TP_Buff[3])
					{
						odm_PSD_RXHP(pDM_Odm);
						pRX_HP_Table->PSD_func_trigger = 1;
						TP_Degrade_flag = 1;
						LatchCnt = 2;
						pre_state_flag = 0;
					}
				}
			}
#endif
}

#if (DEV_BUS_TYPE == RT_USB_INTERFACE)
		for (i=0;i<4;i++)
		{
			TP_Buff[4-i] = TP_Buff[3-i];
		}
#endif
		if((pRX_HP_Table->PSD_func_trigger == 1)&&(LatchCnt == 0))
    		{	
			for (i = 0 ; i < 16 ; i++)
           		{
				sum = 0;
				for(j = 0; j < 5 ; j++)
                			sum += pRX_HP_Table->PSD_bitmap_RXHP[5*i + j];
            
                		if(sum < 5)
                		{
                			ch_map_intf_5M[i] = 1; 
                		}
           		}
	    		for(i = 0; i < (4+4*Is40MHz) ; i++)
           		{
				ch_map_intf_5M[cur_channel - (1+2*Is40MHz) + i] = 0;  
           		}
				
           		psd_intf_flag = 0;
	    		for(i = 0; i < 16; i++)
           		{
         			if(ch_map_intf_5M[i] == 1)
	              	{
	              		psd_intf_flag = 1;           
	              		break;
         			}
	    		}
				
#if (DEV_BUS_TYPE == RT_USB_INTERFACE)
			if(pRX_HP_Table->TP_Mode!=Idle_Mode)
			{
				if(psd_intf_flag == 1)    
				{
					Intf_HighTP_flag = 1;
					De_counter = 32;    
				}
			}
#endif
			ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RX HP psd_intf_flag = %d\n", psd_intf_flag));
           		for(i = 0; i < 16; i++)
          		{
				if(ch_map_intf_5M[i] == 1)
                		{
					Intf_diff_idx = ((cur_channel+Is40MHz-(i+1))>0) ? (s1Byte)(cur_channel-2*Is40MHz-(i-2)) : (s1Byte)((i+1)-(cur_channel+2*Is40MHz));  
                      		if(Intf_diff_idx < MIN_Intf_diff_idx)
						MIN_Intf_diff_idx = Intf_diff_idx;   
		  		}
	    		}
	    		ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RX HP MIN_Intf_diff_idx = %d\n", MIN_Intf_diff_idx)); 
			switch (MIN_Intf_diff_idx){
      				case 0: 
	   			case 1:
	        		case 2:
	        		case 3:	 	 
                 			FA_TH = FA_RXHP_TH1;  
                     		break;
	        		case 4:			
	        		case 5:			
		   			FA_TH = FA_RXHP_TH2;	
               			break;
	        		case 6:			
	        		case 7:			
		      			FA_TH = FA_RXHP_TH3;
                    			break; 
               		case 8:			
	        		case 9:			
		      			FA_TH = FA_RXHP_TH4;
                    			break; 	
	        		case 10:
	        		case 11:
	        		case 12:
	        		case 13:	 
	        		case 14:
	      			case 15:	 	
		      			FA_TH = FA_RXHP_TH5;
                    			break;  		
       		}	
			ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RX HP FA_TH = %d\n", FA_TH));
			pRX_HP_Table->PSD_func_trigger = 0;
		}
         	if(pRX_HP_Table->RXHP_flag == 1)
         	{
              	if ((curRssi > 80)&&(preRssi < 80))
              	{ 
                   		pRX_HP_Table->Cur_IGI = LNA_Low_Gain_1;
              	}
              	else if ((curRssi < 80)&&(preRssi > 80))
              	{
                   		pRX_HP_Table->Cur_IGI = LNA_Low_Gain_2;
			}
	       	else if ((curRssi > 72)&&(preRssi < 72))
	      		{
                		pRX_HP_Table->Cur_IGI = LNA_Low_Gain_2;
	       	}
              	else if ((curRssi < 72)&&( preRssi > 72))
	     		{
                   		pRX_HP_Table->Cur_IGI = LNA_Low_Gain_3;
	       	}
	       	else if (curRssi < 68)		
	       	{
                   		pRX_HP_Table->Cur_pw_th = pw_th_10dB;
				pRX_HP_Table->RXHP_flag = 0;   
				psd_intf_flag = 0;
			}
		}
		else   
		{
			if ((curRssi > 70) && (psd_intf_flag == 1) && (FalseAlmCnt->Cnt_all > FA_TH) &&  
				(pDM_DigTable->CurIGValue == pDM_DigTable->rx_gain_range_max))
			{
             			if (curRssi > 80)
             			{
					pRX_HP_Table->Cur_IGI = LNA_Low_Gain_1;
				}
				else if (curRssi > 72) 
              		{
               			pRX_HP_Table->Cur_IGI = LNA_Low_Gain_2;
				}
             			else
            			{
                   			pRX_HP_Table->Cur_IGI = LNA_Low_Gain_3;
				}
           			pRX_HP_Table->Cur_pw_th = pw_th_16dB;	
				pRX_HP_Table->First_time_enter = TRUE;
				pRX_HP_Table->RXHP_flag = 1;   
			}
		}
		preRssi = curRssi; 
		odm_Write_RXHP(pDM_Odm);	
	}
#endif
#endif
}

void odm_Write_RXHP(
	IN	PDM_ODM_T	pDM_Odm)
{
	pRXHP_T		pRX_HP_Table = &pDM_Odm->DM_RXHP_Table;
	u4Byte		currentIGI;

	if(pRX_HP_Table->Cur_IGI != pRX_HP_Table->Pre_IGI)
	{
		ODM_SetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskByte0, pRX_HP_Table->Cur_IGI);
	     	ODM_SetBBReg(pDM_Odm, rOFDM0_XBAGCCore1, bMaskByte0, pRX_HP_Table->Cur_IGI);	
	}
	
	if(pRX_HP_Table->Cur_pw_th != pRX_HP_Table->Pre_pw_th)
{
		ODM_SetBBReg(pDM_Odm, rOFDM0_XAAGCCore2, BIT8|BIT9, pRX_HP_Table->Cur_pw_th); 
	}

	if(pRX_HP_Table->RXHP_flag == 0)
	{
		pRX_HP_Table->Cur_IGI = 0x20;
	}
	else
	{
		currentIGI = ODM_GetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskByte0);
		if(currentIGI<0x50)
		{
			ODM_SetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskByte0, pRX_HP_Table->Cur_IGI);
	     		ODM_SetBBReg(pDM_Odm, rOFDM0_XBAGCCore1, bMaskByte0, pRX_HP_Table->Cur_IGI);	
		}
	}
	pRX_HP_Table->Pre_IGI = pRX_HP_Table->Cur_IGI;
	pRX_HP_Table->Pre_pw_th = pRX_HP_Table->Cur_pw_th;

}

VOID
odm_PSD_RXHP(
	IN	PDM_ODM_T	pDM_Odm
)
{
	pRXHP_T			pRX_HP_Table  = &pDM_Odm->DM_RXHP_Table;
	PADAPTER		Adapter =  pDM_Odm->Adapter;
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	unsigned int 		pts, start_point, stop_point, initial_gain ;
	static u1Byte		PSD_bitmap_memory[80], init_memory = 0;
	static u1Byte 		psd_cnt=0;
	static u4Byte		PSD_report[80], PSD_report_tmp;
	static u8Byte		lastTxOkCnt=0, lastRxOkCnt=0;
	u1Byte			idx[20]={96,99,102,106,109,112,115,118,122,125,
					0,3,6,10,13,16,19,22,26,29};
	u1Byte			n, i, channel, BBReset,tone_idx;
	u1Byte			PSD_bitmap[10]/*, SSBT=0*/,initial_gain_psd=0, RSSI_BT=0, initialGainUpper;
	s4Byte    			PSD_skip_start, PSD_skip_stop;
	u4Byte			CurrentChannel, RXIQI, RxIdleLowPwr, wlan_channel;
	u4Byte			ReScan, Interval, Is40MHz;
	u8Byte			curTxOkCnt, curRxOkCnt;
	u1Byte			group_idx=0;
	u4Byte			SYN_RF25=0, SYN_RF26=0, SYN_RF27=0, SYN_RF2B=0, SYN_RF2C=0;
	u4Byte			SYN[5] = {0x25, 0x26, 0x27, 0x2B, 0x2C};   
	u4Byte			SYN_group[3][5] = {{0x643BC, 0xFC038, 0x77C1A, 0x41289, 0x01840},    
									    {0x643BC, 0xFC038, 0x07C1A, 0x41289, 0x01840},    
									    {0x243BC, 0xFC438, 0x07C1A, 0x4128B, 0x0FC41}};  
  	u1Byte                 RSSI_BT_new = (u1Byte) ODM_GetBBReg(pDM_Odm, 0xB9C, 0xFF);
       u1Byte                 rssi_ctrl = (u1Byte) ODM_GetBBReg(pDM_Odm, 0xB38, 0xFF);
	
	if(pMgntInfo->bScanInProgress)
	{
		return;
	}

	ReScan = PSD_RESCAN;
	Interval = SCAN_INTERVAL;


	if(init_memory == 0)
	{
		RT_TRACE(	COMP_PSD, DBG_LOUD,("Init memory\n"));
		for(i = 0; i < 80; i++)
			PSD_bitmap_memory[i] = 0xFF;
		init_memory = 1;
	}
	if(psd_cnt == 0)
	{
		RT_TRACE(COMP_PSD, DBG_LOUD,("Enter dm_PSD_Monitor\n"));
		for(i = 0; i < 80; i++)
			PSD_report[i] = 0;
	}

	CurrentChannel = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask);
	if(pDM_Odm->SupportICType == ODM_RTL8192D)
	{
		if((*(pDM_Odm->pMacPhyMode)==ODM_SMSP)||(*(pDM_Odm->pMacPhyMode)==ODM_DMSP))
		{
			SYN_RF25 = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x25, bMaskDWord);
			SYN_RF26 = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x26, bMaskDWord);
			SYN_RF27 = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x27, bMaskDWord);
			SYN_RF2B = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x2B, bMaskDWord);
			SYN_RF2C = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x2C, bMaskDWord);
       	}
		else    
		{
			SYN_RF25 = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x25, bMaskDWord);
			SYN_RF26 = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x26, bMaskDWord);
			SYN_RF27 = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x27, bMaskDWord);
			SYN_RF2B = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x2B, bMaskDWord);
			SYN_RF2C = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x2C, bMaskDWord);
		}
	}
	RXIQI = ODM_GetBBReg(pDM_Odm, 0xC14, bMaskDWord);
	RxIdleLowPwr = (ODM_GetBBReg(pDM_Odm, 0x818, bMaskDWord)&BIT28)>>28;
	Is40MHz = *(pDM_Odm->pBandWidth);
	ODM_RT_TRACE(pDM_Odm,	COMP_PSD, DBG_LOUD,("PSD Scan Start\n"));
	ODM_SetBBReg(pDM_Odm, rFPGA0_RFMOD, BIT24, 0);
	ODM_Write1Byte(pDM_Odm, REG_TXPAUSE, 0xFF);
	ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_AC, bRFRegOffsetMask, 0x32E13);
	ODM_SetBBReg(pDM_Odm, 0xC70, BIT0, 0);
	ODM_SetBBReg(pDM_Odm, 0xC7C, BIT20, 0);
	ODM_SetBBReg(pDM_Odm, 0xC14, bMaskDWord, 0x0);
	ODM_SetBBReg(pDM_Odm, 0x87C, BIT31, 1);
	BBReset = ODM_Read1Byte(pDM_Odm, 0x02);
	ODM_Write1Byte(pDM_Odm, 0x02, BBReset&(~BIT0));
	ODM_Write1Byte(pDM_Odm, 0x02, BBReset|BIT0);
	ODM_SetBBReg(pDM_Odm, 0x87C, BIT31, 0);
	ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 0x0);
      	RSSI_BT = RSSI_BT_new;
	RT_TRACE(COMP_PSD, DBG_LOUD,("PSD: RSSI_BT= %d\n", RSSI_BT));
	
	if(rssi_ctrl == 1)       
		initial_gain_psd = RSSI_BT_new; 
     	else
		initial_gain_psd = pDM_Odm->RSSI_Min;   
	
	RT_TRACE(COMP_PSD, DBG_LOUD,("PSD: RSSI_BT= %d\n", RSSI_BT));
	
	initialGainUpper = 0x54;
	
	RSSI_BT = initial_gain_psd;
	
	RT_TRACE(	COMP_PSD, DBG_LOUD,("PSD: initial gain= 0x%x\n", initial_gain_psd));
	
	pDM_Odm->bDMInitialGainEnable = FALSE;		
	initial_gain = ODM_GetBBReg(pDM_Odm, 0xc50, bMaskDWord) & 0x7F;
	ODM_Write_DIG(pDM_Odm, initial_gain_psd);
	ODM_SetBBReg(pDM_Odm, 0x88c, BIT20|BIT21|BIT22|BIT23, 0xF);

	pts = 128;

	if(pts == 128)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x0);
		start_point = 64;
		stop_point = 192;
	}
	else if(pts == 256)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x1);
		start_point = 128;
		stop_point = 384;
	}
	else if(pts == 512)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x2);
		start_point = 256;
		stop_point = 768;
	}
	else
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x3);
		start_point = 512;
		stop_point = 1536;
	}
	

	curTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast) - lastTxOkCnt;
	curRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast) - lastRxOkCnt;
	lastTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast);
	lastRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast);
	
	PSD_skip_start=80;
	PSD_skip_stop = 0;
	wlan_channel = CurrentChannel & 0x0f;

	RT_TRACE(COMP_PSD,DBG_LOUD,("PSD: current channel: %x, BW:%d \n", wlan_channel, Is40MHz));
	
	if((curRxOkCnt+curTxOkCnt) > 1000)
	{
		PSD_skip_start = (wlan_channel-1)*5 -Is40MHz*10;
		PSD_skip_stop = PSD_skip_start + (1+Is40MHz)*20;
	}

	RT_TRACE(COMP_PSD,DBG_LOUD,("PSD: Skip tone from %d to %d \n", PSD_skip_start, PSD_skip_stop));

 	for (n=0;n<80;n++)
 	{
 		if((n%20)==0)
 		{
			channel = (n/20)*4 + 1;
			if(pDM_Odm->SupportICType == ODM_RTL8192D)
			{
				switch(channel)
				{
					case 1: 
					case 9:
						group_idx = 0;
						break;
					case 5:
						group_idx = 2;
						break;
					case 13:
				 		group_idx = 1;
						break;
				}
				if((*(pDM_Odm->pMacPhyMode)==ODM_SMSP)||(*(pDM_Odm->pMacPhyMode)==ODM_DMSP))   
		{
					for(i = 0; i < SYN_Length; i++)
						ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, SYN[i], bMaskDWord, SYN_group[group_idx][i]);

					ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
					ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, RF_CHNLBW, 0x3FF, channel);
				}
				else 
			{
					for(i = 0; i < SYN_Length; i++)
						ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, SYN[i], bMaskDWord, SYN_group[group_idx][i]);   
					
					ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
				}
			}
			else
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
			}	
		tone_idx = n%20;
		if ((n>=PSD_skip_start) && (n<PSD_skip_stop))
		{	
			PSD_report[n] = initial_gain_psd;//SSBT;
			ODM_RT_TRACE(pDM_Odm,COMP_PSD,DBG_LOUD,("PSD:Tone %d skipped \n", n));
		}
		else
		{
			PSD_report_tmp =  GetPSDData(pDM_Odm, idx[tone_idx], initial_gain_psd);

			if ( PSD_report_tmp > PSD_report[n])
				PSD_report[n] = PSD_report_tmp;
				
		}
	}

	PatchDCTone(pDM_Odm, PSD_report, initial_gain_psd);
      
	ODM_SetBBReg(pDM_Odm, 0xC70, BIT0, 1);
	ODM_SetBBReg(pDM_Odm, 0xC7C, BIT20, 1);
	ODM_SetBBReg(pDM_Odm, rFPGA0_RFMOD, BIT24, 1);
	ODM_Write1Byte(pDM_Odm, REG_TXPAUSE, 0x00);
	ODM_SetBBReg(pDM_Odm, 0x88c, BIT20|BIT21|BIT22|BIT23, 0x0);
	pDM_Odm->bDMInitialGainEnable= TRUE;
	ODM_Write_DIG(pDM_Odm,(u1Byte) initial_gain);
	ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask, CurrentChannel);
	if(pDM_Odm->SupportICType == ODM_RTL8192D)
	{
		if((*(pDM_Odm->pMacPhyMode)==ODM_SMSP)||(*(pDM_Odm->pMacPhyMode)==ODM_DMSP))
		{
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, RF_CHNLBW, bMaskDWord, CurrentChannel);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x25, bMaskDWord, SYN_RF25);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x26, bMaskDWord, SYN_RF26);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x27, bMaskDWord, SYN_RF27);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x2B, bMaskDWord, SYN_RF2B);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_B, 0x2C, bMaskDWord, SYN_RF2C);
		}
		else    
		{
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x25, bMaskDWord, SYN_RF25);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x26, bMaskDWord, SYN_RF26);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x27, bMaskDWord, SYN_RF27);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x2B, bMaskDWord, SYN_RF2B);
			ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x2C, bMaskDWord, SYN_RF2C);
		}
	}
	ODM_SetBBReg(pDM_Odm, 0xC14, bMaskDWord, RXIQI);
	if(RxIdleLowPwr == TRUE)
		ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 1);
	
	psd_cnt++;
	ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD:psd_cnt = %d \n",psd_cnt));
	if (psd_cnt < ReScan)
	{
		ODM_SetTimer(pDM_Odm, &pRX_HP_Table->PSDTimer, Interval); 
	}
	else
			{	
		psd_cnt = 0;
		for(i=0;i<80;i++)
			RT_TRACE(	COMP_PSD, DBG_LOUD,("psd_report[%d]=     %d \n", 2402+i, PSD_report[i]));

		GoodChannelDecision(pDM_Odm, PSD_report, PSD_bitmap,RSSI_BT, PSD_bitmap_memory);

			}
		}

VOID
odm_PSD_RXHPCallback(
	PRT_TIMER		pTimer
)
{
	PADAPTER		Adapter = (PADAPTER)pTimer->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;
	pRXHP_T			pRX_HP_Table  = &pDM_Odm->DM_RXHP_Table;
	
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
	#if USE_WORKITEM
	ODM_ScheduleWorkItem(&pRX_HP_Table->PSDTimeWorkitem);
	#else
	odm_PSD_RXHP(pDM_Odm);
	#endif
#else
	ODM_ScheduleWorkItem(&pRX_HP_Table->PSDTimeWorkitem);
#endif
	
	}

VOID
odm_PSD_RXHPWorkitemCallback(
    IN PVOID            pContext
    )
{
	PADAPTER	pAdapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;
	
	odm_PSD_RXHP(pDM_Odm);
}

#endif

VOID
odm_PathDiversityInit(
	IN	PDM_ODM_T	pDM_Odm
)
{
	if(!(pDM_Odm->SupportAbility & ODM_BB_PATH_DIV))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_PATH_DIV,ODM_DBG_LOUD,("Return: Not Support PathDiv\n"));
		return;
	}

#if RTL8812A_SUPPORT

	if(pDM_Odm->SupportICType & ODM_RTL8812)
		ODM_PathDiversityInit_8812A(pDM_Odm);
#endif	
}


VOID
odm_PathDiversity(
	IN	PDM_ODM_T	pDM_Odm
)
{
	if(!(pDM_Odm->SupportAbility & ODM_BB_PATH_DIV))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_PATH_DIV,ODM_DBG_LOUD,("Return: Not Support PathDiv\n"));
		return;
	}

#if RTL8812A_SUPPORT

	if(pDM_Odm->SupportICType & ODM_RTL8812)
		ODM_PathDiversity_8812A(pDM_Odm);
#endif	
}


#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID
odm_OFDMTXPathDiversity_92C(
	IN	PADAPTER	Adapter)
{
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	PRT_WLAN_STA	pEntry;
	u1Byte	i, DefaultRespPath = 0;
	s4Byte	MinRSSI = 0xFF;
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;
	pDM_PDTable->OFDMTXPath = 0;
	
	if(pMgntInfo->mAssoc)
	{
		RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: Default port RSSI[0]=%d, RSSI[1]=%d\n",
			Adapter->RxStats.RxRSSIPercentage[0], Adapter->RxStats.RxRSSIPercentage[1]));
		if(Adapter->RxStats.RxRSSIPercentage[0] > Adapter->RxStats.RxRSSIPercentage[1])
		{
			pDM_PDTable->OFDMTXPath = pDM_PDTable->OFDMTXPath & (~BIT0);
			MinRSSI =  Adapter->RxStats.RxRSSIPercentage[1];
			DefaultRespPath = 0;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: Default port Select Path-0\n"));
		}
		else
		{
			pDM_PDTable->OFDMTXPath =  pDM_PDTable->OFDMTXPath | BIT0;
			MinRSSI =  Adapter->RxStats.RxRSSIPercentage[0];
			DefaultRespPath = 1;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: Default port Select Path-1\n"));
		}
	}
	for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
			pEntry = AsocEntry_EnumStation(GetFirstExtAdapter(Adapter), i);
		else
			pEntry = AsocEntry_EnumStation(GetDefaultAdapter(Adapter), i);

		if(pEntry!=NULL)
		{
			if(pEntry->bAssociated)
			{
				RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: MACID=%d, RSSI_0=%d, RSSI_1=%d\n", 
					pEntry->AssociatedMacId, pEntry->rssi_stat.RxRSSIPercentage[0], pEntry->rssi_stat.RxRSSIPercentage[1]));
				
				if(pEntry->rssi_stat.RxRSSIPercentage[0] > pEntry->rssi_stat.RxRSSIPercentage[1])
				{
					pDM_PDTable->OFDMTXPath = pDM_PDTable->OFDMTXPath & ~(BIT(pEntry->AssociatedMacId));
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: MACID=%d Select Path-0\n", pEntry->AssociatedMacId));
					if(pEntry->rssi_stat.RxRSSIPercentage[1] < MinRSSI)
					{
						MinRSSI = pEntry->rssi_stat.RxRSSIPercentage[1];
						DefaultRespPath = 0;
					}
				}
				else
				{
					pDM_PDTable->OFDMTXPath = pDM_PDTable->OFDMTXPath | BIT(pEntry->AssociatedMacId);
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: MACID=%d Select Path-1\n", pEntry->AssociatedMacId));
					if(pEntry->rssi_stat.RxRSSIPercentage[0] < MinRSSI)
					{
						MinRSSI = pEntry->rssi_stat.RxRSSIPercentage[0];
						DefaultRespPath = 1;
					}
				}
			}
		}
		else
		{
			break;
		}
	}

	pDM_PDTable->OFDMDefaultRespPath = DefaultRespPath;
}


BOOLEAN
odm_IsConnected_92C(
	IN	PADAPTER	Adapter
)
{
	PRT_WLAN_STA	pEntry;
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	u4Byte		i;
	BOOLEAN		bConnected=FALSE;
	
	if(pMgntInfo->mAssoc)
	{
		bConnected = TRUE;
	}
	else
	{
		for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
		{
			if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
				pEntry = AsocEntry_EnumStation(GetFirstExtAdapter(Adapter), i);
			else
				pEntry = AsocEntry_EnumStation(GetDefaultAdapter(Adapter), i);

			if(pEntry!=NULL)
			{
				if(pEntry->bAssociated)
				{
					bConnected = TRUE;
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	return	bConnected;
}


VOID
odm_ResetPathDiversity_92C(
		IN	PADAPTER	Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;
	PRT_WLAN_STA	pEntry;
	u4Byte	i,j;

	pHalData->RSSI_test = FALSE;
	pDM_PDTable->CCK_Pkt_Cnt = 0;
	pDM_PDTable->OFDM_Pkt_Cnt = 0;
	pHalData->CCK_Pkt_Cnt =0;
	pHalData->OFDM_Pkt_Cnt =0;
	
	if(pDM_PDTable->CCKPathDivEnable == TRUE)	
		PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x01);

	for(i=0; i<2; i++)
	{
		pDM_PDTable->RSSI_CCK_Path_cnt[i]=0;
		pDM_PDTable->RSSI_CCK_Path[i] = 0;
	}
	for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
			pEntry = AsocEntry_EnumStation(GetFirstExtAdapter(Adapter), i);
		else
			pEntry = AsocEntry_EnumStation(GetDefaultAdapter(Adapter), i);

		if(pEntry!=NULL)
		{
			pEntry->rssi_stat.CCK_Pkt_Cnt = 0;
			pEntry->rssi_stat.OFDM_Pkt_Cnt = 0;
			for(j=0; j<2; j++)
			{
				pEntry->rssi_stat.RSSI_CCK_Path_cnt[j] = 0;
				pEntry->rssi_stat.RSSI_CCK_Path[j] = 0;
			}
		}
		else
			break;
	}
}


VOID
odm_CCKTXPathDiversity_92C(
	IN	PADAPTER	Adapter
)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	PRT_WLAN_STA	pEntry;
	s4Byte	MinRSSI = 0xFF;
	u1Byte	i, DefaultRespPath = 0;
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;

	if(pMgntInfo->mAssoc)
	{
		if(pHalData->OFDM_Pkt_Cnt == 0)
		{
			for(i=0; i<2; i++)
			{
				if(pDM_PDTable->RSSI_CCK_Path_cnt[i] > 1)
					pDM_PDTable->RSSI_CCK_Path[i] = pDM_PDTable->RSSI_CCK_Path[i] / (pDM_PDTable->RSSI_CCK_Path_cnt[i]-1);
				else
					pDM_PDTable->RSSI_CCK_Path[i] = 0;
			}
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: pDM_PDTable->RSSI_CCK_Path[0]=%d, pDM_PDTable->RSSI_CCK_Path[1]=%d\n",
				pDM_PDTable->RSSI_CCK_Path[0], pDM_PDTable->RSSI_CCK_Path[1]));
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: pDM_PDTable->RSSI_CCK_Path_cnt[0]=%d, pDM_PDTable->RSSI_CCK_Path_cnt[1]=%d\n",
				pDM_PDTable->RSSI_CCK_Path_cnt[0], pDM_PDTable->RSSI_CCK_Path_cnt[1]));
		
			if(pDM_PDTable->RSSI_CCK_Path[0] > pDM_PDTable->RSSI_CCK_Path[1])
			{
				pDM_PDTable->CCKTXPath = pDM_PDTable->CCKTXPath & (~BIT0);
				MinRSSI =  pDM_PDTable->RSSI_CCK_Path[1];
				DefaultRespPath = 0;
				RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Default port Select CCK Path-0\n"));
			}
			else if(pDM_PDTable->RSSI_CCK_Path[0] < pDM_PDTable->RSSI_CCK_Path[1])
			{
				pDM_PDTable->CCKTXPath =  pDM_PDTable->CCKTXPath | BIT0;
				MinRSSI =  pDM_PDTable->RSSI_CCK_Path[0];
				DefaultRespPath = 1;
				RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Default port Select CCK Path-1\n"));
			}
			else
			{
				if((pDM_PDTable->RSSI_CCK_Path[0] != 0) && (pDM_PDTable->RSSI_CCK_Path[0] < MinRSSI))
				{
					pDM_PDTable->CCKTXPath = pDM_PDTable->CCKTXPath & (~BIT0);
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Default port Select CCK Path-0\n"));
					MinRSSI =  pDM_PDTable->RSSI_CCK_Path[1];
					DefaultRespPath = 0;
				}
				else
				{
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Default port unchange CCK Path\n"));
				}
			}
		}
		else
		{
			pDM_PDTable->CCKTXPath = (pDM_PDTable->CCKTXPath & (~BIT0)) | (pDM_PDTable->OFDMTXPath &BIT0);
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Follow OFDM decision, Default port Select CCK Path-%d\n",
				pDM_PDTable->CCKTXPath &BIT0));
		}
	}
	for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
			pEntry = AsocEntry_EnumStation(GetFirstExtAdapter(Adapter), i);
		else
			pEntry = AsocEntry_EnumStation(GetDefaultAdapter(Adapter), i);

		if(pEntry!=NULL)
		{
			if(pEntry->bAssociated)
			{
				if(pEntry->rssi_stat.OFDM_Pkt_Cnt == 0)
				{
					u1Byte j=0;
					for(j=0; j<2; j++)
					{
						if(pEntry->rssi_stat.RSSI_CCK_Path_cnt[j] > 1)
							pEntry->rssi_stat.RSSI_CCK_Path[j] = pEntry->rssi_stat.RSSI_CCK_Path[j] / (pEntry->rssi_stat.RSSI_CCK_Path_cnt[j]-1);
						else
							pEntry->rssi_stat.RSSI_CCK_Path[j] = 0;
					}
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: MACID=%d, RSSI_CCK0=%d, RSSI_CCK1=%d\n", 
						pEntry->AssociatedMacId, pEntry->rssi_stat.RSSI_CCK_Path[0], pEntry->rssi_stat.RSSI_CCK_Path[1]));
					
					if(pEntry->rssi_stat.RSSI_CCK_Path[0] >pEntry->rssi_stat.RSSI_CCK_Path[1])
					{
						pDM_PDTable->CCKTXPath = pDM_PDTable->CCKTXPath & ~(BIT(pEntry->AssociatedMacId));
						RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: MACID=%d Select CCK Path-0\n", pEntry->AssociatedMacId));
						if(pEntry->rssi_stat.RSSI_CCK_Path[1] < MinRSSI)
						{
							MinRSSI = pEntry->rssi_stat.RSSI_CCK_Path[1];
							DefaultRespPath = 0;
						}
					}
					else if(pEntry->rssi_stat.RSSI_CCK_Path[0] <pEntry->rssi_stat.RSSI_CCK_Path[1])
					{
						pDM_PDTable->CCKTXPath = pDM_PDTable->CCKTXPath | BIT(pEntry->AssociatedMacId);
						RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: MACID=%d Select CCK Path-1\n", pEntry->AssociatedMacId));
						if(pEntry->rssi_stat.RSSI_CCK_Path[0] < MinRSSI)
						{
							MinRSSI = pEntry->rssi_stat.RSSI_CCK_Path[0];
							DefaultRespPath = 1;
						}
					}
					else
					{
						if((pEntry->rssi_stat.RSSI_CCK_Path[0] != 0) && (pEntry->rssi_stat.RSSI_CCK_Path[0] < MinRSSI))
						{
							pDM_PDTable->CCKTXPath = pDM_PDTable->CCKTXPath & ~(BIT(pEntry->AssociatedMacId));
							MinRSSI = pEntry->rssi_stat.RSSI_CCK_Path[1];
							DefaultRespPath = 0;
							RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: MACID=%d Select CCK Path-0\n", pEntry->AssociatedMacId));
						}
						else
						{
							RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: MACID=%d unchange CCK Path\n", pEntry->AssociatedMacId));
						}
					}
				}
				else
				{
					pDM_PDTable->CCKTXPath = (pDM_PDTable->CCKTXPath & (~(BIT(pEntry->AssociatedMacId)))) | (pDM_PDTable->OFDMTXPath & BIT(pEntry->AssociatedMacId));
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Follow OFDM decision, MACID=%d Select CCK Path-%d\n",
						pEntry->AssociatedMacId, (pDM_PDTable->CCKTXPath & BIT(pEntry->AssociatedMacId))>>(pEntry->AssociatedMacId)));
				}
			}
		}
		else
		{
			break;
		}
	}

	RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C:MinRSSI=%d\n",MinRSSI));

	if(MinRSSI == 0xFF)
		DefaultRespPath = pDM_PDTable->CCKDefaultRespPath;

	pDM_PDTable->CCKDefaultRespPath = DefaultRespPath;
}



VOID
odm_PathDiversityAfterLink_92C(
	IN	PADAPTER	Adapter
)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;
	pPD_T		pDM_PDTable = &Adapter->DM_PDTable;
	u1Byte		DefaultRespPath=0;

	if((!IS_92C_SERIAL(pHalData->VersionID)) || (pHalData->PathDivCfg != 1) || (pHalData->eRFPowerState == eRfOff))
	{
		if(pHalData->PathDivCfg == 0)
		{
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("No ODM_TXPathDiversity()\n"));
		}
		else
		{
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("2T ODM_TXPathDiversity()\n"));
		}
		return;
	}
	if(!odm_IsConnected_92C(Adapter))
	{
		RT_TRACE(	COMP_SWAS, DBG_LOUD, ("ODM_TXPathDiversity(): No Connections\n"));
		return;
	}
	
	
	if(pDM_PDTable->TrainingState == 0)
	{
		RT_TRACE(	COMP_SWAS, DBG_LOUD, ("ODM_TXPathDiversity() ==>\n"));
		odm_OFDMTXPathDiversity_92C(Adapter);

		if((pDM_PDTable->CCKPathDivEnable == TRUE) && (pDM_PDTable->OFDM_Pkt_Cnt < 100))
		{
			
			if(pDM_PDTable->CCK_Pkt_Cnt > 300)
				pDM_PDTable->Timer = 20;
			else if(pDM_PDTable->CCK_Pkt_Cnt > 100)
				pDM_PDTable->Timer = 60;
			else
				pDM_PDTable->Timer = 250;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: timer=%d\n",pDM_PDTable->Timer));

			PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x00);
			pDM_PDTable->TrainingState = 1;
			pHalData->RSSI_test = TRUE;
			ODM_SetTimer( pDM_Odm, &pDM_Odm->CCKPathDiversityTimer, pDM_PDTable->Timer);
		}
		else
		{
			pDM_PDTable->CCKTXPath = pDM_PDTable->OFDMTXPath;
			DefaultRespPath = pDM_PDTable->OFDMDefaultRespPath;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_SetRespPath_92C: Skip odm_CCKTXPathDiversity_92C, DefaultRespPath is OFDM\n"));
			odm_SetRespPath_92C(Adapter, DefaultRespPath);
			odm_ResetPathDiversity_92C(Adapter);
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("ODM_TXPathDiversity() <==\n"));
		}
	}
	else if(pDM_PDTable->TrainingState == 1)
	{
		PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x05);
		pDM_PDTable->TrainingState = 2;
		ODM_SetTimer( pDM_Odm, &pDM_Odm->CCKPathDiversityTimer, pDM_PDTable->Timer);
	}
	else
	{
		pDM_PDTable->TrainingState = 0;	
		odm_CCKTXPathDiversity_92C(Adapter); 
		if(pDM_PDTable->OFDM_Pkt_Cnt != 0)
		{
			DefaultRespPath = pDM_PDTable->OFDMDefaultRespPath;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_SetRespPath_92C: DefaultRespPath is OFDM\n"));
		}
		else
		{
			DefaultRespPath = pDM_PDTable->CCKDefaultRespPath;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_SetRespPath_92C: DefaultRespPath is CCK\n"));
		}
		odm_SetRespPath_92C(Adapter, DefaultRespPath);
		odm_ResetPathDiversity_92C(Adapter);
		RT_TRACE(	COMP_SWAS, DBG_LOUD, ("ODM_TXPathDiversity() <==\n"));
	}

}



VOID
odm_CCKTXPathDiversityCallback(
	PRT_TIMER		pTimer
)
{
#if USE_WORKITEM
       PADAPTER	Adapter = (PADAPTER)pTimer->Adapter;
       HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	   PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;
#else
	PADAPTER	Adapter = (PADAPTER)pTimer->Adapter;
#endif

#if DEV_BUS_TYPE==RT_PCI_INTERFACE
#if USE_WORKITEM
	PlatformScheduleWorkItem(&pDM_Odm->CCKPathDiversityWorkitem);
#else
	odm_PathDiversityAfterLink_92C(Adapter);
#endif
#else
	PlatformScheduleWorkItem(&pDM_Odm->CCKPathDiversityWorkitem);
#endif

}


VOID
odm_CCKTXPathDiversityWorkItemCallback(
    IN PVOID            pContext
    )
{
	PADAPTER	Adapter = (PADAPTER)pContext;

	odm_CCKTXPathDiversity_92C(Adapter);
}


VOID
ODM_CCKPathDiversityChkPerPktRssi(
	PADAPTER		Adapter,
	BOOLEAN			bIsDefPort,
	BOOLEAN			bMatchBSSID,
	PRT_WLAN_STA	pEntry,
	PRT_RFD			pRfd,
	pu1Byte			pDesc
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	BOOLEAN			bCount = FALSE;
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;
#if DEV_BUS_TYPE != RT_SDIO_INTERFACE
	BOOLEAN	isCCKrate = RX_HAL_IS_CCK_RATE(Adapter, pDesc);
#else 
	BOOLEAN	isCCKrate = IS_HARDWARE_TYPE_8188E(Adapter) ? RX_HAL_IS_CCK_RATE_88E(pDesc) : RX_HAL_IS_CCK_RATE_92C(pDesc);
#endif

	if((pHalData->PathDivCfg != 1) || (pHalData->RSSI_test == FALSE))
		return;
		
	if(pHalData->RSSI_target==NULL && bIsDefPort && bMatchBSSID)
		bCount = TRUE;
	else if(pHalData->RSSI_target!=NULL && pEntry!=NULL && pHalData->RSSI_target==pEntry)
		bCount = TRUE;

	if(bCount && isCCKrate)
	{
		if(pDM_PDTable->TrainingState == 1 )
		{
			if(pEntry)
			{
				if(pEntry->rssi_stat.RSSI_CCK_Path_cnt[0] != 0)
					pEntry->rssi_stat.RSSI_CCK_Path[0] += pRfd->Status.RxPWDBAll;
				pEntry->rssi_stat.RSSI_CCK_Path_cnt[0]++;
			}
			else
			{
				if(pDM_PDTable->RSSI_CCK_Path_cnt[0] != 0)
					pDM_PDTable->RSSI_CCK_Path[0] += pRfd->Status.RxPWDBAll;
				pDM_PDTable->RSSI_CCK_Path_cnt[0]++;
			}
		}
		else if(pDM_PDTable->TrainingState == 2 )
		{
			if(pEntry)
			{
				if(pEntry->rssi_stat.RSSI_CCK_Path_cnt[1] != 0)
					pEntry->rssi_stat.RSSI_CCK_Path[1] += pRfd->Status.RxPWDBAll;
				pEntry->rssi_stat.RSSI_CCK_Path_cnt[1]++;
			}
			else
			{
				if(pDM_PDTable->RSSI_CCK_Path_cnt[1] != 0)
					pDM_PDTable->RSSI_CCK_Path[1] += pRfd->Status.RxPWDBAll;
				pDM_PDTable->RSSI_CCK_Path_cnt[1]++;
			}
		}
	}
}


BOOLEAN
ODM_PathDiversityBeforeLink92C(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (RT_MEM_SIZE_LEVEL != RT_MEM_SIZE_MINIMUM)
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE*	pHalData = NULL;
	PMGNT_INFO		pMgntInfo = NULL;
	pPD_T			pDM_PDTable = NULL;

	s1Byte			Score = 0;
	PRT_WLAN_BSS	pTmpBssDesc;
	PRT_WLAN_BSS	pTestBssDesc;

	u1Byte			target_chnl = 0;
	u1Byte			index;

	if (pDM_Odm->Adapter == NULL) 
	{
		return FALSE;
	}
	pHalData = GET_HAL_DATA(Adapter);
	pMgntInfo = &Adapter->MgntInfo;
	pDM_PDTable = &Adapter->DM_PDTable;
	
	if((!IS_92C_SERIAL(pHalData->VersionID)) || (pHalData->PathDivCfg!=1) || pMgntInfo->AntennaTest )
	{
		RT_TRACE(COMP_SWAS, DBG_LOUD, 
				("ODM_PathDiversityBeforeLink92C(): No PathDiv Mechanism before link.\n"));
		return FALSE;
	}

	PlatformAcquireSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	if(pHalData->eRFPowerState!=eRfOn || pMgntInfo->RFChangeInProgress || pMgntInfo->bMediaConnect)
	{
		PlatformReleaseSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	
		RT_TRACE(COMP_SWAS, DBG_LOUD, 
				("ODM_PathDiversityBeforeLink92C(): RFChangeInProgress(%x), eRFPowerState(%x)\n", 
				pMgntInfo->RFChangeInProgress,
				pHalData->eRFPowerState));
	
		pDM_PDTable->PathDiv_NoLink_State = 0;
		
		return FALSE;
	}
	else
	{
		PlatformReleaseSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	}

	if(pDM_PDTable->PathDiv_NoLink_State == 0)
	{

		pDM_PDTable->PathDiv_NoLink_State = 1;
	
		Adapter->MgntInfo.tmpNumBssDesc = pMgntInfo->NumBssDesc;
		PlatformMoveMemory((PVOID)Adapter->MgntInfo.tmpbssDesc, (PVOID)pMgntInfo->bssDesc, sizeof(RT_WLAN_BSS)*MAX_BSS_DESC);

		if(pDM_PDTable->DefaultRespPath == 0)
		{
			PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x05);
			odm_SetRespPath_92C(Adapter, 1);
			pDM_PDTable->OFDMTXPath = 0xFFFFFFFF;
			pDM_PDTable->CCKTXPath = 0xFFFFFFFF;
		}
		else
		{
			PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x00);
			odm_SetRespPath_92C(Adapter, 0);
			pDM_PDTable->OFDMTXPath = 0x0;
			pDM_PDTable->CCKTXPath = 0x0;
		}
#if 0	

		pDM_SWAT_Table->PreAntenna = pDM_SWAT_Table->CurAntenna;
		pDM_SWAT_Table->CurAntenna = (pDM_SWAT_Table->CurAntenna==Antenna_A)?Antenna_B:Antenna_A;
		
		RT_TRACE(COMP_SWAS, DBG_LOUD, 
			("ODM_SwAntDivCheckBeforeLink: Change to Ant(%s) for testing.\n", (pDM_SWAT_Table->CurAntenna==Antenna_A)?"A":"B"));
		pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 = ((pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 & 0xfffffcff) | (pDM_SWAT_Table->CurAntenna<<8));
		PHY_SetBBReg(Adapter, rFPGA0_XA_RFInterfaceOE, bMaskDWord, pDM_SWAT_Table->SWAS_NoLink_BK_Reg860);
#endif

		RT_TRACE(COMP_SWAS, DBG_LOUD, ("ODM_PathDiversityBeforeLink92C: Scan one more time\n"));
		pMgntInfo->ScanStep=0;
		target_chnl = odm_SwAntDivSelectScanChnl(Adapter);
		odm_SwAntDivConstructScanChnl(Adapter, target_chnl);
		PlatformSetTimer(Adapter, &pMgntInfo->ScanTimer, 5);

		return TRUE;
	}
	else
	{

		for(index=0; index<Adapter->MgntInfo.tmpNumBssDesc; index++)
		{
			pTmpBssDesc = &(Adapter->MgntInfo.tmpbssDesc[index]);
			pTestBssDesc = &(pMgntInfo->bssDesc[index]);

			if(PlatformCompareMemory(pTestBssDesc->bdBssIdBuf, pTmpBssDesc->bdBssIdBuf, 6)!=0)
			{
				RT_TRACE(COMP_SWAS, DBG_LOUD, ("ODM_PathDiversityBeforeLink92C(): ERROR!! This shall not happen.\n"));
				continue;
			}

			if(pTmpBssDesc->RecvSignalPower > pTestBssDesc->RecvSignalPower)
			{
				RT_TRACE(COMP_SWAS, DBG_LOUD, ("ODM_PathDiversityBeforeLink92C: Compare scan entry: Score++\n"));
				RT_PRINT_STR(COMP_SWAS, DBG_LOUD, "SSID: ", pTestBssDesc->bdSsIdBuf, pTestBssDesc->bdSsIdLen);
				RT_TRACE(COMP_SWAS, DBG_LOUD, ("Original: %d, Test: %d\n", pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
			
				Score++;
				PlatformMoveMemory(pTestBssDesc, pTmpBssDesc, sizeof(RT_WLAN_BSS));
			}
			else if(pTmpBssDesc->RecvSignalPower < pTestBssDesc->RecvSignalPower)
			{
				RT_TRACE(COMP_SWAS, DBG_LOUD, ("ODM_PathDiversityBeforeLink92C: Compare scan entry: Score--\n"));
				RT_PRINT_STR(COMP_SWAS, DBG_LOUD, "SSID: ", pTestBssDesc->bdSsIdBuf, pTestBssDesc->bdSsIdLen);
				RT_TRACE(COMP_SWAS, DBG_LOUD, ("Original: %d, Test: %d\n", pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
				Score--;
			}

		}

		if(pMgntInfo->NumBssDesc!=0 && Score<=0)
		{
			RT_TRACE(COMP_SWAS, DBG_LOUD,
				("ODM_PathDiversityBeforeLink92C(): DefaultRespPath=%d\n", pDM_PDTable->DefaultRespPath));

		}
		else
		{
			RT_TRACE(COMP_SWAS, DBG_LOUD, 
				("ODM_PathDiversityBeforeLink92C(): DefaultRespPath=%d\n", pDM_PDTable->DefaultRespPath));

			if(pDM_PDTable->DefaultRespPath == 0)
			{
				pDM_PDTable->OFDMTXPath = 0xFFFFFFFF;
				pDM_PDTable->CCKTXPath = 0xFFFFFFFF;
				odm_SetRespPath_92C(Adapter, 1);
			}
			else
			{
				pDM_PDTable->OFDMTXPath = 0x0;
				pDM_PDTable->CCKTXPath = 0x0;
				odm_SetRespPath_92C(Adapter, 0);
			}
			PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x01);


		}

		pDM_PDTable->PathDiv_NoLink_State = 0;

		return FALSE;
	}
#else
		return	FALSE;
#endif
	
}




VOID
ODM_PathDivChkPerPktRssi(
	PADAPTER		Adapter,
	BOOLEAN			bIsDefPort,
	BOOLEAN			bMatchBSSID,
	PRT_WLAN_STA	pEntry,
	PRT_RFD			pRfd
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);	
	BOOLEAN			bCount = FALSE;
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;
	pSWAT_T			pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;

	if(pHalData->RSSI_target==NULL && bIsDefPort && bMatchBSSID)
		bCount = TRUE;
	else if(pHalData->RSSI_target!=NULL && pEntry!=NULL && pHalData->RSSI_target==pEntry)
		bCount = TRUE;

	if(bCount)
	{
		if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
		{
			pHalData->RSSI_sum_A += pRfd->Status.RxPWDBAll;
			pHalData->RSSI_cnt_A++;
		}
		else
		{
			pHalData->RSSI_sum_B += pRfd->Status.RxPWDBAll;
			pHalData->RSSI_cnt_B++;

		}
	}
}



VOID
ODM_PathDivRestAfterLink(
	IN	PDM_ODM_T		pDM_Odm
	)
{
	PADAPTER		Adapter=pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	pSWAT_T			pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;

	pHalData->RSSI_cnt_A = 0;
	pHalData->RSSI_cnt_B = 0;
	pHalData->RSSI_test = FALSE;
	pDM_SWAT_Table->try_flag = 0x0;      
	pDM_SWAT_Table->RSSI_Trying = 0;
	pDM_SWAT_Table->SelectAntennaMap=0xAA;
	pDM_SWAT_Table->CurAntenna = MAIN_ANT;  
}


VOID
odm_PathDivChkAntSwitchCallback(
	PRT_TIMER		pTimer
)
{
	PADAPTER		Adapter = (PADAPTER)pTimer->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;

#if DEV_BUS_TYPE==RT_PCI_INTERFACE

#if USE_WORKITEM
	PlatformScheduleWorkItem(&pDM_Odm->PathDivSwitchWorkitem);
#else
	odm_PathDivChkAntSwitch(pDM_Odm);
#endif
#else
	PlatformScheduleWorkItem(&pDM_Odm->PathDivSwitchWorkitem);
#endif


}


VOID
odm_PathDivChkAntSwitchWorkitemCallback(
    IN PVOID            pContext
    )
{
	PADAPTER	pAdapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;

	odm_PathDivChkAntSwitch(pDM_Odm);
}



VOID
odm_PathDivChkAntSwitch(
	PDM_ODM_T		pDM_Odm
)
{
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PMGNT_INFO		pMgntInfo = &Adapter->MgntInfo;


	pSWAT_T			pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	s4Byte			curRSSI=100, RSSI_A, RSSI_B;
	u1Byte			nextAntenna=AUX_ANT;
	static u8Byte		lastTxOkCnt=0, lastRxOkCnt=0;
	u8Byte			curTxOkCnt, curRxOkCnt;
	static u8Byte		TXByteCnt_A=0, TXByteCnt_B=0, RXByteCnt_A=0, RXByteCnt_B=0;
	u8Byte			CurByteCnt=0, PreByteCnt=0;
	static u1Byte		TrafficLoad = TRAFFIC_LOW;
	u1Byte			Score_A=0, Score_B=0;
	u1Byte			i=0x0;
       static u1Byte        pathdiv_para=0x0;     
       static u1Byte        switchfirsttime=0x00;
	u1Byte			regB33 = (u1Byte)ODM_GetBBReg(pDM_Odm, PATHDIV_REG, BIT27);


       static u1Byte        fw_value=0x0;         
       PADAPTER            BuddyAdapter = Adapter->BuddyAdapter;    

	u1Byte                 PathDiv_Trigger = (u1Byte) ODM_GetBBReg(pDM_Odm, PATHDIV_TRI,BIT31);
	u1Byte                 PathDiv_Enable = pHalData->bPathDiv_Enable;


       if((BuddyAdapter==NULL)||(!PathDiv_Enable)||(PathDiv_Trigger)||(pHalData->CurrentBandType == BAND_ON_2_4G))
       {
           return;
       }
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD,("===================>odm_PathDivChkAntSwitch()\n"));

	if(switchfirsttime==0)
	{
	    if(regB33==0)
	    {
	       pDM_SWAT_Table->CurAntenna = MAIN_ANT;   
	    }	    
	}

	if(pDM_Odm->SupportICType != ODM_RTL8192D)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("odm_PathDiversityMechanims(): No PathDiv Mechanism.\n"));
		return;
	}

	if(pHalData->eRFPowerState==eRfOff)
	{
		return;
	}

       /*
	if(	Step != pDM_SWAT_Table->try_flag	)
	{
		ODM_SwAntDivRestAfterLink(Adapter);
	} */
	
	if(pDM_SWAT_Table->try_flag == 0xff)
	{
		if(pMgntInfo->mAssoc && !ACTING_AS_AP(Adapter))
		{
			pHalData->RSSI_target = NULL;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("odm_PathDivMechanism(): RSSI_target is DEF AP!\n"));
		}
		else
		{
			u1Byte			index = 0;
			PRT_WLAN_STA	pEntry = NULL;
			PADAPTER		pTargetAdapter = NULL;
		
			if(	pMgntInfo->mIbss || ACTING_AS_AP(Adapter) )
			{
				pTargetAdapter = Adapter;
			}
			else if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
			{
				pTargetAdapter = GetFirstExtAdapter(Adapter);
			}

			if(pTargetAdapter != NULL)
			{
				for(index=0; index<ODM_ASSOCIATE_ENTRY_NUM; index++)
				{
					pEntry = AsocEntry_EnumStation(pTargetAdapter, index);
					if(pEntry != NULL)
					{
						if(pEntry->bAssociated)
							break;			
					}
				}
			}

			if(pEntry == NULL)
			{
				ODM_PathDivRestAfterLink(pDM_Odm);
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): No Link.\n"));
				return;
			}
			else
			{
				pHalData->RSSI_target = pEntry;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): RSSI_target is PEER STA\n"));
			}
		}
			
		pHalData->RSSI_cnt_A = 0;
		pHalData->RSSI_cnt_B = 0;
		pDM_SWAT_Table->try_flag = 0;
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): Set try_flag to 0 prepare for peak!\n"));
		return;
	}
	else
	{
		curTxOkCnt = Adapter->TxStats.NumTxBytesUnicast - lastTxOkCnt;
		curRxOkCnt = Adapter->RxStats.NumRxBytesUnicast - lastRxOkCnt;
		lastTxOkCnt = Adapter->TxStats.NumTxBytesUnicast;
		lastRxOkCnt = Adapter->RxStats.NumRxBytesUnicast;
	
		if(pDM_SWAT_Table->try_flag == 1)  
		{
			if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
			{
				TXByteCnt_A += curTxOkCnt;
				RXByteCnt_A += curRxOkCnt;
			}
			else
			{
				TXByteCnt_B += curTxOkCnt;
				RXByteCnt_B += curRxOkCnt;
			}
		
			nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? AUX_ANT : MAIN_ANT;
			pDM_SWAT_Table->RSSI_Trying--;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: RSSI_Trying = %d\n",pDM_SWAT_Table->RSSI_Trying));
			if(pDM_SWAT_Table->RSSI_Trying == 0)
			{
				CurByteCnt = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? (TXByteCnt_A+RXByteCnt_A) : (TXByteCnt_B+RXByteCnt_B);
				PreByteCnt = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? (TXByteCnt_B+RXByteCnt_B) : (TXByteCnt_A+RXByteCnt_A);
				
				if(TrafficLoad == TRAFFIC_HIGH)
				{
					PreByteCnt =PreByteCnt*9;
				}
				else if(TrafficLoad == TRAFFIC_LOW)
				{
					PreByteCnt =PreByteCnt*2;
				}
				if(pHalData->RSSI_cnt_A > 0)
					RSSI_A = pHalData->RSSI_sum_A/pHalData->RSSI_cnt_A; 
				else
					RSSI_A = 0;
				if(pHalData->RSSI_cnt_B > 0)
					RSSI_B = pHalData->RSSI_sum_B/pHalData->RSSI_cnt_B; 
		             else
					RSSI_B = 0;
				curRSSI = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
				pDM_SWAT_Table->PreRSSI =  (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_B : RSSI_A;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: PreRSSI = %d, CurRSSI = %d\n",pDM_SWAT_Table->PreRSSI, curRSSI));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: preAntenna= %s, curAntenna= %s \n", 
				(pDM_SWAT_Table->PreAntenna == MAIN_ANT?"MAIN":"AUX"), (pDM_SWAT_Table->CurAntenna == MAIN_ANT?"MAIN":"AUX")));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: RSSI_A= %d, RSSI_cnt_A = %d, RSSI_B= %d, RSSI_cnt_B = %d\n",
					RSSI_A, pHalData->RSSI_cnt_A, RSSI_B, pHalData->RSSI_cnt_B));
			}

		}
		else  
		{
		
			if(pHalData->RSSI_cnt_A > 0)
				RSSI_A = pHalData->RSSI_sum_A/pHalData->RSSI_cnt_A; 
			else
				RSSI_A = 0;
			if(pHalData->RSSI_cnt_B > 0)
				RSSI_B = pHalData->RSSI_sum_B/pHalData->RSSI_cnt_B; 
			else
				RSSI_B = 0;	
			curRSSI = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
			pDM_SWAT_Table->PreRSSI =  (pDM_SWAT_Table->PreAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: PreRSSI = %d, CurRSSI = %d\n", pDM_SWAT_Table->PreRSSI, curRSSI));
		       ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: preAntenna= %s, curAntenna= %s \n", 
			(pDM_SWAT_Table->PreAntenna == MAIN_ANT?"MAIN":"AUX"), (pDM_SWAT_Table->CurAntenna == MAIN_ANT?"MAIN":"AUX")));

			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: RSSI_A= %d, RSSI_cnt_A = %d, RSSI_B= %d, RSSI_cnt_B = %d\n",
				RSSI_A, pHalData->RSSI_cnt_A, RSSI_B, pHalData->RSSI_cnt_B));
		}

		if((pDM_SWAT_Table->try_flag == 1)&&(pDM_SWAT_Table->RSSI_Trying == 0))
		{

			if(pDM_SWAT_Table->TestMode == TP_MODE)
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: TestMode = TP_MODE"));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH= TRY:CurByteCnt = %"i64fmt"d,", CurByteCnt));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH= TRY:PreByteCnt = %"i64fmt"d\n",PreByteCnt));		
				if(CurByteCnt < PreByteCnt)
				{
					if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
						pDM_SWAT_Table->SelectAntennaMap=pDM_SWAT_Table->SelectAntennaMap<<1;
					else
						pDM_SWAT_Table->SelectAntennaMap=(pDM_SWAT_Table->SelectAntennaMap<<1)+1;
				}
				else
				{
					if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
						pDM_SWAT_Table->SelectAntennaMap=(pDM_SWAT_Table->SelectAntennaMap<<1)+1;
					else
						pDM_SWAT_Table->SelectAntennaMap=pDM_SWAT_Table->SelectAntennaMap<<1;
				}
				for (i= 0; i<8; i++)
				{
					if(((pDM_SWAT_Table->SelectAntennaMap>>i)&BIT0) == 1)
						Score_A++;
					else
						Score_B++;
				}
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("SelectAntennaMap=%x\n ",pDM_SWAT_Table->SelectAntennaMap));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Score_A=%d, Score_B=%d\n", Score_A, Score_B));
			
				if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
				{
					nextAntenna = (Score_A >= Score_B)?MAIN_ANT:AUX_ANT;
				}
				else
				{
					nextAntenna = (Score_B >= Score_A)?AUX_ANT:MAIN_ANT;
				}
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: nextAntenna=%s\n",(nextAntenna==MAIN_ANT)?"MAIN":"AUX"));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: preAntenna= %s, curAntenna= %s \n", 
				(pDM_SWAT_Table->PreAntenna == MAIN_ANT?"MAIN":"AUX"), (pDM_SWAT_Table->CurAntenna == MAIN_ANT?"MAIN":"AUX")));

				if(nextAntenna != pDM_SWAT_Table->CurAntenna)
				{
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Switch back to another antenna"));
				}
				else
				{
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: current anntena is good\n"));
				}	
			}

                    
			if(pDM_SWAT_Table->TestMode == RSSI_MODE)
			{	
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: TestMode = RSSI_MODE"));
				pDM_SWAT_Table->SelectAntennaMap=0xAA;
				if(curRSSI < pDM_SWAT_Table->PreRSSI)
				{
					nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)?AUX_ANT : MAIN_ANT;
				}
				else
				{
					nextAntenna =pDM_SWAT_Table->CurAntenna;
				}
			}
			
			pDM_SWAT_Table->try_flag = 0;
			pHalData->RSSI_test = FALSE;
			pHalData->RSSI_sum_A = 0;
			pHalData->RSSI_cnt_A = 0;
			pHalData->RSSI_sum_B = 0;
			pHalData->RSSI_cnt_B = 0;
			TXByteCnt_A = 0;
			TXByteCnt_B = 0;
			RXByteCnt_A = 0;
			RXByteCnt_B = 0;
			
		}

		else if(pDM_SWAT_Table->try_flag == 0)
		{
			if(TrafficLoad == TRAFFIC_HIGH)
			{
				if ((curTxOkCnt+curRxOkCnt) > 3750000)//if(PlatformDivision64(curTxOkCnt+curRxOkCnt, 2) > 1875000)
					TrafficLoad = TRAFFIC_HIGH;
				else
					TrafficLoad = TRAFFIC_LOW;
			}
			else if(TrafficLoad == TRAFFIC_LOW)
				{
				if ((curTxOkCnt+curRxOkCnt) > 3750000)//if(PlatformDivision64(curTxOkCnt+curRxOkCnt, 2) > 1875000)
					TrafficLoad = TRAFFIC_HIGH;
				else
					TrafficLoad = TRAFFIC_LOW;
			}
			if(TrafficLoad == TRAFFIC_HIGH)
				pDM_SWAT_Table->bTriggerAntennaSwitch = 0;

				nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? AUX_ANT : MAIN_ANT;
				pDM_SWAT_Table->try_flag = 1;
				pHalData->RSSI_test = TRUE;
			if((curRxOkCnt+curTxOkCnt) > 1000)
			{
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
	                    pDM_SWAT_Table->RSSI_Trying = 4;                           
#else
	                    pDM_SWAT_Table->RSSI_Trying = 2;
#endif
				pDM_SWAT_Table->TestMode = TP_MODE;
			}
			else
			{
				pDM_SWAT_Table->RSSI_Trying = 2;
				pDM_SWAT_Table->TestMode = RSSI_MODE;

			}
                          
			pHalData->RSSI_sum_A = 0;
			pHalData->RSSI_cnt_A = 0;
			pHalData->RSSI_sum_B = 0;
			pHalData->RSSI_cnt_B = 0;
		}
	}
	
	if(nextAntenna != pDM_SWAT_Table->CurAntenna)
	{
	
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Change TX Antenna!\n "));
		if(nextAntenna==MAIN_ANT)
		{
		    ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Next Antenna is RF PATH A\n "));
		    pathdiv_para = 0x02;  
		    fw_value = 0x03;
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
                 odm_PathDiversity_8192D(pDM_Odm, pathdiv_para);
#else
                 ODM_FillH2CCmd(Adapter, ODM_H2C_PathDiv,1,(pu1Byte)(&fw_value));	
#endif
		}	
	       else if(nextAntenna==AUX_ANT)
	       {
	           ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Next Antenna is RF PATH B\n "));
	           if(switchfirsttime==0) 
	           {
	               switchfirsttime=0x01;
                      pathdiv_para = 0x00;
			  fw_value=0x00;   
					  
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
                     odm_PathDiversity_8192D(pDM_Odm, pathdiv_para);
#else
                     ODM_FillH2CCmd(Adapter, ODM_H2C_PathDiv,1,(pu1Byte)(&fw_value));	
			  ODM_delay_ms(500);
                     odm_PathDiversity_8192D(pDM_Odm, pathdiv_para);
			 		 
			 fw_value=0x01;   
                     ODM_FillH2CCmd(Adapter, ODM_H2C_PathDiv,1,(pu1Byte)(&fw_value));	
#endif	
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: FIRST TIME To DO PATH SWITCH!\n "));	
	           }		   
		    else
		    {
		        pathdiv_para = 0x01;
			 fw_value = 0x02;	
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
                     odm_PathDiversity_8192D(pDM_Odm, pathdiv_para);
#else
                     ODM_FillH2CCmd(Adapter, ODM_H2C_PathDiv,1,(pu1Byte)(&fw_value));	
#endif	
		    }		
	       }
	}

	pDM_SWAT_Table->PreAntenna = pDM_SWAT_Table->CurAntenna;
	pDM_SWAT_Table->CurAntenna = nextAntenna;
	pDM_SWAT_Table->PreRSSI = curRSSI;


	if(pDM_SWAT_Table->RSSI_Trying == 0)
		return;

	if(pDM_SWAT_Table->RSSI_Trying%2 == 0)
	{
		if(pDM_SWAT_Table->TestMode == TP_MODE)
		{
			if(TrafficLoad == TRAFFIC_HIGH)
			{
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 10 );
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Test another antenna for 10 ms\n"));
#else
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 20 );
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Test another antenna for 20 ms\n"));
#endif				
			}
			else if(TrafficLoad == TRAFFIC_LOW)
			{
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 50 );
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Test another antenna for 50 ms\n"));
			}
		}
		else  
		{
			ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 500 );
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Test another antenna for 500 ms\n"));
		}
	}
	else
	{
		if(pDM_SWAT_Table->TestMode == TP_MODE)
		{
			if(TrafficLoad == TRAFFIC_HIGH)
				
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 90 );
#else		
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 180);
#endif				
			else if(TrafficLoad == TRAFFIC_LOW)
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 100 );
		}
		else
			ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 500 );
	}
}


VOID
odm_SetRespPath_92C(
	IN	PADAPTER	Adapter,
	IN	u1Byte	DefaultRespPath
	)
{
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;

	RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_SetRespPath_92C: Select Response Path=%d\n",DefaultRespPath));
	if(DefaultRespPath != pDM_PDTable->DefaultRespPath)
	{
		if(DefaultRespPath == 0)
		{
			PlatformEFIOWrite1Byte(Adapter, 0x6D8, (PlatformEFIORead1Byte(Adapter, 0x6D8)&0xc0)|0x15);	
		}
		else
		{
			PlatformEFIOWrite1Byte(Adapter, 0x6D8, (PlatformEFIORead1Byte(Adapter, 0x6D8)&0xc0)|0x2A);
		}	
	}
	pDM_PDTable->DefaultRespPath = DefaultRespPath;
}


VOID
ODM_FillTXPathInTXDESC(
		IN	PADAPTER	Adapter,
		IN	PRT_TCB		pTcb,
		IN	pu1Byte		pDesc
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u4Byte	TXPath;
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;

	if(pHalData->PathDivCfg == 1)
	{	
		TXPath = (pDM_PDTable->OFDMTXPath >> pTcb->macId) & BIT0;
		if(TXPath == 0)
		{
			SET_TX_DESC_TX_ANTL_92C(pDesc,1);
			SET_TX_DESC_TX_ANT_HT_92C(pDesc,1);
		}
		else
		{
			SET_TX_DESC_TX_ANTL_92C(pDesc,2);
			SET_TX_DESC_TX_ANT_HT_92C(pDesc,2);
		}
		TXPath = (pDM_PDTable->CCKTXPath >> pTcb->macId) & BIT0;
		if(TXPath == 0)
		{
			SET_TX_DESC_TX_ANT_CCK_92C(pDesc,1);
		}
		else
		{
			SET_TX_DESC_TX_ANT_CCK_92C(pDesc,2);
		}
	}
}

VOID
odm_PathDivInit_92D(
IN	PDM_ODM_T 	pDM_Odm)
{
	pPATHDIV_PARA	pathIQK = &pDM_Odm->pathIQK;

	pathIQK->org_2g_RegC14=0x0;
	pathIQK->org_2g_RegC4C=0x0;
	pathIQK->org_2g_RegC80=0x0;
	pathIQK->org_2g_RegC94=0x0;
	pathIQK->org_2g_RegCA0=0x0;
	pathIQK->org_5g_RegC14=0x0;
	pathIQK->org_5g_RegCA0=0x0;
	pathIQK->org_5g_RegE30=0x0;
	pathIQK->swt_2g_RegC14=0x0;
	pathIQK->swt_2g_RegC4C=0x0;
	pathIQK->swt_2g_RegC80=0x0;
	pathIQK->swt_2g_RegC94=0x0;
	pathIQK->swt_2g_RegCA0=0x0;
	pathIQK->swt_5g_RegC14=0x0;
	pathIQK->swt_5g_RegCA0=0x0;
	pathIQK->swt_5g_RegE30=0x0;

}

#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN| ODM_CE))

VOID
odm_PHY_SaveAFERegisters(
	IN	PDM_ODM_T	pDM_Odm,
	IN	pu4Byte		AFEReg,
	IN	pu4Byte		AFEBackup,
	IN	u4Byte		RegisterNum
	)
{
	u4Byte	i;
	
	for( i = 0 ; i < RegisterNum ; i++){
		AFEBackup[i] = ODM_GetBBReg(pDM_Odm, AFEReg[i], bMaskDWord);
	}
}

VOID
odm_PHY_ReloadAFERegisters(
	IN	PDM_ODM_T	pDM_Odm,
	IN	pu4Byte		AFEReg,
	IN	pu4Byte		AFEBackup,
	IN	u4Byte		RegiesterNum
	)
{
	u4Byte	i;

	for(i = 0 ; i < RegiesterNum; i++)
	{
	
		ODM_SetBBReg(pDM_Odm, AFEReg[i], bMaskDWord, AFEBackup[i]);
	}
}

VOID
ODM_SingleDualAntennaDefaultSetting(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	PADAPTER	pAdapter	 =  pDM_Odm->Adapter;
	u1Byte btAntNum = 2;
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN))
	btAntNum=BT_GetPgAntNum(pAdapter);
#elif (DM_ODM_SUPPORT_TYPE & (ODM_CE))
#ifdef CONFIG_BT_COEXIST
	btAntNum = hal_btcoex_GetPgAntNum(pAdapter);
#endif
#endif

	if(btAntNum == 2)
	{
		pDM_SWAT_Table->ANTA_ON=TRUE;
		pDM_SWAT_Table->ANTB_ON=TRUE;
	}
#ifdef CONFIG_BT_COEXIST
	else if(btAntNum == 1)
	{// Set antenna A as default
		pDM_SWAT_Table->ANTA_ON=TRUE;
		pDM_SWAT_Table->ANTB_ON=FALSE;
	}
	else
	{
	}
#endif
}



BOOLEAN
ODM_SingleDualAntennaDetection(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			mode
	)
{
	PADAPTER	pAdapter	 =  pDM_Odm->Adapter;
	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	u4Byte		CurrentChannel,RfLoopReg;
	u1Byte		n;
	u4Byte		Reg88c, Regc08, Reg874, Regc50, Reg948=0, Regb2c=0, Reg92c=0, AFE_rRx_Wait_CCA=0;
	u1Byte		initial_gain = 0x5a;
	u4Byte		PSD_report_tmp;
	u4Byte		AntA_report = 0x0, AntB_report = 0x0,AntO_report=0x0;
	BOOLEAN		bResult = TRUE;
	u4Byte		AFE_Backup[16];
	u4Byte		AFE_REG_8723A[16] = {
					rRx_Wait_CCA, 	rTx_CCK_RFON, 
					rTx_CCK_BBON, 	rTx_OFDM_RFON,
					rTx_OFDM_BBON, 	rTx_To_Rx,
					rTx_To_Tx, 		rRx_CCK, 
					rRx_OFDM, 		rRx_Wait_RIFS, 
					rRx_TO_Rx,		rStandby,
					rSleep,			rPMPD_ANAEN, 	
					rFPGA0_XCD_SwitchControl, rBlue_Tooth};

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection()============> \n"));	

	
	if(!(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8192C|ODM_RTL8723B)))
		return bResult;

	if(!IS_ANT_DETECT_SUPPORT_SINGLE_TONE(pAdapter))
		return bResult;

	if(pDM_Odm->SupportICType == ODM_RTL8192C)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT10|BIT11, 0x3);
		ODM_SetBBReg(pDM_Odm, 0x808, BIT12|BIT13, 0x1);
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x0);
	}

	
	CurrentChannel = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_A, ODM_CHANNEL, bRFRegOffsetMask);
	RfLoopReg = ODM_GetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x00, bRFRegOffsetMask);
	if(!(pDM_Odm->SupportICType == ODM_RTL8723B))
	ODM_SetBBReg(pDM_Odm, rFPGA0_XA_RFInterfaceOE, ODM_DPDT, Antenna_A); 
#if (RTL8723B_SUPPORT == 1)
	else
	{
		Reg92c = ODM_GetBBReg(pDM_Odm, 0x92c, bMaskDWord);
		Reg948 = ODM_GetBBReg(pDM_Odm, rS0S1_PathSwitch, bMaskDWord);
		Regb2c = ODM_GetBBReg(pDM_Odm, AGC_table_select, bMaskDWord);
		ODM_SetBBReg(pDM_Odm, rDPDT_control, 0x3, 0x1);
		ODM_SetBBReg(pDM_Odm, rfe_ctrl_anta_src, 0xff, 0x77);
		ODM_SetBBReg(pDM_Odm, rS0S1_PathSwitch, 0x3ff, 0x000);
		ODM_SetBBReg(pDM_Odm, AGC_table_select, BIT31, 0x0);
	}
#endif
	ODM_StallExecution(10);
	
	Reg88c = ODM_GetBBReg(pDM_Odm, rFPGA0_AnalogParameter4, bMaskDWord);
	Regc08 = ODM_GetBBReg(pDM_Odm, rOFDM0_TRMuxPar, bMaskDWord);
	Reg874 = ODM_GetBBReg(pDM_Odm, rFPGA0_XCD_RFInterfaceSW, bMaskDWord);
	Regc50 = ODM_GetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskDWord);	
	
	if(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8192C))
	odm_PHY_SaveAFERegisters(pDM_Odm, AFE_REG_8723A, AFE_Backup, 16);	
	else if(pDM_Odm->SupportICType == ODM_RTL8723B)
		AFE_rRx_Wait_CCA = ODM_GetBBReg(pDM_Odm, rRx_Wait_CCA,bMaskDWord);
	
	ODM_SetBBReg(pDM_Odm, rFPGA0_PSDFunction, BIT14|BIT15, 0x0); 
	
	ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, ODM_CHANNEL, bRFRegOffsetMask, 0x7401);    
	
	if(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8192C))
	{
		ODM_SetBBReg(pDM_Odm, rRx_Wait_CCA, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rTx_CCK_RFON, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rTx_CCK_BBON, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rTx_OFDM_RFON, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rTx_OFDM_BBON, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rTx_To_Rx, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rTx_To_Tx, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rRx_CCK, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rRx_OFDM, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rRx_Wait_RIFS, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rRx_TO_Rx, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rStandby, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rSleep, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rPMPD_ANAEN, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rFPGA0_XCD_SwitchControl, bMaskDWord, 0x6FDB25A4);
		ODM_SetBBReg(pDM_Odm, rBlue_Tooth, bMaskDWord, 0x6FDB25A4);
	}
	else if(pDM_Odm->SupportICType == ODM_RTL8723B)
	{
		ODM_SetBBReg(pDM_Odm, rRx_Wait_CCA, bMaskDWord, 0x01c00016);
	}

	ODM_SetBBReg(pDM_Odm, rFPGA0_AnalogParameter4, bMaskDWord, 0xCCF000C0);
	
	ODM_SetBBReg(pDM_Odm, rOFDM0_TRMuxPar, bMaskDWord, 0x000800E4);
	ODM_SetBBReg(pDM_Odm, rFPGA0_XCD_RFInterfaceSW, bMaskDWord, 0x22208000);

	ODM_SetBBReg(pDM_Odm, rTx_IQK_Tone_A, bMaskDWord, 0x10008C1C);
	ODM_SetBBReg(pDM_Odm, rTx_IQK, bMaskDWord, 0x01007c00);	

	ODM_SetBBReg(pDM_Odm, rConfig_AntA, bMaskDWord, 0x00080000);
	ODM_SetBBReg(pDM_Odm, rConfig_AntA, bMaskDWord, 0x0f600000);
	ODM_SetBBReg(pDM_Odm, rRx_IQK, bMaskDWord, 0x01004800);
	ODM_SetBBReg(pDM_Odm, rRx_IQK_Tone_A, bMaskDWord, 0x10008c1f);
	ODM_SetBBReg(pDM_Odm, rTx_IQK_PI_A, bMaskDWord, 0x82150008);
	ODM_SetBBReg(pDM_Odm, rRx_IQK_PI_A, bMaskDWord, 0x28150008);
	ODM_SetBBReg(pDM_Odm, rIQK_AGC_Rsp, bMaskDWord, 0x001028d0);	

	if(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8192C))
	ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x0, 0xFFFFF, 0x50008);	
	
	ODM_SetBBReg(pDM_Odm, rFPGA0_IQK, bMaskDWord, 0x80800000);
	ODM_SetBBReg(pDM_Odm, rIQK_AGC_Pts, bMaskDWord, 0xf9000000);
	ODM_SetBBReg(pDM_Odm, rIQK_AGC_Pts, bMaskDWord, 0xf8000000);
	
	ODM_StallExecution(10000);

	PSD_report_tmp=0x0;
	for (n=0;n<2;n++)
 	{
 		PSD_report_tmp =  GetPSDData(pDM_Odm, 14, initial_gain);	
		if(PSD_report_tmp >AntA_report)
			AntA_report=PSD_report_tmp;
	}

	if(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8192C))
		ODM_SetBBReg(pDM_Odm, rFPGA0_XA_RFInterfaceOE, ODM_DPDT, Antenna_B); 
#if (RTL8723B_SUPPORT == 1)
	else if(pDM_Odm->SupportICType == ODM_RTL8723B)
		ODM_SetBBReg(pDM_Odm, rDPDT_control, 0x3, 0x2);
#endif

	ODM_StallExecution(10);	

	PSD_report_tmp=0x0;
	for (n=0;n<2;n++)
 	{
 		PSD_report_tmp =  GetPSDData(pDM_Odm, 14, initial_gain);	
		if(PSD_report_tmp > AntB_report)
			AntB_report=PSD_report_tmp;
	}

	if(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8192C))
		ODM_SetBBReg(pDM_Odm, rFPGA0_XA_RFInterfaceOE, ODM_DPDT, 0); 
#if (RTL8723B_SUPPORT == 1)
	else if(pDM_Odm->SupportICType == ODM_RTL8723B)
		ODM_SetBBReg(pDM_Odm, rDPDT_control, 0x3, 0x0);
#endif

	ODM_StallExecution(10);	
	
	PSD_report_tmp=0x0;
	for (n=0;n<2;n++)
 	{
 		PSD_report_tmp =  GetPSDData(pDM_Odm, 14, initial_gain);	
		if(PSD_report_tmp > AntO_report)
			AntO_report=PSD_report_tmp;
	}

	ODM_SetBBReg(pDM_Odm, rFPGA0_IQK, bMaskDWord, 0x00000000);

	if(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8192C))
		ODM_SetBBReg(pDM_Odm, rFPGA0_XA_RFInterfaceOE, ODM_DPDT, Antenna_A); 
#if (RTL8723B_SUPPORT == 1)
	else if(pDM_Odm->SupportICType == ODM_RTL8723B)
	{
		ODM_SetBBReg(pDM_Odm, rDPDT_control, bMaskDWord, Reg92c);

		ODM_SetBBReg(pDM_Odm, rS0S1_PathSwitch, bMaskDWord, Reg948);
		ODM_SetBBReg(pDM_Odm, AGC_table_select, bMaskDWord, Regb2c);
	}
#endif
	ODM_SetBBReg(pDM_Odm, rFPGA0_AnalogParameter4, bMaskDWord, Reg88c);
	ODM_SetBBReg(pDM_Odm, rOFDM0_TRMuxPar, bMaskDWord, Regc08);
	ODM_SetBBReg(pDM_Odm, rFPGA0_XCD_RFInterfaceSW, bMaskDWord, Reg874);
	ODM_SetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, 0x7F, 0x40);
	ODM_SetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskDWord, Regc50);
	ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask,CurrentChannel);
	ODM_SetRFReg(pDM_Odm, ODM_RF_PATH_A, 0x00, bRFRegOffsetMask,RfLoopReg);

	if(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8192C))
	odm_PHY_ReloadAFERegisters(pDM_Odm, AFE_REG_8723A, AFE_Backup, 16);	
	else if(pDM_Odm->SupportICType == ODM_RTL8723B)
		ODM_SetBBReg(pDM_Odm, rRx_Wait_CCA, bMaskDWord, AFE_rRx_Wait_CCA);

	if(pDM_Odm->SupportICType == ODM_RTL8723A)
	{
		if(mode==ANTTESTB)
		{
			if(AntA_report >=	100)
			{
				if(AntB_report > (AntA_report+1))
				{
					pDM_SWAT_Table->ANTB_ON=FALSE;
							ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Single Antenna A\n"));		
				}	
				else
				{
					pDM_SWAT_Table->ANTB_ON=TRUE;
							ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Dual Antenna is A and B\n"));	
				}	
			}
			else
			{
							ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Need to check again\n"));
				pDM_SWAT_Table->ANTB_ON=FALSE;
				bResult = FALSE;
			}
		}	
		else if(mode==ANTTESTALL)
		{
			if((AntO_report >=100) && (AntO_report <=118))
			{
				if(AntA_report > (AntO_report+1))
				{
					pDM_SWAT_Table->ANTA_ON=FALSE;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("Ant A is OFF\n"));
				}	
				else
				{
					pDM_SWAT_Table->ANTA_ON=TRUE;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("Ant A is ON\n"));
				}

				if(AntB_report > (AntO_report+2))
				{
					pDM_SWAT_Table->ANTB_ON=FALSE;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("Ant B is OFF\n"));
				}	
				else
				{
					pDM_SWAT_Table->ANTB_ON=TRUE;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("Ant B is ON\n"));
				}
				
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("psd_report_A[%d]= %d \n", 2416, AntA_report));	
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("psd_report_B[%d]= %d \n", 2416, AntB_report));	
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("psd_report_O[%d]= %d \n", 2416, AntO_report));
				
				pDM_Odm->AntDetectedInfo.bAntDetected= TRUE;
				pDM_Odm->AntDetectedInfo.dBForAntA = AntA_report;
				pDM_Odm->AntDetectedInfo.dBForAntB = AntB_report;
				pDM_Odm->AntDetectedInfo.dBForAntO = AntO_report;
				
				}
			else
				{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("return FALSE!!\n"));
				bResult = FALSE;
			}
		}
	}
	else if(pDM_Odm->SupportICType == ODM_RTL8192C)
	{
		if(AntA_report >=	100)
		{
			if(AntB_report > (AntA_report+2))
			{
				pDM_SWAT_Table->ANTA_ON=FALSE;
				pDM_SWAT_Table->ANTB_ON=TRUE;
				ODM_SetBBReg(pDM_Odm,  rFPGA0_XA_RFInterfaceOE, 0x300, Antenna_B);
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Single Antenna B\n"));		
			}	
			else if(AntA_report > (AntB_report+2))
			{
				pDM_SWAT_Table->ANTA_ON=TRUE;
				pDM_SWAT_Table->ANTB_ON=FALSE;
				ODM_SetBBReg(pDM_Odm,  rFPGA0_XA_RFInterfaceOE, 0x300, Antenna_A);
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Single Antenna A\n"));
			}	
			else
			{
				pDM_SWAT_Table->ANTA_ON=TRUE;
				pDM_SWAT_Table->ANTB_ON=TRUE;
				RT_TRACE(COMP_ANTENNA, DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Dual Antenna \n"));
			}
		}
		else
		{
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Need to check again\n"));
			pDM_SWAT_Table->ANTA_ON=TRUE;
			pDM_SWAT_Table->ANTB_ON=FALSE;
			bResult = FALSE;
		}
	}
	else if(pDM_Odm->SupportICType == ODM_RTL8723B)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("psd_report_A[%d]= %d \n", 2416, AntA_report));	
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("psd_report_B[%d]= %d \n", 2416, AntB_report));	
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("psd_report_O[%d]= %d \n", 2416, AntO_report));
		
		if(mode==ANTTESTB)
		{
			if(AntA_report >=100 && AntA_report <= 116)
			{
				if(AntB_report >= (AntA_report+4) && AntB_report > 116)
				{
					pDM_SWAT_Table->ANTB_ON=FALSE;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Single Antenna A\n"));		
				}	
				else if(AntB_report >=100 && AntB_report <= 116)
				{
					pDM_SWAT_Table->ANTB_ON=TRUE;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Dual Antenna is A and B\n"));	
				}
				else
				{
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Need to check again\n"));
					pDM_SWAT_Table->ANTB_ON=FALSE;
					bResult = FALSE;
				}
			}
			else
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Need to check again\n"));
				pDM_SWAT_Table->ANTB_ON=FALSE;
				bResult = FALSE;
			}
		}	
		else if(mode==ANTTESTALL)
		{
			if((AntA_report >= 100) && (AntB_report >= 100) && (AntA_report <= 120) && (AntB_report <= 120))
			{
				if((AntA_report - AntB_report < 2) || (AntB_report - AntA_report < 2))
				{
					pDM_SWAT_Table->ANTA_ON=TRUE;
					pDM_SWAT_Table->ANTB_ON=TRUE;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("ODM_SingleDualAntennaDetection(): Dual Antenna\n"));
				}
				else if(((AntA_report - AntB_report >= 2) && (AntA_report - AntB_report <= 4)) || 
					((AntB_report - AntA_report >= 2) && (AntB_report - AntA_report <= 4)))
				{
					pDM_SWAT_Table->ANTA_ON=FALSE;
					pDM_SWAT_Table->ANTB_ON=FALSE;
					bResult = FALSE;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Need to check again\n"));
				}
				else
				{
					pDM_SWAT_Table->ANTA_ON = TRUE;
					pDM_SWAT_Table->ANTB_ON=FALSE;
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("ODM_SingleDualAntennaDetection(): Single Antenna A\n"));
				}
				
				pDM_Odm->AntDetectedInfo.bAntDetected= TRUE;
				pDM_Odm->AntDetectedInfo.dBForAntA = AntA_report;
				pDM_Odm->AntDetectedInfo.dBForAntB = AntB_report;
				pDM_Odm->AntDetectedInfo.dBForAntO = AntO_report;
				
			}
			else
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("return FALSE!!\n"));
				bResult = FALSE;
			}
		}
	}
		
	return bResult;

}


#endif  

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN| ODM_CE))

VOID
odm_Set_RA_DM_ARFB_by_Noisy(
	IN	PDM_ODM_T	pDM_Odm
)
{
	if (pDM_Odm->bNoisyState){
		ODM_Write4Byte(pDM_Odm,0x430,0x00000000);
		ODM_Write4Byte(pDM_Odm,0x434,0x05040200);
	}
	else{
		ODM_Write4Byte(pDM_Odm,0x430,0x02010000);
		ODM_Write4Byte(pDM_Odm,0x434,0x07050403);
	}
	
}

VOID
ODM_UpdateNoisyState(
	IN	PDM_ODM_T	pDM_Odm,
	IN 	BOOLEAN 	bNoisyStateFromC2H
	)
{
	if(pDM_Odm->SupportICType == ODM_RTL8821  || pDM_Odm->SupportICType == ODM_RTL8812  || 
	   pDM_Odm->SupportICType == ODM_RTL8723B || pDM_Odm->SupportICType == ODM_RTL8192E || pDM_Odm->SupportICType == ODM_RTL8188E)
	{
		pDM_Odm->bNoisyState = bNoisyStateFromC2H;
	}
	odm_Set_RA_DM_ARFB_by_Noisy(pDM_Odm);
};

u4Byte
Set_RA_DM_Ratrbitmap_by_Noisy(
	IN	PDM_ODM_T	pDM_Odm,
	IN	WIRELESS_MODE	WirelessMode,
	IN	u4Byte			ratr_bitmap,
	IN	u1Byte			rssi_level
)
{
	u4Byte ret_bitmap = ratr_bitmap;
	switch (WirelessMode)
	{
		case WIRELESS_MODE_AC_24G :
		case WIRELESS_MODE_AC_5G :
		case WIRELESS_MODE_AC_ONLY:
			if (pDM_Odm->bNoisyState){
				if (rssi_level==1)
					ret_bitmap&=0xfe3f0e08;
				else if (rssi_level==2)
					ret_bitmap&=0xff3f8f8c;
				else if (rssi_level==3)
					ret_bitmap&=0xffffffcc ;
				else
					ret_bitmap&=0xffffffff ;
			}
			else{                                  
				if (rssi_level==1){
					ret_bitmap&=0xfc3e0c08;
				}
				else if (rssi_level==2){
					ret_bitmap&=0xfe3f0e08;
				}
				else if (rssi_level==3){
					ret_bitmap&=0xffbfefcc;
				}
				else{
					ret_bitmap&=0x0fffffff;
				}
			}
			break;
		case WIRELESS_MODE_B:
		case WIRELESS_MODE_A:
		case WIRELESS_MODE_G:
		case WIRELESS_MODE_N_24G:
		case WIRELESS_MODE_N_5G:
			if (pDM_Odm->bNoisyState){
				if (rssi_level==1)
					ret_bitmap&=0x0f0e0c08;
				else if (rssi_level==2)
					ret_bitmap&=0x0f8f0e0c;
				else if (rssi_level==3)
					ret_bitmap&=0x0fefefcc ;
				else
					ret_bitmap&=0xffffffff ;
			}
			else{
				if (rssi_level==1){
					ret_bitmap&=0x0f8f0e08;
				}
				else if (rssi_level==2){
					ret_bitmap&=0x0fcf8f8c;
				}
				else if (rssi_level==3){
					ret_bitmap&=0x0fffffcc;
				}
				else{
					ret_bitmap&=0x0fffffff;
				}
			}
			break;
		default:
			break;
	}
	return ret_bitmap;

}



VOID
ODM_UpdateInitRate(
	IN	PDM_ODM_T	pDM_Odm,
	IN	u1Byte		Rate
	)
{
	u1Byte			p = 0;

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_TX_PWR_TRACK, ODM_DBG_LOUD,("Get C2H Command! Rate=0x%x\n", Rate));
	
	if(pDM_Odm->SupportICType == ODM_RTL8821  || pDM_Odm->SupportICType == ODM_RTL8812  || 
	   pDM_Odm->SupportICType == ODM_RTL8723B || pDM_Odm->SupportICType == ODM_RTL8192E || pDM_Odm->SupportICType == ODM_RTL8188E)
	{
		pDM_Odm->TxRate = Rate;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	#if DEV_BUS_TYPE==RT_PCI_INTERFACE
		#if USE_WORKITEM
		PlatformScheduleWorkItem(&pDM_Odm->RaRptWorkitem);
		#else
		if(pDM_Odm->SupportICType == ODM_RTL8821)
		{
			ODM_TxPwrTrackSetPwr8821A(pDM_Odm, MIX_MODE, ODM_RF_PATH_A, 0);
		}
		else if(pDM_Odm->SupportICType == ODM_RTL8812)
		{
			for (p = ODM_RF_PATH_A; p < MAX_PATH_NUM_8812A; p++) 		
			{
				ODM_TxPwrTrackSetPwr8812A(pDM_Odm, MIX_MODE, p, 0);
			}
		}
		else if(pDM_Odm->SupportICType == ODM_RTL8723B)
		{
			ODM_TxPwrTrackSetPwr_8723B(pDM_Odm, MIX_MODE, ODM_RF_PATH_A, 0);
		}
		else if(pDM_Odm->SupportICType == ODM_RTL8192E)
		{
			for (p = ODM_RF_PATH_A; p < MAX_PATH_NUM_8192E; p++) 		
			{
				ODM_TxPwrTrackSetPwr92E(pDM_Odm, MIX_MODE, p, 0);
			}
		}
		else if(pDM_Odm->SupportICType == ODM_RTL8188E)
		{
			ODM_TxPwrTrackSetPwr88E(pDM_Odm, MIX_MODE, ODM_RF_PATH_A, 0);
		}
		#endif
	#else
		PlatformScheduleWorkItem(&pDM_Odm->RaRptWorkitem);
	#endif	
#endif
	}
	else
		return;
}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID
ODM_UpdateInitRateWorkItemCallback(
    IN PVOID            pContext
    )
{
	PADAPTER	Adapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;

	u1Byte			p = 0;	

	if(pDM_Odm->SupportICType == ODM_RTL8821)
	{
		ODM_TxPwrTrackSetPwr8821A(pDM_Odm, MIX_MODE, ODM_RF_PATH_A, 0);
	}
	else if(pDM_Odm->SupportICType == ODM_RTL8812)
	{
		for (p = ODM_RF_PATH_A; p < MAX_PATH_NUM_8812A; p++)   
		{
			ODM_TxPwrTrackSetPwr8812A(pDM_Odm, MIX_MODE, p, 0);
		}
	}
	else if(pDM_Odm->SupportICType == ODM_RTL8723B)
	{
			ODM_TxPwrTrackSetPwr_8723B(pDM_Odm, MIX_MODE, ODM_RF_PATH_A, 0);
	}
	else if(pDM_Odm->SupportICType == ODM_RTL8192E)
	{
		for (p = ODM_RF_PATH_A; p < MAX_PATH_NUM_8192E; p++)   
		{
			ODM_TxPwrTrackSetPwr92E(pDM_Odm, MIX_MODE, p, 0);
		}
	}
	else if(pDM_Odm->SupportICType == ODM_RTL8188E)
	{
			ODM_TxPwrTrackSetPwr88E(pDM_Odm, MIX_MODE, ODM_RF_PATH_A, 0);
	}
}
#endif
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
/* Justin: According to the current RRSI to adjust Response Frame TX power, 2012/11/05 */
void odm_dtc(PDM_ODM_T pDM_Odm)
{
#ifdef CONFIG_DM_RESP_TXAGC
	#define DTC_BASE            35	/* RSSI higher than this value, start to decade TX power */
	#define DTC_DWN_BASE       (DTC_BASE-5)	/* RSSI lower than this value, start to increase TX power */

	/* RSSI vs TX power step mapping: decade TX power */
	static const u8 dtc_table_down[]={
		DTC_BASE,
		(DTC_BASE+5),
		(DTC_BASE+10),
		(DTC_BASE+15),
		(DTC_BASE+20),
		(DTC_BASE+25)
	};

	/* RSSI vs TX power step mapping: increase TX power */
	static const u8 dtc_table_up[]={
		DTC_DWN_BASE,
		(DTC_DWN_BASE-5),
		(DTC_DWN_BASE-10),
		(DTC_DWN_BASE-15),
		(DTC_DWN_BASE-15),
		(DTC_DWN_BASE-20),
		(DTC_DWN_BASE-20),
		(DTC_DWN_BASE-25),
		(DTC_DWN_BASE-25),
		(DTC_DWN_BASE-30),
		(DTC_DWN_BASE-35)
	};

	u8 i;
	u8 dtc_steps=0;
	u8 sign;
	u8 resp_txagc=0;

	#if 0
	/* As DIG is disabled, DTC is also disable */
	if(!(pDM_Odm->SupportAbility & ODM_XXXXXX))
		return;
	#endif

	if (DTC_BASE < pDM_Odm->RSSI_Min) {
		/* need to decade the CTS TX power */
		sign = 1;
		for (i=0;i<ARRAY_SIZE(dtc_table_down);i++)
		{
			if ((dtc_table_down[i] >= pDM_Odm->RSSI_Min) || (dtc_steps >= 6))
				break;
			else
				dtc_steps++;
		}
	}
#if 0
	else if (DTC_DWN_BASE > pDM_Odm->RSSI_Min)
	{
		/* needs to increase the CTS TX power */
		sign = 0;
		dtc_steps = 1;
		for (i=0;i<ARRAY_SIZE(dtc_table_up);i++)
		{
			if ((dtc_table_up[i] <= pDM_Odm->RSSI_Min) || (dtc_steps>=10))
				break;
			else
				dtc_steps++;
		}
	}
#endif
	else
	{
		sign = 0;
		dtc_steps = 0;
	}

	resp_txagc = dtc_steps | (sign << 4);
	resp_txagc = resp_txagc | (resp_txagc << 5);
	ODM_Write1Byte(pDM_Odm, 0x06d9, resp_txagc);

	DBG_871X("%s RSSI_Min:%u, set RESP_TXAGC to %s %u\n", 
		__func__, pDM_Odm->RSSI_Min, sign?"minus":"plus", dtc_steps);
#endif /* CONFIG_RESP_TXAGC_ADJUST */
}

#endif /* #if (DM_ODM_SUPPORT_TYPE == ODM_CE) */

