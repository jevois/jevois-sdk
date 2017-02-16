#ifndef __IEP_DRC_EBIOS_H__
#define __IEP_DRC_EBIOS_H__

#include "../iep.h"

#define IEP_DRC_SPA_TAB_LEN 9
#define IEP_DRC_INT_TAB_LEN 256
#define IEP_DRC_LGC_TAB_LEN 256
#define IEP_LH_INTERVAL_NUM 8
#define IEP_LH_THRES_NUM    7

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 en        : 1;
    __u32 r0        : 7;
    __u32 mod       : 2;
    __u32 r1        : 6;
    __u32   sync_edge   : 1;
    __u32   field_parity  : 1;
    __u32 r2        : 13;
    __u32 bist_en     : 1;
  } bits;
} __imgehc_gnectl_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 disp_w      : 12;
    __u32 r0        : 4;
    __u32 disp_h      : 12;
    __u32 r1        : 4;
  } bits;
} __imgehc_drcsize_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 db_en     : 1;
    __u32 dbrdy_ctl   : 1;
    __u32 r0        : 6;
    __u32 win_en      : 1;
    __u32   hsv_mode_en     : 1;
    __u32 r1        : 22;
  } bits;
} __imgehc_drcctl_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 lgc_addr    : 32;
  } bits;
} __imgehc_drclgc_staadd_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 lgc_abslumshf : 1;
    __u32 adjust_en   : 1;
    __u32 r0        : 6;
    __u32 lgc_abslumperval: 8;
    __u32 r1        : 8;
    __u32 gain_autoload_dis: 1;
    __u32 r2        : 7;
  } bits;
} __imgehc_drc_set_reg_t;  

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 win_left    : 12;
    __u32 r0        : 4;
    __u32 win_top     : 12;
    __u32 r1        : 4;
  } bits;
} __imgehc_drc_wp_reg0_t;  

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 win_right   : 12;
    __u32 r0        : 4;
    __u32 win_bottom    : 12;
    __u32 r1        : 4;
  } bits;
} __imgehc_drc_wp_reg1_t;  

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 wb_en     : 1;
    __u32   r0        : 7;
    __u32 wb_mode     : 1;
    __u32   r1        : 7;
    __u32   wb_ps     : 1;
    __u32   r2        : 7;
    __u32   field           : 1;
    __u32   r3              : 6;
    __u32   wb_sts      : 1;
  } bits;
} __imgehc_wbctl_reg_t;  

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   wb_addr     : 32;
  } bits;
} __imgehc_wbaddr_reg_t; 

typedef union
{
  __u32 dwval;
  struct
  {
    __u32   linestride    : 32;
  } bits;
} __imgehc_wbline_reg_t; 

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 lh_rec_clr    : 1;
    __u32 lh_mod      : 1;
    __u32 r0        : 30;
  } bits;
} __imgehc_lhctl_reg_t;  

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 lh_thres_val1 : 8;
    __u32 lh_thres_val2 : 8;
    __u32 lh_thres_val3 : 8;
    __u32 lh_thres_val4 : 8;
  } bits;
} __imgehc_lhthr_reg0_t; 

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 lh_thres_val5 : 8;
    __u32 lh_thres_val6 : 8;
    __u32 lh_thres_val7 : 8;
    __u32 r0        : 8;
  } bits;
} __imgehc_lhthr_reg1_t; 

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 lh_lum_data   : 32;
  } bits;
} __imgehc_lhslum_reg_t; 

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 lh_cnt_data   : 32;
  } bits;
} __imgehc_lhscnt_reg_t; 

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_yg_coff   : 13;
    __u32 r0        : 19;
  } bits;
} __imgehc_cscygcoff_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_yg_con    : 14;
    __u32 r0        : 18;
  } bits;
} __imgehc_cscygcon_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_ur_coff   : 13;
    __u32 r0        : 19;
  } bits;
} __imgehc_cscurcoff_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_ur_con    : 14;
    __u32 r0        : 18;
  } bits;
} __imgehc_cscurcon_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_vb_coff   : 13;
    __u32 r0        : 19;
  } bits;
} __imgehc_cscvbcoff_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 csc_vb_con    : 14;
    __u32 r0        : 18;
  } bits;
} __imgehc_cscvbcon_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 spa_coff0   : 8;
    __u32 spa_coff1   : 8;
    __u32 spa_coff2   : 8;
    __u32 r0        : 8;
  } bits;
} __imgehc_drcspacoff_reg_t; 

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 inten_coff0   : 8;
    __u32 inten_coff1   : 8;
    __u32 inten_coff2   : 8;
    __u32 inten_coff3   : 8;
  } bits;
} __imgehc_drcintcoff_reg_t; 

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 lumagain_coff0  : 16;
    __u32 lumagain_coff1  : 16;
  } bits;
} __imgehc_drclgcoff_reg_t;  


typedef struct
{
  __imgehc_gnectl_reg_t     gnectl;
  __imgehc_drcsize_reg_t          drcsize;//0x04
  __u32             r0[2]; 
  __imgehc_drcctl_reg_t     drcctl;
  __imgehc_drclgc_staadd_reg_t  drclgc_addr;//0x14
  __imgehc_drc_set_reg_t      drc_set;//0x18
  __imgehc_drc_wp_reg0_t      drc_wp0;//0x1c
  __imgehc_drc_wp_reg1_t      drc_wp1;//0x20
  __imgehc_wbctl_reg_t      wbctl; 
  __imgehc_wbaddr_reg_t     wbaddr;
  __imgehc_wbline_reg_t     wbline;
  __imgehc_lhctl_reg_t      lhctl; 
  __imgehc_lhthr_reg0_t     lhthr0;
  __imgehc_lhthr_reg1_t     lhthr1;
  __u32             r2;  
  __imgehc_lhslum_reg_t     lhslum[8]; 
  __imgehc_lhscnt_reg_t     lhscnt[8]; 
  __u32             r3[16];
  __imgehc_cscygcoff_reg_t    cscygcoff[3];
  __imgehc_cscygcon_reg_t     cscygcon;
  __imgehc_cscurcoff_reg_t    cscurcoff[3];
  __imgehc_cscurcon_reg_t     cscurcon;
  __imgehc_cscvbcoff_reg_t    cscvbcoff[3];
  __imgehc_cscvbcon_reg_t     cscvbcon;
  __imgehc_drcspacoff_reg_t   drcspacoff[3]; 
  __u32             r4;    
  __imgehc_drcintcoff_reg_t   drcintcoff[64];//0x0100 ~ 0x01fc
  __imgehc_drclgcoff_reg_t    drclgcoff[128];//0x0200 ~ 0x03fc
} __iep_drc_dev_t;


#define ____SEPARATOR_GLOBAL____
__s32 DRC_EBIOS_Set_Reg_Base (__u32 sel, __u32 base);
__u32 DRC_EBIOS_Get_Reg_Base (__u32 sel);
__s32 DRC_EBIOS_Enable (__u32 sel);
__s32 DRC_EBIOS_Disable (__u32 sel);
__s32 DRC_EBIOS_Set_Mode (__u32 sel, __u32 mod);
__s32 DRC_EBIOS_Bist_Enable (__u32 sel, __u32 en);
__s32 DRC_EBIOS_Set_Reg_Refresh_Edge (__u32 sel, __u32 falling);
__s32 DRC_EBIOS_Set_Csc_Coeff (__u32 sel, __u32 mod);
__s32 DRC_EBIOS_Set_Display_Size (__u32 sel, __u32 width, __u32 height);
__s32 DRC_EBIOS_Win_Enable (__u32 sel, __u32 en);
__u32 DRC_EBIOS_Set_Win_Para (__u32 sel, __u32 top, __u32 bot, __u32 left, __u32 right);
#define ____SEPARATOR_DRC____
__s32 DRC_EBIOS_Drc_Cfg_Rdy (__u32 sel);
__u32 DRC_EBIOS_Drc_Set_Lgc_Addr (__u32 sel, __u32 addr);
__u32 DRC_EBIOS_Drc_Set_Lgc_Autoload_Disable (__u32 sel, __u32 disable);
__u32 DRC_EBIOS_Drc_Adjust_Enable (__u32 sel, __u32 en);
__u32 DRC_EBIOS_Drc_Set_Adjust_Para (__u32 sel, __u32 abslumperval, __u32 abslumshf);
__u32 DRC_EBIOS_Drc_Set_Spa_Coeff (__u32 sel, __u8 spatab[IEP_DRC_SPA_TAB_LEN]);
__u32 DRC_EBIOS_Drc_Set_Int_Coeff (__u32 sel,  __u8 inttab[IEP_DRC_INT_TAB_LEN]);
__u32 DRC_EBIOS_Drc_Set_Mode (__u32 sel, __u32 mode);
#define ____SEPARATOR_LH____
__u32 DRC_EBIOS_Lh_Set_Mode (__u32 sel, __u32 mod);
__u32 DRC_EBIOS_Lh_Clr_Rec (__u32 sel);
__u32 DRC_EBIOS_Lh_Set_Thres (__u32 sel, __u8 thres[IEP_LH_THRES_NUM]);
__u32 DRC_EBIOS_Lh_Get_Sum_Rec (__u32 sel, __u32 * sum);
__u32 DRC_EBIOS_Lh_Get_Cnt_Rec (__u32 sel, __u32 * cnt);
#endif

