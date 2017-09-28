#ifndef __PANEL_H__
#define __PANEL_H__
#include "../de/bsp_display.h"
#include "lcd_source.h"

extern void LCD_OPEN_FUNC(u32 sel, LCD_FUNC func, u32 delay/*ms*/);
extern void LCD_CLOSE_FUNC(u32 sel, LCD_FUNC func, u32 delay/*ms*/);

typedef struct{
	char name[32];
	disp_lcd_panel_fun func;
}__lcd_panel_t;

extern __lcd_panel_t * panel_array[];

struct sunxi_lcd_drv
{
  struct sunxi_disp_source_ops      src_ops;
};

#if !defined(SUPPORT_DSI)
typedef enum
{
	DSI_DCS_ENTER_IDLE_MODE                         =       0x39 ,                 
	DSI_DCS_ENTER_INVERT_MODE                       =       0x21 ,                 
	DSI_DCS_ENTER_NORMAL_MODE                       =       0x13 ,                 
	DSI_DCS_ENTER_PARTIAL_MODE                      =       0x12 ,                 
	DSI_DCS_ENTER_SLEEP_MODE                        =       0x10 ,                 
	DSI_DCS_EXIT_IDLE_MODE                          =       0x38 ,                 
	DSI_DCS_EXIT_INVERT_MODE                        =       0x20 ,                 
	DSI_DCS_EXIT_SLEEP_MODE                         =       0x11 ,                 
	DSI_DCS_GET_ADDRESS_MODE                        =       0x0b ,                 
	DSI_DCS_GET_BLUE_CHANNEL                        =       0x08 ,                 
	DSI_DCS_GET_DIAGNOSTIC_RESULT           =       0x0f ,                 
	DSI_DCS_GET_DISPLAY_MODE                        =       0x0d ,                 
	DSI_DCS_GET_GREEN_CHANNEL                       =       0x07 ,                 
	DSI_DCS_GET_PIXEL_FORMAT                        =       0x0c ,                 
	DSI_DCS_GET_POWER_MODE                          =       0x0a ,                 
	DSI_DCS_GET_RED_CHANNEL                         =       0x06 ,                 
	DSI_DCS_GET_SCANLINE                            =       0x45 ,                 
	DSI_DCS_GET_SIGNAL_MODE                         =       0x0e ,                 
	DSI_DCS_NOP                                                     =       0x00 ,                 
	DSI_DCS_READ_DDB_CONTINUE                       =       0xa8 ,                 
	DSI_DCS_READ_DDB_START                          =       0xa1 ,                 
	DSI_DCS_READ_MEMORY_CONTINUE            =       0x3e ,                 
	DSI_DCS_READ_MEMORY_START                       =       0x2e ,                 
	DSI_DCS_SET_ADDRESS_MODE                        =       0x36 ,                 
	DSI_DCS_SET_COLUMN_ADDRESS                      =       0x2a ,                 
	DSI_DCS_SET_DISPLAY_OFF                         =       0x28 ,                 
	DSI_DCS_SET_DISPLAY_ON                          =       0x29 ,                 
	DSI_DCS_SET_GAMMA_CURVE                         =       0x26 ,                 
	DSI_DCS_SET_PAGE_ADDRESS                        =       0x2b ,                 
	DSI_DCS_SET_PARTIAL_AREA                        =       0x30 ,                 
	DSI_DCS_SET_PIXEL_FORMAT                        =       0x3a ,                 
	DSI_DCS_SET_SCROLL_AREA                         =       0x33 ,                 
	DSI_DCS_SET_SCROLL_START                        =       0x37 ,                 
	DSI_DCS_SET_TEAR_OFF                            =       0x34 ,                 
	DSI_DCS_SET_TEAR_ON                                     =       0x35 ,                 
	DSI_DCS_SET_TEAR_SCANLINE                       =       0x44 ,                 
	DSI_DCS_SOFT_RESET                                      =       0x01 ,                 
	DSI_DCS_WRITE_LUT                                       =       0x2d ,                 
	DSI_DCS_WRITE_MEMORY_CONTINUE           =       0x3c ,                 
	DSI_DCS_WRITE_MEMORY_START                      =       0x2c ,                 
}__dsi_dcs_t;
#endif

extern int sunxi_disp_get_source_ops(struct sunxi_disp_source_ops *src_ops);
int lcd_init(void);

#endif
