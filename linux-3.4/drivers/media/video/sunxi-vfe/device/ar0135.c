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
#include "../jevois_data.h"

MODULE_AUTHOR("JeVois Inc");
MODULE_DESCRIPTION("A low-level driver for ar0135 sensors");
MODULE_LICENSE("GPL");

#define AF_WIN_NEW_COORD

#define DEV_DBG_EN      1

#if (DEV_DBG_EN == 1)
#define vfe_dev_dbg(x, arg...) printk("[AR0135] "x, ##arg)
#else
#define vfe_dev_dbg(x, arg...)
#endif
#define vfe_dev_err(x, arg...) printk("[AR0135] "x, ##arg)
#define vfe_dev_print(x, arg...) printk("[AR0135] "x, ##arg)

#define SENSOR_WRITE(reg)                                               \
  ret = sensor_write(sd, reg, val);                                     \
  if (ret < 0) { vfe_dev_err("Error writing %u to sensor register %u in %s\n", val, reg, __func__); return ret; }

#define SENSOR_READ(reg)                                                \
  ret = sensor_read(sd, reg, &val);                                     \
  if (ret < 0) { vfe_dev_err("Error reading from sensor register %u in %s\n", reg, __func__); return ret;	}

#include "icm20948.c"

#define MCLK              (24*1000*1000)
#define VREF_POL          V4L2_MBUS_VSYNC_ACTIVE_HIGH
#define HREF_POL          V4L2_MBUS_HSYNC_ACTIVE_HIGH
#define CLK_POL           V4L2_MBUS_PCLK_SAMPLE_RISING
#define V4L2_IDENT_SENSOR 0x0135


#define CSI_STBY_ON     1
#define CSI_STBY_OFF    0

#define CSI_RST_ON      0
#define CSI_RST_OFF     1

#define CSI_PWR_ON      1
#define CSI_PWR_OFF     0

#define regval_list reg_list_a16_d16

#define SENSOR_FRAME_RATE 30

#define I2C_ADDR 0x20
#define SENSOR_NAME "ar0135"

struct sensor_format_struct;

static inline struct sensor_info *to_state(struct v4l2_subdev *sd)
{
  return container_of(sd, struct sensor_info, sd);
}

static struct regval_list sensor_default_regs[] = {
  { 0x0B,  0x200  },
  { 0x3028, 0x0010 }, 	
  
  { 0x302E, 2 }, 		   
  { 0x3030, 44 }, 	    
  { 0x302C, 1 }, 		   
  { 0x302A, 8 }, 		   
  
  { 0x3032, 0x0000 }, 	
  { 0x30B0, 0x0080 }, 	
  
  { 0x301A, 0x10DC },  	
  { 0x300C, 1388 },  	
  
  { 0x3002, 0x0000 },  	
  { 0x3004, 0x0001 },  	
  { 0x3006, 959 },  	
  { 0x3008, 1282 },  	
  { 0x300A, 0x03DE },  	
  { 0x3012, 0x0700 },  	
  { 0x3014, 0x00C0 },  	
  { 0x30A6, 0x0001 },  	

  { 0x3040, 0x0000 },  	
  { 0x3064, 0x1982 },  	
  { 0x31C6, 0x8000 },  	

  { 0x3056, 0x20 },
  { 0x3058, 0x20 },
  { 0x305a, 0x20 },
  { 0x305c, 0x50 },

  { 0x3100, 3 },
  { 0x3102, 0x700 },
  { 0x311c, 0x02a0 },
  { 0x311e, 0x0001 },

};

static struct reg_list_a8_d8 imu_default_regs[] = {
  { 0x03, 0x60 },
  { 0x06, 0x01 },
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


static int sensor_read(struct v4l2_subdev *sd, unsigned short reg, unsigned short *value)
{
  int ret = 0; int cnt = 0;
  
  ret = cci_read_a16_d16(sd,reg,value);
  while (ret != 0 && cnt < 3)
  {
  	ret = cci_read_a16_d16(sd, reg, value);
  	++cnt;
  }
  
  if (cnt) vfe_dev_dbg("sensor read retry=%d\n",cnt);
  
  return ret;
}

static int sensor_write(struct v4l2_subdev *sd, unsigned short reg, unsigned short value)
{
  int ret = 0; int cnt = 0;
 
  ret = cci_write_a16_d16(sd, reg, value);
  while (ret != 0 && cnt < 2)
  {
  	ret = cci_write_a16_d16(sd, reg, value);
  	cnt++;
  }
  if (cnt) vfe_dev_dbg("sensor write retry=%d\n",cnt);
  
  return ret;
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

static int sensor_g_autoexp(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned short val;

  SENSOR_READ(0x3100);
  if (val & 0x0001) *value = V4L2_EXPOSURE_AUTO; else *value = V4L2_EXPOSURE_MANUAL;
  info->autoexp = *value;

  return 0;
}

static int sensor_s_autoexp(struct v4l2_subdev *sd, enum v4l2_exposure_auto_type value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned short val;

  
  SENSOR_READ(0x3100);
  switch (value)
  {
  case V4L2_EXPOSURE_AUTO: val |= 3; break;
  case V4L2_EXPOSURE_MANUAL: val &= ~3; break;
  default: return -EINVAL;
  }
  SENSOR_WRITE(0x3100);

  info->autoexp = value;

  return 0;
}

static int sensor_g_exposure_absolute(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned short val;

  SENSOR_READ(0x3012);
  info->exp = val;
  *value = val;

  return 0;
}

static int sensor_s_exposure_absolute(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned short val;

  val = value & 0xffff; SENSOR_WRITE(0x3012);
  info->exp = val;

  return 0;
}

static int sensor_g_autogain(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned short val;

  SENSOR_READ(0x3100);
  if (val & 0x0001) *value = 1; else *value = 0;
  info->autoexp = *value;
  return 0;
}

static int sensor_s_autogain(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned short val;

  
  SENSOR_READ(0x3100);
  switch (value)
  {
  case 1: val |= 3; break;
  case 0: val &= ~3; break;
  default: return -EINVAL;
  }
  SENSOR_WRITE(0x3100);

  info->autoexp = value;

  return 0;
}

static int sensor_g_gain(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; unsigned short val;

  SENSOR_READ(0x30b0);
  *value = (val & 0x0030) >> 4;
  return 0;
}

static int sensor_s_gain(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned short val;

  if (value > 3) value = 3; else if (value < 0) value = 0;
  SENSOR_READ(0x30b0);
  val &= ~0x30;
  val |= (value << 4);
  SENSOR_WRITE(0x30b0);

  return 0;
}

static int sensor_g_wb_red(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned short val;
  SENSOR_READ(0x305a); *value = (unsigned int)(val);
  return 0;
}

static int sensor_g_wb_blue(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned short val;
  SENSOR_READ(0x3058); *value = (unsigned int)(val);
  return 0;
}

static int sensor_s_wb_red(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned short val = value & 0xffff;
  SENSOR_WRITE(0x305a);
  return 0;
}

static int sensor_s_wb_blue(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned short val = value & 0xff;
  SENSOR_WRITE(0x3058);
  return 0;
}

static int sensor_g_brightness(struct v4l2_subdev *sd, __s32 *value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned short val;

  SENSOR_READ(0x3102);
  
  info->brightness = val;
  *value = val;
  return 0;
}

static int sensor_s_brightness(struct v4l2_subdev *sd, int value)
{
  int ret; struct sensor_info *info = to_state(sd); unsigned short val;
  
  val = value & 0xffff; SENSOR_WRITE(0x3102);

  info->brightness = value;
  return 0;
}

static int sensor_g_hmirror(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned short val;

  SENSOR_READ(0x3040);
  *value = (val & (1 << 14)) ? 1 : 0;

  return 0;
}

static int sensor_g_vflip(struct v4l2_subdev *sd, int *value)
{
  int ret; unsigned short val;

  SENSOR_READ(0x3040);
  *value = (val & (1 << 15)) ? 1 : 0;

  return 0;
}

static int sensor_s_hmirror(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned short val;
  
  SENSOR_READ(0x3040);
  switch (value)
  {
  case 1: val |= (1 << 14); break;
  case 0: val &= ~(1 << 14); break;
  default: return -EINVAL;
  }
  SENSOR_WRITE(0x3040);

  return 0;
}

static int sensor_s_vflip(struct v4l2_subdev *sd, int value)
{
  int ret; unsigned short val = value & 0xff;

  SENSOR_READ(0x3040);
  switch (value)
  {
  case 1: val |= (1 << 15); break;
  case 0: val &= ~(1 << 15); break;
  default: return -EINVAL;
  }
  SENSOR_WRITE(0x3040);

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

static int imu_detect(struct v4l2_subdev *sd)
{
  int ret; unsigned char imuval;

  IMU_READ(0x00);

  if (imuval != 0xEA) { vfe_dev_print("No IMU detected.\n"); return 1; }

  vfe_dev_print("IMU ICM-20948 ready\n");
  return 0;
}
  
static int sensor_detect(struct v4l2_subdev *sd)
{
  int ret; unsigned short val; int tmp;
  struct sensor_info *info = to_state(sd);

  vfe_dev_print("sensor detect start\n");

  SENSOR_READ(0x3000);

  if (val == 0x754) vfe_dev_print("Detected Aptina AR0135 camera\n");
  else if (val == 0x2406) vfe_dev_print("Detected Aptina AR0134 camera\n");
  else return -ENODEV;

  SENSOR_READ(0x301a); tmp = val; val |= (1 << 5); SENSOR_WRITE(0x301a);
  
  SENSOR_READ(0x31fa);
  if (val & (0x4 << 8)) { vfe_dev_print("Sensor is COLOR\n"); info->jevois = JEVOIS_SENSOR_COLOR; }
  else { vfe_dev_print("Sensor is MONO\n"); info->jevois = JEVOIS_SENSOR_MONO; }

  val = tmp; SENSOR_WRITE(0x301a);

  if (imu_detect(sd)) vfe_dev_print("Sensor has no IMU\n");
  else info->jevois |= JEVOIS_SENSOR_ICM20948;
  
  return 0;
}

static int sensor_init(struct v4l2_subdev *sd, u32 val)
{
  int ret; struct sensor_info *info = to_state(sd);
  unsigned char imuval;
  
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
  info->gain = 0;
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

  if (info->stby_mode == 0) info->init_first_flag = 0;
  
  info->preview_first_flag = 1;

  if (info->jevois & JEVOIS_SENSOR_ICM20948)
  {
    vfe_dev_dbg("Writing IMU default regs\n");
    ret = imu_write_array(sd, imu_default_regs, ARRAY_SIZE(imu_default_regs));
    if (ret < 0) { vfe_dev_err("write imu_default_regs error\n"); return ret; }

    ret = imu_load_dmp(sd);
    if (ret) { vfe_dev_err("FAIL to load IMU DMP code\n"); return ret; }

    imuval = 0x80 | 0x40;
    IMU_WRITE(0x03);
  }
  
  return 0;
}

static long sensor_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
  int ret; unsigned short val; unsigned char imuval;
  unsigned short * data = (unsigned short *)arg;
  struct sensor_info *info = to_state(sd);

  switch (cmd)
  {
  case _IOW('V', 192, int): val = data[1]; SENSOR_WRITE(data[0]); break;
  case _IOWR('V', 193, int): SENSOR_READ(data[0]); data[1] = val; break;
  case _IOW('V', 194, int): imuval = data[1] & 0xff; IMU_WRITE(data[0] & 0xff); break;
  case _IOWR('V', 195, int): IMU_READ(data[0] & 0xff); data[1] = imuval; break;
  case _IOW('V', 196, struct jevois_data):
  {
    struct jevois_data * kdata = (struct jevois_data *)arg;
    ret = imu_write_a8_d8_array(sd, kdata->addr, kdata->size, kdata->data);
  }
  break;

  case _IOWR('V', 197, struct jevois_data):
  {
    struct jevois_data * kdata = (struct jevois_data *)arg;
    ret = imu_read_a8_d8_array(sd, kdata->addr, kdata->size, kdata->data);
  }
  break;

  case _IOWR('V', 198, int): *((unsigned int *)arg) = info->jevois; break;
    
  default:
    return -EINVAL;
  }

  return 0;
}

static struct sensor_win_size sensor_win_sizes[] = {
  {
    .width      = 1280,
    .height     = 960,
    .hoffset    = 1,
    .voffset    = 0,
    .intg_max   = 510,
    .regs       = sensor_vga_regs,
    .regs_size  = ARRAY_SIZE(sensor_vga_regs),
    .set_size   = NULL,
  },
  {
    .width      = 1280,
    .height     = 720,
    .hoffset    = 1,
    .voffset    = 120,
    .intg_max   = 510,
    .regs       = sensor_vga_regs,
    .regs_size  = ARRAY_SIZE(sensor_vga_regs),
    .set_size   = NULL,
  },
  {
    .width      = VGA_WIDTH,
    .height     = VGA_HEIGHT,
    .hoffset    = 1,
    .voffset    = 0,
    .intg_max   = 510,
    .regs       = sensor_vga_regs,
    .regs_size  = ARRAY_SIZE(sensor_vga_regs),
    .set_size   = NULL,
  },
  {
    .width      = VGA_WIDTH,
    .height     = 360,
    .hoffset    = 1,
    .voffset    = 120,
    .intg_max   = 510,
    .regs       = sensor_vga_regs,
    .regs_size  = ARRAY_SIZE(sensor_vga_regs),
    .set_size   = NULL,
  },
  {
    .width      = QVGA_WIDTH,
    .height     = QVGA_HEIGHT,
    .hoffset    = 321,
    .voffset    = 240,
    .intg_max   = 278,
    .regs       = sensor_qvga_regs,
    .regs_size  = ARRAY_SIZE(sensor_qvga_regs),
    .set_size   = NULL,
  },
  {
    .width      = QVGA_WIDTH,
    .height     = 180,
    .hoffset    = 321,
    .voffset    = 300,
    .intg_max   = 278,
    .regs       = sensor_qvga_regs,
    .regs_size  = ARRAY_SIZE(sensor_qvga_regs),
    .set_size   = NULL,
  },
  {
    .width      = QQVGA_WIDTH,
    .height     = QQVGA_HEIGHT,
    .hoffset    = 561,
    .voffset    = 360,
    .intg_max   = 278,
    .regs       = sensor_qvga_regs,
    .regs_size  = ARRAY_SIZE(sensor_qvga_regs),
    .set_size   = NULL,
  },
  {
    .width      = QQVGA_WIDTH,
    .height     = 90,
    .hoffset    = 561,
    .voffset    = 390,
    .intg_max   = 278,
    .regs       = sensor_qvga_regs,
    .regs_size  = ARRAY_SIZE(sensor_qvga_regs),
    .set_size   = NULL,
  },
};

#define N_WIN_SIZES (ARRAY_SIZE(sensor_win_sizes))

static int sensor_enum_fmt(struct v4l2_subdev *sd, unsigned index, enum v4l2_mbus_pixelcode *code)
{
  struct sensor_info *info = to_state(sd);

  switch (index)
  {
  case 0:
    if (info->jevois & JEVOIS_SENSOR_MONO) *code = V4L2_MBUS_FMT_Y8_1X8;
    else *code = V4L2_MBUS_FMT_SBGGR8_1X8;
    return 0;
    
  default: return -EINVAL;
  }
}

static int sensor_try_fmt_internal(struct v4l2_subdev *sd,
                                   struct v4l2_mbus_framefmt *fmt,
                                   struct sensor_win_size **ret_wsize)
{
  struct sensor_win_size *wsize;
  struct sensor_info *info = to_state(sd);
 
  if (info->jevois & JEVOIS_SENSOR_MONO)
  {
    if (fmt->code != V4L2_MBUS_FMT_Y8_1X8)
    {
      return -1;
    }
  }
  else
  {
    if (fmt->code != V4L2_MBUS_FMT_SBGGR8_1X8)
    {
      return -1;
    }
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
  int ret; struct sensor_info *info = to_state(sd); unsigned short val; struct sensor_win_size * wsize; 
  int binfac = 1;

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

  if (wsize->width < 1280)
  {
    SENSOR_READ(0x3032); val |= 0x0002; SENSOR_WRITE(0x3032);

    if (info->jevois & JEVOIS_SENSOR_MONO)
    {
      SENSOR_READ(0x30b0); val |= (1 << 7); SENSOR_WRITE(0x30b0);
    }
    else
    {
      SENSOR_READ(0x306e); val |= (1 << 4); SENSOR_WRITE(0x306e);
      SENSOR_READ(0x30b0); val &= ~(1 << 7); SENSOR_WRITE(0x30b0);
    }

    binfac = 2;
  }
  else
  {
    SENSOR_READ(0x3032); val &= ~0x0002; SENSOR_WRITE(0x3032);
    if (info->jevois & JEVOIS_SENSOR_MONO)
    {
      SENSOR_READ(0x30b0); val |= (1 << 7); SENSOR_WRITE(0x30b0);
    }
    else      
    {
      SENSOR_READ(0x306e); val &= ~(1 << 4); SENSOR_WRITE(0x306e);
      SENSOR_READ(0x30b0); val &= ~(1 << 7); SENSOR_WRITE(0x30b0);
    }

    binfac = 1;
  }

  info->width = wsize->width;
  info->height = wsize->height;
  info->exp_max_lines = wsize->intg_max;
  info->mbus_code = fmt->code;

  val = wsize->voffset; SENSOR_WRITE(0x3002);
  val += wsize->height * binfac + (wsize->height < 480 ? 2:0);
  SENSOR_WRITE(0x3006);

  val += 23; SENSOR_WRITE(0x300a);
  val = 1388; SENSOR_WRITE(0x300c);
  
  val = wsize->hoffset; SENSOR_WRITE(0x3004);
  val += wsize->width * binfac + 1; SENSOR_WRITE(0x3008);
  
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
  int ret = 0; unsigned short val;
  long tpfrows, activelines;
  
  if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) return -EINVAL;

  info->capture_mode = cp->capturemode;
  if (info->capture_mode == V4L2_MODE_IMAGE) { vfe_dev_dbg("Cannot set frame rate in image capture mode\n"); return 0; }

  if (tpf->numerator == 0 || tpf->denominator == 0)
  { tpf->numerator = 1; tpf->denominator = 30; vfe_dev_err("Frame rate reset to 30fps\n"); }

  info->tpf.denominator = tpf->denominator; info->tpf.numerator = tpf->numerator;

  tpfrows = (1000000000 / tpf->denominator) * tpf->numerator / 18694;

  tpfrows = (tpfrows * 89) / 100;
  
  SENSOR_READ(0x3006); activelines = val;
  SENSOR_READ(0x3002); activelines -= val;

  val = tpfrows;
  if (val < activelines + 24) val = activelines + 24;

  SENSOR_WRITE(0x300a);

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
  case V4L2_CID_BRIGHTNESS: return v4l2_ctrl_query_fill(qc, 1, 0xa00, 1, 0x700);
  case V4L2_CID_VFLIP: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_HFLIP: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_GAIN: return v4l2_ctrl_query_fill(qc, 0, 3, 1, 0);
  case V4L2_CID_AUTOGAIN: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
  case V4L2_CID_EXPOSURE_AUTO: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_EXPOSURE_ABSOLUTE: return v4l2_ctrl_query_fill(qc, 3, 5000, 1, 1700);
  case V4L2_CID_RED_BALANCE: return v4l2_ctrl_query_fill(qc, 1, 0x40, 1, 0x20);
  case V4L2_CID_BLUE_BALANCE: return v4l2_ctrl_query_fill(qc, 1, 0x40, 1, 0x20);

  default:
    return -EINVAL;
  }
}

static int sensor_querymenu(struct v4l2_subdev *sd, struct v4l2_querymenu *qm)
{
  qm->id &= ~(V4L2_CTRL_FLAG_NEXT_CTRL | V4L2_CTRL_FLAG_NEXT_COMPOUND);
  
  switch (qm->id)
  {
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

  default:
    return -EINVAL;
  }
}

static int sensor_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
  int ret;

  switch (ctrl->id)
  {
  case V4L2_CID_BRIGHTNESS: ret = sensor_g_brightness(sd, &ctrl->value); break;
  case V4L2_CID_VFLIP: ret = sensor_g_vflip(sd, &ctrl->value); break;
  case V4L2_CID_HFLIP: ret = sensor_g_hmirror(sd, &ctrl->value); break;
  case V4L2_CID_GAIN: ret = sensor_g_gain(sd, &ctrl->value); break;
  case V4L2_CID_AUTOGAIN: ret = sensor_g_autogain(sd, &ctrl->value); break;
  case V4L2_CID_EXPOSURE_AUTO: ret = sensor_g_autoexp(sd, &ctrl->value); break;
  case V4L2_CID_EXPOSURE_ABSOLUTE: ret = sensor_g_exposure_absolute(sd, &ctrl->value); break;
  case V4L2_CID_RED_BALANCE: ret = sensor_g_wb_red(sd, &ctrl->value); break;
  case V4L2_CID_BLUE_BALANCE: ret = sensor_g_wb_blue(sd, &ctrl->value); break;
  default:
    return -EINVAL;
  }
  
  return ret;
}

static int sensor_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{

  switch (ctrl->id)
  {
  case V4L2_CID_BRIGHTNESS: return sensor_s_brightness(sd, ctrl->value);
  case V4L2_CID_VFLIP: return sensor_s_vflip(sd, ctrl->value);
  case V4L2_CID_HFLIP: return sensor_s_hmirror(sd, ctrl->value);
  case V4L2_CID_GAIN: return sensor_s_gain(sd, ctrl->value);
  case V4L2_CID_AUTOGAIN: return sensor_s_autogain(sd, ctrl->value);
  case V4L2_CID_EXPOSURE_AUTO: return sensor_s_autoexp(sd, (enum v4l2_exposure_auto_type)ctrl->value);
  case V4L2_CID_EXPOSURE_ABSOLUTE: return sensor_s_exposure_absolute(sd, ctrl->value);
  case V4L2_CID_RED_BALANCE: return sensor_s_wb_red(sd, ctrl->value);
  case V4L2_CID_BLUE_BALANCE: return sensor_s_wb_blue(sd, ctrl->value);
  default:
    return -EINVAL;
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

