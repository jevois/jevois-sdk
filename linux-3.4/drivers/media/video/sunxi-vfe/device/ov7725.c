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
#include "ov7725_regs.h"

MODULE_AUTHOR("JeVois Inc");
MODULE_DESCRIPTION("A low-level driver for ov7725 sensors");
MODULE_LICENSE("GPL");

#define AF_WIN_NEW_COORD

#define DEV_DBG_EN      0

#if (DEV_DBG_EN == 1)
#define vfe_dev_dbg(x, arg...) printk("[OV7725] "x, ##arg)
#else
#define vfe_dev_dbg(x, arg...)
#endif
#define vfe_dev_err(x, arg...) printk("[OV7725] "x, ##arg)
#define vfe_dev_print(x, arg...) printk("[OV7725] "x, ##arg)

#define SENSOR_WRITE(reg)                                               \
  ret = sensor_write(sd, reg, val);                                     \
  if (ret < 0) { vfe_dev_err("Error writing %u to sensor register %u in %s\n", val, reg, __func__); return ret; }

#define SENSOR_READ(reg)                                                \
  ret = sensor_read(sd, reg, &val);                                     \
  if (ret < 0) { vfe_dev_err("Error reading from sensor register %u in %s\n", reg, __func__); return ret;	}

#define MCLK              (24*1000*1000)
#define VREF_POL          V4L2_MBUS_VSYNC_ACTIVE_HIGH
#define HREF_POL          V4L2_MBUS_HSYNC_ACTIVE_LOW
#define CLK_POL           V4L2_MBUS_PCLK_SAMPLE_RISING
#define V4L2_IDENT_SENSOR 0x7725


#define CSI_STBY_ON     1
#define CSI_STBY_OFF    0

#define CSI_RST_ON      0
#define CSI_RST_OFF     1

#define CSI_PWR_ON      1
#define CSI_PWR_OFF     0

#define regval_list reg_list_a8_d8

#define SENSOR_FRAME_RATE 30

#define I2C_ADDR 0x42
#define SENSOR_NAME "ov7725"

struct sensor_format_struct;

static inline struct sensor_info *to_state(struct v4l2_subdev *sd)
{
  return container_of(sd, struct sensor_info, sd);
}

static struct regval_list sensor_default_regs[] = {
  { COM7, 0x80 },
  { PAUSE, 5 },
  { COM10, COM10_VSYNC_NEG | COM10_HREF_NEG },////| COM10_PCLK_NOBLANK },
  { COM2, OCAP_2x },

  { COM12, 0x03 },
  { HSTART, 0x22 },
  { HSIZE, 0xa4 },
  { VSTART, 0x07 },
  { VSIZE, 0xf0 },
  { HREF, 0x00 },
  { HOUTSIZE, 0xa0 },
  { VOUTSIZE, 0xf0 },
  { EXHCH, 0x00 },
  { CLKRC, 0x01 },
  { TGT_B, 0x7f },
  { FIXGAIN, 0x09 },
  { AWB_CTRL0, 0xe0 },
  { DSP_CTRL1, 0xff },
  { DSP_CTRL2, 0x20 },
  { DSP_CTRL3, 0x00 },
  { DSP_CTRL4, 0x48 },
  { COM8, 0xf0 },
  { COM4, 0x41 },
  { COM6, 0xc5 },
  { COM9, 0x41 },
  { BDBASE, 0x7f },
  { DBSTEP, 0x03 },
  { AEW, 0x40 },
  { AEB, 0x30 },
  { VPT, 0xa1 },
  { EXHCL, 0x00 },
  { AWB_CTRL3, 0xaa },

  { COM8, 0xff },
  { EDGE_TRSHLD, 0x05 },
  { DNSOFF, 0x01 },
  { EDGE_UPPER, 0x03 },
  { EDGE_LOWER, 0x00 },
  
  { MTX1, 0xb0 },
  { MTX2, 0x9d },
  { MTX3, 0x13 },
  { MTX4, 0x16 },
  { MTX5, 0x7b },
  { MTX6, 0x91 },
  { MTX_CTRL, 0x1e },
  
  { BRIGHT, 0x08 },
  { CNTRST, 0x20 },
  { UVAD_J0, 0x81 },
  { SDE, 0x06 },

  { GAM1, 0x0c },
  { GAM2, 0x16 },
  { GAM3, 0x2a },
  { GAM4, 0x4e },
  { GAM5, 0x61 },
  { GAM6, 0x6f },
  { GAM7, 0x7b },
  { GAM8, 0x86 },
  { GAM9, 0x8e },
  { GAM10, 0x97 },
  { GAM11, 0xa4 },
  { GAM12, 0xaf },
  { GAM13, 0xc5 },
  { GAM14, 0xd7 },
  { GAM15, 0xe8 },
  { SLOP, 0x20 },

  { DM_LNL, 0x00 },
  { BDBASE, 0x7f },
  { DBSTEP, 0x03 },

  { LC_RADI, 0x10 },
  { LC_COEF, 0x10 },
  { LC_COEFB, 0x14 },
  { LC_COEFR, 0x17 },
  { LC_CTR, 0x05 },
  { COM5, 0x65 },
};


static struct regval_list sensor_vga_regs[] = {
};

static struct regval_list sensor_qvga_regs[] = {
};

static struct regval_list sensor_qqvga_regs[] = {
};

static struct regval_list sensor_cif_regs[] = {
};

static struct regval_list sensor_qcif_regs[] = {
};

static struct regval_list sensor_qqcif_regs[] = {
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
    if (regs->addr == PAUSE) msleep(regs->data);
    else if (sensor_write(sd, regs->addr, regs->data)) return -EINVAL;
    ++i; ++regs;
  }
  return 0;
}

static int sensor_g_autogain(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_READ(COM8);
  if (val & AGC_ON) *value = 1; else *value = 0;
  info->autogain = *value;
  return 0;
}

static int sensor_s_autogain(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_READ(COM8);
  if (value) val |= AGC_ON; else val &= ~AGC_ON;
  SENSOR_WRITE(COM8);
  info->autogain = value;
  return 0;
}

static int sensor_g_gain(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; unsigned char val;

  SENSOR_READ(GAIN);

  *value = (16 + (val & 0x0f));
  if (val & 0x10) *value <<= 1;
  if (val & 0x20) *value <<= 1;
  if (val & 0x40) *value <<= 1;
  if (val & 0x80) *value <<= 1;
  
  return 0;
}

static int sensor_s_gain(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val;

  if (value > 255) value = 255;

  val = 0;
  if (value > 31) { val |= 0x10; value = value >> 1; }
  if (value > 31) { val |= 0x20; value = value >> 1; }
  if (value > 31) { val |= 0x40; value = value >> 1; }
  if (value > 31) { val |= 0x80; value = value >> 1; }
  if (value > 16) { val |= ((value - 16) & 0x0f); }

  SENSOR_WRITE(GAIN);

  return 0;
}

static int sensor_g_autoexp(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_READ(COM8);
  if (val & AEC_ON) *value = V4L2_EXPOSURE_AUTO; else *value = V4L2_EXPOSURE_MANUAL;
  info->autoexp = *value;
  return 0;
}

static int sensor_s_autoexp(struct v4l2_subdev *sd, enum v4l2_exposure_auto_type value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  
  SENSOR_READ(COM8);
  switch (value)
  {
  case V4L2_EXPOSURE_AUTO: val |= (AEC_ON | AGC_ON); break;
  case V4L2_EXPOSURE_MANUAL: val &= ~(AEC_ON | AGC_ON); break;
  default: return -EINVAL;
  }
  SENSOR_WRITE(COM8);

  info->autoexp = value;
  return 0;
}

static int sensor_g_exposure_absolute(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val; unsigned int exposure;

  SENSOR_READ(AEC);
  exposure = val;
  SENSOR_READ(AECH);
  exposure |= (val << 8);
  info->exp = (exposure * info->trow + 50) / 100;
  *value = info->exp;
  return 0;
}

static int sensor_s_exposure_absolute(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val; unsigned int exposure;

  exposure = (value * 100 + 50) / info->trow;
  if (exposure < 1) exposure = 1; else if (exposure > info->exp_max_lines) exposure = info->exp_max_lines;
  
  vfe_dev_dbg("OV7725 set exp: value=%d expo=%u em=%d tr=%d\n", value, exposure, info->exp_max_lines, info->trow);

  val = (exposure >> 8); SENSOR_WRITE(AECH);
  val = (exposure & 0xff); SENSOR_WRITE(AEC);

  info->exp = (exposure * info->trow + 50) / 100;
  
  return 0;
}

static int sensor_g_autowb(struct v4l2_subdev *sd, int *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_READ(COM8);
  if (val & AWB_ON) { *value = 1; info->autowb = 1; } else { *value = 0; info->autowb = 0; }
  return 0;
}

static int sensor_s_autowb(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;
  
  SENSOR_READ(COM8);
  if (value) val |= AWB_ON; else val &= ~AWB_ON;
  SENSOR_WRITE(COM8);
  if (value) info->autowb = 1; else info->autowb = 0;
  return 0;
}

static int sensor_g_wb_red(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned char val;
  SENSOR_READ(RED); *value = (unsigned int)(val);
  return 0;
}

static int sensor_g_wb_blue(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned char val;
  SENSOR_READ(BLUE); *value = (unsigned int)(val);
  return 0;
}

static int sensor_s_wb_red(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val = value & 0xff;
  SENSOR_WRITE(RED);
  return 0;
}

static int sensor_s_wb_blue(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val = value & 0xff;
  SENSOR_WRITE(BLUE);
  return 0;
}


static int sensor_g_brightness(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);

  *value = info->brightness;
  return 0;
}

static int sensor_s_brightness(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;

  SENSOR_READ(SIGN);
  if (value > 0) val &= ~(1 << 3); else val |= (1 << 3);
  SENSOR_WRITE(SIGN);
  
  switch (value)
  {
  case -4: val = 0x38; SENSOR_WRITE(BRIGHT); break;
  case -3: val = 0x28; SENSOR_WRITE(BRIGHT); break;
  case -2: val = 0x18; SENSOR_WRITE(BRIGHT); break;
  case -1: val = 0x08; SENSOR_WRITE(BRIGHT); break;
  case  0: val = 0x08; SENSOR_WRITE(BRIGHT); break;
  case  1: val = 0x18; SENSOR_WRITE(BRIGHT); break;
  case  2: val = 0x28; SENSOR_WRITE(BRIGHT); break;
  case  3: val = 0x38; SENSOR_WRITE(BRIGHT); break;
  case  4: val = 0x48; SENSOR_WRITE(BRIGHT); break;
  default: return -EINVAL;
  }
    
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
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;
  
  switch (value)
  {
  case 0: val = 0x10; SENSOR_WRITE(CNTRST); break;
  case 1: val = 0x14; SENSOR_WRITE(CNTRST); break;
  case 2: val = 0x18; SENSOR_WRITE(CNTRST); break;
  case 3: val = 0x1c; SENSOR_WRITE(CNTRST); break;
  case 4: val = 0x20; SENSOR_WRITE(CNTRST); break;
  case 5: val = 0x24; SENSOR_WRITE(CNTRST); break;
  case 6: val = 0x28; SENSOR_WRITE(CNTRST); break;
  case 7: val = 0x2c; SENSOR_WRITE(CNTRST); break;
  case 8: val = 0x30; SENSOR_WRITE(CNTRST); break;
  default: return -EINVAL;
  }
  
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
  /*int ret;*/ struct sensor_info *info = to_state(sd); /*unsigned char val;*/

  /* fixme this chip has auto sharpness

  
  int ret; struct sensor_info *info = to_state(sd); unsigned char val;
  int v = value;
  
  SENSOR_READ(REG_COM14);

  if (v) val |= COM14_EDGE_EN; else val &= ~COM14_EDGE_EN;

  v -= 1; if (v > 0x0f) { val |= COM14_EEF_X2; v >>= 1; } else { val &= ~COM14_EEF_X2; }
  SENSOR_WRITE(REG_COM14);

  SENSOR_READ(REG_EDGE);
  val &= ~EDGE_FACTOR_MASK; val |= ((unsigned char)v & 0x0f);
  SENSOR_WRITE(REG_EDGE);
  */
  
  info->sharpness = value;

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
  int ret;
  struct sensor_info *info = to_state(sd); unsigned char val;

  switch (value)
  {
  case 0: val = 0x00; SENSOR_WRITE(USAT); SENSOR_WRITE(VSAT); break;
  case 1: val = 0x10; SENSOR_WRITE(USAT); SENSOR_WRITE(VSAT); break;
  case 2: val = 0x20; SENSOR_WRITE(USAT); SENSOR_WRITE(VSAT); break;
  case 3: val = 0x30; SENSOR_WRITE(USAT); SENSOR_WRITE(VSAT); break;
  case 4: val = 0x40; SENSOR_WRITE(USAT); SENSOR_WRITE(VSAT); break;
  case 5: val = 0x50; SENSOR_WRITE(USAT); SENSOR_WRITE(VSAT); break;
  case 6: val = 0x60; SENSOR_WRITE(USAT); SENSOR_WRITE(VSAT); break;
  case 7: val = 0x70; SENSOR_WRITE(USAT); SENSOR_WRITE(VSAT); break;
  case 8: val = 0x80; SENSOR_WRITE(USAT); SENSOR_WRITE(VSAT); break;
  default: return -EINVAL;
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
    SENSOR_READ(COM8); val &= ~BNDF_ON_OFF; SENSOR_WRITE(COM8);
    break;

  case V4L2_CID_POWER_LINE_FREQUENCY_50HZ:
    SENSOR_READ(COM8); val |= BNDF_ON_OFF; SENSOR_WRITE(COM8);
    val = ratex100 * info->exp_max_lines / 10000;
    SENSOR_WRITE(BDBASE);
    val = 0x07;
    SENSOR_WRITE(DBSTEP);
    break;
    
  case V4L2_CID_POWER_LINE_FREQUENCY_60HZ:
    SENSOR_READ(COM8); val |= BNDF_ON_OFF; SENSOR_WRITE(COM8);
    val = ratex100 * info->exp_max_lines / 12000;
    SENSOR_WRITE(BDBASE);
    val = 0x07;
    SENSOR_WRITE(DBSTEP);
   break;
    
  default: return -EINVAL;
  }

  info->band_filter = value;
  return 0;
}

static int sensor_g_hmirror(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned char val;

  SENSOR_READ(COM3);
  *value = (val & HFLIP_IMG) ? 0 : 1;
  return 0;
}

static int sensor_g_vflip(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned char val;

  SENSOR_READ(COM3);
  *value = (val & VFLIP_IMG) ? 0 : 1;
  return 0;
}

static int sensor_s_hmirror(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val;
  
  SENSOR_READ(COM3);
  switch (value)
  {
  case 0: val |= HFLIP_IMG; break;
  case 1: val &= ~HFLIP_IMG; break;
  default: return -EINVAL;
  }
  SENSOR_WRITE(COM3);
  return 0;
}

static int sensor_s_vflip(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned char val = value & 0xff;

  SENSOR_READ(COM3);
  switch (value)
  {
  case 0: val |= VFLIP_IMG; break;
  case 1: val &= ~VFLIP_IMG; break;
  default: return -EINVAL;
  }
  SENSOR_WRITE(COM3);
  return 0;
}

static struct regval_list sensor_wbp_auto_regs[] =
{ { COM8, 0xff }, { COM5, 0x65 }, { ADVFL, 0x00 }, { ADVFH, 0x00 }, };

static struct regval_list sensor_wbp_sunny_regs[] =
{ { COM8, 0xfd }, { BLUE, 0x5a }, { RED, 0x5c }, { COM5, 0x65 }, { ADVFL, 0x00 }, { ADVFH, 0x00 }, };

static struct regval_list sensor_wbp_cloudy_regs[] =
{ { COM8, 0xfd }, { BLUE, 0x58 }, { RED, 0x60 }, { COM5, 0x65 }, { ADVFL, 0x00 }, { ADVFH, 0x00 }, };

static struct regval_list sensor_wbp_office_regs[] =
{ { COM8, 0xfd }, { BLUE, 0x84 }, { RED, 0x4c }, { COM5, 0x65 }, { ADVFL, 0x00 }, { ADVFH, 0x00 }, };

static struct regval_list sensor_wbp_home_regs[] =
{ { COM8, 0xfd }, { BLUE, 0x96 }, { RED, 0x40 }, { COM5, 0x65 }, { ADVFL, 0x00 }, { ADVFH, 0x00 }, };

static struct regval_list sensor_wbp_night_regs[] =
{ { COM8, 0xff }, { COM5, 0xe5 }, { FIXGAIN, 0xff }, };

static struct regval_list sensor_wbp_horizon_regs[] =
{ { COM8, 0xfd }, { BLUE, 0x96 }, { RED, 0x40 }, { COM5, 0x65 }, { ADVFL, 0x00 }, { ADVFH, 0x00 }, };

static struct regval_list sensor_wbp_flash_regs[] =
{ { COM8, 0xfd }, { BLUE, 0x84 }, { RED, 0x4c }, { COM5, 0x65 }, { ADVFL, 0x00 }, { ADVFH, 0x00 }, };

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
    
    vfe_gpio_write(sd, PWDN, CSI_STBY_ON);
    
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
    usleep_range(10000, 12000);
    
    vfe_gpio_write(sd, PWDN, CSI_STBY_ON);
    vfe_dev_dbg("CSI_SUBDEV_PWR_ON - standby - PWDW=%d\n", CSI_STBY_ON);

    vfe_gpio_write(sd, RESET, CSI_RST_ON);
    vfe_dev_dbg("CSI_SUBDEV_PWR_ON - reset\n");
    msleep(1);
    
    vfe_set_mclk_freq(sd, MCLK);
    vfe_set_mclk(sd, ON);
    vfe_dev_dbg("CSI_SUBDEV_PWR_ON - clock\n");
    msleep(10);
    
    vfe_gpio_write(sd, POWER_EN, CSI_PWR_ON);
    vfe_set_pmu_channel(sd, IOVDD, ON);
    vfe_set_pmu_channel(sd, AVDD, ON);
    vfe_set_pmu_channel(sd, DVDD, ON);
    vfe_set_pmu_channel(sd, AFVDD, ON);
    vfe_dev_dbg("CSI_SUBDEV_PWR_ON - power\n");
    msleep(20);
    
    vfe_gpio_write(sd, PWDN, CSI_STBY_OFF);
    vfe_dev_dbg("CSI_SUBDEV_PWR_ON - standby off - PWDW=%d\n", CSI_STBY_OFF);
    msleep(10);
    
    vfe_gpio_write(sd, RESET, CSI_RST_OFF);
    vfe_dev_dbg("CSI_SUBDEV_PWR_ON - reset off\n");
    msleep(30);

    cci_unlock(sd);
    vfe_dev_dbg("CSI_SUBDEV_PWR_ON - ready\n");
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

  SENSOR_READ(PID); if (val != 0x77) return -ENODEV;
  SENSOR_READ(VER); if (val != 0x21) return -ENODEV;

  vfe_dev_print("Detected Omnivision OV7725 camera\n");

  return 0;
}

static int sensor_init(struct v4l2_subdev *sd, u32 val)
{
  int ret;
  struct sensor_info *info = to_state(sd);
  
  vfe_dev_dbg("sensor_init 0x%x\n", val);
  
  ret = sensor_detect(sd);
  if (ret) { vfe_dev_err("Sensor chip found is not a target chip for this driver\n"); return ret; }
  vfe_dev_dbg("sensor_detect ok\n");

  vfe_get_standby_mode(sd, &info->stby_mode);

  if ((info->stby_mode == HW_STBY || info->stby_mode == SW_STBY) && info->init_first_flag == 0)
  { vfe_dev_print("stby_mode and init_first_flag = 0\n"); return 0; }
  
  info->focus_status = 0;
  info->low_speed = 0;
  info->width = 0;
  info->height = 0;
  info->brightness = 0;
  info->contrast = 4;
  info->saturation = 4;
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
  /* VGA */
  {
    .width      = VGA_WIDTH,
    .height     = VGA_HEIGHT,
    .hoffset    = 140,
    .voffset    = 14,
    .intg_max   = 510,
    .regs       = sensor_vga_regs,
    .regs_size  = ARRAY_SIZE(sensor_vga_regs),
    .set_size   = NULL,
  },
  /* CIF */
  {
    .width      = CIF_WIDTH,
    .height     = CIF_HEIGHT,
    .hoffset    = 316,
    .voffset    = 8,
    .intg_max   = 382,
    .regs       = sensor_cif_regs,
    .regs_size  = ARRAY_SIZE(sensor_cif_regs),
    .set_size   = NULL,
  },
  /* QVGA */
  {
    .width      = QVGA_WIDTH,
    .height     = QVGA_HEIGHT,
    .hoffset    = 252,
    .voffset    = 8,
    .intg_max   = 278,
    .regs       = sensor_qvga_regs,
    .regs_size  = ARRAY_SIZE(sensor_qvga_regs),
    .set_size   = NULL,
  },
  /* QCIF */
  {
    .width      = QCIF_WIDTH,
    .height     = QCIF_HEIGHT,
    .hoffset    = 316,
    .voffset    = 8,
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
    .voffset    = 8,
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
    .voffset    = 8,
    .intg_max   = 190,
    .regs       = sensor_qqcif_regs,
    .regs_size  = ARRAY_SIZE(sensor_qqcif_regs),
    .set_size   = NULL,
  },
};

#define N_WIN_SIZES (ARRAY_SIZE(sensor_win_sizes))

static struct regval_list rgb565_regs[] = {
  { DSP_CTRL3, 0x0 }, { DSP_CTRL4, DSP_OFMT_RGB }
};

static struct regval_list yuyv_regs[] = {
  { DSP_CTRL3, 0x0 }, { DSP_CTRL4, DSP_OFMT_YUV }
};

static struct regval_list bayer_regs[] = {
  { DSP_CTRL3, 0x0 }, { DSP_CTRL4, DSP_OFMT_RAW10 }
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
  
  for (wsize = sensor_win_sizes; wsize < sensor_win_sizes + N_WIN_SIZES; ++wsize)
    if (fmt->width >= wsize->width && fmt->height >= wsize->height) break;
  
  if (wsize >= sensor_win_sizes + N_WIN_SIZES) --wsize;
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
  cfg->flags = V4L2_MBUS_MASTER | VREF_POL | HREF_POL | CLK_POL;

  return 0;
}

static int sensor_s_fmt(struct v4l2_subdev *sd, struct v4l2_mbus_framefmt *fmt)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned char val; struct sensor_win_size * wsize; 
  unsigned int com2, href;

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

  SENSOR_READ(COM7);
  if (wsize->width <= 320) val |= SLCT_QVGA; else val &= ~SLCT_QVGA;

  switch(fmt->code)
  {
  case V4L2_MBUS_FMT_YUYV8_2X8:
    val &= ~(SENSOR_RAW | OFMT_RGB | OFMT_P_BRAW | OFMT_BRAW); val |= OFMT_YUV; SENSOR_WRITE(COM7);
    sensor_write_array(sd, yuyv_regs, ARRAY_SIZE(yuyv_regs));
    SENSOR_READ(COM3); val |= (SWAP_YUV | SWAP_RGB); SENSOR_WRITE(COM3);
    break;
    
  case V4L2_MBUS_FMT_RGB565_2X8_LE:
    val &= ~(SENSOR_RAW | OFMT_YUV | OFMT_P_BRAW | OFMT_BRAW); val |= (OFMT_RGB | FMT_RGB565); SENSOR_WRITE(COM7);
    sensor_write_array(sd, rgb565_regs, ARRAY_SIZE(rgb565_regs));
    SENSOR_READ(COM3); val |= (SWAP_YUV | SWAP_RGB); SENSOR_WRITE(COM3);
    break;

  case V4L2_MBUS_FMT_SBGGR8_1X8:
    val &= ~(OFMT_YUV | OFMT_P_BRAW | OFMT_RGB); val |= (SENSOR_RAW | OFMT_BRAW); SENSOR_WRITE(COM7);
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
  if (ret < 0) vfe_dev_dbg("sensor_s_fmt: failed to set banding filter -- IGNORED\n");

  SENSOR_READ(HREF); href = val & 0x80;
  SENSOR_READ(COM2); com2 = val;
  
  val |= SOFT_SLEEP_MODE; SENSOR_WRITE(COM2);
  msleep(20);
  
  val = wsize->hoffset >> 2; SENSOR_WRITE(HSTART);
  val = wsize->width >> 2; SENSOR_WRITE(HSIZE); SENSOR_WRITE(HOUTSIZE);

  val = href |
    ((wsize->voffset & 1) << HREF_VSTART_SHIFT) |
    ((wsize->hoffset & 3) << HREF_HSTART_SHIFT) |
    ((wsize->height & 1) << HREF_VSIZE_SHIFT) |
    ((wsize->width & 3) << HREF_HSIZE_SHIFT);
  SENSOR_WRITE(HREF);
  
  val = wsize->voffset >> 1; SENSOR_WRITE(VSTART);
  val = wsize->height >> 1; SENSOR_WRITE(VSIZE); SENSOR_WRITE(VOUTSIZE);

  SENSOR_READ(EXHCH);
  val &= 0x7;
  val |= ((wsize->height & 1) << EXHCH_VSIZE_SHIFT) | ((wsize->width & 3) << EXHCH_HSIZE_SHIFT);
  SENSOR_WRITE(EXHCH);

  val = com2; SENSOR_WRITE(COM2);
  
  vfe_dev_dbg("sensor_s_fmt: hstart=0x%x hsize=0x%x vstart=0x%x vsize=0x%x\n", wsize->hoffset, wsize->width,
	      wsize->voffset, wsize->height);
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
  case CIF_WIDTH: prescaler = 2; dpix = 1040; dlin = 384; break;
  case VGA_WIDTH: prescaler = 2; dpix = 1600; dlin = 500; break;
  default: return -EINVAL;
  }

  if (ratex100 > 60*100) return -EINVAL;
  else if (ratex100 > 30*100) { basex100 = 6000; if (prescaler < 2) return -EINVAL; else prescaler >>= 1; }
  else if (ratex100 > 15*100) { basex100 = 3000; if (prescaler < 1) return -EINVAL; }
  else if (ratex100 > 750)    { basex100 = 1500; prescaler <<= 1; }
  else if (ratex100 > 375)    { basex100 = 750; prescaler <<= 2; }
  else if (ratex100 >= 10)    { basex100 = 375; prescaler <<= 3; }
  else return -EINVAL;

  
  vfe_dev_dbg("JEVOIS prescaler is %u\n", prescaler);
  val = prescaler - 1;
  SENSOR_WRITE(CLKRC); 

  fint = MCLK * ((val >> 7) + 1) / ((2 * ((val & 0x3f) + 1)));
  if (info->mbus_code == V4L2_MBUS_FMT_SBGGR8_1X8) info->trow = (748 * 1000000UL) / fint;
  else info->trow = (2 * 748 * 1000000UL) / fint;

  usdelta = 100UL * 1000000UL / ratex100 - 100UL * 1000000UL / basex100;
  tmpll = (unsigned long long)(usdelta); tmpll *= dlin * basex100; do_div(tmpll, 100000000ULL);
  num_dummy_lines = (unsigned long)tmpll;
  
  val = (num_dummy_lines >> 8) & 0xff; SENSOR_WRITE(DM_LNH);
  val = num_dummy_lines & 0xff; SENSOR_WRITE(DM_LNL);

  val = 0; SENSOR_WRITE(EXHCL); SENSOR_READ(EXHCH); val &= ~(0xf0); SENSOR_WRITE(EXHCH);
  
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
  case V4L2_CID_GAIN: return v4l2_ctrl_query_fill(qc, 0, 255, 1, 16);
  case V4L2_CID_AUTOGAIN: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
  case V4L2_CID_EXPOSURE_AUTO: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_DO_WHITE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 9, 1, 0);
  case V4L2_CID_AUTO_WHITE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);

  case V4L2_CID_EXPOSURE_ABSOLUTE: return v4l2_ctrl_query_fill(qc, 1, 1259, 1, 1259);
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

