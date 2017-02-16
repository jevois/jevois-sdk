/*
 * drivers/media/video/sun4i_csi/device/hi253.c
 *
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * A V4L2 driver for Hynix HI253 cameras.
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/videodev2.h>
#include <linux/clk.h>
#include <media/v4l2-device.h>
#include <media/v4l2-chip-ident.h>
#include <media/v4l2-mediabus.h>//linux-3.0
#include <linux/io.h>
#include <plat/sys_config.h>
#include <linux/regulator/consumer.h>
#include <mach/system.h>
#include "../include/sun4i_csi_core.h"
#include "../include/sun4i_dev_csi.h"

MODULE_AUTHOR ("raymonxiu");
MODULE_DESCRIPTION ("A low-level driver for Hynix HI253 sensors");
MODULE_LICENSE ("GPL");

#define DEV_DBG_EN      0
#if(DEV_DBG_EN == 1)
#define csi_dev_dbg(x,arg...) printk(KERN_INFO"[CSI_DEBUG][HI253]"x,##arg)
#else
#define csi_dev_dbg(x,arg...)
#endif
#define csi_dev_err(x,arg...) printk(KERN_INFO"[CSI_ERR][HI253]"x,##arg)
#define csi_dev_print(x,arg...) printk(KERN_INFO"[CSI][HI253]"x,##arg)

#define MCLK (24*1000*1000)
#define VREF_POL  CSI_LOW
#define HREF_POL  CSI_HIGH
#define CLK_POL   CSI_RISING
#define IO_CFG    0          


#define CSI_STBY_ON     1
#define CSI_STBY_OFF    0
#define CSI_RST_ON      0
#define CSI_RST_OFF     1
#define CSI_PWR_ON      1
#define CSI_PWR_OFF     0


#define V4L2_IDENT_SENSOR 0x253

#define REG_TERM 0xff
#define VAL_TERM 0xff


#define REG_ADDR_STEP 1
#define REG_DATA_STEP 1
#define REG_STEP      (REG_ADDR_STEP+REG_DATA_STEP)

/*
 * Basic window sizes.  These probably belong somewhere more globally
 * useful.
 */
#define UXGA_WIDTH    1600
#define UXGA_HEIGHT   1200
#define HD720_WIDTH   1280
#define HD720_HEIGHT  720
#define SVGA_WIDTH    800
#define SVGA_HEIGHT   600
#define VGA_WIDTH   640
#define VGA_HEIGHT    480
#define QVGA_WIDTH    320
#define QVGA_HEIGHT   240
#define CIF_WIDTH   352
#define CIF_HEIGHT    288
#define QCIF_WIDTH    176
#define QCIF_HEIGHT   144

/*
 * Our nominal (default) frame rate.
 */
#define SENSOR_FRAME_RATE 20

/*
 * The hi253 sits on i2c with ID 0x40
 */
#define I2C_ADDR 0x40


/* Registers */


/*
 * Information we maintain about a known sensor.
 */
struct sensor_format_struct;  /* coming later */
__csi_subdev_info_t ccm_info_con =
{
  .mclk   = MCLK,
  .vref   = VREF_POL,
  .href   = HREF_POL,
  .clock  = CLK_POL,
  .iocfg  = IO_CFG,
};

struct sensor_info {
  struct v4l2_subdev sd;
  struct sensor_format_struct * fmt; /* Current format */
  __csi_subdev_info_t * ccm_info;
  int width;
  int height;
  int brightness;
  int contrast;
  int saturation;
  int hue;
  int hflip;
  int vflip;
  int gain;
  int autogain;
  int exp;
  enum v4l2_exposure_auto_type autoexp;
  int autowb;
  enum v4l2_whiteblance wb;
  enum v4l2_colorfx clrfx;
  enum v4l2_flash_mode flash_mode;
  u8 clkrc;     /* Clock divider value */
};

static inline struct sensor_info * to_state (struct v4l2_subdev * sd)
{
  return container_of (sd, struct sensor_info, sd);
}


struct regval_list {
  unsigned char reg_num[REG_ADDR_STEP];
  unsigned char value[REG_DATA_STEP];
};


/*
 * The default register settings
 *
 */
static struct regval_list sensor_default_regs[] = {
  {{0x01}, {0x79}},
  {{0x08}, {0x0f}},
  {{0x01}, {0x78}},
  
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  
  {{0x0e}, {0x03}},
  {{0x0e}, {0x73}},
  
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  
  {{0x0e}, {0x00}},
  {{0x01}, {0x71}},
  {{0x08}, {0x00}},
  
  {{0x01}, {0x73}},
  {{0x01}, {0x71}},
  
  {{0x03}, {0x20}},
  {{0x10}, {0x1c}},
  
  {{0x03}, {0x22}},
  {{0x10}, {0x69}},
  
  
  {{0x03}, {0x00}},
  {{0x10}, {0x11}},
  {{0x11}, {0x90}},
  {{0x12}, {0x04}},
  
  {{0x0b}, {0xaa}},
  {{0x0c}, {0xaa}},
  {{0x0d}, {0xaa}},
  
  {{0x20}, {0x00}},
  {{0x21}, {0x04}},
  {{0x22}, {0x00}},
  {{0x23}, {0x07}},
  
  {{0x24}, {0x04}},
  {{0x25}, {0xb0}},
  {{0x26}, {0x06}},
  {{0x27}, {0x40}},
  
  {{0x40}, {0x01}},
  {{0x41}, {0x98}},
  {{0x42}, {0x00}},
  {{0x43}, {0x47}},
  
  {{0x45}, {0x04}},
  {{0x46}, {0x18}},
  {{0x47}, {0xd8}},
  
  {{0x80}, {0x2e}},
  {{0x81}, {0x7e}},
  {{0x82}, {0x90}},
  {{0x83}, {0x00}},
  {{0x84}, {0x0c}},
  {{0x85}, {0x00}},
  {{0x90}, {0x0c}},
  {{0x91}, {0x0c}},
  {{0x92}, {0x98}},
  {{0x93}, {0x90}},
  {{0x94}, {0x75}},
  {{0x95}, {0x70}},
  {{0x96}, {0xdc}},
  {{0x97}, {0xfe}},
  {{0x98}, {0x38}},
  
  {{0x99}, {0x43}},
  {{0x9a}, {0x43}},
  {{0x9b}, {0x43}},
  {{0x9c}, {0x43}},
  
  {{0xa0}, {0x00}},
  {{0xa2}, {0x00}},
  {{0xa4}, {0x00}},
  {{0xa6}, {0x00}},
  
  {{0xa8}, {0x43}},
  {{0xaa}, {0x43}},
  {{0xac}, {0x43}},
  {{0xae}, {0x43}},
  
  {{0x03}, {0x02}},
  {{0x10}, {0x00}},
  {{0x11}, {0x00}},
  {{0x12}, {0x03}},
  {{0x13}, {0x03}},
  {{0x14}, {0x00}},
  {{0x16}, {0x00}},
  {{0x17}, {0x8c}},
  {{0x18}, {0x4C}},
  {{0x19}, {0x00}},
  {{0x1a}, {0x39}},
  {{0x1b}, {0x00}},
  {{0x1c}, {0x09}},
  {{0x1d}, {0x40}},
  {{0x1e}, {0x30}},
  {{0x1f}, {0x10}},
  {{0x20}, {0x77}},
  {{0x21}, {0xde}},
  {{0x22}, {0xa7}},
  {{0x23}, {0x30}},
  {{0x24}, {0x4a}},
  {{0x25}, {0x10}},
  {{0x27}, {0x3c}},
  {{0x28}, {0x00}},
  {{0x29}, {0x0c}},
  {{0x2a}, {0x80}},
  {{0x2b}, {0x80}},
  {{0x2c}, {0x02}},
  {{0x2d}, {0xa0}},
  {{0x2e}, {0x00}},
  {{0x2f}, {0x00}},// {0xa1->{0x00 [20110809 update]
  {{0x30}, {0x05}},
  {{0x31}, {0x99}},
  {{0x32}, {0x00}},
  {{0x33}, {0x00}},
  {{0x34}, {0x22}},
  {{0x38}, {0x88}},
  {{0x39}, {0x88}},
  {{0x50}, {0x20}},
  {{0x51}, {0x00}},
  {{0x52}, {0x01}},
  {{0x53}, {0xc1}},
  {{0x54}, {0x10}},
  {{0x55}, {0x1c}},
  {{0x56}, {0x11}},
  {{0x58}, {0x10}},
  {{0x59}, {0x0e}},
  {{0x5d}, {0xa2}},
  {{0x5e}, {0x5a}},
  {{0x60}, {0x87}},
  {{0x61}, {0x99}},
  {{0x62}, {0x88}},
  {{0x63}, {0x97}},
  {{0x64}, {0x88}},
  {{0x65}, {0x97}},
  {{0x67}, {0x0c}},
  {{0x68}, {0x0c}},
  {{0x69}, {0x0c}},
  {{0x6a}, {0xb4}},
  {{0x6b}, {0xc4}},
  {{0x6c}, {0xb5}},
  {{0x6d}, {0xc2}},
  {{0x6e}, {0xb5}},
  {{0x6f}, {0xc0}},
  {{0x70}, {0xb6}},
  {{0x71}, {0xb8}},
  {{0x72}, {0x89}},
  {{0x73}, {0x96}},
  {{0x74}, {0x89}},
  {{0x75}, {0x96}},
  {{0x76}, {0x89}},
  {{0x77}, {0x96}},
  {{0x7c}, {0x85}},
  {{0x7d}, {0xaf}},
  {{0x80}, {0x01}},
  {{0x81}, {0x7f}},
  {{0x82}, {0x13}},
  {{0x83}, {0x24}},
  {{0x84}, {0x7D}},
  {{0x85}, {0x81}},
  {{0x86}, {0x7D}},
  {{0x87}, {0x81}},
  {{0x88}, {0xab}},
  {{0x89}, {0xbc}},
  {{0x8a}, {0xac}},
  {{0x8b}, {0xba}},
  {{0x8c}, {0xad}},
  {{0x8d}, {0xb8}},
  {{0x8e}, {0xae}},
  {{0x8f}, {0xb2}},
  {{0x90}, {0xb3}},
  {{0x91}, {0xb7}},
  {{0x92}, {0x48}},
  {{0x93}, {0x54}},
  {{0x94}, {0x7D}},
  {{0x95}, {0x81}},
  {{0x96}, {0x7D}},
  {{0x97}, {0x81}},
  {{0xa0}, {0x02}},
  {{0xa1}, {0x7B}},
  {{0xa2}, {0x02}},
  {{0xa3}, {0x7B}},
  {{0xa4}, {0x7B}},
  {{0xa5}, {0x02}},
  {{0xa6}, {0x7B}},
  {{0xa7}, {0x02}},
  {{0xa8}, {0x85}},
  {{0xa9}, {0x8C}},
  {{0xaa}, {0x85}},
  {{0xab}, {0x8C}},
  {{0xac}, {0x10}},
  {{0xad}, {0x16}},
  {{0xae}, {0x10}},
  {{0xaf}, {0x16}},
  {{0xb0}, {0x99}},
  {{0xb1}, {0xA3}},
  {{0xb2}, {0xA4}},
  {{0xb3}, {0xAE}},
  {{0xb4}, {0x9B}},
  {{0xb5}, {0xA2}},
  {{0xb6}, {0xA6}},
  {{0xb7}, {0xAC}},
  {{0xb8}, {0x9B}},
  {{0xb9}, {0x9F}},
  {{0xba}, {0xA6}},
  {{0xbb}, {0xAA}},
  {{0xbc}, {0x9B}},
  {{0xbd}, {0x9F}},
  {{0xbe}, {0xA6}},
  {{0xbf}, {0xaa}},
  {{0xc4}, {0x2c}},
  {{0xc5}, {0x43}},
  {{0xc6}, {0x63}},
  {{0xc7}, {0x79}},
  {{0xc8}, {0x2d}},
  {{0xc9}, {0x42}},
  {{0xca}, {0x2d}},
  {{0xcb}, {0x42}},
  {{0xcc}, {0x64}},
  {{0xcd}, {0x78}},
  {{0xce}, {0x64}},
  {{0xcf}, {0x78}},
  {{0xd0}, {0x0a}},
  {{0xd1}, {0x09}},
  {{0xd2}, {0x20}},
  {{0xd3}, {0x00}},
  
  {{0xd4}, {0x0c}},
  {{0xd5}, {0x0c}},
  {{0xd6}, {0x98}},
  {{0xd7}, {0x90}},
  
  {{0xe0}, {0xc4}},
  {{0xe1}, {0xc4}},
  {{0xe2}, {0xc4}},
  {{0xe3}, {0xc4}},
  {{0xe4}, {0x00}},
  {{0xe8}, {0x80}},
  {{0xe9}, {0x40}},
  {{0xea}, {0x7f}},
  {{0xf0}, {0x01}},
  {{0xf1}, {0x01}},
  {{0xf2}, {0x01}},
  {{0xf3}, {0x01}},
  {{0xf4}, {0x01}},
  
  {{0x03}, {0x03}},
  {{0x10}, {0x10}},
  
  {{0x03}, {0x10}},
  {{0x10}, {0x03}},
  {{0x12}, {0x30}},
  {{0x13}, {0x0a}},
  {{0x20}, {0x00}},
  
  {{0x30}, {0x00}},
  {{0x31}, {0x00}},
  {{0x32}, {0x00}},
  {{0x33}, {0x00}},
  
  {{0x34}, {0x30}},
  {{0x35}, {0x00}},
  {{0x36}, {0x00}},
  {{0x38}, {0x00}},
  {{0x3e}, {0x58}},
  {{0x3f}, {0x00}},
  
  {{0x40}, {0x80}},
  {{0x41}, {0x00}},
  {{0x48}, {0x80}},
  
  {{0x60}, {0x67}},
  {{0x61}, {0x7c}},
  {{0x62}, {0x7c}},
  {{0x63}, {0x50}},
  {{0x64}, {0x41}},
  
  {{0x66}, {0x42}},
  {{0x67}, {0x20}},
  
  {{0x6a}, {0x80}},
  {{0x6b}, {0x84}},
  {{0x6c}, {0x80}},
  {{0x6d}, {0x80}},
  
  
  {{0x03}, {0x11}},
  {{0x10}, {0x7f}},
  {{0x11}, {0x40}},
  {{0x12}, {0x0a}},
  {{0x13}, {0xbb}},
  
  {{0x26}, {0x31}},
  {{0x27}, {0x34}},
  {{0x28}, {0x0f}},
  {{0x29}, {0x10}},
  {{0x2b}, {0x30}},
  {{0x2c}, {0x32}},
  
  {{0x30}, {0x70}},
  {{0x31}, {0x10}},
  {{0x32}, {0x58}},
  {{0x33}, {0x09}},
  {{0x34}, {0x06}},
  {{0x35}, {0x03}},
  
  {{0x36}, {0x70}},
  {{0x37}, {0x18}},
  {{0x38}, {0x58}},
  {{0x39}, {0x09}},
  {{0x3a}, {0x06}},
  {{0x3b}, {0x03}},
  
  {{0x3c}, {0x80}},
  {{0x3d}, {0x18}},
  {{0x3e}, {0xa0}},
  {{0x3f}, {0x0c}},
  {{0x40}, {0x09}},
  {{0x41}, {0x06}},
  
  {{0x42}, {0x80}},
  {{0x43}, {0x18}},
  {{0x44}, {0xa0}},
  {{0x45}, {0x12}},
  {{0x46}, {0x10}},
  {{0x47}, {0x10}},
  
  {{0x48}, {0x90}},
  {{0x49}, {0x40}},
  {{0x4a}, {0x80}},
  {{0x4b}, {0x13}},
  {{0x4c}, {0x10}},
  {{0x4d}, {0x11}},
  
  {{0x4e}, {0x80}},
  {{0x4f}, {0x30}},
  {{0x50}, {0x80}},
  {{0x51}, {0x13}},
  {{0x52}, {0x10}},
  {{0x53}, {0x13}},
  
  {{0x54}, {0x11}},
  {{0x55}, {0x17}},
  {{0x56}, {0x20}},
  {{0x57}, {0x01}},
  {{0x58}, {0x00}},
  {{0x59}, {0x00}},
  
  {{0x5a}, {0x1f}},
  {{0x5b}, {0x00}},
  {{0x5c}, {0x00}},
  
  {{0x60}, {0x3f}},
  {{0x62}, {0x60}},
  {{0x70}, {0x06}},
  
  {{0x03}, {0x12}},
  {{0x20}, {0x0f}},
  {{0x21}, {0x0f}},
  
  {{0x25}, {0x00}},
  
  {{0x28}, {0x00}},
  {{0x29}, {0x00}},
  {{0x2a}, {0x00}},
  
  {{0x30}, {0x50}},
  {{0x31}, {0x18}},
  {{0x32}, {0x32}},
  {{0x33}, {0x40}},
  {{0x34}, {0x50}},
  {{0x35}, {0x70}},
  {{0x36}, {0xa0}},
  
  {{0x3b}, {0x06}},
  {{0x3c}, {0x06}},
  
  
  {{0x40}, {0xa0}},
  {{0x41}, {0x40}},
  {{0x42}, {0xa0}},
  {{0x43}, {0x90}},
  {{0x44}, {0x90}},
  {{0x45}, {0x80}},
  
  {{0x46}, {0xb0}},
  {{0x47}, {0x55}},
  {{0x48}, {0xa0}},
  {{0x49}, {0x90}},
  {{0x4a}, {0x90}},
  {{0x4b}, {0x80}},
  
  {{0x4c}, {0xb0}},
  {{0x4d}, {0x40}},
  {{0x4e}, {0x90}},
  {{0x4f}, {0x90}},
  {{0x50}, {0xa0}},
  {{0x51}, {0x80}},
  
  {{0x52}, {0xb0}},
  {{0x53}, {0x60}},
  {{0x54}, {0xc0}},
  {{0x55}, {0xc0}},
  {{0x56}, {0xc0}},
  {{0x57}, {0x80}},
  
  {{0x58}, {0x90}},
  {{0x59}, {0x40}},
  {{0x5a}, {0xd0}},
  {{0x5b}, {0xd0}},
  {{0x5c}, {0xe0}},
  {{0x5d}, {0x80}},
  
  {{0x5e}, {0x88}},
  {{0x5f}, {0x40}},
  {{0x60}, {0xe0}},
  {{0x61}, {0xe0}},
  {{0x62}, {0xe0}},
  {{0x63}, {0x80}},
  
  {{0x70}, {0x15}},
  {{0x71}, {0x01}},
  
  {{0x72}, {0x18}},
  {{0x73}, {0x01}},
  
  {{0x74}, {0x25}},
  {{0x75}, {0x15}},
  
  
  {{0x90}, {0x5d}},
  {{0x91}, {0x88}},
  {{0x98}, {0x7d}},
  {{0x99}, {0x28}},
  {{0x9A}, {0x14}},
  {{0x9B}, {0xc8}},
  {{0x9C}, {0x02}},
  {{0x9D}, {0x1e}},
  {{0x9E}, {0x28}},
  {{0x9F}, {0x07}},
  {{0xA0}, {0x32}},
  {{0xA4}, {0x04}},
  {{0xA5}, {0x0e}},
  {{0xA6}, {0x0c}},
  {{0xA7}, {0x04}},
  {{0xA8}, {0x3c}},
  
  {{0xAA}, {0x14}},
  {{0xAB}, {0x11}},
  {{0xAC}, {0x0f}},
  {{0xAD}, {0x16}},
  {{0xAE}, {0x15}},
  {{0xAF}, {0x14}},
  
  {{0xB1}, {0xaa}},
  {{0xB2}, {0x96}},
  {{0xB3}, {0x28}},
  {{0xB8}, {0x78}},
  {{0xB9}, {0xa0}},
  {{0xBA}, {0xb4}},
  {{0xBB}, {0x14}},
  {{0xBC}, {0x14}},
  {{0xBD}, {0x14}},
  {{0xBE}, {0x64}},
  {{0xBF}, {0x64}},
  {{0xC0}, {0x64}},
  {{0xC1}, {0x64}},
  {{0xC2}, {0x04}},
  {{0xC3}, {0x03}},
  {{0xC4}, {0x0c}},
  {{0xC5}, {0x30}},
  {{0xC6}, {0x2a}},
  {{0xD0}, {0x0c}},
  {{0xD1}, {0x80}},
  {{0xD2}, {0x67}},
  {{0xD3}, {0x00}},
  {{0xD4}, {0x00}},
  {{0xD5}, {0x02}},
  {{0xD6}, {0xff}},
  {{0xD7}, {0x18}},
  
  {{0x03}, {0x13}},
  {{0x10}, {0xcb}},
  {{0x11}, {0x7b}},
  {{0x12}, {0x07}},
  {{0x14}, {0x00}},
  
  {{0x20}, {0x15}},
  {{0x21}, {0x13}},
  {{0x22}, {0x33}},
  {{0x23}, {0x05}},
  {{0x24}, {0x09}},
  
  {{0x25}, {0x0a}},
  
  {{0x26}, {0x18}},
  {{0x27}, {0x30}},
  {{0x29}, {0x12}},
  {{0x2a}, {0x50}},
  
  {{0x2b}, {0x00}},
  {{0x2c}, {0x00}},
  {{0x25}, {0x06}},
  {{0x2d}, {0x0c}},
  {{0x2e}, {0x12}},
  {{0x2f}, {0x12}},
  
  {{0x50}, {0x18}},
  {{0x51}, {0x1c}},
  {{0x52}, {0x1a}},
  {{0x53}, {0x14}},
  {{0x54}, {0x17}},
  {{0x55}, {0x14}},
  
  {{0x56}, {0x18}},
  {{0x57}, {0x1c}},
  {{0x58}, {0x1a}},
  {{0x59}, {0x14}},
  {{0x5a}, {0x17}},
  {{0x5b}, {0x14}},
  
  {{0x5c}, {0x0a}},
  {{0x5d}, {0x0b}},
  {{0x5e}, {0x0a}},
  {{0x5f}, {0x08}},
  {{0x60}, {0x09}},
  {{0x61}, {0x08}},
  
  {{0x62}, {0x08}},
  {{0x63}, {0x08}},
  {{0x64}, {0x08}},
  {{0x65}, {0x06}},
  {{0x66}, {0x06}},
  {{0x67}, {0x06}},
  
  {{0x68}, {0x07}},
  {{0x69}, {0x07}},
  {{0x6a}, {0x07}},
  {{0x6b}, {0x05}},
  {{0x6c}, {0x05}},
  {{0x6d}, {0x05}},
  
  {{0x6e}, {0x07}},
  {{0x6f}, {0x07}},
  {{0x70}, {0x07}},
  {{0x71}, {0x05}},
  {{0x72}, {0x05}},
  {{0x73}, {0x05}},
  
  {{0x80}, {0xfd}},
  {{0x81}, {0x1f}},
  {{0x82}, {0x05}},
  {{0x83}, {0x31}},
  
  {{0x90}, {0x05}},
  {{0x91}, {0x05}},
  {{0x92}, {0x33}},
  {{0x93}, {0x30}},
  {{0x94}, {0x03}},
  {{0x95}, {0x14}},
  {{0x97}, {0x20}},
  {{0x99}, {0x20}},
  
  {{0xa0}, {0x01}},
  {{0xa1}, {0x02}},
  {{0xa2}, {0x01}},
  {{0xa3}, {0x02}},
  {{0xa4}, {0x05}},
  {{0xa5}, {0x05}},
  {{0xa6}, {0x07}},
  {{0xa7}, {0x08}},
  {{0xa8}, {0x07}},
  {{0xa9}, {0x08}},
  {{0xaa}, {0x07}},
  {{0xab}, {0x08}},
  
  {{0xb0}, {0x22}},
  {{0xb1}, {0x2a}},
  {{0xb2}, {0x28}},
  {{0xb3}, {0x22}},
  {{0xb4}, {0x2a}},
  {{0xb5}, {0x28}},
  
  {{0xb6}, {0x22}},
  {{0xb7}, {0x2a}},
  {{0xb8}, {0x28}},
  {{0xb9}, {0x22}},
  {{0xba}, {0x2a}},
  {{0xbb}, {0x28}},
  
  {{0xbc}, {0x25}},
  {{0xbd}, {0x2a}},
  {{0xbe}, {0x27}},
  {{0xbf}, {0x25}},
  {{0xc0}, {0x2a}},
  {{0xc1}, {0x27}},
  
  {{0xc2}, {0x1e}},
  {{0xc3}, {0x24}},
  {{0xc4}, {0x20}},
  {{0xc5}, {0x1e}},
  {{0xc6}, {0x24}},
  {{0xc7}, {0x20}},
  
  {{0xc8}, {0x18}},
  {{0xc9}, {0x20}},
  {{0xca}, {0x1e}},
  {{0xcb}, {0x18}},
  {{0xcc}, {0x20}},
  {{0xcd}, {0x1e}},
  
  {{0xce}, {0x18}},
  {{0xcf}, {0x20}},
  {{0xd0}, {0x1e}},
  {{0xd1}, {0x18}},
  {{0xd2}, {0x20}},
  {{0xd3}, {0x1e}},
  
  {{0x03}, {0x14}},
  {{0x10}, {0x11}},
  
  {{0x14}, {0x80}},
  {{0x15}, {0x80}},
  {{0x16}, {0x80}},
  {{0x17}, {0x80}},
  {{0x18}, {0x80}},
  {{0x19}, {0x80}},
  
  {{0x20}, {0x60}},
  {{0x21}, {0x80}},
  
  {{0x22}, {0x80}},
  {{0x23}, {0x80}},
  {{0x24}, {0x80}},
  
  {{0x30}, {0xc8}},
  {{0x31}, {0x2b}},
  {{0x32}, {0x00}},
  {{0x33}, {0x00}},
  {{0x34}, {0x90}},
  
  {{0x40}, {0x48}},
  {{0x50}, {0x34}},
  {{0x60}, {0x29}},
  {{0x70}, {0x34}},
  
  {{0x03}, {0x15}},
  {{0x10}, {0x0f}},
  
  {{0x14}, {0x42}},
  {{0x15}, {0x32}},
  {{0x16}, {0x24}},
  {{0x17}, {0x2f}},
  
  {{0x30}, {0x8f}},
  {{0x31}, {0x59}},
  {{0x32}, {0x0a}},
  {{0x33}, {0x15}},
  {{0x34}, {0x5b}},
  {{0x35}, {0x06}},
  {{0x36}, {0x07}},
  {{0x37}, {0x40}},
  {{0x38}, {0x87}},
  
  {{0x40}, {0x92}},
  {{0x41}, {0x1b}},
  {{0x42}, {0x89}},
  {{0x43}, {0x81}},
  {{0x44}, {0x00}},
  {{0x45}, {0x01}},
  {{0x46}, {0x89}},
  {{0x47}, {0x9e}},
  {{0x48}, {0x28}},
  
  
  {{0x50}, {0x02}},
  {{0x51}, {0x82}},
  {{0x52}, {0x00}},
  {{0x53}, {0x07}},
  {{0x54}, {0x11}},
  {{0x55}, {0x98}},
  {{0x56}, {0x00}},
  {{0x57}, {0x0b}},
  {{0x58}, {0x8b}},
  
  {{0x80}, {0x03}},
  {{0x85}, {0x40}},
  {{0x87}, {0x02}},
  {{0x88}, {0x00}},
  {{0x89}, {0x00}},
  {{0x8a}, {0x00}},
  
  {{0x03}, {0x16}},
  {{0x10}, {0x31}},
  {{0x18}, {0x5e}},// Double_AG 5e->37
  {{0x19}, {0x5d}},// Double_AG 5e->36
  {{0x1a}, {0x0e}},
  {{0x1b}, {0x01}},
  {{0x1c}, {0xdc}},
  {{0x1d}, {0xfe}},
  
  {{0x30}, {0x00}},
  {{0x31}, {0x0a}},
  {{0x32}, {0x1f}},
  {{0x33}, {0x33}},
  {{0x34}, {0x53}},
  {{0x35}, {0x6c}},
  {{0x36}, {0x81}},
  {{0x37}, {0x94}},
  {{0x38}, {0xa4}},
  {{0x39}, {0xb3}},
  {{0x3a}, {0xc0}},
  {{0x3b}, {0xcb}},
  {{0x3c}, {0xd5}},
  {{0x3d}, {0xde}},
  {{0x3e}, {0xe6}},
  {{0x3f}, {0xee}},
  {{0x40}, {0xf5}},
  {{0x41}, {0xfc}},
  {{0x42}, {0xff}},
  {{0x50}, {0x00}},
  {{0x51}, {0x09}},
  {{0x52}, {0x1f}},
  {{0x53}, {0x37}},
  {{0x54}, {0x5b}},
  {{0x55}, {0x76}},
  {{0x56}, {0x8d}},
  {{0x57}, {0xa1}},
  {{0x58}, {0xb2}},
  {{0x59}, {0xbe}},
  {{0x5a}, {0xc9}},
  {{0x5b}, {0xd2}},
  {{0x5c}, {0xdb}},
  {{0x5d}, {0xe3}},
  {{0x5e}, {0xeb}},
  {{0x5f}, {0xf0}},
  {{0x60}, {0xf5}},
  {{0x61}, {0xf7}},
  {{0x62}, {0xf8}},
  {{0x70}, {0x00}},
  {{0x71}, {0x07}},
  {{0x72}, {0x0c}},
  {{0x73}, {0x18}},
  {{0x74}, {0x31}},
  {{0x75}, {0x4d}},
  {{0x76}, {0x69}},
  {{0x77}, {0x83}},
  {{0x78}, {0x9b}},
  {{0x79}, {0xb1}},
  {{0x7a}, {0xc3}},
  {{0x7b}, {0xd2}},
  {{0x7c}, {0xde}},
  {{0x7d}, {0xe8}},
  {{0x7e}, {0xf0}},
  {{0x7f}, {0xf5}},
  {{0x80}, {0xfa}},
  {{0x81}, {0xfd}},
  {{0x82}, {0xff}},
  
  {{0x03}, {0x17}},
  {{0x10}, {0xf7}},
  
  {{0x03}, {0x20}},
  {{0x11}, {0x1c}},
  {{0x18}, {0x30}},
  {{0x1a}, {0x08}},
  {{0x20}, {0x01}},
  {{0x21}, {0x30}},
  {{0x22}, {0x10}},
  {{0x23}, {0x00}},
  {{0x24}, {0x00}},
  
  {{0x28}, {0xe7}},
  {{0x29}, {0x0d}},
  {{0x2a}, {0xff}},
  {{0x2b}, {0x04}},
  
  {{0x2c}, {0xc2}},
  {{0x2d}, {0xcf}}, 
  {{0x2e}, {0x33}},
  {{0x30}, {0x78}},
  {{0x32}, {0x03}},
  {{0x33}, {0x2e}},
  {{0x34}, {0x30}},
  {{0x35}, {0xd4}},
  {{0x36}, {0xfe}},
  {{0x37}, {0x32}},
  {{0x38}, {0x04}},
  
  {{0x39}, {0x22}},
  {{0x3a}, {0xde}},
  
  {{0x3b}, {0x22}},
  {{0x3c}, {0xde}},
  
  {{0x50}, {0x45}},
  {{0x51}, {0x88}},
  
  {{0x56}, {0x03}},
  {{0x57}, {0xf7}},
  {{0x58}, {0x14}},
  {{0x59}, {0x88}},
  {{0x5a}, {0x04}},
  
  {{0x60}, {0xff}},
  {{0x61}, {0xff}},
  {{0x62}, {0xea}},
  {{0x63}, {0xab}},
  {{0x64}, {0xea}},
  {{0x65}, {0xab}},
  {{0x66}, {0xeb}},
  {{0x67}, {0xeb}},
  {{0x68}, {0xeb}},
  {{0x69}, {0xeb}},
  {{0x6a}, {0xea}},
  {{0x6b}, {0xab}},
  {{0x6c}, {0xea}},
  {{0x6d}, {0xab}},
  {{0x6e}, {0xff}},
  {{0x6f}, {0xff}},
  
  
  {{0x70}, {0x76}},
  {{0x71}, {0x89}},
  
  {{0x76}, {0x43}},
  {{0x77}, {0xe2}},
  
  {{0x78}, {0x23}},
  {{0x79}, {0x42}},
  {{0x7a}, {0x23}},
  {{0x7b}, {0x22}},
  {{0x7d}, {0x23}},
  
  {{0x83}, {0x01}},
  {{0x84}, {0x5f}},
  {{0x85}, {0x00}},
  
  {{0x86}, {0x02}},
  {{0x87}, {0x00}},
  
  {{0x88}, {0x05}},
  {{0x89}, {0x7c}},
  {{0x8a}, {0x00}},
  
  {{0x8B}, {0x75}},
  {{0x8C}, {0x00}},
  {{0x8D}, {0x61}},
  {{0x8E}, {0x00}},
  
  {{0x9c}, {0x18}},
  {{0x9d}, {0x00}},
  {{0x9e}, {0x02}},
  {{0x9f}, {0x00}},
  
  
  {{0xb0}, {0x18}},
  {{0xb1}, {0x14}},
  {{0xb2}, {0xa0}},
  {{0xb3}, {0x18}},
  {{0xb4}, {0x1a}},
  {{0xb5}, {0x44}},
  {{0xb6}, {0x2f}},
  {{0xb7}, {0x28}},
  {{0xb8}, {0x25}},
  {{0xb9}, {0x22}},
  {{0xba}, {0x21}},
  {{0xbb}, {0x20}},
  {{0xbc}, {0x1f}},
  {{0xbd}, {0x1f}},
  
  {{0xc0}, {0x14}},
  {{0xc1}, {0x1f}},
  {{0xc2}, {0x1f}},
  {{0xc3}, {0x18}},
  {{0xc4}, {0x10}},
  
  {{0xc8}, {0x80}},
  {{0xc9}, {0x40}},
  
  {{0x03}, {0x22}},
  {{0x10}, {0xfd}},
  {{0x11}, {0x2e}},
  {{0x19}, {0x01}},
  {{0x20}, {0x30}},
  {{0x21}, {0x80}},
  {{0x24}, {0x01}},
  
  {{0x30}, {0x80}},
  {{0x31}, {0x80}},
  {{0x38}, {0x11}},
  {{0x39}, {0x34}},
  
  {{0x40}, {0xf7}},
  {{0x41}, {0x55}},
  {{0x42}, {0x33}},
  
  {{0x43}, {0xf7}},
  {{0x44}, {0x55}},
  {{0x45}, {0x44}},
  
  {{0x46}, {0x00}},
  {{0x50}, {0xb2}},
  {{0x51}, {0x81}},
  {{0x52}, {0x98}},
  
  {{0x80}, {0x40}},
  {{0x81}, {0x20}},
  {{0x82}, {0x3e}},
  
  {{0x83}, {0x5e}},
  {{0x84}, {0x1e}},
  {{0x85}, {0x5e}},
  {{0x86}, {0x22}},
  
  {{0x87}, {0x40}},
  {{0x88}, {0x30}},
  {{0x89}, {0x3f}},
  {{0x8a}, {0x28}},
  
  {{0x8b}, {0x40}},
  {{0x8c}, {0x33}},
  {{0x8d}, {0x39}},
  {{0x8e}, {0x30}},
  
  {{0x8f}, {0x53}},
  {{0x90}, {0x52}},
  {{0x91}, {0x51}},
  {{0x92}, {0x4e}},
  {{0x93}, {0x4a}},
  {{0x94}, {0x45}},
  {{0x95}, {0x3d}},
  {{0x96}, {0x31}},
  {{0x97}, {0x28}},
  {{0x98}, {0x24}},
  {{0x99}, {0x20}},
  {{0x9a}, {0x20}},
  
  {{0x9b}, {0x77}},
  {{0x9c}, {0x77}},
  {{0x9d}, {0x48}},
  {{0x9e}, {0x38}},
  {{0x9f}, {0x30}},
  
  {{0xa0}, {0x60}},
  {{0xa1}, {0x34}},
  {{0xa2}, {0x6f}},
  {{0xa3}, {0xff}},
  
  {{0xa4}, {0x14}},
  {{0xa5}, {0x2c}},
  {{0xa6}, {0xcf}},
  
  {{0xad}, {0x40}},
  {{0xae}, {0x4a}},
  
  {{0xaf}, {0x28}}, 
  {{0xb0}, {0x26}}, 
  
  {{0xb1}, {0x00}},
  {{0xb4}, {0xea}},
  {{0xb8}, {0xa0}},
  {{0xb9}, {0x00}},
  
  {{0x03}, {0x20}},
  {{0x10}, {0x9c}},
  
  {{0x03}, {0x22}},
  {{0x10}, {0xe9}},
  
  {{0x03}, {0x00}},
  {{0x0e}, {0x03}},
  {{0x0e}, {0x73}},
  
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  {{0x03}, {0x00}},
  
  {{0x03}, {0x00}},
  {{0x01}, {0xc0}},
  
  {{0xff}, {0xff}},
  {{0xff}, {0xff}},
  
};

static struct regval_list sensor_uxga_regs[] = {
  {{0x03}, {0x00}},
  
  {{0x10}, {0x00}},//VGA Size
  
  {{0x20}, {0x00}},
  {{0x21}, {0x04}},
  {{0x22}, {0x00}},
  {{0x23}, {0x07}},
  
  {{0x40}, {0x01}},//HBLANK: 0x70 = 112
  {{0x41}, {0x98}},
  {{0x42}, {0x00}},//VBLANK: 0x04 = 4
  {{0x43}, {0x47}},
};

static struct regval_list sensor_hd720_regs[] = {
  {{0x03}, {0x00}},
  
  {{0x10}, {0x00}},//VGA Size
  
  {{0x20}, {0x00}},
  {{0x21}, {0xf0}},
  {{0x22}, {0x00}},
  {{0x23}, {0xa0}},
  
  {{0x24}, {0x02}},
  {{0x25}, {0xd0}},
  {{0x26}, {0x05}},
  {{0x27}, {0x00}},
  
  {{0x40}, {0x01}},//HBLANK: 0x70 = 112
  {{0x41}, {0x98}},
  {{0x42}, {0x00}},//VBLANK: 0x04 = 4
  {{0x43}, {0x47}},
  
  {{0x03}, {0x18}}, 
  {{0x10}, {0x00}},
};

static struct regval_list sensor_svga_regs[] = {
  {{0x03}, {0x00}}, 
  
  {{0x10}, {0x11}},//VGA Size
  
  {{0x20}, {0x00}},
  {{0x21}, {0x04}},
  {{0x22}, {0x00}},
  {{0x23}, {0x07}},
  
  {{0x24}, {0x04}},
  {{0x25}, {0xb0}},
  {{0x26}, {0x06}},
  {{0x27}, {0x40}},
  
  {{0x40}, {0x01}},//HBLANK: 0x70 = 112
  {{0x41}, {0x98}},
  {{0x42}, {0x00}},//VBLANK: 0x04 = 4
  {{0x43}, {0x47}},
  
  {{0x03}, {0x18}}, 
  {{0x10}, {0x00}},
};

static struct regval_list sensor_vga_regs[] = {
  {{0x03}, {0x00}},
  
  {{0x10}, {0x11}},//VGA Size
  
  {{0x20}, {0x00}},
  {{0x21}, {0x04}},
  {{0x22}, {0x00}},
  {{0x23}, {0x07}},
  
  {{0x40}, {0x01}},//HBLANK: 0x70 = 112
  {{0x41}, {0x98}},
  {{0x42}, {0x00}},//VBLANK: 0x04 = 4
  {{0x43}, {0x47}},
  
  {{0x03}, {0x18}},
  {{0x12}, {0x20}},
  {{0x10}, {0x07}},
  {{0x11}, {0x00}},
  {{0x20}, {0x05}},
  {{0x21}, {0x00}},
  {{0x22}, {0x01}},
  {{0x23}, {0xe0}},
  {{0x24}, {0x00}},
  {{0x25}, {0x06}},
  {{0x26}, {0x00}},
  {{0x27}, {0x00}},
  {{0x28}, {0x05}},
  {{0x29}, {0x06}},
  {{0x2a}, {0x01}},
  {{0x2b}, {0xe0}},
  {{0x2c}, {0x0a}},
  {{0x2d}, {0x00}},
  {{0x2e}, {0x0a}},
  {{0x2f}, {0x00}},
  {{0x30}, {0x45}},
  
  
};

static struct regval_list sensor_qvga_regs[] = {
  {{0x03}, {0x00}},
  
  {{0x10}, {0x11}},//VGA Size
  
  {{0x20}, {0x00}},
  {{0x21}, {0x04}},
  {{0x22}, {0x00}},
  {{0x23}, {0x07}},
  
  {{0x40}, {0x01}},//HBLANK: 0x70 = 112
  {{0x41}, {0x98}},
  {{0x42}, {0x00}},//VBLANK: 0x04 = 4
  {{0x43}, {0x47}},
  
  {{0x03}, {0x18}},
  {{0x12}, {0x20}},
  {{0x10}, {0x07}},
  {{0x11}, {0x00}},
  {{0x20}, {0x02}},
  {{0x21}, {0x80}},
  {{0x22}, {0x00}},
  {{0x23}, {0xf0}},
  {{0x24}, {0x00}},
  {{0x25}, {0x06}},
  {{0x26}, {0x00}},
  {{0x27}, {0x00}},
  {{0x28}, {0x02}},
  {{0x29}, {0x86}},
  {{0x2a}, {0x00}},
  {{0x2b}, {0xf0}},
  {{0x2c}, {0x14}},
  {{0x2d}, {0x00}},
  {{0x2e}, {0x14}},
  {{0x2f}, {0x00}},
  {{0x30}, {0x65}},
};



/*
 * The white balance settings
 * Here only tune the R G B channel gain.
 * The white balance enalbe bit is modified in sensor_s_autowb and sensor_s_wb
 */
static struct regval_list sensor_wb_auto_regs[] = {
  {{0x03}, {0x22}},
  {{0x11}, {0x2e}},
  
  {{0x83}, {0x5e}},
  {{0x84}, {0x1e}},
  {{0x85}, {0x5e}},
  {{0x86}, {0x22}},
};

static struct regval_list sensor_wb_cloud_regs[] = {
  {{0x03}, {0x22}},
  {{0x11}, {0x28}},
  {{0x80}, {0x71}},
  {{0x82}, {0x2b}},
  {{0x83}, {0x72}},
  {{0x84}, {0x70}},
  {{0x85}, {0x2b}},
  {{0x86}, {0x28}},
};

static struct regval_list sensor_wb_daylight_regs[] = {
  {{0x03}, {0x22}},
  {{0x11}, {0x28}},
  {{0x80}, {0x59}},
  {{0x82}, {0x29}},
  {{0x83}, {0x60}},
  {{0x84}, {0x50}},
  {{0x85}, {0x2f}},
  {{0x86}, {0x23}},
};

static struct regval_list sensor_wb_incandescence_regs[] = {
  {{0x03}, {0x22}},
  {{0x11}, {0x28}},
  {{0x80}, {0x29}},
  {{0x82}, {0x54}},
  {{0x83}, {0x2e}},
  {{0x84}, {0x23}},
  {{0x85}, {0x58}},
  {{0x86}, {0x4f}},
};

static struct regval_list sensor_wb_fluorescent_regs[] = {
  {{0x03}, {0x22}},
  {{0x11}, {0x28}},
  {{0x80}, {0x41}},
  {{0x82}, {0x42}},
  {{0x83}, {0x44}},
  {{0x84}, {0x34}},
  {{0x85}, {0x46}},
  {{0x86}, {0x3a}},
};

static struct regval_list sensor_wb_tungsten_regs[] = {
  {{0x03}, {0x22}},
  {{0x80}, {0x24}},
  {{0x81}, {0x20}},
  {{0x82}, {0x58}},
  {{0x83}, {0x27}},
  {{0x84}, {0x22}},
  {{0x85}, {0x58}},
  {{0x86}, {0x52}},
};

/*
 * The color effect settings
 */
static struct regval_list sensor_colorfx_none_regs[] = {
  {{0x03}, {0x10}},
  {{0x11}, {0x03}},
  {{0x12}, {0x30}},
  {{0x13}, {0x02}},
  {{0x44}, {0x80}},
  {{0x45}, {0x80}},
};

static struct regval_list sensor_colorfx_bw_regs[] = {
  {{0x03}, {0x10}},
  {{0x11}, {0x03}},
  {{0x12}, {0x03}},
  {{0x13}, {0x02}},
  {{0x40}, {0x00}},
  {{0x44}, {0x80}},
  {{0x45}, {0x80}},
};

static struct regval_list sensor_colorfx_sepia_regs[] = {
  {{0x03}, {0x10}},
  {{0x11}, {0x03}},
  {{0x12}, {0x33}},
  {{0x13}, {0x02}},
  {{0x44}, {0x70}},
  {{0x45}, {0x98}},
};

static struct regval_list sensor_colorfx_negative_regs[] = {
  {{0x03}, {0x10}},
  {{0x11}, {0x03}},
  {{0x12}, {0x08}},
  {{0x13}, {0x02}},
  {{0x14}, {0x00}},
};

static struct regval_list sensor_colorfx_emboss_regs[] = {
};

static struct regval_list sensor_colorfx_sketch_regs[] = {
};

static struct regval_list sensor_colorfx_sky_blue_regs[] = {
  {{0x03}, {0x10}},
  {{0x11}, {0x03}},
  {{0x12}, {0x03}},
  {{0x40}, {0x00}},
  {{0x13}, {0x02}},
  {{0x44}, {0xb0}},
  {{0x45}, {0x40}},
};

static struct regval_list sensor_colorfx_grass_green_regs[] = {
  {{0x03}, {0x10}},
  {{0x11}, {0x03}},
  {{0x12}, {0x03}},
  {{0x40}, {0x00}},
  {{0x13}, {0x02}},
  {{0x44}, {0x30}},
  {{0x45}, {0x50}},
};

static struct regval_list sensor_colorfx_skin_whiten_regs[] = {
};

static struct regval_list sensor_colorfx_vivid_regs[] = {
};

/*
 * The brightness setttings
 */
static struct regval_list sensor_brightness_neg4_regs[] = {
  {{0x03}, {0x10}},
  {{0x40}, {0xe0}}
};

static struct regval_list sensor_brightness_neg3_regs[] = {
  {{0x03}, {0x10}},
  {{0x40}, {0xc0}}
};

static struct regval_list sensor_brightness_neg2_regs[] = {
  {{0x03}, {0x10}},
  {{0x40}, {0xa0}}
};

static struct regval_list sensor_brightness_neg1_regs[] = {
  {{0x03}, {0x10}},
  {{0x40}, {0x90}}
};

static struct regval_list sensor_brightness_zero_regs[] = {
  {{0x03}, {0x10}},
  {{0x40}, {0x00}}
};

static struct regval_list sensor_brightness_pos1_regs[] = {
  {{0x03}, {0x10}},
  {{0x40}, {0x10}}
};

static struct regval_list sensor_brightness_pos2_regs[] = {
  {{0x03}, {0x10}},
  {{0x40}, {0x20}}
};

static struct regval_list sensor_brightness_pos3_regs[] = {
  {{0x03}, {0x10}},
  {{0x40}, {0x30}}
};

static struct regval_list sensor_brightness_pos4_regs[] = {
  {{0x03}, {0x10}},
  {{0x40}, {0x40}}
};

/*
 * The contrast setttings
 */
static struct regval_list sensor_contrast_neg4_regs[] = {
  {{0x03}, {0x10}},
  {{0x48}, {0x44}}
};

static struct regval_list sensor_contrast_neg3_regs[] = {
  {{0x03}, {0x10}},
  {{0x48}, {0x54}}
};

static struct regval_list sensor_contrast_neg2_regs[] = {
  {{0x03}, {0x10}},
  {{0x48}, {0x64}}
};

static struct regval_list sensor_contrast_neg1_regs[] = {
  {{0x03}, {0x10}},
  {{0x48}, {0x74}}
};

static struct regval_list sensor_contrast_zero_regs[] = {
  {{0x03}, {0x10}},
  {{0x48}, {0x84}}
};

static struct regval_list sensor_contrast_pos1_regs[] = {
  {{0x03}, {0x10}},
  {{0x48}, {0x94}}
};

static struct regval_list sensor_contrast_pos2_regs[] = {
  {{0x03}, {0x10}},
  {{0x48}, {0xa4}}
};

static struct regval_list sensor_contrast_pos3_regs[] = {
  {{0x03}, {0x10}},
  {{0x48}, {0xb4}}
};

static struct regval_list sensor_contrast_pos4_regs[] = {
  {{0x03}, {0x10}},
  {{0x48}, {0xc4}}
};

/*
 * The saturation setttings
 */
static struct regval_list sensor_saturation_neg4_regs[] = {
  {{0x03}, {0x10}},
  {{0x61}, {0x50}},
  {{0x62}, {0x50}}
};

static struct regval_list sensor_saturation_neg3_regs[] = {
  {{0x03}, {0x10}},
  {{0x61}, {0x60}},
  {{0x62}, {0x60}}
};

static struct regval_list sensor_saturation_neg2_regs[] = {
  {{0x03}, {0x10}},
  {{0x61}, {0x70}},
  {{0x62}, {0x70}}
};

static struct regval_list sensor_saturation_neg1_regs[] = {
  {{0x03}, {0x10}},
  {{0x61}, {0x80}},
  {{0x62}, {0x80}}
};

static struct regval_list sensor_saturation_zero_regs[] = {
  {{0x03}, {0x10}},
  {{0x61}, {0x90}},
  {{0x62}, {0x90}}
};

static struct regval_list sensor_saturation_pos1_regs[] = {
  {{0x03}, {0x10}},
  {{0x61}, {0xa0}},
  {{0x62}, {0xa0}}
};

static struct regval_list sensor_saturation_pos2_regs[] = {
  {{0x03}, {0x10}},
  {{0x61}, {0xb0}},
  {{0x62}, {0xb0}}
};

static struct regval_list sensor_saturation_pos3_regs[] = {
  {{0x03}, {0x10}},
  {{0x61}, {0xc0}},
  {{0x62}, {0xc0}}
};

static struct regval_list sensor_saturation_pos4_regs[] = {
  {{0x03}, {0x10}},
  {{0x61}, {0xd0}},
  {{0x62}, {0xd0}}
};

/*
 * The exposure target setttings
 */
static struct regval_list sensor_ev_neg4_regs[] = {
  {{0x03}, {0x20}},
  {{0x70}, {0x58}}
};

static struct regval_list sensor_ev_neg3_regs[] = {
  {{0x03}, {0x20}},
  {{0x70}, {0x60}}
};

static struct regval_list sensor_ev_neg2_regs[] = {
  {{0x03}, {0x20}},
  {{0x70}, {0x68}}
};

static struct regval_list sensor_ev_neg1_regs[] = {
  {{0x03}, {0x20}},
  {{0x70}, {0x70}}
};

static struct regval_list sensor_ev_zero_regs[] = {
  {{0x03}, {0x20}},
  {{0x70}, {0x76}}
};

static struct regval_list sensor_ev_pos1_regs[] = {
  {{0x03}, {0x20}},
  {{0x70}, {0x80}}
};

static struct regval_list sensor_ev_pos2_regs[] = {
  {{0x03}, {0x20}},
  {{0x70}, {0x88}}
};

static struct regval_list sensor_ev_pos3_regs[] = {
  {{0x03}, {0x20}},
  {{0x70}, {0x90}}
};

static struct regval_list sensor_ev_pos4_regs[] = {
  {{0x03}, {0x20}},
  {{0x70}, {0x98}}
};


/*
 * Here we'll try to encapsulate the changes for just the output
 * video format.
 *
 */


static struct regval_list sensor_fmt_yuv422_yuyv[] = {
  {{0x03}, {0x10}},
  {{0x10}, {0x03}}
};


static struct regval_list sensor_fmt_yuv422_yvyu[] = {
  {{0x03}, {0x10}},
  {{0x10}, {0x02}}
};

static struct regval_list sensor_fmt_yuv422_vyuy[] = {
  {{0x03}, {0x10}},
  {{0x10}, {0x00}}
};

static struct regval_list sensor_fmt_yuv422_uyvy[] = {
  {{0x03}, {0x10}},
  {{0x10}, {0x01}}
};




/*
 * Low-level register I/O.
 *
 */


/*
 * On most platforms, we'd rather do straight i2c I/O.
 */
static int sensor_read (struct v4l2_subdev * sd, unsigned char * reg,
                        unsigned char * value)
{
  struct i2c_client * client = v4l2_get_subdevdata (sd);
  u8 data[REG_STEP];
  struct i2c_msg msg;
  int ret, i;
  
  for (i = 0; i < REG_ADDR_STEP; i++)
  { data[i] = reg[i]; }
  
  data[REG_ADDR_STEP] = 0xff;
  /*
   * Send out the register address...
   */
  msg.addr = client->addr;
  msg.flags = 0;
  msg.len = REG_ADDR_STEP;
  msg.buf = data;
  ret = i2c_transfer (client->adapter, &msg, 1);
  if (ret < 0) {
    csi_dev_err ("Error %d on register write\n", ret);
    return ret;
  }
  /*
   * ...then read back the result.
   */
  
  msg.flags = I2C_M_RD;
  msg.len = REG_DATA_STEP;
  msg.buf = &data[REG_ADDR_STEP];
  
  ret = i2c_transfer (client->adapter, &msg, 1);
  if (ret >= 0) {
    for (i = 0; i < REG_DATA_STEP; i++)
    { value[i] = data[i + REG_ADDR_STEP]; }
    ret = 0;
  }
  else {
    csi_dev_err ("Error %d on register read\n", ret);
  }
  return ret;
}


static int sensor_write (struct v4l2_subdev * sd, unsigned char * reg,
                         unsigned char * value)
{
  struct i2c_client * client = v4l2_get_subdevdata (sd);
  struct i2c_msg msg;
  unsigned char data[REG_STEP];
  int ret, i;
  
  for (i = 0; i < REG_ADDR_STEP; i++)
  { data[i] = reg[i]; }
  for (i = REG_ADDR_STEP; i < REG_STEP; i++)
  { data[i] = value[i - REG_ADDR_STEP]; }
  

  msg.addr = client->addr;
  msg.flags = 0;
  msg.len = REG_STEP;
  msg.buf = data;
  
  
  ret = i2c_transfer (client->adapter, &msg, 1);
  if (ret > 0) {
    ret = 0;
  }
  else
    if (ret < 0) {
      csi_dev_err ("sensor_write error!\n");
    }
  return ret;
}


/*
 * Write a list of register settings;
 */
static int sensor_write_array (struct v4l2_subdev * sd, struct regval_list * vals , uint size)
{
  int i, ret;
  
  if (size == 0)
  { return -EINVAL; }
  
  for (i = 0; i < size ; i++)
  {
    ret = sensor_write (sd, vals->reg_num, vals->value);
    if (ret < 0)
    {
      csi_dev_err ("sensor_write_err!\n");
      return ret;
    }
    
    vals++;
  }
  
  return 0;
}


/*
 * Stuff that knows about the sensor.
 */

static int sensor_power (struct v4l2_subdev * sd, int on)
{
  struct csi_dev * dev = (struct csi_dev *) dev_get_drvdata (sd->v4l2_dev->dev);
  struct sensor_info * info = to_state (sd);
  char csi_stby_str[32], csi_power_str[32], csi_reset_str[32];
  
  if (info->ccm_info->iocfg == 0) {
    strcpy (csi_stby_str, "csi_stby");
    strcpy (csi_power_str, "csi_power_en");
    strcpy (csi_reset_str, "csi_reset");
  }
  else
    if (info->ccm_info->iocfg == 1) {
      strcpy (csi_stby_str, "csi_stby_b");
      strcpy (csi_power_str, "csi_power_en_b");
      strcpy (csi_reset_str, "csi_reset_b");
    }
    
  switch (on)
  {
  case CSI_SUBDEV_STBY_ON:
    csi_dev_dbg ("CSI_SUBDEV_STBY_ON\n");
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_OFF, csi_reset_str);
    msleep (10);
    clk_enable (dev->csi_module_clk);
    msleep (100);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_STBY_ON, csi_stby_str);
    msleep (100);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_STBY_OFF, csi_stby_str);
    msleep (100);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_STBY_ON, csi_stby_str);
    msleep (100);
    clk_disable (dev->csi_module_clk);
    
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_ON, csi_reset_str);
    msleep (10);
    break;
  case CSI_SUBDEV_STBY_OFF:
    csi_dev_dbg ("CSI_SUBDEV_STBY_OFF\n");
    clk_enable (dev->csi_module_clk);
    msleep (10);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_OFF, csi_reset_str);
    msleep (10);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_ON, csi_reset_str);
    msleep (100);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_OFF, csi_reset_str);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_STBY_OFF, csi_stby_str);
    msleep (10);
    break;
  case CSI_SUBDEV_PWR_ON:
    csi_dev_dbg ("CSI_SUBDEV_PWR_ON\n");
    clk_disable (dev->csi_module_clk);
    gpio_set_one_pin_io_status (dev->csi_pin_hd, 1, csi_stby_str);
    gpio_set_one_pin_io_status (dev->csi_pin_hd, 1, csi_reset_str);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_STBY_ON, csi_stby_str);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_ON, csi_reset_str);
    msleep (1);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_PWR_ON, csi_power_str);
    msleep (10);
    if (dev->dvdd) {
      regulator_enable (dev->dvdd);
      msleep (10);
    }
    if (dev->avdd) {
      regulator_enable (dev->avdd);
      msleep (10);
    }
    if (dev->iovdd) {
      regulator_enable (dev->iovdd);
      msleep (10);
    }
    clk_enable (dev->csi_module_clk);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_OFF, csi_reset_str);
    msleep (10);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_ON, csi_reset_str);
    msleep (100);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_OFF, csi_reset_str);
    msleep (100);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_STBY_OFF, csi_stby_str);
    msleep (10);
    break;
    
  case CSI_SUBDEV_PWR_OFF:
    csi_dev_dbg ("CSI_SUBDEV_PWR_OFF\n");
    if (dev->iovdd) {
      regulator_disable (dev->iovdd);
      msleep (10);
    }
    if (dev->avdd) {
      regulator_disable (dev->avdd);
      msleep (10);
    }
    if (dev->dvdd) {
      regulator_disable (dev->dvdd);
      msleep (10);
    }
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_PWR_OFF, csi_power_str);
    msleep (10);
    
    clk_disable (dev->csi_module_clk);
    
    gpio_set_one_pin_io_status (dev->csi_pin_hd, 0, csi_reset_str);
    gpio_set_one_pin_io_status (dev->csi_pin_hd, 0, csi_stby_str);
    break;
  default:
    return -EINVAL;
  }
  return 0;
}

static int sensor_reset (struct v4l2_subdev * sd, u32 val)
{
  struct csi_dev * dev = (struct csi_dev *) dev_get_drvdata (sd->v4l2_dev->dev);
  struct sensor_info * info = to_state (sd);
  char csi_reset_str[32];
  
  if (info->ccm_info->iocfg == 0) {
    strcpy (csi_reset_str, "csi_reset");
  }
  else
    if (info->ccm_info->iocfg == 1) {
      strcpy (csi_reset_str, "csi_reset_b");
    }
    
  switch (val)
  {
  case CSI_SUBDEV_RST_OFF:
    csi_dev_dbg ("CSI_SUBDEV_RST_OFF\n");
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_OFF, csi_reset_str);
    msleep (10);
    break;
  case CSI_SUBDEV_RST_ON:
    csi_dev_dbg ("CSI_SUBDEV_RST_ON\n");
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_ON, csi_reset_str);
    msleep (10);
    break;
  case CSI_SUBDEV_RST_PUL:
    csi_dev_dbg ("CSI_SUBDEV_RST_PUL\n");
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_OFF, csi_reset_str);
    msleep (10);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_ON, csi_reset_str);
    msleep (100);
    gpio_write_one_pin_value (dev->csi_pin_hd, CSI_RST_OFF, csi_reset_str);
    msleep (10);
    break;
  default:
    return -EINVAL;
  }
  
  return 0;
}

static int sensor_detect (struct v4l2_subdev * sd)
{
  int ret;
  struct regval_list regs;
  
  regs.reg_num[0] = 0x03;
  regs.value[0] = 0x00;
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_detect!\n");
    return ret;
  }
  
  regs.reg_num[0] = 0x04;
  ret = sensor_read (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_read err at sensor_detect!\n");
    return ret;
  }
  
  if (regs.value[0] != 0x92)
  { return -ENODEV; }
  
  return 0;
}

static int sensor_init (struct v4l2_subdev * sd, u32 val)
{
  int ret;
  csi_dev_dbg ("sensor_init\n");
  /*Make sure it is a target sensor*/
  ret = sensor_detect (sd);
  if (ret) {
    csi_dev_err ("chip found is not an target chip.\n");
    return ret;
  }
  return sensor_write_array (sd, sensor_default_regs , ARRAY_SIZE (sensor_default_regs) );
}

static long sensor_ioctl (struct v4l2_subdev * sd, unsigned int cmd, void * arg)
{
  int ret = 0;
  
  switch (cmd) {
  case CSI_SUBDEV_CMD_GET_INFO:
    {
      struct sensor_info * info = to_state (sd);
      __csi_subdev_info_t * ccm_info = arg;
      
      csi_dev_dbg ("CSI_SUBDEV_CMD_GET_INFO\n");
      
      ccm_info->mclk  = info->ccm_info->mclk ;
      ccm_info->vref  = info->ccm_info->vref ;
      ccm_info->href  = info->ccm_info->href ;
      ccm_info->clock = info->ccm_info->clock;
      ccm_info->iocfg = info->ccm_info->iocfg;
      
      csi_dev_dbg ("ccm_info.mclk=%x\n ", info->ccm_info->mclk);
      csi_dev_dbg ("ccm_info.vref=%x\n ", info->ccm_info->vref);
      csi_dev_dbg ("ccm_info.href=%x\n ", info->ccm_info->href);
      csi_dev_dbg ("ccm_info.clock=%x\n ", info->ccm_info->clock);
      csi_dev_dbg ("ccm_info.iocfg=%x\n ", info->ccm_info->iocfg);
      break;
    }
  case CSI_SUBDEV_CMD_SET_INFO:
    {
      struct sensor_info * info = to_state (sd);
      __csi_subdev_info_t * ccm_info = arg;
      
      csi_dev_dbg ("CSI_SUBDEV_CMD_SET_INFO\n");
      
      info->ccm_info->mclk  = ccm_info->mclk  ;
      info->ccm_info->vref  = ccm_info->vref  ;
      info->ccm_info->href  = ccm_info->href  ;
      info->ccm_info->clock = ccm_info->clock ;
      info->ccm_info->iocfg = ccm_info->iocfg ;
      
      csi_dev_dbg ("ccm_info.mclk=%x\n ", info->ccm_info->mclk);
      csi_dev_dbg ("ccm_info.vref=%x\n ", info->ccm_info->vref);
      csi_dev_dbg ("ccm_info.href=%x\n ", info->ccm_info->href);
      csi_dev_dbg ("ccm_info.clock=%x\n ", info->ccm_info->clock);
      csi_dev_dbg ("ccm_info.iocfg=%x\n ", info->ccm_info->iocfg);
      
      break;
    }
  default:
    return -EINVAL;
  }
  return ret;
}


/*
 * Store information about the video data format.
 */
static struct sensor_format_struct {
  __u8 * desc;
  enum v4l2_mbus_pixelcode mbus_code;//linux-3.0
  struct regval_list * regs;
  int regs_size;
  int bpp;   /* Bytes per pixel */
} sensor_formats[] = {
  {
    .desc   = "YUYV 4:2:2",
    .mbus_code  = V4L2_MBUS_FMT_YUYV8_2X8,//linux-3.0
    .regs     = sensor_fmt_yuv422_yuyv,
    .regs_size = ARRAY_SIZE (sensor_fmt_yuv422_yuyv),
    .bpp    = 2,
  },
  {
    .desc   = "YVYU 4:2:2",
    .mbus_code  = V4L2_MBUS_FMT_YVYU8_2X8,//linux-3.0
    .regs     = sensor_fmt_yuv422_yvyu,
    .regs_size = ARRAY_SIZE (sensor_fmt_yuv422_yvyu),
    .bpp    = 2,
  },
  {
    .desc   = "UYVY 4:2:2",
    .mbus_code  = V4L2_MBUS_FMT_UYVY8_2X8,//linux-3.0
    .regs     = sensor_fmt_yuv422_uyvy,
    .regs_size = ARRAY_SIZE (sensor_fmt_yuv422_uyvy),
    .bpp    = 2,
  },
  {
    .desc   = "VYUY 4:2:2",
    .mbus_code  = V4L2_MBUS_FMT_VYUY8_2X8,//linux-3.0
    .regs     = sensor_fmt_yuv422_vyuy,
    .regs_size = ARRAY_SIZE (sensor_fmt_yuv422_vyuy),
    .bpp    = 2,
  },
};
#define N_FMTS ARRAY_SIZE(sensor_formats)


/*
 * Then there is the issue of window sizes.  Try to capture the info here.
 */


static struct sensor_win_size {
  int width;
  int height;
  int hstart;   /* Start/stop values for the camera.  Note */
  int hstop;    /* that they do not always make complete */
  int vstart;   /* sense to humans, but evidently the sensor */
  int vstop;    /* will do the right thing... */
  struct regval_list * regs; /* Regs to tweak */
  int regs_size;
  int (*set_size) (struct v4l2_subdev * sd);
  /* h/vref stuff */
} sensor_win_sizes[] = {
  /* UXGA */
  {
    .width      = UXGA_WIDTH,
    .height     = UXGA_HEIGHT,
    .regs       = sensor_uxga_regs,
    .regs_size  = ARRAY_SIZE (sensor_uxga_regs),
    .set_size   = NULL,
  },
  /* 720p */
  {
    .width      = HD720_WIDTH,
    .height     = HD720_HEIGHT,
    .regs       = sensor_hd720_regs,
    .regs_size  = ARRAY_SIZE (sensor_hd720_regs),
    .set_size   = NULL,
  },
  /* SVGA */
  {
    .width      = SVGA_WIDTH,
    .height     = SVGA_HEIGHT,
    .regs       = sensor_svga_regs,
    .regs_size  = ARRAY_SIZE (sensor_svga_regs),
    .set_size   = NULL,
  },
  /* VGA */
  {
    .width      = VGA_WIDTH,
    .height     = VGA_HEIGHT,
    .regs       = sensor_vga_regs,
    .regs_size  = ARRAY_SIZE (sensor_vga_regs),
    .set_size   = NULL,
  },
  /* QVGA */
  {
    .width    = QVGA_WIDTH,
    .height   = QVGA_HEIGHT,
    .regs     = sensor_qvga_regs,
    .regs_size  = ARRAY_SIZE (sensor_qvga_regs),
    .set_size   = NULL,
  }
};

#define N_WIN_SIZES (ARRAY_SIZE(sensor_win_sizes))




static int sensor_enum_fmt (struct v4l2_subdev * sd, unsigned index,
                            enum v4l2_mbus_pixelcode * code)
{

  if (index >= N_FMTS)
  { return -EINVAL; }
  
  *code = sensor_formats[index].mbus_code;//linux-3.0
  return 0;
}


static int sensor_try_fmt_internal (struct v4l2_subdev * sd,
                                    struct v4l2_mbus_framefmt * fmt,
                                    struct sensor_format_struct ** ret_fmt,
                                    struct sensor_win_size ** ret_wsize)
{
  int index;
  struct sensor_win_size * wsize;
  csi_dev_dbg ("sensor_try_fmt_internal\n");
  for (index = 0; index < N_FMTS; index++)
    if (sensor_formats[index].mbus_code == fmt->code)
    { break; }
    
  if (index >= N_FMTS) {
    /* default to first format */
    index = 0;
    fmt->code = sensor_formats[0].mbus_code;//linux-3.0
  }
  
  if (ret_fmt != NULL)
  { *ret_fmt = sensor_formats + index; }
  
  /*
   * Fields: the sensor devices claim to be progressive.
   */
  fmt->field = V4L2_FIELD_NONE;//linux-3.0
  
  
  /*
   * Round requested image size down to the nearest
   * we support, but not below the smallest.
   */
  for (wsize = sensor_win_sizes; wsize < sensor_win_sizes + N_WIN_SIZES;
       wsize++)
    if (fmt->width >= wsize->width && fmt->height >= wsize->height)
    { break; }
    
  if (wsize >= sensor_win_sizes + N_WIN_SIZES)
  { wsize--; }   /* Take the smallest one */
  if (ret_wsize != NULL)
  { *ret_wsize = wsize; }
  /*
   * Note the size we'll actually handle.
   */
  fmt->width = wsize->width;//linux-3.0
  fmt->height = wsize->height;//linux-3.0
  
  return 0;
}

static int sensor_try_fmt (struct v4l2_subdev * sd,
                           struct v4l2_mbus_framefmt * fmt)
{
  return sensor_try_fmt_internal (sd, fmt, NULL, NULL);
}

/*
 * Set a format.
 */
static int sensor_s_fmt (struct v4l2_subdev * sd,
                         struct v4l2_mbus_framefmt * fmt)
{
  int ret;
  struct sensor_format_struct * sensor_fmt;
  struct sensor_win_size * wsize;
  struct sensor_info * info = to_state (sd);
  csi_dev_dbg ("sensor_s_fmt\n");
  ret = sensor_try_fmt_internal (sd, fmt, &sensor_fmt, &wsize);
  if (ret)
  { return ret; }
  
  
  sensor_write_array (sd, sensor_fmt->regs , sensor_fmt->regs_size);
  
  ret = 0;
  if (wsize->regs)
  {
    ret = sensor_write_array (sd, wsize->regs , wsize->regs_size);
    if (ret < 0)
    { return ret; }
  }
  
  if (wsize->set_size)
  {
    ret = wsize->set_size (sd);
    if (ret < 0)
    { return ret; }
  }
  
  info->fmt = sensor_fmt;
  info->width = wsize->width;
  info->height = wsize->height;
  
  return 0;
}

/*
 * Implement G/S_PARM.  There is a "high quality" mode we could try
 * to do someday; for now, we just do the frame rate tweak.
 */
static int sensor_g_parm (struct v4l2_subdev * sd, struct v4l2_streamparm * parms)
{
  struct v4l2_captureparm * cp = &parms->parm.capture;
  
  if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
  { return -EINVAL; }
  
  memset (cp, 0, sizeof (struct v4l2_captureparm) );
  cp->capability = V4L2_CAP_TIMEPERFRAME;
  cp->timeperframe.numerator = 1;
  cp->timeperframe.denominator = SENSOR_FRAME_RATE;
  
  return 0;
}

static int sensor_s_parm (struct v4l2_subdev * sd, struct v4l2_streamparm * parms)
{
  

  return -EINVAL;
}


/*
 * Code for dealing with controls.
 * fill with different sensor module
 * different sensor module has different settings here
 * if not support the follow function ,retrun -EINVAL
 */

/* *********************************************begin of ******************************************** */
static int sensor_queryctrl (struct v4l2_subdev * sd,
                             struct v4l2_queryctrl * qc)
{
  /* Fill in min, max, step and default value for these controls. */
  /* see include/linux/videodev2.h for details */
  /* see sensor_s_parm and sensor_g_parm for the meaning of value */
  
  switch (qc->id) {
  case V4L2_CID_BRIGHTNESS:
    return v4l2_ctrl_query_fill (qc, -4, 4, 1, 1);
  case V4L2_CID_CONTRAST:
    return v4l2_ctrl_query_fill (qc, -4, 4, 1, 1);
  case V4L2_CID_SATURATION:
    return v4l2_ctrl_query_fill (qc, -4, 4, 1, 1);
  case V4L2_CID_VFLIP:
  case V4L2_CID_HFLIP:
    return v4l2_ctrl_query_fill (qc, 0, 1, 1, 0);
  case V4L2_CID_EXPOSURE:
    return v4l2_ctrl_query_fill (qc, -4, 4, 1, 0);
  case V4L2_CID_EXPOSURE_AUTO:
    return v4l2_ctrl_query_fill (qc, 0, 1, 1, 0);
  case V4L2_CID_DO_WHITE_BALANCE:
    return v4l2_ctrl_query_fill (qc, 0, 5, 1, 0);
  case V4L2_CID_AUTO_WHITE_BALANCE:
    return v4l2_ctrl_query_fill (qc, 0, 1, 1, 1);
  case V4L2_CID_COLORFX:
    return v4l2_ctrl_query_fill (qc, 0, 9, 1, 0);
  case V4L2_CID_CAMERA_FLASH_MODE:
    return v4l2_ctrl_query_fill (qc, 0, 4, 1, 0);
  }
  return -EINVAL;
}

static int sensor_g_hflip (struct v4l2_subdev * sd, __s32 * value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  struct regval_list regs;
  
  regs.reg_num[0] = 0x03;
  regs.value[0] = 0x00;
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_g_hflip!\n");
    return ret;
  }
  
  regs.reg_num[0] = 0x11;
  ret = sensor_read (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_read err at sensor_g_hflip!\n");
    return ret;
  }
  
  regs.value[0] &= (1 << 0);
  regs.value[0] = regs.value[0] >> 0;
  
  *value = regs.value[0];
  info->hflip = regs.value[0];
  return 0;
}

static int sensor_s_hflip (struct v4l2_subdev * sd, int value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  struct regval_list regs;
  
  regs.reg_num[0] = 0x03;
  regs.value[0] = 0x00;
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_s_hflip!\n");
    return ret;
  }
  
  regs.reg_num[0] = 0x11;
  ret = sensor_read (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_read err at sensor_s_hflip!\n");
    return ret;
  }
  
  switch (value) {
  case 0:
    regs.value[0] &= 0xfe;
    break;
  case 1:
    regs.value[0] |= 0x01;
    break;
  default:
    return -EINVAL;
  }
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_s_hflip!\n");
    return ret;
  }
  msleep (100);
  info->hflip = value;
  return 0;
}

static int sensor_g_vflip (struct v4l2_subdev * sd, __s32 * value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  struct regval_list regs;
  
  regs.reg_num[0] = 0x03;
  regs.value[0] = 0x00;
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_g_vflip!\n");
    return ret;
  }
  
  regs.reg_num[0] = 0x11;
  ret = sensor_read (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_read err at sensor_g_vflip!\n");
    return ret;
  }
  
  regs.value[0] &= (1 << 1);
  regs.value[0] = regs.value[0] >> 1;
  
  *value = regs.value[0];
  info->hflip = regs.value[0];
  return 0;
}

static int sensor_s_vflip (struct v4l2_subdev * sd, int value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  struct regval_list regs;
  
  regs.reg_num[0] = 0x03;
  regs.value[0] = 0x00;
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_s_vflip!\n");
    return ret;
  }
  
  regs.reg_num[0] = 0x11;
  ret = sensor_read (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_read err at sensor_s_vflip!\n");
    return ret;
  }
  
  switch (value) {
  case 0:
    regs.value[0] &= 0xfd;
    break;
  case 1:
    regs.value[0] |= 0x02;
    break;
  default:
    return -EINVAL;
  }
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_g_vflip!\n");
    return ret;
  }
  msleep (100);
  info->hflip = value;
  return 0;
}

static int sensor_g_autogain (struct v4l2_subdev * sd, __s32 * value)
{
  return -EINVAL;
}

static int sensor_s_autogain (struct v4l2_subdev * sd, int value)
{
  return -EINVAL;
}

static int sensor_g_autoexp (struct v4l2_subdev * sd, __s32 * value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  struct regval_list regs;
  
  regs.reg_num[0] = 0x03;
  regs.value[0] = 0x20;  
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_g_autoexp!\n");
    return ret;
  }
  
  regs.reg_num[0] = 0x10;
  ret = sensor_read (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_read err at sensor_g_autoexp!\n");
    return ret;
  }
  
  regs.value[0] &= 0x80;
  if (regs.value[0] == 0x80) {
    *value = V4L2_EXPOSURE_AUTO;
  }
  else
  {
    *value = V4L2_EXPOSURE_MANUAL;
  }
  
  info->autoexp = *value;
  return 0;
}

static int sensor_s_autoexp (struct v4l2_subdev * sd,
                             enum v4l2_exposure_auto_type value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  struct regval_list regs;
  
  regs.reg_num[0] = 0x03;
  regs.value[0] = 0x20;  
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_s_autoexp!\n");
    return ret;
  }
  
  regs.reg_num[0] = 0x10;
  ret = sensor_read (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_read err at sensor_s_autoexp!\n");
    return ret;
  }
  
  switch (value) {
  case V4L2_EXPOSURE_AUTO:
    regs.value[0] |= 0x80;
    break;
  case V4L2_EXPOSURE_MANUAL:
    regs.value[0] &= 0x7f;
    break;
  case V4L2_EXPOSURE_SHUTTER_PRIORITY:
    return -EINVAL;
  case V4L2_EXPOSURE_APERTURE_PRIORITY:
    return -EINVAL;
  default:
    return -EINVAL;
  }
  
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_s_autoexp!\n");
    return ret;
  }
  msleep (10);
  info->autoexp = value;
  
  return 0;
}

static int sensor_g_autowb (struct v4l2_subdev * sd, int * value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  struct regval_list regs;
  
  regs.reg_num[0] = 0x03;
  regs.value[0] = 0x22;
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_g_autowb!\n");
    return ret;
  }
  
  regs.reg_num[0] = 0x10;
  ret = sensor_read (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_read err at sensor_g_autowb!\n");
    return ret;
  }
  
  regs.value[0] &= (1 << 7);
  regs.value[0] = regs.value[0] >> 7;
  
  *value = regs.value[0];
  info->autowb = *value;
  
  return 0;
}

static int sensor_s_autowb (struct v4l2_subdev * sd, int value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  struct regval_list regs;
  
  ret = sensor_write_array (sd, sensor_wb_auto_regs, ARRAY_SIZE (sensor_wb_auto_regs) );
  if (ret < 0) {
    csi_dev_err ("sensor_write_array err at sensor_s_autowb!\n");
    return ret;
  }
  
  regs.reg_num[0] = 0x10;
  ret = sensor_read (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_read err at sensor_s_autowb!\n");
    return ret;
  }
  
  switch (value) {
  case 0:
    regs.value[0] &= 0x7f;
    break;
  case 1:
    regs.value[0] |= 0x80;
    break;
  default:
    break;
  }
  ret = sensor_write (sd, regs.reg_num, regs.value);
  if (ret < 0) {
    csi_dev_err ("sensor_write err at sensor_s_autowb!\n");
    return ret;
  }
  msleep (10);
  info->autowb = value;
  return 0;
}

static int sensor_g_hue (struct v4l2_subdev * sd, __s32 * value)
{
  return -EINVAL;
}

static int sensor_s_hue (struct v4l2_subdev * sd, int value)
{
  return -EINVAL;
}

static int sensor_g_gain (struct v4l2_subdev * sd, __s32 * value)
{
  return -EINVAL;
}

static int sensor_s_gain (struct v4l2_subdev * sd, int value)
{
  return -EINVAL;
}
/* *********************************************end of ******************************************** */

static int sensor_g_brightness (struct v4l2_subdev * sd, __s32 * value)
{
  struct sensor_info * info = to_state (sd);
  
  *value = info->brightness;
  return 0;
}

static int sensor_s_brightness (struct v4l2_subdev * sd, int value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  
  switch (value) {
  case -4:
    ret = sensor_write_array (sd, sensor_brightness_neg4_regs, ARRAY_SIZE (sensor_brightness_neg4_regs) );
    break;
  case -3:
    ret = sensor_write_array (sd, sensor_brightness_neg3_regs, ARRAY_SIZE (sensor_brightness_neg3_regs) );
    break;
  case -2:
    ret = sensor_write_array (sd, sensor_brightness_neg2_regs, ARRAY_SIZE (sensor_brightness_neg2_regs) );
    break;
  case -1:
    ret = sensor_write_array (sd, sensor_brightness_neg1_regs, ARRAY_SIZE (sensor_brightness_neg1_regs) );
    break;
  case 0:
    ret = sensor_write_array (sd, sensor_brightness_zero_regs, ARRAY_SIZE (sensor_brightness_zero_regs) );
    break;
  case 1:
    ret = sensor_write_array (sd, sensor_brightness_pos1_regs, ARRAY_SIZE (sensor_brightness_pos1_regs) );
    break;
  case 2:
    ret = sensor_write_array (sd, sensor_brightness_pos2_regs, ARRAY_SIZE (sensor_brightness_pos2_regs) );
    break;
  case 3:
    ret = sensor_write_array (sd, sensor_brightness_pos3_regs, ARRAY_SIZE (sensor_brightness_pos3_regs) );
    break;
  case 4:
    ret = sensor_write_array (sd, sensor_brightness_pos4_regs, ARRAY_SIZE (sensor_brightness_pos4_regs) );
    break;
  default:
    return -EINVAL;
  }
  
  if (ret < 0) {
    csi_dev_err ("sensor_write_array err at sensor_s_brightness!\n");
    return ret;
  }
  msleep (10);
  info->brightness = value;
  return 0;
}

static int sensor_g_contrast (struct v4l2_subdev * sd, __s32 * value)
{
  struct sensor_info * info = to_state (sd);
  
  *value = info->contrast;
  return 0;
}

static int sensor_s_contrast (struct v4l2_subdev * sd, int value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  
  switch (value) {
  case -4:
    ret = sensor_write_array (sd, sensor_contrast_neg4_regs, ARRAY_SIZE (sensor_contrast_neg4_regs) );
    break;
  case -3:
    ret = sensor_write_array (sd, sensor_contrast_neg3_regs, ARRAY_SIZE (sensor_contrast_neg3_regs) );
    break;
  case -2:
    ret = sensor_write_array (sd, sensor_contrast_neg2_regs, ARRAY_SIZE (sensor_contrast_neg2_regs) );
    break;
  case -1:
    ret = sensor_write_array (sd, sensor_contrast_neg1_regs, ARRAY_SIZE (sensor_contrast_neg1_regs) );
    break;
  case 0:
    ret = sensor_write_array (sd, sensor_contrast_zero_regs, ARRAY_SIZE (sensor_contrast_zero_regs) );
    break;
  case 1:
    ret = sensor_write_array (sd, sensor_contrast_pos1_regs, ARRAY_SIZE (sensor_contrast_pos1_regs) );
    break;
  case 2:
    ret = sensor_write_array (sd, sensor_contrast_pos2_regs, ARRAY_SIZE (sensor_contrast_pos2_regs) );
    break;
  case 3:
    ret = sensor_write_array (sd, sensor_contrast_pos3_regs, ARRAY_SIZE (sensor_contrast_pos3_regs) );
    break;
  case 4:
    ret = sensor_write_array (sd, sensor_contrast_pos4_regs, ARRAY_SIZE (sensor_contrast_pos4_regs) );
    break;
  default:
    return -EINVAL;
  }
  
  if (ret < 0) {
    csi_dev_err ("sensor_write_array err at sensor_s_contrast!\n");
    return ret;
  }
  msleep (10);
  info->contrast = value;
  return 0;
}

static int sensor_g_saturation (struct v4l2_subdev * sd, __s32 * value)
{
  struct sensor_info * info = to_state (sd);
  
  *value = info->saturation;
  return 0;
}

static int sensor_s_saturation (struct v4l2_subdev * sd, int value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  
  switch (value) {
  case -4:
    ret = sensor_write_array (sd, sensor_saturation_neg4_regs, ARRAY_SIZE (sensor_saturation_neg4_regs) );
    break;
  case -3:
    ret = sensor_write_array (sd, sensor_saturation_neg3_regs, ARRAY_SIZE (sensor_saturation_neg3_regs) );
    break;
  case -2:
    ret = sensor_write_array (sd, sensor_saturation_neg2_regs, ARRAY_SIZE (sensor_saturation_neg2_regs) );
    break;
  case -1:
    ret = sensor_write_array (sd, sensor_saturation_neg1_regs, ARRAY_SIZE (sensor_saturation_neg1_regs) );
    break;
  case 0:
    ret = sensor_write_array (sd, sensor_saturation_zero_regs, ARRAY_SIZE (sensor_saturation_zero_regs) );
    break;
  case 1:
    ret = sensor_write_array (sd, sensor_saturation_pos1_regs, ARRAY_SIZE (sensor_saturation_pos1_regs) );
    break;
  case 2:
    ret = sensor_write_array (sd, sensor_saturation_pos2_regs, ARRAY_SIZE (sensor_saturation_pos2_regs) );
    break;
  case 3:
    ret = sensor_write_array (sd, sensor_saturation_pos3_regs, ARRAY_SIZE (sensor_saturation_pos3_regs) );
    break;
  case 4:
    ret = sensor_write_array (sd, sensor_saturation_pos4_regs, ARRAY_SIZE (sensor_saturation_pos4_regs) );
    break;
  default:
    return -EINVAL;
  }
  
  if (ret < 0) {
    csi_dev_err ("sensor_write_array err at sensor_s_saturation!\n");
    return ret;
  }
  msleep (10);
  info->saturation = value;
  return 0;
}

static int sensor_g_exp (struct v4l2_subdev * sd, __s32 * value)
{
  struct sensor_info * info = to_state (sd);
  
  *value = info->exp;
  return 0;
}

static int sensor_s_exp (struct v4l2_subdev * sd, int value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  
  switch (value) {
  case -4:
    ret = sensor_write_array (sd, sensor_ev_neg4_regs, ARRAY_SIZE (sensor_ev_neg4_regs) );
    break;
  case -3:
    ret = sensor_write_array (sd, sensor_ev_neg3_regs, ARRAY_SIZE (sensor_ev_neg3_regs) );
    break;
  case -2:
    ret = sensor_write_array (sd, sensor_ev_neg2_regs, ARRAY_SIZE (sensor_ev_neg2_regs) );
    break;
  case -1:
    ret = sensor_write_array (sd, sensor_ev_neg1_regs, ARRAY_SIZE (sensor_ev_neg1_regs) );
    break;
  case 0:
    ret = sensor_write_array (sd, sensor_ev_zero_regs, ARRAY_SIZE (sensor_ev_zero_regs) );
    break;
  case 1:
    ret = sensor_write_array (sd, sensor_ev_pos1_regs, ARRAY_SIZE (sensor_ev_pos1_regs) );
    break;
  case 2:
    ret = sensor_write_array (sd, sensor_ev_pos2_regs, ARRAY_SIZE (sensor_ev_pos2_regs) );
    break;
  case 3:
    ret = sensor_write_array (sd, sensor_ev_pos3_regs, ARRAY_SIZE (sensor_ev_pos3_regs) );
    break;
  case 4:
    ret = sensor_write_array (sd, sensor_ev_pos4_regs, ARRAY_SIZE (sensor_ev_pos4_regs) );
    break;
  default:
    return -EINVAL;
  }
  
  if (ret < 0) {
    csi_dev_err ("sensor_write_array err at sensor_s_exp!\n");
    return ret;
  }
  msleep (10);
  info->exp = value;
  return 0;
}

static int sensor_g_wb (struct v4l2_subdev * sd, int * value)
{
  struct sensor_info * info = to_state (sd);
  enum v4l2_whiteblance * wb_type = (enum v4l2_whiteblance *) value;
  
  *wb_type = info->wb;
  
  return 0;
}

static int sensor_s_wb (struct v4l2_subdev * sd,
                        enum v4l2_whiteblance value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  
  if (value == V4L2_WB_AUTO) {
    ret = sensor_s_autowb (sd, 1);
    return ret;
  }
  else {
    ret = sensor_s_autowb (sd, 0);
    if (ret < 0) {
      csi_dev_err ("sensor_s_autowb error, return %x!\n", ret);
      return ret;
    }
    
    switch (value) {
    case V4L2_WB_CLOUD:
      ret = sensor_write_array (sd, sensor_wb_cloud_regs, ARRAY_SIZE (sensor_wb_cloud_regs) );
      break;
    case V4L2_WB_DAYLIGHT:
      ret = sensor_write_array (sd, sensor_wb_daylight_regs, ARRAY_SIZE (sensor_wb_daylight_regs) );
      break;
    case V4L2_WB_INCANDESCENCE:
      ret = sensor_write_array (sd, sensor_wb_incandescence_regs, ARRAY_SIZE (sensor_wb_incandescence_regs) );
      break;
    case V4L2_WB_FLUORESCENT:
      ret = sensor_write_array (sd, sensor_wb_fluorescent_regs, ARRAY_SIZE (sensor_wb_fluorescent_regs) );
      break;
    case V4L2_WB_TUNGSTEN:
      ret = sensor_write_array (sd, sensor_wb_tungsten_regs, ARRAY_SIZE (sensor_wb_tungsten_regs) );
      break;
    default:
      return -EINVAL;
    }
  }
  
  if (ret < 0) {
    csi_dev_err ("sensor_s_wb error, return %x!\n", ret);
    return ret;
  }
  msleep (10);
  info->wb = value;
  return 0;
}

static int sensor_g_colorfx (struct v4l2_subdev * sd,
                             __s32 * value)
{
  struct sensor_info * info = to_state (sd);
  enum v4l2_colorfx * clrfx_type = (enum v4l2_colorfx *) value;
  
  *clrfx_type = info->clrfx;
  return 0;
}

static int sensor_s_colorfx (struct v4l2_subdev * sd,
                             enum v4l2_colorfx value)
{
  int ret;
  struct sensor_info * info = to_state (sd);
  
  switch (value) {
  case V4L2_COLORFX_NONE:
    ret = sensor_write_array (sd, sensor_colorfx_none_regs, ARRAY_SIZE (sensor_colorfx_none_regs) );
    break;
  case V4L2_COLORFX_BW:
    ret = sensor_write_array (sd, sensor_colorfx_bw_regs, ARRAY_SIZE (sensor_colorfx_bw_regs) );
    break;
  case V4L2_COLORFX_SEPIA:
    ret = sensor_write_array (sd, sensor_colorfx_sepia_regs, ARRAY_SIZE (sensor_colorfx_sepia_regs) );
    break;
  case V4L2_COLORFX_NEGATIVE:
    ret = sensor_write_array (sd, sensor_colorfx_negative_regs, ARRAY_SIZE (sensor_colorfx_negative_regs) );
    break;
  case V4L2_COLORFX_EMBOSS:
    ret = sensor_write_array (sd, sensor_colorfx_emboss_regs, ARRAY_SIZE (sensor_colorfx_emboss_regs) );
    break;
  case V4L2_COLORFX_SKETCH:
    ret = sensor_write_array (sd, sensor_colorfx_sketch_regs, ARRAY_SIZE (sensor_colorfx_sketch_regs) );
    break;
  case V4L2_COLORFX_SKY_BLUE:
    ret = sensor_write_array (sd, sensor_colorfx_sky_blue_regs, ARRAY_SIZE (sensor_colorfx_sky_blue_regs) );
    break;
  case V4L2_COLORFX_GRASS_GREEN:
    ret = sensor_write_array (sd, sensor_colorfx_grass_green_regs, ARRAY_SIZE (sensor_colorfx_grass_green_regs) );
    break;
  case V4L2_COLORFX_SKIN_WHITEN:
    ret = sensor_write_array (sd, sensor_colorfx_skin_whiten_regs, ARRAY_SIZE (sensor_colorfx_skin_whiten_regs) );
    break;
  case V4L2_COLORFX_VIVID:
    ret = sensor_write_array (sd, sensor_colorfx_vivid_regs, ARRAY_SIZE (sensor_colorfx_vivid_regs) );
    break;
  default:
    return -EINVAL;
  }
  
  if (ret < 0) {
    csi_dev_err ("sensor_s_colorfx error, return %x!\n", ret);
    return ret;
  }
  msleep (10);
  info->clrfx = value;
  return 0;
}

static int sensor_g_flash_mode (struct v4l2_subdev * sd,
                                __s32 * value)
{
  struct sensor_info * info = to_state (sd);
  enum v4l2_flash_mode * flash_mode = (enum v4l2_flash_mode *) value;
  
  *flash_mode = info->flash_mode;
  return 0;
}

static int sensor_s_flash_mode (struct v4l2_subdev * sd,
                                enum v4l2_flash_mode value)
{
  struct sensor_info * info = to_state (sd);
  struct csi_dev * dev = (struct csi_dev *) dev_get_drvdata (sd->v4l2_dev->dev);
  char csi_flash_str[32];
  int flash_on, flash_off;
  
  if (info->ccm_info->iocfg == 0) {
    strcpy (csi_flash_str, "csi_flash");
  }
  else
    if (info->ccm_info->iocfg == 1) {
      strcpy (csi_flash_str, "csi_flash_b");
    }
    
  flash_on = (dev->flash_pol != 0) ? 1 : 0;
  flash_off = (flash_on == 1) ? 0 : 1;
  
  switch (value) {
  case V4L2_FLASH_MODE_OFF:
    gpio_write_one_pin_value (dev->csi_pin_hd, flash_off, csi_flash_str);
    break;
  case V4L2_FLASH_MODE_AUTO:
    return -EINVAL;
    break;
  case V4L2_FLASH_MODE_ON:
    gpio_write_one_pin_value (dev->csi_pin_hd, flash_on, csi_flash_str);
    break;
  case V4L2_FLASH_MODE_TORCH:
    return -EINVAL;
    break;
  case V4L2_FLASH_MODE_RED_EYE:
    return -EINVAL;
    break;
  default:
    return -EINVAL;
  }
  
  info->flash_mode = value;
  return 0;
}

static int sensor_g_ctrl (struct v4l2_subdev * sd, struct v4l2_control * ctrl)
{
  switch (ctrl->id) {
  case V4L2_CID_BRIGHTNESS:
    return sensor_g_brightness (sd, &ctrl->value);
  case V4L2_CID_CONTRAST:
    return sensor_g_contrast (sd, &ctrl->value);
  case V4L2_CID_SATURATION:
    return sensor_g_saturation (sd, &ctrl->value);
  case V4L2_CID_HUE:
    return sensor_g_hue (sd, &ctrl->value);
  case V4L2_CID_VFLIP:
    return sensor_g_vflip (sd, &ctrl->value);
  case V4L2_CID_HFLIP:
    return sensor_g_hflip (sd, &ctrl->value);
  case V4L2_CID_GAIN:
    return sensor_g_gain (sd, &ctrl->value);
  case V4L2_CID_AUTOGAIN:
    return sensor_g_autogain (sd, &ctrl->value);
  case V4L2_CID_EXPOSURE:
    return sensor_g_exp (sd, &ctrl->value);
  case V4L2_CID_EXPOSURE_AUTO:
    return sensor_g_autoexp (sd, &ctrl->value);
  case V4L2_CID_DO_WHITE_BALANCE:
    return sensor_g_wb (sd, &ctrl->value);
  case V4L2_CID_AUTO_WHITE_BALANCE:
    return sensor_g_autowb (sd, &ctrl->value);
  case V4L2_CID_COLORFX:
    return sensor_g_colorfx (sd, &ctrl->value);
  case V4L2_CID_CAMERA_FLASH_MODE:
    return sensor_g_flash_mode (sd, &ctrl->value);
  }
  return -EINVAL;
}

static int sensor_s_ctrl (struct v4l2_subdev * sd, struct v4l2_control * ctrl)
{
  switch (ctrl->id) {
  case V4L2_CID_BRIGHTNESS:
    return sensor_s_brightness (sd, ctrl->value);
  case V4L2_CID_CONTRAST:
    return sensor_s_contrast (sd, ctrl->value);
  case V4L2_CID_SATURATION:
    return sensor_s_saturation (sd, ctrl->value);
  case V4L2_CID_HUE:
    return sensor_s_hue (sd, ctrl->value);
  case V4L2_CID_VFLIP:
    return sensor_s_vflip (sd, ctrl->value);
  case V4L2_CID_HFLIP:
    return sensor_s_hflip (sd, ctrl->value);
  case V4L2_CID_GAIN:
    return sensor_s_gain (sd, ctrl->value);
  case V4L2_CID_AUTOGAIN:
    return sensor_s_autogain (sd, ctrl->value);
  case V4L2_CID_EXPOSURE:
    return sensor_s_exp (sd, ctrl->value);
  case V4L2_CID_EXPOSURE_AUTO:
    return sensor_s_autoexp (sd,
                             (enum v4l2_exposure_auto_type) ctrl->value);
  case V4L2_CID_DO_WHITE_BALANCE:
    return sensor_s_wb (sd,
                        (enum v4l2_whiteblance) ctrl->value);
  case V4L2_CID_AUTO_WHITE_BALANCE:
    return sensor_s_autowb (sd, ctrl->value);
  case V4L2_CID_COLORFX:
    return sensor_s_colorfx (sd,
                             (enum v4l2_colorfx) ctrl->value);
  case V4L2_CID_CAMERA_FLASH_MODE:
    return sensor_s_flash_mode (sd,
                                (enum v4l2_flash_mode) ctrl->value);
  }
  return -EINVAL;
}

static int sensor_g_chip_ident (struct v4l2_subdev * sd,
                                struct v4l2_dbg_chip_ident * chip)
{
  struct i2c_client * client = v4l2_get_subdevdata (sd);
  
  return v4l2_chip_ident_i2c_client (client, chip, V4L2_IDENT_SENSOR, 0);
}


/* ----------------------------------------------------------------------- */

static const struct v4l2_subdev_core_ops sensor_core_ops = {
  .g_chip_ident = sensor_g_chip_ident,
  .g_ctrl = sensor_g_ctrl,
  .s_ctrl = sensor_s_ctrl,
  .queryctrl = sensor_queryctrl,
  .reset = sensor_reset,
  .init = sensor_init,
  .s_power = sensor_power,
  .ioctl = sensor_ioctl,
};

static const struct v4l2_subdev_video_ops sensor_video_ops = {
  .enum_mbus_fmt = sensor_enum_fmt,//linux-3.0
  .try_mbus_fmt = sensor_try_fmt,//linux-3.0
  .s_mbus_fmt = sensor_s_fmt,//linux-3.0
  .s_parm = sensor_s_parm,//linux-3.0
  .g_parm = sensor_g_parm,//linux-3.0
};

static const struct v4l2_subdev_ops sensor_ops = {
  .core = &sensor_core_ops,
  .video = &sensor_video_ops,
};

/* ----------------------------------------------------------------------- */

static int sensor_probe (struct i2c_client * client,
                         const struct i2c_device_id * id)
{
  struct v4l2_subdev * sd;
  struct sensor_info * info;

  info = kzalloc (sizeof (struct sensor_info), GFP_KERNEL);
  if (info == NULL)
  { return -ENOMEM; }
  sd = &info->sd;
  v4l2_i2c_subdev_init (sd, client, &sensor_ops);
  
  info->fmt = &sensor_formats[0];
  info->ccm_info = &ccm_info_con;
  
  info->brightness = 0;
  info->contrast = 0;
  info->saturation = 0;
  info->hue = 0;
  info->hflip = 0;
  info->vflip = 0;
  info->gain = 0;
  info->autogain = 1;
  info->exp = 0;
  info->autoexp = 0;
  info->autowb = 1;
  info->wb = 0;
  info->clrfx = 0;
  

  return 0;
}


static int sensor_remove (struct i2c_client * client)
{
  struct v4l2_subdev * sd = i2c_get_clientdata (client);
  
  v4l2_device_unregister_subdev (sd);
  kfree (to_state (sd) );
  return 0;
}

static const struct i2c_device_id sensor_id[] = {
  { "hi253", 0 },
  { }
};
MODULE_DEVICE_TABLE (i2c, sensor_id);

static struct i2c_driver sensor_driver = {
  .driver = {
    .owner = THIS_MODULE,
    .name = "hi253",
  },
  .probe = sensor_probe,
  .remove = sensor_remove,
  .id_table = sensor_id,
};
static __init int init_sensor (void)
{
  return i2c_add_driver (&sensor_driver);
}

static __exit void exit_sensor (void)
{
  i2c_del_driver (&sensor_driver);
}

module_init (init_sensor);
module_exit (exit_sensor);
