#include "lcd_edp_anx9804.h"
#include "panels.h"


static void LCD_power_on(__u32 sel);
static void LCD_power_off(__u32 sel);
static void LCD_bl_open(__u32 sel);
static void LCD_bl_close(__u32 sel);

static void LCD_panel_init(__u32 sel);
static void LCD_panel_exit(__u32 sel);
void anx9804_init(__panel_para_t * info)
{
	__u8 c;
	__s32 i;
	__u32 count = 0;
	__u32 lanes;
	__u32 data_rate;
	__u32 colordepth;

	lanes = info->lcd_edp_tx_lane;
	data_rate = 0x06;
	if(info->lcd_edp_tx_rate == 1) {
		data_rate = 0x06;//1.62G
	}	else if(info->lcd_edp_tx_rate == 2) {
		data_rate = 0x0a;//2.7G
	}

	colordepth = (info->lcd_edp_colordepth == 1)? 0x00:0x10;//0x00: 6bit;  0x10:8bit

	sunxi_lcd_iic_write(0x72, DP_TX_RST_CTRL_REG, DP_TX_RST_HW_RST);
	sunxi_lcd_delay_ms(10);
	sunxi_lcd_iic_write(0x72, DP_TX_RST_CTRL_REG, 0x00);
	sunxi_lcd_iic_write(0x72, DP_POWERD_CTRL_REG, 0x00 );

	sunxi_lcd_iic_read(0x72, DP_TX_DEV_IDH_REG , &c);
	if(c==0x98) {
		OSAL_PRINTF("ANX9804 Chip found\n");
	}	else {
		OSAL_PRINTF("ANX9804 Chip not found\n");
	}

#if 0
	for(i=0;i<50;i++)
	{
		sunxi_lcd_iic_read(0x70, DP_TX_SYS_CTRL1_REG, &c);
		sunxi_lcd_iic_write(0x70, DP_TX_SYS_CTRL1_REG, c);
		sunxi_lcd_iic_read(0x70, DP_TX_SYS_CTRL1_REG, &c);
		if((c&DP_TX_SYS_CTRL1_DET_STA)!=0) {
			OSAL_PRINTF("ANX9804 clock is detected.\n");
			break;
		}

		sunxi_lcd_delay_ms(10);
	}
#endif

	for(i=0;i<50;i++) {
		sunxi_lcd_iic_read(0x70, DP_TX_SYS_CTRL2_REG, &c);
		sunxi_lcd_iic_write(0x70, DP_TX_SYS_CTRL2_REG, c);
		sunxi_lcd_iic_read(0x70, DP_TX_SYS_CTRL2_REG, &c);
		if((c&DP_TX_SYS_CTRL2_CHA_STA)==0) {
			OSAL_PRINTF("ANX9804 clock is stable.\n");
			break;
		}
		sunxi_lcd_delay_ms(10);
	}

	sunxi_lcd_iic_write(0x72, DP_TX_VID_CTRL2_REG, colordepth);

	sunxi_lcd_iic_write(0x70, DP_TX_PLL_CTRL_REG, 0x07);
	sunxi_lcd_iic_write(0x72, DP_TX_PLL_FILTER_CTRL3, 0x19);
	sunxi_lcd_iic_write(0x72, DP_TX_PLL_CTRL3, 0xd9);


	sunxi_lcd_iic_write(0x72, DP_TX_RST_CTRL2_REG, 0x40);


	sunxi_lcd_iic_write(0x72, ANALOG_DEBUG_REG1, 0xf0);
	sunxi_lcd_iic_write(0x72, ANALOG_DEBUG_REG3, 0x99);
	sunxi_lcd_iic_write(0x72, DP_TX_PLL_FILTER_CTRL1, 0x7b);
	sunxi_lcd_iic_write(0x70, DP_TX_LINK_DEBUG_REG, 0x30);
	sunxi_lcd_iic_write(0x72, DP_TX_PLL_FILTER_CTRL, 0x06);

	sunxi_lcd_iic_write(0x70, DP_TX_SYS_CTRL3_REG, 0x30);
	sunxi_lcd_iic_write(0x70, 0xc8, 0x00);
	sunxi_lcd_iic_write(0x70, 0xa3, 0x00);
	sunxi_lcd_iic_write(0x70, 0xa4, 0x00);
	sunxi_lcd_iic_write(0x70, 0xa5, 0x00);
	sunxi_lcd_iic_write(0x70, 0xa6, 0x00);

#if 0
	sunxi_lcd_iic_write(0x70,  0xE4,  0x80);

	sunxi_lcd_iic_write(0x70, 0xE5,  0x19);

	sunxi_lcd_iic_write(0x70,  0xE6,  0x01);
	sunxi_lcd_iic_write(0x70,  0xE7,  0x00);
	sunxi_lcd_iic_write(0x70,  0xE8,  0x00);

	sunxi_lcd_iic_write(0x70,  0xE9, 0x01);

	for(i=0; i<50; i++) {
		sunxi_lcd_iic_read(0x70,  0xE9,  &c);
		if(c==0x00)	{
			break;
		}
	}

	sunxi_lcd_iic_write(  0x70,  0xF0,   &max_bandwidth);
	sunxi_lcd_iic_write(  0x70,  0xF1,   &max_lanes);
	debug_pr__s32f("max_bandwidth = %.2x, max_lanes = %.2x\n", (WORD)max_bandwidth, (WORD)max_lanes);
#endif

	sunxi_lcd_iic_write(0x72,  DP_TX_RST_CTRL2_REG,  0x44);
	sunxi_lcd_iic_write(0x72,  DP_TX_RST_CTRL2_REG,  0x40);

	sunxi_lcd_iic_write(0x72, DP_POWERD_CTRL_REG, 0x10 );//audio power down
	sunxi_lcd_iic_write(0x70, DP_TX_HDCP_CONTROL_0_REG, 0x00 );
	sunxi_lcd_iic_write(0x70, 0xA7, 0x00 );//Spread spectrum 30 kHz

	/* enable ssc function */
	sunxi_lcd_iic_write(0x70, 0xa7, 0x00);                  
	sunxi_lcd_iic_write(0x70, 0xa0, 0x00);                  
	sunxi_lcd_iic_write(0x72, 0xde, 0x99);                  
	sunxi_lcd_iic_read(0x70, 0xc7, &c);                     
	sunxi_lcd_iic_write(0x70, 0xc7, c & (~0x40));
	sunxi_lcd_iic_read(0x70, 0xd8, &c);                     
	sunxi_lcd_iic_write(0x70, 0xd8, (c | 0x01));
	sunxi_lcd_iic_write(0x70, 0xc7, 0x02);                  
	sunxi_lcd_iic_write(0x70, 0xd0, 0xb8);                  
	sunxi_lcd_iic_write(0x70, 0xd1, 0x6D);                  
	sunxi_lcd_iic_write(0x70, 0xa7, 0x10);                  
	sunxi_lcd_iic_read(0x72, 0x07, &c);                     
	sunxi_lcd_iic_write(0x72, 0x07, c | 0x80);
	sunxi_lcd_iic_write(0x72, 0x07, c & (~0x80));

	sunxi_lcd_iic_write(0x70, DP_TX_LINK_BW_SET_REG, data_rate);

	sunxi_lcd_iic_write(0x70, DP_TX_LANE_COUNT_SET_REG, lanes);

	sunxi_lcd_iic_write(0x70, DP_TX_LINK_TRAINING_CTRL_REG, DP_TX_LINK_TRAINING_CTRL_EN);
	sunxi_lcd_delay_ms(5);
	sunxi_lcd_iic_read(0x70, DP_TX_LINK_TRAINING_CTRL_REG, &c);
	while((c&0x01)!=0) {
		OSAL_PRINTF("ANX9804 Waiting...\n");
		sunxi_lcd_delay_ms(5);
		count ++;
		if(count > 100) {
			OSAL_PRINTF("ANX9804 Link training fail\n");
			break;
		}
		sunxi_lcd_iic_read(0x70, DP_TX_LINK_TRAINING_CTRL_REG, &c);
	}




	sunxi_lcd_iic_write(0x72, 0x08, 0x81);

	sunxi_lcd_iic_write(0x70, 0x82, 0x33);
}

static void LCD_cfg_panel_info(__panel_extend_para_t * info)
{
	__u32 i = 0, j=0;
	__u32 items;
	__u8 lcd_gamma_tbl[][2] =
	{
		{0, 0},
		{15, 15},
		{30, 30},
		{45, 45},
		{60, 60},
		{75, 75},
		{90, 90},
		{105, 105},
		{120, 120},
		{135, 135},
		{150, 150},
		{165, 165},
		{180, 180},
		{195, 195},
		{210, 210},
		{225, 225},
		{240, 240},
		{255, 255},
	};

	__u8 lcd_bright_curve_tbl[][2] =
	{
		{0    ,0  },//0
		{15   ,3  },//0
		{30   ,6  },//0
		{45   ,9  },// 1
		{60   ,12  },// 2
		{75   ,16  },// 5
		{90   ,22  },//9
		{105   ,28 },
		{120  ,36 },//23
		{135  ,44 },//33
		{150  ,54 },
		{165  ,67 },
		{180  ,84 },
		{195  ,108},
		{210  ,137},
		{225 ,171},
		{240 ,210},
		{255 ,255},
	};

	__u32 lcd_cmap_tbl[2][3][4] = {
	{
		{LCD_CMAP_G0,LCD_CMAP_B1,LCD_CMAP_G2,LCD_CMAP_B3},
		{LCD_CMAP_B0,LCD_CMAP_R1,LCD_CMAP_B2,LCD_CMAP_R3},
		{LCD_CMAP_R0,LCD_CMAP_G1,LCD_CMAP_R2,LCD_CMAP_G3},
		},
		{
		{LCD_CMAP_B3,LCD_CMAP_G2,LCD_CMAP_B1,LCD_CMAP_G0},
		{LCD_CMAP_R3,LCD_CMAP_B2,LCD_CMAP_R1,LCD_CMAP_B0},
		{LCD_CMAP_G3,LCD_CMAP_R2,LCD_CMAP_G1,LCD_CMAP_R0},
		},
	};

	memset(info,0,sizeof(__panel_extend_para_t));

	items = sizeof(lcd_gamma_tbl)/2;
	for(i=0; i<items-1; i++) {
		__u32 num = lcd_gamma_tbl[i+1][0] - lcd_gamma_tbl[i][0];

		for(j=0; j<num; j++) {
			__u32 value = 0;

			value = lcd_gamma_tbl[i][1] + ((lcd_gamma_tbl[i+1][1] - lcd_gamma_tbl[i][1]) * j)/num;
			info->lcd_gamma_tbl[lcd_gamma_tbl[i][0] + j] = (value<<16) + (value<<8) + value;
		}
	}
	info->lcd_gamma_tbl[255] = (lcd_gamma_tbl[items-1][1]<<16) + (lcd_gamma_tbl[items-1][1]<<8) + lcd_gamma_tbl[items-1][1];

	items = sizeof(lcd_bright_curve_tbl)/2;
	for(i=0; i<items-1; i++) {
		__u32 num = lcd_bright_curve_tbl[i+1][0] - lcd_bright_curve_tbl[i][0];

		for(j=0; j<num; j++) {
			__u32 value = 0;

			value = lcd_bright_curve_tbl[i][1] + ((lcd_bright_curve_tbl[i+1][1] - lcd_bright_curve_tbl[i][1]) * j)/num;
			info->lcd_bright_curve_tbl[lcd_bright_curve_tbl[i][0] + j] = value;
		}
	}
	info->lcd_bright_curve_tbl[255] = lcd_bright_curve_tbl[items-1][1];

	memcpy(info->lcd_cmap_tbl, lcd_cmap_tbl, sizeof(lcd_cmap_tbl));

}

static __s32 LCD_open_flow(__u32 sel)
{
	LCD_OPEN_FUNC(sel, LCD_power_on, 0);  
	LCD_OPEN_FUNC(sel, LCD_panel_init, 10);  
	LCD_OPEN_FUNC(sel, sunxi_lcd_tcon_enable, 50);    
	LCD_OPEN_FUNC(sel, LCD_bl_open, 0);    

	return 0;
}

static __s32 LCD_close_flow(__u32 sel)
{
	LCD_CLOSE_FUNC(sel, LCD_bl_close, 0);      
	LCD_CLOSE_FUNC(sel, sunxi_lcd_tcon_disable, 0);        
	LCD_CLOSE_FUNC(sel, LCD_panel_exit,	100);  
	LCD_CLOSE_FUNC(sel, LCD_power_off, 100);  

	return 0;
}

static void LCD_power_on(__u32 sel)
{
	sunxi_lcd_power_enable(sel, 0);//config lcd_power pin to open lcd power0
	sunxi_lcd_pin_cfg(sel, 1);
}

static void LCD_power_off(__u32 sel)
{
	sunxi_lcd_pin_cfg(sel, 0);
	sunxi_lcd_power_disable(sel, 0);//config lcd_power pin to close lcd power0
}

static void LCD_bl_open(__u32 sel)
{
	sunxi_lcd_pwm_enable(sel);//open pwm module
	sunxi_lcd_backlight_enable(sel);//config lcd_bl_en pin to open lcd backlight
}

static void LCD_bl_close(__u32 sel)
{
	sunxi_lcd_backlight_disable(sel);//config lcd_bl_en pin to close lcd backlight
	sunxi_lcd_pwm_disable(sel);//close pwm module
}

static void LCD_panel_init(__u32 sel)
{
	__panel_para_t *info = OSAL_malloc(sizeof(__panel_para_t));

	sunxi_lcd_get_panel_para(sel, info);
	anx9804_init(info);
	OSAL_free(info);
	return;
}

static void LCD_panel_exit(__u32 sel)
{
	return ;
}

__lcd_panel_t edp_anx9804_panel = {
	/* panel driver name, must mach the name of lcd_drv_name in sys_config.fex */
	.name = "edp_anx9804",
	.func = {
		.cfg_panel_info = LCD_cfg_panel_info,
		.cfg_open_flow = LCD_open_flow,
		.cfg_close_flow = LCD_close_flow,
		/*.lcd_user_defined_func = LCD_user_defined_func,*/
	},
};

