/*
 JeVois Smart Embedded Machine Vision Toolkit - Copyright (C) 2016 by Laurent Itti, the University of Southern
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
#include <asm/div64.h>

#include "camera.h"
#include "ov9650_regs.h"

MODULE_AUTHOR("JeVois Inc");
MODULE_DESCRIPTION("A low-level driver for ov9650 sensors");
MODULE_LICENSE("GPL");

#define AF_WIN_NEW_COORD

#define DEV_DBG_EN      0

#if (DEV_DBG_EN == 1)
#define vfe_dev_dbg(x, arg...) printk("[OV9650] "x, ##arg)
#else
#define vfe_dev_dbg(x, arg...)
#endif
#define vfe_dev_err(x, arg...) printk("[OV9650] "x, ##arg)
#define vfe_dev_print(x, arg...) printk("[OV9650] "x, ##arg)

#define SENSOR_WRITE(reg)                                               \
  ret = sensor_write(sd, reg, val);                                     \
  if (ret < 0) { vfe_dev_err("Error writing %u to sensor register %u in %s\n", val, reg, __func__); return ret; }

#define SENSOR_READ(reg)                                                \
  ret = sensor_read(sd, reg, &val);                                     \
  if (ret < 0) { vfe_dev_err("Error reading from sensor register %u in %s\n", reg, __func__); return ret;	}

#define MCLK              (24*1000*1000)
#define VREF_POL          V4L2_MBUS_VSYNC_ACTIVE_HIGH
#define HREF_POL          V4L2_MBUS_HSYNC_ACTIVE_HIGH
#define CLK_POL           V4L2_MBUS_PCLK_SAMPLE_RISING
#define V4L2_IDENT_SENSOR 0x9650


#define CSI_STBY_ON     1
#define CSI_STBY_OFF    0

#define CSI_RST_ON      1
#define CSI_RST_OFF     0

#define CSI_PWR_ON      1
#define CSI_PWR_OFF     0

#define regval_list reg_list_a8_d8

#define SENSOR_FRAME_RATE 30

#define I2C_ADDR 0x60
#define SENSOR_NAME "ov9650"

struct sensor_format_struct;

static inline struct sensor_info *to_state(struct v4l2_subdev *sd)
{
  return container_of(sd, struct sensor_info, sd);
}

static struct regval_list sensor_default_regs[] = {
  { REG_COM7, COM7_RESET },
  
  { REG_COM2, COM2_DRIVE2X },
  { REG_COM5, 0x01 },
  { REG_COM10, COM10_VS_NEG | COM10_HS_NEG },
  { REG_COM17, 0x08 },
  { 0x16, 0x06 },

  /*
  { REG_CLKRC, 0x81 },
  { REG_DBLV, 0x0a },
  { REG_MBD, 0x3e },
  { REG_COM11, 0x09 },
  { REG_COM8, 0xe0 },
  { REG_BLUE, 0x80 },
  { REG_RED, 0x80 },
  { REG_GAIN, 0x00 },
  { REG_AECH, 0x00 },
  { REG_COM8, 0xe5 },
  { REG_OFON, 0x50 },
  { REG_ACOM, 0x92 },
  { REG_ADC, 0x00 },
  { 0x35, 0x81 },
  { REG_MVFP, 0x04 },
  { REG_AA1, 0x80 },
  { REG_COM7, 0x40 },
  { REG_COM1, 0x00 },
  { REG_COM3, 0x04 },
  { REG_COM4, 0x80 },
  { REG_HSTOP, 0xc6 },
  { REG_HSTART, 0x26 },
  { REG_HREF, 0xad },
  { REG_VREF, 0x00 },
  { REG_VSTOP, 0x3d },
  { REG_VSTART, 0x01 },
  { REG_EDGE, 0xa6 },
  { REG_COM9, 0x2e },
  { REG_COM16, 0x02 },
  */
  { REG_PSHFT, 0x00 },
  { 0x16, 0x06 },
  { REG_CHLF, 0xe2 },
  { REG_ARBLM, 0xbf },
  { 0x96, 0x04 },
  { REG_TSLB, 0x00 },
  { 0x8e, 0x00 },
  { REG_COM12, 0x77 },
  { REG_COM21, 0x06 },
  { 0x94, 0x88 },
  { 0x95, 0x88 },
  { REG_COM15, 0xc1 },
  { REG_GRCOM, 0x3f },
  { REG_COM6, 0x42 },
  { REG_COM13, 0x92 },
  { REG_HV, 0x40 },
  { 0x5c, 0xb9 },
  { 0x5d, 0x96 },
  { 0x5e, 0x10 },
  { 0x59, 0xc0 },
  { 0x5a, 0xaf },
  { 0x5b, 0x55 },
  { 0x43, 0xf0 },
  { 0x44, 0x10 },
  { 0x45, 0x68 },
  { 0x46, 0x96 },
  { 0x47, 0x60 },
  { 0x48, 0x80 },
  { 0x5f, 0xe0 },
  { 0x60, 0x8c },
  { 0x61, 0x20 },
  { REG_COM26, 0xd9 },
  { REG_COM25, 0x74 },
  { REG_COM23, 0x02 },
  { REG_COM8, 0xe7 },
  { REG_MTX1, 0x3a },
  { REG_MTX2, 0x3d },
  { REG_MTX3, 0x03 },
  { REG_MTX4, 0x12 },
  { REG_MTX5, 0x26 },
  { REG_MTX6, 0x38 },
  { REG_MTX7, 0x40 },
  { REG_MTX8, 0x40 },
  { REG_MTX9, 0x40 },
  { REG_MTXS, 0x0d },
  { REG_COM22, 0x23 },
  { REG_COM14, 0x02 },
  { REG_AA2, 0xb8 },
  { REG_AA3, 0x92 },
  { 0xab, 0x0a },
  { REG_DBLC1, 0xdf },
  { REG_DBLC_B, 0x00 },
  { REG_DBLC_R, 0x00 },
  { REG_DBLC_GB, 0x00 },
  { REG_DBLC_GR, 0x00 },
  { REG_TSLB, 0x01 },
  { REG_AEW, 0x70 },
  { REG_AEB, 0x64 },
  { REG_VPT, 0xc3 },
  { REG_EXHCH, 0x00 },
  { REG_EXHCL, 0x00 },
  { REG_GSP1, 0x40 },
  { REG_GSP2, 0x30 },
  { REG_GSP3, 0x4b },
  { REG_GSP4, 0x60 },
  { REG_GSP5, 0x70 },
  { REG_GSP6, 0x70 },
  { REG_GSP7, 0x70 },
  { REG_GSP8, 0x70 },
  { REG_GSP9, 0x60 },
  { REG_GSP10, 0x60 },
  { REG_GSP11, 0x50 },
  { REG_GSP12, 0x48 },
  { REG_GSP13, 0x3a },
  { REG_GSP14, 0x2e },
  { REG_GSP15, 0x28 },
  { REG_GSP16, 0x22 },
  { REG_GST1, 0x04 },
  { REG_GST2, 0x07 },
  { REG_GST3, 0x10 },
  { REG_GST4, 0x28 },
  { REG_GST5, 0x36 },
  { REG_GST6, 0x44 },
  { REG_GST7, 0x52 },
  { REG_GST8, 0x60 },
  { REG_GST9, 0x6c },
  { REG_GST10, 0x78 },
  { REG_GST11, 0x8c },
  { REG_GST12, 0x9e },
  { REG_GST13, 0xbb },
  { REG_GST14, 0xd2 },
  { REG_GST15, 0xe6 },

  { REG_COM2,   COM2_DRIVE2X },
  { REG_COM5,   0x00 }, 
  { REG_CLKRC,  0x81 }, 
  { REG_RSVD35, 0x81 }, 
  { REG_MANU,   0x80 }, 
  { REG_MANV,   0x80 }, 
  { REG_EXHCH,  0x00 }, 
  { REG_EXHCL,  0x00 }, 
  { REG_ADVFH,  0x00 }, 
  { REG_ADVFL,  0x00 }, 
  { REG_COM8,   COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT | COM8_AGC | COM8_AWB | COM8_AEC},
  { REG_COM24,  0x00 },
  { 0xaa,       0x00 },
  { 0xab,       0x00 },
  { REG_COM6,   0x43 },
  { REG_DBLV, 0x0a },
  { REG_MBD, 0x3e },
  { REG_COM11, 0x09 },
  { REG_COM8, 0xe0 },
  { REG_BLUE, 0x80 },
  { REG_RED, 0x80 },
  { REG_GAIN, 0x00 },
  { REG_AECH, 0x00 },
  { REG_COM8, 0xe5 },
  { REG_OFON, 0x50 },
  { REG_ACOM, 0x92 },
  { REG_ADC, 0x00 },
  { 0x35, 0x81 },
  { REG_COM5, 0x20 },
  { REG_MVFP, 0x04 },
  { REG_AA1, 0x80 },
  { REG_COM7, 0x40 },
  { REG_COM1, 0x00 },
  { REG_COM3, 0x04 },
  { REG_COM4, 0x80 },
  { REG_HSTOP, 0xc6 },
  { REG_HSTART, 0x26 },
  { REG_HREF, 0xad },
  { REG_VREF, 0x00 },
  { REG_VSTOP, 0x3d },
  { REG_VSTART, 0x01 },
  { REG_EDGE, 0xa6 },
  { REG_COM9, 0x6e | COM9_EXPO_DROP },
  { REG_COM16, 0x02 },
  { REG_COM17, 0x08 },
  { REG_PSHFT, 0x00 },
  { 0x16, 0x06 },
  { REG_CHLF, 0xe2 },
  { REG_ARBLM, 0xbf },
  { 0x96, 0x04 },
  { REG_TSLB, 0x00 },
  { 0x8e, 0x00 },
  { REG_COM12, 0x77 },
  { REG_COM21, 0x06 },
  { 0x94, 0x88 },
  { 0x95, 0x88 },
  { REG_COM15, 0xc1 },
  { REG_GRCOM, 0x3f },
  { REG_COM13, 0x92 },
  { REG_HV, 0x40 },
  { 0x5c, 0xb9 },
  { 0x5d, 0x96 },
  { 0x5e, 0x10 },
  { 0x59, 0xc0 },
  { 0x5a, 0xaf },
  { 0x5b, 0x55 },
  { 0x43, 0xf0 },
  { 0x44, 0x10 },
  { 0x45, 0x68 },
  { 0x46, 0x96 },
  { 0x47, 0x60 },
  { 0x48, 0x80 },
  { 0x5f, 0xe0 },
  { 0x60, 0x8c },
  { 0x61, 0x20 },
  { REG_COM26, 0xd9 },
  { REG_COM25, 0x74 },
  { REG_COM23, 0x02 },
  { REG_COM8, 0xe7 },
  { REG_MTX1, 0x3a },
  { REG_MTX2, 0x3d },
  { REG_MTX3, 0x03 },
  { REG_MTX4, 0x12 },
  { REG_MTX5, 0x26 },
  { REG_MTX6, 0x38 },
  { REG_MTX7, 0x40 },
  { REG_MTX8, 0x40 },
  { REG_MTX9, 0x40 },
  { REG_MTXS, 0x0d },
  { REG_COM22,  COM22_WHTPCOR | COM22_WHTPCOROPT | COM22_DENOISE_ENABLE },
  { REG_COM14, 0x02 },
  { REG_AA2, 0xb8 },
  { REG_AA3, 0x92 },
  { 0xab, 0x0a },
  { REG_DBLC1, 0xdf },
  { REG_DBLC_B, 0x00 },
  { REG_DBLC_R, 0x00 },
  { REG_DBLC_GB, 0x00 },
  { REG_DBLC_GR, 0x00 },
  { REG_TSLB, 0x01 },
  { REG_AEW, 0x70 },
  { REG_AEB, 0x64 },
  { REG_VPT, 0xc3 },

  { 0x13, 0xe5 },
  { 0x01, 0x97 },
  { 0x02, 0x7d },
  { 0x14, 0x1e },
  { 0x11, 0x81 },
  { 0x41, 0x02 },
  { 0x3f, 0xa6 },
  { 0x3d, 0x92 },
  { 0x66, 0x01 },

  { REG_LCC5, 0x01 },
  { REG_LCC4, 25 },
  { REG_LCC3, 15 },
};

static struct regval_list sensor_sxga_regs[] = {
  { REG_COM1, 0x00 },
  { REG_COM3, 0x00 },
  { REG_COM4, 0x00 },
  { REG_CLKRC, 0x80 },
  { REG_COM7, 0x00 },
  { REG_ADC, 0x81 },
  { REG_ACOM, 0x93 },
  { REG_OFON, 0x51 },
  { REG_EXHCH, 0x00 },
  { REG_EXHCL, 0x00 },
  { REG_ADVFH, 0x00 },
  { REG_ADVFL, 0x00 },
  { REG_DM_LNH, 0x00 },
  { REG_DM_LNL, 0x00 },
};

static struct regval_list sensor_vga_regs[] = {
  { REG_COM1, 0x00 },
  { REG_COM3, COM3_VARIOPIXEL1 },
  { REG_COM4, 0x00 },
  { REG_CLKRC, 0x81 },
  { REG_COM7, COM7_FMT_VGA },
  { REG_ADC, 0x91 },
  { REG_ACOM, 0x12 },
  { REG_OFON, 0x43 },
  { REG_EXHCH, 0x00 },
  { REG_EXHCL, 0x00 },
  { REG_ADVFH, 0x00 },
  { REG_ADVFL, 0x00 },
  { REG_DM_LNH, 0x00 },
  { REG_DM_LNL, 0x00 },
};

static struct regval_list sensor_qvga_regs[] = {
  { REG_COM1, 0x00 },
  { REG_COM3, COM3_VARIOPIXEL1 },
  { REG_COM4, COM4_VARIOPIXEL2 },
  { REG_CLKRC, 0x83 },
  { REG_COM7, COM7_FMT_QVGA },
  { REG_ADC, 0x91 },
  { REG_ACOM, 0x12 },
  { REG_OFON, 0x43 },
  { REG_EXHCH, 0x00 },
  { REG_EXHCL, 0x00 },
  { REG_ADVFH, 0x00 },
  { REG_ADVFL, 0x00 },
  { REG_DM_LNH, 0x00 },
  { REG_DM_LNL, 0x00 },

};

static struct regval_list sensor_qqvga_regs[] = {
  { REG_COM1, COM1_QQVGA_QQCIF | COM1_SKIP2 },
  { REG_COM3, COM3_VARIOPIXEL1 },
  { REG_COM4, COM4_VARIOPIXEL2 },
  { REG_CLKRC, 0x83 },
  { REG_COM7, COM7_FMT_QVGA },
  { REG_ADC, 0x91 },
  { REG_ACOM, 0x12 },
  { REG_OFON, 0x43 },
  { REG_EXHCH, 0x00 },
  { REG_EXHCL, 0x00 },
  { REG_ADVFH, 0x00 },
  { REG_ADVFL, 0x00 },
  { REG_DM_LNH, 0x00 },
  { REG_DM_LNL, 0x00 },

};

static struct regval_list sensor_cif_regs[] = {
  { REG_COM1, 0x00 },
  { REG_COM3, COM3_VARIOPIXEL1 },
  { REG_COM4, 0x00 },
  { REG_CLKRC, 0x83 },
  { REG_COM7, COM7_FMT_CIF },
  { REG_ADC, 0x91 },
  { REG_ACOM, 0x12 },
  { REG_OFON, 0x43 },
  { REG_EXHCH, 0x00 },
  { REG_EXHCL, 0x00 },
  { REG_ADVFH, 0x00 },
  { REG_ADVFL, 0x00 },
  { REG_DM_LNH, 0x00 },
  { REG_DM_LNL, 0x00 },

};

static struct regval_list sensor_qcif_regs[] = {
  { REG_COM1, 0x00 },
  { REG_COM3, COM3_VARIOPIXEL1 },
  { REG_COM4, COM4_VARIOPIXEL2 },
  { REG_CLKRC, 0x87 },
  { REG_COM7, COM7_FMT_QCIF },
  { REG_ADC, 0x91 },
  { REG_ACOM, 0x12 },
  { REG_OFON, 0x43 },
  { REG_EXHCH, 0x00 },
  { REG_EXHCL, 0x00 },
  { REG_ADVFH, 0x00 },
  { REG_ADVFL, 0x00 },
  { REG_DM_LNH, 0x00 },
  { REG_DM_LNL, 0x00 },

};

static struct regval_list sensor_qqcif_regs[] = {
  { REG_COM1, COM1_QQVGA_QQCIF | COM1_SKIP2 },
  { REG_COM3, COM3_VARIOPIXEL1 },
  { REG_COM4, COM4_VARIOPIXEL2 },
  { REG_CLKRC, 0x87 },
  { REG_COM7, COM7_FMT_QCIF },
  { REG_ADC, 0x91 },
  { REG_ACOM, 0x12 },
  { REG_OFON, 0x43 },
  { REG_EXHCH, 0x00 },
  { REG_EXHCL, 0x00 },
  { REG_ADVFH, 0x00 },
  { REG_ADVFL, 0x00 },
  { REG_DM_LNH, 0x00 },
  { REG_DM_LNL, 0x00 },
};


static struct regval_list sensor_oe_disable_regs[] = {
};

static struct regval_list sensor_oe_enable_regs[] = {
};

static struct regval_list sensor_sw_stby_on_regs[] = {
};

static struct regval_list sensor_sw_stby_off_regs[] = {
};


inline int sensor_read(struct v4l2_subdev *sd, unsigned short reg, unsigned char *value)
{
  return cci_read_a8_d8_no_restart(sd, reg, value);
}

inline int sensor_write(struct v4l2_subdev *sd, unsigned short reg, unsigned char value)
{
  return cci_write_a8_d8(sd, reg, value);
}

static int sensor_write_array(struct v4l2_subdev *sd, struct regval_list *regs, int array_size)
{
  int i = 0;

  if (!regs) return -EINVAL;

  while (i < array_size)
  {
    if (sensor_write(sd, regs->addr, regs->data)) return -EINVAL;
    ++i; ++regs;
  }
  return 0;
}

static int sensor_g_window(struct v4l2_subdev *sd, unsigned int * hstart, unsigned int * vstart,
                            unsigned int * hstop, unsigned int * vstop)
{
  int ret; unsigned char val;

  SENSOR_READ(REG_HSTART); *hstart = (unsigned int)(val) << 3;
  SENSOR_READ(REG_HSTOP);  *hstop = (unsigned int)(val) << 3;
  SENSOR_READ(REG_HREF);   *hstart |= (unsigned int)(val) & 0x07; *hstop |= ((unsigned int)(val) >> 3) & 0x07;
  SENSOR_READ(REG_VSTART); *vstart = (unsigned int)(val) << 3;
  SENSOR_READ(REG_VSTOP);  *vstop = (unsigned int)(val) << 3;
  SENSOR_READ(REG_VREF);   *vstart |= (unsigned int)(val) & 0x07; *vstop |= ((unsigned int)(val) >> 3) & 0x07;

  vfe_dev_dbg("sensor_g_window: hstart=0x%x hstop=0x%x vstart=0x%x vstop=0x%x\n", *hstart, *hstop, *vstart, *vstop);
  return ret;
}

static int sensor_s_window(struct v4l2_subdev *sd, unsigned int hstart, unsigned int vstart,
                            unsigned int hstop, unsigned int vstop)
{
  int ret; unsigned char val;
  unsigned char href, vref, com2;
  SENSOR_READ(REG_HREF); href = val & 0xc0;
  SENSOR_READ(REG_VREF); vref = val & 0xc0;
  SENSOR_READ(REG_COM2); com2 = val;

  val |= COM2_SOFTSLEEP; SENSOR_WRITE(REG_COM2);

  val = hstart >> 3; SENSOR_WRITE(REG_HSTART);
  val = hstop >> 3; SENSOR_WRITE(REG_HSTOP);
  val = href | (hstart & 0x07) | ((hstop & 0x07) << 3); SENSOR_WRITE(REG_HREF);
  val = vstart >> 3; SENSOR_WRITE(REG_VSTART);
  val = vstop >> 3; SENSOR_WRITE(REG_VSTOP);
  val = vref | (vstart & 0x07) | ((vstop & 0x07) << 3); SENSOR_WRITE(REG_VREF);

  val = com2; SENSOR_WRITE(REG_COM2);
  
  vfe_dev_dbg("sensor_set_window: hstart=0x%x hstop=0x%x vstart=0x%x vstop=0x%x\n", hstart, hstop, vstart, vstop);
  return ret;
}

static int sensor_g_autogain(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_READ(REG_COM8);
  if (val & COM8_AGC) *value = 1; else *value = 0;
  info->autogain = *value;
  return 0;
}

static int sensor_s_autogain(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_READ(REG_COM8);
  if (value) val |= COM8_AGC; else val &= ~COM8_AGC;
  SENSOR_WRITE(REG_COM8);
  info->autogain = value;
  return 0;
}

static int sensor_g_gain(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; unsigned char val; unsigned int tmp;
  SENSOR_READ(REG_GAIN); tmp = val;
  SENSOR_READ(REG_VREF); tmp |= ((unsigned int)(val & 0xC0)) << 2;

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

  val = rgain & 0xff; SENSOR_WRITE(REG_GAIN);
  SENSOR_READ(REG_VREF); val &= ~0xc0; val |= ((rgain >> 2) & 0xc0); SENSOR_WRITE(REG_VREF);

  return 0;
}

static int sensor_g_autoexp(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_READ(REG_COM8);
  if (val & COM8_AEC) *value = V4L2_EXPOSURE_AUTO; else *value = V4L2_EXPOSURE_MANUAL;
  info->autoexp = *value;
  return 0;
}

static int sensor_s_autoexp(struct v4l2_subdev *sd, enum v4l2_exposure_auto_type value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  
  SENSOR_READ(REG_COM8);
  switch (value)
  {
  case V4L2_EXPOSURE_AUTO: val |= (COM8_AEC | COM8_AGC); break;
  case V4L2_EXPOSURE_MANUAL: val &= ~(COM8_AEC | COM8_AGC); break;
  default: return -EINVAL;
  }
  SENSOR_WRITE(REG_COM8);

  info->autoexp = value;
  return 0;
}

static int sensor_g_exposure_absolute(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val; unsigned int exposure;

  SENSOR_READ(REG_COM1);
  exposure = val & 0x03;
  SENSOR_READ(REG_AECH);
  exposure |= (val << 2);
  SENSOR_READ(REG_AECHM);
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
  
  vfe_dev_dbg("OV9650 set exp: value=%d expo=%u em=%d tr=%d\n", value, exposure, info->exp_max_lines, info->trow);

  SENSOR_READ(REG_COM1); val &= ~(0x03); val |= (exposure & 0x3); SENSOR_WRITE(REG_COM1);
  val = (exposure >> 2) & 0xff; SENSOR_WRITE(REG_AECH);
  val = (exposure >> 10) & 0x3f; SENSOR_WRITE(REG_AECHM);

  info->exp = (exposure * info->trow + 50) / 100;
  
  return 0;
}

static int sensor_g_autowb(struct v4l2_subdev *sd, int *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_READ(REG_COM8);
  if (val & COM8_AWB) { *value = 1; info->autowb = 1; } else { *value = 0; info->autowb = 0; }
  return 0;
}

static int sensor_s_autowb(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;
  
  SENSOR_READ(REG_COM8);
  if (value) val |= COM8_AWB; else val &= ~COM8_AWB;
  SENSOR_WRITE(REG_COM8);
  if (value) info->autowb = 1; else info->autowb = 0;
  return 0;
}

static int sensor_g_wb_red(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned char val;
  SENSOR_READ(REG_RED); *value = (unsigned int)(val);
  return 0;
}

static int sensor_g_wb_blue(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned char val;
  SENSOR_READ(REG_BLUE); *value = (unsigned int)(val);
  return 0;
}

static int sensor_s_wb_red(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val = value & 0xff;
  SENSOR_WRITE(REG_RED);
  return 0;
}

static int sensor_s_wb_blue(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val = value & 0xff;
  SENSOR_WRITE(REG_BLUE);
  return 0;
}

/*
static int sensor_g_hue(struct v4l2_subdev *sd, __s32 *value)
{
  return -EINVAL;
}

static int sensor_s_hue(struct v4l2_subdev *sd, int value)
{
  return -EINVAL;
}
*/

static struct regval_list sensor_brightness_neg3_regs[] = { { REG_AEW, 0x1c }, { REG_AEB, 0x12 }, { REG_VPT, 0x50 } };
static struct regval_list sensor_brightness_neg2_regs[] = { { REG_AEW, 0x3d }, { REG_AEB, 0x30 }, { REG_VPT, 0x71 } };
static struct regval_list sensor_brightness_neg1_regs[] = { { REG_AEW, 0x50 }, { REG_AEB, 0x44 }, { REG_VPT, 0x92 } };
static struct regval_list sensor_brightness_zero_regs[] = { { REG_AEW, 0x70 }, { REG_AEB, 0x64 }, { REG_VPT, 0xc3 } };
static struct regval_list sensor_brightness_pos1_regs[] = { { REG_AEW, 0x90 }, { REG_AEB, 0x84 }, { REG_VPT, 0xd4 } };
static struct regval_list sensor_brightness_pos2_regs[] = { { REG_AEW, 0xc4 }, { REG_AEB, 0xbf }, { REG_VPT, 0xe9 } };
static struct regval_list sensor_brightness_pos3_regs[] = { { REG_AEW, 0xd8 }, { REG_AEB, 0xd0 }, { REG_VPT, 0xfa } };

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
  case -3: ret = sensor_write_array(sd, sensor_brightness_neg3_regs, ARRAY_SIZE(sensor_brightness_neg3_regs)); break;
  case -2: ret = sensor_write_array(sd, sensor_brightness_neg2_regs, ARRAY_SIZE(sensor_brightness_neg2_regs)); break;
  case -1: ret = sensor_write_array(sd, sensor_brightness_neg1_regs, ARRAY_SIZE(sensor_brightness_neg1_regs)); break;
  case  0: ret = sensor_write_array(sd, sensor_brightness_zero_regs, ARRAY_SIZE(sensor_brightness_zero_regs)); break;
  case  1: ret = sensor_write_array(sd, sensor_brightness_pos1_regs, ARRAY_SIZE(sensor_brightness_pos1_regs)); break;
  case  2: ret = sensor_write_array(sd, sensor_brightness_pos2_regs, ARRAY_SIZE(sensor_brightness_pos2_regs)); break;
  case  3: ret = sensor_write_array(sd, sensor_brightness_pos3_regs, ARRAY_SIZE(sensor_brightness_pos3_regs)); break;
  default: return -EINVAL;
  }
  
  if (ret < 0) { vfe_dev_err("sensor_write_array err at sensor_s_brightness!\n"); return ret; }
    
  info->brightness = value;
  return 0;
}

static struct regval_list sensor_contrast_neg3_regs[] = {
  { REG_GSP1, 0x20 }, { REG_GSP2, 0x50 }, { REG_GSP3, 0x80 }, { REG_GSP4, 0xC0 }, { REG_GSP5, 0xC0 },
  { REG_GSP6, 0xA0 }, { REG_GSP7, 0x90 }, { REG_GSP8, 0x78 }, { REG_GSP9, 0x78 }, { REG_GSP10, 0x78 },
  { REG_GSP11, 0x40 }, { REG_GSP12, 0x20 }, { REG_GSP13, 0x20 }, { REG_GSP14, 0x20 }, { REG_GSP15, 0x14 },
  { REG_GSP16, 0x0E }, { REG_GST1, 0x02 }, { REG_GST2, 0x07 }, { REG_GST3, 0x17 }, { REG_GST4, 0x47 },
  { REG_GST5, 0x5F }, { REG_GST6, 0x73 }, { REG_GST7, 0x85 }, { REG_GST8, 0x94 }, { REG_GST9, 0xA3 },
  { REG_GST10, 0xB2 }, { REG_GST11, 0xC2 }, { REG_GST12, 0xCA }, { REG_GST13, 0xDA }, { REG_GST14, 0xEA },
  { REG_GST15, 0xF4 } };

static struct regval_list sensor_contrast_neg2_regs[] = {
  { REG_GST1, 0x02 }, { REG_GST2, 0x07 }, { REG_GST3, 0x1f }, { REG_GST4, 0x49 }, { REG_GST5, 0x5a },
  { REG_GST6, 0x6a }, { REG_GST7, 0x79 }, { REG_GST8, 0x87 }, { REG_GST9, 0x94 }, { REG_GST10, 0x9f },
  { REG_GST11, 0xaf }, { REG_GST12, 0xbb }, { REG_GST13, 0xcf }, { REG_GST14, 0xdf }, { REG_GST15, 0xee },
  { REG_GSP1, 0x20 }, { REG_GSP2, 0x50 }, { REG_GSP3, 0xc0 }, { REG_GSP4, 0xa8 }, { REG_GSP5, 0x88 },
  { REG_GSP6, 0x80 }, { REG_GSP7, 0x78 }, { REG_GSP8, 0x70 }, { REG_GSP9, 0x68 }, { REG_GSP10, 0x58 },
  { REG_GSP11, 0x40 }, { REG_GSP12, 0x30 }, { REG_GSP13, 0x28 }, { REG_GSP14, 0x20 }, { REG_GSP15, 0x1e },
  { REG_GSP16, 0x17 } };

static struct regval_list sensor_contrast_neg1_regs[] = {
  { REG_GST1, 0x02 }, { REG_GST2, 0x06 }, { REG_GST3, 0x16 }, { REG_GST4, 0x3A }, { REG_GST5, 0x4C },
  { REG_GST6, 0x5C }, { REG_GST7, 0x6A }, { REG_GST8, 0x78 }, { REG_GST9, 0x84 }, { REG_GST10, 0x8E },
  { REG_GST11, 0x9E }, { REG_GST12, 0xAE }, { REG_GST13, 0xC6 }, { REG_GST14, 0xDA }, { REG_GST15, 0xEA },
  { REG_GSP1, 0x20 }, { REG_GSP2, 0x40 }, { REG_GSP3, 0x80 }, { REG_GSP4, 0x90 }, { REG_GSP5, 0x90 },
  { REG_GSP6, 0x80 }, { REG_GSP7, 0x70 }, { REG_GSP8, 0x70 }, { REG_GSP9, 0x60 }, { REG_GSP10, 0x50 },
  { REG_GSP11, 0x40 }, { REG_GSP12, 0x40 }, { REG_GSP13, 0x30 }, { REG_GSP14, 0x28 }, { REG_GSP15, 0x20 },
  { REG_GSP16, 0x1c } };

static struct regval_list sensor_contrast_zero_regs[] = {
  { REG_GST1, 0x04 }, { REG_GST2, 0x07 }, { REG_GST3, 0x10 }, { REG_GST4, 0x28 }, { REG_GST5, 0x36 },
  { REG_GST6, 0x44 }, { REG_GST7, 0x52 }, { REG_GST8, 0x60 }, { REG_GST9, 0x6C }, { REG_GST10, 0x78 },
  { REG_GST11, 0x8C }, { REG_GST12, 0x9E }, { REG_GST13, 0xBB }, { REG_GST14, 0xD2 }, { REG_GST15, 0xE6 },
  { REG_GSP1, 0x40 }, { REG_GSP2, 0x30 }, { REG_GSP3, 0x4b /* FIXME was 48 */}, { REG_GSP4, 0x60 }, { REG_GSP5, 0x70 },
  { REG_GSP6, 0x70 }, { REG_GSP7, 0x70 }, { REG_GSP8, 0x70 }, { REG_GSP9, 0x60 }, { REG_GSP10, 0x60 },
  { REG_GSP11, 0x50 }, { REG_GSP12, 0x48 }, { REG_GSP13, 0x3A }, { REG_GSP14, 0x2E }, { REG_GSP15, 0x28 },
  { REG_GSP16, 0x22 } };

static struct regval_list sensor_contrast_pos1_regs[] = {
  { REG_GST1, 0x04 }, { REG_GST2, 0x09 }, { REG_GST3, 0x13 }, { REG_GST4, 0x29 }, { REG_GST5, 0x35 },
  { REG_GST6, 0x41 }, { REG_GST7, 0x4D }, { REG_GST8, 0x59 }, { REG_GST9, 0x64 }, { REG_GST10, 0x6F },
  { REG_GST11, 0x85 }, { REG_GST12, 0x97 }, { REG_GST13, 0xB7 }, { REG_GST14, 0xCF }, { REG_GST15, 0xE3 },
  { REG_GSP1, 0x40 }, { REG_GSP2, 0x50 }, { REG_GSP3, 0x50 }, { REG_GSP4, 0x58 }, { REG_GSP5, 0x60 },
  { REG_GSP6, 0x60 }, { REG_GSP7, 0x60 }, { REG_GSP8, 0x60 }, { REG_GSP9, 0x58 }, { REG_GSP10, 0x58 },
  { REG_GSP11, 0x58 }, { REG_GSP12, 0x48 }, { REG_GSP13, 0x40 }, { REG_GSP14, 0x30 }, { REG_GSP15, 0x28 },
  { REG_GSP16, 0x26 } };

static struct regval_list sensor_contrast_pos2_regs[] = {
  { REG_GST1, 0x05 }, { REG_GST2, 0x0b }, { REG_GST3, 0x16 }, { REG_GST4, 0x2c }, { REG_GST5, 0x37 },
  { REG_GST6, 0x41 }, { REG_GST7, 0x4b }, { REG_GST8, 0x55 }, { REG_GST9, 0x5f }, { REG_GST10, 0x69 },
  { REG_GST11, 0x7c }, { REG_GST12, 0x8f }, { REG_GST13, 0xb1 }, { REG_GST14, 0xcf }, { REG_GST15, 0xe5 },
  { REG_GSP1, 0x50 }, { REG_GSP2, 0x60 }, { REG_GSP3, 0x58 }, { REG_GSP4, 0x58 }, { REG_GSP5, 0x58 },
  { REG_GSP6, 0x50 }, { REG_GSP7, 0x50 }, { REG_GSP8, 0x50 }, { REG_GSP9, 0x50 }, { REG_GSP10, 0x50 },
  { REG_GSP11, 0x4c }, { REG_GSP12, 0x4c }, { REG_GSP13, 0x44 }, { REG_GSP14, 0x3c }, { REG_GSP15, 0x2c },
  { REG_GSP16, 0x23 } };

static struct regval_list sensor_contrast_pos3_regs[] = {
  { REG_GST1, 0x04 }, { REG_GST2, 0x09 }, { REG_GST3, 0x14 }, { REG_GST4, 0x28 }, { REG_GST5, 0x32 },
  { REG_GST6, 0x3C }, { REG_GST7, 0x46 }, { REG_GST8, 0x4F }, { REG_GST9, 0x58 }, { REG_GST10, 0x61 },
  { REG_GST11, 0x73 }, { REG_GST12, 0x85 }, { REG_GST13, 0xA5 }, { REG_GST14, 0xc5 }, { REG_GST15, 0xDD },
  { REG_GSP1, 0x40 }, { REG_GSP2, 0x50 }, { REG_GSP3, 0x58 }, { REG_GSP4, 0x50 }, { REG_GSP5, 0x50 },
  { REG_GSP6, 0x50 }, { REG_GSP7, 0x50 }, { REG_GSP8, 0x48 }, { REG_GSP9, 0x48 }, { REG_GSP10, 0x48 },
  { REG_GSP11, 0x48 }, { REG_GSP12, 0x48 }, { REG_GSP13, 0x40 }, { REG_GSP14, 0x40 }, { REG_GSP15, 0x30 },
  { REG_GSP16, 0x2E } };

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
  case 0: ret = sensor_write_array(sd, sensor_contrast_neg3_regs, ARRAY_SIZE(sensor_contrast_neg3_regs)); break;
  case 1: ret = sensor_write_array(sd, sensor_contrast_neg2_regs, ARRAY_SIZE(sensor_contrast_neg2_regs)); break;
  case 2: ret = sensor_write_array(sd, sensor_contrast_neg1_regs, ARRAY_SIZE(sensor_contrast_neg1_regs)); break;
  case 3: ret = sensor_write_array(sd, sensor_contrast_zero_regs, ARRAY_SIZE(sensor_contrast_zero_regs)); break;
  case 4: ret = sensor_write_array(sd, sensor_contrast_pos1_regs, ARRAY_SIZE(sensor_contrast_pos1_regs)); break;
  case 5: ret = sensor_write_array(sd, sensor_contrast_pos2_regs, ARRAY_SIZE(sensor_contrast_pos2_regs)); break;
  case 6: ret = sensor_write_array(sd, sensor_contrast_pos3_regs, ARRAY_SIZE(sensor_contrast_pos3_regs)); break;
  default: return -EINVAL;
  }
  
  if (ret < 0) { vfe_dev_err("sensor_write_array err at sensor_s_contrast!\n"); return ret; }
  
  info->contrast = value;
  return 0;
}

static int sensor_g_sharpness(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);

  *value = info->sharpness;
  return 0;
}

static int sensor_s_sharpness(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;
  int v = value;
  
  SENSOR_READ(REG_COM14);

  if (v) val |= COM14_EDGE_EN; else val &= ~COM14_EDGE_EN;

  v -= 1; if (v > 0x0f) { val |= COM14_EEF_X2; v >>= 1; } else { val &= ~COM14_EEF_X2; }
  SENSOR_WRITE(REG_COM14);

  SENSOR_READ(REG_EDGE);
  val &= ~EDGE_FACTOR_MASK; val |= ((unsigned char)v & 0x0f);
  SENSOR_WRITE(REG_EDGE);
  
  info->sharpness = value;
  return 0;
}

static struct regval_list sensor_saturation_neg2_regs[] = {
  { REG_MTX1, 0x1d }, { REG_MTX2, 0x1f }, { REG_MTX3, 0x02 }, { REG_MTX4, 0x09 }, { REG_MTX5, 0x13 },
  { REG_MTX6, 0x1c } };

static struct regval_list sensor_saturation_neg1_regs[] = {
  { REG_MTX1, 0x2e }, { REG_MTX2, 0x31 }, { REG_MTX3, 0x02 }, { REG_MTX4, 0x0e }, { REG_MTX5, 0x1E },
  { REG_MTX6, 0x2d } };

static struct regval_list sensor_saturation_zero_regs[] = {
  { REG_MTX1, 0x3a }, { REG_MTX2, 0x3d }, { REG_MTX3, 0x03 }, { REG_MTX4, 0x12 }, { REG_MTX5, 0x26 },
  { REG_MTX6, 0x38 }, { REG_MTX7, 0x40 }, { REG_MTX8, 0x40 }, { REG_MTX9, 0x40 }, { REG_MTXS, 0x0d } };

static struct regval_list sensor_saturation_pos1_regs[] = {
  { REG_MTX1, 0x46 }, { REG_MTX2, 0x49 }, { REG_MTX3, 0x04 }, { REG_MTX4, 0x16 }, { REG_MTX5, 0x2e },
  { REG_MTX6, 0x43 } };

static struct regval_list sensor_saturation_pos2_regs[] = {
  { REG_MTX1, 0x57 }, { REG_MTX2, 0x5c }, { REG_MTX3, 0x05 }, { REG_MTX4, 0x1b }, { REG_MTX5, 0x39 },
  { REG_MTX6, 0x54 } };

static int sensor_g_saturation(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);

  *value = info->saturation;
  return 0;
}

static int sensor_s_saturation(struct v4l2_subdev *sd, int value)
{
  int ret;
  struct sensor_info *info = to_state(sd);

  if (info->mbus_code == V4L2_MBUS_FMT_RGB565_2X8_LE)
    vfe_dev_dbg("OV9650: saturation not available in RGB565 mode\n");
  else
  {
    switch (value)
    {
    case 0: ret = sensor_write_array(sd, sensor_saturation_neg2_regs, ARRAY_SIZE(sensor_saturation_neg2_regs)); break;
    case 1: ret = sensor_write_array(sd, sensor_saturation_neg1_regs, ARRAY_SIZE(sensor_saturation_neg1_regs)); break;
    case 2: ret = sensor_write_array(sd, sensor_saturation_zero_regs, ARRAY_SIZE(sensor_saturation_zero_regs)); break;
    case 3: ret = sensor_write_array(sd, sensor_saturation_pos1_regs, ARRAY_SIZE(sensor_saturation_pos1_regs)); break;
    case 4: ret = sensor_write_array(sd, sensor_saturation_pos2_regs, ARRAY_SIZE(sensor_saturation_pos2_regs)); break;
    default: return -EINVAL;
    }
    
    if (ret < 0) { vfe_dev_err("sensor_write_array err at sensor_s_saturation!\n"); return ret;	}
  }
  
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

  info->band_filter = value;
  return 0;
}

static int sensor_g_hmirror(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned char val;

  SENSOR_READ(REG_MVFP);
  *value = (val & MVFP_MIRROR) ? 0 : 1;
  return 0;
}

static int sensor_g_vflip(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned char val;

  SENSOR_READ(REG_MVFP);
  *value = (val & MVFP_FLIP) ? 0 : 1;
  return 0;
}

static int sensor_s_hmirror(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val;

  SENSOR_READ(REG_MVFP);
  switch (value)
  {
  case 0: val |= MVFP_MIRROR; break;
  case 1: val &= ~MVFP_MIRROR; break;
  default: return -EINVAL;
  }
  SENSOR_WRITE(REG_MVFP);
  return 0;
}

static int sensor_s_vflip(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val = value & 0xff;

  SENSOR_READ(REG_MVFP);
  switch (value)
  {
  case 0: val |= MVFP_FLIP; break;
  case 1: val &= ~MVFP_FLIP; break;
  default: return -EINVAL;
  }
  SENSOR_WRITE(REG_MVFP);
  return 0;
}

static struct regval_list sensor_wbp_auto_regs[] = {
  { REG_COM8, 0xe7 }, { REG_COM16, 0x02 }, { REG_EDGE, 0xa6 }, { REG_COM13, 0x92 }, { REG_LCC5, 0x01 },
  { REG_COM9, 0x4e }, { REG_COM11, 0x09 }, };

static struct regval_list sensor_wbp_sunny_regs[] = {
  { REG_COM8, 0xc5 }, { REG_BLUE, 0x74 }, { REG_RED, 0x90 }, { REG_COM9, 0x4e }, { REG_COM16, 0x02 },
  { REG_EDGE, 0xa6 }, { REG_COM13, 0x92 }, { REG_LCC5, 0x01 }, { REG_COM11, 0x09 }, };

static struct regval_list sensor_wbp_cloudy_regs[] = {
  { REG_COM8, 0xc5 }, { REG_BLUE, 0x74 }, { REG_RED, 0x90 }, { REG_COM9, 0x4e }, { REG_COM16, 0x02 },
  { REG_EDGE, 0xa6 }, { REG_COM13, 0x92 }, { REG_LCC5, 0x01 }, { REG_COM11, 0x09 }, };

static struct regval_list sensor_wbp_office_regs[] = {
  { REG_COM8, 0xe5 }, { REG_BLUE, 0x97 }, { REG_RED, 0x7d }, { REG_COM9, 0x6e }, { REG_COM16, 0x02 },
  { REG_EDGE, 0xa6 }, { REG_COM13, 0x92 }, { REG_LCC5, 0x01 }, { REG_COM11, 0x09 }, };

static struct regval_list sensor_wbp_home_regs[] = {
  { REG_COM8, 0xe5 }, { REG_BLUE, 0x9c }, { REG_RED, 0x54 }, { REG_COM9, 0x6e }, { REG_COM16, 0x02 },
  { REG_EDGE, 0xa6 }, { REG_COM13, 0x92 }, { REG_LCC5, 0x01 }, { REG_COM11, 0xe8 },  };

static struct regval_list sensor_wbp_night_regs[] = {
  { REG_COM16, 0x00 }, { REG_EDGE, 0xf3 }, { REG_COM13, 0x12 }, { REG_LCC5, 0x00 }, { REG_COM8, 0xe7 },
  { REG_COM9, 0x6e }, { REG_COM11, 0xe8 }, };

static struct regval_list sensor_wbp_horizon_regs[] = {
  { REG_COM8, 0xc5 }, { REG_BLUE, 0x9c }, { REG_RED, 0x54 }, { REG_COM9, 0x4e }, { REG_COM16, 0x02 },
  { REG_EDGE, 0xa6 }, { REG_COM13, 0x92 }, { REG_LCC5, 0x01 }, { REG_COM11, 0x09 }, };

static struct regval_list sensor_wbp_flash_regs[] = {
  { REG_COM8, 0xe5 }, { REG_BLUE, 0x97 }, { REG_RED, 0x7d }, { REG_COM9, 0x2e }, { REG_COM16, 0x02 },
  { REG_EDGE, 0xa6 }, { REG_COM13, 0x92 }, { REG_LCC5, 0x01 }, { REG_COM11, 0x09 }, };

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

  if (value == V4L2_WHITE_BALANCE_AUTO)
  {
    ret = sensor_s_autowb(sd, 1);
    if (ret < 0) { vfe_dev_err("sensor_s_autowb error, return %x\n", ret); return ret; }
    ret = sensor_write_array(sd, sensor_wbp_auto_regs, ARRAY_SIZE(sensor_wbp_auto_regs));
  }
  else
  {
    ret = sensor_s_autowb(sd, 0);
    if (ret < 0) { vfe_dev_err("sensor_s_autowb error, return %x\n", ret); return ret; }

    switch (value)
    {
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

    default:
      return -EINVAL;
    }
  }

  if (ret < 0) { vfe_dev_err("sensor_s_wb error, return %x!\n",ret); return ret; }

  msleep(50);

  info->wb = value;
  return 0;
}

static int sensor_power(struct v4l2_subdev *sd, int on)
{
  int ret;

  ret = 0;
  switch(on)
  {
  case CSI_SUBDEV_STBY_ON:
    vfe_dev_dbg("CSI_SUBDEV_STBY_ON\n");
    
    ret = sensor_write_array(sd, sensor_sw_stby_on_regs, ARRAY_SIZE(sensor_sw_stby_on_regs));
    if(ret < 0) vfe_dev_err("soft stby failed\n");
    msleep(10);
    
    vfe_dev_print("disable oe\n");
    ret = sensor_write_array(sd, sensor_oe_disable_regs, ARRAY_SIZE(sensor_oe_disable_regs));
    if(ret < 0) vfe_dev_err("disable oe falied!\n");
    
    cci_lock(sd);
    
    vfe_gpio_write(sd,PWDN,CSI_STBY_ON);
    
    cci_unlock(sd);
    
    vfe_set_mclk(sd,OFF);
    break;
    
  case CSI_SUBDEV_STBY_OFF:
    vfe_dev_dbg("CSI_SUBDEV_STBY_OFF\n");
    cci_lock(sd);
    
    vfe_set_mclk_freq(sd, MCLK);
    vfe_set_mclk(sd, ON);
    msleep(10);
    
    vfe_gpio_write(sd, PWDN, CSI_STBY_OFF);
    msleep(10);
    
    cci_unlock(sd);
    vfe_dev_print("enable oe\n");
    ret = sensor_write_array(sd, sensor_oe_enable_regs,  ARRAY_SIZE(sensor_oe_enable_regs));
    if(ret < 0) vfe_dev_err("enable oe failed\n");
    
    ret = sensor_write_array(sd, sensor_sw_stby_off_regs, ARRAY_SIZE(sensor_sw_stby_off_regs));
    if(ret < 0) vfe_dev_err("soft stby off failed\n");
    msleep(10);
    break;
    
  case CSI_SUBDEV_PWR_ON:
    vfe_dev_dbg("CSI_SUBDEV_PWR_ON\n");
    cci_lock(sd);
    
    vfe_gpio_set_status(sd, PWDN, 1);
    vfe_gpio_set_status(sd, RESET, 1);
    usleep_range(10000,12000);
    
    vfe_gpio_write(sd, PWDN, CSI_STBY_ON);
    
    vfe_gpio_write(sd, RESET, CSI_RST_ON);
    msleep(1);
    
    vfe_set_mclk_freq(sd, MCLK);
    vfe_set_mclk(sd, ON);
    msleep(10);
    
    vfe_gpio_write(sd, POWER_EN, CSI_PWR_ON);
    vfe_set_pmu_channel(sd, IOVDD, ON);
    vfe_set_pmu_channel(sd, AVDD, ON);
    vfe_set_pmu_channel(sd, DVDD, ON);
    vfe_set_pmu_channel(sd, AFVDD, ON);
    msleep(20);
    
    vfe_gpio_write(sd, PWDN, CSI_STBY_OFF);
    msleep(10);
    
    vfe_gpio_write(sd, RESET, CSI_RST_OFF);
    msleep(30);
    
    cci_unlock(sd);
    break;
    
  case CSI_SUBDEV_PWR_OFF:
    vfe_dev_dbg("CSI_SUBDEV_PWR_OFF\n");
    cci_lock(sd);
    
    vfe_set_mclk(sd, OFF);
    
    vfe_gpio_write(sd, POWER_EN, CSI_PWR_OFF);
    vfe_set_pmu_channel(sd, AFVDD, OFF);
    vfe_set_pmu_channel(sd, DVDD, OFF);
    vfe_set_pmu_channel(sd, AVDD, OFF);
    vfe_set_pmu_channel(sd, IOVDD, OFF);
    
    msleep(10);
    vfe_gpio_write(sd, POWER_EN, CSI_STBY_OFF);
    vfe_gpio_write(sd, RESET, CSI_RST_ON);
    
    vfe_gpio_set_status(sd, RESET, 0);//set the gpio to input
    vfe_gpio_set_status(sd, PWDN, 0);//set the gpio to input
    
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
  case 0: vfe_gpio_write(sd, RESET, CSI_RST_OFF); msleep(30); break;
  case 1: vfe_gpio_write(sd, RESET, CSI_RST_ON); msleep(10); break;
  default: return -EINVAL;
  }
  
  return 0;
}

static int sensor_detect(struct v4l2_subdev *sd)
{
  int ret; unsigned char val;

  SENSOR_READ(REG_PID); if (val != 0x96) return -ENODEV;
  SENSOR_READ(REG_VER); if (val != 0x50 && val != 0x52 && val != 0x53) return -ENODEV;

  vfe_dev_print("Detected Omnivision OV965x camera: ov96%x\n", val);

  return 0;
}

static int sensor_init(struct v4l2_subdev *sd, u32 val)
{
  int ret;
  struct sensor_info *info = to_state(sd);
  
  vfe_dev_dbg("sensor_init 0x%x\n", val);
  
  ret = sensor_detect(sd);
  if (ret) { vfe_dev_err("Sensor chip found is not a target chip for this driver\n"); return ret; }

  vfe_get_standby_mode(sd, &info->stby_mode);

  if ((info->stby_mode == HW_STBY || info->stby_mode == SW_STBY) && info->init_first_flag == 0)
  { vfe_dev_print("stby_mode and init_first_flag = 0\n"); return 0; }
  
  info->focus_status = 0;
  info->low_speed = 0;
  info->width = 0;
  info->height = 0;
  info->brightness = 0;
  info->contrast = 3;
  info->saturation = 2;
  info->hue = 0;
  info->gain = 16;
  info->autogain = 1;
  info->exp_bias = 0;
  info->autoexp = 1;
  info->autowb = 1;
  info->wb = V4L2_WHITE_BALANCE_AUTO;
  info->clrfx = V4L2_COLORFX_NONE;
  info->band_filter = V4L2_CID_POWER_LINE_FREQUENCY_60HZ;
  info->tpf.numerator = 1;
  info->tpf.denominator = 30;    /* 30fps */
  info->sharpness = 6;
  info->exp = 500;
  
  vfe_dev_dbg("Writing sensor default regs\n");
  ret = sensor_write_array(sd, sensor_default_regs, ARRAY_SIZE(sensor_default_regs));
  if (ret < 0) { vfe_dev_err("write sensor_default_regs error\n"); return ret; }

  sensor_s_band_filter(sd, info->band_filter);
  sensor_s_autowb(sd, info->autowb);

  if (info->stby_mode == 0) info->init_first_flag = 0;
  
  info->preview_first_flag = 1;

  return 0;
}

static long sensor_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
  int ret; unsigned char val;
  unsigned char *data = (unsigned char *)arg;

  switch (cmd)
  {
  case _IOW('V', 192, int): val = data[2]; SENSOR_WRITE(data[0]); break;
  case _IOWR('V', 193, int): SENSOR_READ(data[0]); data[1] = 0; data[2] = val; data[3] = 0; break;
  default: return -EINVAL;
  }

  return 0;
}


static struct sensor_win_size sensor_win_sizes[] = {
  /* SXGA */
  {
    .width      = JVSXGA_WIDTH,
    .height     = JVSXGA_HEIGHT,
    .hoffset    = 220,
    .voffset    = 8,
    .intg_max   = 1048,
    .regs       = sensor_sxga_regs,
    .regs_size  = ARRAY_SIZE(sensor_sxga_regs),
    .set_size   = NULL,
  },
  /* VGA */
  {
    .width      = VGA_WIDTH,
    .height     = VGA_HEIGHT,
    .hoffset    = 304,
    .voffset    = 8,
    .intg_max   = 498,
    .regs       = sensor_vga_regs,
    .regs_size  = ARRAY_SIZE(sensor_vga_regs),
    .set_size   = NULL,
  },
  /* CIF */
  {
    .width      = CIF_WIDTH,
    .height     = CIF_HEIGHT,
    .hoffset    = 316,
    .voffset    = 4,
    .intg_max   = 382,
    .regs       = sensor_cif_regs,
    .regs_size  = ARRAY_SIZE(sensor_cif_regs),
    .set_size   = NULL,
  },
  /* QVGA */
  {
    .width      = QVGA_WIDTH,
    .height     = QVGA_HEIGHT,
    .hoffset    = 304,
    .voffset    = 6,
    .intg_max   = 248,
    .regs       = sensor_qvga_regs,
    .regs_size  = ARRAY_SIZE(sensor_qvga_regs),
    .set_size   = NULL,
  },
  /* QCIF */
  {
    .width      = QCIF_WIDTH,
    .height     = QCIF_HEIGHT,
    .hoffset    = 316,
    .voffset    = 6,
    .intg_max   = 190,
    .regs       = sensor_qcif_regs,
    .regs_size  = ARRAY_SIZE(sensor_qcif_regs),
    .set_size   = NULL,
  },
  /* QQVGA */
  {
    .width      = QQVGA_WIDTH,
    .height     = QQVGA_HEIGHT,
    .hoffset    = 304,
    .voffset    = 2,
    .intg_max   = 248,
    .regs       = sensor_qqvga_regs,
    .regs_size  = ARRAY_SIZE(sensor_qqvga_regs),
    .set_size   = NULL,
  },
  /* QQCIF */
  {
    .width      = QQCIF_WIDTH,
    .height     = QQCIF_HEIGHT,
    .hoffset    = 316,
    .voffset    = 2,
    .intg_max   = 190,
    .regs       = sensor_qqcif_regs,
    .regs_size  = ARRAY_SIZE(sensor_qqcif_regs),
    .set_size   = NULL,
  },
};

#define N_WIN_SIZES (ARRAY_SIZE(sensor_win_sizes))

static struct regval_list rgb565_regs[] = {
  { REG_COM15, COM15_R00FF | COM15_RGB565 | 0x01 }, { REG_MTX1, 0xb7 }, { REG_MTX2, 0x2e }, { REG_MTX3, 0x09 },
  { REG_MTX4, 0x1f }, { REG_MTX5, 0xb1 }, { REG_MTX6, 0x12 }, { REG_MTX7, 0x06 }, { REG_MTX8, 0x55 },
  { REG_MTX9, 0xdb }, { REG_MTXS, 0x77 }, { REG_COM13, 0x90 },
};

static struct regval_list yuyv_regs[] = {
  { REG_COM15, COM15_R00FF | 0x01 }, { REG_MTX1, 0x3a }, { REG_MTX2, 0x3d }, { REG_MTX3, 0x03 },
  { REG_MTX4, 0x12 }, { REG_MTX5, 0x26 }, { REG_MTX6, 0x38 }, { REG_MTX7, 0x40 }, { REG_MTX8, 0x40 },
  { REG_MTX9, 0x40 }, { REG_MTXS, 0x0d }, { REG_COM13, 0x92 },
};

static struct regval_list bayer_regs[] = {
  { REG_COM15, COM15_R00FF | 0x01 }, { REG_MTX1, 0x3a }, { REG_MTX2, 0x3d }, { REG_MTX3, 0x03 },
  { REG_MTX4, 0x12 }, { REG_MTX5, 0x26 }, { REG_MTX6, 0x38 }, { REG_MTX7, 0x40 }, { REG_MTX8, 0x40 },
  { REG_MTX9, 0x40 }, { REG_MTXS, 0x0d }, { REG_COM13, 0x92 },
};

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

static int sensor_try_fmt_internal(struct v4l2_subdev *sd,
                                   struct v4l2_mbus_framefmt *fmt,
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
  
  if (wsize >= sensor_win_sizes + N_WIN_SIZES) wsize--;
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
  int ret; struct sensor_info *info = to_state(sd); unsigned char val; struct sensor_win_size * wsize; 
  unsigned int hstart, vstart, hstop, vstop;

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

  SENSOR_READ(REG_COM7);
  switch(fmt->code)
  {
  case V4L2_MBUS_FMT_YUYV8_2X8:
    val &= ~(COM7_RGB | COM7_BAYER); val |= COM7_YUV; SENSOR_WRITE(REG_COM7);
    sensor_write_array(sd, yuyv_regs, ARRAY_SIZE(yuyv_regs));
    break;
    
  case V4L2_MBUS_FMT_RGB565_2X8_LE:
    val &= ~(COM7_YUV | COM7_BAYER); val |= COM7_RGB; SENSOR_WRITE(REG_COM7);
    sensor_write_array(sd, rgb565_regs, ARRAY_SIZE(rgb565_regs));
    break;

  case V4L2_MBUS_FMT_SBGGR8_1X8:
    val &= ~COM7_YUV; val |= COM7_BAYER | COM7_RGB; SENSOR_WRITE(REG_COM7);
    sensor_write_array(sd, bayer_regs, ARRAY_SIZE(bayer_regs));
    break;
  }
  
  info->width = wsize->width;
  info->height = wsize->height;
  info->exp_max_lines = wsize->intg_max;
  info->mbus_code = fmt->code;

  ret = sensor_s_brightness(sd, info->brightness);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset brightness -- IGNORED\n");

  ret = sensor_s_contrast(sd, info->contrast);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset contrast -- IGNORED\n");

  ret = sensor_s_saturation(sd, info->saturation);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to reset saturation -- IGNORED\n");

  ret = sensor_s_band_filter(sd, info->band_filter);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to banding filter -- IGNORED\n");

  hstart = wsize->hoffset; vstart = wsize->voffset;

  if (fmt->code == V4L2_MBUS_FMT_YUYV8_2X8 && wsize->width == JVSXGA_WIDTH) { hstart += 16; vstart += 2; }
  
  switch (wsize->width)
  {
  case JVSXGA_WIDTH: hstop = hstart + wsize->width; vstop = vstart + wsize->height; break;
  case VGA_WIDTH: hstop = hstart + wsize->width * 2; vstop = vstart + wsize->height; break;
  case CIF_WIDTH: hstop = hstart + wsize->width * 2; vstop = vstart + wsize->height; break;
  case QVGA_WIDTH: hstop = hstart + wsize->width * 4; vstop = vstart + wsize->height; break;
  case QCIF_WIDTH: hstop = hstart + wsize->width * 4; vstop = vstart + wsize->height; break;
  case QQVGA_WIDTH: hstop = hstart + wsize->width * 8; vstop = vstart + wsize->height * 2; break;
  case QQCIF_WIDTH: hstop = hstart + wsize->width * 8; vstop = vstart + wsize->height * 2; break;
  default:
    vfe_dev_dbg("sensor_s_fmt: unsupported window size for offset adjustment\n");
    hstop = hstart + wsize->width; vstop = vstart + wsize->height;
  }

  ret = sensor_s_window(sd, hstart, vstart, hstop, vstop);
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to adjust window offsets -- IGNORED\n");

  return 0;
}

static int sensor_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
  struct sensor_info *info = to_state(sd);
  struct v4l2_captureparm *cp = &parms->parm.capture;
  
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
  unsigned int ratex100, basex100, dpix, dlin; int ret = 0; unsigned char prescaler = 0; unsigned char val;
  unsigned long fint, usdelta, num_dummy_lines = 0UL;
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

  switch(info->width)
  {
  case QQCIF_WIDTH: prescaler = 8; dpix = 1040; dlin = 192; break;
  case QQVGA_WIDTH: prescaler = 4; dpix = 1600; dlin = 250; break;
  case QCIF_WIDTH: prescaler = 8; dpix = 1040; dlin = 192; break;
  case QVGA_WIDTH: prescaler = 4; dpix = 1600; dlin = 250; break;
  case CIF_WIDTH: prescaler = 4; dpix = 1040; dlin = 384; break;
  case VGA_WIDTH: prescaler = 2; dpix = 1600; dlin = 500; break;
  case JVSXGA_WIDTH: prescaler = 1; dpix = 1520; dlin = 1050; break;
  default: return -EINVAL;
  }

  if (ratex100 > 1200*100) return -EINVAL;
  else if (ratex100 > 60*100) { basex100 = 12000; if (prescaler < 8) return -EINVAL; else prescaler >>= 3; }
  else if (ratex100 > 30*100) { basex100 = 6000; if (prescaler < 4) return -EINVAL; else prescaler >>= 2; }
  else if (ratex100 > 15*100) { basex100 = 3000; if (prescaler < 2) return -EINVAL; else prescaler >>= 1; }
  else if (ratex100 > 750)    { basex100 = 1500; if (prescaler < 1) return -EINVAL; }
  else if (ratex100 > 375)    { basex100 = 750; prescaler <<= 1; }
  else if (ratex100 >= 10)    { basex100 = 375; prescaler <<= 2; }
  else return -EINVAL;

  if (info->width == JVSXGA_WIDTH && info->mbus_code != V4L2_MBUS_FMT_SBGGR8_1X8)
  { prescaler >>= 1; if (prescaler == 0) return -EINVAL; }
  
  vfe_dev_dbg("JEVOIS prescaler is %u\n", prescaler);
  val = 0x80 | (prescaler - 1);
  SENSOR_WRITE(REG_CLKRC); 

  fint = MCLK * ((val >> 7) + 1) / ((2 * ((val & 0x3f) + 1)));
  if (info->mbus_code == V4L2_MBUS_FMT_SBGGR8_1X8) info->trow = (1520 * 1000000UL) / fint;
  else info->trow = (2 * 1520 * 1000000UL) / fint;

  usdelta = 100UL * 1000000UL / ratex100 - 100UL * 1000000UL / basex100;
  tmpll = (unsigned long long)(usdelta); tmpll *= dlin * basex100; do_div(tmpll, 100000000ULL);
  num_dummy_lines = (unsigned long)tmpll;
  
  val = (num_dummy_lines >> 8) & 0xff; SENSOR_WRITE(REG_DM_LNH);
  val = num_dummy_lines & 0xff; SENSOR_WRITE(REG_DM_LNL);

  val = 0; SENSOR_WRITE(REG_EXHCL); SENSOR_READ(REG_EXHCH); val &= ~(0x70); SENSOR_WRITE(REG_EXHCH);
  
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
  case V4L2_CID_BRIGHTNESS: return v4l2_ctrl_query_fill(qc, -3, 3, 1, 0);
  case V4L2_CID_CONTRAST: return v4l2_ctrl_query_fill(qc, 0, 6, 1, 3);
  case V4L2_CID_SATURATION: return v4l2_ctrl_query_fill(qc, 0, 4, 1, 2);
  case V4L2_CID_VFLIP: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_HFLIP: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_GAIN: return v4l2_ctrl_query_fill(qc, 16, 1023, 1, 16);
  case V4L2_CID_AUTOGAIN: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
  case V4L2_CID_EXPOSURE_AUTO: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_DO_WHITE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 9, 1, 0);
  case V4L2_CID_AUTO_WHITE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);

  case V4L2_CID_EXPOSURE_ABSOLUTE: return v4l2_ctrl_query_fill(qc, 1, 1000, 1, 1000);
  case V4L2_CID_RED_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 255, 1, 128);
  case V4L2_CID_BLUE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 255, 1, 128);
  case V4L2_CID_SHARPNESS: return v4l2_ctrl_query_fill(qc, 0, 32, 1, 6);
  case V4L2_CID_POWER_LINE_FREQUENCY: return v4l2_ctrl_query_fill(qc, 0, 2, 1, 2);

  default: vfe_dev_dbg("Unsupported control 0x%x\n", qc->id); return -EINVAL;
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

static struct cci_driver cci_drv = {
  .name = SENSOR_NAME,
};

static int sensor_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  struct v4l2_subdev *sd; struct sensor_info *info;

  info = kzalloc(sizeof(struct sensor_info), GFP_KERNEL);
  if (info == NULL) return -ENOMEM;

  sd = &info->sd;
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
  .driver = { .owner = THIS_MODULE, .name = SENSOR_NAME },
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

