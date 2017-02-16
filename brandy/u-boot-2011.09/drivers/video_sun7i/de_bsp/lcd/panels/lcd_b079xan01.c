#include "lcd_b079xan01.h"
#include "panels.h"

static void LCD_power_on (__u32 sel);
static void LCD_power_off (__u32 sel);
static void LCD_bl_open (__u32 sel);
static void LCD_bl_close (__u32 sel);

static void LCD_panel_init (__u32 sel);
static void LCD_panel_exit (__u32 sel);

void spi_24bit_3wire (__u32 tx)
{
  __u8 i;
  
  spi_csx_set (0);
  
  for (i = 0; i < 24; i++) {
    sunxi_lcd_delay_us (1);
    spi_sck_set (0);
    sunxi_lcd_delay_us (1);
    if (tx & 0x800000)
    { spi_sdi_set (1); }
    else
    { spi_sdi_set (0); }
    sunxi_lcd_delay_us (1);
    spi_sck_set (1);
    sunxi_lcd_delay_us (1);
    tx <<= 1;
  }
  spi_sdi_set (1);
  sunxi_lcd_delay_us (1);
  spi_csx_set (1);
  sunxi_lcd_delay_us (3);
}
void lp079x01_init (__panel_para_t * info)
{
  ssd2828_rst (0);
  panel_rst (0);
  lcd_2828_pd (0);
  sunxi_lcd_delay_ms (20);
  ssd2828_rst (1);
  panel_rst (1);
  sunxi_lcd_delay_ms (50);
  
  spi_24bit_3wire (0x7000B1);
  spi_24bit_3wire (0x723240);
  
  spi_24bit_3wire (0x7000B2);
  spi_24bit_3wire (0x725078);
  
  spi_24bit_3wire (0x7000B3);
  spi_24bit_3wire (0x72243C);
  
  spi_24bit_3wire (0x7000B4);
  spi_24bit_3wire (0x720300);
  
  spi_24bit_3wire (0x7000B5);
  spi_24bit_3wire (0x720400);
  
  spi_24bit_3wire (0x7000B6);
  spi_24bit_3wire (0x72000B);
  
  spi_24bit_3wire (0x7000DE);
  spi_24bit_3wire (0x720003);
  
  spi_24bit_3wire (0x7000D6);
  spi_24bit_3wire (0x720005);
  
  spi_24bit_3wire (0x7000B9);
  spi_24bit_3wire (0x720000);
  
  spi_24bit_3wire (0x7000BA);
  spi_24bit_3wire (0x72C015);
  
  spi_24bit_3wire (0x7000BB);
  spi_24bit_3wire (0x720008);
  
  spi_24bit_3wire (0x7000B9);
  spi_24bit_3wire (0x720001);
  
  spi_24bit_3wire (0x7000c4);
  spi_24bit_3wire (0x720001);
  
  spi_24bit_3wire (0x7000B7);
  spi_24bit_3wire (0x720342);
  
  spi_24bit_3wire (0x7000B8);
  spi_24bit_3wire (0x720000);
  
  spi_24bit_3wire (0x7000BC);
  spi_24bit_3wire (0x720000);
  
  spi_24bit_3wire (0x700011);
  
  sunxi_lcd_delay_ms (200);
  spi_24bit_3wire (0x700029);
  
  sunxi_lcd_delay_ms (200);
  spi_24bit_3wire (0x7000B7);
  spi_24bit_3wire (0x72030b);
  
}


void lp079x01_exit (__panel_para_t * info)
{
  spi_24bit_3wire (0x7000B7);
  spi_24bit_3wire (0x720342);
  
  sunxi_lcd_delay_ms (50);
  spi_24bit_3wire (0x700028);
  sunxi_lcd_delay_ms (10);
  spi_24bit_3wire (0x700010);
  sunxi_lcd_delay_ms (20);
  ssd2828_rst (0);
}

static void LCD_cfg_panel_info (__panel_extend_para_t * info)
{
  __u32 i = 0, j = 0;
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
    {0    , 0  },
    {15   , 3  },
    {30   , 6  },
    {45   , 9  },
    {60   , 12  },
    {75   , 16  },
    {90   , 22  },
    {105   , 28 },
    {120  , 36 },
    {135  , 44 },
    {150  , 54 },
    {165  , 67 },
    {180  , 84 },
    {195  , 108},
    {210  , 137},
    {225 , 171},
    {240 , 210},
    {255 , 255},
  };
  
  __u32 lcd_cmap_tbl[2][3][4] = {
    {
      {LCD_CMAP_G0, LCD_CMAP_B1, LCD_CMAP_G2, LCD_CMAP_B3},
      {LCD_CMAP_B0, LCD_CMAP_R1, LCD_CMAP_B2, LCD_CMAP_R3},
      {LCD_CMAP_R0, LCD_CMAP_G1, LCD_CMAP_R2, LCD_CMAP_G3},
    },
    {
      {LCD_CMAP_B3, LCD_CMAP_G2, LCD_CMAP_B1, LCD_CMAP_G0},
      {LCD_CMAP_R3, LCD_CMAP_B2, LCD_CMAP_R1, LCD_CMAP_B0},
      {LCD_CMAP_G3, LCD_CMAP_R2, LCD_CMAP_G1, LCD_CMAP_R0},
    },
  };
  
  memset (info, 0, sizeof (__panel_extend_para_t) );
  
  items = sizeof (lcd_gamma_tbl) / 2;
  for (i = 0; i < items - 1; i++) {
    __u32 num = lcd_gamma_tbl[i + 1][0] - lcd_gamma_tbl[i][0];
    
    for (j = 0; j < num; j++) {
      __u32 value = 0;
      
      value = lcd_gamma_tbl[i][1] + ( (lcd_gamma_tbl[i + 1][1] - lcd_gamma_tbl[i][1]) * j) / num;
      info->lcd_gamma_tbl[lcd_gamma_tbl[i][0] + j] = (value << 16) + (value << 8) + value;
    }
  }
  info->lcd_gamma_tbl[255] = (lcd_gamma_tbl[items - 1][1] << 16) + (lcd_gamma_tbl[items - 1][1] << 8) + lcd_gamma_tbl[items - 1][1];
  
  items = sizeof (lcd_bright_curve_tbl) / 2;
  for (i = 0; i < items - 1; i++) {
    __u32 num = lcd_bright_curve_tbl[i + 1][0] - lcd_bright_curve_tbl[i][0];
    
    for (j = 0; j < num; j++) {
      __u32 value = 0;
      
      value = lcd_bright_curve_tbl[i][1] + ( (lcd_bright_curve_tbl[i + 1][1] - lcd_bright_curve_tbl[i][1]) * j) / num;
      info->lcd_bright_curve_tbl[lcd_bright_curve_tbl[i][0] + j] = value;
    }
  }
  info->lcd_bright_curve_tbl[255] = lcd_bright_curve_tbl[items - 1][1];
  
  memcpy (info->lcd_cmap_tbl, lcd_cmap_tbl, sizeof (lcd_cmap_tbl) );
  
}

static __s32 LCD_open_flow (__u32 sel)
{
  LCD_OPEN_FUNC (sel, LCD_power_on, 0); 
  LCD_OPEN_FUNC (sel, LCD_panel_init, 10); 
  LCD_OPEN_FUNC (sel, sunxi_lcd_tcon_enable, 50);   
  LCD_OPEN_FUNC (sel, LCD_bl_open, 0);   
  
  return 0;
}

static __s32 LCD_close_flow (__u32 sel)
{
  LCD_CLOSE_FUNC (sel, LCD_bl_close, 0);     
  LCD_CLOSE_FUNC (sel, sunxi_lcd_tcon_disable, 0);       
  LCD_CLOSE_FUNC (sel, LCD_panel_exit, 100); 
  LCD_CLOSE_FUNC (sel, LCD_power_off, 100); 
  
  return 0;
}

static void LCD_power_on (__u32 sel)
{
  sunxi_lcd_power_enable (sel, 0);
  sunxi_lcd_pin_cfg (sel, 1);
}

static void LCD_power_off (__u32 sel)
{
  sunxi_lcd_pin_cfg (sel, 0);
  sunxi_lcd_power_disable (sel, 0);
}

static void LCD_bl_open (__u32 sel)
{
  sunxi_lcd_pwm_enable (sel);
  sunxi_lcd_backlight_enable (sel);
}

static void LCD_bl_close (__u32 sel)
{
  sunxi_lcd_backlight_disable (sel);
  sunxi_lcd_pwm_disable (sel);
}

static void LCD_panel_init (__u32 sel)
{
  __panel_para_t * info = OSAL_malloc (sizeof (__panel_para_t) );
  
  sunxi_lcd_get_panel_para (sel, info);
  lp079x01_init (info);
  OSAL_free (info);
  return;
}

static void LCD_panel_exit (__u32 sel)
{
  __panel_para_t * info = OSAL_malloc (sizeof (__panel_para_t) );
  
  sunxi_lcd_get_panel_para (sel, info);
  lp079x01_exit (info);
  OSAL_free (info);
  return;
}

__lcd_panel_t b079xan01_panel = {
  /* panel driver name, must mach the name of lcd_drv_name in sys_config.fex */
  .name = "b079xan01",
  .func = {
    .cfg_panel_info = LCD_cfg_panel_info,
    .cfg_open_flow = LCD_open_flow,
    .cfg_close_flow = LCD_close_flow,
    /*.lcd_user_defined_func = LCD_user_defined_func,*/
  },
};


