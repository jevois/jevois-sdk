
#ifndef __DE_FCE_TYPE_H__
#define __DE_FCE_TYPE_H__

#include "de_rtmx.h"

#define HIST_FRAME_MASK  0x00000002	
#define CE_FRAME_MASK    0x00000002   

#define LCE_PARA_NUM  2
#define LCE_MODE_NUM  2

#define AUTOCE_PARA_NUM  5
#define AUTOCE_MODE_NUM  3

#define CE_PARA_NUM  2
#define CE_MODE_NUM  2

#define FTC_PARA_NUM  2
#define FTC_MODE_NUM  2

#define AVG_NUM 8

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int en				   :  1 ;   
		unsigned int res0              :  15;   
		unsigned int hist_en		   :  1 ;   
		unsigned int ce_en             :  1 ;   
		unsigned int lce_en            :  1 ;   
		unsigned int res1              :  1 ;   
		unsigned int ftc_en            :  1 ;   
		unsigned int res2              :  10;   
		unsigned int win_en            :  1 ;   
	} bits;
} FCE_GCTRL_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int width				:  12;   
		unsigned int res0				:  4 ;   
		unsigned int height				:  12;   
		unsigned int res1				:  4 ;   
	} bits;
} FCE_SIZE_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int win_left			:  12;   
		unsigned int res0               :  4 ;	 
		unsigned int win_top			:  12;   
		unsigned int res1               :  4 ;	 
	} bits;
} FCE_WIN0_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int win_right			:  12;   
		unsigned int res0               :  4 ;	 
		unsigned int win_bot			:  12;   
		unsigned int res1               :  4 ;	 
	} bits;
} FCE_WIN1_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lce_gain			:  6 ;	  
		unsigned int res0				:  2 ;    
		unsigned int lce_blend			:  8 ;	  
		unsigned int res1				:  16 ;   
	} bits;
} LCE_GAIN_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int sum				:  32 ;	  
	} bits;
} HIST_SUM_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int hist_valid			:  1  ;	  
		unsigned int res0				:  31 ;    
	} bits;
} HIST_STATUS_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int celut_access_switch :  1 ;	  
		unsigned int res0				:  31;    
	} bits;
} CE_STATUS_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int ftc_gain1			:  8 ;	  
		unsigned int res0				:  8 ;    
		unsigned int ftc_gain2			:  8 ;	  
		unsigned int res1				:  8 ;    
	} bits;
} FTC_GAIN_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int csc_bypass			:  1 ;	  
		unsigned int res0				:  31;    
	} bits;
} FTC_CSCBYPASS_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lut0				:  8 ;	  
		unsigned int lut1				:  8 ;	  
		unsigned int lut2				:  8 ;	  
		unsigned int lut3				:  8 ;	  
	} bits;
} CE_LUT_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int hist				:  22;	  
		unsigned int res0				:  10;    
	} bits;
} HIST_CNT_REG;

typedef struct
{
	FCE_GCTRL_REG		ctrl   		;
	FCE_SIZE_REG		size		;
	FCE_WIN0_REG		win0		;
	FCE_WIN1_REG		win1		;
	LCE_GAIN_REG		lcegain		;
	unsigned int        res0[3]		;
	HIST_SUM_REG		histsum		;
	HIST_STATUS_REG		histstauts	;
	CE_STATUS_REG		cestatus	;
	unsigned int        res1		;
	FTC_GAIN_REG        ftcgain     ;  
	unsigned int        res2[3]		;
	FTC_CSCBYPASS_REG	cscbypass	;
	unsigned int        res3[47]	;
	CE_LUT_REG			celut[64]	;
	HIST_CNT_REG		hist[256]	;
}__fce_reg_t;

typedef struct
{
	unsigned int fce_en;

	unsigned int ce_en;
	unsigned int up_precent_thr;
	unsigned int down_precent_thr;
	unsigned int b_automode;
	unsigned int update_diff_thr;

	unsigned int present_black;
	unsigned int present_white;
	unsigned int slope_black_lmt;
	unsigned int slope_white_lmt;
	unsigned int black_prec;
	unsigned int white_prec;
	unsigned int lowest_black;
	unsigned int highest_white;

	unsigned int lce_en;
	unsigned int lce_blend;
	unsigned int lce_gain;

	unsigned int ftc_en;
	unsigned int ftc_gain1;
	unsigned int ftc_gain2;

	unsigned int hist_en;

	unsigned int win_en;
	de_rect win;
}__fce_config_data;

typedef struct
{
	unsigned int Runtime;	
	unsigned int IsEnable;	
	unsigned int TwoHistReady;
}__hist_status_t;

typedef struct
{
	unsigned int IsEnable;	
	unsigned int b_autoMode;
	unsigned int width;
	unsigned int height;
	unsigned int up_precent_thr;
	unsigned int down_precent_thr;
	unsigned int update_diff_thr;
	unsigned int slope_black_lmt;
	unsigned int slope_white_lmt;

}__ce_status_t;

typedef struct
{
  unsigned int hist_mean;
  unsigned int old_hist_mean;
           int hist_mean_diff;
  unsigned int avg_mean_saved[AVG_NUM];
  unsigned int avg_mean_idx;
  unsigned int avg_mean;
  unsigned int counter;
  unsigned int diff_coeff;

  unsigned int black_thr0;
  unsigned int black_thr1;
  unsigned int white_thr0;
  unsigned int white_thr1;

  unsigned int black_slp0;
  unsigned int black_slp1;
  unsigned int white_slp0;
  unsigned int white_slp1;

}hist_data;

#endif
