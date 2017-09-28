
#ifndef __DE_PEAK_TYPE_H__
#define __DE_PEAK_TYPE_H__

#include "de_rtmx.h"

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int en				   :  1 ;   
		unsigned int res0              :  7 ;   
		unsigned int win_en			   :  1 ;   
		unsigned int res1              :  23;   
	} bits;
} LP_CTRL_REG;

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
} LP_SIZE_REG;

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
} LP_WIN0_REG;

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
} LP_WIN1_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int bp1_ratio			:  6 ;	  
		unsigned int res0				:  2 ;    
		unsigned int bp0_ratio			:  6 ;	  
		unsigned int res1				:  2 ;    
		unsigned int hp_ratio			:  6 ;	  
		unsigned int res2				:  9 ;    
		unsigned int filter_sel			:  1 ;    
	} bits;
} LP_FILTER_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int c0					:  9 ;	  
		unsigned int res0				:  7 ;    
		unsigned int c1					:  9 ;	  
		unsigned int res1				:  7 ;    
	} bits;
} LP_CSTM_FILTER0_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int c2					:  9 ;	  
		unsigned int res0				:  7 ;    
		unsigned int c3					:  9 ;	  
		unsigned int res1				:  7 ;    
	} bits;
} LP_CSTM_FILTER1_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int c4					:  9 ;	  
		unsigned int res0				:  23;    
	} bits;
} LP_CSTM_FILTER2_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int gain				:  8 ;	  
		unsigned int res0				:  24;    
	} bits;
} LP_GAIN_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int beta				:  5 ;	  
		unsigned int res0				:  11;    
		unsigned int dif_up				:  8 ;	  
		unsigned int res1				:  8 ;    
	} bits;
} LP_GAINCTRL_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int neg_gain			:  6 ;	  
		unsigned int res0				:  26;    
	} bits;
} LP_SHOOTCTRL_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int corthr				:  8 ;	  
		unsigned int res0				:  24;    
	} bits;
} LP_CORING_REG;

typedef struct
{
	LP_CTRL_REG			ctrl   		;
	LP_SIZE_REG			size		;
	LP_WIN0_REG			win0		;
	LP_WIN1_REG			win1		;
	LP_FILTER_REG		filter		;
	LP_CSTM_FILTER0_REG cfilter0    ;
	LP_CSTM_FILTER1_REG cfilter1	;
	LP_CSTM_FILTER2_REG	cfilter2	;
	LP_GAIN_REG         gain        ;  
	LP_GAINCTRL_REG		gainctrl    ;
	LP_SHOOTCTRL_REG	shootctrl	;
	LP_CORING_REG		coring		;
}__peak_reg_t;

typedef struct
{
	unsigned int peak_en;
	unsigned int gain;

	unsigned int win_en;
	de_rect win;

}__peak_config_data;

#endif
