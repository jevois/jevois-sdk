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
#ifndef __PCI_HAL_H__
#define __PCI_HAL_H__


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

struct rt_pci_capabilities_header {
  u8  capability_id;
  u8  next;
};

struct pci_priv {
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

#ifdef CONFIG_RTL8192C
void rtl8192ce_set_hal_ops (_adapter * padapter);
#define hal_set_hal_ops rtl8192ce_set_hal_ops
#endif
#ifdef CONFIG_RTL8192D
void rtl8192de_set_hal_ops (_adapter * padapter);
#define hal_set_hal_ops rtl8192de_set_hal_ops
#endif


#ifdef CONFIG_RTL8188E
void rtl8188ee_set_hal_ops (_adapter * padapter);
#define hal_set_hal_ops rtl8188ee_set_hal_ops
#endif

#endif

