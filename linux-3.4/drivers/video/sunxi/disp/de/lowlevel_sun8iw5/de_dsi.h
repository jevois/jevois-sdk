#ifndef __de_dsi_h__
#define __de_dsi_h__

#include "ebios_lcdc_tve.h"

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 dsi_en                     :  1 ;   
		__u32 res0                       : 31 ;   
	} bits;
} dsi_ctl_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 dsi_irq_en  				 : 16 ;   
		__u32 dsi_irq_flag               : 16 ;   
	} bits;
} dsi_gint0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 video_line_int_num         : 13 ;   
		__u32 res0                       : 19 ;   
	} bits;
} dsi_gint1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 video_mode_burst           :  1 ;   
		__u32 hsa_hse_dis                :  1 ;   
		__u32 hbp_dis                    :  1 ;   
		__u32 trail_fill				 :  1 ;	  
		__u32 trail_inv  				 :  4 ;	  
		__u32 res0                       :  8 ;   
		__u32 brdy_set			         :  8 ;   
		__u32 brdy_l_sel		         :  3 ;   
		__u32 res1		                 :  5 ;   
	} bits;
} dsi_basic_ctl_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 inst_st                    :  1 ;   
		__u32 res0                       :  3 ;   
		__u32 src_sel                    :  2 ;   
		__u32 res1                       :  4 ;   
		__u32 fifo_manual_reset          :  1 ;   
		__u32 res2                       :  1 ;   
		__u32 fifo_gating                :  1 ;   
		__u32 res3                       :  3 ;   
		__u32 ecc_en                     :  1 ;   
		__u32 crc_en                     :  1 ;   
		__u32 hs_eotp_en                 :  1 ;   
		__u32 res4                       : 13 ;   
	} bits;
} dsi_basic_ctl0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 dsi_mode                   :  1 ;   
		__u32 video_frame_start          :  1 ;   
		__u32 video_precision_mode_align :  1 ;   
		__u32 res0                       :  1 ;   
		__u32 video_start_delay          : 13 ;   
		__u32 res1                       : 15 ;   
	} bits;
} dsi_basic_ctl1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 vsa                  : 12 ;   
		__u32 res0                       :  4 ;   
		__u32 vbp                  : 12 ;   
		__u32 res1                       :  4 ;   
	} bits;
} dsi_basic_size0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 vact                 : 12 ;   
		__u32 res0                       :  4 ;   
		__u32 vt                   : 13 ;   
		__u32 res1                       :  3 ;   
	} bits;
} dsi_basic_size1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 lane_den                   :  4 ;   
		__u32 lane_cen                   :  1 ;   
		__u32 res0                       : 11 ;   
		__u32 trans_start_condition      :  4 ;   
		__u32 trans_packet               :  4 ;   
		__u32 escape_enrty               :  4 ;   
		__u32 instru_mode                :  4 ;   
	} bits;
} dsi_basic_inst0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 inst0_sel                  :  4 ;   
		__u32 inst1_sel                  :  4 ;   
		__u32 inst2_sel                  :  4 ;   
		__u32 inst3_sel                  :  4 ;   
		__u32 inst4_sel                  :  4 ;   
		__u32 inst5_sel                  :  4 ;   
		__u32 inst6_sel                  :  4 ;   
		__u32 inst7_sel                  :  4 ;   
	} bits;
} dsi_basic_inst1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 loop_n0                    : 12 ;   
		__u32 res0                       :  4 ;   
		__u32 loop_n1                    : 12 ;   
		__u32 res1                       :  4 ;   
	} bits;
} dsi_basic_inst2_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 inst0_jump                 :  4 ;   
		__u32 inst1_jump                 :  4 ;   
		__u32 inst2_jump                 :  4 ;   
		__u32 inst3_jump                 :  4 ;   
		__u32 inst4_jump                 :  4 ;   
		__u32 inst5_jump                 :  4 ;   
		__u32 inst6_jump                 :  4 ;   
		__u32 inst7_jump                 :  4 ;   
	} bits;
} dsi_basic_inst3_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 jump_cfg_num              : 16 ;   
		__u32 jump_cfg_point            :  4 ;   
		__u32 jump_cfg_to               :  4 ;   
		__u32 res0                       :  4 ;   
		__u32 jump_cfg_en               :  1 ;   
		__u32 res1                       :  3 ;   
	} bits;
} dsi_basic_inst4_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 trans_start_set            : 13 ;   
		__u32 res0                       : 19 ;   
	} bits;
} dsi_basic_tran0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 trans_size                 : 16 ;   
		__u32 res0                       : 12 ;   
		__u32 trans_end_condition        :  1 ;   
		__u32 res1                       :  3 ;   
	} bits;
} dsi_basic_tran1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 trans_cycle_set            : 16 ;   
		__u32 res0                       : 16 ;   
	} bits;
} dsi_basic_tran2_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 trans_blank_set            : 16 ;   
		__u32 res0                       : 16 ;   
	} bits;
} dsi_basic_tran3_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 hs_zero_reduce_set         : 16 ;   
		__u32 res0                       : 16 ;   
	} bits;
} dsi_basic_tran4_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 drq_set			         : 10 ;   
		__u32 res0                       : 18 ;   
		__u32 drq_mode			         :  1 ;   
		__u32 res1                       :  3 ;   
	} bits;
} dsi_basic_tran5_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 pixel_format               :  4 ;   
		__u32 pixel_endian               :  1 ;   
		__u32 res0                       : 11 ;   
		__u32 pd_plug_dis                :  1 ;   
		__u32 res1                       : 15 ;   
	} bits;
} dsi_pixel_ctl0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 res0                            ;   
	} bits;
} dsi_pixel_ctl1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 dt                         :  6 ;   
		__u32 vc                         :  2 ;   
		__u32 wc                         : 16 ;   
		__u32 ecc                        :  8 ;   
	} bits;
} dsi_pixel_ph_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 pd_tran0                   :  8 ;   
		__u32 res0                       :  8 ;   
		__u32 pd_trann                   :  8 ;   
		__u32 res1                       :  8 ;   
	} bits;
} dsi_pixel_pd_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 crc_force                  : 16 ;   
		__u32 res0                       : 16 ;   
	} bits;
} dsi_pixel_pf0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 crc_init_line0             : 16 ;   
		__u32 crc_init_linen             : 16 ;   
	} bits;
} dsi_pixel_pf1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 dt                         :  6 ;   
		__u32 vc                         :  2 ;   
		__u32 d0                         :  8 ;   
		__u32 d1                         :  8 ;   
		__u32 ecc                        :  8 ;   
	} bits;
} dsi_short_pkg_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 dt                         :  6 ;   
		__u32 vc                         :  2 ;   
		__u32 wc                         :  16 ;   
		__u32 ecc                        :  8 ;   
	} bits;
} dsi_blk_pkg0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 pd                	     :  8 ;   
		__u32 res0                       :  8 ;   
		__u32 pf                   		 : 16 ;   
	} bits;
} dsi_blk_pkg1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 line_num            	     : 16 ;   
		__u32 line_syncpoint             : 16 ;   
	} bits;
} dsi_burst_line_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 drq_edge0            	     : 16 ;   
		__u32 drq_edge1		             : 16 ;   
	} bits;
} dsi_burst_drq_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 tx_size                    :  8 ;   
		__u32 tx_status                  :  1 ;   
		__u32 tx_flag                    :  1 ;   
		__u32 res0                       :  6 ;   
		__u32 rx_size                    :  5 ;   
		__u32 res1                       :  3 ;   
		__u32 rx_status                  :  1 ;   
		__u32 rx_flag                    :  1 ;   
		__u32 rx_overflow                :  1 ;   
		__u32 res2                       :  5 ;   
	} bits;
} dsi_cmd_ctl_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 byte0						 :  8 ;   
		__u32 byte1                      :  8 ;   
		__u32 byte2                      :  8 ;   
		__u32 byte3                      :  8 ;   
	} bits;
} dsi_cmd_data_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 video_curr_line            : 13 ;   
		__u32 res0                       : 19 ;   
	} bits;
} dsi_debug0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 video_curr_lp2hs           : 16 ;   
		__u32 res0                       : 16 ;   
	} bits;
} dsi_debug1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 trans_low_flag             :  1 ;   
		__u32 trans_fast_flag            :  1 ;   
		__u32 res0                       :  2 ;   
		__u32 curr_loop_num              : 16 ;   
		__u32 curr_instru_num            :  3 ;   
		__u32 res1                       :  1 ;   
		__u32 instru_unknow_flag         :  8 ;   
	} bits;
} dsi_debug2_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 res0                       : 16 ;   
		__u32 curr_fifo_num              : 16 ;   
	} bits;
} dsi_debug3_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 test_data                  : 24 ;   
		__u32 res0                       :  4 ;   
		__u32 dsi_fifo_bist_en           :  1 ;   
		__u32 res1                       :  3 ;   
	} bits;
} dsi_debug4_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 res0                       	  ;   
	} bits;
}dsi_reservd_reg_t;

typedef struct
{
	dsi_ctl_reg_t				dsi_gctl;				
	dsi_gint0_reg_t				dsi_gint0;				
	dsi_gint1_reg_t				dsi_gint1;				
	dsi_basic_ctl_reg_t			dsi_basic_ctl;			
	dsi_basic_ctl0_reg_t		dsi_basic_ctl0;			
	dsi_basic_ctl1_reg_t		dsi_basic_ctl1;			
	dsi_basic_size0_reg_t		dsi_basic_size0;		
	dsi_basic_size1_reg_t		dsi_basic_size1;		
	dsi_basic_inst0_reg_t		dsi_inst_func[8];		
	dsi_basic_inst1_reg_t		dsi_inst_loop_sel;		
	dsi_basic_inst2_reg_t		dsi_inst_loop_num;		
	dsi_basic_inst3_reg_t		dsi_inst_jump_sel;		
	dsi_basic_inst4_reg_t		dsi_inst_jump_cfg[2];	
	dsi_basic_inst2_reg_t		dsi_inst_loop_num2;		
	dsi_reservd_reg_t			dsi_reg058[2];			
	dsi_basic_tran0_reg_t		dsi_trans_start;		
	dsi_reservd_reg_t			dsi_reg064[5];			
	dsi_basic_tran4_reg_t		dsi_trans_zero;			
	dsi_basic_tran5_reg_t		dsi_tcon_drq;			
	dsi_pixel_ctl0_reg_t		dsi_pixel_ctl0;			
	dsi_pixel_ctl1_reg_t		dsi_pixel_ctl1;			
	dsi_reservd_reg_t			dsi_reg088[2];			
	dsi_pixel_ph_reg_t			dsi_pixel_ph;			
	dsi_pixel_pd_reg_t			dsi_pixel_pd;			
	dsi_pixel_pf0_reg_t			dsi_pixel_pf0;			
	dsi_pixel_pf1_reg_t			dsi_pixel_pf1;			
	dsi_reservd_reg_t			dsi_reg0a0[4];			
	dsi_short_pkg_reg_t			dsi_sync_hss;			
	dsi_short_pkg_reg_t			dsi_sync_hse;			
	dsi_short_pkg_reg_t			dsi_sync_vss;			
	dsi_short_pkg_reg_t			dsi_sync_vse;			
	dsi_blk_pkg0_reg_t			dsi_blk_hsa0;			
	dsi_blk_pkg1_reg_t			dsi_blk_hsa1;			
	dsi_blk_pkg0_reg_t			dsi_blk_hbp0;			
	dsi_blk_pkg1_reg_t			dsi_blk_hbp1;			
	dsi_blk_pkg0_reg_t			dsi_blk_hfp0;			
	dsi_blk_pkg1_reg_t			dsi_blk_hfp1;			
	dsi_reservd_reg_t			dsi_reg0d8[2];			
	dsi_blk_pkg0_reg_t			dsi_blk_hblk0;			
	dsi_blk_pkg1_reg_t			dsi_blk_hblk1;			
	dsi_blk_pkg0_reg_t			dsi_blk_vblk0;			
	dsi_blk_pkg1_reg_t			dsi_blk_vblk1;			
	dsi_burst_line_reg_t		dsi_burst_line;			
	dsi_burst_drq_reg_t			dsi_burst_drq;			
	dsi_reservd_reg_t			dsi_reg0f0[66];			
	dsi_cmd_ctl_reg_t			dsi_cmd_ctl;			
	dsi_reservd_reg_t			dsi_reg204[15];			
	dsi_cmd_data_reg_t			dsi_cmd_rx[8];			
	dsi_reservd_reg_t			dsi_reg260[32];			
	dsi_debug0_reg_t			dsi_debug_video0;		
	dsi_debug1_reg_t			dsi_debug_video1;		
	dsi_reservd_reg_t			dsi_reg2e8[2];			
	dsi_debug2_reg_t			dsi_debug_inst;			
	dsi_debug3_reg_t			dsi_debug_fifo;			
	dsi_debug4_reg_t			dsi_debug_data;			
	dsi_reservd_reg_t			dsi_reg2fc;				
	dsi_cmd_data_reg_t			dsi_cmd_tx[64];			
}__de_dsi_dev_t;




typedef union
{
	__u32 dwval;
	struct
	{
		__u32 module_en           :  1 ;   
		__u32 res0                :  3 ;   
		__u32 lane_num            :  2 ;   
		__u32 res1                : 26 ;   
	} bits;
} dphy_ctl_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 tx_d0_force         :  1 ;   
		__u32 tx_d1_force         :  1 ;   
		__u32 tx_d2_force         :  1 ;   
		__u32 tx_d3_force         :  1 ;   
		__u32 tx_clk_force        :  1 ;   
		__u32 res0                :  3 ;   
		__u32 lptx_endian         :  1 ;   
		__u32 hstx_endian         :  1 ;   
		__u32 lptx_8b9b_en        :  1 ;   
		__u32 hstx_8b9b_en        :  1 ;   
		__u32 force_lp11          :  1 ;   
		__u32 res1                :  3 ;   
		__u32 ulpstx_data0_exit   :  1 ;   
		__u32 ulpstx_data1_exit   :  1 ;   
		__u32 ulpstx_data2_exit   :  1 ;   
		__u32 ulpstx_data3_exit   :  1 ;   
		__u32 ulpstx_clk_exit     :  1 ;   
		__u32 res2                :  3 ;   
		__u32 hstx_data_exit      :  1 ;   
		__u32 hstx_clk_exit       :  1 ;   
		__u32 res3                :  2 ;   
		__u32 hstx_clk_cont       :  1 ;   
		__u32 ulpstx_enter        :  1 ;   
		__u32 res4                :  2 ;   
	} bits;
} dphy_tx_ctl_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 res0                :  8 ;   
		__u32 lprx_endian         :  1 ;   
		__u32 hsrx_endian         :  1 ;   
		__u32 lprx_8b9b_en        :  1 ;   
		__u32 hsrx_8b9b_en        :  1 ;   
		__u32 hsrx_sync           :  1 ;   
		__u32 res1                :  3 ;   
		__u32 lprx_trnd_mask      :  4 ;   
		__u32 rx_d0_force         :  1 ;   
		__u32 rx_d1_force         :  1 ;   
		__u32 rx_d2_force         :  1 ;   
		__u32 rx_d3_force         :  1 ;   
		__u32 rx_clk_force        :  1 ;   
		__u32 res2                :  6 ;   
		__u32 dbc_en              :  1 ;   
	} bits;
} dphy_rx_ctl_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 lpx_tm_set          :  8 ;   
		__u32 dterm_set           :  8 ;   
		__u32 hs_pre_set          :  8 ;   
		__u32 hs_trail_set        :  8 ;   
	} bits;
} dphy_tx_time0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 ck_prep_set         :  8 ;   
		__u32 ck_zero_set         :  8 ;   
		__u32 ck_pre_set          :  8 ;   
		__u32 ck_post_set         :  8 ;   
	} bits;
} dphy_tx_time1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 ck_trail_set        :  8 ;   
		__u32 hs_dly_set          : 16 ;   
		__u32 res0                :  4 ;   
		__u32 hs_dly_mode         :  1 ;   
		__u32 res1                :  3 ;   
	} bits;
} dphy_tx_time2_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 lptx_ulps_exit_set  : 20 ;   
		__u32 res0                : 12 ;   
	} bits;
} dphy_tx_time3_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 hstx_ana0_set       :  8 ;   
		__u32 hstx_ana1_set       :  8 ;   
		__u32 res0                : 16 ;   
	} bits;
} dphy_tx_time4_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 lprx_to_en          :  1 ;   
		__u32 freq_cnt_en         :  1 ;   
		__u32 res0                :  2 ;   
		__u32 hsrx_clk_miss_en    :  1 ;   
		__u32 hsrx_sync_err_to_en :  1 ;   
		__u32 res1                :  2 ;   
		__u32 lprx_to             :  8 ;   
		__u32 hsrx_clk_miss       :  8 ;   
		__u32 hsrx_sync_err_to    :  8 ;   
	} bits;
} dphy_rx_time0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 lprx_ulps_wp        : 20 ;   
		__u32 rx_dly              : 12 ;   
	} bits;
} dphy_rx_time1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 hsrx_ana0_set       :  8 ;   
		__u32 hsrx_ana1_set       :  8 ;   
		__u32 res0                : 16 ;   
	} bits;
} dphy_rx_time2_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 freq_cnt            : 16 ;   
		__u32 res0                :  8 ;   
		__u32 lprst_dly           :  8 ;   
	} bits;
} dphy_rx_time3_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 reg_selsck          :  1 ;   
		__u32 reg_rsd             :  1 ;   
		__u32 reg_sfb             :  2 ;   
		__u32 reg_plr             :  4 ;   
		__u32 reg_den             :  4 ;   
		__u32 reg_slv             :  3 ;   
		__u32 reg_sdiv2           :  1 ;   
		__u32 reg_srxck           :  4 ;   
		__u32 reg_srxdt           :  4 ;   
		__u32 reg_dmpd            :  4 ;   
		__u32 reg_dmpc            :  1 ;   
		__u32 reg_pwenc           :  1 ;   
		__u32 reg_pwend           :  1 ;   
		__u32 reg_pws             :  1 ;   
	} bits;
} dphy_ana0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 reg_stxck           :  1 ;   
		__u32 res0                :  3 ;   
		__u32 reg_svdl0           :  4 ;   
		__u32 reg_svdl1           :  4 ;   
		__u32 reg_svdl2           :  4 ;   
		__u32 reg_svdl3           :  4 ;   
		__u32 reg_svdlc           :  4 ;   
		__u32 reg_svtt            :  4 ;   
		__u32 reg_csmps           :  2 ;   
		__u32 res1                :  1 ;   
		__u32 reg_vttmode         :  1 ;   
	} bits;
} dphy_ana1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 ana_cpu_en          :  1 ;   
		__u32 enib                :  1 ;   
		__u32 enrvs               :  1 ;   
		__u32 res0                :  1 ;   
		__u32 enck_cpu            :  1 ;   
		__u32 entxc_cpu           :  1 ;   
		__u32 enckq_cpu           :  1 ;   
		__u32 res1                :  1 ;   
		__u32 entx_cpu            :  4 ;   
		__u32 res2                :  1 ;   
		__u32 entermc_cpu         :  1 ;   
		__u32 enrxc_cpu           :  1 ;   
		__u32 res3                :  1 ;   
		__u32 enterm_cpu          :  4 ;   
		__u32 enrx_cpu            :  4 ;   
		__u32 enp2s_cpu           :  4 ;   
		__u32 res4                :  4 ;   
	} bits;
} dphy_ana2_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 enlptx_cpu          :  4 ;   
		__u32 enlprx_cpu          :  4 ;   
		__u32 enlpcd_cpu          :  4 ;   
		__u32 enlprxc_cpu         :  1 ;   
		__u32 enlptxc_cpu         :  1 ;   
		__u32 enlpcdc_cpu         :  1 ;   
		__u32 res0                :  1 ;   
		__u32 entest              :  1 ;   
		__u32 enckdbg             :  1 ;   
		__u32 enldor              :  1 ;   
		__u32 res1                :  5 ;   
		__u32 enldod              :  1 ;   
		__u32 enldoc              :  1 ;   
		__u32 endiv               :  1 ;   
		__u32 envttc              :  1 ;   
		__u32 envttd              :  4 ;   
	} bits;
} dphy_ana3_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 reg_txpusd          :  2 ;   
		__u32 reg_txpusc          :  2 ;   
		__u32 reg_txdnsd          :  2 ;   
		__u32 reg_txdnsc          :  2 ;   
		__u32 reg_tmsd            :  2 ;   
		__u32 reg_tmsc            :  2 ;   
		__u32 reg_ckdv            :  5 ;   
		__u32 res0                :  3 ;   
		__u32 reg_dmplvd		  :  4 ;
		__u32 reg_dmplvc		  :  1 ;
		__u32 res1				  :  7 ;

	} bits;
} dphy_ana4_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 sot_d0_int          :  1 ;   
		__u32 sot_d1_int          :  1 ;   
		__u32 sot_d2_int          :  1 ;   
		__u32 sot_d3_int          :  1 ;   
		__u32 sot_err_d0_int      :  1 ;   
		__u32 sot_err_d1_int      :  1 ;   
		__u32 sot_err_d2_int      :  1 ;   
		__u32 sot_err_d3_int      :  1 ;   
		__u32 sot_sync_err_d0_int :  1 ;   
		__u32 sot_sync_err_d1_int :  1 ;   
		__u32 sot_sync_err_d2_int :  1 ;   
		__u32 sot_sync_err_d3_int :  1 ;   
		__u32 rx_alg_err_d0_int   :  1 ;   
		__u32 rx_alg_err_d1_int   :  1 ;   
		__u32 rx_alg_err_d2_int   :  1 ;   
		__u32 rx_alg_err_d3_int   :  1 ;   
		__u32 res0                :  6 ;   
		__u32 cd_lp0_err_clk_int  :  1 ;   
		__u32 cd_lp1_err_clk_int  :  1 ;   
		__u32 cd_lp0_err_d0_int   :  1 ;   
		__u32 cd_lp1_err_d0_int   :  1 ;   
		__u32 cd_lp0_err_d1_int   :  1 ;   
		__u32 cd_lp1_err_d1_int   :  1 ;   
		__u32 cd_lp0_err_d2_int   :  1 ;   
		__u32 cd_lp1_err_d2_int   :  1 ;   
		__u32 cd_lp0_err_d3_int   :  1 ;   
		__u32 cd_lp1_err_d3_int   :  1 ;   
	} bits;
} dphy_int_en0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 ulps_d0_int         :  1 ;   
		__u32 ulps_d1_int         :  1 ;   
		__u32 ulps_d2_int         :  1 ;   
		__u32 ulps_d3_int         :  1 ;   
		__u32 ulps_wp_d0_int      :  1 ;   
		__u32 ulps_wp_d1_int      :  1 ;   
		__u32 ulps_wp_d2_int      :  1 ;   
		__u32 ulps_wp_d3_int      :  1 ;   
		__u32 ulps_clk_int        :  1 ;   
		__u32 ulps_wp_clk_int     :  1 ;   
		__u32 res0                :  2 ;   
		__u32 lpdt_d0_int         :  1 ;   
		__u32 rx_trnd_d0_int      :  1 ;   
		__u32 tx_trnd_err_d0_int  :  1 ;   
		__u32 undef1_d0_int       :  1 ;   
		__u32 undef2_d0_int       :  1 ;   
		__u32 undef3_d0_int       :  1 ;   
		__u32 undef4_d0_int       :  1 ;   
		__u32 undef5_d0_int       :  1 ;   
		__u32 rst_d0_int          :  1 ;   
		__u32 rst_d1_int          :  1 ;   
		__u32 rst_d2_int          :  1 ;   
		__u32 rst_d3_int          :  1 ;   
		__u32 esc_cmd_err_d0_int  :  1 ;   
		__u32 esc_cmd_err_d1_int  :  1 ;   
		__u32 esc_cmd_err_d2_int  :  1 ;   
		__u32 esc_cmd_err_d3_int  :  1 ;   
		__u32 false_ctl_d0_int    :  1 ;   
		__u32 false_ctl_d1_int    :  1 ;   
		__u32 false_ctl_d2_int    :  1 ;   
		__u32 false_ctl_d3_int    :  1 ;   
	} bits;
} dphy_int_en1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 res0                     ;   
	} bits;
} dphy_int_en2_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 sot_d0_pd           :  1 ;   
		__u32 sot_d1_pd           :  1 ;   
		__u32 sot_d2_pd           :  1 ;   
		__u32 sot_d3_pd           :  1 ;   
		__u32 sot_err_d0_pd       :  1 ;   
		__u32 sot_err_d1_pd       :  1 ;   
		__u32 sot_err_d2_pd       :  1 ;   
		__u32 sot_err_d3_pd       :  1 ;   
		__u32 sot_sync_err_d0_pd  :  1 ;   
		__u32 sot_sync_err_d1_pd  :  1 ;   
		__u32 sot_sync_err_d2_pd  :  1 ;   
		__u32 sot_sync_err_d3_pd  :  1 ;   
		__u32 rx_alg_err_d0_pd    :  1 ;   
		__u32 rx_alg_err_d1_pd    :  1 ;   
		__u32 rx_alg_err_d2_pd    :  1 ;   
		__u32 rx_alg_err_d3_pd    :  1 ;   
		__u32 res0                :  6 ;   
		__u32 cd_lp0_err_clk_pd   :  1 ;   
		__u32 cd_lp1_err_clk_pd   :  1 ;   
		__u32 cd_lp0_err_d1_pd    :  1 ;   
		__u32 cd_lp1_err_d1_pd    :  1 ;   
		__u32 cd_lp0_err_d0_pd    :  1 ;   
		__u32 cd_lp1_err_d0_pd    :  1 ;   
		__u32 cd_lp0_err_d2_pd    :  1 ;   
		__u32 cd_lp1_err_d2_pd    :  1 ;   
		__u32 cd_lp0_err_d3_pd    :  1 ;   
		__u32 cd_lp1_err_d3_pd    :  1 ;   
	} bits;
} dphy_int_pd0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 ulps_d0_pd          :  1 ;   
		__u32 ulps_d1_pd          :  1 ;   
		__u32 ulps_d2_pd          :  1 ;   
		__u32 ulps_d3_pd          :  1 ;   
		__u32 ulps_wp_d0_pd       :  1 ;   
		__u32 ulps_wp_d1_pd       :  1 ;   
		__u32 ulps_wp_d2_pd       :  1 ;   
		__u32 ulps_wp_d3_pd       :  1 ;   
		__u32 ulps_clk_pd         :  1 ;   
		__u32 ulps_wp_clk_pd      :  1 ;   
		__u32 res0                :  2 ;   
		__u32 lpdt_d0_pd          :  1 ;   
		__u32 rx_trnd_d0_pd       :  1 ;   
		__u32 tx_trnd_err_d0_pd   :  1 ;   
		__u32 undef1_d0_pd        :  1 ;   
		__u32 undef2_d0_pd        :  1 ;   
		__u32 undef3_d0_pd        :  1 ;   
		__u32 undef4_d0_pd        :  1 ;   
		__u32 undef5_d0_pd        :  1 ;   
		__u32 rst_d0_pd           :  1 ;   
		__u32 rst_d1_pd           :  1 ;   
		__u32 rst_d2_pd           :  1 ;   
		__u32 rst_d3_pd           :  1 ;   
		__u32 esc_cmd_err_d0_pd   :  1 ;   
		__u32 esc_cmd_err_d1_pd   :  1 ;   
		__u32 esc_cmd_err_d2_pd   :  1 ;   
		__u32 esc_cmd_err_d3_pd   :  1 ;   
		__u32 false_ctl_d0_pd     :  1 ;   
		__u32 false_ctl_d1_pd     :  1 ;   
		__u32 false_ctl_d2_pd     :  1 ;   
		__u32 false_ctl_d3_pd     :  1 ;   
	} bits;
} dphy_int_pd1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 res0                     ;   
	} bits;
} dphy_int_pd2_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 lptx_sta_d0         :  3 ;   
		__u32 res0                :  1 ;   
		__u32 lptx_sta_d1         :  3 ;   
		__u32 res1                :  1 ;   
		__u32 lptx_sta_d2         :  3 ;   
		__u32 res2                :  1 ;   
		__u32 lptx_sta_d3         :  3 ;   
		__u32 res3                :  1 ;   
		__u32 lptx_sta_clk        :  3 ;   
		__u32 res4                :  9 ;   
		__u32 direction           :  1 ;   
		__u32 res5                :  3 ;   
	} bits;
} dphy_dbg0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 lptx_dbg_en         :  1 ;   
		__u32 hstx_dbg_en         :  1 ;   
		__u32 res0                :  2 ;   
		__u32 lptx_set_d0         :  2 ;   
		__u32 lptx_set_d1         :  2 ;   
		__u32 lptx_set_d2         :  2 ;   
		__u32 lptx_set_d3         :  2 ;   
		__u32 lptx_set_ck         :  2 ;   
		__u32 res1                : 18 ;   
	} bits;
} dphy_dbg1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 hstx_data                ;   
	} bits;
} dphy_dbg2_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 lprx_sta_d0         :  4 ;   
		__u32 lprx_sta_d1         :  4 ;   
		__u32 lprx_sta_d2         :  4 ;   
		__u32 lprx_sta_d3         :  4 ;   
		__u32 lprx_sta_clk        :  4 ;   
		__u32 res0                : 12 ;   
	} bits;
} dphy_dbg3_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 lprx_phy_d0         :  2 ;   
		__u32 lprx_phy_d1         :  2 ;   
		__u32 lprx_phy_d2         :  2 ;   
		__u32 lprx_phy_d3         :  2 ;   
		__u32 lprx_phy_clk        :  2 ;   
		__u32 res0                :  6 ;   
		__u32 lpcd_phy_d0         :  2 ;   
		__u32 lpcd_phy_d1         :  2 ;   
		__u32 lpcd_phy_d2         :  2 ;   
		__u32 lpcd_phy_d3         :  2 ;   
		__u32 lpcd_phy_clk        :  2 ;   
		__u32 res1                :  6 ;   
	} bits;
} dphy_dbg4_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 hsrx_data                ;   
	} bits;
} dphy_dbg5_reg_t;


typedef union
{
	__u32 dwval;
	struct
	{
		__u32 res0                      ;   
	} bits;
}dphy_reservd_reg_t;

typedef struct
{
	dphy_ctl_reg_t					dphy_gctl;		
	dphy_tx_ctl_reg_t				dphy_tx_ctl;	
	dphy_rx_ctl_reg_t				dphy_rx_ctl;	
	dphy_reservd_reg_t				dphy_reg00c;	
	dphy_tx_time0_reg_t				dphy_tx_time0;	
	dphy_tx_time1_reg_t				dphy_tx_time1;	
	dphy_tx_time2_reg_t				dphy_tx_time2;	
	dphy_tx_time3_reg_t				dphy_tx_time3;	
	dphy_tx_time4_reg_t				dphy_tx_time4;	
	dphy_reservd_reg_t				dphy_reg024[3];	
	dphy_rx_time0_reg_t				dphy_rx_time0;	
	dphy_rx_time1_reg_t				dphy_rx_time1;	
	dphy_rx_time2_reg_t				dphy_rx_time2;	
	dphy_reservd_reg_t				dphy_reg03c;	
	dphy_rx_time3_reg_t				dphy_rx_time3;	
	dphy_reservd_reg_t				dphy_reg044[2];	
	dphy_ana0_reg_t					dphy_ana0;		
	dphy_ana1_reg_t					dphy_ana1;		
	dphy_ana2_reg_t					dphy_ana2;		
	dphy_ana3_reg_t					dphy_ana3;		
	dphy_ana4_reg_t					dphy_ana4;		
	dphy_int_en0_reg_t				dphy_int_en0;	
	dphy_int_en1_reg_t				dphy_int_en1;	
	dphy_int_en2_reg_t				dphy_int_en2;	
	dphy_reservd_reg_t				dphy_reg06c;	
	dphy_int_pd0_reg_t				dphy_int_pd0;	
	dphy_int_pd1_reg_t				dphy_int_pd1;	
	dphy_int_pd2_reg_t				dphy_int_pd2;	
	dphy_reservd_reg_t				dphy_reg07c[25];
	dphy_dbg0_reg_t					dphy_dbg0;		
	dphy_dbg1_reg_t					dphy_dbg1;		
	dphy_dbg2_reg_t					dphy_dbg2;		
	dphy_dbg3_reg_t					dphy_dbg3;		
	dphy_dbg4_reg_t					dphy_dbg4;		
	dphy_dbg5_reg_t					dphy_dbg5;		
}__de_dsi_dphy_dev_t;





typedef union
{
	struct
	{
		__u32 byte012                    : 24 ;   
		__u32 byte3                    	 :  8 ;   
	}bytes;
	struct
	{
		__u32 bit00                    	 :  1 ;   
		__u32 bit01                    	 :  1 ;   
		__u32 bit02                    	 :  1 ;   
		__u32 bit03                    	 :  1 ;   
		__u32 bit04                    	 :  1 ;   
		__u32 bit05                    	 :  1 ;   
		__u32 bit06                    	 :  1 ;   
		__u32 bit07                    	 :  1 ;   
		__u32 bit08                    	 :  1 ;   
		__u32 bit09                    	 :  1 ;   
		__u32 bit10                    	 :  1 ;   
		__u32 bit11                    	 :  1 ;   
		__u32 bit12                    	 :  1 ;   
		__u32 bit13                    	 :  1 ;   
		__u32 bit14                    	 :  1 ;   
		__u32 bit15                    	 :  1 ;   
		__u32 bit16                    	 :  1 ;   
		__u32 bit17                    	 :  1 ;   
		__u32 bit18                    	 :  1 ;   
		__u32 bit19                    	 :  1 ;   
		__u32 bit20                    	 :  1 ;   
		__u32 bit21                    	 :  1 ;   
		__u32 bit22                    	 :  1 ;   
		__u32 bit23                    	 :  1 ;   
		__u32 bit24                    	 :  1 ;   
		__u32 bit25                    	 :  1 ;   
		__u32 bit26                    	 :  1 ;   
		__u32 bit27                    	 :  1 ;   
		__u32 bit28                    	 :  1 ;   
		__u32 bit29                    	 :  1 ;   
		__u32 bit30                    	 :  1 ;   
		__u32 bit31                    	 :  1 ;   
	} bits;
} dsi_ph_t;


typedef enum
{
    DSI_DT_VSS                  = 0x01,
    DSI_DT_VSE                  = 0x11,
    DSI_DT_HSS                  = 0x21,
    DSI_DT_HSE                  = 0x31,
    DSI_DT_EOT                  = 0x08,
    DSI_DT_CM_OFF               = 0x02,
    DSI_DT_CM_ON                = 0x12,
    DSI_DT_SHUT_DOWN            = 0x22,
    DSI_DT_TURN_ON              = 0x32,
    DSI_DT_GEN_WR_P0            = 0x03,
    DSI_DT_GEN_WR_P1            = 0x13,
    DSI_DT_GEN_WR_P2            = 0x23,
    DSI_DT_GEN_RD_P0            = 0x04,
    DSI_DT_GEN_RD_P1            = 0x14,
    DSI_DT_GEN_RD_P2            = 0x24,
    DSI_DT_DCS_WR_P0            = 0x05,
    DSI_DT_DCS_WR_P1            = 0x15,
    DSI_DT_DCS_RD_P0            = 0x06,
    DSI_DT_MAX_RET_SIZE         = 0x37,
    DSI_DT_NULL                 = 0x09,
    DSI_DT_BLK                  = 0x19,
    DSI_DT_GEN_LONG_WR          = 0x29,
    DSI_DT_DCS_LONG_WR          = 0x39,
    DSI_DT_PIXEL_RGB565         = 0x0E,
    DSI_DT_PIXEL_RGB666P        = 0x1E,
    DSI_DT_PIXEL_RGB666         = 0x2E,
    DSI_DT_PIXEL_RGB888         = 0x3E,

    DSI_DT_ACK_ERR              = 0x02,
    DSI_DT_EOT_PERI             = 0x08,
    DSI_DT_GEN_RD_R1            = 0x11,
    DSI_DT_GEN_RD_R2            = 0x12,
    DSI_DT_GEN_LONG_RD_R        = 0x1A,
    DSI_DT_DCS_LONG_RD_R        = 0x1C,
    DSI_DT_DCS_RD_R1            = 0x21,
    DSI_DT_DCS_RD_R2            = 0x22,
}__dsi_dt_t;

typedef enum
{
	DSI_DCS_ENTER_IDLE_MODE				=	0x39 ,		
	DSI_DCS_ENTER_INVERT_MODE			=	0x21 ,		
	DSI_DCS_ENTER_NORMAL_MODE			=	0x13 ,		
	DSI_DCS_ENTER_PARTIAL_MODE			=	0x12 ,		
	DSI_DCS_ENTER_SLEEP_MODE			=	0x10 ,		
	DSI_DCS_EXIT_IDLE_MODE				=	0x38 ,		
	DSI_DCS_EXIT_INVERT_MODE			=	0x20 ,		
	DSI_DCS_EXIT_SLEEP_MODE				=	0x11 ,		
	DSI_DCS_GET_ADDRESS_MODE			=	0x0b ,		
	DSI_DCS_GET_BLUE_CHANNEL			=	0x08 ,		
	DSI_DCS_GET_DIAGNOSTIC_RESULT		=	0x0f ,		
	DSI_DCS_GET_DISPLAY_MODE			=	0x0d ,		
	DSI_DCS_GET_GREEN_CHANNEL			=	0x07 ,		
	DSI_DCS_GET_PIXEL_FORMAT			=	0x0c ,		
	DSI_DCS_GET_POWER_MODE				=	0x0a ,		
	DSI_DCS_GET_RED_CHANNEL				=	0x06 ,		
	DSI_DCS_GET_SCANLINE				=	0x45 ,		
	DSI_DCS_GET_SIGNAL_MODE				=	0x0e ,		
	DSI_DCS_NOP							=	0x00 ,		
	DSI_DCS_READ_DDB_CONTINUE			=	0xa8 ,		
	DSI_DCS_READ_DDB_START				=	0xa1 ,		
	DSI_DCS_READ_MEMORY_CONTINUE		=	0x3e ,		
	DSI_DCS_READ_MEMORY_START			=	0x2e ,		
	DSI_DCS_SET_ADDRESS_MODE			=	0x36 ,		
	DSI_DCS_SET_COLUMN_ADDRESS			=	0x2a ,		
	DSI_DCS_SET_DISPLAY_OFF				=	0x28 ,		
	DSI_DCS_SET_DISPLAY_ON				=	0x29 ,		
	DSI_DCS_SET_GAMMA_CURVE				=	0x26 ,		
	DSI_DCS_SET_PAGE_ADDRESS			=	0x2b ,		
	DSI_DCS_SET_PARTIAL_AREA			=	0x30 ,		
	DSI_DCS_SET_PIXEL_FORMAT			=	0x3a ,		
	DSI_DCS_SET_SCROLL_AREA				=	0x33 ,		
	DSI_DCS_SET_SCROLL_START			=	0x37 ,		
	DSI_DCS_SET_TEAR_OFF				=	0x34 ,		
	DSI_DCS_SET_TEAR_ON					=	0x35 ,		
	DSI_DCS_SET_TEAR_SCANLINE			=	0x44 ,		
	DSI_DCS_SOFT_RESET					=	0x01 ,		
	DSI_DCS_WRITE_LUT					=	0x2d ,		
	DSI_DCS_WRITE_MEMORY_CONTINUE		=	0x3c ,		
	DSI_DCS_WRITE_MEMORY_START			=	0x2c ,		
}__dsi_dcs_t;

typedef enum
{
	DSI_START_LP11				= 0,
	DSI_START_TBA				= 1,
	DSI_START_HSTX				= 2,
	DSI_START_LPTX				= 3,
	DSI_START_LPRX				= 4,
	DSI_START_HSC         = 5,
	DSI_START_HSD         = 6,
}__dsi_start_t;

typedef enum
{
	DSI_INST_ID_LP11			= 0,
	DSI_INST_ID_TBA				= 1,
	DSI_INST_ID_HSC				= 2,
	DSI_INST_ID_HSD				= 3,
	DSI_INST_ID_LPDT			= 4,
	DSI_INST_ID_HSCEXIT			= 5,
	DSI_INST_ID_NOP				= 6,
	DSI_INST_ID_DLY				= 7,
	DSI_INST_ID_END				= 15,
}__dsi_inst_id_t;

typedef enum
{
	DSI_INST_MODE_STOP			= 0,
	DSI_INST_MODE_TBA			= 1,
	DSI_INST_MODE_HS			= 2,
	DSI_INST_MODE_ESCAPE		= 3,
	DSI_INST_MODE_HSCEXIT		= 4,
	DSI_INST_MODE_NOP			= 5,
}__dsi_inst_mode_t;

typedef enum
{
	DSI_INST_ESCA_LPDT			= 0,
	DSI_INST_ESCA_ULPS			= 1,
	DSI_INST_ESCA_UN1			= 2,
	DSI_INST_ESCA_UN2			= 3,
	DSI_INST_ESCA_RESET			= 4,
	DSI_INST_ESCA_UN3			= 5,
	DSI_INST_ESCA_UN4			= 6,
	DSI_INST_ESCA_UN5			= 7,
}__dsi_inst_escape_t;

typedef enum
{
	DSI_INST_PACK_PIXEL			= 0,
	DSI_INST_PACK_COMMAND		= 1,
}__dsi_inst_packet_t;



__u8 	dsi_ecc_pro(__u32 dsi_ph);
__u16 	dsi_crc_pro(__u8* pd_p,__u32 pd_bytes);
__u16 	dsi_crc_pro_pd_repeat(__u8 pd,__u32 pd_bytes);


#endif
