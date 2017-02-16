#ifndef __IEP_DEU_EBIOS_H__
#define __IEP_DEU_EBIOS_H__

#include "ebios_de.h"

typedef union
{
  u32 dwval;
  struct
  {
    u32 en        : 1;
    u32 r0        : 3;
    u32 reg_rdy     : 1;
    u32   coeff_rdy   : 1;
    u32   r1        : 2;
    u32 csc_en      : 1;
    u32 r2        : 3;
    u32 out_port_sel  : 1;
    u32 r3        : 19;
    
  } bits;
} __imgehc_deu_en_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 width     : 12;
    u32 r0        : 4;
    u32 height      : 12;
    u32   r1        : 4;
  } bits;
} __imgehc_deu_psize_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 win_left    : 12;
    u32 r0        : 4;
    u32 win_top     : 12;
    u32   r1        : 4;
  } bits;
} __imgehc_deu_pwp0_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 win_right   : 12;
    u32 r0        : 4;
    u32 win_bot     : 12;
    u32   r1        : 4;
  } bits;
} __imgehc_deu_pwp1_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 dcti_en     : 1;
    u32 r0        : 5;
    u32 dcti_hill_en  : 1;
    u32 dcti_suphill_en : 1;
    u32 dcti_filter1_sel: 2;
    u32 dcti_filter2_sel: 2;
    u32 dcti_path_limit : 4;
    u32   dcti_gain   : 6;
    u32   r1        : 2;
    u32   uv_diff_sign_mode_sel: 2;
    u32   uv_same_sign_mode_sel: 2;
    u32   uv_diff_sign_maxmin_mode_sel: 1;
    u32   uv_same_sign_maxmin_mode_sel: 1;
    u32   r2              : 1;
    u32 uv_separate_en  : 1;
  } bits;
} __imgehc_deu_dcti_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 lp_en     : 1;
    u32 r0        : 7;
    u32 tau       : 5;
    u32   r1        : 3;
    u32 alpha     : 5;
    u32   r2              : 3;
    u32   beta            : 5;
    u32 r3        : 3;
  } bits;
} __imgehc_deu_lp0_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 lp_mode     : 1;
    u32   str       : 1;
    u32 r0        : 6;
    u32 corthr      : 8;
    u32 neggain     : 2;
    u32   r1              : 2;
    u32   delta           : 2;
    u32   r2              : 2;
    u32 limit_thr   : 8;
  } bits;
} __imgehc_deu_lp1_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 straddr     : 32;
  } bits;
} __imgehc_deu_lp_straddr_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 wle_en      : 1;
    u32 r0        : 7;
    u32 wle_thr     : 8;
    u32 wle_gain    : 8;
    u32   r1              : 8;
  } bits;
} __imgehc_deu_wle_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 ble_en      : 1;
    u32 r0        : 7;
    u32 ble_thr     : 8;
    u32 ble_gain    : 8;
    u32   r1              : 8;
  } bits;
} __imgehc_deu_ble_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 csc_g_coff    : 13;
    u32 r0        : 19;
  } bits;
} __imgehc_deu_cscgcoff_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 csc_g_con   : 14;
    u32 r0        : 18;
  } bits;
} __imgehc_deu_cscgcon_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 csc_r_coff    : 13;
    u32 r0        : 19;
  } bits;
} __imgehc_deu_cscrcoff_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 csc_r_con   : 14;
    u32 r0        : 18;
  } bits;
} __imgehc_deu_cscrcon_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 csc_b_coff    : 13;
    u32 r0        : 19;
  } bits;
} __imgehc_deu_cscbcoff_reg_t;

typedef union
{
  u32 dwval;
  struct
  {
    u32 csc_b_con   : 14;
    u32 r0        : 18;
  } bits;
} __imgehc_deu_cscbcon_reg_t;

typedef struct
{
  __imgehc_deu_en_reg_t     en;  
  __imgehc_deu_psize_reg_t    psize; 
  __imgehc_deu_pwp0_reg_t     pwp0;
  __imgehc_deu_pwp1_reg_t     pwp1;
  __imgehc_deu_dcti_reg_t     dcti;
  __imgehc_deu_lp0_reg_t      lp0; 
  __imgehc_deu_lp1_reg_t      lp1; 
  __imgehc_deu_lp_straddr_reg_t   straddr;//0x1c
  u32                         r0[2]; 
  __imgehc_deu_wle_reg_t      wle; 
  __imgehc_deu_ble_reg_t      ble; 
  __imgehc_deu_cscgcoff_reg_t   cscgcoff[3];//0x30~0x38
  __imgehc_deu_cscgcon_reg_t    cscgcon; 
  __imgehc_deu_cscrcoff_reg_t   cscrcoff[3];//0x40~0x48
  __imgehc_deu_cscrcon_reg_t    cscrcon; 
  __imgehc_deu_cscbcoff_reg_t   cscbcoff[3];//0x50~0x58
  __imgehc_deu_cscbcon_reg_t    cscbcon; 
} __iep_deu_dev_t;

#define ____SEPARATOR_GLOBAL____
s32 DEU_EBIOS_Set_Reg_Base (u32 sel, u32 base);
u32 DEU_EBIOS_Get_Reg_Base (u32 sel);
s32 DEU_EBIOS_Enable (u32 sel, u32 en);
s32 DEU_EBIOS_Csc_Enable (u32 sel, u32 en);
s32 DEU_EBIOS_Set_Csc_Coeff (u32 sel, u32 mode);
s32 DEU_EBIOS_Set_Display_Size (u32 sel, u32 width, u32 height);
s32 DEU_EBIOS_Set_Win_Para (u32 sel, u32 top, u32 bot, u32 left, u32 right);
s32 DEU_EBIOS_Cfg_Rdy (u32 sel);
s32 DEU_EBIOS_Set_Output_Chnl (u32 sel, u32 be_sel);
#define ____SEPARATOR_LP____
s32 DEU_EBIOS_LP_STR_Cfg_Rdy (u32 sel);
s32 DEU_EBIOS_LP_Set_Mode (u32 sel, u32 en_2d);
s32 DEU_EBIOS_LP_STR_Enable (u32 sel, u32 en);
s32 DEU_EBIOS_LP_Set_STR_Addr (u32 sel, u32 address);
s32 DEU_EBIOS_LP_Set_Para (u32 sel, u32 level, u32 filtertype, __u8 * pttab);
s32 DEU_EBIOS_LP_Enable (u32 sel, u32 en);
#define ____SEPARATOR_DCTI____
s32 DEU_EBIOS_DCTI_Set_Para (u32 sel, u32 level);
s32 DEU_EBIOS_DCTI_Enable (u32 sel, u32 en);
#define ____SEPARATOR_WLE_BLE____
s32 DEU_EBIOS_WLE_Set_Para (u32 sel, u32 level);
s32 DEU_EBIOS_WLE_Enable (u32 sel, u32 en);
s32 DEU_EBIOS_BLE_Set_Para (u32 sel, u32 level);
s32 DEU_EBIOS_BLE_Enable (u32 sel, u32 en);

extern __u8 deu_str_tab[512];
extern __u8 deu_lp_tab_l[5][5][5];
extern __u8 deu_lp_tab_s[5][5][5];
#endif
