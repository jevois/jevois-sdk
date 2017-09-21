
#ifndef __DE_FCC_TYPE__
#define __DE_FCC_TYPE__

#include "de_rtmx.h"

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int en				   :  1 ;// Default: 0x0;
		unsigned int res0              :  7 ;// Default: ;
		unsigned int win_en			   :  1 ;// Default: 0x0;
		unsigned int res1              :  23;// Default: ;
	} bits;
} FCC_CTRL_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int width				:13;// Default: 0x0;
		unsigned int res0				:3;
		unsigned int height				:13;// Default: 0x0;
		unsigned int res1				:3;
	} bits;
} FCC_SIZE_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int left				:13;// Default: 0x0;
		unsigned int res0				:3;
		unsigned int top				:13;// Default: 0x0;
		unsigned int res1				:3;
	} bits;
} FCC_WIN0_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int right				:13;// Default: 0x0;
		unsigned int res0				:3;
		unsigned int bot				:13;// Default: 0x0;
		unsigned int res1				:3;
	} bits;
} FCC_WIN1_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int hmin				:12;// Default: 0x0;
		unsigned int res0				:4;
		unsigned int hmax				:12;// Default: 0x0;
		unsigned int res1				:4;
	} bits;
} FCC_HUE_RANGE_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int sgain				:9;// Default: 0x0;
		unsigned int res0				:7;// Default: ;
		unsigned int hgain				:9;// Default: 0x0;
		unsigned int res1				:7;// Default: ;
	} bits;
} FCC_HS_GAIN_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int bypass				:1;
		unsigned int res0				:31;// Default: ;
	} bits;
} FCC_CSC_CTL_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int coff				:13;// Default: 0x0;
		unsigned int res0				:19;// Default: ;
	} bits;
} FCC_CSC_COEFF_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int cont			:20;// Default: 0x0;
		unsigned int res0			:12;// Default: ;
	} bits;
} FCC_CSC_CONST_REG;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int res0			:24;// Default: 0x0;
		unsigned int alpha			:8;
	} bits;
} FCC_GLB_APH_REG;

typedef struct
{
	FCC_CTRL_REG		fcc_ctl;		
	FCC_SIZE_REG		fcc_size;		
	FCC_WIN0_REG		fcc_win0;		
	FCC_WIN1_REG		fcc_win1;		
	FCC_HUE_RANGE_REG	fcc_range[6];	
	unsigned int		res0[2];		
	FCC_HS_GAIN_REG		fcc_gain[6];	
	unsigned int		res1[2];		
	FCC_CSC_CTL_REG		fcc_csc_ctl;	
	unsigned int		res2[3];		
	FCC_CSC_COEFF_REG	fcc_csc_coff0[3];
	FCC_CSC_CONST_REG	fcc_csc_const0;	
	FCC_CSC_COEFF_REG	fcc_csc_coff1[3];
	FCC_CSC_CONST_REG	fcc_csc_const1;	
	FCC_CSC_COEFF_REG	fcc_csc_coff2[3];
	FCC_CSC_CONST_REG	fcc_csc_const2;	
	FCC_GLB_APH_REG		fcc_glb_alpha;	

}__fcc_reg_t;

typedef struct
{
	unsigned int fcc_en;
	unsigned int sgain[6];

	unsigned int win_en;
	de_rect win;
}__fcc_config_data;


#endif

