#ifndef __IEP_WB_EBIOS_H__
#define __IEP_WB_EBIOS_H__

#include "iep.h"

#define ____SEPARATOR_DEFEINE____
#define WB_END_IE       0x1
#define WB_FIFO_EMPTY_ERROR_IE (0x1<<4)
#define WB_FIFO_OVF_ERROR_IE (0x1<<5)
#define WB_UNFINISH_ERROR_IE (0x1<<6)

#define ____SEPARATOR_REGISTERS____
typedef union
{
  __u32 dwval;
  struct
  {
    __u32 en        : 1;
    __u32 r0        : 3;
    __u32 reg_rdy_en    : 1;
    __u32 r1        : 3;
    __u32 wb_mode     : 1;
    __u32   r2              : 3;
    __u32   in_port_sel     : 1;
    __u32   r3              : 3;
    __u32   wb_en           : 1;
    __u32   r4              : 3;
    __u32   r5        : 1;
    __u32   r6              : 10;
    __u32   dma_rst_mode    : 1;
  } bits;
} __wbc_gctrl_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   width           : 11;
    __u32   r0        : 5;
    __u32   height          : 11;
    __u32   r1              : 5;
  } bits;
} __wbc_size_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   crop_left       : 11;
    __u32   r0        : 5;
    __u32   crop_top        : 11;
    __u32   r1              : 5;
  } bits;
} __wbc_crop_coord_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   crop_width      : 11;
    __u32   r0        : 5;
    __u32   crop_height     : 11;
    __u32   r1              : 5;
  } bits;
} __wbc_crop_size_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   addr      : 32;
  } bits;
} __wbc_ch0_addr_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   addr      : 32;
  } bits;
} __wbc_ch1_addr_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   lstrd     : 32;
  } bits;
} __wbc_ch0_lstrd_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   lstrd     : 32;
  } bits;
} __wbc_ch1_lstrd_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   factor      : 2;
    __u32   r0        : 30;
  } bits;
} __wbc_resizer_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   format      : 1;
    __u32   r0        : 3;
    __u32   ps              : 1;
    __u32   r1              : 27;
  } bits;
} __wbc_format_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 wb_end_int_en     : 1;
    __u32 r0            : 3;
    __u32 wb_fifo_empty_int_en  : 1;
    __u32 wb_fifo_ovf_int_en    : 1;
    __u32   wb_unfinish_int_en      : 1;
    __u32   r1            : 9;
    __u32   wb_end_int_timing       : 1;
    __u32   r2            : 15;
  } bits;
} __wbc_int_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 wb_end_flag       : 1;
    __u32 r0            : 3;
    __u32 wb_fifo_empty_err   : 1;
    __u32 wb_fifo_ovf_err     : 1;
    __u32   wb_unfinish_err     : 1;
    __u32   r1            : 1;
    __u32   wb_busy             : 1;
    __u32   r2            : 23;
  } bits;
} __wbc_status_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   burst_len   : 2;
    __u32   r0              : 30;
  } bits;
} __wbc_dma_reg_t; 

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_yr_coff   : 13;
    __u32 r0        : 19;
  } bits;
} __wbc_cscyrcoff_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_yr_con    : 14;
    __u32 r0        : 18;
  } bits;
} __wbc_cscyrcon_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_ug_coff   : 13;
    __u32 r0        : 19;
  } bits;
} __wbc_cscugcoff_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_ug_con    : 14;
    __u32 r0        : 18;
  } bits;
} __wbc_cscugcon_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_vb_coff   : 13;
    __u32 r0        : 19;
  } bits;
} __wbc_cscvbcoff_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_vb_con    : 14;
    __u32 r0        : 18;
  } bits;
} __wbc_cscvbcon_reg_t;

typedef struct
{
  __wbc_gctrl_reg_t     gctrl; 
  __wbc_size_reg_t      size;
  __wbc_crop_coord_reg_t    crop_coord;//0x08
  __wbc_crop_size_reg_t   crop_size; 
  __wbc_ch0_addr_reg_t    addr0; 
  __wbc_ch1_addr_reg_t    addr1; 
  __u32           r0[2]; 
  __wbc_ch0_lstrd_reg_t   lstrd0;
  __wbc_ch1_lstrd_reg_t   lstrd1;
  __u32                       r1[2]; 
  __wbc_resizer_reg_t     resizer;//0x30
  __wbc_format_reg_t      format;
  __wbc_int_reg_t       intp;
  __wbc_status_reg_t      status;
  __wbc_dma_reg_t       dma; 
  __u32           r2[3]; 
  __wbc_cscyrcoff_reg_t   cscyrcoef[3];//0x50~0x58
  __wbc_cscyrcon_reg_t    cscyrcon;//0x5c
  __wbc_cscugcoff_reg_t   cscugcoef[3];//0x60~0x68
  __wbc_cscugcon_reg_t    cscugcon;//0x6c
  __wbc_cscvbcoff_reg_t   cscvbcoef[3];//0x70~0x78
  __wbc_cscvbcon_reg_t    cscvbcon;//0x7c
} __iep_wb_dev_t;

#define ____SEPARATOR_FUNCTIONS____
__s32 WB_EBIOS_Set_Reg_Base (__u32 sel, __u32 base);
__u32 WB_EBIOS_Get_Reg_Base (__u32 sel);
__s32 WB_EBIOS_Enable (__u32 sel);
__s32 WB_EBIOS_Reset (__u32 sel);
__s32 WB_EBIOS_Set_Reg_Rdy (__u32 sel);
__s32 WB_EBIOS_Set_Capture_Mode (__u32 sel);
__s32 WB_EBIOS_Writeback_Enable (__u32 sel);
__s32 WB_EBIOS_Writeback_Disable (__u32 sel);
__s32 WB_EBIOS_Set_Para (__u32 sel,  disp_size insize, disp_window cropwin, disp_window outwin, disp_fb_info outfb);
__s32 WB_EBIOS_Set_Addr (__u32 sel, __u32 addr[3]);
__u32 WB_EBIOS_Get_Status (__u32 sel);
__s32 WB_EBIOS_EnableINT (__u32 sel);
__s32 WB_EBIOS_DisableINT (__u32 sel);
__u32 WB_EBIOS_QueryINT (__u32 sel);
__u32 WB_EBIOS_ClearINT (__u32 sel);
__s32 WB_EBIOS_Init (__u32 sel);

#endif