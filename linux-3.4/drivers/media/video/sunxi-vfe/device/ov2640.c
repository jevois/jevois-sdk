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
#define DEV_DBG_EN      0
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

#define SENSOR_WRITE2(reg, rval)                                         \
  ret = sensor_write(sd, reg, rval);                                     \
  if (ret < 0) { vfe_dev_err("Error writing %u to sensor register %u in %s\n", rval, reg, __func__); return ret; }

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
#define REG_DLY 0xfe

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
  { 0xff, 0x0 }, 
  { 0x2c, 0xff }, 
  { 0x2e, 0xdf }, 
  { 0xff, 0x1 }, 
  { 0x3c, 0x32 }, 
  { 0x11, 0x0 }, 
  { 0x9, 0x2 }, 
  { 0x4, 0xa8 }, 
  { 0x13, 0xe5 }, 
  { 0x14, 0xc0 }, 
  { 0x2c, 0xc }, 
  { 0x33, 0x78 }, 
  { 0x3a, 0x33 }, 
  { 0x3b, 0xfb }, 
  { 0x3e, 0x0 }, 
  { 0x43, 0x11 }, 
  { 0x16, 0x10 }, 
  { 0x39, 0x2 }, 
  { 0x35, 0x88 }, 
  { 0x22, 0xa }, 
  { 0x37, 0x40 }, 
  { 0x23, 0x0 }, 
  { 0x34, 0xa0 }, 
  { 0x6, 0x2 }, 
  { 0x6, 0x88 }, 
  { 0x7, 0xc0 }, 
  { 0xd, 0xb7 }, 
  { 0xe, 0x1 }, 
  { 0x4c, 0x0 }, 
  { 0x4a, 0x81 }, 
  { 0x21, 0x99 }, 
  { 0x24, 0x40 }, 
  { 0x25, 0x38 }, 
  { 0x26, 0x82 }, 
  { 0x5c, 0x0 }, 
  { 0x63, 0x0 }, 
  { 0x46, 0x22 }, 
  { 0xc, 0x3a }, 
  { 0x5d, 0x55 }, 
  { 0x5e, 0x7d }, 
  { 0x5f, 0x7d }, 
  { 0x60, 0x55 }, 
  { 0x61, 0x70 }, 
  { 0x62, 0x80 }, 
  { 0x7c, 0x5 }, 
  { 0x20, 0x80 }, 
  { 0x28, 0x30 }, 
  { 0x6c, 0x0 }, 
  { 0x6d, 0x80 }, 
  { 0x6e, 0x0 }, 
  { 0x70, 0x2 }, 
  { 0x71, 0x94 }, 
  { 0x73, 0xc1 }, 
  { 0x3d, 0x34 }, 
  { 0x12, 0x4 }, 
  { 0x5a, 0x57 }, 
  { 0x4f, 0xbb }, 
  { 0x50, 0x9c }, 
  { 0xff, 0x0 }, 
  { 0xe5, 0x7f }, 
  { 0xf9, 0xc0 }, 
  { 0x41, 0x24 }, 
  { 0xe0, 0x14 }, 
  { 0x76, 0xff }, 
  { 0x33, 0xa0 }, 
  { 0x42, 0x20 }, 
  { 0x43, 0x18 }, 
  { 0x4c, 0x0 }, 
  { 0x87, 0xd0 }, 
  { 0x88, 0x3f }, 
  { 0xd7, 0x3 }, 
  { 0xd9, 0x10 }, 
  { 0xd3, 0x82 }, 
  { 0xc8, 0x8 }, 
  { 0xc9, 0x80 }, 
  { 0x7c, 0x0 }, 
  { 0x7d, 0x0 }, 
  { 0x7c, 0x3 }, 
  { 0x7d, 0x48 }, 
  { 0x7d, 0x48 }, 
  { 0x7c, 0x8 }, 
  { 0x7d, 0x20 }, 
  { 0x7d, 0x10 }, 
  { 0x7d, 0xe }, 
  { 0x90, 0x0 }, 
  { 0x91, 0xe }, 
  { 0x91, 0x1a }, 
  { 0x91, 0x31 }, 
  { 0x91, 0x5a }, 
  { 0x91, 0x69 }, 
  { 0x91, 0x75 }, 
  { 0x91, 0x7e }, 
  { 0x91, 0x88 }, 
  { 0x91, 0x8f }, 
  { 0x91, 0x96 }, 
  { 0x91, 0xa3 }, 
  { 0x91, 0xaf }, 
  { 0x91, 0xc4 }, 
  { 0x91, 0xd7 }, 
  { 0x91, 0xe8 }, 
  { 0x91, 0x20 }, 
  { 0x92, 0x0 }, 
  { 0x93, 0x6 }, 
  { 0x93, 0xe3 }, 
  { 0x93, 0x3 }, 
  { 0x93, 0x3 }, 
  { 0x93, 0x0 }, 
  { 0x93, 0x2 }, 
  { 0x93, 0x0 }, 
  { 0x93, 0x0 }, 
  { 0x93, 0x0 }, 
  { 0x93, 0x0 }, 
  { 0x93, 0x0 }, 
  { 0x93, 0x0 }, 
  { 0x93, 0x0 }, 
  { 0x96, 0x0 }, 
  { 0x97, 0x8 }, 
  { 0x97, 0x19 }, 
  { 0x97, 0x2 }, 
  { 0x97, 0xc }, 
  { 0x97, 0x24 }, 
  { 0x97, 0x30 }, 
  { 0x97, 0x28 }, 
  { 0x97, 0x26 }, 
  { 0x97, 0x2 }, 
  { 0x97, 0x98 }, 
  { 0x97, 0x80 }, 
  { 0x97, 0x0 }, 
  { 0x97, 0x0 }, 
  { 0xa4, 0x0 }, 
  { 0xa8, 0x0 }, 
  { 0xc5, 0x11 }, 
  { 0xc6, 0x51 }, 
  { 0xbf, 0x80 }, 
  { 0xc7, 0x10 }, 
  { 0xb6, 0x66 }, 
  { 0xb8, 0xa5 }, 
  { 0xb7, 0x64 }, 
  { 0xb9, 0x7c }, 
  { 0xb3, 0xaf }, 
  { 0xb4, 0x97 }, 
  { 0xb5, 0xff }, 
  { 0xb0, 0xc5 }, 
  { 0xb1, 0x94 }, 
  { 0xb2, 0xf }, 
  { 0xc4, 0x5c }, 
  { 0xa6, 0x0 }, 
  { 0xa7, 0x20 }, 
  { 0xa7, 0xd8 }, 
  { 0xa7, 0x1b }, 
  { 0xa7, 0x31 }, 
  { 0xa7, 0x0 }, 
  { 0xa7, 0x18 }, 
  { 0xa7, 0x20 }, 
  { 0xa7, 0xd8 }, 
  { 0xa7, 0x19 }, 
  { 0xa7, 0x31 }, 
  { 0xa7, 0x0 }, 
  { 0xa7, 0x18 }, 
  { 0xa7, 0x20 }, 
  { 0xa7, 0xd8 }, 
  { 0xa7, 0x19 }, 
  { 0xa7, 0x31 }, 
  { 0xa7, 0x0 }, 
  { 0xa7, 0x18 }, 
  { 0x7f, 0x0 }, 
  { 0xe5, 0x1f }, 
  { 0xe1, 0x77 }, 
  { 0xdd, 0x7f }, 
  { 0xc2, CTRL0_YUV422 | CTRL0_YUV_EN },
  { 0xda, IMAGE_MODE_YUV422 },
  { 0xff, 0x0 }, 
  { 0xe0, 0x4 }, 
  { 0xc0, 0xc8 }, 
  { 0xc1, 0x96 }, 
  { 0x86, 0x3d }, 
  { 0x51, 0x90 }, 
  { 0x52, 0x2c }, 
  { 0x53, 0x0 }, 
  { 0x54, 0x0 }, 
  { 0x55, 0x88 }, 
  { 0x57, 0x0 }, 
  { 0xc2, CTRL0_YUV422 | CTRL0_YUV_EN },
  { 0xda, IMAGE_MODE_YUV422 },
  { 0x50, 0x92 }, 
  { 0x5a, 0x50 }, 
  { 0x5b, 0x3c }, 
  { 0x5c, 0x0 }, 
  { 0xd3, 0x4 }, 
  { 0xe0, 0x0 }, 
  { 0xff, 0x0 }, 
  { 0x5, 0x1 },
  { 0xda, IMAGE_MODE_YUV422 },
  { 0xd7, 0x3 }, 
  { 0xe0, 0x0 }, 
  { 0x5, 0x0 }, 
};
static struct regval_list sensor_uxga_regs[] = {
  { BANK_SEL, BANK_SEL_SENS },
  { CLKRC, 0x00 },
  { COM7, COM7_RES_UXGA },
  { COM1, COM1_VWIN_LSB_UXGA },
  { HSTART, 0x11 },
  { HEND, 0x75 },
  { VSTART, 0x01 },
  { VEND, 0x97 },
  { REG3D, REG3DA_UXGA },
  { REG32, 0x36 },
  { REG2A, 0x00 },
  { FRARL, 0x00 },
  { FLL, 0x00 },
  { FLH, 0x00 },
  { ADDVFL, 0x00 },
  { ADDVFH, 0x00 },

  { 0x35,    0x88 },
  { 0x22,    0x0a },
  { 0x37,    0x40 },
  { 0x34,    0xa0 },
  { 0x06,    0x02 },
  { 0x0d,    0xb7 },
  { 0x0e,    0x01 },
  { 0x42,    0x83 },
  
  { BANK_SEL, BANK_SEL_DSP },
  { SIZEL, SIZEL_HSIZE8_11_SET(UXGA_WIDTH) | SIZEL_HSIZE8_SET(UXGA_WIDTH) | SIZEL_VSIZE8_SET(UXGA_HEIGHT) },
  { HSIZE8, HSIZE8_SET(UXGA_WIDTH) },
  { VSIZE8, VSIZE8_SET(UXGA_HEIGHT) },
  { CTRL2, CTRL2_DCW_EN | CTRL2_SDE_EN | CTRL2_UV_AVG_EN | CTRL2_CMX_EN | CTRL2_UV_ADJ_EN },
  { HSIZE, HSIZE_SET(UXGA_WIDTH) },
  { VSIZE, VSIZE_SET(UXGA_HEIGHT) },
  { XOFFL, XOFFL_SET(0) },
  { YOFFL, YOFFL_SET(0) },
  { VHYX, VHYX_HSIZE_SET(UXGA_WIDTH) | VHYX_VSIZE_SET(UXGA_HEIGHT) | VHYX_XOFF_SET(0) | VHYX_YOFF_SET(0)},
  { TEST, TEST_HSIZE_SET(UXGA_WIDTH) },
  { CTRLI, CTRLI_LP_DP | CTRLI_V_DIV_SET(0) | CTRLI_H_DIV_SET(0) },
  { ZMOW, ZMOW_OUTW_SET(UXGA_WIDTH) },
  { ZMOH, ZMOH_OUTH_SET(UXGA_HEIGHT) },
  { ZMHH, ZMHH_OUTW_SET(UXGA_WIDTH) | ZMHH_OUTH_SET(UXGA_HEIGHT) },
  { R_DVP_SP, 0x00 },
};

static struct regval_list sensor_svga_regs[] = {
  { BANK_SEL, BANK_SEL_SENS },
  { CLKRC, 0x00 },
  { COM7, COM7_RES_SVGA },
  { COM1, COM1_VWIN_LSB_SVGA },
  { HSTART, 0x11 },
  { HEND, 0x43 },
  { VSTART, 0x00 },
  { VEND, 0x4B },
  { REG3D, REG3DA_SVGA },
  { REG32, 0x09 },   
  { REG2A, 0x00 },
  { FRARL, 0x00 },
  { FLL, 0x00 },
  { FLH, 0x00 },
  { ADDVFL, 0x00 },
  { ADDVFH, 0x00 },

  { 0x35,    0xda },
  { 0x22,    0x1a },
  { 0x37,    0xc3 },
  { 0x34,    0xc0 },
  { 0x06,    0x88 },
  { 0x0d,    0x87 },
  { 0x0e,    0x41 },
  { 0x42,    0x03 },

  { BANK_SEL, BANK_SEL_DSP },
  { SIZEL, SIZEL_HSIZE8_11_SET(SVGA_WIDTH) | SIZEL_HSIZE8_SET(SVGA_WIDTH) | SIZEL_VSIZE8_SET(SVGA_HEIGHT) },
  { HSIZE8, HSIZE8_SET(SVGA_WIDTH) },
  { VSIZE8, VSIZE8_SET(SVGA_HEIGHT) },
  { CTRL2, CTRL2_DCW_EN | CTRL2_SDE_EN | CTRL2_UV_AVG_EN | CTRL2_CMX_EN | CTRL2_UV_ADJ_EN },
  { HSIZE, HSIZE_SET(SVGA_WIDTH) },
  { VSIZE, VSIZE_SET(SVGA_HEIGHT) },
  { XOFFL, XOFFL_SET(0) },
  { YOFFL, YOFFL_SET(0) },
  { VHYX, VHYX_HSIZE_SET(SVGA_WIDTH) | VHYX_VSIZE_SET(SVGA_HEIGHT) | VHYX_XOFF_SET(0) | VHYX_YOFF_SET(0)},
  { TEST, TEST_HSIZE_SET(SVGA_WIDTH) },
  { CTRLI, CTRLI_LP_DP | CTRLI_V_DIV_SET(0) | CTRLI_H_DIV_SET(0) },
  { ZMOW, ZMOW_OUTW_SET(SVGA_WIDTH) },
  { ZMOH, ZMOH_OUTH_SET(SVGA_HEIGHT) },
  { ZMHH, ZMHH_OUTW_SET(SVGA_WIDTH) | ZMHH_OUTH_SET(SVGA_HEIGHT) },
  { R_DVP_SP, 0x00 },
};

static struct regval_list sensor_cif_regs[] = {
  { BANK_SEL, BANK_SEL_SENS },
  { CLKRC, 0x00 },
  { COM7, COM7_RES_CIF },
  { COM1, COM1_VWIN_LSB_CIF },
  { HSTART, 0x11 },
  { HEND, 0x43 },
  { VSTART, 0x00 },
  { VEND, 0x4B },
  { REG3D, REG3DA_CIF },
  { REG32, 0x09 },   
  { REG2A, 0x00 },
  { FRARL, 0x00 },
  { FLL, 0x00 },
  { FLH, 0x00 },
  { ADDVFL, 0x00 },
  { ADDVFH, 0x00 },
  /*
  { 0x35,    0xda },
  { 0x22,    0x1a },
  { 0x37,    0xc3 },
  { 0x34,    0xc0 },
  { 0x06,    0x88 },
  { 0x0d,    0x87 },
  { 0x0e,    0x41 },
  { 0x42,    0x03 },
  */
  { BANK_SEL, BANK_SEL_DSP },
  { SIZEL, SIZEL_HSIZE8_11_SET(CIF_WIDTH) | SIZEL_HSIZE8_SET(CIF_WIDTH) | SIZEL_VSIZE8_SET(CIF_HEIGHT) },
  { HSIZE8, HSIZE8_SET(CIF_WIDTH) },
  { VSIZE8, VSIZE8_SET(CIF_HEIGHT) },
  { CTRL2, CTRL2_DCW_EN | CTRL2_SDE_EN | CTRL2_UV_AVG_EN | CTRL2_CMX_EN | CTRL2_UV_ADJ_EN },
  { HSIZE, HSIZE_SET(CIF_WIDTH) },
  { VSIZE, VSIZE_SET(CIF_HEIGHT) },
  { XOFFL, XOFFL_SET(0) },
  { YOFFL, YOFFL_SET(0) },
  { VHYX, VHYX_HSIZE_SET(CIF_WIDTH) | VHYX_VSIZE_SET(CIF_HEIGHT) | VHYX_XOFF_SET(0) | VHYX_YOFF_SET(0)},
  { TEST, TEST_HSIZE_SET(CIF_WIDTH) },
  { CTRLI, CTRLI_LP_DP | CTRLI_V_DIV_SET(0) | CTRLI_H_DIV_SET(0) },
  { ZMOW, ZMOW_OUTW_SET(CIF_WIDTH) },
  { ZMOH, ZMOH_OUTH_SET(CIF_HEIGHT) },
  { ZMHH, ZMHH_OUTW_SET(CIF_WIDTH) | ZMHH_OUTH_SET(CIF_HEIGHT) },
  { R_DVP_SP, 0x00 },


  { BANK_SEL, BANK_SEL_SENS },
  { COM7, COM7_RES_CIF },
  { COM1, 0x0A },
  { REG32, REG32_CIF },
  { HSTART, 0x11 },
  { HEND, 0x43 },
  { VSTART, 0x00 },
  { VEND, 0x25 },
  { CLKRC, 0x01 },
  { BD50, 0xca },
  { BD60, 0xa8 },
  { 0x5a, 0x23 },
  { 0x6d, 0x00 },
  { 0x3d, 0x38 },
  { 0x39, 0x92 },
  { 0x35, 0xda },
  { 0x22, 0x1a },
  { 0x37, 0xc3 },
  { 0x23, 0x00 },
  { ARCOM2, 0xc0 },
  { 0x06, 0x88 },
  { 0x07, 0xc0 },
  { COM4, 0x87 },
  { 0x0e, 0x41 },
  { 0x4c, 0x00 },
  { BANK_SEL, BANK_SEL_DSP },
  { REG_RESET, RESET_DVP },
  { HSIZE8, 0x32 },
  { VSIZE8, 0x25 },
  { SIZEL, 0x00 },
  { HSIZE, 0x64 },
  { VSIZE, 0x4a },
  { XOFFL, 0x00 },
  { YOFFL, 0x00 },
  { VHYX, 0x00 },
  { TEST, 0x00 },
  { CTRL2, CTRL2_DCW_EN | 0x1D },
  { CTRLI, CTRLI_LP_DP | 0x00 },
  { R_DVP_SP, 0x82 },
};

static struct regval_list sensor_oe_disable_regs[] = {
};

static struct regval_list sensor_oe_enable_regs[] = {
};

static struct regval_list sensor_sw_stby_on_regs[] = {
};

static struct regval_list sensor_sw_stby_off_regs[] = {
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

  if (reg == REG_DLY) { msleep(value); return 0; }

  ret = cci_write_a8_d8(sd, reg, value);
  while (ret != 0 && cnt < 2)
  {
  	ret = cci_write_a8_d8(sd, reg, value);
  	++ cnt;
  }
  if (cnt > 0) vfe_dev_dbg("sensor write retry=%d\n",cnt);
  
  return ret;
}

static int sensor_write_array(struct v4l2_subdev *sd, struct regval_list *regs, int array_size)
{
  int i = 0;
  
  if (!regs) return -EINVAL;

  while(i < array_size)
  {
    if (regs->addr == REG_DLY) msleep(regs->data);
    else { LOG_ERR_RET(sensor_write(sd, regs->addr, regs->data)); }
    ++i;  ++regs;
  }
  return 0;
}

static int table_mask_write(struct v4l2_subdev *sd, const unsigned char * ptab)   
{
  unsigned char  address, value, val, mask; int ret;
  const unsigned char  *pdata = ptab;   
  
  if (NULL == pdata) return -1;   
  
  while(1)   
  {   
    address = *pdata++;   
    value = *pdata++;   
    mask = *pdata++;           
    if (0 == address && 0 == value && 0 == mask) break;   
    
    if (mask != 0xFF)   
    {
      SENSOR_READ(address);
      val &= (~mask);             
      value &= mask;   
      value |= val;   
    }
    SENSOR_WRITE2(address, value);
  }   
  
  return 0;   
}   

static int sensor_g_hmirror(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(REG04);
  
  val &= REG04_HFLIP_IMG;

  *value = val ? 1 : 0;
  info->hflip = *value;

  return 0;
}

static int sensor_s_hmirror(struct v4l2_subdev *sd, int value)
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
  case 0: val &= ~(REG04_VFLIP_IMG | REG04_VREF_EN); break;
  case 1: val |= REG04_VFLIP_IMG | REG04_VREF_EN; break;
  default: return -EINVAL;
  }

  SENSOR_WRITE(REG04);

  msleep(50);
  info->vflip = value;
  
  return 0;
}

static int sensor_g_autogain(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(COM8);
  if (val & COM8_AGC_EN) *value = 1; else *value = 0;
  info->autogain = *value;
  return 0;
}

static int sensor_s_autogain(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(COM8);
  if (value) val |= COM8_AGC_EN; else val &= ~COM8_AGC_EN;
  SENSOR_WRITE(COM8);
  info->autogain = value;
  return 0;
}

static int sensor_g_gain(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; unsigned char val; unsigned int tmp;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(GAIN); tmp = val;
  SENSOR_READ(REG45); tmp |= ((unsigned int)(val & 0xC0)) << 2;

  *value = (16 + (tmp & 0x000f));
  if (tmp & 0x0010) *value <<= 1;
  if (tmp & 0x0020) *value <<= 1;
  if (tmp & 0x0040) *value <<= 1;
  if (tmp & 0x0080) *value <<= 1;
  if (tmp & 0x0100) *value <<= 1;
  if (tmp & 0x0200) *value <<= 1;

  return 0;
}

static int sensor_s_gain(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val; unsigned int rgain;

  rgain = 0;
  if (value > 31) { rgain |= 0x10; value = value >> 1; }
  if (value > 31) { rgain |= 0x20; value = value >> 1; }
  if (value > 31) { rgain |= 0x40; value = value >> 1; }
  if (value > 31) { rgain |= 0x80; value = value >> 1; }
  if (value > 31) { rgain |= 0x100; value = value >> 1; }
  if (value > 31) { rgain |= 0x200; value = value >> 1; }
  if (value > 16) { rgain |= ((value - 16) & 0x0f); }

  SENSOR_BANK(BANK_SEL_SENS);
  val = rgain & 0xff; SENSOR_WRITE(GAIN);
  SENSOR_READ(REG45); val &= ~0xc0; val |= ((rgain >> 2) & 0xc0); SENSOR_WRITE(REG45);

  return 0;
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
  case V4L2_EXPOSURE_AUTO: val |= (COM8_AEC_EN | COM8_AGC_EN); break;
  case V4L2_EXPOSURE_MANUAL: val &= ~(COM8_AEC_EN | COM8_AGC_EN); break;
  case V4L2_EXPOSURE_SHUTTER_PRIORITY: return -EINVAL;
  case V4L2_EXPOSURE_APERTURE_PRIORITY: return -EINVAL;
  default: return -EINVAL;
  }

  SENSOR_WRITE(COM8);
  msleep(50);
  info->autoexp = value;

  return 0;
}

static int sensor_g_exposure_absolute(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val; unsigned int exposure;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(REG04);
  exposure = val & 0x03;
  SENSOR_READ(AEC);
  exposure |= (val << 2);
  SENSOR_READ(REG45);
  exposure |= ((val & 0x3f) << 10);
  info->exp = (exposure * info->trow + 50) / 100;
  *value = info->exp;

  return 0;
}

static int sensor_s_exposure_absolute(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val; unsigned int exposure;

  exposure = (value * 100 + 50) / info->trow;
  if (exposure < 1) exposure = 1; else if (exposure > info->exp_max_lines) exposure = info->exp_max_lines;
  
  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(REG04); val &= ~(0x03); val |= (exposure & 0x3); SENSOR_WRITE(REG04);
  val = (exposure >> 2) & 0xff; SENSOR_WRITE(AEC);
  val = (exposure >> 10) & 0x3f; SENSOR_WRITE(REG45);

  info->exp = (exposure * info->trow + 50) / 100;

  return 0;
}

static int sensor_g_autowb(struct v4l2_subdev *sd, int *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_BANK(BANK_SEL_DSP);
  SENSOR_READ(CTRL1);
  if (val & CTRL1_AWB) *value = 1; else *value = 0;
  info->autowb = *value;
  return 0;
}

static int sensor_s_autowb(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_BANK(BANK_SEL_DSP);

  switch(value)
  {
  case 0:
    SENSOR_READ(REG_C7); val |= AWB_OFF; val &= ~AWB_SIMPLE; SENSOR_WRITE(REG_C7);
    SENSOR_READ(CTRL1); val &= ~CTRL1_AWB; SENSOR_WRITE(CTRL1);
    break;
  case 1:
    SENSOR_READ(REG_C7); val |= AWB_SIMPLE; val &= ~AWB_OFF; SENSOR_WRITE(REG_C7);
    SENSOR_READ(CTRL1); val |= CTRL1_AWB; SENSOR_WRITE(CTRL1);
    break;
  default: break;
  }

  msleep(50);
  info->autowb = value;
  return 0;
}

static int sensor_g_wb_red(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned char val;
  SENSOR_BANK(BANK_SEL_DSP);
  SENSOR_READ(REG_RED); *value = (unsigned int)(val);
  return 0;
}

static int sensor_g_wb_blue(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned char val;
  SENSOR_BANK(BANK_SEL_DSP);
  SENSOR_READ(REG_BLUE); *value = (unsigned int)(val);
  return 0;
}

static int sensor_s_wb_red(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val = value & 0xff;
  SENSOR_BANK(BANK_SEL_DSP);
  SENSOR_WRITE(REG_RED);
  return 0;
}

static int sensor_s_wb_blue(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val = value & 0xff;
  SENSOR_BANK(BANK_SEL_DSP);
  SENSOR_WRITE(REG_BLUE);
  return 0;
}

const static unsigned char OV2640_BRIGHTNESS_DISABLE[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x00,   0xff,   
  0x7D,   0x00,   0x04,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_BRIGHTNESS_ENABLE[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x00,   0xff,   
  0x7D,   0x04,   0x04,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_BRIGHTNESS_NEG4[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x09,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x08,   0x08,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_BRIGHTNESS_NEG3[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x09,   0xff,   
  0x7D,   0x10,   0xff,   
  0x7D,   0x08,   0x08,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_BRIGHTNESS_NEG2[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x09,   0xff,   
  0x7D,   0x00,   0xff,   
  0x7D,   0x00,   0x08,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_BRIGHTNESS_NEG1[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x09,   0xff,   
  0x7D,   0x10,   0xff,   
  0x7D,   0x00,   0x08,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_BRIGHTNESS_ZERO[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x09,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x00,   0x08,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_BRIGHTNESS_POS1[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x09,   0xff,   
  0x7D,   0x30,   0xff,   
  0x7D,   0x00,   0x08,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_BRIGHTNESS_POS2[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x09,   0xff,   
  0x7D,   0x40,   0xff,   
  0x7D,   0x00,   0x08,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_BRIGHTNESS_POS3[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x09,   0xff,   
  0x7D,   0x50,   0xff,   
  0x7D,   0x00,   0x08,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_BRIGHTNESS_POS4[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x09,   0xff,   
  0x7D,   0x60,   0xff,   
  0x7D,   0x00,   0x08,   
  0x00,   0x00,   0x00   
};   

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
  case -4: ret = table_mask_write(sd, OV2640_BRIGHTNESS_NEG4); break;
  case -3: ret = table_mask_write(sd, OV2640_BRIGHTNESS_NEG3); break;
  case -2: ret = table_mask_write(sd, OV2640_BRIGHTNESS_NEG2); break;
  case -1: ret = table_mask_write(sd, OV2640_BRIGHTNESS_NEG1); break;
  case  0: ret = table_mask_write(sd, OV2640_BRIGHTNESS_ZERO); break;
  case  1: ret = table_mask_write(sd, OV2640_BRIGHTNESS_POS1); break;
  case  2: ret = table_mask_write(sd, OV2640_BRIGHTNESS_POS2); break;
  case  3: ret = table_mask_write(sd, OV2640_BRIGHTNESS_POS3); break;
  case  4: ret = table_mask_write(sd, OV2640_BRIGHTNESS_POS4); break;
  default: return -EINVAL;
  }
  
  if (ret < 0) { vfe_dev_err("table_mask_write err at sensor_s_brightness!\n"); return ret; }

  ret = table_mask_write(sd, OV2640_BRIGHTNESS_ENABLE);
  if (ret < 0) { vfe_dev_err("table_mask_write err at sensor_s_brightness!\n"); return ret; }
  
  msleep(50);
  info->brightness = value;

  return 0;
}

const static unsigned char OV2640_CONTRAST_DISABLE[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x00,   0xff,   
  0x7D,   0x00,   0x04,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_CONTRAST_ENABLE[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x00,   0xff,   
  0x7D,   0x04,   0x04,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_CONTRAST_NEG4[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x07,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x10,   0xff,   
  0x7D,   0x4a,   0xff,   
  0x7D,   0x06,   0x0c,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_CONTRAST_NEG3[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x07,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x14,   0xff,   
  0x7D,   0x40,   0xff,   
  0x7D,   0x06,   0x0c,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_CONTRAST_NEG2[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x07,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x18,   0xff,   
  0x7D,   0x34,   0xff,   
  0x7D,   0x06,   0x0c,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_CONTRAST_NEG1[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x07,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x1c,   0xff,   
  0x7D,   0x2a,   0xff,   
  0x7D,   0x06,   0x0c,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_CONTRAST_ZERO[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x07,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x06,   0x0c,   
  0x00,   0x00,   0x00   
};
const static unsigned char OV2640_CONTRAST_POS1[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x07,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x24,   0xff,   
  0x7D,   0x16,   0xff,   
  0x7D,   0x06,   0x0c,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_CONTRAST_POS2[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x07,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x28,   0xff,   
  0x7D,   0x0c,   0xff,   
  0x7D,   0x06,   0x0c,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_CONTRAST_POS3[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x07,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x2c,   0xff,   
  0x7D,   0x02,   0xff,   
  0x7D,   0x06,   0x0c,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_CONTRAST_POS4[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x07,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x30,   0xff,   
  0x7D,   0x08,   0xff,   
  0x7D,   0x0e,   0x0c,   
  0x00,   0x00,   0x00   
};   

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
  case 0: ret = table_mask_write(sd, OV2640_CONTRAST_NEG4); break;
  case 1: ret = table_mask_write(sd, OV2640_CONTRAST_NEG3); break;
  case 2: ret = table_mask_write(sd, OV2640_CONTRAST_NEG2); break;
  case 3: ret = table_mask_write(sd, OV2640_CONTRAST_NEG1); break;
  case 4: ret = table_mask_write(sd, OV2640_CONTRAST_ZERO); break;
  case 5: ret = table_mask_write(sd, OV2640_CONTRAST_POS1); break;
  case 6: ret = table_mask_write(sd, OV2640_CONTRAST_POS2); break;
  case 7: ret = table_mask_write(sd, OV2640_CONTRAST_POS3); break;
  case 8: ret = table_mask_write(sd, OV2640_CONTRAST_POS4); break;
  default: return -EINVAL;
  }
  
  if (ret < 0) { vfe_dev_err("table_mask_write err at sensor_s_brightness!\n"); return ret; }

  ret = table_mask_write(sd, OV2640_CONTRAST_ENABLE);
  if (ret < 0) { vfe_dev_err("table_mask_write err at sensor_s_brightness!\n"); return ret; }
  
  msleep(50);
  info->contrast = value;

  return 0;
}

static int sensor_g_sharpness(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);

  *value = info->sharpness;
  return 0;
}

static unsigned char const sharpness_values[] = { 0x20 /* auto */, 0xc0, 0xc1, 0xc2, 0xc4, 0xc8, 0xd0, 0xdf };

static int sensor_s_sharpness(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd);
  if (value < 0 || value >= ARRAY_SIZE(sharpness_values)) return -EINVAL;
  
  SENSOR_BANK(BANK_SEL_DSP);
  SENSOR_WRITE2(0x92, 0x01);
  SENSOR_WRITE2(0x93, sharpness_values[value]);
  
  info->sharpness = value;
  return 0;
}

const static unsigned char OV2640_SATURATION_DISABLE[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x00,   0xff,   
  0x7D,   0x00,   0x02,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_SATURATION_ENABLE[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x00,   0xff,   
  0x7D,   0x02,   0x02,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_SATURATION_NEG4[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x03,   0xff,   
  0x7D,   0x00,   0xff,   
  0x7D,   0x00,   0xff,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_SATURATION_NEG3[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x03,   0xff,   
  0x7D,   0x10,   0xff,   
  0x7D,   0x10,   0xff,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_SATURATION_NEG2[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x03,   0xff,   
  0x7D,   0x20,   0xff,   
  0x7D,   0x20,   0xff,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_SATURATION_NEG1[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x03,   0xff,   
  0x7D,   0x30,   0xff,   
  0x7D,   0x30,   0xff,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_SATURATION_ZERO[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x03,   0xff,   
  0x7D,   0x40,   0xff,   
  0x7D,   0x40,   0xff,   
  0x00,   0x00,   0x00   
};   
const static unsigned char OV2640_SATURATION_POS1[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x03,   0xff,   
  0x7D,   0x50,   0xff,   
  0x7D,   0x50,   0xff,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_SATURATION_POS2[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x03,   0xff,   
  0x7D,   0x60,   0xff,   
  0x7D,   0x60,   0xff,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_SATURATION_POS3[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x03,   0xff,   
  0x7D,   0x70,   0xff,   
  0x7D,   0x70,   0xff,   
  0x00,   0x00,   0x00   
};   

const static unsigned char OV2640_SATURATION_POS4[]=   
{   
  0xFF,   0x00,   0xff,   
  0x7C,   0x03,   0xff,   
  0x7D,   0x80,   0xff,   
  0x7D,   0x80,   0xff,   
  0x00,   0x00,   0x00   
};

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
  case 0: ret = table_mask_write(sd, OV2640_SATURATION_NEG4); break;
  case 1: ret = table_mask_write(sd, OV2640_SATURATION_NEG3); break;
  case 2: ret = table_mask_write(sd, OV2640_SATURATION_NEG2); break;
  case 3: ret = table_mask_write(sd, OV2640_SATURATION_NEG1); break;
  case 4: ret = table_mask_write(sd, OV2640_SATURATION_ZERO); break;
  case 5: ret = table_mask_write(sd, OV2640_SATURATION_POS1); break;
  case 6: ret = table_mask_write(sd, OV2640_SATURATION_POS2); break;
  case 7: ret = table_mask_write(sd, OV2640_SATURATION_POS3); break;
  case 8: ret = table_mask_write(sd, OV2640_SATURATION_POS4); break;
  default: return -EINVAL;
  }
  
  if (ret < 0) { vfe_dev_err("table_mask_write err at sensor_s_brightness!\n"); return ret; }

  ret = table_mask_write(sd, OV2640_SATURATION_ENABLE);
  if (ret < 0) { vfe_dev_err("table_mask_write err at sensor_s_brightness!\n"); return ret; }

  msleep(50);
  info->saturation = value;

  return 0;
}

static int sensor_g_band_filter(struct v4l2_subdev *sd, int *value)
{
  *value = to_state(sd)->band_filter;
  return 0;
}

static int sensor_s_band_filter(struct v4l2_subdev *sd, int value)
{
 struct sensor_info *info = to_state(sd);
 /*
   int ret; struct sensor_info *info = to_state(sd); unsigned char val;
   int ratex100 = 100 * info->tpf.denominator / info->tpf.numerator;
   
   switch (value)
   {
   case V4L2_CID_POWER_LINE_FREQUENCY_DISABLED:
   SENSOR_READ(REG_COM8); val &= ~COM8_BFILT; SENSOR_WRITE(REG_COM8);
   break;
   
   case V4L2_CID_POWER_LINE_FREQUENCY_50HZ:
   SENSOR_READ(REG_COM8); val |= COM8_BFILT; SENSOR_WRITE(REG_COM8);
   SENSOR_READ(REG_COM11); val &= ~COM11_BANDING; SENSOR_WRITE(REG_COM11);
   SENSOR_READ(REG_COM17); val |= 1; SENSOR_WRITE(REG_COM17);
   
   val = ratex100 * info->exp_max_lines / 10000;
   SENSOR_WRITE(REG_BD50ST);
    break;
    
    case V4L2_CID_POWER_LINE_FREQUENCY_60HZ:
    SENSOR_READ(REG_COM8); val |= COM8_BFILT; SENSOR_WRITE(REG_COM8);
    SENSOR_READ(REG_COM11); val &= ~COM11_BANDING; SENSOR_WRITE(REG_COM11);
    SENSOR_READ(REG_COM17); val &= ~1; SENSOR_WRITE(REG_COM17);
    
    val = ratex100 * info->exp_max_lines / 12000;
    SENSOR_WRITE(REG_BD60ST);
    break;
    
    default: return -EINVAL;
    }
 */
  info->band_filter = value;

  return 0;
}


static struct regval_list sensor_wbp_auto_regs[] = {
  { BANK_SEL, BANK_SEL_SENS },
  { COM9, COM9_AGC_GAIN_128x },
  { BANK_SEL, BANK_SEL_DSP },
  { 0xb2,   0x1f },
  { 0xb4,   0x1f },
  { 0xb8,   0x1f },
  { 0xb5,   0x9b },
  { 0xa1,   0x91 },
  { 0xa4,   0x62 },
  { 0xb9,   0x6c },
};   

static struct regval_list sensor_wbp_sunny_regs[] = {
  { BANK_SEL, BANK_SEL_SENS },
  { COM9, COM9_AGC_GAIN_32x },
  { BANK_SEL, BANK_SEL_DSP },
  { REG_RED, 0x5e },
  { REG_GREEN, 0x41 },
  { REG_BLUE, 0x54 },
  { 0xb2,   0x00 },
  { 0xb4,   0x1f },
  { 0xb8,   0x00 },
  { 0xb5,   0x9b },
  { 0xa1,   0xc2 },
  { 0xa4,   0x86 },
  { 0xb9,   0x6c },
};   
   
static struct regval_list sensor_wbp_cloudy_regs[] = {
  { BANK_SEL, BANK_SEL_SENS },
  { COM9, COM9_AGC_GAIN_64x },
  { BANK_SEL, BANK_SEL_DSP },
  { REG_RED, 0x65 },
  { REG_GREEN, 0x41 },
  { REG_BLUE, 0x4f },
  { 0xb2,   0x00 }, 
  { 0xb4,   0x1f },
  { 0xb8,   0x00 },
  { 0xb5,   0xb2 },
  { 0xa1,   0xc2 },
  { 0xa4,   0x86 },
  { 0xb9,   0x6c },
};   
   
static struct regval_list sensor_wbp_office_regs[] = {
  { BANK_SEL, BANK_SEL_SENS },
  { COM9, COM9_AGC_GAIN_64x },
  { BANK_SEL, BANK_SEL_DSP },
  { REG_RED, 0x52 },
  { REG_GREEN, 0x41 },
  { REG_BLUE, 0x66 },
  { 0xb2,   0x1f },
  { 0xb4,   0x00 },
  { 0xb8,   0x00 },
  { 0xb5,   0x9b },
  { 0xa1,   0xc2 },
  { 0xa4,   0x86 },
  { 0xb9,   0x6c },
};   
   
static struct regval_list sensor_wbp_home_regs[] = {
  { BANK_SEL, BANK_SEL_SENS },
  { COM9, COM9_AGC_GAIN_64x },
  { BANK_SEL, BANK_SEL_DSP },
  { REG_RED, 0x42 },
  { REG_GREEN, 0x3f },
  { REG_BLUE, 0x71 },
  { 0xb2,   0x00 },
  { 0xb4,   0x00 },
  { 0xb8,   0x1f },
  { 0xb5,   0x9b },
  { 0xa1,   0xc2 },
  { 0xa4,   0x86 },
  { 0xb9,   0x6c },
};   
   
static struct regval_list sensor_wbp_night_regs[] = {
  { BANK_SEL, BANK_SEL_SENS },
  { 0x0f, 0x4b },
  { 0x03, 0xcf },
  { COM9, COM9_AGC_GAIN_128x },
  { BANK_SEL, BANK_SEL_DSP },
  { REG_RED, 0x42 },
  { REG_GREEN, 0x3f },
  { REG_BLUE, 0x71 },
  { 0xb2,   0x00 },
  { 0xb4,   0x00 },
  { 0xb8,   0x1f },
  { 0xb5,   0x9b },
  { 0xa1,   0xc2 },
  { 0xa4,   0x86 },
  { 0xb9,   0x44 },
  { 0x7d,   0x12 },
  { 0x79,   0x28 },
  { 0x7a,   0x28 },
  { 0x0f,   0x4b },
  { 0x03,   0x8f },
};   
   
static struct regval_list sensor_wbp_horizon_regs[] = {
  { BANK_SEL, BANK_SEL_SENS },
  { COM9, COM9_AGC_GAIN_32x },
  { BANK_SEL, BANK_SEL_DSP },
  { REG_RED, 0x42 },
  { REG_GREEN, 0x41 },
  { REG_BLUE, 0x71 },
  { 0xb2,   0x00 },
  { 0xb4,   0x1f },
  { 0xb8,   0x00 },
  { 0xb5,   0x9b },
  { 0xa1,   0xc2 },
  { 0xa4,   0x86 },
  { 0xb9,   0x6c },
};   
 
static struct regval_list sensor_wbp_flash_regs[] = {
  { BANK_SEL, BANK_SEL_SENS },
  { COM9, COM9_AGC_GAIN_128x },
  { BANK_SEL, BANK_SEL_DSP },
  { REG_RED, 0x52 },
  { REG_GREEN, 0x41 },
  { REG_BLUE, 0x66 },
  { 0xb2,   0x1f },
  { 0xb4,   0x00 },
  { 0xb8,   0x00 },
  { 0xb5,   0x9b },
  { 0xa1,   0xc2 },
  { 0xa4,   0x86 },
  { 0xb9,   0x6c },
};   

static int sensor_g_wb(struct v4l2_subdev *sd, int *value)
{
  struct sensor_info *info = to_state(sd);
  enum v4l2_auto_n_preset_white_balance *wb_type = (enum v4l2_auto_n_preset_white_balance*)value;

  *wb_type = info->wb;

  return 0;
}

static int sensor_s_wb(struct v4l2_subdev *sd, enum v4l2_auto_n_preset_white_balance value)
{
  int ret; struct sensor_info *info = to_state(sd);

  switch (value)
  {
  case V4L2_WHITE_BALANCE_AUTO:
  case V4L2_WHITE_BALANCE_MANUAL:
    ret = sensor_write_array(sd, sensor_wbp_auto_regs, ARRAY_SIZE(sensor_wbp_auto_regs));
    break;
  case V4L2_WHITE_BALANCE_CLOUDY:
    ret = sensor_write_array(sd, sensor_wbp_cloudy_regs, ARRAY_SIZE(sensor_wbp_cloudy_regs));
    break;
  case V4L2_WHITE_BALANCE_DAYLIGHT:
      ret = sensor_write_array(sd, sensor_wbp_sunny_regs, ARRAY_SIZE(sensor_wbp_sunny_regs));
      break;
  case V4L2_WHITE_BALANCE_HORIZON:
    ret = sensor_write_array(sd, sensor_wbp_horizon_regs, ARRAY_SIZE(sensor_wbp_horizon_regs));
    break;
  case V4L2_WHITE_BALANCE_FLASH:
    ret = sensor_write_array(sd, sensor_wbp_flash_regs, ARRAY_SIZE(sensor_wbp_flash_regs));
    break;
  case V4L2_WHITE_BALANCE_INCANDESCENT:
    ret = sensor_write_array(sd, sensor_wbp_home_regs, ARRAY_SIZE(sensor_wbp_home_regs));
    break;
  case V4L2_WHITE_BALANCE_FLUORESCENT_H:
  case V4L2_WHITE_BALANCE_FLUORESCENT:
    ret = sensor_write_array(sd, sensor_wbp_office_regs, ARRAY_SIZE(sensor_wbp_office_regs));
    break;
  case V4L2_WHITE_BALANCE_SHADE:
    ret = sensor_write_array(sd, sensor_wbp_night_regs, ARRAY_SIZE(sensor_wbp_night_regs));
    break;
  default: return -EINVAL;
  }

  if (ret < 0) { vfe_dev_err("sensor_s_wb error, return %x!\n",ret); return ret; }

  if (value == V4L2_WHITE_BALANCE_AUTO) ret = sensor_s_autowb(sd, 1);
  else ret = sensor_s_autowb(sd, 0);
  if (ret < 0) { vfe_dev_err("sensor_s_wb error, return %x\n", ret); return ret; }


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

static int sensor_init(struct v4l2_subdev *sd, u32 value)
{
  int ret; unsigned char val; struct sensor_info *info = to_state(sd);
  
  vfe_dev_dbg("sensor_init 0x%x\n",value);

  ret = sensor_detect(sd);
  if (ret) { vfe_dev_err("chip found is not an target chip.\n"); return ret; }
  
  vfe_get_standby_mode(sd, &info->stby_mode);

  if ((info->stby_mode == HW_STBY || info->stby_mode == SW_STBY) && info->init_first_flag == 0)
  {
    vfe_dev_print("stby_mode and init_first_flag = 0\n");
    return 0;
  }

  SENSOR_BANK(BANK_SEL_SENS);
  val = COM7_SRST; SENSOR_WRITE(COM7);
  msleep(10);
  SENSOR_BANK(BANK_SEL_DSP);
  
  info->focus_status = 0;
  info->low_speed = 0;
  info->width = 0;
  info->height = 0;
  info->brightness = 0;
  info->contrast = 4;
  info->sharpness = 0;
  info->saturation = 4;
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
  info->band_filter = V4L2_CID_POWER_LINE_FREQUENCY_AUTO;
  info->jevois = JEVOIS_SENSOR_COLOR;

  info->tpf.numerator = 1;
  info->tpf.denominator = 30;    /* default 30fps */

  ret = sensor_write_array(sd, sensor_default_regs, ARRAY_SIZE(sensor_default_regs));
  if (ret < 0) { vfe_dev_err("write sensor_default_regs error\n"); return ret; }
  
  if (info->stby_mode == 0) info->init_first_flag = 0;
  info->preview_first_flag = 1;
  
  return 0;
}

static long sensor_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
  int ret; unsigned char val;
  unsigned char *data = (unsigned char *)arg;
  struct sensor_info *info = to_state(sd);

  switch (cmd)
  {
  case _IOW('V', 192, int): val = data[2]; SENSOR_WRITE(data[0]); break;
  case _IOWR('V', 193, int): SENSOR_READ(data[0]); data[1] = 0; data[2] = val; data[3] = 0; break;
  case _IOWR('V', 198, int): *((unsigned int *)arg) = info->jevois; break;
  default: return -EINVAL;
  }
  return 0;
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
    .intg_max   = 1248,
    .regs       = sensor_uxga_regs,
    .regs_size  = ARRAY_SIZE(sensor_uxga_regs),
    .set_size   = NULL,
  },
  /* SXGA */
  {
    .width      = JVSXGA_WIDTH,
    .height     = JVSXGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .intg_max   = 1248,
    .regs       = sensor_uxga_regs,
    .regs_size  = ARRAY_SIZE(sensor_uxga_regs),
    .set_size   = NULL,
  },
  /* 720p */
  {
    .width      = HD720_WIDTH,
    .height     = HD720_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .intg_max   = 1248,
    .regs       = sensor_uxga_regs,
    .regs_size  = ARRAY_SIZE(sensor_uxga_regs),
    .set_size   = NULL,
  },
  /* XGA */
  {
    .width      = XGA_WIDTH,
    .height     = XGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .intg_max   = 1248,
    .regs       = sensor_uxga_regs,
    .regs_size  = ARRAY_SIZE(sensor_uxga_regs),
    .set_size   = NULL,
  },
  /* SVGA */
  {
    .width      = SVGA_WIDTH,
    .height     = SVGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .intg_max   = 672,
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
    .intg_max   = 672,
    .regs       = sensor_svga_regs,
    .regs_size  = ARRAY_SIZE(sensor_svga_regs),
    .set_size   = NULL,
  },
  /* CIF */
  {
    .width      = CIF_WIDTH,
    .height     = CIF_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .intg_max   = 336,
    .regs       = sensor_cif_regs,
    .regs_size  = ARRAY_SIZE(sensor_cif_regs),
    .set_size   = NULL,
  },
  /* QVGA */
  {
    .width      = QVGA_WIDTH,
    .height     = QVGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .intg_max   = 336,
    .regs       = sensor_cif_regs,
    .regs_size  = ARRAY_SIZE(sensor_cif_regs),
    .set_size   = NULL,
  },
  /* QCIF */
  {
    .width      = QCIF_WIDTH,
    .height     = QCIF_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .intg_max   = 336,
    .regs       = sensor_cif_regs,
    .regs_size  = ARRAY_SIZE(sensor_cif_regs),
    .set_size   = NULL,
  },
  /* QQVGA */
  {
    .width      = QQVGA_WIDTH,
    .height     = QQVGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .intg_max   = 336,
    .regs       = sensor_cif_regs,
    .regs_size  = ARRAY_SIZE(sensor_cif_regs),
    .set_size   = NULL,
  },
  /* QQCIF */
  {
    .width      = QQCIF_WIDTH,
    .height     = QQCIF_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .intg_max   = 336,
    .regs       = sensor_cif_regs,
    .regs_size  = ARRAY_SIZE(sensor_cif_regs),
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
  int pclk_div_shift = 0;

  vfe_dev_dbg("sensor_s_fmt: version 008 ........................................\n");

  ret = sensor_try_fmt_internal(sd, fmt, &wsize);
  if (ret) return ret;

  SENSOR_BANK(BANK_SEL_DSP);
  SENSOR_WRITE2(R_BYPASS, R_BYPASS_DSP_BYPAS);
  SENSOR_WRITE2(REG_RESET, RESET_DVP);
  msleep(2);

  switch(fmt->code)
  {
  case V4L2_MBUS_FMT_YUYV8_2X8:
    SENSOR_WRITE2(IMAGE_MODE, IMAGE_MODE_YUV422);
    SENSOR_READ(CTRL0); val &= ~(CTRL0_RGB_EN | CTRL0_RAW_EN); val |= CTRL0_YUV422 | CTRL0_YUV_EN; SENSOR_WRITE(CTRL0);
    pclk_div_shift = 1;
    break;

  case V4L2_MBUS_FMT_RGB565_2X8_LE:
    SENSOR_WRITE2(IMAGE_MODE, IMAGE_MODE_RGB565);
    SENSOR_READ(CTRL0); val &= ~(CTRL0_YUV422 | CTRL0_YUV_EN | CTRL0_RAW_EN); val |= CTRL0_RGB_EN; SENSOR_WRITE(CTRL0);
    pclk_div_shift = 1;
    break;

  case V4L2_MBUS_FMT_SBGGR8_1X8:
    SENSOR_WRITE2(IMAGE_MODE, IMAGE_MODE_RAW10);
    SENSOR_READ(CTRL0); val &= ~(CTRL0_YUV422 | CTRL0_YUV_EN | CTRL0_RGB_EN); val |= CTRL0_RAW_EN; SENSOR_WRITE(CTRL0);
    pclk_div_shift = 0;
    break;
  }

  SENSOR_WRITE2(0xd7, 0x01);
  SENSOR_WRITE2(0xe1, 0x67);

  vfe_dev_dbg("sensor_s_fmt: using wsize->width=%d, wsize->height=%d\n", wsize->width, wsize->height);
  if (wsize->regs)
  {
    vfe_dev_dbg("sensor_s_fmt: writing window setup registers\n");
    ret = sensor_write_array(sd, wsize->regs , wsize->regs_size);
    if (ret < 0) return ret;
  }
  
  if (wsize->set_size) { ret = wsize->set_size(sd); if (ret < 0) return ret; }

  SENSOR_WRITE2(CTRLI, CTRLI_LP_DP | CTRLI_V_DIV_SET(0) | CTRLI_H_DIV_SET(0));
  SENSOR_WRITE2(ZMOW, ZMOW_OUTW_SET(wsize->width));
  SENSOR_WRITE2(ZMOH, ZMOH_OUTH_SET(wsize->height));
  SENSOR_WRITE2(ZMHH, ZMHH_OUTW_SET(wsize->width) | ZMHH_OUTH_SET(wsize->height));
  
  vfe_dev_dbg("sensor_s_fmt: using mbus_code = %x. Writing format setup registers.\n", fmt->code);

  SENSOR_BANK(BANK_SEL_DSP);
  SENSOR_WRITE2(R_BYPASS, R_BYPASS_USE_DSP);
  SENSOR_WRITE2(REG_RESET, 0x00);
  msleep(2);

  info->width = wsize->width;
  info->height = wsize->height;
  info->exp_max_lines = wsize->intg_max;
  info->mbus_code = fmt->code;

  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_READ(COM7);
  if (val & COM7_RES_UXGA) info->trow = 1922 >> pclk_div_shift;
  else if (val & COM7_RES_SVGA) info->trow = 1190 >> pclk_div_shift;
  else if (val & COM7_RES_CIF) info->trow = 595 >> pclk_div_shift;
  
  ret = sensor_s_brightness(sd, info->brightness);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset brightness -- IGNORED\n");

  ret = sensor_s_contrast(sd, info->contrast);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset contrast -- IGNORED\n");

  ret = sensor_s_sharpness(sd, info->sharpness);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset sharpness -- IGNORED\n");
  
  ret = sensor_s_saturation(sd, info->saturation);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset saturation -- IGNORED\n");

  ret = sensor_s_band_filter(sd, info->band_filter);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset banding filter -- IGNORED\n");

  ret = sensor_s_wb(sd, info->wb);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset white balance preset -- IGNORED\n");
  
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
  struct v4l2_captureparm *cp = &parms->parm.capture;
  struct v4l2_fract *tpf = &cp->timeperframe;
  struct sensor_info *info = to_state(sd);
  unsigned int ratex100, basex100, dpix, dlin; int ret = 0; unsigned char prescaler = 0;
  unsigned long fint, usdelta, num_dummy_lines = 0UL, num_vblank = 0UL;
  unsigned long long tmpll;
  
  vfe_dev_dbg("sensor_s_parm capmode=0x%x tpf.num=%d tpf.den=%d\n", cp->capturemode, tpf->numerator, tpf->denominator);
  
  if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) return -EINVAL;

  info->capture_mode = cp->capturemode;
  if (info->capture_mode == V4L2_MODE_IMAGE) { vfe_dev_dbg("Cannot set frame rate in image capture mode\n"); return 0; }

  if (tpf->numerator == 0 || tpf->denominator == 0)
  { tpf->numerator = 1; tpf->denominator = 30; vfe_dev_err("Frame rate reset to 30fps\n"); }

  info->tpf.denominator = tpf->denominator; info->tpf.numerator = tpf->numerator;
  ratex100 = (100 * tpf->denominator) / tpf->numerator;
  if (ratex100 < 30*100) info->low_speed = 1;
  vfe_dev_dbg("JEVOIS: set frame rate %d/100 fps width=%d\n", ratex100, info->width);

  if (info->width <= CIF_WIDTH)       { prescaler = 4; dpix = 595;  dlin = 336; }
  else if (info->width <= SVGA_WIDTH) { prescaler = 4; dpix = 1190; dlin = 672; }
  else                                { prescaler = 1; dpix = 1922; dlin = 1248; }

  if (info->width > CIF_WIDTH && info->width <= SVGA_WIDTH)
  {
    if (ratex100 > 40*100) return -EINVAL;
    else if (ratex100 > 20*100)
    { basex100 = 4000; num_dummy_lines = 20; if (prescaler < 2) return -EINVAL; else prescaler >>= 1; }
    else if (ratex100 > 10*100)
    { basex100 = 2000; num_dummy_lines = 25; if (prescaler < 1) return -EINVAL; }
    else if (ratex100 > 500)
    { basex100 = 1000; num_dummy_lines = 28; prescaler <<= 1; }
    else if (ratex100 > 250)
    { basex100 = 500; num_dummy_lines = 24; prescaler <<= 2; }
    else if (ratex100 >= 10)
    { basex100 = 250; num_dummy_lines = 22; prescaler <<= 4; }
    else return -EINVAL;
  }
  else
  {
    if (ratex100 > 60*100) return -EINVAL;
    else if (ratex100 > 30*100) { basex100 = 6000; if (prescaler < 2) return -EINVAL; else prescaler >>= 1; }
    else if (ratex100 > 15*100) { basex100 = 3000; if (prescaler < 1) return -EINVAL; }
    else if (ratex100 > 750)    { basex100 = 1500; prescaler <<= 1; }
    else if (ratex100 > 375)    { basex100 = 750; prescaler <<= 2; }
    else if (ratex100 >= 10)    { basex100 = 375; prescaler <<= 4; }
    else return -EINVAL;
    num_dummy_lines = 0;
  }
  
  if (info->mbus_code != V4L2_MBUS_FMT_SBGGR8_1X8) { prescaler >>= 1; if (prescaler == 0) return -EINVAL; }
  
  vfe_dev_dbg("JEVOIS prescaler is %u\n", prescaler);
  SENSOR_BANK(BANK_SEL_SENS);
  SENSOR_WRITE2(CLKRC, CLKRC_DIV_SET(prescaler)); 

  fint = MCLK / prescaler;
  if (info->mbus_code == V4L2_MBUS_FMT_SBGGR8_1X8) info->trow = (dpix * 1000000UL) / fint;
  else info->trow = (2 * dpix * 1000000UL) / fint;

  usdelta = 100UL * 1000000UL / ratex100 - 100UL * 1000000UL / basex100;
  tmpll = (unsigned long long)(usdelta); tmpll *= dlin * basex100; do_div(tmpll, 100000000ULL);
  num_dummy_lines += (unsigned long)tmpll;
  
  if (num_dummy_lines > dlin / 2) { num_vblank = num_dummy_lines / 2; num_dummy_lines -= num_vblank; }
  SENSOR_WRITE2(ADDVFH, (unsigned char)((num_vblank >> 8) & 0xff));
  SENSOR_WRITE2(ADDVFL, (unsigned char)(num_vblank & 0xff));

  SENSOR_WRITE2(FLH, (unsigned char)((num_dummy_lines >> 8) & 0xff));
  SENSOR_WRITE2(FLL, (unsigned char)(num_dummy_lines & 0xff));

  SENSOR_WRITE2(REG2A, 0); SENSOR_WRITE2(FRARL, 0);
  
  vfe_dev_dbg("JEVOIS: ratex100=%u basex100=%u usdelta=%ld num_dummy_lines=%ld\n",
              ratex100, basex100, usdelta, num_dummy_lines);
  return 0;
}


#ifndef V4L2_CTRL_FLAG_NEXT_COMPOUND
#define V4L2_CTRL_FLAG_NEXT_COMPOUND    0x40000000
#endif

static int sensor_queryctrl(struct v4l2_subdev *sd, struct v4l2_queryctrl *qc)
{
  qc->id &= ~(V4L2_CTRL_FLAG_NEXT_CTRL | V4L2_CTRL_FLAG_NEXT_COMPOUND);
  
  switch (qc->id)
  {
  case V4L2_CID_BRIGHTNESS: return v4l2_ctrl_query_fill(qc, -4, 4, 1, 0);
  case V4L2_CID_CONTRAST: return v4l2_ctrl_query_fill(qc, 0, 8, 1, 4);
  case V4L2_CID_SATURATION: return v4l2_ctrl_query_fill(qc, 0, 8, 1, 4);
  case V4L2_CID_VFLIP: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_HFLIP: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_GAIN: return v4l2_ctrl_query_fill(qc, 16, 2000, 1, 16);
  case V4L2_CID_AUTOGAIN: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
  case V4L2_CID_EXPOSURE_AUTO: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_DO_WHITE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 9, 1, 0);
  case V4L2_CID_AUTO_WHITE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);

  case V4L2_CID_EXPOSURE_ABSOLUTE: return v4l2_ctrl_query_fill(qc, 1, 5000, 1, 5000);
  case V4L2_CID_RED_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 255, 1, 128);
  case V4L2_CID_BLUE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 255, 1, 128);
  case V4L2_CID_SHARPNESS: return v4l2_ctrl_query_fill(qc, 0, 7, 1, 0);
  case V4L2_CID_POWER_LINE_FREQUENCY: return v4l2_ctrl_query_fill(qc, 0, 2, 1, 2);

  default: /* vfe_dev_dbg("Unsupported control 0x%x\n", qc->id); */ return -EINVAL;
  }
}

static int sensor_querymenu(struct v4l2_subdev *sd, struct v4l2_querymenu *qm)
{
  qm->id &= ~(V4L2_CTRL_FLAG_NEXT_CTRL | V4L2_CTRL_FLAG_NEXT_COMPOUND);
  
  switch (qm->id)
  {
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
    switch (qm->index)
    {
    case V4L2_WHITE_BALANCE_MANUAL: strcpy(qm->name, "manual"); return 0;
    case V4L2_WHITE_BALANCE_AUTO: strcpy(qm->name, "auto"); return 0;
    case V4L2_WHITE_BALANCE_INCANDESCENT: strcpy(qm->name, "incandescent"); return 0;
    case V4L2_WHITE_BALANCE_FLUORESCENT: strcpy(qm->name, "fluorescent"); return 0;
    case V4L2_WHITE_BALANCE_FLUORESCENT_H: strcpy(qm->name, "fluorescent_h"); return 0;
    case V4L2_WHITE_BALANCE_HORIZON: strcpy(qm->name, "horizon"); return 0;
    case V4L2_WHITE_BALANCE_DAYLIGHT: strcpy(qm->name, "daylight"); return 0;
    case V4L2_WHITE_BALANCE_FLASH: strcpy(qm->name, "flash"); return 0;
    case V4L2_WHITE_BALANCE_CLOUDY: strcpy(qm->name, "cloudy"); return 0;
    case V4L2_WHITE_BALANCE_SHADE: strcpy(qm->name, "shade"); return 0;
    default: vfe_dev_dbg("Bad V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE index %d\n", qm->index); return -EINVAL;
    }
    break;

  case V4L2_CID_EXPOSURE_AUTO:
    switch (qm->index)
    {
    case V4L2_EXPOSURE_AUTO: strcpy(qm->name, "auto"); return 0;
    case V4L2_EXPOSURE_MANUAL: strcpy(qm->name, "manual"); return 0;
    case V4L2_EXPOSURE_SHUTTER_PRIORITY: strcpy(qm->name, "shutter"); return 0;
    case V4L2_EXPOSURE_APERTURE_PRIORITY: strcpy(qm->name, "aperture"); return 0;
    default: vfe_dev_dbg("Bad V4L2_CID_EXPOSURE_AUTO index %d\n", qm->index); return -EINVAL;
    }
    break;

  case V4L2_CID_POWER_LINE_FREQUENCY:
    switch (qm->index)
    {
    case V4L2_CID_POWER_LINE_FREQUENCY_DISABLED: strcpy(qm->name, "disabled"); return 0;
    case V4L2_CID_POWER_LINE_FREQUENCY_50HZ: strcpy(qm->name, "50hz"); return 0;
    case V4L2_CID_POWER_LINE_FREQUENCY_60HZ: strcpy(qm->name, "60hz"); return 0;
    case V4L2_CID_POWER_LINE_FREQUENCY_AUTO: strcpy(qm->name, "auto"); return 0;
    default: vfe_dev_dbg("Bad V4L2_CID_POWER_LINE_FREQUENCY index %d\n", qm->index); return -EINVAL;
    }
    break;

  default: vfe_dev_dbg("Unsupported querymenu on control 0x%x\n", qm->id); return -EINVAL;
  }
}

static int sensor_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
  int ret;

  switch (ctrl->id)
  {
  case V4L2_CID_BRIGHTNESS: ret = sensor_g_brightness(sd, &ctrl->value); break;
  case V4L2_CID_CONTRAST: ret = sensor_g_contrast(sd, &ctrl->value); break;
  case V4L2_CID_SATURATION: ret = sensor_g_saturation(sd, &ctrl->value); break;
  case V4L2_CID_VFLIP: ret = sensor_g_vflip(sd, &ctrl->value); break;
  case V4L2_CID_HFLIP: ret = sensor_g_hmirror(sd, &ctrl->value); break;
  case V4L2_CID_GAIN: ret = sensor_g_gain(sd, &ctrl->value); break;
  case V4L2_CID_AUTOGAIN: ret = sensor_g_autogain(sd, &ctrl->value); break;
  case V4L2_CID_EXPOSURE_AUTO: ret = sensor_g_autoexp(sd, &ctrl->value); break;

  case V4L2_CID_DO_WHITE_BALANCE: ctrl->value = 0; ret = 0; break;
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE: ret = sensor_g_wb(sd, &ctrl->value); break;

  case V4L2_CID_AUTO_WHITE_BALANCE: ret = sensor_g_autowb(sd, &ctrl->value); break;
  case V4L2_CID_RED_BALANCE: ret = sensor_g_wb_red(sd, &ctrl->value); break;
  case V4L2_CID_BLUE_BALANCE: ret = sensor_g_wb_blue(sd, &ctrl->value); break;
  case V4L2_CID_SHARPNESS: ret = sensor_g_sharpness(sd, &ctrl->value); break;
  case V4L2_CID_EXPOSURE_ABSOLUTE: ret = sensor_g_exposure_absolute(sd, &ctrl->value); break;
  case V4L2_CID_POWER_LINE_FREQUENCY: ret = sensor_g_band_filter(sd, &ctrl->value); break;
  default: vfe_dev_dbg("Unsupported control 0x%x\n", ctrl->id); return -EINVAL;
  }
  
  return ret;
}

static int sensor_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{

  switch (ctrl->id)
  {
  case V4L2_CID_BRIGHTNESS: return sensor_s_brightness(sd, ctrl->value);
  case V4L2_CID_CONTRAST:  return sensor_s_contrast(sd, ctrl->value);
  case V4L2_CID_SATURATION: return sensor_s_saturation(sd, ctrl->value);
  case V4L2_CID_VFLIP: return sensor_s_vflip(sd, ctrl->value);
  case V4L2_CID_HFLIP: return sensor_s_hmirror(sd, ctrl->value);
  case V4L2_CID_GAIN: return sensor_s_gain(sd, ctrl->value);
  case V4L2_CID_AUTOGAIN: return sensor_s_autogain(sd, ctrl->value);
  case V4L2_CID_EXPOSURE_AUTO: return sensor_s_autoexp(sd, (enum v4l2_exposure_auto_type)ctrl->value);

  case V4L2_CID_DO_WHITE_BALANCE:
  {
    int oldval; sensor_g_autowb(sd, &oldval);
    if (oldval) return 0;
    sensor_s_autowb(sd, 1);
    mdelay(600);
    return sensor_s_autowb(sd, 0);
  }
  
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
    return sensor_s_wb(sd, (enum v4l2_auto_n_preset_white_balance) ctrl->value);

  case V4L2_CID_AUTO_WHITE_BALANCE: return sensor_s_autowb(sd, ctrl->value);
  case V4L2_CID_RED_BALANCE: return sensor_s_wb_red(sd, ctrl->value);
  case V4L2_CID_BLUE_BALANCE: return sensor_s_wb_blue(sd, ctrl->value);
  case V4L2_CID_SHARPNESS: return sensor_s_sharpness(sd, ctrl->value);
  case V4L2_CID_EXPOSURE_ABSOLUTE: return sensor_s_exposure_absolute(sd, ctrl->value);
  case V4L2_CID_POWER_LINE_FREQUENCY: return sensor_s_band_filter(sd, ctrl->value);
  default: vfe_dev_dbg("Unsupported control 0x%x\n", ctrl->id); return -EINVAL;
  }
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
  .querymenu = sensor_querymenu,
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

