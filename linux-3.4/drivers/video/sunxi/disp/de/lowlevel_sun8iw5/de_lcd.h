#ifndef __de_lcd_h__
#define __de_lcd_h__

#include "ebios_lcdc_tve.h"


typedef union
{
  __u32 dwval;
  struct
  {
    __u32 io_map_sel                 :  1 ;   
    __u32 res0                       : 29 ;   
    __u32 tcon_gamma_en              :  1 ;   
    __u32 tcon_en                    :  1 ;   
  } bits;
} tcon_gctl_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 tcon_irq_flag        : 16 ;   
    __u32 tcon_irq_en              : 16 ;   
  } bits;
} tcon_gint0_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 tcon1_line_int_num         : 12 ;   
    __u32 res0                       :  4 ;   
    __u32 tcon0_line_int_num         : 12 ;   
    __u32 res1                       :  4 ;   
  } bits;
} tcon_gint1_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 tcon0_frm_test             :  2 ;   
    __u32 res0                       :  2 ;   
    __u32 tcon0_frm_mode_b           :  1 ;   
    __u32 tcon0_frm_mode_g           :  1 ;   
    __u32 tcon0_frm_mode_r           :  1 ;   
    __u32 res1                       : 24 ;   
    __u32 tcon0_frm_en               :  1 ;   
  } bits;
} tcon0_frm_ctl_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 seed_value                 : 13 ;   
    __u32 res0                       : 19 ;   
  } bits;
} tcon0_frm_seed_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 frm_table_value                 ;   
  } bits;
} tcon0_frm_tab_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 src_sel                  :  3 ;   
    __u32 res0                       :  1 ;   
    __u32 start_delay              :  5 ;   
    __u32 res1                       : 11 ;   
    __u32 interlace_en               :  1 ;   
    __u32 fifo1_rst            :  1 ;   
    __u32 test_value                 :  1 ;   
    __u32 rb_swap                  :  1 ;   
    __u32 tcon0_if                   :  2 ;   
    __u32 res2                       :  2 ;   
    __u32 tcon0_work_mode            :  1 ;   
    __u32 res3                       :  2 ;   
    __u32 tcon0_en                   :  1 ;   
  } bits;
} tcon0_ctl_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 tcon0_dclk_div             :  7 ;   
    __u32 res0                       : 21 ;   
    __u32 tcon0_dclk_en              :  4 ;   
  } bits;
} tcon0_dclk_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 y                        : 12 ;   
    __u32 res0                       :  4 ;   
    __u32 x                        : 12 ;   
    __u32 res1                       :  4 ;   
  } bits;
} tcon0_basic0_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 hbp                        : 12 ;   
    __u32 res0                       :  4 ;   
    __u32 ht                         : 13 ;   
    __u32 res1                       :  2 ;   
    __u32 reservd                    :  1 ;   
  } bits;
} tcon0_basic1_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 vbp                        : 12 ;   
    __u32 res0                       :  4 ;   
    __u32 vt                         : 13 ;   
    __u32 res1                       :  3 ;   
  } bits;
} tcon0_basic2_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 vspw                       : 10 ;   
    __u32 res0                       :  6 ;   
    __u32 hspw                       : 10 ;   
    __u32 res1                       :  6 ;   
  } bits;
} tcon0_basic3_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 res0                       : 20 ;   
    __u32 syuv_fdly            :  2 ;   
    __u32 syuv_seq                   :  2 ;   
    __u32 srgb_seq                   :  4 ;   
    __u32 hv_mode                    :  4 ;   
  } bits;
} tcon0_hv_if_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 trigger_en                 :  1 ;   
    __u32 trigger_start              :  1 ;   
    __u32 trigger_fifo_en            :  1 ;   
    __u32 trigger_fifo_bist_en       :  1 ;   
    __u32 trigger_sync_mode          :  2 ;   
    __u32 res0                       : 10 ;   
    __u32 flush                      :  1 ;   
    __u32 auto_                       :  1 ;   
    __u32 res1                       :  4 ;   
    __u32 rd_flag                    :  1 ;   
    __u32 wr_flag                    :  1 ;   
    __u32 vsync_cs_sel               :  1 ;   
    __u32 ca                         :  1 ;   
    __u32 da                         :  1 ;   
    __u32 res2                       :  1 ;   
    __u32 cpu_mode                    :  4 ;   
  } bits;
} tcon0_cpu_if_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 data_wr                    : 24 ;   
    __u32 res0                       :  8 ;   
  } bits;
} tcon0_cpu_wr_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 data_rd0                   : 24 ;   
    __u32 res0                       :  8 ;   
  } bits;
} tcon0_cpu_rd0_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 data_rd1                   : 24 ;   
    __u32 res0                       :  8 ;   
  } bits;
} tcon0_cpu_rd1_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 tcon0_lvds_data_revert     :  4 ;   
    __u32 tcon0_lvds_clk_revert      :  1 ;   
    __u32 res0                       : 15 ;   
    __u32 tcon0_lvds_clk_sel         :  1 ;   
    __u32 res1                       :  2 ;   
    __u32 tcon0_lvds_correct_mode    :  1 ;   
    __u32 tcon0_lvds_debug_mode      :  1 ;   
    __u32 tcon0_lvds_debug_en        :  1 ;   
    __u32 tcon0_lvds_bitwidth        :  1 ;   
    __u32 tcon0_lvds_mode            :  1 ;   
    __u32 tcon0_lvds_dir             :  1 ;   
    __u32 tcon0_lvds_even_odd_dir    :  1 ;   
    __u32 tcon0_lvds_link            :  1 ;   
    __u32 tcon0_lvds_en              :  1 ;   
  } bits;
} tcon0_lvds_if_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 data_inv                   : 24 ;   
    __u32 sync_inv                   :  2 ;   
    __u32 clk_inv                    :  1 ;   
    __u32 de_inv                     :  1 ;   
    __u32 dclk_sel                   :  3 ;   
    __u32 io_output_sel              :  1 ;   
  } bits;
} tcon0_io_pol_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 data_output_tri_en         : 24 ;   
    __u32 io0_output_tri_en          :  1 ;   
    __u32 io1_output_tri_en          :  1 ;   
    __u32 io2_output_tri_en          :  1 ;   
    __u32 io3_output_tri_en          :  1 ;   
    __u32 rgb_endian                 :  1 ;   
    __u32 res0                       :  3 ;   
  } bits;
} tcon0_io_tri_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 src_sel                  :  2 ;   
    __u32 res0                       :  2 ;   
    __u32 start_delay                :  5 ;   
    __u32 res1                       : 11 ;   
    __u32 interlace_en               :  1 ;   
    __u32 res2                       : 10 ;   
    __u32 tcon1_en                   :  1 ;   
  } bits;
} tcon1_ctl_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 y                        : 12 ;   
    __u32 res0                       :  4 ;   
    __u32 x                          : 12 ;   
    __u32 res1                       :  4 ;   
  } bits;
} tcon1_basic0_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 ls_yo                      : 12 ;   
    __u32 res0                       :  4 ;   
    __u32 ls_xo                      : 12 ;   
    __u32 res1                       :  4 ;   
  } bits;
} tcon1_basic1_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 yo                       : 12 ;   
    __u32 res0                       :  4 ;   
    __u32 xo                       : 12 ;   
    __u32 res1                       :  4 ;   
  } bits;
} tcon1_basic2_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 hbp                        : 12 ;   
    __u32 res0                       :  4 ;   
    __u32 ht                         : 13 ;   
    __u32 res1                       :  3 ;   
  } bits;
} tcon1_basic3_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 vbp                        : 12 ;   
    __u32 res0                       :  4 ;   
    __u32 vt                         : 13 ;   
    __u32 res1                       :  3 ;   
  } bits;
} tcon1_basic4_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 vspw                       : 10 ;   
    __u32 res0                       :  6 ;   
    __u32 hspw                       : 10 ;   
    __u32 res1                       :  6 ;   
  } bits;
} tcon1_basic5_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 sync_y                     : 16 ;   
    __u32 sync_x                     : 16 ;   
  } bits;
} tcon1_ps_sync_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 data_inv                   : 24 ;   
    __u32 io0_inv                    :  1 ;   
    __u32 io1_inv                    :  1 ;   
    __u32 io2_inv                    :  1 ;   
    __u32 io3_inv                    :  1 ;   
    __u32 res0                       :  4 ;   
  } bits;
} tcon1_io_pol_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 data_output_tri_en         : 24 ;   
    __u32 io0_output_tri_en          :  1 ;   
    __u32 io1_output_tri_en          :  1 ;   
    __u32 io2_output_tri_en          :  1 ;   
    __u32 io3_output_tri_en          :  1 ;   
    __u32 res0                       :  4 ;   
  } bits;
} tcon1_io_tri_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 ecc_fifo_setting           :  8 ;   
    __u32 ecc_fifo_blank_en          :  1 ;   
    __u32 res0                       :  7 ;   
    __u32 ecc_fifo_err_bits          :  8 ;   
    __u32 res1                       :  6 ;   
    __u32 ecc_fifo_err_flag          :  1 ;   
    __u32 ecc_fifo_bist_en           :  1 ;   
  } bits;
} tcon_ecc_fifo_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 tcon1_current_line         : 12 ;   
    __u32 res0                       :  1 ;   
    __u32 ecc_fifo_bypass            :  1 ;   
    __u32 res1                       :  2 ;   
    __u32 tcon0_current_line         : 12 ;   
    __u32 tcon1_field_polarity       :  1 ;   
    __u32 tcon0_field_polarity       :  1 ;   
    __u32 tcon1_fifo_under_flow      :  1 ;   
    __u32 tcon0_fifo_under_flow      :  1 ;   
  } bits;
} tcon_debug_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 res0                       : 31 ;   
    __u32 ceu_en                     :  1 ;   
  } bits;
} tcon_ceu_ctl_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 value                : 13 ;   
    __u32 res0                       : 19 ;   
  } bits;
} tcon_ceu_coef_mul_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 value         : 19 ;   
    __u32 res0                       : 13 ;   
  } bits;
} tcon_ceu_coef_add_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 max         :  8 ;   
    __u32 res0                       :  8 ;   
    __u32 min         :  8 ;   
    __u32 res1                       :  8 ;   
  } bits;
} tcon_ceu_coef_rang_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 block_size                 : 12 ;   
    __u32 res0                       :  4 ;   
    __u32 block_space                : 12 ;   
    __u32 res1                       :  4 ;   
  } bits;
} tcon0_cpu_tri0_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 block_num                  : 16 ;   
    __u32 block_current_num          : 16 ;   
  } bits;
} tcon0_cpu_tri1_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 trans_start_set            : 13 ;   
    __u32 sync_mode                  :  2 ;   
    __u32 trans_start_mode           :  1 ;   
    __u32 start_delay                : 16 ;   
  } bits;
} tcon0_cpu_tri2_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 counter_m                  :  8 ;   
    __u32 counter_n                  : 16 ;   
    __u32 res0                       :  4 ;   
    __u32 tri_int_mode               :  2 ;   
    __u32 res1                       :  2 ;   
  } bits;
} tcon0_cpu_tri3_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 out_format                 :  1 ;   
    __u32 res0                       : 30 ;   
    __u32 cmap_en               :  1 ;   
  } bits;
} tcon_cmap_ctl_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 out0                   : 16 ;   
    __u32 out1                   : 16 ;   
  } bits;
} tcon_cmap_odd0_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 out2                   : 16 ;   
    __u32 out3                   : 16 ;   
  } bits;
} tcon_cmap_odd1_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 out0                  : 16 ;   
    __u32 out1                  : 16 ;   
  } bits;
} tcon_cmap_even0_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 out2                  : 16 ;   
    __u32 out3                  : 16 ;   
  } bits;
} tcon_cmap_even1_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 safe_period_mode           :  2 ;   
    __u32 res0                       : 14 ;   
    __u32 safe_period_fifo_num       : 13 ;   
    __u32 res1                       :  3 ;   
  } bits;
} tcon_safe_period_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 dsi_src        :  2 ;   
    __u32 res0                       :  6 ;   
    __u32 hdmi_src            :  2 ;   
    __u32 res1                       : 22 ;   
  } bits;
} tcon_mux_ctl_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 pwsmb                  :  1 ;   
    __u32 pwslv                  :  1 ;   
    __u32 res0                       :  2 ;   
    __u32 pd                     :  2 ;   
    __u32 res1                       :  2 ;   
    __u32 v                      :  2 ;   
    __u32 res2                       :  2 ;   
    __u32 den                    :  4 ;   
    __u32 denc                   :  1 ;   
    __u32 c                      :  2 ;   
    __u32 res3                       :  1 ;   
    __u32 en_drvd                  :  4 ;   
    __u32 en_drvc                  :  1 ;   
    __u32 res4                       :  5 ;   
    __u32 en_ldo                   :  1 ;   
    __u32 en_mb                    :  1 ;   
  } bits;
} tcon0_lvds_ana_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 res0                       : 31 ;   
    __u32 tcon1_fill_en              :  1 ;   
  } bits;
} tcon1_fill_ctl_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 fill_begin                 : 24 ;   
    __u32 res0                       :  8 ;   
  } bits;
} tcon1_fill_begin_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 fill_end                   : 24 ;   
    __u32 res0                       :  8 ;   
  } bits;
} tcon1_fill_end_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 fill_value                 : 24 ;   
    __u32 res0                       :  8 ;   
  } bits;
} tcon1_fill_data_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 pixel            : 24 ;   
    __u32 res0                       :  8 ;   
  } bits;
  struct
  {
    __u32 blue             :  8 ;   
    __u32 green            :  8 ;   
    __u32 red            :  8 ;   
    __u32 res0                       :  8 ;   
  } bytes;
} tcon_gamma_tlb_reg_t;

typedef union
{
  __u32 dwval;
  struct
  {
    __u32 res0                          ;   
  } bits;
} tcon_reservd_reg_t;

typedef struct
{
  tcon_gctl_reg_t       tcon_gctl;       
  tcon_gint0_reg_t      tcon_gint0;      
  tcon_gint1_reg_t      tcon_gint1;      
  tcon_reservd_reg_t      tcon_reg00c;     
  tcon0_frm_ctl_reg_t     tcon0_frm_ctl;     
  tcon0_frm_seed_reg_t    tcon0_frm_seed_pr;   
  tcon0_frm_seed_reg_t    tcon0_frm_seed_pg;   
  tcon0_frm_seed_reg_t    tcon0_frm_seed_pb;   
  tcon0_frm_seed_reg_t    tcon0_frm_seed_lr;   
  tcon0_frm_seed_reg_t    tcon0_frm_seed_lg;   
  tcon0_frm_seed_reg_t    tcon0_frm_seed_lb;   
  tcon0_frm_tab_reg_t     tcon0_frm_tbl_0;   
  tcon0_frm_tab_reg_t     tcon0_frm_tbl_1;   
  tcon0_frm_tab_reg_t     tcon0_frm_tbl_2;   
  tcon0_frm_tab_reg_t     tcon0_frm_tbl_3;   
  tcon_reservd_reg_t      tcon_reg03c;     
  tcon0_ctl_reg_t       tcon0_ctl;       
  tcon0_dclk_reg_t      tcon0_dclk;      
  tcon0_basic0_reg_t      tcon0_basic0;    
  tcon0_basic1_reg_t      tcon0_basic1;    
  tcon0_basic2_reg_t      tcon0_basic2;    
  tcon0_basic3_reg_t      tcon0_basic3;    
  tcon0_hv_if_reg_t     tcon0_hv_ctl;    
  tcon_reservd_reg_t      tcon_reg05c;     
  tcon0_cpu_if_reg_t      tcon0_cpu_ctl;     
  tcon0_cpu_wr_reg_t      tcon0_cpu_wr;    
  tcon0_cpu_rd0_reg_t     tcon0_cpu_rd;    
  tcon0_cpu_rd1_reg_t     tcon0_cpu_fet;     
  tcon_reservd_reg_t      tcon_reg070[5];    
  tcon0_lvds_if_reg_t     tcon0_lvds_ctl;    
  tcon0_io_pol_reg_t      tcon0_io_pol;    
  tcon0_io_tri_reg_t      tcon0_io_tri;    
  tcon1_ctl_reg_t       tcon1_ctl;       
  tcon1_basic0_reg_t      tcon1_basic0;    
  tcon1_basic1_reg_t      tcon1_basic1;    
  tcon1_basic2_reg_t      tcon1_basic2;    
  tcon1_basic3_reg_t      tcon1_basic3;    
  tcon1_basic4_reg_t      tcon1_basic4;    
  tcon1_basic5_reg_t      tcon1_basic5;    
  tcon_reservd_reg_t      tcon_reg0ac;     
  tcon1_ps_sync_reg_t     tcon1_ps_ctl;    
  tcon_reservd_reg_t      tcon_reg0b4[15];   
  tcon1_io_pol_reg_t      tcon1_io_pol;    
  tcon1_io_tri_reg_t      tcon1_io_tri;    
  tcon_ecc_fifo_reg_t     tcon_ecfifo_ctl;   
  tcon_debug_reg_t      tcon_debug;      
  tcon_ceu_ctl_reg_t      tcon_ceu_ctl;    
  tcon_reservd_reg_t      tcon_reg104[3];    
  tcon_ceu_coef_mul_reg_t   tcon_ceu_coef_rr;  
  tcon_ceu_coef_mul_reg_t   tcon_ceu_coef_rg;  
  tcon_ceu_coef_mul_reg_t   tcon_ceu_coef_rb;  
  tcon_ceu_coef_add_reg_t   tcon_ceu_coef_rc;  
  tcon_ceu_coef_mul_reg_t   tcon_ceu_coef_gr;  
  tcon_ceu_coef_mul_reg_t   tcon_ceu_coef_gg;  
  tcon_ceu_coef_mul_reg_t   tcon_ceu_coef_gb;  
  tcon_ceu_coef_add_reg_t   tcon_ceu_coef_gc;  
  tcon_ceu_coef_mul_reg_t   tcon_ceu_coef_br;  
  tcon_ceu_coef_mul_reg_t   tcon_ceu_coef_bg;  
  tcon_ceu_coef_mul_reg_t   tcon_ceu_coef_bb;  
  tcon_ceu_coef_add_reg_t   tcon_ceu_coef_bc;  
  tcon_ceu_coef_rang_reg_t  tcon_ceu_coef_rv;  
  tcon_ceu_coef_rang_reg_t  tcon_ceu_coef_gv;  
  tcon_ceu_coef_rang_reg_t  tcon_ceu_coef_bv;  
  tcon_reservd_reg_t      tcon_reg14c[5];    
  tcon0_cpu_tri0_reg_t    tcon0_cpu_tri0;    
  tcon0_cpu_tri1_reg_t    tcon0_cpu_tri1;    
  tcon0_cpu_tri2_reg_t    tcon0_cpu_tri2;    
  tcon0_cpu_tri3_reg_t    tcon0_cpu_tri3;    
  tcon_reservd_reg_t      tcon_reg170[4];    
  tcon_cmap_ctl_reg_t     tcon_cmap_ctl;     
  tcon_reservd_reg_t      tcon_reg184[3];    
  tcon_cmap_odd0_reg_t    tcon_cmap_odd0;    
  tcon_cmap_odd1_reg_t    tcon_cmap_odd1;    
  tcon_cmap_even0_reg_t   tcon_cmap_even0;   
  tcon_cmap_even1_reg_t   tcon_cmap_even1;   
  tcon_reservd_reg_t      tcon_reg1a0[20];   
  tcon_safe_period_reg_t    tcon_volume_ctl;   
  tcon_reservd_reg_t      tcon_reg1f4[3];    
  tcon_mux_ctl_reg_t      tcon_mul_ctl;    
  tcon_reservd_reg_t      tcon_reg204[7];    
  tcon0_lvds_ana_reg_t    tcon0_lvds_ana[2];   
  tcon_reservd_reg_t      tcon_reg228[54];   
  tcon1_fill_ctl_reg_t    tcon_fill_ctl;     
  tcon1_fill_begin_reg_t    tcon_fill_start0;  
  tcon1_fill_end_reg_t    tcon_fill_end0;    
  tcon1_fill_data_reg_t   tcon_fill_data0;   
  tcon1_fill_begin_reg_t    tcon_fill_start1;  
  tcon1_fill_end_reg_t    tcon_fill_end1;    
  tcon1_fill_data_reg_t   tcon_fill_data1;   
  tcon1_fill_begin_reg_t    tcon_fill_start2;  
  tcon1_fill_end_reg_t    tcon_fill_end2;    
  tcon1_fill_data_reg_t   tcon_fill_data2;   
  tcon_reservd_reg_t      tcon_reg328[54];   
  tcon_gamma_tlb_reg_t    tcon_gamma_tlb[256]; 
} __de_lcd_dev_t;


__s32 tcon_cmap (__u32 sel, __u32 mode, unsigned int lcd_cmap_tbl[2][3][4]);
__s32 tcon_gamma (__u32 sel, __u32 mode, __u32 gamma_tbl[256]);
__s32 tcon_ceu (__u32 sel, __u32 mode, __s32 b, __s32 c, __s32 s, __s32 h);
__s32 tcon0_frm (__u32 sel, __u32 mode);

#endif

