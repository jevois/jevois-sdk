/*
 JeVois Smart Embedded Machine Vision Toolkit - Copyright (C) 2018 by Laurent Itti, the University of Southern
 California (USC), and iLab at USC. See http://iLab.usc.edu and http:jevois.org for information about this project.

 This file is part of the JeVois Smart Embedded Machine Vision Toolkit.  This program is free software; you can
 redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software
 Foundation, version 2.  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.  You should have received a copy of the GNU General Public License along with this program;
 if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

 Contact information: Laurent Itti - 3641 Watt Way, HNB-07A - Los Angeles, CA 90089-2520 - USA.
 Tel: +1 213 740 3527 - itti@pollux.usc.edu - http://iLab.usc.edu - http://jevois.org
*/

/*
 * A V4L2 driver for ov2640 cameras.
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
#include <media/v4l2-mediabus.h>
#include <linux/io.h>


#include "camera.h"
#include "ov2640_regs.h"

MODULE_AUTHOR("jevois");
MODULE_DESCRIPTION("A low-level driver for ov2640 sensors");
MODULE_LICENSE("GPL");

#define AF_WIN_NEW_COORD
#define DEV_DBG_EN      1
#if(DEV_DBG_EN == 1)
#define vfe_dev_dbg(x,arg...) printk("[OV2640]"x,##arg)
#else
#define vfe_dev_dbg(x,arg...)
#endif
#define vfe_dev_err(x,arg...) printk("[OV2640]"x,##arg)
#define vfe_dev_print(x,arg...) printk("[OV2640]"x,##arg)

#define LOG_ERR_RET(x)  {                                           \
    int ret;                                                        \
    ret = x;                                                        \
    if(ret < 0) {                                                   \
      vfe_dev_err("error at %s\n",__func__);                        \
      return ret;                                                   \
    }                                                               \
  }

#define SENSOR_WRITE(reg)                                               \
  ret = sensor_write(sd, reg, val);                                     \
  if (ret < 0) { vfe_dev_err("Error writing %u to sensor register %u in %s\n", val, reg, __func__); return ret; }

#define SENSOR_READ(reg)                                                \
  ret = sensor_read(sd, reg, &val);                                     \
  if (ret < 0) { vfe_dev_err("Error reading from sensor register %u in %s\n", reg, __func__); return ret;	}

#define SENSOR_BANK(bank)                                               \
  ret = sensor_write(sd, BANK_SEL, bank);                               \
  if (ret < 0) { vfe_dev_err("Error selecting bank %u in %s\n", bank, __func__); return ret; }

#define MCLK              (24*1000*1000)
#define VREF_POL          V4L2_MBUS_VSYNC_ACTIVE_HIGH
#define HREF_POL          V4L2_MBUS_HSYNC_ACTIVE_HIGH
#define CLK_POL           V4L2_MBUS_PCLK_SAMPLE_RISING
#define V4L2_IDENT_SENSOR 0x2640

#define CSI_STBY_ON     1
#define CSI_STBY_OFF    0
#define CSI_RST_ON      0
#define CSI_RST_OFF     1
#define CSI_PWR_ON      1
#define CSI_PWR_OFF     0

#define regval_list reg_list_a8_d8

#define SENSOR_FRAME_RATE 30

#define I2C_ADDR 0x60
#define SENSOR_NAME "ov2640"

static struct v4l2_subdev *glb_sd;

struct sensor_format_struct;  /* coming later */

struct cfg_array {
	struct regval_list * regs;
	int size;
};

static inline struct sensor_info *to_state(struct v4l2_subdev *sd)
{
  return container_of(sd, struct sensor_info, sd);
}

static struct regval_list sensor_default_regs[] = {
  {0xff,0x01},
  {0x12,0x80},
  {0xff,0x00},
  {0x2c,0xff},
  {0x2e,0xdf},
  {0xff,0x01},
  
  {0x03,0x0f},// 0x8f peak
  {0x0f,0x4b},
  
  
  {0x3c,0x32},
  {0x11,0x00},
  {0x09,0x01},//03},
  {0x04,0xa8},//b7,b6 directs
  {0x13,0xe5},
  {0x14,0x28},
  {0x2c,0x0c},
  {0x33,0x78},
  {0x3a,0x33},
  {0x3b,0xfB},
  {0x3e,0x00},
  {0x43,0x11},
  {0x16,0x10},
  {0x39,0x02},
  {0x35,0x88},
  {0x22,0x09},
  {0x37,0x40},
  {0x23,0x00},
  {0x34,0xa0},
  {0x36,0x1a},
  {0x06,0x02},
  {0x07,0xc0},
  {0x0d,0xb7},
  {0x0e,0x01},
  {0x4c,0x00},
  {0x4a,0x81},
  {0x21,0x99},
  
  {0x24,0x48},
  {0x25,0x38},//40
  {0x26,0x82},//82
  
  {0x5c,0x00},
  {0x63,0x00},
  {0x46,0x3f},
  {0x0c,0x3c},
  {0x61,0x70},
  {0x62,0x80},
  {0x7c,0x05},
  {0x20,0x80},
  {0x28,0x30},
  {0x6c,0x00},
  {0x6d,0x80},
  {0x6e,0x00},
  {0x70,0x02},
  {0x71,0x94},
  {0x73,0xc1},
  {0x3d,0x34},
  {0x5a,0x57},
  {0x4f,0xbb},
  {0x50,0x9c},
  {0xff,0x00},
  {0xe5,0x7f},
  {0xf9,0xc0},
  {0x41,0x24},
  {0xe0,0x14},
  {0x76,0xff},
  {0x33,0xa0},
  {0x42,0x20},
  {0x43,0x18},
  {0x4c,0x00},
  {0x87,0xd0},
  {0x88,0x3f},
  {0xd7,0x03},
  {0xd9,0x10},
  {0xd3,0x82},
  {0xc8,0x08},
  {0xc9,0x80},
  {0x7c,0x00},
  {0x7d,0x02},//0x00 peak//0x07,��������?����e��????t��???��??2??
  {0x7c,0x03},
  {0x7d,0x28},//0x48//0x40 ?a???�̨�??-o��D?��?,3y��????����e��????t��?��?D�䨢?
  {0x7d,0x28},//0x48 peak//0x40 ?a???�̨�??-o��D?��?,3y��????����e��????t��?��?D�䨢?
  
  {0x7c,0x08},
  {0x7d,0x20},
  {0x7d,0x10},//0x10
  {0x7d,0x0e},//0x0e
  
  {0x7c,0x00},
  {0x7d,0x04},//0x48//0x40
  {0x7c,0x07},//0x48 peak//0x40
  {0x7d,0x20},
  {0x7d,0x1c},
  {0x7d,0x2a},//0x10
  {0x7d,0x06},//0x0e
  
  {0xff,0x00},//register page
  {0x7c,0x00},
  {0x7d,0x04},
  {0x7c,0x09},
  {0x7d,0x00},//brightness value
  {0x7d,0x00},//sign
  
  {0xff,0x00},
  {0x7c,0x00},
  {0x7d,0x00},
  {0x7c,0x05},
  {0x7d,0x80},
  {0x7d,0x80},
  
  {0x90,0x00},
  {0x91,0x0e},
  {0x91,0x1a},//e3
  {0x91,0x31},
  {0x91,0x5a},
  {0x91,0x69},
  {0x91,0x75},
  {0x91,0x7e},
  {0x91,0x88},
  {0x91,0x8f},
  {0x91,0x96},
  {0x91,0xa3},
  {0x91,0xaf},
  {0x91,0xc4},
  {0x91,0xd7},
  {0x91,0xe8},
  {0x91,0x20},
  
  {0x90,0x00},
  {0x91,0x04},
  {0x91,0x0c},
  {0x91,0x20},
  {0x91,0x4c},
  {0x91,0x60},
  {0x91,0x74},
  {0x91,0x82},
  {0x91,0x8e},
  {0x91,0x97},
  {0x91,0x9f},
  {0x91,0xaa},
  {0x91,0xb4},
  {0x91,0xc8},
  {0x91,0xd9},
  {0x91,0xe8},
  {0x91,0x20},
  
  
  
  {0x92,0x00},
  {0x93,0x06},
  {0x93,0xc8},//e3
  {0x93,0x05},
  {0x93,0x05},
  {0x93,0x00},
  {0x93,0x04},
  {0x93,0x00},
  {0x93,0x00},
  {0x93,0x00},
  {0x93,0x00},
  {0x93,0x00},
  {0x93,0x00},
  {0x93,0x00},
  
  {0x96,0x00},
  {0x97,0x08},
  {0x97,0x19},
  {0x97,0x02},
  {0x97,0x0c},
  {0x97,0x24},
  {0x97,0x30},
  {0x97,0x28},
  {0x97,0x26},
  {0x97,0x02},
  {0x97,0x98},
  {0x97,0x80},
  {0x97,0x00},
  {0x97,0x00},
  {0xc3,0xef},//ed
  {0xa4,0x00},
  {0xa8,0x00},
  
  {0xbf,0x00},
  {0xba,0xdc},
  {0xbb,0x08},
  {0xb6,0x20},
  {0xb8,0x30},
  {0xb7,0x20},
  {0xb9,0x30},
  {0xb3,0xb4},
  {0xb4,0xca},
  {0xb5,0x34},
  {0xb0,0x46},
  {0xb1,0x46},
  {0xb2,0x06},
  {0xc7,0x00},
  {0xc6,0x51},
  {0xc5,0x11},
  {0xc4,0x9c},
  {0xc0,0xc8},
  {0xc1,0x96},
  {0x86,0x3d},
  {0x50,0x92},
  {0x51,0x90},
  {0x52,0x2c},
  {0x53,0x00},
  {0x54,0x00},
  {0x55,0x88},
  {0x57,0x00},
  {0x5a,0x50},
  {0x5b,0x3c},
  {0x5c,0x00},
  {0xc3,0xed},
  {0x7f,0x00},
  {0xda,0x01},
  {0xe5,0x1f},
  {0xe1,0x67},
  {0xe0,0x00},
  {0xdd,0x7f},
  {0x05,0x00},
  
  {0xff,0x01},
  {0x5d,0x55},//0x00
  {0x5e,0x55},//0x3c
  {0x5f,0x55},//0x28
  {0x60,0x55},//0x55
  
  {0xff,0x00},
  {0xc3,0xef},
  {0xa6,0x00},
  {0xa7,0x0f},
  {0xa7,0x4e},
  {0xa7,0x7a},
  {0xa7,0x33},
  {0xa7,0x00},
  {0xa7,0x23},
  {0xa7,0x27},
  {0xa7,0x3a},
  {0xa7,0x70},
  {0xa7,0x33},
  {0xa7,0x00},//L
  {0xa7,0x23},
  {0xa7,0x20},
  {0xa7,0x0c},
  {0xa7,0x66},
  {0xa7,0x33},
  {0xa7,0x00},
  {0xa7,0x23},
  {0xc3,0xef},
  
  {0xa6,0x00},
  {0xa7,0x20},
  {0xa7,0x00},
  {0xa7,0x20},//red
  {0xa7,0x31},
  {0xa7,0x10},
  {0xa7,0x28},
  {0xa7,0x00},
  {0xa7,0x20},
  {0xa7,0x1a},//green
  {0xa7,0x31},
  {0xa7,0x10},
  {0xa7,0x28},
  {0xa7,0x00},
  {0xa7,0x20},
  {0xa7,0x19},//blue
  {0xa7,0x31},
  {0xa7,0x10},
  {0xa7,0x28},
  
  {0xff,0x00},
  {0x92,0x00},
  {0x93,0x06},
  {0x93,0xe3},//e
  {0x93,0x05},
  {0x93,0x03},
  {0x93,0x00},
  {0x93,0x04},
  
  
  {0xe0, 0x04},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x86, 0x3d},
  {0x50, 0x89},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x57, 0x00},
  {0x5a, 0xa0},
  {0x5b, 0x78},
  {0x5c, 0x00},
  {0xd3, 0x82},
  {0xe0, 0x00},

  /*
	{ BANK_SEL, BANK_SEL_DSP },
	{ 0x2c,   0xff },
	{ 0x2e,   0xdf },
	{ BANK_SEL, BANK_SEL_SENS },
	{ 0x3c,   0x32 },
	{ CLKRC,  CLKRC_DIV_SET(1) },
	{ COM2,   COM2_OCAP_Nx_SET(3) },
	{ REG04,  REG04_DEF | REG04_HREF_EN },
	{ COM8,   COM8_DEF | COM8_BNDF_EN | COM8_AGC_EN | COM8_AEC_EN },
	{ COM9,   COM9_AGC_GAIN_8x | 0x08},
	{ 0x2c,   0x0c },
	{ 0x33,   0x78 },
	{ 0x3a,   0x33 },
	{ 0x3b,   0xfb },
	{ 0x3e,   0x00 },
	{ 0x43,   0x11 },
	{ 0x16,   0x10 },
	{ 0x39,   0x02 },
	{ 0x35,   0x88 },
	{ 0x22,   0x0a },
	{ 0x37,   0x40 },
	{ 0x23,   0x00 },
	{ ARCOM2, 0xa0 },
	{ 0x06,   0x02 },
	{ 0x06,   0x88 },
	{ 0x07,   0xc0 },
	{ 0x0d,   0xb7 },
	{ 0x0e,   0x01 },
	{ 0x4c,   0x00 },
	{ 0x4a,   0x81 },
	{ 0x21,   0x99 },
	{ AEW,    0x40 },
	{ AEB,    0x38 },
	{ VV,     VV_HIGH_TH_SET(0x08) | VV_LOW_TH_SET(0x02) },
	{ 0x5c,   0x00 },
	{ 0x63,   0x00 },
	{ FLL,    0x22 },
	{ COM3,   0x38 | COM3_BAND_AUTO },
	{ REG5D,  0x55 },
	{ REG5E,  0x7d },
	{ REG5F,  0x7d },
	{ REG60,  0x55 },
	{ HISTO_LOW,   0x70 },
	{ HISTO_HIGH,  0x80 },
	{ 0x7c,   0x05 },
	{ 0x20,   0x80 },
	{ 0x28,   0x30 },
	{ 0x6c,   0x00 },
	{ 0x6d,   0x80 },
	{ 0x6e,   0x00 },
	{ 0x70,   0x02 },
	{ 0x71,   0x94 },
	{ 0x73,   0xc1 },
	{ 0x3d,   0x34 },
	{ COM7,   COM7_RES_UXGA | COM7_ZOOM_EN },
	{ REG5A,  BD50_MAX_AEC_STEP_SET(6) | BD60_MAX_AEC_STEP_SET(8) },
	{ COM25,  COM25_50HZ_BANDING_AEC_MSBS_SET(0x0bb) | COM25_60HZ_BANDING_AEC_MSBS_SET(0x09c) },
	{ BD50,   BD50_50HZ_BANDING_AEC_LSBS_SET(0x0bb) },
	{ BD60,   BD60_60HZ_BANDING_AEC_LSBS_SET(0x09c) },
	{ BANK_SEL,  BANK_SEL_DSP },
	{ 0xe5,   0x7f },
	{ MC_BIST,  MC_BIST_RESET | MC_BIST_BOOT_ROM_SEL },
	{ 0x41,   0x24 },
	{ RESET,  RESET_JPEG | RESET_DVP },
	{ 0x76,   0xff },
	{ 0x33,   0xa0 },
	{ 0x42,   0x20 },
	{ 0x43,   0x18 },
	{ 0x4c,   0x00 },
	{ CTRL3,  CTRL3_BPC_EN | CTRL3_WPC_EN | 0x10 },
	{ 0x88,   0x3f },
	{ 0xd7,   0x03 },
	{ 0xd9,   0x10 },
	{ R_DVP_SP,  R_DVP_SP_AUTO_MODE | 0x2 },
	{ 0xc8,   0x08 },
	{ 0xc9,   0x80 },
	{ BPADDR, 0x00 },
	{ BPDATA, 0x00 },
	{ BPADDR, 0x03 },
	{ BPDATA, 0x48 },
	{ BPDATA, 0x48 },
	{ BPADDR, 0x08 },
	{ BPDATA, 0x20 },
	{ BPDATA, 0x10 },
	{ BPDATA, 0x0e },
	{ 0x90,   0x00 },
	{ 0x91,   0x0e },
	{ 0x91,   0x1a },
	{ 0x91,   0x31 },
	{ 0x91,   0x5a },
	{ 0x91,   0x69 },
	{ 0x91,   0x75 },
	{ 0x91,   0x7e },
	{ 0x91,   0x88 },
	{ 0x91,   0x8f },
	{ 0x91,   0x96 },
	{ 0x91,   0xa3 },
	{ 0x91,   0xaf },
	{ 0x91,   0xc4 },
	{ 0x91,   0xd7 },
	{ 0x91,   0xe8 },
	{ 0x91,   0x20 },
	{ 0x92,   0x00 },
	{ 0x93,   0x06 },
	{ 0x93,   0xe3 },
	{ 0x93,   0x03 },
	{ 0x93,   0x03 },
	{ 0x93,   0x00 },
	{ 0x93,   0x02 },
	{ 0x93,   0x00 },
	{ 0x93,   0x00 },
	{ 0x93,   0x00 },
	{ 0x93,   0x00 },
	{ 0x93,   0x00 },
	{ 0x93,   0x00 },
	{ 0x93,   0x00 },
	{ 0x96,   0x00 },
	{ 0x97,   0x08 },
	{ 0x97,   0x19 },
	{ 0x97,   0x02 },
	{ 0x97,   0x0c },
	{ 0x97,   0x24 },
	{ 0x97,   0x30 },
	{ 0x97,   0x28 },
	{ 0x97,   0x26 },
	{ 0x97,   0x02 },
	{ 0x97,   0x98 },
	{ 0x97,   0x80 },
	{ 0x97,   0x00 },
	{ 0x97,   0x00 },
	{ 0xa4,   0x00 },
	{ 0xa8,   0x00 },
	{ 0xc5,   0x11 },
	{ 0xc6,   0x51 },
	{ 0xbf,   0x80 },
	{ 0xc7,   0x10 },
	{ 0xb6,   0x66 },
	{ 0xb8,   0xA5 },
	{ 0xb7,   0x64 },
	{ 0xb9,   0x7C },
	{ 0xb3,   0xaf },
	{ 0xb4,   0x97 },
	{ 0xb5,   0xFF },
	{ 0xb0,   0xC5 },
	{ 0xb1,   0x94 },
	{ 0xb2,   0x0f },
	{ 0xc4,   0x5c },
	{ 0xa6,   0x00 },
	{ 0xa7,   0x20 },
	{ 0xa7,   0xd8 },
	{ 0xa7,   0x1b },
	{ 0xa7,   0x31 },
	{ 0xa7,   0x00 },
	{ 0xa7,   0x18 },
	{ 0xa7,   0x20 },
	{ 0xa7,   0xd8 },
	{ 0xa7,   0x19 },
	{ 0xa7,   0x31 },
	{ 0xa7,   0x00 },
	{ 0xa7,   0x18 },
	{ 0xa7,   0x20 },
	{ 0xa7,   0xd8 },
	{ 0xa7,   0x19 },
	{ 0xa7,   0x31 },
	{ 0xa7,   0x00 },
	{ 0xa7,   0x18 },
	{ 0x7f,   0x00 },
	{ 0xe5,   0x1f },
	{ 0xe1,   0x77 },
	{ 0xdd,   0x7f },
	{ CTRL0,  CTRL0_YUV422 | CTRL0_YUV_EN | CTRL0_RGB_EN },
  */
	{ BANK_SEL, BANK_SEL_SENS },
    { COM10, COM10_HREF_INV | COM10_VSINC_INV },
};

static struct regval_list sensor_uxga_regs[] = {   
  {0xff,0x00},
  {0xe0,0x04},
  {0xc0,0xc8},
  {0xc1,0x96},
  {0x86,0x3d},
  {0x50,0x00},
  {0x51,0x90},
  {0x52,0x2c},
  {0x53,0x00},
  {0x54,0x00},
  {0x55,0x88},
  {0x57,0x00},
  {0x5a,0x90},
  {0x5b,0x2c},
  {0x5c,0x05},
  {0xd3,0x82},
  {0xe0,0x00},
};


static struct regval_list sensor_sxga_regs[] = {
  
};

static struct regval_list sensor_xga_regs[] = {
  {0xff,0x00},
  {0xe0,0x04},
  {0xc0,0xc8},
  {0xc1,0x96},
  {0x86,0x3d},
  {0x50,0x00},
  {0x51,0x90},
  {0x52,0x2c},
  {0x53,0x00},
  {0x54,0x00},
  {0x55,0x88},
  {0x57,0x00},
  {0x5a,0x90},
  {0x5b,0x2c},
  {0x5c,0x05},
  {0xd3,0x82},
  {0xe0,0x00},
};


static struct regval_list sensor_720p_regs[] = {
  {0xff,0x00},
  {0xe0,0x04},
  {0xc0,0xc8},
  {0xc1,0x96},
  {0x86,0x3d},
  {0x50,0x00},
  {0x51,0x90},
  {0x52,0x2c},
  {0x53,0x00},
  {0x54,0x00},
  {0x55,0x88},
  {0x57,0x00},
  {0x5a,0x40},
  {0x5b,0xb4},
  {0x5c,0x01},
  {0xd3,0x82},
  {0xe0,0x00},
};

static struct regval_list sensor_svga_regs[] = {
  {0xff,0x00},
  {0xe0,0x04},
  {0xc0,0xc8},
  {0xc1,0x96},
  {0x86,0x35},
  {0x50,0x89},
  {0x51,0x90},
  {0x52,0x2c},
  {0x53,0x00},
  {0x54,0x00},
  {0x55,0x88},
  {0x57,0x00},
  {0x5a,0xc8},
  {0x5b,0x96},
  {0x5c,0x00},
  {0xd3,0x82},
  {0xe0,0x00},
};

static struct regval_list sensor_vga_regs[] = {
  {0xff, 0x00},
  {0xe0, 0x04},
  {0xc0, 0xc8},
  {0xc1, 0x96},
  {0x86, 0x3d},
  {0x50, 0x89},
  {0x51, 0x90},
  {0x52, 0x2c},
  {0x53, 0x00},
  {0x54, 0x00},
  {0x55, 0x88},
  {0x57, 0x00},
  {0x5a, 0xa0},
  {0x5b, 0x78},
  {0x5c, 0x00},
  {0xd3, 0x82},
  {0xe0, 0x00},
};

static struct regval_list sensor_oe_disable_regs[] = {
  {0xc3,0x00},
  {0xc4,0x00},
};

static struct regval_list sensor_oe_enable_regs[] = {
};

static struct regval_list sensor_sw_stby_on_regs[] = {
};

static struct regval_list sensor_sw_stby_off_regs[] = {
};


static struct regval_list sensor_wb_manual[] = {
};

static struct regval_list sensor_wb_auto_regs[] = {
  {0xff, 0x00}, 
  {0xc7, 0x00},
  
};

static struct regval_list sensor_wb_incandescence_regs[] = {
  {0xff, 0x00},
  {0xc7, 0x40},
  {0xcc, 0x42},
  {0xcd, 0x3f},
  {0xce, 0x71},
  
};

static struct regval_list sensor_wb_fluorescent_regs[] = {
  {0xff, 0x00},
  {0xc7, 0x40},
  {0xcc, 0x52},
  {0xcd, 0x41},
  {0xce, 0x66},
};

static struct regval_list sensor_wb_tungsten_regs[] = {
  {0xff, 0x00},
  {0xc7, 0x40},
  {0xcc, 0x52},
  {0xcd, 0x41},
  {0xce, 0x66},
  
};

static struct regval_list sensor_wb_horizon[] = {
};

static struct regval_list sensor_wb_daylight_regs[] = {
  {0xff, 0x00},
  {0xc7, 0x40},
  {0xcc, 0x5e},
  {0xcd, 0x41},
  {0xce, 0x54},
  
};

static struct regval_list sensor_wb_flash[] = {
};

static struct regval_list sensor_wb_cloud_regs[] = {
  {0xff, 0x00},
  {0xc7, 0x40},
  {0xcc, 0x65},
  {0xcd, 0x41},
  {0xce, 0x4f},  
};

static struct regval_list sensor_wb_shade[] = {
};

static struct cfg_array sensor_wb[] = {
  {
  	.regs = sensor_wb_manual,            
    .size = ARRAY_SIZE(sensor_wb_manual),
  },
  {
  	.regs = sensor_wb_auto_regs,         
    .size = ARRAY_SIZE(sensor_wb_auto_regs),
  },
  {
  	.regs = sensor_wb_incandescence_regs,
    .size = ARRAY_SIZE(sensor_wb_incandescence_regs),
  },
  {
  	.regs = sensor_wb_fluorescent_regs,  
    .size = ARRAY_SIZE(sensor_wb_fluorescent_regs),
  },
  {
  	.regs = sensor_wb_tungsten_regs,     
    .size = ARRAY_SIZE(sensor_wb_tungsten_regs),
  },
  {
  	.regs = sensor_wb_horizon,           
    .size = ARRAY_SIZE(sensor_wb_horizon),
  },
  {
  	.regs = sensor_wb_daylight_regs,     
    .size = ARRAY_SIZE(sensor_wb_daylight_regs),
  },
  {
  	.regs = sensor_wb_flash,             
    .size = ARRAY_SIZE(sensor_wb_flash),
  },
  {
  	.regs = sensor_wb_cloud_regs,        
    .size = ARRAY_SIZE(sensor_wb_cloud_regs),
  },
  {
  	.regs = sensor_wb_shade,             
    .size = ARRAY_SIZE(sensor_wb_shade),
  },
};

static struct regval_list sensor_brightness_neg4_regs[] = {
};

static struct regval_list sensor_brightness_neg3_regs[] = {
};

static struct regval_list sensor_brightness_neg2_regs[] = {
};

static struct regval_list sensor_brightness_neg1_regs[] = {
};

static struct regval_list sensor_brightness_zero_regs[] = {
};

static struct regval_list sensor_brightness_pos1_regs[] = {
};

static struct regval_list sensor_brightness_pos2_regs[] = {
};

static struct regval_list sensor_brightness_pos3_regs[] = {
};

static struct regval_list sensor_brightness_pos4_regs[] = {
};

static struct regval_list sensor_contrast_neg4_regs[] = {
};

static struct regval_list sensor_contrast_neg3_regs[] = {
};

static struct regval_list sensor_contrast_neg2_regs[] = {
};

static struct regval_list sensor_contrast_neg1_regs[] = {
};

static struct regval_list sensor_contrast_zero_regs[] = {
};

static struct regval_list sensor_contrast_pos1_regs[] = {
};

static struct regval_list sensor_contrast_pos2_regs[] = {
};

static struct regval_list sensor_contrast_pos3_regs[] = {
};

static struct regval_list sensor_contrast_pos4_regs[] = {
};

static struct regval_list sensor_saturation_neg4_regs[] = {
};

static struct regval_list sensor_saturation_neg3_regs[] = {
};

static struct regval_list sensor_saturation_neg2_regs[] = {
};

static struct regval_list sensor_saturation_neg1_regs[] = {
};

static struct regval_list sensor_saturation_zero_regs[] = {
};

static struct regval_list sensor_saturation_pos1_regs[] = {
};

static struct regval_list sensor_saturation_pos2_regs[] = {
};

static struct regval_list sensor_saturation_pos3_regs[] = {
};

static struct regval_list sensor_saturation_pos4_regs[] = {
};

static struct regval_list sensor_ev_neg4_regs[] = {
  {0xff,0x00},//register page
  {0x7c,0x00},
  {0x7d,0x04},
  {0x7c,0x09},
  {0x7d,0x28},//brightness value
  {0x7d,0x0e},//sign
};

static struct regval_list sensor_ev_neg3_regs[] = {
  {0xff,0x00},//register page
	{0x7c,0x00},
  {0x7d,0x04},
  {0x7c,0x09},
  {0x7d,0x20},//brightness value
  {0x7d,0x0e},//sign
};

static struct regval_list sensor_ev_neg2_regs[] = {
  {0xff,0x00},//register page
  {0x7c,0x00},
  {0x7d,0x04},
  {0x7c,0x09},
  {0x7d,0x18},//brightness value
  {0x7d,0x0e},//sign
};

static struct regval_list sensor_ev_neg1_regs[] = {
  {0xff,0x00},//register page
  {0x7c,0x00},
  {0x7d,0x04},
  {0x7c,0x09},
  {0x7d,0x10},//brightness value
  {0x7d,0x0e},//sign
};

static struct regval_list sensor_ev_zero_regs[] = {
  {0xff,0x00},//register page
  {0x7c,0x00},
  {0x7d,0x04},
  {0x7c,0x09},
  {0x7d,0x00},//brightness value
  {0x7d,0x00},//sign
};

static struct regval_list sensor_ev_pos1_regs[] = {
  {0xff,0x00},//register page
  {0x7c,0x00},
  {0x7d,0x04},
  {0x7c,0x09},
  {0x7d,0x00},//brightness value
  {0x7d,0x00},//sign
};

static struct regval_list sensor_ev_pos2_regs[] = {
  {0xff,0x00},//register page
  {0x7c,0x00},
  {0x7d,0x04},
  {0x7c,0x09},
  {0x7d,0x10},//brightness value
  {0x7d,0x00},//sign
};

static struct regval_list sensor_ev_pos3_regs[] = {
  {0xff,0x00},//register page
  {0x7c,0x00},
  {0x7d,0x04},
  {0x7c,0x09},
  {0x7d,0x20},//brightness value
  {0x7d,0x00},//sign
};

static struct regval_list sensor_ev_pos4_regs[] = {
  {0xff,0x00},//register page
  {0x7c,0x00},
  {0x7d,0x04},
  {0x7c,0x09},
  {0x7d,0x30},//brightness value
  {0x7d,0x00},//sign
};

static int sensor_read(struct v4l2_subdev *sd, unsigned short reg, unsigned char *value)
{
  int ret = 0; int cnt = 0;
  
  ret = cci_read_a8_d8_no_restart(sd,reg,value);
  while(ret != 0 && cnt < 2)
  {
  	ret = cci_read_a8_d8_no_restart(sd,reg,value);
  	cnt++;
  }
  if (cnt > 0) vfe_dev_dbg("sensor read retry=%d\n",cnt);
  
  return ret;
}

static int sensor_write(struct v4l2_subdev *sd, unsigned short reg, unsigned char value)
{
  int ret = 0; int cnt = 0;
  ret = cci_write_a8_d8_no_restart(sd,reg,value);
  while(ret != 0 && cnt < 2)
  {
  	ret = cci_write_a8_d8_no_restart(sd,reg,value);
  	cnt++;
  }
  if (cnt > 0)vfe_dev_dbg("sensor write retry=%d\n",cnt);
  
  return ret;
}

static int sensor_write_array(struct v4l2_subdev *sd, struct regval_list *regs, int array_size)
{
  int i = 0;
  
  if (!regs) return -EINVAL;

  while(i < array_size)
  {
    LOG_ERR_RET(sensor_write(sd, regs->addr, regs->data));
    ++i;  ++regs;
  }
  return 0;
}

static int sensor_g_hflip(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(REG04);
  
  val &= REG04_HFLIP_IMG;

  *value = val ? 1 : 0;
  info->hflip = *value;

  return 0;
}

static int sensor_s_hflip(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(REG04);
  
  switch (value)
  {
  case 0: val &= ~REG04_HFLIP_IMG; break;
  case 1: val |= REG04_HFLIP_IMG; break;
  default: return -EINVAL;
  }

  SENSOR_WRITE(REG04);

  msleep(50);
  info->hflip = value;
    
  return 0;
}

static int sensor_g_vflip(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(REG04);
  
  val &= REG04_VFLIP_IMG;

  *value = val ? 1 : 0;
  info->vflip = *value;

  return 0;
}

static int sensor_s_vflip(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(REG04);
  
  switch (value)
  {
  case 0: val &= ~REG04_VFLIP_IMG; break;
  case 1: val |= REG04_VFLIP_IMG; break;
  default: return -EINVAL;
  }

  SENSOR_WRITE(REG04);

  msleep(50);
  info->vflip = value;
  
  return 0;
}

static int sensor_g_autogain(struct v4l2_subdev *sd, __s32 *value)
{
  return -EINVAL;
}

static int sensor_s_autogain(struct v4l2_subdev *sd, int value)
{
  return -EINVAL;
}

static int sensor_g_autoexp(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;
  
  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(COM8);

  if (val & COM8_AEC_EN) *value = V4L2_EXPOSURE_AUTO; else *value = V4L2_EXPOSURE_MANUAL;
  
  info->autoexp = *value;

  return 0;
}

static int sensor_s_autoexp(struct v4l2_subdev *sd, enum v4l2_exposure_auto_type value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(COM8);
  
  switch (value)
  {
  case V4L2_EXPOSURE_AUTO: val |= COM8_AEC_EN; break;
  case V4L2_EXPOSURE_MANUAL: val &= ~COM8_AEC_EN; break;
  case V4L2_EXPOSURE_SHUTTER_PRIORITY: return -EINVAL;
  case V4L2_EXPOSURE_APERTURE_PRIORITY: return -EINVAL;
  default: return -EINVAL;
  }

  SENSOR_WRITE(COM8);
  msleep(50);
  info->autoexp = value;

  return 0;
}

static int sensor_g_autowb(struct v4l2_subdev *sd, int *value)
{
  int ret;
  struct sensor_info *info = to_state(sd);
  unsigned char val;
  
  ret = sensor_read(sd, 0x13, &val);
  if (ret < 0) {
    vfe_dev_err("sensor_read err at sensor_g_autowb!\n");
    return ret;
  }
  
  val &= (1<<2);
  val = val>>2;	
  
  *value = val;
  info->autowb = *value;
  
  return 0;
}

static int sensor_s_autowb(struct v4l2_subdev *sd, int value)
{
  int ret;
  struct sensor_info *info = to_state(sd);
	unsigned char val;
    
	ret = sensor_write_array(sd, sensor_wb_auto_regs, ARRAY_SIZE(sensor_wb_auto_regs));
	if (ret < 0) {
      vfe_dev_err("sensor_write_array err at sensor_s_autowb!\n");
      return ret;
	}
    
	ret = sensor_read(sd, 0xc7, &val);
	if (ret < 0) {
      vfe_dev_err("sensor_read err at sensor_s_autowb!\n");
      return ret;
	}
    
	switch(value) {
	case 0:
      val |= 0x40;
      break;
	case 1:
      val &=~0x40;
      break;
	default:
      break;
	}
	ret = sensor_write(sd, 0xc7, val);
	if (ret < 0) {
      vfe_dev_err("sensor_write err at sensor_s_autowb!\n");
      return ret;
	}
    
	msleep(50);
    
	info->autowb = value;
	return 0;
}

static int sensor_g_hue(struct v4l2_subdev *sd, __s32 *value)
{
  return -EINVAL;
}

static int sensor_s_hue(struct v4l2_subdev *sd, int value)
{
  return -EINVAL;
}

static int sensor_g_gain(struct v4l2_subdev *sd, __s32 *value)
{
  return -EINVAL;
}

static int sensor_s_gain(struct v4l2_subdev *sd, int value)
{
  return -EINVAL;
}

static int sensor_g_brightness(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);
  
  *value = info->brightness;
  return 0;
}

static int sensor_s_brightness(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd);
  
  switch (value)
  {
  case -4: ret = sensor_write_array(sd, sensor_brightness_neg4_regs, ARRAY_SIZE(sensor_brightness_neg4_regs)); break;
  case -3: ret = sensor_write_array(sd, sensor_brightness_neg3_regs, ARRAY_SIZE(sensor_brightness_neg3_regs)); break;
  case -2: ret = sensor_write_array(sd, sensor_brightness_neg2_regs, ARRAY_SIZE(sensor_brightness_neg2_regs)); break;
  case -1: ret = sensor_write_array(sd, sensor_brightness_neg1_regs, ARRAY_SIZE(sensor_brightness_neg1_regs)); break;
  case 0:  ret = sensor_write_array(sd, sensor_brightness_zero_regs, ARRAY_SIZE(sensor_brightness_zero_regs)); break;
  case 1:  ret = sensor_write_array(sd, sensor_brightness_pos1_regs, ARRAY_SIZE(sensor_brightness_pos1_regs)); break;
  case 2:  ret = sensor_write_array(sd, sensor_brightness_pos2_regs, ARRAY_SIZE(sensor_brightness_pos2_regs)); break;
  case 3:  ret = sensor_write_array(sd, sensor_brightness_pos3_regs, ARRAY_SIZE(sensor_brightness_pos3_regs)); break;
  case 4:  ret = sensor_write_array(sd, sensor_brightness_pos4_regs, ARRAY_SIZE(sensor_brightness_pos4_regs)); break;
  default: return -EINVAL;
  }
  
  if (ret < 0) { vfe_dev_err("sensor_write_array err at sensor_s_brightness!\n"); return ret; }
  msleep(50);
  info->brightness = value;

  return 0;
}

static int sensor_g_contrast(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);

  *value = info->contrast;
  return 0;
}

static int sensor_s_contrast(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd);
  
  switch (value)
  {
  case -4: ret = sensor_write_array(sd, sensor_contrast_neg4_regs, ARRAY_SIZE(sensor_contrast_neg4_regs)); break;
  case -3: ret = sensor_write_array(sd, sensor_contrast_neg3_regs, ARRAY_SIZE(sensor_contrast_neg3_regs)); break;
  case -2: ret = sensor_write_array(sd, sensor_contrast_neg2_regs, ARRAY_SIZE(sensor_contrast_neg2_regs)); break;
  case -1: ret = sensor_write_array(sd, sensor_contrast_neg1_regs, ARRAY_SIZE(sensor_contrast_neg1_regs)); break;
  case 0:  ret = sensor_write_array(sd, sensor_contrast_zero_regs, ARRAY_SIZE(sensor_contrast_zero_regs)); break;
  case 1:  ret = sensor_write_array(sd, sensor_contrast_pos1_regs, ARRAY_SIZE(sensor_contrast_pos1_regs)); break;
  case 2:  ret = sensor_write_array(sd, sensor_contrast_pos2_regs, ARRAY_SIZE(sensor_contrast_pos2_regs)); break;
  case 3:  ret = sensor_write_array(sd, sensor_contrast_pos3_regs, ARRAY_SIZE(sensor_contrast_pos3_regs)); break;
  case 4:  ret = sensor_write_array(sd, sensor_contrast_pos4_regs, ARRAY_SIZE(sensor_contrast_pos4_regs)); break;
  default: return -EINVAL;
  }
  
  if (ret < 0) { vfe_dev_err("sensor_write_array err at sensor_s_contrast!\n"); return ret; }
  msleep(50);
  info->contrast = value;

  return 0;
}

static int sensor_g_saturation(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);
  
  *value = info->saturation;
  return 0;
}

static int sensor_s_saturation(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd);
  
  switch (value)
  {
  case -4: ret = sensor_write_array(sd, sensor_saturation_neg4_regs, ARRAY_SIZE(sensor_saturation_neg4_regs)); break;
  case -3: ret = sensor_write_array(sd, sensor_saturation_neg3_regs, ARRAY_SIZE(sensor_saturation_neg3_regs)); break;
  case -2: ret = sensor_write_array(sd, sensor_saturation_neg2_regs, ARRAY_SIZE(sensor_saturation_neg2_regs)); break;
  case -1: ret = sensor_write_array(sd, sensor_saturation_neg1_regs, ARRAY_SIZE(sensor_saturation_neg1_regs)); break;
  case 0:  ret = sensor_write_array(sd, sensor_saturation_zero_regs, ARRAY_SIZE(sensor_saturation_zero_regs)); break;
  case 1:  ret = sensor_write_array(sd, sensor_saturation_pos1_regs, ARRAY_SIZE(sensor_saturation_pos1_regs)); break;
  case 2:  ret = sensor_write_array(sd, sensor_saturation_pos2_regs, ARRAY_SIZE(sensor_saturation_pos2_regs)); break;
  case 3:  ret = sensor_write_array(sd, sensor_saturation_pos3_regs, ARRAY_SIZE(sensor_saturation_pos3_regs)); break;
  case 4:  ret = sensor_write_array(sd, sensor_saturation_pos4_regs, ARRAY_SIZE(sensor_saturation_pos4_regs)); break;
  default: return -EINVAL;
  }
  
  if (ret < 0) { vfe_dev_err("sensor_write_array err at sensor_s_saturation!\n"); return ret; }
  msleep(50);
  info->saturation = value;

  return 0;
}

static int sensor_g_exp_bias(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);
  
  *value = info->exp_bias;
  return 0;
}

static int sensor_s_exp_bias(struct v4l2_subdev *sd, int value)
{
  int ret;
  struct sensor_info *info = to_state(sd);
  vfe_dev_dbg("%s,%d:value = %d\n",__func__,__LINE__,value);
  switch (value) {
  case -4:
    ret = sensor_write_array(sd, sensor_ev_neg4_regs, ARRAY_SIZE(sensor_ev_neg4_regs));
    break;
  case -3:
    ret = sensor_write_array(sd, sensor_ev_neg3_regs, ARRAY_SIZE(sensor_ev_neg3_regs));
    break;
  case -2:
    ret = sensor_write_array(sd, sensor_ev_neg2_regs, ARRAY_SIZE(sensor_ev_neg2_regs));
    break;
  case -1:
    ret = sensor_write_array(sd, sensor_ev_neg1_regs, ARRAY_SIZE(sensor_ev_neg1_regs));
    break;
  case 0:
    ret = sensor_write_array(sd, sensor_ev_zero_regs, ARRAY_SIZE(sensor_ev_zero_regs));
    break;
  case 1:
    ret = sensor_write_array(sd, sensor_ev_pos1_regs, ARRAY_SIZE(sensor_ev_pos1_regs));
    break;
  case 2:
    ret = sensor_write_array(sd, sensor_ev_pos2_regs, ARRAY_SIZE(sensor_ev_pos2_regs));
    break;
  case 3:
    ret = sensor_write_array(sd, sensor_ev_pos3_regs, ARRAY_SIZE(sensor_ev_pos3_regs));
    break;
  case 4:
    ret = sensor_write_array(sd, sensor_ev_pos4_regs, ARRAY_SIZE(sensor_ev_pos4_regs));
    break;
  default:
    return -EINVAL;
  }

  if (ret < 0) {
    vfe_dev_err("sensor_write_array err at sensor_s_exp!\n");
    return ret;
	}
  
  msleep(50);
  
  info->exp = value;
  return 0;
}

static int sensor_g_wb(struct v4l2_subdev *sd, int *value)
{
  struct sensor_info *info = to_state(sd);
  enum v4l2_auto_n_preset_white_balance *wb_type = (enum v4l2_auto_n_preset_white_balance*)value;

  *wb_type = info->wb;
  
  return 0;
}

static int sensor_s_wb(struct v4l2_subdev *sd, enum v4l2_auto_n_preset_white_balance value)
{
  int ret;
  struct sensor_info *info = to_state(sd);
  
  vfe_dev_dbg("func=%s,value=%d\n",__func__,value);
  if (value == V4L2_WHITE_BALANCE_AUTO) {
    
    ret = sensor_s_autowb(sd, 1);
    return ret;
  }
  else {
    
    ret = sensor_s_autowb(sd, 0);
    if(ret < 0) {
      vfe_dev_err("sensor_s_autowb error, return %x!\n",ret);
      return ret;
    }
    
    switch (value) {
    case V4L2_WHITE_BALANCE_CLOUDY:
      ret = sensor_write_array(sd, sensor_wb_cloud_regs, ARRAY_SIZE(sensor_wb_cloud_regs));
      break;
    case V4L2_WHITE_BALANCE_DAYLIGHT:
      ret = sensor_write_array(sd, sensor_wb_daylight_regs, ARRAY_SIZE(sensor_wb_daylight_regs));
      break;
    case V4L2_WHITE_BALANCE_INCANDESCENT:
      ret = sensor_write_array(sd, sensor_wb_incandescence_regs, ARRAY_SIZE(sensor_wb_incandescence_regs));
      break;
    case V4L2_WHITE_BALANCE_FLUORESCENT:
      ret = sensor_write_array(sd, sensor_wb_fluorescent_regs, ARRAY_SIZE(sensor_wb_fluorescent_regs));
      break;
    case V4L2_WHITE_BALANCE_FLUORESCENT_H:
      ret = sensor_write_array(sd, sensor_wb_tungsten_regs, ARRAY_SIZE(sensor_wb_tungsten_regs));
      break;
    default:
      return -EINVAL;
    }
  }
  
  if (ret < 0) {
    vfe_dev_err("sensor_s_wb error, return %x!\n",ret);
    return ret;
  }
  
  msleep(50);
  
  info->wb = value;
  return 0;
}

/*
 * Stuff that knows about the sensor.
 */

static int sensor_power(struct v4l2_subdev *sd, int on)
{
  int ret;
  
  ret = 0;
  switch(on)
  {
  case CSI_SUBDEV_STBY_ON:
      vfe_dev_dbg("CSI_SUBDEV_STBY_ON!\n");
      ret = sensor_write_array(sd, sensor_sw_stby_on_regs ,ARRAY_SIZE(sensor_sw_stby_on_regs));
      if(ret < 0)
        vfe_dev_err("soft stby falied!\n");
      msleep(10);
      vfe_dev_print("disalbe oe!\n");
      ret = sensor_write_array(sd, sensor_oe_disable_regs, ARRAY_SIZE(sensor_oe_disable_regs));
      if(ret < 0)
        vfe_dev_err("disalbe oe falied!\n");
      cci_lock(sd);
      vfe_gpio_write(sd,PWDN,CSI_STBY_ON);
      cci_unlock(sd);
      vfe_set_mclk(sd,OFF);
      break;
  case CSI_SUBDEV_STBY_OFF:
      vfe_dev_dbg("CSI_SUBDEV_STBY_OFF!\n");
      cci_lock(sd);
      vfe_set_mclk_freq(sd,MCLK);
      vfe_set_mclk(sd,ON);
      msleep(10);
      vfe_gpio_write(sd,PWDN,CSI_STBY_OFF);
      msleep(10);
      cci_unlock(sd);
      vfe_dev_print("enable oe!\n");
      ret = sensor_write_array(sd, sensor_oe_enable_regs,  ARRAY_SIZE(sensor_oe_enable_regs));
      if(ret < 0)
        vfe_dev_err("enable oe falied!\n");
      ret = sensor_write_array(sd, sensor_sw_stby_off_regs ,ARRAY_SIZE(sensor_sw_stby_off_regs));
      if(ret < 0)
        vfe_dev_err("soft stby off falied!\n");
      msleep(10);
      break;
    case CSI_SUBDEV_PWR_ON:
      vfe_dev_dbg("CSI_SUBDEV_PWR_ON!\n");
      cci_lock(sd);
      vfe_gpio_set_status(sd,PWDN,1);//set the gpio to output
      vfe_gpio_set_status(sd,RESET,1);//set the gpio to output
      vfe_gpio_write(sd,PWDN,CSI_STBY_ON);
      vfe_gpio_write(sd,RESET,CSI_RST_ON);
      msleep(1);
      vfe_set_mclk_freq(sd,MCLK);
      vfe_set_mclk(sd,ON);
      msleep(10);
      vfe_gpio_write(sd,POWER_EN,CSI_PWR_ON);
      vfe_set_pmu_channel(sd,IOVDD,ON);
      vfe_set_pmu_channel(sd,AVDD,ON);
      vfe_set_pmu_channel(sd,DVDD,ON);
      vfe_set_pmu_channel(sd,AFVDD,ON);
      vfe_gpio_write(sd,PWDN,CSI_STBY_OFF);
      msleep(10);
      vfe_gpio_write(sd,RESET,CSI_RST_OFF);
      msleep(30);
      cci_unlock(sd);
      break;
    case CSI_SUBDEV_PWR_OFF:
      vfe_dev_dbg("CSI_SUBDEV_PWR_OFF!\n");
      cci_lock(sd);
      vfe_set_mclk(sd,OFF);
      vfe_gpio_write(sd,POWER_EN,CSI_PWR_OFF);
      vfe_set_pmu_channel(sd,AFVDD,OFF);
      vfe_set_pmu_channel(sd,DVDD,OFF);
      vfe_set_pmu_channel(sd,AVDD,OFF);
      vfe_set_pmu_channel(sd,IOVDD,OFF);
      msleep(10);
      vfe_gpio_write(sd,POWER_EN,CSI_STBY_OFF);
      vfe_gpio_write(sd,RESET,CSI_RST_ON);
      vfe_gpio_set_status(sd,RESET,0);//set the gpio to input
      vfe_gpio_set_status(sd,PWDN,0);//set the gpio to input
      cci_unlock(sd);
      break;
    default:
      return -EINVAL;
  }

  return 0;
}

static int sensor_reset(struct v4l2_subdev *sd, u32 val)
{
  switch(val)
  {
  case 0: vfe_gpio_write(sd, RESET, CSI_RST_OFF); msleep(10); break;
  case 1: vfe_gpio_write(sd, RESET, CSI_RST_ON);  msleep(10); break;
  default: return -EINVAL;
  }
  
  return 0;
}

static int sensor_detect(struct v4l2_subdev *sd)
{
  int ret; unsigned char val;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(0x0a); if (val != 0x26) return -ENODEV;
  SENSOR_READ(0x0b); if (val != 0x42) return -ENODEV;
  
  return 0;
}

static int sensor_init(struct v4l2_subdev *sd, u32 val)
{
  int ret;
  struct sensor_info *info = to_state(sd);
  
  vfe_dev_dbg("sensor_init 0x%x\n",val);

  ret = sensor_detect(sd);
  if (ret) { vfe_dev_err("chip found is not an target chip.\n"); return ret; }

  vfe_get_standby_mode(sd, &info->stby_mode);

  if ((info->stby_mode == HW_STBY || info->stby_mode == SW_STBY) && info->init_first_flag == 0)
  {
    vfe_dev_print("stby_mode and init_first_flag = 0\n");
    return 0;
  }

  info->focus_status = 0;
  info->low_speed = 0;
  info->width = 0;
  info->height = 0;
  info->brightness = 0;
  info->contrast = 0;
  info->saturation = 0;
  info->hue = 0;
  info->hflip = 0;
  info->vflip = 0;
  info->gain = 0;
  info->autogain = 1;
  info->exp_bias = 0;
  info->autoexp = 1;
  info->autowb = 1;
  info->wb = V4L2_WHITE_BALANCE_AUTO;
  info->clrfx = V4L2_COLORFX_NONE;
  info->band_filter = V4L2_CID_POWER_LINE_FREQUENCY_50HZ;
  info->jevois = JEVOIS_SENSOR_COLOR;

  info->tpf.numerator = 1;
  info->tpf.denominator = 30;    /* default 30fps */

  ret = sensor_write_array(sd, sensor_default_regs, ARRAY_SIZE(sensor_default_regs));
  if(ret < 0) { vfe_dev_err("write sensor_default_regs error\n"); return ret; }
  
  
  if (info->stby_mode == 0) info->init_first_flag = 0;
  
  info->preview_first_flag = 1;
  
  return 0;
}

static long sensor_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
  int ret=0;
  return ret;
}

/*
 * Then there is the issue of window sizes.  Try to capture the info here.
 */


static struct sensor_win_size sensor_win_sizes[] = {
  /* UXGA */
  {
    .width      = UXGA_WIDTH,
    .height     = UXGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = sensor_uxga_regs,
    .regs_size  = ARRAY_SIZE(sensor_uxga_regs),
    .set_size   = NULL,
  },
  /* SXGA */
  {
    .width      = SXGA_WIDTH,
    .height     = SXGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = sensor_sxga_regs,
    .regs_size  = ARRAY_SIZE(sensor_sxga_regs),
    .set_size   = NULL,
  },
  /* 720p */
  {
    .width      = HD720_WIDTH,
    .height     = HD720_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = sensor_720p_regs,
    .regs_size  = ARRAY_SIZE(sensor_720p_regs),
    .set_size   = NULL,
  },
  /* XGA */
  {
    .width      = XGA_WIDTH,
    .height     = XGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = sensor_xga_regs,
    .regs_size  = ARRAY_SIZE(sensor_xga_regs),
    .set_size   = NULL,
  },
  /* SVGA */
  {
    .width      = SVGA_WIDTH,
    .height     = SVGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = sensor_svga_regs,
    .regs_size  = ARRAY_SIZE(sensor_svga_regs),
    .set_size   = NULL,
  },
  /* VGA */
  {
    .width      = VGA_WIDTH,
    .height     = VGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = sensor_vga_regs,
    .regs_size  = ARRAY_SIZE(sensor_vga_regs),
    .set_size   = NULL,
  },
};

#define N_WIN_SIZES (ARRAY_SIZE(sensor_win_sizes))

static int sensor_enum_fmt(struct v4l2_subdev *sd, unsigned index, enum v4l2_mbus_pixelcode *code)
{
  switch (index)
  {
  case 0: *code = V4L2_MBUS_FMT_YUYV8_2X8; return 0;
  case 1: *code = V4L2_MBUS_FMT_RGB565_2X8_LE; return 0;
  case 2: *code = V4L2_MBUS_FMT_SBGGR8_1X8; return 0;
    
  default: return -EINVAL;
  }
}

static int sensor_try_fmt_internal(struct v4l2_subdev *sd, struct v4l2_mbus_framefmt *fmt,
                                   struct sensor_win_size **ret_wsize)
{
  struct sensor_win_size *wsize;

  switch (fmt->code)
  {
  case V4L2_MBUS_FMT_YUYV8_2X8:
  case V4L2_MBUS_FMT_RGB565_2X8_LE:
  case V4L2_MBUS_FMT_SBGGR8_1X8:
    break;

  default:
    vfe_dev_dbg("sensor_try_fmt_internal: v4l2_mbus_framefmt code %x not found\n", fmt->code);
    return -1;
  }

  fmt->field = V4L2_FIELD_NONE;

  for (wsize = sensor_win_sizes; wsize < sensor_win_sizes + N_WIN_SIZES; wsize++)
    if (fmt->width >= wsize->width && fmt->height >= wsize->height) break;

  if (wsize >= sensor_win_sizes + N_WIN_SIZES) wsize--;   /* Take the smallest one */
  if (ret_wsize != NULL) *ret_wsize = wsize;

  vfe_dev_dbg("sensor_try_fmt_internal: requested %dx%d, using %dx%d\n", fmt->width, fmt->height,
              wsize->width, wsize->height);
 
  fmt->width = wsize->width;
  fmt->height = wsize->height;

  return 0;
}

static int sensor_try_fmt(struct v4l2_subdev *sd, struct v4l2_mbus_framefmt *fmt)
{
  return sensor_try_fmt_internal(sd, fmt, NULL);
}

static int sensor_g_mbus_config(struct v4l2_subdev *sd, struct v4l2_mbus_config *cfg)
{
  cfg->type = V4L2_MBUS_PARALLEL;
  cfg->flags = V4L2_MBUS_MASTER | VREF_POL | HREF_POL | CLK_POL ;
  
  return 0;
}

static int sensor_s_fmt(struct v4l2_subdev *sd, struct v4l2_mbus_framefmt *fmt)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val; struct sensor_win_size *wsize;

  ret = sensor_try_fmt_internal(sd, fmt, &wsize);
  if (ret) return ret;

  vfe_dev_dbg("sensor_s_fmt: using wsize->width=%d, wsize->height=%d\n", wsize->width, wsize->height);
  if (wsize->regs)
  {
    vfe_dev_dbg("sensor_s_fmt: writing window setup registers\n");
    ret = sensor_write_array(sd, wsize->regs , wsize->regs_size);
    if (ret < 0) return ret;
  }
  
  if (wsize->set_size) { ret = wsize->set_size(sd); if (ret < 0) return ret; }

  vfe_dev_dbg("sensor_s_fmt: using mbus_code = %x. Writing format setup registers.\n", fmt->code);

  SENSOR_BANK(BANK_SEL_DSP);
  val = RESET_DVP; SENSOR_WRITE(0xe0);
  switch(fmt->code)
  {
  case V4L2_MBUS_FMT_YUYV8_2X8: val = IMAGE_MODE_YUV422 | IMAGE_MODE_LBYTE_FIRST; break;
  case V4L2_MBUS_FMT_RGB565_2X8_LE: val = IMAGE_MODE_RGB565; break;
  case V4L2_MBUS_FMT_SBGGR8_1X8: val = IMAGE_MODE_RAW10; break;
  }
  SENSOR_WRITE(IMAGE_MODE);
  val = 0x01; SENSOR_WRITE(0xd7);
  val = 0x67; SENSOR_WRITE(0xe1);
  
  val = 0; SENSOR_WRITE(0xe0);

  info->width = wsize->width;
  info->height = wsize->height;
  info->exp_max_lines = wsize->intg_max;
  info->mbus_code = fmt->code;
  /*
  ret = sensor_s_brightness(sd, info->brightness);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset brightness -- IGNORED\n");

  ret = sensor_s_contrast(sd, info->contrast);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset contrast -- IGNORED\n");

  ret = sensor_s_saturation(sd, info->saturation);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset saturation -- IGNORED\n");

  ret = sensor_s_band_filter(sd, info->band_filter);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to banding filter -- IGNORED\n");
  */
  return 0;
}

static int sensor_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
  struct v4l2_captureparm *cp = &parms->parm.capture;
  struct sensor_info *info = to_state(sd);
  
  if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) return -EINVAL;
  
  memset(cp, 0, sizeof(struct v4l2_captureparm));
  cp->capability = V4L2_CAP_TIMEPERFRAME;
  cp->timeperframe.numerator = info->tpf.numerator;
  cp->timeperframe.denominator = info->tpf.denominator;
  
  return 0;
}

static int sensor_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
    #if 0
  struct v4l2_captureparm *cp = &parms->parm.capture;
  struct v4l2_fract *tpf = &cp->timeperframe;
  struct sensor_info *info = to_state(sd);
  unsigned char div;

  vfe_dev_dbg("sensor_s_parm\n");

  if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE){
  	vfe_dev_dbg("parms->type!=V4L2_BUF_TYPE_VIDEO_CAPTURE\n");
    return -EINVAL;
  }

  if (info->tpf.numerator == 0){
  	vfe_dev_dbg("info->tpf.numerator == 0\n");
    return -EINVAL;
  }

  info->capture_mode = cp->capturemode;

  if (info->capture_mode == V4L2_MODE_IMAGE) {
    vfe_dev_dbg("capture mode is not video mode,can not set frame rate!\n");
    return 0;
  }

  if (tpf->numerator == 0 || tpf->denominator == 0) {
    tpf->numerator = 1;
    tpf->denominator = SENSOR_FRAME_RATE;/* Reset to full rate */
    vfe_dev_err("sensor frame rate reset to full rate!\n");
  }

  div = SENSOR_FRAME_RATE/(tpf->denominator/tpf->numerator);
  if(div > 15 || div == 0)
  {
  	vfe_dev_print("SENSOR_FRAME_RATE=%d\n",SENSOR_FRAME_RATE);
  	vfe_dev_print("tpf->denominator=%d\n",tpf->denominator);
  	vfe_dev_print("tpf->numerator=%d\n",tpf->numerator);
    return -EINVAL;
  }

  vfe_dev_dbg("set frame rate %d\n",tpf->denominator/tpf->numerator);

  info->tpf.denominator = SENSOR_FRAME_RATE;
  info->tpf.numerator = div;

	if(info->tpf.denominator/info->tpf.numerator < 30)
		info->low_speed = 1;
  return 0;
    #endif
    return 0;
}


static int sensor_queryctrl(struct v4l2_subdev *sd,
    struct v4l2_queryctrl *qc)
{
	/* Fill in min, max, step and default value for these controls. */
	/* see include/linux/videodev2.h for details */
	/* see sensor_s_parm and sensor_g_parm for the meaning of value */
	switch (qc->id) {
	case V4L2_CID_VFLIP:
	case V4L2_CID_HFLIP:
		return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
	case V4L2_CID_EXPOSURE:
    case V4L2_CID_AUTO_EXPOSURE_BIAS:
		return v4l2_ctrl_query_fill(qc, -4, 4, 1, 0);
	case V4L2_CID_EXPOSURE_AUTO:
		return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
	case V4L2_CID_DO_WHITE_BALANCE:
    case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
		return v4l2_ctrl_query_fill(qc, 0, 9, 1, 0);
	case V4L2_CID_AUTO_WHITE_BALANCE:
		return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
	}
	return -EINVAL;
}

static int sensor_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
  switch (ctrl->id)
  {
  case V4L2_CID_BRIGHTNESS: return sensor_g_brightness(sd, &ctrl->value);
  case V4L2_CID_CONTRAST: return sensor_g_contrast(sd, &ctrl->value);
  case V4L2_CID_SATURATION: return sensor_g_saturation(sd, &ctrl->value);
  case V4L2_CID_HUE: return sensor_g_hue(sd, &ctrl->value);
  case V4L2_CID_VFLIP: return sensor_g_vflip(sd, &ctrl->value);
  case V4L2_CID_HFLIP: return sensor_g_hflip(sd, &ctrl->value);
  case V4L2_CID_GAIN: return sensor_g_gain(sd, &ctrl->value);
  case V4L2_CID_AUTOGAIN: return sensor_g_autogain(sd, &ctrl->value);
  case V4L2_CID_EXPOSURE:
  case V4L2_CID_AUTO_EXPOSURE_BIAS: return sensor_g_exp_bias(sd, &ctrl->value);
  case V4L2_CID_EXPOSURE_AUTO: return sensor_g_autoexp(sd, &ctrl->value);
  case V4L2_CID_DO_WHITE_BALANCE:
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE: return sensor_g_wb(sd, &ctrl->value);
  case V4L2_CID_AUTO_WHITE_BALANCE: return sensor_g_autowb(sd, &ctrl->value);
  }
  return -EINVAL;
}

static int sensor_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
  struct v4l2_queryctrl qc; int ret;
  
  qc.id = ctrl->id;
  ret = sensor_queryctrl(sd, &qc);
  if (ret < 0) return ret;

  if (qc.type == V4L2_CTRL_TYPE_MENU || qc.type == V4L2_CTRL_TYPE_INTEGER || qc.type == V4L2_CTRL_TYPE_BOOLEAN)
  {
    if (ctrl->value < qc.minimum || ctrl->value > qc.maximum) return -ERANGE;
  }

  switch (ctrl->id)
  {
  case V4L2_CID_BRIGHTNESS: return sensor_s_brightness(sd, ctrl->value);
  case V4L2_CID_CONTRAST: return sensor_s_contrast(sd, ctrl->value);
  case V4L2_CID_SATURATION: return sensor_s_saturation(sd, ctrl->value);
  case V4L2_CID_HUE: return sensor_s_hue(sd, ctrl->value);
  case V4L2_CID_VFLIP: return sensor_s_vflip(sd, ctrl->value);
  case V4L2_CID_HFLIP: return sensor_s_hflip(sd, ctrl->value);
  case V4L2_CID_GAIN: return sensor_s_gain(sd, ctrl->value);
  case V4L2_CID_AUTOGAIN: return sensor_s_autogain(sd, ctrl->value);
  case V4L2_CID_EXPOSURE:
  case V4L2_CID_AUTO_EXPOSURE_BIAS: return sensor_s_exp_bias(sd, ctrl->value);
  case V4L2_CID_EXPOSURE_AUTO: return sensor_s_autoexp(sd,	(enum v4l2_exposure_auto_type) ctrl->value);
  case V4L2_CID_DO_WHITE_BALANCE:
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
    return sensor_s_wb(sd, (enum v4l2_auto_n_preset_white_balance)ctrl->value);
  case V4L2_CID_AUTO_WHITE_BALANCE: return sensor_s_autowb(sd, ctrl->value);
  }
  return -EINVAL;
}


static int sensor_g_chip_ident(struct v4l2_subdev *sd, struct v4l2_dbg_chip_ident *chip)
{
  struct i2c_client *client = v4l2_get_subdevdata(sd);
  
  return v4l2_chip_ident_i2c_client(client, chip, V4L2_IDENT_SENSOR, 0);
}


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
  .enum_mbus_fmt = sensor_enum_fmt,
  .try_mbus_fmt = sensor_try_fmt,
  .s_mbus_fmt = sensor_s_fmt,
  .s_parm = sensor_s_parm,
  .g_parm = sensor_g_parm,
  .g_mbus_config = sensor_g_mbus_config,
};

static const struct v4l2_subdev_ops sensor_ops = {
  .core = &sensor_core_ops,
  .video = &sensor_video_ops,
};

/* ----------------------------------------------------------------------- */
static struct cci_driver cci_drv = {
  .name = SENSOR_NAME,
};

static int sensor_probe(struct i2c_client *client,
      const struct i2c_device_id *id)
{
  struct v4l2_subdev *sd;
  struct sensor_info *info;

  info = kzalloc(sizeof(struct sensor_info), GFP_KERNEL);
  if (info == NULL) return -ENOMEM;
  sd = &info->sd;
  glb_sd = sd;
  cci_dev_probe_helper(sd, client, &sensor_ops, &cci_drv);
  info->af_first_flag = 1;
  info->init_first_flag = 1;
  info->auto_focus = 0;

  return 0;
}


static int sensor_remove(struct i2c_client *client)
{
  struct v4l2_subdev *sd;

  sd = cci_dev_remove_helper(client, &cci_drv);
  kfree(to_state(sd));
  return 0;
}

static const struct i2c_device_id sensor_id[] = {
  { SENSOR_NAME, 0 },
  { }
};
MODULE_DEVICE_TABLE(i2c, sensor_id);


static struct i2c_driver sensor_driver = {
  .driver = {
    .owner = THIS_MODULE,
    .name = SENSOR_NAME,
  },
  .probe = sensor_probe,
  .remove = sensor_remove,
  .id_table = sensor_id,
};
static __init int init_sensor(void)
{
  return cci_dev_init_helper(&sensor_driver);
}

static __exit void exit_sensor(void)
{
  cci_dev_exit_helper(&sensor_driver);
}

module_init(init_sensor);
module_exit(exit_sensor);

