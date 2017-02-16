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
#ifndef __DRV_TYPES_PCI_H__
#define __DRV_TYPES_PCI_H__


#ifdef PLATFORM_LINUX
#include <linux/pci.h>
#endif


#define INTEL_VENDOR_ID       0x8086
#define SIS_VENDOR_ID         0x1039
#define ATI_VENDOR_ID         0x1002
#define ATI_DEVICE_ID         0x7914
#define AMD_VENDOR_ID         0x1022

#define PCI_MAX_BRIDGE_NUMBER     255
#define PCI_MAX_DEVICES       32
#define PCI_MAX_FUNCTION        8

#define PCI_CONF_ADDRESS          0x0CF8  
#define PCI_CONF_DATA         0x0CFC  

#define PCI_CLASS_BRIDGE_DEV      0x06
#define PCI_SUBCLASS_BR_PCI_TO_PCI  0x04

#define   PCI_CAPABILITY_ID_PCI_EXPRESS 0x10

#define U1DONTCARE          0xFF
#define U2DONTCARE          0xFFFF
#define U4DONTCARE          0xFFFFFFFF

#define PCI_VENDER_ID_REALTEK   0x10ec

#define HAL_HW_PCI_8180_DEVICE_ID             0x8180
#define HAL_HW_PCI_8185_DEVICE_ID             0x8185 
#define HAL_HW_PCI_8188_DEVICE_ID             0x8188 
#define HAL_HW_PCI_8198_DEVICE_ID             0x8198 
#define HAL_HW_PCI_8190_DEVICE_ID             0x8190 
#define HAL_HW_PCI_8723E_DEVICE_ID    0x8723 
#define HAL_HW_PCI_8192_DEVICE_ID             0x8192 
#define HAL_HW_PCI_8192SE_DEVICE_ID   0x8192 
#define HAL_HW_PCI_8174_DEVICE_ID             0x8174 
#define HAL_HW_PCI_8173_DEVICE_ID             0x8173 
#define HAL_HW_PCI_8172_DEVICE_ID             0x8172 
#define HAL_HW_PCI_8171_DEVICE_ID             0x8171 
#define HAL_HW_PCI_0045_DEVICE_ID     0x0045 
#define HAL_HW_PCI_0046_DEVICE_ID     0x0046 
#define HAL_HW_PCI_0044_DEVICE_ID     0x0044 
#define HAL_HW_PCI_0047_DEVICE_ID     0x0047 
#define HAL_HW_PCI_700F_DEVICE_ID     0x700F
#define HAL_HW_PCI_701F_DEVICE_ID     0x701F
#define HAL_HW_PCI_DLINK_DEVICE_ID    0x3304
#define HAL_HW_PCI_8192CET_DEVICE_ID    0x8191 
#define HAL_HW_PCI_8192CE_DEVICE_ID   0x8178 
#define HAL_HW_PCI_8191CE_DEVICE_ID   0x8177 
#define HAL_HW_PCI_8188CE_DEVICE_ID   0x8176 
#define HAL_HW_PCI_8192CU_DEVICE_ID       0x8191 
#define HAL_HW_PCI_8192DE_DEVICE_ID   0x8193 
#define HAL_HW_PCI_002B_DEVICE_ID     0x002B 
#define HAL_HW_PCI_8188EE_DEVICE_ID   0x8179

#define HAL_MEMORY_MAPPED_IO_RANGE_8190PCI    0x1000    
#define HAL_HW_PCI_REVISION_ID_8190PCI      0x00
#define HAL_MEMORY_MAPPED_IO_RANGE_8192PCIE 0x4000 
#define HAL_HW_PCI_REVISION_ID_8192PCIE     0x01
#define HAL_MEMORY_MAPPED_IO_RANGE_8192SE   0x4000 
#define HAL_HW_PCI_REVISION_ID_8192SE     0x10
#define HAL_HW_PCI_REVISION_ID_8192CE     0x1
#define HAL_MEMORY_MAPPED_IO_RANGE_8192CE   0x4000 
#define HAL_HW_PCI_REVISION_ID_8192DE     0x0
#define HAL_MEMORY_MAPPED_IO_RANGE_8192DE   0x4000 

enum pci_bridge_vendor {
  PCI_BRIDGE_VENDOR_INTEL = 0x0,//0b'0000,0001
  PCI_BRIDGE_VENDOR_ATI,
  PCI_BRIDGE_VENDOR_AMD,
  PCI_BRIDGE_VENDOR_SIS ,//= 0x08,//0b'0000,1000
  PCI_BRIDGE_VENDOR_UNKNOWN,
  PCI_BRIDGE_VENDOR_MAX ,//= 0x80
} ;

typedef struct _PCI_COMMON_CONFIG {
  u16 VendorID;
  u16 DeviceID;
  u16 Command;
  u16 Status;
  u8  RevisionID;
  u8  ProgIf;
  u8  SubClass;
  u8  BaseClass;
  u8  CacheLineSize;
  u8  LatencyTimer;
  u8  HeaderType;
  u8  BIST;
  
  union {
    struct _PCI_HEADER_TYPE_0 {
      u32 BaseAddresses[6];
      u32 CIS;
      u16 SubVendorID;
      u16 SubSystemID;
      u32 ROMBaseAddress;
      u8  CapabilitiesPtr;
      u8  Reserved1[3];
      u32 Reserved2;
      
      u8  InterruptLine;
      u8  InterruptPin;
      u8  MinimumGrant;
      u8  MaximumLatency;
    } type0;
    #if 0
    struct _PCI_HEADER_TYPE_1 {
      ULONG BaseAddresses[PCI_TYPE1_ADDRESSES];
      UCHAR PrimaryBusNumber;
      UCHAR SecondaryBusNumber;
      UCHAR SubordinateBusNumber;
      UCHAR SecondaryLatencyTimer;
      UCHAR IOBase;
      UCHAR IOLimit;
      USHORT SecondaryStatus;
      USHORT MemoryBase;
      USHORT MemoryLimit;
      USHORT PrefetchableMemoryBase;
      USHORT PrefetchableMemoryLimit;
      ULONG PrefetchableMemoryBaseUpper32;
      ULONG PrefetchableMemoryLimitUpper32;
      USHORT IOBaseUpper;
      USHORT IOLimitUpper;
      ULONG Reserved2;
      ULONG ExpansionROMBase;
      UCHAR InterruptLine;
      UCHAR InterruptPin;
      USHORT BridgeControl;
    } type1;
    
    struct _PCI_HEADER_TYPE_2 {
      ULONG BaseAddress;
      UCHAR CapabilitiesPtr;
      UCHAR Reserved2;
      USHORT SecondaryStatus;
      UCHAR PrimaryBusNumber;
      UCHAR CardbusBusNumber;
      UCHAR SubordinateBusNumber;
      UCHAR CardbusLatencyTimer;
      ULONG MemoryBase0;
      ULONG MemoryLimit0;
      ULONG MemoryBase1;
      ULONG MemoryLimit1;
      USHORT IOBase0_LO;
      USHORT IOBase0_HI;
      USHORT IOLimit0_LO;
      USHORT IOLimit0_HI;
      USHORT IOBase1_LO;
      USHORT IOBase1_HI;
      USHORT IOLimit1_LO;
      USHORT IOLimit1_HI;
      UCHAR InterruptLine;
      UCHAR InterruptPin;
      USHORT BridgeControl;
      USHORT SubVendorID;
      USHORT SubSystemID;
      ULONG LegacyBaseAddress;
      UCHAR Reserved3[56];
      ULONG SystemControl;
      UCHAR MultiMediaControl;
      UCHAR GeneralStatus;
      UCHAR Reserved4[2];
      UCHAR GPIO0Control;
      UCHAR GPIO1Control;
      UCHAR GPIO2Control;
      UCHAR GPIO3Control;
      ULONG IRQMuxRouting;
      UCHAR RetryStatus;
      UCHAR CardControl;
      UCHAR DeviceControl;
      UCHAR Diagnostic;
    } type2;
    #endif
  } u;
  
  u8  DeviceSpecific[108];
} PCI_COMMON_CONFIG , *PPCI_COMMON_CONFIG;

typedef struct _RT_PCI_CAPABILITIES_HEADER {
  u8   CapabilityID;
  u8   Next;
} RT_PCI_CAPABILITIES_HEADER, *PRT_PCI_CAPABILITIES_HEADER;

struct pci_priv {
  BOOLEAN   pci_clk_req;
  
  u8  pciehdr_offset;
  u8  pcie_cap;
  u8  linkctrl_reg;
  
  u8  busnumber;
  u8  devnumber;
  u8  funcnumber;
  
  u8  pcibridge_busnum;
  u8  pcibridge_devnum;
  u8  pcibridge_funcnum;
  u8  pcibridge_vendor;
  u16 pcibridge_vendorid;
  u16 pcibridge_deviceid;
  u8  pcibridge_pciehdr_offset;
  u8  pcibridge_linkctrlreg;
  
  u8  amd_l1_patch;
};

typedef struct _RT_ISR_CONTENT
{
  union {
    u32     IntArray[2];
    u32     IntReg4Byte;
    u16     IntReg2Byte;
  };
} RT_ISR_CONTENT, *PRT_ISR_CONTENT;

static inline void NdisRawWritePortUlong (u32 port,  u32 val)
{
  outl (val, port);
}

static inline void NdisRawWritePortUchar (u32 port,  u8 val)
{
  outb (val, port);
}

static inline void NdisRawReadPortUchar (u32 port, u8 * pval)
{
  *pval = inb (port);
}

static inline void NdisRawReadPortUshort (u32 port, u16 * pval)
{
  *pval = inw (port);
}

static inline void NdisRawReadPortUlong (u32 port, u32 * pval)
{
  *pval = inl (port);
}


#endif

