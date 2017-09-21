#ifndef __DISP_CLK_H__
#define __DISP_CLK_H__

#include "disp_display_i.h"

#define  MOD_CLK_AHB_TVE0  0
#define  MOD_CLK_AHB_TVE1  0
#define  AHB_CLK_TVE0  0
#define  AHB_CLK_TVE1  0

typedef struct
{
	__u32 tve_clk;
	__u32 pre_scale;//required divide LCDx_ch1_clk2 by 2 for LCDx_ch1_clk1 or NOT: 1:not divided , 2: divide by two
	__u32 hdmi_clk;
	__u32 pll_clk;
	__u32 pll_2x;
}__disp_tv_vga_clk_t;

typedef struct
{
	__disp_tv_vga_clk_t tv_clk_tab[30];
	__disp_tv_vga_clk_t vga_clk_tab[12];//number related to number of vga mode supported
}__disp_clk_tab;

typedef struct
{
	__u32 factor_n;
	__u32 factor_k;
	__u32 divider_m;
}__disp_ccmu_coef;


typedef struct __CCMU_MIPI_PLL_REG0040
{
	__u32   FactorM:4;         
	__u32   FactorK:2;         
	__u32   reserved0:2;       
	__u32   FactorN:4;         
	__u32   reserved1:4;       
	__u32   VfbSel:1;          
	__u32   FeedBackDiv:1;     
	__u32   reserved2:2;       
	__u32   SdmEn:1;           
	__u32   PllSrc:1;          
	__u32   Ldo2En:1;          
	__u32   Ldo1En:1;          
	__u32   reserved3:1;       
	__u32   Sel625Or750:1;     
	__u32   SDiv2:1;           
	__u32   FracMode:1;        
	__u32   Lock:1;            
	__u32   reserved4:2;       
	__u32   PLLEn:1;           
} __ccmu_mipi_pll_reg0040_t;

typedef struct __CCMU_MIPI_PLL_BIAS_REG0240
{
	__u32  reserved0:28;          
	__u32  pllvdd_ldo_out_ctrl:3; 
	__u32  vco_rst:31;            
}__ccmu_mipi_pll_bias_reg0240_t;

__s32 image_clk_init(__u32 screen_id);
__s32 image_clk_exit(__u32 screen_id);
__s32 image_clk_on(__u32 screen_id, __u32 type);
__s32 image_clk_off(__u32 screen_id, __u32 type);

__s32 scaler_clk_init(__u32 scaler_id);
__s32 scaler_clk_exit(__u32 scaler_id);
__s32 scaler_clk_on(__u32 scaler_id);
__s32 scaler_clk_off(__u32 scaler_id);

__s32 lcdc_clk_init(__u32 screen_id);
__s32 lcdc_clk_exit(__u32 screen_id);
__s32 lcdc_clk_on(__u32 screen_id, __u32 tcon_index, __u32 type);
__s32 lcdc_clk_off(__u32 screen_id);

__s32 tve_clk_init(__u32 screen_id);
__s32 tve_clk_exit(__u32 screen_id);
__s32 tve_clk_on(__u32 screen_id);
__s32 tve_clk_off(__u32 screen_id);

__s32 hdmi_clk_init(void);
__s32 hdmi_clk_exit(void);
__s32 hdmi_clk_on(void);
__s32 hdmi_clk_off(void);

__s32 lvds_clk_init(void);
__s32 lvds_clk_exit(void);
__s32 lvds_clk_on(void);
__s32 lvds_clk_off(void);

__s32 dsi_clk_init(void);
__s32 dsi_clk_exit(void);
__s32 dsi_clk_on(void);
__s32 dsi_clk_off(void);


__s32 disp_pll_init(void);
__s32 disp_clk_cfg(__u32 screen_id, __u32 type, __u8 mode);
__s32 disp_clk_adjust(__u32 screen_id, __u32 type);

extern __disp_clk_tab clk_tab;

#endif
