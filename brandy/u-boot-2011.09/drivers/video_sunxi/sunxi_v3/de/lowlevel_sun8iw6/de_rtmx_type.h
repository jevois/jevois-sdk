#ifndef __DE_RTMX_TYPE_H__
#define __DE_RTMX_TYPE_H__

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int rt_en			:1;
		unsigned int r0				:3;
		unsigned int finish_irq_en	:1;
		unsigned int error_irq_en	:1;
		unsigned int r1				:2;
		unsigned int sync_rev		:1;
		unsigned int flied_rev		:1;
		unsigned int r2				:2;
		unsigned int rtwb_port		:2;
		unsigned int r3				:18;//bit14~31
	}bits;
}__glb_ctl_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int finish_irq		:1;
		unsigned int error_irq		:1;
		unsigned int r0				:2;
		unsigned int busy_status	:1;
		unsigned int error_status	:1;
		unsigned int r1				:2;
		unsigned int even_odd_flag	:1;
		unsigned int r2				:23;//bit9~31
	}bits;
}__glb_status_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int dbuff_rdy		:1;
		unsigned int r0				:31;//bit1~31
	}bits;
}__glb_dbuff_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int width			:13;//bit0~12
		unsigned int r0				:3;//bit13~15
		unsigned int height			:13;//bit16~28
		unsigned int r1				:3;//bit29~31
	}bits;
}__glb_size_reg_t;

typedef struct
{
	__glb_ctl_reg_t				glb_ctl;		
	__glb_status_reg_t			glb_status;		
	__glb_dbuff_reg_t			glb_dbuff;		
	__glb_size_reg_t			glb_size;		
}__glb_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_en			:1;//bit0
		unsigned int r0				:3;//bit1~3
		unsigned int lay_fcolor_en	:1;//bit4
		unsigned int r1				:3;//bit5~7
		unsigned int lay_fmt		:5;//bit8~12
		unsigned int r2				:2;//bit13~14
		unsigned int ui_sel			:1;//bit15
		unsigned int r3				:7;//bit16~22
		unsigned int lay_top_down	:1;//bit23
		unsigned int r4				:8;//bit24~31
	}bits;
}__vi_lay_attr_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_width		:13;//bit0~12
		unsigned int r0				:3;//bit13~15
		unsigned int lay_height		:13;//bit16~28
		unsigned int r1				:3;//bit29~31
	}bits;
}__vi_lay_size_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_coorx		:16;//bit0~15
		unsigned int lay_coory		:16;//bit16~31
	}bits;
}__vi_lay_coor_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_pitch		:32;//bit0~31
	}bits;
}__vi_lay_pitch_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_top_laddr	:32;//bit0~31
	}bits;
}__vi_lay_top_laddr_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_bot_laddr	:32;//bit0~31
	}bits;
}__vi_lay_bot_laddr_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_vb			:8;//bit0~7
		unsigned int lay_ug			:8;//bit8~15
		unsigned int lay_yr			:8;//bit16~23
		unsigned int r0				:8;//bit24~31
	}bits;
}__vi_lay_fcolor_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay0_top_haddr	:8;//bit0~7
		unsigned int lay1_top_haddr	:8;//bit8~15
		unsigned int lay2_top_haddr	:8;//bit16~23
		unsigned int lay3_top_haddr	:8;//bit24~31
	}bits;
}__vi_lay_top_haddr_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay0_bot_haddr	:8;//bit0~7
		unsigned int lay1_bot_haddr	:8;//bit8~15
		unsigned int lay2_bot_haddr	:8;//bit16~23
		unsigned int lay3_bot_haddr	:8;//bit24~31
	}bits;
}__vi_lay_bot_haddr_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int ovl_width		:13;//bit0~12
		unsigned int r0				:3;//bit13~15
		unsigned int ovl_height		:13;//bit16~28
		unsigned int r1				:3;//bit29~31
	}bits;
}__vi_ovl_size_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int m				:14;//bit0~13
		unsigned int r0				:2;//bit14~15
		unsigned int n				:14;//bit16~29
		unsigned int r1				:2;//bit30~31
	}bits;
}__vi_hori_ds_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int m				:14;//bit0~13
		unsigned int r0				:2;//bit14~15
		unsigned int n				:14;//bit16~29
		unsigned int r1				:2;//bit30~31
	}bits;
}__vi_vert_ds_reg_t;

typedef struct
{
	__vi_lay_attr_reg_t			lay_attr;		
	__vi_lay_size_reg_t			lay_size;		
	__vi_lay_coor_reg_t			lay_coor;		
	__vi_lay_pitch_reg_t		lay_pitch[3];	
	__vi_lay_top_laddr_reg_t	lay_top_laddr[3];
	__vi_lay_bot_laddr_reg_t	lay_bot_laddr[3];
}__vi_lay_reg_t;

typedef struct
{
	__vi_lay_reg_t				vi_lay_cfg[4];	
	__vi_lay_fcolor_reg_t		vi_lay_fcolor[4];
	__vi_lay_top_haddr_reg_t	vi_lay_top_haddr[3];//0xd0~0xd8
	__vi_lay_bot_haddr_reg_t	vi_lay_bot_haddr[3];//0xdc~0xe4
	__vi_ovl_size_reg_t			vi_ovl_size[2];	
	__vi_hori_ds_reg_t			vi_hori_ds[2];	
	__vi_vert_ds_reg_t			vi_vert_ds[2];	
}__vi_ovl_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_en			:1;//bit0
		unsigned int lay_alpmod		:2;//bit1~2
		unsigned int r0				:1;//bit3
		unsigned int lay_fcolor_en	:1;//bit4
		unsigned int r1				:3;//bit5~7
		unsigned int lay_fmt		:5;//bit8~12
		unsigned int r2				:3;//bit13~15
		unsigned int lay_alpctl		:2;//bit16~17
		unsigned int r3				:5;//bit18~22
		unsigned int lay_top_down	:1;//bit23
		unsigned int lay_alpha		:8;//bit24~31
	}bits;
}__ui_lay_attr_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_width		:13;//bit0~12
		unsigned int r0				:3;//bit13~15
		unsigned int lay_height		:13;//bit16~28
		unsigned int r1				:3;//bit29~31
	}bits;
}__ui_lay_size_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_coorx		:16;//bit0~15
		unsigned int lay_coory		:16;//bit16~31
	}bits;
}__ui_lay_coor_reg_t;


typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_pitch		:32;//bit0~31
	}bits;
}__ui_lay_pitch_reg_t;


typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_top_laddr	:32;//bit0~31
	}bits;
}__ui_lay_top_laddr_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_bot_laddr	:32;//bit0~31
	}bits;
}__ui_lay_bot_laddr_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay_blue		:8;//bit0~7
		unsigned int lay_green		:8;//bit8~15
		unsigned int lay_red		:8;//bit16~23
		unsigned int lay_alpha		:8;//bit24~31
	}bits;
}__ui_lay_fcolor_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay0_top_haddr	:8;//bit0~7
		unsigned int lay1_top_haddr	:8;//bit8~15
		unsigned int lay2_top_haddr	:8;//bit16~23
		unsigned int lay3_top_haddr	:8;//bit24~31
	}bits;
}__ui_lay_top_haddr_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int lay0_bot_haddr	:8;//bit0~7
		unsigned int lay1_bot_haddr	:8;//bit8~15
		unsigned int lay2_bot_haddr	:8;//bit16~23
		unsigned int lay3_bot_haddr	:8;//bit24~31
	}bits;
}__ui_lay_bot_haddr_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int ovl_width		:13;//bit0~12
		unsigned int r0				:3;//bit13~15
		unsigned int ovl_height		:13;//bit16~28
		unsigned int r1				:3;//bit29~31
	}bits;
}__ui_ovl_size_reg_t;

typedef struct
{
	__ui_lay_attr_reg_t			lay_attr;		
	__ui_lay_size_reg_t			lay_size;		
	__ui_lay_coor_reg_t			lay_coor;		
	__ui_lay_pitch_reg_t		lay_pitch;		
	__ui_lay_top_laddr_reg_t	lay_top_laddr;	
	__ui_lay_bot_laddr_reg_t	lay_bot_laddr;	
	__ui_lay_fcolor_reg_t		lay_fcolor;		
	unsigned int				r0;				
}__ui_lay_reg_t;

typedef struct
{
	__ui_lay_reg_t				ui_lay_cfg[4];	
	__ui_lay_top_haddr_reg_t	ui_lay_top_haddr;
	__ui_lay_bot_haddr_reg_t	ui_lay_bot_haddr;
	__ui_ovl_size_reg_t			ui_ovl_size;	
}__ui_ovl_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int p0_fcolor_en	:1;//bit0
		unsigned int p1_fcolor_en	:1;//bit1
		unsigned int p2_fcolor_en	:1;//bit2
		unsigned int p3_fcolor_en	:1;//bit3
		unsigned int p4_fcolor_en	:1;//bit4
		unsigned int r0				:3;//bit5~7
		unsigned int p0_en			:1;//bit8
		unsigned int p1_en			:1;//bit9
		unsigned int p2_en			:1;//bit10
		unsigned int p3_en			:1;//bit11
		unsigned int p4_en			:1;//bit12
		unsigned int r1				:19;//bit13~31
	}bits;
}__bld_fcolor_ctl_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int blue			:8;//bit0~7
		unsigned int green			:8;//bit8~15
		unsigned int red			:8;//bit16~23
		unsigned int alpha			:8;//bit24~31
	}bits;
}__bld_fcolor_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int width			:13;//bit0~12
		unsigned int r0				:3;//bit13~15
		unsigned int height			:13;//bit16~28
		unsigned int r1				:3;//bit29~31
	}bits;
}__bld_isize_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int coorx			:16;//bit0~15
		unsigned int coory			:13;//bit16~31
	}bits;
}__bld_offset_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int ch0_routr_ctl	:4;//bit0~3
		unsigned int ch1_routr_ctl	:4;//bit4~7
		unsigned int ch2_routr_ctl	:4;//bit8~11
		unsigned int ch3_routr_ctl	:4;//bit12~15
		unsigned int ch4_routr_ctl	:4;//bit16~19
		unsigned int r0				:12;//bit20~31
	}bits;
}__bld_route_ctl_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int p0_alpha_mode	:1;//bit0
		unsigned int p1_alpha_mode	:1;//bit1
		unsigned int p2_alpha_mode	:1;//bit2
		unsigned int p3_alpha_mode	:1;//bit3
		unsigned int p4_alpha_mode	:1;//bit4
		unsigned int r0				:27;//bit5~31
	}bits;
}__bld_premultiply_ctl_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int blue			:8;//bit0~7
		unsigned int green			:8;//bit8~15
		unsigned int red			:8;//bit16~23
		unsigned int alpha			:8;//bit24~31
	}bits;
}__bld_bkcolor_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int width			:13;//bit0~12
		unsigned int r0				:3;//bit13~15
		unsigned int height			:13;//bit16~28
		unsigned int r1				:3;//bit29~31
	}bits;
}__bld_output_size_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int pixel_fs		:4;//bit0~3
		unsigned int r0				:4;//bit4~7
		unsigned int pixel_fd		:4;//bit8~11
		unsigned int r1				:4;//bit12~15
		unsigned int alpha_fs		:4;//bit16~19
		unsigned int r2				:4;//bit20~23
		unsigned int alpha_fd		:4;//bit24~27
		unsigned int r3				:4;//bit28~31
	}bits;
}__bld_ctl_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int key0_en		:1;//bit0
		unsigned int key0_dir		:2;//bit1~2
		unsigned int r0				:1;//bit3
		unsigned int key1_en		:1;//bit4
		unsigned int key1_dir		:2;//bit5~6
		unsigned int r1				:1;//bit7
		unsigned int key2_en		:1;//bit8
		unsigned int key2_dir		:2;//bit9~10
		unsigned int r2				:1;//bit11
		unsigned int key3_en		:1;//bit12
		unsigned int key3_dir		:2;//bit13~14
		unsigned int r3				:17;//bit15~31
	}bits;
}__bld_colorkey_ctl_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int key0_blue		:1;//bit0
		unsigned int key0_green		:1;//bit1
		unsigned int key0_red		:1;//bit2
		unsigned int r0				:5;//bit3~7
		unsigned int key1_blue		:1;//bit8
		unsigned int key1_green		:1;//bit9
		unsigned int key1_red		:1;//bit10
		unsigned int r1				:5;//bit11~15
		unsigned int key2_blue		:1;//bit16
		unsigned int key2_green		:1;//bit17
		unsigned int key2_red		:1;//bit18
		unsigned int r2				:5;//bit19~23
		unsigned int key3_blue		:1;//bit24
		unsigned int key3_green		:1;//bit25
		unsigned int key3_red		:1;//bit26
		unsigned int r3				:5;//bit27~31
	}bits;
}__bld_colorkey_cfg_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int max_blue		:8;//bit0~7
		unsigned int max_green		:8;//bit8~15
		unsigned int max_red		:8;//bit16~23
		unsigned int r0				:8;//bit24~31
	}bits;
}__bld_colorkey_max_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int min_blue		:8;//bit0~7
		unsigned int min_green		:8;//bit8~15
		unsigned int min_red		:8;//bit16~23
		unsigned int r0				:8;//bit24~31
	}bits;
}__bld_colorkey_min_reg_t;

typedef union
{
	unsigned int dwval;
	struct
	{
		unsigned int premultiply_en	:1;//bit0
		unsigned int interlace_en	:1;//bit1
		unsigned int r0				:30;//bit2~31
	}bits;
}__bld_out_color_ctl_reg_t;

typedef struct
{
	__bld_fcolor_reg_t			fcolor;	
	__bld_isize_reg_t			insize;	
	__bld_offset_reg_t			offset;	
	unsigned int				r0;		
}__bld_pipe_reg_t;

typedef struct
{
	__bld_fcolor_ctl_reg_t		bld_fcolor_ctl;	
	__bld_pipe_reg_t			bld_pipe_attr[5];
	unsigned int				r0[11];			
	__bld_route_ctl_reg_t		bld_route_ctl;	
	__bld_premultiply_ctl_reg_t	bld_premultiply_ctl;//0x84
	__bld_bkcolor_reg_t			bld_bkcolor;	
	__bld_output_size_reg_t		bld_output_size;
	__bld_ctl_reg_t				bld_mode[4];	
	unsigned int				r1[4];			
	__bld_colorkey_ctl_reg_t	bld_ck_ctl;		
	__bld_colorkey_cfg_reg_t	bld_ck_cfg;		
	unsigned int				r2[2];			
	__bld_colorkey_max_reg_t	bld_ck_max[4];	
	unsigned int				r3[4];			
	__bld_colorkey_min_reg_t	bld_ck_min[4];	
	unsigned int				r4[3];			
	__bld_out_color_ctl_reg_t	bld_out_ctl;	
}__bld_reg_t;

typedef struct
{
	__glb_reg_t					*glb_ctl;	
	__bld_reg_t					*bld_ctl;	
	__vi_ovl_reg_t			*vi_ovl[VI_CHN_NUM];	
	__ui_ovl_reg_t			*ui_ovl[UI_CHN_NUM];	
}__rtmx_reg_t;

#endif
