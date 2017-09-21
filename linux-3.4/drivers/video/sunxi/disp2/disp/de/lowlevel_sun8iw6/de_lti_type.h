
#ifndef __DE_LTI_TYPE_H__
#define __DE_LTI_TYPE_H__

#include "de_rtmx.h"

#define LTI_PARA_NUM 1
#define LTI_MODE_NUM 2

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int en				   :  1 ;   
		unsigned int res0              :  7;   
		unsigned int sel			   :  1 ;   
		unsigned int res1              :  7;   
		unsigned int nonl_en		   :  1 ;   
		unsigned int res2              :  7;   
		unsigned int win_en		   	   :  1 ;   
		unsigned int res3              :  7;   

	} bits;
} LTI_EN;

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
} LTI_SIZE;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int c0			        :  8;   
		unsigned int res0               :  8 ;	 
		unsigned int c1			        :  8;   
		unsigned int res1               :  8 ;	 
	} bits;
} LTI_FIR_COFF0;
typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int c2			        :  8;   
		unsigned int res0               :  8 ;	 
		unsigned int c3			        :  8;   
		unsigned int res1               :  8 ;	 
	} bits;
} LTI_FIR_COFF1;
typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int c4			        :  8;   
		unsigned int res0               :  24 ;	 
	} bits;
} LTI_FIR_COFF2;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lti_fil_gain       :  4 ;	  
		unsigned int res0				:  28 ;    

	} bits;
} LTI_FIR_GAIN;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lti_cor_th			:  10 ;	  
		unsigned int res0				:  22 ;    
	} bits;
} LTI_COR_TH;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int offset				:  8 ;	  
		unsigned int res0				:  8 ;    
		unsigned int slope				:  5 ;	  
		unsigned int res1				:  11 ;    
	} bits;
} LTI_DIFF_CTL;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int edge_gain			:  5 ;	  
		unsigned int res0				:  27;    
	} bits;
} LTI_EDGE_GAIN;


typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int core_x				:  8 ;	  
		unsigned int res0				:  8;    
		unsigned int clip				:  8 ;	  
		unsigned int res1				:  4;    
		unsigned int peak_limit			:  3 ;	  
		unsigned int res2				:  1;    
	} bits;
} LTI_OS_CON;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int win_range			:  8 ;	  
		unsigned int res0				:  24;    
	} bits;
} LTI_WIN_EXPANSION;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int elvel_th			:  8 ;	  
		unsigned int res0				:  24;    
	} bits;
} LTI_EDGE_ELVEL_TH;

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
} LTI_WIN0_REG;

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
} LTI_WIN1_REG;

typedef struct
{
	LTI_EN			    ctrl   		   ;
	unsigned int		res0[2]		   ;   
	LTI_SIZE			size		   ;
	LTI_FIR_COFF0       coef0          ;  
	LTI_FIR_COFF1       coef1          ;  
	LTI_FIR_COFF2       coef2          ;  
	LTI_FIR_GAIN        gain           ;  
	LTI_COR_TH          corth          ;  
	LTI_DIFF_CTL        diff           ;  
	LTI_EDGE_GAIN       edge_gain      ;  
	LTI_OS_CON          os_con         ;  
	LTI_WIN_EXPANSION   win_range      ;  
	LTI_EDGE_ELVEL_TH   elvel_th       ;  
	LTI_WIN0_REG		win0	   	   ;  
	LTI_WIN1_REG		win1	   	   ;  
}__lti_reg_t;

typedef struct
{
	unsigned int lti_en;
	unsigned int gain;

	unsigned int win_en;
	de_rect win;

}__lti_config_data;

#endif
