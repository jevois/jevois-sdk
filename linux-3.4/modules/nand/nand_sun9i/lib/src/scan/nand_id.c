/*
 * Copyright (C) 2013 Allwinnertech
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include "../include/nand_scan.h"


static struct __OptionalPhyOpPar_t PhysicArchiPara0 =
{
  {0x00, 0x30},          
  {0x11, 0x81},          
  {0x00, 0x00, 0x35},    
  {0x85, 0x11, 0x81},    
  0x70,                  
  0xf1,                  
  0xf2,                  
  0x01,                  
  1                      
};

static struct __OptionalPhyOpPar_t PhysicArchiPara1 =
{
  {0x60, 0x30},          
  {0x11, 0x81},          
  {0x60, 0x60, 0x35},    
  {0x85, 0x11, 0x81},    
  0x70,                  
  0xf1,                  
  0xf2,                  
  0x00,                  
  1                      
};

static struct __OptionalPhyOpPar_t PhysicArchiPara2 =
{
  {0x00, 0x30},          
  {0x11, 0x81},          
  {0x00, 0x00, 0x35},    
  {0x85, 0x11, 0x81},    
  0x70,                  
  0xf1,                  
  0xf2,                  
  0x02,                  
  1                      
};

static struct __OptionalPhyOpPar_t PhysicArchiPara3 =
{
  {0x60, 0x60},          
  {0x11, 0x81},          
  {0x60, 0x60, 0x35},    
  {0x85, 0x11, 0x81},    
  0x70,                  
  0xf1,                  
  0xf2,                  
  0x02,                  
  1                      
};

static struct __OptionalPhyOpPar_t PhysicArchiPara4 =
{
  {0x00, 0x30},          
  {0x11, 0x80},          
  {0x00, 0x00, 0x35},    
  {0x85, 0x11, 0x80},    
  0x70,                  
  0x78,                  
  0x78,                  
  0x01,                  
  1                      
};

static struct __OptionalPhyOpPar_t PhysicArchiPara5 =
{
  {0x00, 0x30},          
  {0x11, 0x80},          
  {0x00, 0x00, 0x30},    
  {0x8c, 0x11, 0x8c},    
  0x71,                  
  0x70,                  
  0x70,                  
  0x00,                  
  0                      
};

static struct __OptionalPhyOpPar_t PhysicArchiPara6 =
{
  {0x00, 0x30},          
  {0x11, 0x80},          
  {0x00, 0x00, 0x30},    
  {0x8c, 0x11, 0x8c},    
  0x71,                  
  0x70,                  
  0x70,                  
  0x00,                  
  1024                   
};

static struct __OptionalPhyOpPar_t PhysicArchiPara7 =
{
  {0x00, 0x30},          
  {0x11, 0x80},          
  {0x00, 0x00, 0x30},    
  {0x8c, 0x11, 0x8c},    
  0x71,                  
  0x70,                  
  0x70,                  
  0x00,                  
  2048                   
};

static struct __OptionalPhyOpPar_t PhysicArchiPara8 =
{
  {0x00, 0x30},          
  {0x11, 0x80},          
  {0x00, 0x00, 0x30},    
  {0x8c, 0x11, 0x8c},    
  0x71,                  
  0x70,                  
  0x70,                  
  0x02,                  
  1                      
};

static struct __OptionalPhyOpPar_t PhysicArchiPara9 =
{
  {0x00, 0x30},          
  {0x11, 0x81},          
  {0x00, 0x00, 0x30},    
  {0x8c, 0x11, 0x8c},    
  0x71,                  
  0x70,                  
  0x70,                  
  0x02,                  
  1                      
};

static struct __OptionalPhyOpPar_t DefualtPhysicArchiPara =
{
  {0x00, 0x30},          
  {0x11, 0x81},          
  {0x00, 0x00, 0x35},    
  {0x85, 0x11, 0x81},    
  0x70,                  
  0xf1,                  
  0xf2,                  
  0x00,                  
  1                      
};

static struct __NfcInitDdrInfo DefDDRInfo = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
} ;

static struct __NfcInitDdrInfo DDRInfo1 = {
  0,
  0,
  0,
  0,
  0,
  0,
  2,
  2,
} ;

/*
  LSB Page Type: OpOpt[19:16]  (.OperationOpt)
  0x0 : hynix 26nm, 20nm; micron 256 page;
  0x1 : samsung & toshiba 128 page;
  0x2 : micron 20nm, 256 page; (20nm)
  0x3 : hynix 16nm, 128 page per block
  0x4 : hynix 16nm, 256 page per block
  0x5 : micrion l85a, 512 page per block(20nm)
  0x6 : micrion l95b, 512 page per block(16nm)
  0x6~0xF : Reserved
*/


struct __NandPhyInfoPar_t SamsungNandTbl[] =
{
  { {0xec, 0xd5, 0x14, 0xb6, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     4096,   0x00000008,   896,    30,     0,       0,        0,     &PhysicArchiPara3, &DefDDRInfo,  0x000011},  
  { {0xec, 0xd7, 0x55, 0xb6, 0xff, 0xff, 0xff, 0xff }, 2,     8,     128,     4096,   0x00000028,   896,    30,     0,       0,        0,     &PhysicArchiPara3, &DefDDRInfo,  0x000012},  
  { {0xec, 0xd7, 0xd5, 0x29, 0xff, 0xff, 0xff, 0xff }, 2,     8,     128,     4096,   0x00000028,   896,    30,     0,       0,        0,     &PhysicArchiPara3, &DefDDRInfo,  0x000013},  
  { {0xec, 0xd7, 0x94, 0x72, 0xff, 0xff, 0xff, 0xff }, 1,    16,     128,     4096,   0x00000008,   896,    30,     2,       0,        0,     &PhysicArchiPara3, &DefDDRInfo,  0x000014},  
  { {0xec, 0xd5, 0x98, 0x71, 0xff, 0xff, 0xff, 0xff }, 1,     8,     256,     2048,   0x00000008,   896,    30,     3,       0,        0,     &PhysicArchiPara3, &DefDDRInfo,  0x000015},  
  
  { {0xec, 0xd5, 0x94, 0x29, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     4096,   0x00000008,   896,    30,     0,       0,        0,     &PhysicArchiPara3, &DefDDRInfo,  0x000016},  
  { {0xec, 0xd5, 0x84, 0x72, 0xff, 0xff, 0xff, 0xff }, 1,    16,     128,     2048,   0x00011000,   896,    24,     2,       0,        0,     &PhysicArchiPara3, &DefDDRInfo,  0x000017 },  
  { {0xec, 0xd5, 0x94, 0x76, 0x54, 0xff, 0xff, 0xff }, 1,    16,     128,     2048,   0x00011408,   896,    30,     2,       0,        0,     &PhysicArchiPara3, &DefDDRInfo,  0x000018 },  
  { {0xec, 0xd3, 0x84, 0x72, 0xff, 0xff, 0xff, 0xff }, 1,    16,     128,     1024,   0x00000000,   896,    24,     2,       0,        0,     &PhysicArchiPara3, &DefDDRInfo,  0x000019 },  
  { {0xec, 0xd7, 0x94, 0x76, 0xff, 0xff, 0xff, 0xff }, 1,    16,     128,     4096,   0x00011088,   896,    30,     3,       0,        0,     &PhysicArchiPara3, &DefDDRInfo,  0x00001a },  
  { {0xec, 0xd7, 0x94, 0x7A, 0xff, 0xff, 0xff, 0xff }, 1,    16,     128,     4096,   0x00011088,   896,    30,     3,       0,        0,     &PhysicArchiPara3, &DefDDRInfo,  0x00001b },  
  { {0xec, 0xde, 0xd5, 0x7A, 0x58, 0xff, 0xff, 0xff }, 2,    16,     128,   4096,   0x00011888,   896,    30,   3,     0,    0,   &PhysicArchiPara3, &DefDDRInfo,  0x00001c },  
  
  { {0xec, 0xd7, 0x94, 0x7A, 0x54, 0xc3, 0xff, 0xff }, 1,    16,     128,     4096,   0x20000088,   896,    60,     1,       0,        3,     &PhysicArchiPara3, &DefDDRInfo,  0x00001d },  
  { {0xec, 0xd7, 0x14, 0x76, 0x54, 0xc2, 0xff, 0xff }, 1,    16,     128,     4096,   0x20011088,   896,    30,     3,       0,        3,     &PhysicArchiPara3, &DefDDRInfo,  0x000022 },  
  { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 0,     0,       0,        0,   0x00000000,     0,     0,     0,       0,        0,             0,              0,       0xffffff },  
};


struct __NandPhyInfoPar_t HynixNandTbl[] =
{
  
  { {0xad, 0xd3, 0x14, 0xb6, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     2048,   0x00000008,   896,    30,     0,      0,         0,    &PhysicArchiPara3, &DefDDRInfo,  0x010014 },  
  { {0xad, 0xd5, 0x14, 0xb6, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     4096,   0x00000008,   896,    30,     0,      0,         0,    &PhysicArchiPara3, &DefDDRInfo,  0x010015 },  
  { {0xad, 0xd7, 0x55, 0xb6, 0xff, 0xff, 0xff, 0xff }, 2,     8,     128,     4096,   0x00000008,   896,    30,     0,      0,         0,    &PhysicArchiPara3, &DefDDRInfo,  0x010016 },  
  { {0xad, 0xd5, 0x94, 0x25, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     4096,   0x00000008,   896,    30,     2,      0,         0,    &PhysicArchiPara3, &DefDDRInfo,  0x010017 },  
  { {0xad, 0xd7, 0x95, 0x25, 0xff, 0xff, 0xff, 0xff }, 2,     8,     128,     4096,   0x00000008,   896,    30,     2,      0,         0,    &PhysicArchiPara3, &DefDDRInfo,  0x010018 },  
  { {0xad, 0xd5, 0x95, 0x25, 0xff, 0xff, 0xff, 0xff }, 2,     8,     128,     4096,   0x00000008,   896,    30,     2,      0,         0,    &PhysicArchiPara3, &DefDDRInfo,  0x010019 },  
  { {0xad, 0xd5, 0x94, 0x9A, 0xff, 0xff, 0xff, 0xff }, 1,    16,     256,     1024,   0x00001000,   896,    30,     2,      0,         0,    &PhysicArchiPara3, &DefDDRInfo,  0x01001a },  
  { {0xad, 0xd7, 0x94, 0x9A, 0xff, 0xff, 0xff, 0xff }, 1,    16,     256,     2048,   0x00001008,   896,    30,     2,      0,         0,    &PhysicArchiPara3, &DefDDRInfo,  0x01001b },  
  { {0xad, 0xde, 0xd5, 0x9A, 0xff, 0xff, 0xff, 0xff }, 2,    16,     256,     2048,   0x00001008,   896,    30,     2,      0,         0,    &PhysicArchiPara3, &DefDDRInfo,  0x01001c },  
  { {0xad, 0xd7, 0x94, 0x25, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     8192,   0x00001008,   896,    30,     2,      0,         0,    &PhysicArchiPara3, &DefDDRInfo,  0x01001d },  
  { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 0,     0,       0,        0,   0x00000000,     0,     0,     0,      0,         0,               0,            0,      0xffffff },
};


struct __NandPhyInfoPar_t ToshibaNandTbl[] =
{
  { {0x98, 0xd3, 0x94, 0xba, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     2048,   0x00000008,   896,    20,     0,     0,          0,   &PhysicArchiPara6, &DefDDRInfo,  0x020008 },  
  { {0x98, 0xd7, 0x95, 0xba, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     8192,   0x00000008,   896,    30,     2,     0,          0,   &PhysicArchiPara7, &DefDDRInfo,  0x020009 },  
  { {0x98, 0xd5, 0x94, 0xba, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     4096,   0x00000008,   896,    30,     2,     0,          0,   &PhysicArchiPara7, &DefDDRInfo,  0x02000a },   
  { {0x98, 0xd5, 0x94, 0x32, 0xff, 0xff, 0xff, 0xff }, 1,    16,     128,     2048,   0x00000008,   896,    25,     1,     0,          0,   &PhysicArchiPara8, &DefDDRInfo,  0x02000b },   
  { {0x98, 0xd7, 0x94, 0x32, 0xff, 0xff, 0xff, 0xff }, 1,    16,     128,     4096,   0x00000008,   896,    25,     2,     0,          0,   &PhysicArchiPara8, &DefDDRInfo,  0x02000c },   
  { {0x98, 0xd7, 0x95, 0x32, 0xff, 0xff, 0xff, 0xff }, 2,    16,     128,     4096,   0x00000008,   896,    25,     1,     0,          0,   &PhysicArchiPara8, &DefDDRInfo,  0x02000d },   
  
  { {0x98, 0xd7, 0xd4, 0x32, 0x76, 0x55, 0xff, 0xff }, 1,    16,     128,     4096,   0x00000088,   896,    30,     2,     0,          0,   &PhysicArchiPara8, &DefDDRInfo,  0x020015 },   
  { {0x98, 0xd3, 0x91, 0x26, 0x76, 0xff, 0xff, 0xff }, 1,     8,      64,     4096,   0x00088,   896,    30,     3,    0,       0,   &PhysicArchiPara9,    &DefDDRInfo,     0x020019 },   
  
  { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 0,     0,       0,        0,   0x00000000,     0,     0,     0,         0,      0,          0,             0,         0xffffff},  
};


struct __NandPhyInfoPar_t MicronNandTbl[] =
{
  { {0x2c, 0xd5, 0x94, 0x3e, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     4096,   0x00000008,   896,    30,     0,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x030009 },  
  { {0x2c, 0xd5, 0xd5, 0x3e, 0xff, 0xff, 0xff, 0xff }, 2,     8,     128,     4096,   0x00000008,   896,    30,     0,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x03000a },  
  { {0x2c, 0xd7, 0x94, 0x3e, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     8192,   0x00000208,   896,    30,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x03000b },  
  { {0x2c, 0xd7, 0xd5, 0x3e, 0xff, 0xff, 0xff, 0xff }, 2,     8,     128,     4096,   0x00000008,   896,    30,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x03000c },  
  { {0x2c, 0xd9, 0xd5, 0x3e, 0xff, 0xff, 0xff, 0xff }, 2,     8,     128,     8192,   0x00000008,   896,    30,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x03000d },  
  { {0x2c, 0x68, 0x04, 0x46, 0xff, 0xff, 0xff, 0xff }, 1,     8,     256,     4096,   0x00001208,   896,    30,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x03000e },  
  { {0x2c, 0x88, 0x05, 0xC6, 0xff, 0xff, 0xff, 0xff }, 2,     8,     256,     4096,   0x00001208,   896,    30,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x03000f },  
  { {0x2c, 0x88, 0x04, 0x4B, 0xff, 0xff, 0xff, 0xff }, 1,    16,     256,     4096,   0x00001208,   896,    40,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x030010 },  
  { {0x2c, 0x68, 0x04, 0x4A, 0xff, 0xff, 0xff, 0xff }, 1,     8,     256,     4096,   0x00001208,   896,    40,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x030011 },  
  { {0x2c, 0x48, 0x04, 0x4A, 0xff, 0xff, 0xff, 0xff }, 1,     8,     256,     2048,   0x00001208,   896,    40,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x030012 },  
  { {0x2c, 0x48, 0x04, 0x46, 0xff, 0xff, 0xff, 0xff }, 1,     8,     256,     2048,   0x00001208,   896,    30,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x030013 },  
  { {0x2c, 0x88, 0x24, 0x4B, 0xff, 0xff, 0xff, 0xff }, 1,    16,     256,     4096,   0x00001208,   870,    40,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x030016 },  
  { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 0,     0,       0,        0,   0x0000,        0,      0,     0,     0,          0,            0,            0,        0xffffff },  
};


struct __NandPhyInfoPar_t IntelNandTbl[] =
{
  { {0x89, 0xd7, 0x94, 0x3e, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     8192,   0x00000008,   864,    30,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x040002 },  
  { {0x89, 0xd5, 0x94, 0x3e, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     4096,   0x00000008,   864,    30,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x040003 },  
  { {0x89, 0xd7, 0xd5, 0x3e, 0xff, 0xff, 0xff, 0xff }, 1,     8,     128,     8192,   0x00000008,   864,    30,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x040004 },  
  { {0x89, 0x68, 0x04, 0x46, 0xff, 0xff, 0xff, 0xff }, 1,     8,     256,     4096,   0x00000208,   864,    30,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x040005 },  
  { {0x89, 0x88, 0x24, 0x4B, 0xff, 0xff, 0xff, 0xff }, 1,    16,     256,     4096,   0x1f000208,   864,    40,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x040006 },  
  { {0x89, 0xA8, 0x25, 0xCB, 0xff, 0xff, 0xff, 0xff }, 2,    16,     256,     4096,   0x00000208,   864,    30,     2,     0,          0,   &PhysicArchiPara4, &DefDDRInfo,  0x040007 },  
  { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 0,     0,       0,        0,   0x00000000,     0,     0,      0,     0,         0,          0,              0,        0xffffff },  
};


struct __NandPhyInfoPar_t StNandTbl[] =
{
  { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 0,     0,       0,        0,   0x00000000,     0,     0,    0,       0,         0,           0,            0,         0xffffff },  
};

struct __NandPhyInfoPar_t SpansionNandTbl[] =
{
  { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 0,     0,       0,        0,   0x00000000,     0,     0,     0,      0,         0,          0,              0,        0xffffff },  
};

struct __NandPhyInfoPar_t PowerNandTbl[] =
{
  { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 0,     0,       0,        0,   0x00000000,     0,     0,     0,      0,         0,          0,                0,      0xffffff },  
};


struct __NandPhyInfoPar_t SandiskNandTbl[] =
{
  { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 0,   0,     0,      0, 0x00000000,   0,     0,   0,    0,         0,      0,                 0,       0xffffff },  
};


struct __NandPhyInfoPar_t DefaultNandTbl[] =
{
  { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 0,     0,       0,        0,   0x00000000,     0,     0,     0,           0,    0,   &DefualtPhysicArchiPara, &DefDDRInfo, 0xffffff},
};

