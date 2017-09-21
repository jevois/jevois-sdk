#ifndef __REG67__SAT__H__
#define __REG67__SAT__H__

#include "iep.h"
typedef union
{
	__u32 dwval;
	struct
	{
		__u32	en				:1;
		__u32	r0				:30;
	}bits;
}sac_ctrl_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32 sat_w			:12;
		__u32 r0			:4;	
		__u32 sat_h			:12;
		__u32 r1			:4; 
	}bits;
}sac_size_reg_t; 


typedef union
{
	__u32 dwval;
	struct
	{
		__u32	win_left        :12;//bit0~11
		__u32	r0				:4;
		__u32	win_top			:12;//bit16~27
		__u32	r1				:4;
	}bits;
}sac_win0_reg_t;	

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	win_right		:12;//bit0~11
		__u32	r0				:4;
		__u32	win_bottom		:12;//bit16~27
		__u32	r1				:4;
	}bits;
}sac_win1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	low_sat		:6;	
		__u32	r0			:26;
	}bits;
}sac_low_sat_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	module_en	:1;	
		__u32   r0			:15;   
		__u32	ram_switch	:1;	
		__u32   r1			:15;
	}bits;
}sa_global_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	r_num		:24;   
		__u32	r0			:8;	
	}bits;
}sac_num_reg_t;	    

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	en				:1;
		__u32	r0				:30;
	}bits;
}sat_ctrl_reg_t;


typedef union
{
	__u32 dwval;
	struct
	{
		__u32	win_left        :12;//bit0~11
		__u32	r0				:4;
		__u32	win_top			:12;//bit16~27
		__u32	r1				:4;
	}bits;
}sat_win0_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	win_right		:12;//bit0~11
		__u32	r0				:4;
		__u32	win_bottom		:12;//bit16~27
		__u32	r1				:4;
	}bits;
}sat_win1_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_g_coff		:13;//bit0~bit12
		__u32	r0				:19;//bit13~bit31
	}bits;
}sat_cscgcoff_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_g_con		:14;//bit0~bit13
		__u32	r0				:18;//bit14~bit31
	}bits;
}sat_cscgcon_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_r_coff		:13;//bit0~bit12
		__u32	r0				:19;//bit13~bit31
	}bits;
}sat_cscrcoff_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_r_con		:14;//bit0~bit13
		__u32	r0				:18;//bit14~bit31
	}bits;
}sat_cscrcon_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_b_coff		:13;//bit0~bit12
		__u32	r0				:19;//bit13~bit31
	}bits;
}sat_cscbcoff_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_b_con		:14;//bit0~bit13
		__u32	r0				:18;//bit14~bit31
	}bits;
}sat_cscbcon_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	coeff0			:8;//bit0~bit7
		__u32	coeff1			:8;//bit8~bit15
		__u32	coeff2			:8;//bit16~bit23
		__u32	coeff3			:8;//bit24~bit31
	}bits;
}sat_cbcoef_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	coeff0			:8;//bit0~bit7
		__u32	coeff1			:8;//bit8~bit15
		__u32	coeff2			:8;//bit16~bit23
		__u32	coeff3			:8;//bit24~bit31
	}bits;
}sat_crcoef_reg_t;


typedef struct
{
	sac_ctrl_reg_t			sacctrl;	
	sac_size_reg_t		    sacsize;	   
	sac_win0_reg_t			sacwin0;    
	sac_win1_reg_t			sacwin1;    
	sac_low_sat_reg_t		lowsat;	       
	sac_num_reg_t			R_num;	       
	sa_global_reg_t			saglobal;	
	__u32					r0;        	
	sat_ctrl_reg_t			satctrl;	   
	__u32					r1;        	
	sat_win0_reg_t			satwin0;    
	sat_win1_reg_t			satwin1;    
	sat_cscgcoff_reg_t		cscgcoff[3];
	sat_cscgcon_reg_t		cscgcon;	
	sat_cscrcoff_reg_t		cscrcoff[3];
	sat_cscrcon_reg_t		cscrcon;	
	sat_cscbcoff_reg_t		cscbcoff[3];
	sat_cscbcon_reg_t		cscbcon;	
	__u32					r2[40];		
	sat_cbcoef_reg_t		satcbtab[64];
	sat_crcoef_reg_t		satcrtab[64];
}__sa_reg;

__s32 SAT_Set_Reg_Base(__u32 sel, __u32 base);
__u32 SAT_Get_Reg_Base(__u32 sel);
__s32 SAT_Enable_Disable( __u32 sel,__u32 en);
__s32 SAT_Set_Display_Size( __u32 sel,__u32 width,__u32 height);
__s32 SAT_Set_Window(__u32 sel, disp_window *window);
__s32 SAT_Set_Sync_Para(__u32 sel);


#endif
