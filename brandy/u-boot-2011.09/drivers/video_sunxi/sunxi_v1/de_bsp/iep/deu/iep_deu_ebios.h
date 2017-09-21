#ifndef __IEP_DEU_EBIOS_H__
#define __IEP_DEU_EBIOS_H__

#include "../iep.h"

typedef union   
{
	__u32 dwval;
	struct
	{
		__u32	en				:1;
		__u32	r0				:3;
		__u32	reg_rdy			:1;
		__u32   coeff_rdy		:1;
		__u32   r1				:2;
		__u32	csc_en			:1;
		__u32	r2				:3;
		__u32	out_port_sel	:1;
		__u32	r3				:19;//bit13~bit31
		
	}bits;
}__imgehc_deu_en_reg_t;

typedef union   
{
	__u32 dwval;
	struct
	{
		__u32	width			:12;//bit0~bit11
		__u32	r0				:4;
		__u32	height			:12;//bit16~bit27
		__u32   r1				:4;
	}bits;
}__imgehc_deu_psize_reg_t;//0x04

typedef union   
{
	__u32 dwval;
	struct
	{
		__u32	win_left		:12;//bit0~bit11
		__u32	r0				:4;
		__u32	win_top			:12;//bit16~bit27
		__u32   r1				:4;
	}bits;
}__imgehc_deu_pwp0_reg_t;

typedef union   
{
	__u32 dwval;
	struct
	{
		__u32	win_right		:12;//bit0~bit11
		__u32	r0				:4;
		__u32	win_bot			:12;//bit16~bit27
		__u32   r1				:4;
	}bits;
}__imgehc_deu_pwp1_reg_t;

typedef union   
{
	__u32 dwval;
	struct
	{
		__u32	dcti_en			:1;
		__u32	r0				:5;
		__u32	dcti_hill_en	:1;
		__u32	dcti_suphill_en	:1;
		__u32	dcti_filter1_sel:2;
		__u32	dcti_filter2_sel:2;
		__u32	dcti_path_limit :4;
		__u32   dcti_gain		:6;
		__u32   r1				:2;
		__u32   uv_diff_sign_mode_sel:2;//bit24~bit25
		__u32   uv_same_sign_mode_sel:2;//bit26~bit27
		__u32   uv_diff_sign_maxmin_mode_sel:1;//bit28
		__u32   uv_same_sign_maxmin_mode_sel:1;//bit29
		__u32   r2              :1;
		__u32	uv_separate_en	:1;
	}bits;
}__imgehc_deu_dcti_reg_t;

typedef union   
{
	__u32 dwval;
	struct
	{
		__u32	lp_en			:1;
		__u32	r0				:7;
		__u32	tau				:5;
		__u32   r1				:3;
		__u32	alpha			:5;
		__u32   r2              :3;
		__u32   beta            :5;
		__u32	r3				:3;
	}bits;
}__imgehc_deu_lp0_reg_t;

typedef union   
{
	__u32 dwval;
	struct
	{
		__u32	lp_mode			:1;
		__u32   str				:1;
		__u32	r0				:6;
		__u32	corthr			:8;
		__u32	neggain			:2;
		__u32   r1              :2;
		__u32   delta           :2;
		__u32   r2              :2;
		__u32	limit_thr		:8;
	}bits;
}__imgehc_deu_lp1_reg_t;

typedef union   
{
	__u32 dwval;
	struct
	{
		__u32	straddr			:32;//bit0~bit31	
	}bits;
}__imgehc_deu_lp_straddr_reg_t;

typedef union   
{
	__u32 dwval;
	struct
	{
		__u32	wle_en			:1;
		__u32	r0				:7;
		__u32	wle_thr			:8;
		__u32	wle_gain		:8;
		__u32   r1              :8;
	}bits;
}__imgehc_deu_wle_reg_t;

typedef union   
{
	__u32 dwval;
	struct
	{
		__u32	ble_en			:1;
		__u32	r0				:7;
		__u32	ble_thr			:8;
		__u32	ble_gain		:8;
		__u32   r1              :8;
	}bits;
}__imgehc_deu_ble_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_g_coff		:13;//bit0~bit12
		__u32	r0				:19;//bit13~bit31
	}bits;
}__imgehc_deu_cscgcoff_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_g_con		:14;//bit0~bit13
		__u32	r0				:18;//bit14~bit31
	}bits;
}__imgehc_deu_cscgcon_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_r_coff		:13;//bit0~bit12
		__u32	r0				:19;//bit13~bit31
	}bits;
}__imgehc_deu_cscrcoff_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_r_con		:14;//bit0~bit13
		__u32	r0				:18;//bit14~bit31
	}bits;
}__imgehc_deu_cscrcon_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_b_coff		:13;//bit0~bit12
		__u32	r0				:19;//bit13~bit31
	}bits;
}__imgehc_deu_cscbcoff_reg_t;

typedef union
{
	__u32 dwval;
	struct
	{
		__u32	csc_b_con		:14;//bit0~bit13
		__u32	r0				:18;//bit14~bit31
	}bits;
}__imgehc_deu_cscbcon_reg_t;

typedef struct 
{
	__imgehc_deu_en_reg_t			en;	
	__imgehc_deu_psize_reg_t		psize;
	__imgehc_deu_pwp0_reg_t			pwp0;
	__imgehc_deu_pwp1_reg_t			pwp1;
	__imgehc_deu_dcti_reg_t			dcti;
	__imgehc_deu_lp0_reg_t			lp0;
	__imgehc_deu_lp1_reg_t			lp1;
	__imgehc_deu_lp_straddr_reg_t   straddr;//0x1c
	__u32							r0[2];
	__imgehc_deu_wle_reg_t			wle;
	__imgehc_deu_ble_reg_t			ble;
	__imgehc_deu_cscgcoff_reg_t		cscgcoff[3];//0x30~0x38
	__imgehc_deu_cscgcon_reg_t		cscgcon;
	__imgehc_deu_cscrcoff_reg_t		cscrcoff[3];//0x40~0x48
	__imgehc_deu_cscrcon_reg_t		cscrcon;
	__imgehc_deu_cscbcoff_reg_t		cscbcoff[3];//0x50~0x58
	__imgehc_deu_cscbcon_reg_t		cscbcon;
}__iep_deu_dev_t;

#define ____SEPARATOR_GLOBAL____
__s32 DEU_EBIOS_Set_Reg_Base(__u32 sel, __u32 base);
__u32 DEU_EBIOS_Get_Reg_Base(__u32 sel);
__s32 DEU_EBIOS_Enable(__u32 sel, __u32 en);
__s32 DEU_EBIOS_Csc_Enable(__u32 sel, __u32 en);
__s32 DEU_EBIOS_Set_Csc_Coeff(__u32 sel, __u32 mode);
__s32 DEU_EBIOS_Set_Display_Size(__u32 sel, __u32 width, __u32 height);
__s32 DEU_EBIOS_Set_Win_Para(__u32 sel, __u32 top, __u32 bot, __u32 left, __u32 right);
__s32 DEU_EBIOS_Cfg_Rdy(__u32 sel);
__s32 DEU_EBIOS_Set_Output_Chnl(__u32 sel, __u32 be_sel);
#define ____SEPARATOR_LP____
__s32 DEU_EBIOS_LP_STR_Cfg_Rdy(__u32 sel);
__s32 DEU_EBIOS_LP_Set_Mode(__u32 sel, __u32 en_2d);
__s32 DEU_EBIOS_LP_STR_Enable(__u32 sel, __u32 en);
__s32 DEU_EBIOS_LP_Set_STR_Addr(__u32 sel, __u32 address);
__s32 DEU_EBIOS_LP_Set_Para(__u32 sel, __u32 level, __u32 filtertype, __u8 *pttab);
__s32 DEU_EBIOS_LP_Enable(__u32 sel, __u32 en);
#define ____SEPARATOR_DCTI____
__s32 DEU_EBIOS_DCTI_Set_Para(__u32 sel, __u32 level);
__s32 DEU_EBIOS_DCTI_Enable(__u32 sel, __u32 en);
#define ____SEPARATOR_WLE_BLE____
__s32 DEU_EBIOS_WLE_Set_Para(__u32 sel, __u32 level);
__s32 DEU_EBIOS_WLE_Enable(__u32 sel, __u32 en);
__s32 DEU_EBIOS_BLE_Set_Para(__u32 sel, __u32 level);
__s32 DEU_EBIOS_BLE_Enable(__u32 sel, __u32 en);
#endif
