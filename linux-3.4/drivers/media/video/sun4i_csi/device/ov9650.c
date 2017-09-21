/*
 * A V4L2 driver for ov9650 cameras.
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
#include "../../sunxi-vfe/vfe_subdev.h"


#define ABS_SENSOR(x)                 ((x) > 0 ? (x) : -(x))

#define HXGA_WIDTH    4000
#define HXGA_HEIGHT   3000
#define QUXGA_WIDTH   3264
#define QUXGA_HEIGHT  2448
#define QSXGA_WIDTH   2592
#define QSXGA_HEIGHT  1936
#define QXGA_WIDTH    2048
#define QXGA_HEIGHT   1536
#define HD1080_WIDTH  1920
#define HD1080_HEIGHT 1080
#define UXGA_WIDTH    1600
#define UXGA_HEIGHT   1200
#define SXGA_WIDTH    1280
#define SXGA_HEIGHT   960
#define HD720_WIDTH   1280
#define HD720_HEIGHT  720
#define XGA_WIDTH     1024
#define XGA_HEIGHT    768
#define SVGA_WIDTH    800
#define SVGA_HEIGHT   600
#define VGA_WIDTH     640
#define VGA_HEIGHT    480
#define QVGA_WIDTH    320
#define QVGA_HEIGHT   240
#define CIF_WIDTH     352
#define CIF_HEIGHT    288
#define QCIF_WIDTH    176
#define QCIF_HEIGHT   144

#define CSI_GPIO_HIGH     1
#define CSI_GPIO_LOW     0
#define CCI_BITS_8           8
#define CCI_BITS_16         16

MODULE_AUTHOR("Laurent Itti");
MODULE_DESCRIPTION("A low-level driver for ov9650 sensors");
MODULE_LICENSE("GPL");

#define AF_WIN_NEW_COORD

#define DEV_DBG_EN      1

#if(DEV_DBG_EN == 1)
#define vfe_dev_dbg(x, arg...) printk("[OV9650] "x, ##arg)
#else
#define vfe_dev_dbg(x, arg...)
#endif
#define vfe_dev_err(x, arg...) printk("[OV9650] "x, ##arg)
#define vfe_dev_print(x, arg...) printk("[OV9650] "x, ##arg)

#define LOG_ERR_RET(x)  { int ret; ret = x; if (ret < 0) { vfe_dev_err("error in %s\n", __func__); return ret; } }

#define SENSOR_WRITE(reg, val)                                          \
  ret = sensor_write(sd, reg, val);                                     \
  if (ret < 0) { vfe_dev_err("Error writing %u to sensor register %u in %s\n", val, reg, __func__); return ret; }

#define SENSOR_READ(reg)                                                \
  ret = sensor_read(sd, reg, &val);                                     \
  if (ret < 0) { vfe_dev_err("Error reading from sensor register %u in %s\n", reg, __func__); return ret;	}

#define MCLK              (24*1000*1000)
#define VREF_POL          V4L2_MBUS_VSYNC_ACTIVE_HIGH
#define HREF_POL          V4L2_MBUS_HSYNC_ACTIVE_LOW
#define CLK_POL           V4L2_MBUS_PCLK_SAMPLE_RISING
#define V4L2_IDENT_SENSOR 0x9650


#define CSI_STBY_ON     1
#define CSI_STBY_OFF    0

#define CSI_RST_ON      1
#define CSI_RST_OFF     0

#define CSI_PWR_ON      1
#define CSI_PWR_OFF     0

struct reg_list_a8_d8
{
    unsigned char addr;
    unsigned char data;
};

#define regval_list reg_list_a8_d8

#define REG_DLY  0xffff

#define SENSOR_FRAME_RATE 30

#define I2C_ADDR 0x60
#define SENSOR_NAME "ov9650"

struct sensor_win_size {
	int	width;
	int	height;
	int	hstart;		/* Start/stop values for the camera.  Note */
	int	hstop;		/* that they do not always make complete */
	int	vstart;		/* sense to humans, but evidently the sensor */
	int	vstop;		/* will do the right thing... */
	struct regval_list *regs; /* Regs to tweak */
	int regs_size;
	int (*set_size) (struct v4l2_subdev *sd);
};

struct sensor_info {
    struct v4l2_subdev                    sd;
    struct sensor_format_struct           *fmt;  /* Current format */
    enum standby_mode                     stby_mode;
    unsigned int                          width;
    unsigned int                          height;
    unsigned int                          capture_mode;  
    unsigned int                          af_first_flag;
    unsigned int                          init_first_flag;
    unsigned int                          preview_first_flag;
    unsigned int                          auto_focus; 
    unsigned int                          focus_status;  
    unsigned int                          low_speed;   
    int                                   brightness;
    int                                   contrast;
    int                                   saturation;
    int                                   hue;
    unsigned int                          hflip;
    unsigned int                          vflip;
    unsigned int                          gain;
    unsigned int                          autogain;
    unsigned int                          exp;
    int                                   exp_bias;
    enum v4l2_exposure_auto_type          autoexp;
    unsigned int                          autowb;
    enum v4l2_auto_n_preset_white_balance wb;
    enum v4l2_colorfx                     clrfx;
    enum v4l2_flash_led_mode              flash_mode;
    enum v4l2_power_line_frequency        band_filter;
    struct v4l2_fract                     tpf; 
    struct sensor_win_size                *current_wins;
    struct flash_dev_info                 *fl_dev_info;
};

struct sensor_format_struct;

static inline struct sensor_info *to_state(struct v4l2_subdev *sd)
{
  return container_of(sd, struct sensor_info, sd);
}


static struct regval_list OV9652_YCbCr8bit[]={   
    {0x12,   0x80},      
    {0x11,   0x83},        
    {0x12,   0x00},
    {0x13,   0xaf},        
    {0x01,   0x80},      
    {0x02,   0x80},        
    {0x04,   0x00},
    {0x0c,   0x00},        
    {0x0d,   0x40},        
    {0x0e,   0x80},  
    {0x0f,   0x4f},        
    {0x14,   0x4a},        
    {0x1b,   0x01},        
    {0x24,   0x80},        
    {0x25,   0x78},        
    {0x26,   0xd3},        
    {0x27,   0x90},        
    {0x2a,   0x00},        
    {0x2b,   0x00},        
    {0x33,   0x02},        
    {0x37,   0x02},        
    {0x38,   0x13},        
    {0x39,   0xf0},        
    {0x3a,   0x00},        
    {0x3b,   0x01},        
    {0x3c,   0x46},        
    {0x3d,   0x90},        
    {0x3e,   0x02},        
    {0x3f,   0xf2},        
    {0x41,   0x02},        
    {0x42,   0xc9},      
    {0x4f,   0x50},        
    {0x50,   0x43},        
    {0x51,   0x0d},        
    {0x52,   0x19},        
    {0x53,   0x4c},        
    {0x54,   0x65},        
    {0x62,   0x70},        
    {0x63,   0x00},        
    {0x64,   0x04},        
    {0x65,   0x00},        
    {0x66,   0x00},    
    {0x69,   0x00},//Bit[2:1]   --MSB   of   banding   filter   value          
    {0x6a,   0x11},  
    {0x6c,   0x40},  
    {0x6d,   0x30},        
    {0x6e,   0x4b},        
    {0x6f,   0x60},        
    {0x70,   0x70},        
    {0x71,   0x70},        
    {0x72,   0x70},        
    {0x73,   0x70},        
    {0x74,   0x60},        
    {0x75,   0x60},        
    {0x76,   0x50},        
    {0x77,   0x48},        
    {0x78,   0x3a},        
    {0x79,   0x2e},        
    {0x7a,   0x28},        
    {0x7b,   0x22},        
    {0x7c,   0x04},        
    {0x7d,   0x07},        
    {0x7e,   0x10},        
    {0x7f,   0x28},        
    {0x80,   0x36},        
    {0x81,   0x44},        
    {0x82,   0x52},        
    {0x83,   0x60},        
    {0x84,   0x6c},        
    {0x85,   0x78},        
    {0x86,   0x8c},        
    {0x87,   0x9e},        
    {0x88,   0xbb},        
    {0x89,   0xd2},        
    {0x8a,   0xe6},        
    {0x13,   0xaf},        
      };

#include "ov9650_regs.h"

static struct regval_list sensor_default_regs_k44[] = {
  { REG_COM2, 0x10 },	/* Set soft sleep mode */
  { REG_COM5, 0x00 },	/* System clock options */
  { REG_COM2, 0x01 },	/* Output drive, soft sleep mode */
  { REG_COM10, 0x00 },	/* Slave mode, HREF vs HSYNC, signals negate */
  { REG_EDGE, 0xa6 },	/* Edge enhancement treshhold and factor */
  { REG_COM16, 0x02 },	/* Color matrix coeff double option */
  { REG_COM17, 0x08 },	/* Single frame out, banding filter */
  { 0x16, 0x06 },
  { REG_CHLF, 0xc0 },	/* Reserved  */
  { 0x34, 0xbf },
  { 0xa8, 0x80 },
  { 0x96, 0x04 },
  { 0x8e, 0x00 },
  { REG_COM12, 0x77 },	/* HREF option, UV average  */
  { 0x8b, 0x06 },
  { 0x35, 0x91 },
  { 0x94, 0x88 },
  { 0x95, 0x88 },
  { REG_COM15, 0xc1 },	/* Output range, RGB 555/565 */
  { REG_GRCOM, 0x2f },	/* Analog BLC & regulator */
  { REG_COM6, 0x43 },	/* HREF & ADBLC options */
  { REG_COM8, 0xe5 },	/* AGC/AEC options */
  { REG_COM13, 0x90 },	/* Gamma selection, colour matrix, UV delay */
  { REG_HV, 0x80 },	/* Manual banding filter MSB  */
  { 0x5c, 0x96 },		/* Reserved up to 0xa5 */
  { 0x5d, 0x96 },
  { 0x5e, 0x10 },
  { 0x59, 0xeb },
  { 0x5a, 0x9c },
  { 0x5b, 0x55 },
  { 0x43, 0xf0 },
  { 0x44, 0x10 },
  { 0x45, 0x55 },
  { 0x46, 0x86 },
  { 0x47, 0x64 },
  { 0x48, 0x86 },
  { 0x5f, 0xe0 },
  { 0x60, 0x8c },
  { 0x61, 0x20 },
  { 0xa5, 0xd9 },
  { 0xa4, 0x74 },		/* reserved */
  { REG_COM23, 0x02 },	/* Color gain analog/_digital_ */
  { REG_COM8, 0xe7 },	/* Enable AEC, AWB, AEC */
  { REG_COM22, 0x23 },	/* Edge enhancement, denoising */
  { 0xa9, 0xb8 },
  { 0xaa, 0x92 },
  { 0xab, 0x0a },
  { REG_DBLC1, 0xdf },	/* Digital BLC */
  { REG_DBLC_B, 0x00 },	/* Digital BLC B chan offset */
  { REG_DBLC_R, 0x00 },	/* Digital BLC R chan offset */
  { REG_DBLC_GB, 0x00 },	/* Digital BLC GB chan offset */
  { REG_DBLC_GR, 0x00 },
  { REG_COM9, 0x3a },	/* Gain ceiling 16x */
};

static struct regval_list sensor_default_regs[] = {
  { 0x09, 0x10 },	/* Set soft sleep mode */
  { 0x0e, 0x00 },	/* System clock options */
  { 0x09, 0x01 },	/* Output drive, soft sleep mode */
  { 0x15, 0x00 },	/* Slave mode, HREF vs HSYNC, signals negate */
  { 0x3f, 0xa6 },	/* Edge enhancement treshhold and factor */
  { 0x41, 0x02 },	/* Color matrix coeff double option */
  { 0x42, 0x08 },	/* Single frame out, banding filter */
  { 0x16, 0x06 },
  { 0x33, 0xc0 },	/* Reserved  */
  { 0x34, 0xbf },
  { 0xa8, 0x80 },
  { 0x96, 0x04 },
  { 0x8e, 0x00 },
  { 0x3c, 0x77 },	/* HREF option, UV average  */
  { 0x8b, 0x06 },
  { 0x35, 0x91 },
  { 0x94, 0x88 },
  { 0x95, 0x88 },
  { 0x40, 0xc1 },	/* Output range, RGB 555/565 */
  { 0x29, 0x2f },	/* Analog BLC & regulator */
  { 0x0f, 0x43 },	/* HREF & ADBLC options */
  { 0x13, 0xe5 },	/* AGC/AEC options */
  { 0x3d, 0x90 },	/* Gamma selection, colour matrix, UV delay */
  { 0x69, 0x80 },	/* Manual banding filter MSB  */
  { 0x5c, 0x96 },		/* Reserved up to 0xa5 */
  { 0x5d, 0x96 },
  { 0x5e, 0x10 },
  { 0x59, 0xeb },
  { 0x5a, 0x9c },
  { 0x5b, 0x55 },
  { 0x43, 0xf0 },
  { 0x44, 0x10 },
  { 0x45, 0x55 },
  { 0x46, 0x86 },
  { 0x47, 0x64 },
  { 0x48, 0x86 },
  { 0x5f, 0xe0 },
  { 0x60, 0x8c },
  { 0x61, 0x20 },
  { 0xa5, 0xd9 },
  { 0xa4, 0x74 },		/* reserved */
  { 0x8d, 0x02 },	/* Color gain analog/_digital_ */
  { 0x13, 0xe7 },	/* Enable AEC, AWB, AEC */
  { 0x8c, 0x23 },	/* Edge enhancement, denoising */
  { 0xa9, 0xb8 },
  { 0xaa, 0x92 },
  { 0xab, 0x0a },
  { 0x8f, 0xdf },	/* Digital BLC */
  { 0x90, 0x00 },	/* Digital BLC B chan offset */
  { 0x91, 0x00 },	/* Digital BLC R chan offset */
  { 0x9f, 0x00 },	/* Digital BLC GB chan offset */
  { 0xa0, 0x00 },
  { 0x14, 0x3a },	/* Gain ceiling 16x */
};

static struct regval_list sensor_sxga_regs[] = {
  { 0x12, 0x00 },
  { 0x0c, 0x00 },
  { 0x0d, 0x00 },
  { 0x17, 0x1e },
  { 0x18, 0xbe },
  { 0x32, 0xbf },
  { 0x19, 0x01 },
  { 0x1a, 0x81 },
  { 0x03, 0x12 },
  { 0x2a, 0x10 },
  { 0x2b, 0x34 },
  { 0x37, 0x81 },
  { 0x38, 0x93 },
  { 0x39, 0x51 },
};

static struct regval_list sensor_vga_regs[] = {
  { 0x12, 0x40 },
  { 0x0c, 0x04 },
  { 0x0d, 0x80 },
  { 0x17, 0x26 },
  { 0x18, 0xc6 },
  { 0x32, 0xed },
  { 0x19, 0x01 },
  { 0x1a, 0x3d },
  { 0x03, 0x00 },
  { 0x2a, 0x10 },
  { 0x2b, 0x40 },
  { 0x37, 0x91 },
  { 0x38, 0x12 },
  { 0x39, 0x43 },
};

static struct regval_list sensor_qvga_regs[] = {
  { 0x12, 0x10 },
  { 0x0c, 0x04 },
  { 0x0d, 0x80 },
  { 0x17, 0x25 },
  { 0x18, 0xc5 },
  { 0x32, 0xbf },
  { 0x19, 0x00 },
  { 0x1a, 0x80 },
  { 0x03, 0x12 },
  { 0x2a, 0x10 },
  { 0x2b, 0x40 },
  { 0x37, 0x91 },
  { 0x38, 0x12 },
  { 0x39, 0x43 },
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


/*
 * The white balance settings
 * Here only tune the R G B channel gain.
 * The white balance enalbe bit is modified in sensor_s_autowb and sensor_s_wb
 */

 /*
static struct regval_list sensor_wb_manual[] = {
};
*/
static struct regval_list sensor_wb_auto_regs[] = {
  /* FIXME
    {0xff, 0x00}, 
    {0xc7, 0x00},
  */
};

static struct regval_list sensor_wb_incandescence_regs[] = {
  /* FIXME
   {0xff, 0x00},
    {0xc7, 0x40},
    {0xcc, 0x42},
    {0xcd, 0x3f},
    {0xce, 0x71},
  */
};

static struct regval_list sensor_wb_fluorescent_regs[] = {
  /* FIXME
   {0xff, 0x00},
    {0xc7, 0x40},
    {0xcc, 0x52},
    {0xcd, 0x41},
    {0xce, 0x66},
  */
};

static struct regval_list sensor_wb_tungsten_regs[] = {
  /* FIXME
    {0xff, 0x00},
    {0xc7, 0x40},
    {0xcc, 0x52},
    {0xcd, 0x41},
    {0xce, 0x66},
  */
};
/*
static struct regval_list sensor_wb_horizon[] = {
};
*/
static struct regval_list sensor_wb_daylight_regs[] = {
  /* FIXME
	{0xff, 0x00},
    {0xc7, 0x40},
    {0xcc, 0x5e},
    {0xcd, 0x41},
    {0xce, 0x54},
  */
};

static struct regval_list sensor_wb_cloud_regs[] = {
  /* FIXME
	{0xff, 0x00},
    {0xc7, 0x40},
    {0xcc, 0x65},
    {0xcd, 0x41},
    {0xce, 0x4f},
  */
};

static struct regval_list sensor_brightness_neg3_regs[] = { { 0x24, 0x1c }, { 0x25, 0x12 }, { 0x26, 0x50 } };
static struct regval_list sensor_brightness_neg2_regs[] = { { 0x24, 0x3d }, { 0x25, 0x30 }, { 0x26, 0x71 } };
static struct regval_list sensor_brightness_neg1_regs[] = { { 0x24, 0x50 }, { 0x25, 0x44 }, { 0x26, 0x92 } };
static struct regval_list sensor_brightness_zero_regs[] = { { 0x24, 0x70 }, { 0x25, 0x64 }, { 0x26, 0xc3 } };
static struct regval_list sensor_brightness_pos1_regs[] = { { 0x24, 0x90 }, { 0x25, 0x84 }, { 0x26, 0xd4 } };
static struct regval_list sensor_brightness_pos2_regs[] = { { 0x24, 0xc4 }, { 0x25, 0xbf }, { 0x26, 0xe9 } };
static struct regval_list sensor_brightness_pos3_regs[] = { { 0x24, 0xd8 }, { 0x25, 0xd0 }, { 0x26, 0xfa } };

static struct regval_list sensor_contrast_neg3_regs[] = {
  { 0x6C, 0x20 }, { 0x6D, 0x50 }, { 0x6E, 0x80 }, { 0x6F, 0xC0 }, { 0x70, 0xC0 }, { 0x71, 0xA0 }, { 0x72, 0x90 },
  { 0x73, 0x78 }, { 0x74, 0x78 }, { 0x75, 0x78 }, { 0x76, 0x40 }, { 0x77, 0x20 }, { 0x78, 0x20 }, { 0x79, 0x20 },
  { 0x7A, 0x14 }, { 0x7B, 0x0E }, { 0x7C, 0x02 }, { 0x7D, 0x07 }, { 0x7E, 0x17 }, { 0x7F, 0x47 }, { 0x80, 0x5F },
  { 0x81, 0x73 }, { 0x82, 0x85 }, { 0x83, 0x94 }, { 0x84, 0xA3 }, { 0x85, 0xB2 }, { 0x86, 0xC2 }, { 0x87, 0xCA },
  { 0x88, 0xDA }, { 0x89, 0xEA }, { 0x8A, 0xF4 } };

static struct regval_list sensor_contrast_neg2_regs[] = {
  { 0x7C, 0x02 }, { 0x7D, 0x07 }, { 0x7E, 0x1f }, { 0x7F, 0x49 }, { 0x80, 0x5a }, { 0x81, 0x6a }, { 0x82, 0x79 },
  { 0x83, 0x87 }, { 0x84, 0x94 }, { 0x85, 0x9f }, { 0x86, 0xaf }, { 0x87, 0xbb }, { 0x88, 0xcf }, { 0x89, 0xdf },
  { 0x8A, 0xee }, { 0x6C, 0x20 }, { 0x6D, 0x50 }, { 0x6E, 0xc0 }, { 0x6F, 0xa8 }, { 0x70, 0x88 }, { 0x71, 0x80 },
  { 0x72, 0x78 }, { 0x73, 0x70 }, { 0x74, 0x68 }, { 0x75, 0x58 }, { 0x76, 0x40 }, { 0x77, 0x30 }, { 0x78, 0x28 },
  { 0x79, 0x20 }, { 0x7A, 0x1e }, { 0x7B, 0x17 } };

static struct regval_list sensor_contrast_neg1_regs[] = {
  { 0x7C, 0x02 }, { 0x7D, 0x06 }, { 0x7E, 0x16 }, { 0x7F, 0x3A }, { 0x80, 0x4C }, { 0x81, 0x5C }, { 0x82, 0x6A },
  { 0x83, 0x78 }, { 0x84, 0x84 }, { 0x85, 0x8E }, { 0x86, 0x9E }, { 0x87, 0xAE }, { 0x88, 0xC6 }, { 0x89, 0xDA },
  { 0x8A, 0xEA }, { 0x6C, 0x20 }, { 0x6D, 0x40 }, { 0x6E, 0x80 }, { 0x6F, 0x90 }, { 0x70, 0x90 }, { 0x71, 0x80 },
  { 0x72, 0x70 }, { 0x73, 0x70 }, { 0x74, 0x60 }, { 0x75, 0x50 }, { 0x76, 0x40 }, { 0x77, 0x40 }, { 0x78, 0x30 },
  { 0x79, 0x28 }, { 0x7A, 0x20 }, { 0x7B, 0x1c } };

static struct regval_list sensor_contrast_zero_regs[] = {
  { 0x7C, 0x04 }, { 0x7D, 0x07 }, { 0x7E, 0x10 }, { 0x7F, 0x28 }, { 0x80, 0x36 }, { 0x81, 0x44 }, { 0x82, 0x52 },
  { 0x83, 0x60 }, { 0x84, 0x6C }, { 0x85, 0x78 }, { 0x86, 0x8C }, { 0x87, 0x9E }, { 0x88, 0xBB }, { 0x89, 0xD2 },
  { 0x8A, 0xE6 }, { 0x6C, 0x40 }, { 0x6D, 0x30 }, { 0x6E, 0x48 }, { 0x6F, 0x60 }, { 0x70, 0x70 }, { 0x71, 0x70 },
  { 0x72, 0x70 }, { 0x73, 0x70 }, { 0x74, 0x60 }, { 0x75, 0x60 }, { 0x76, 0x50 }, { 0x77, 0x48 }, { 0x78, 0x3A },
  { 0x79, 0x2E }, { 0x7A, 0x28 }, { 0x7B, 0x22 } };

static struct regval_list sensor_contrast_pos1_regs[] = {
  { 0x7C, 0x04 }, { 0x7D, 0x09 }, { 0x7E, 0x13 }, { 0x7F, 0x29 }, { 0x80, 0x35 }, { 0x81, 0x41 }, { 0x82, 0x4D },
  { 0x83, 0x59 }, { 0x84, 0x64 }, { 0x85, 0x6F }, { 0x86, 0x85 }, { 0x87, 0x97 }, { 0x88, 0xB7 }, { 0x89, 0xCF },
  { 0x8A, 0xE3 }, { 0x6C, 0x40 }, { 0x6D, 0x50 }, { 0x6E, 0x50 }, { 0x6F, 0x58 }, { 0x70, 0x60 }, { 0x71, 0x60 },
  { 0x72, 0x60 }, { 0x73, 0x60 }, { 0x74, 0x58 }, { 0x75, 0x58 }, { 0x76, 0x58 }, { 0x77, 0x48 }, { 0x78, 0x40 },
  { 0x79, 0x30 }, { 0x7A, 0x28 }, { 0x7B, 0x26 } };

static struct regval_list sensor_contrast_pos2_regs[] = {
  { 0x7c, 0x05 }, { 0x7d, 0x0b }, { 0x7e, 0x16 }, { 0x7f, 0x2c }, { 0x80, 0x37 }, { 0x81, 0x41 }, { 0x82, 0x4b },
  { 0x83, 0x55 }, { 0x84, 0x5f }, { 0x85, 0x69 }, { 0x86, 0x7c }, { 0x87, 0x8f }, { 0x88, 0xb1 }, { 0x89, 0xcf },
  { 0x8a, 0xe5 }, { 0x6c, 0x50 }, { 0x6d, 0x60 }, { 0x6e, 0x58 }, { 0x6f, 0x58 }, { 0x70, 0x58 }, { 0x71, 0x50 },
  { 0x72, 0x50 }, { 0x73, 0x50 }, { 0x74, 0x50 }, { 0x75, 0x50 }, { 0x76, 0x4c }, { 0x77, 0x4c }, { 0x78, 0x44 },
  { 0x79, 0x3c }, { 0x7a, 0x2c }, { 0x7b, 0x23 } };

static struct regval_list sensor_contrast_pos3_regs[] = {
  { 0x7c, 0x04 }, { 0x7d, 0x09 }, { 0x7e, 0x14 }, { 0x7f, 0x28 }, { 0x80, 0x32 }, { 0x81, 0x3C }, { 0x82, 0x46 },
  { 0x83, 0x4F }, { 0x84, 0x58 }, { 0x85, 0x61 }, { 0x86, 0x73 }, { 0x87, 0x85 }, { 0x88, 0xA5 }, { 0x89, 0xc5 },
  { 0x8a, 0xDD }, { 0x6c, 0x40 }, { 0x6d, 0x50 }, { 0x6e, 0x58 }, { 0x6f, 0x50 }, { 0x70, 0x50 }, { 0x71, 0x50 },
  { 0x72, 0x50 }, { 0x73, 0x48 }, { 0x74, 0x48 }, { 0x75, 0x48 }, { 0x76, 0x48 }, { 0x77, 0x48 }, { 0x78, 0x40 },
  { 0x79, 0x40 }, { 0x7a, 0x30 }, { 0x7b, 0x2E } };

static struct regval_list sensor_saturation_neg2_regs[] = {
  { 0x4f, 0x1d }, { 0x50, 0x1f }, { 0x51, 0x02 }, { 0x52, 0x09 }, { 0x53, 0x13 }, { 0x54, 0x1c } };

static struct regval_list sensor_saturation_neg1_regs[] = {
  { 0x4f, 0x2e }, { 0x50, 0x31 }, { 0x51, 0x02 }, { 0x52, 0x0e }, { 0x53, 0x1E }, { 0x54, 0x2d } };

static struct regval_list sensor_saturation_zero_regs[] = {
  { 0x4f, 0x3a }, { 0x50, 0x3d }, { 0x51, 0x03 }, { 0x52, 0x12 }, { 0x53, 0x26 }, { 0x54, 0x38 } };

static struct regval_list sensor_saturation_pos1_regs[] = {
  { 0x4f, 0x46 }, { 0x50, 0x49 }, { 0x51, 0x04 }, { 0x52, 0x16 }, { 0x53, 0x2e }, { 0x54, 0x43 } };

static struct regval_list sensor_saturation_pos2_regs[] = {
  { 0x4f, 0x57 }, { 0x50, 0x5c }, { 0x51, 0x05 }, { 0x52, 0x1b }, { 0x53, 0x39 }, { 0x54, 0x54 } };

static struct regval_list sensor_lightmode_auto_regs[] = {
  { 0x13, 0xe7 }, { 0x11, 0x81 }, { 0x41, 0x02 }, { 0x3f, 0xa6 }, { 0x3d, 0x92 }, { 0x66, 0x01 }, { 0x14, 0x1e } };

static struct regval_list sensor_lightmode_sunny_regs[] = {
  { 0x13, 0xc5 }, { 0x01, 0x74 }, { 0x02, 0x90 }, { 0x14, 0x0e }, { 0x11, 0x81 }, { 0x41, 0x02 }, { 0x3f, 0xa6 },
  { 0x3d, 0x92 }, { 0x66, 0x01 } };

static struct regval_list sensor_lightmode_cloudy_regs[] = {
  { 0x13, 0xc5 }, { 0x01, 0x74 }, { 0x02, 0x90 }, { 0x14, 0x1e }, { 0x11, 0x81 }, { 0x41, 0x02 }, { 0x3f, 0xa6 },
  { 0x3d, 0x92 }, { 0x66, 0x01 } };

static struct regval_list sensor_lightmode_office_regs[] = {
  { 0x13, 0xe5 }, { 0x01, 0x97 }, { 0x02, 0x7d }, { 0x14, 0x1e }, { 0x11, 0x81 }, { 0x41, 0x02 }, { 0x3f, 0xa6 },
  { 0x3d, 0x92 }, { 0x66, 0x01 } };

static struct regval_list sensor_lightmode_home_regs[] = {
  { 0x13, 0xe5 }, { 0x01, 0x9c }, { 0x02, 0x54 }, { 0x14, 0x1e }, { 0x11, 0x81 }, { 0x41, 0x02 }, { 0x3f, 0xa6 },
  { 0x3d, 0x92 }, { 0x66, 0x01 } };

static struct regval_list sensor_lightmode_night_regs[] = {
  { 0x11, 0x83 }, { 0x41, 0x00 }, { 0x3f, 0xf3 }, { 0x3d, 0x12 }, { 0x66, 0x00 }, { 0x13, 0xe7 }, { 0x14, 0x1e } };

static struct regval_list sensor_vga_yuv_regs[] = {
  { 0x12, 0x80 }, { 0x11, 0x81 }, { 0x6b, 0x0a }, { 0x6a, 0x3e }, { 0x3b, 0x09 }, { 0x13, 0xe0 }, { 0x01, 0x80 },
  { 0x02, 0x80 }, { 0x00, 0x00 }, { 0x10, 0x00 }, { 0x13, 0xe5 }, { 0x39, 0x50 }, { 0x38, 0x92 }, { 0x37, 0x00 },
  { 0x35, 0x81 }, { 0x0e, 0x20 }, { 0x1e, 0x04 }, { 0xa8, 0x80 }, { 0x12, 0x40 }, { 0x04, 0x00 }, { 0x0c, 0x04 },
  { 0x0d, 0x80 }, { 0x18, 0xc6 }, { 0x17, 0x26 }, { 0x32, 0xad }, { 0x03, 0x00 }, { 0x1a, 0x3d }, { 0x19, 0x01 },
  { 0x3f, 0xa6 }, { 0x14, 0x2e }, { 0x15, 0x02 }, { 0x41, 0x02 }, { 0x42, 0x08 }, { 0x1b, 0x00 }, { 0x16, 0x06 },
  { 0x33, 0xe2 }, { 0x34, 0xbf }, { 0x96, 0x04 }, { 0x3a, 0x00 }, { 0x8e, 0x00 }, { 0x3c, 0x77 }, { 0x8b, 0x06 },
  { 0x94, 0x88 }, { 0x95, 0x88 }, { 0x40, 0xc1 }, { 0x29, 0x3f }, { 0x0f, 0x42 }, { 0x3d, 0x92 }, { 0x69, 0x40 },
  { 0x5c, 0xb9 }, { 0x5d, 0x96 }, { 0x5e, 0x10 }, { 0x59, 0xc0 }, { 0x5a, 0xaf }, { 0x5b, 0x55 }, { 0x43, 0xf0 },
  { 0x44, 0x10 }, { 0x45, 0x68 }, { 0x46, 0x96 }, { 0x47, 0x60 }, { 0x48, 0x80 }, { 0x5f, 0xe0 }, { 0x60, 0x8c },
  { 0x61, 0x20 }, { 0xa5, 0xd9 }, { 0xa4, 0x74 }, { 0x8d, 0x02 }, { 0x13, 0xe7 }, { 0x4f, 0x3a }, { 0x50, 0x3d },
  { 0x51, 0x03 }, { 0x52, 0x12 }, { 0x53, 0x26 }, { 0x54, 0x38 }, { 0x55, 0x40 }, { 0x56, 0x40 }, { 0x57, 0x40 },
  { 0x58, 0x0d }, { 0x8c, 0x23 }, { 0x3e, 0x02 }, { 0xa9, 0xb8 }, { 0xaa, 0x92 }, { 0xab, 0x0a }, { 0x8f, 0xdf },
  { 0x90, 0x00 }, { 0x91, 0x00 }, { 0x9f, 0x00 }, { 0xa0, 0x00 }, { 0x3a, 0x01 }, { 0x24, 0x70 }, { 0x25, 0x64 },
  { 0x26, 0xc3 }, { 0x2a, 0x00 }, { 0x2b, 0x00 }, { 0x6c, 0x40 }, { 0x6d, 0x30 }, { 0x6e, 0x4b }, { 0x6f, 0x60 },
  { 0x70, 0x70 }, { 0x71, 0x70 }, { 0x72, 0x70 }, { 0x73, 0x70 }, { 0x74, 0x60 }, { 0x75, 0x60 }, { 0x76, 0x50 },
  { 0x77, 0x48 }, { 0x78, 0x3a }, { 0x79, 0x2e }, { 0x7a, 0x28 }, { 0x7b, 0x22 }, { 0x7c, 0x04 }, { 0x7d, 0x07 },
  { 0x7e, 0x10 }, { 0x7f, 0x28 }, { 0x80, 0x36 }, { 0x81, 0x44 }, { 0x82, 0x52 }, { 0x83, 0x60 }, { 0x84, 0x6c },
  { 0x85, 0x78 }, { 0x86, 0x8c }, { 0x87, 0x9e }, { 0x88, 0xbb }, { 0x89, 0xd2 }, { 0x8a, 0xe6 } };

static struct regval_list sensor_sxga_yuv_regs[] = {
  { 0x12, 0x80 }, { 0x11, 0x80 }, { 0x6b, 0x0a }, { 0x6a, 0x41 }, { 0x3b, 0x09 }, { 0x13, 0xe0 }, { 0x01, 0x80 },
  { 0x02, 0x80 }, { 0x00, 0x00 }, { 0x10, 0x00 }, { 0x13, 0xe5 }, { 0x39, 0x50 }, { 0x38, 0x93 }, { 0x37, 0x00 },
  { 0x35, 0x81 }, { 0x0e, 0x20 }, { 0x1e, 0x04 }, { 0xa8, 0x80 }, { 0x12, 0x00 }, { 0x04, 0x00 }, { 0x0c, 0x00 },
  { 0x0d, 0x00 }, { 0x18, 0xbd }, { 0x17, 0x1d }, { 0x32, 0xad }, { 0x03, 0x12 }, { 0x1a, 0x81 }, { 0x19, 0x01 },
  { 0x14, 0x2e }, { 0x15, 0x00 }, { 0x3f, 0xa6 }, { 0x41, 0x02 }, { 0x42, 0x08 }, { 0x1b, 0x00 }, { 0x16, 0x06 },
  { 0x33, 0xe2 }, { 0x34, 0xbf }, { 0x96, 0x04 }, { 0x3a, 0x00 }, { 0x8e, 0x00 }, { 0x3c, 0x77 }, { 0x8b, 0x06 },
  { 0x94, 0x88 }, { 0x95, 0x88 }, { 0x40, 0xc1 }, { 0x29, 0x3f }, { 0x0f, 0x42 }, { 0x3d, 0x92 }, { 0x69, 0x40 },
  { 0x5c, 0xb9 }, { 0x5d, 0x96 }, { 0x5e, 0x10 }, { 0x59, 0xc0 }, { 0x5a, 0xaf }, { 0x5b, 0x55 }, { 0x43, 0xf0 },
  { 0x44, 0x10 }, { 0x45, 0x68 }, { 0x46, 0x96 }, { 0x47, 0x60 }, { 0x48, 0x80 }, { 0x5f, 0xe0 }, { 0x60, 0x8c },
  { 0x61, 0x20 }, { 0xa5, 0xd9 }, { 0xa4, 0x74 }, { 0x8d, 0x02 }, { 0x13, 0xe7 }, { 0x4f, 0x3a }, { 0x50, 0x3d },
  { 0x51, 0x03 }, { 0x52, 0x12 }, { 0x53, 0x26 }, { 0x54, 0x38 }, { 0x55, 0x40 }, { 0x56, 0x40 }, { 0x57, 0x40 },
  { 0x58, 0x0d }, { 0x8c, 0x23 }, { 0x3e, 0x02 }, { 0xa9, 0xb8 }, { 0xaa, 0x92 }, { 0xab, 0x0a }, { 0x8f, 0xdf },
  { 0x90, 0x00 }, { 0x91, 0x00 }, { 0x9f, 0x00 }, { 0xa0, 0x00 }, { 0x3a, 0x01 }, { 0x24, 0x70 }, { 0x25, 0x64 },
  { 0x26, 0xc3 }, { 0x2a, 0x00 }, { 0x2b, 0x00 }, { 0x6c, 0x40 }, { 0x6d, 0x30 }, { 0x6e, 0x4b }, { 0x6f, 0x60 },
  { 0x70, 0x70 }, { 0x71, 0x70 }, { 0x72, 0x70 }, { 0x73, 0x70 }, { 0x74, 0x60 }, { 0x75, 0x60 }, { 0x76, 0x50 },
  { 0x77, 0x48 }, { 0x78, 0x3a }, { 0x79, 0x2e }, { 0x7a, 0x28 }, { 0x7b, 0x22 }, { 0x7c, 0x04 }, { 0x7d, 0x07 },
  { 0x7e, 0x10 }, { 0x7f, 0x28 }, { 0x80, 0x36 }, { 0x81, 0x44 }, { 0x82, 0x52 }, { 0x83, 0x60 }, { 0x84, 0x6c },
  { 0x85, 0x78 }, { 0x86, 0x8c }, { 0x87, 0x9e }, { 0x88, 0xbb }, { 0x89, 0xd2 }, { 0x8a, 0xe6 } };

/*
 * The exposure target setttings
 */
static struct regval_list sensor_ev_neg4_regs[] = {
  /* FIXME
    {0xff,0x00},//register page
	{0x7c,0x00},
	{0x7d,0x04},
	{0x7c,0x09},
	{0x7d,0x28},//brightness value
    {0x7d,0x0e},//sign
  */
};

static struct regval_list sensor_ev_neg3_regs[] = {
  /* FIXME
    {0xff,0x00},//register page
	{0x7c,0x00},
	{0x7d,0x04},
	{0x7c,0x09},
	{0x7d,0x20},//brightness value
    {0x7d,0x0e},//sign
  */
};

static struct regval_list sensor_ev_neg2_regs[] = {
  /* FIXME
    {0xff,0x00},//register page
	{0x7c,0x00},
	{0x7d,0x04},
	{0x7c,0x09},
	{0x7d,0x18},//brightness value
    {0x7d,0x0e},//sign
  */
};

static struct regval_list sensor_ev_neg1_regs[] = {
  /* FIXME
    {0xff,0x00},//register page
	{0x7c,0x00},
	{0x7d,0x04},
	{0x7c,0x09},
	{0x7d,0x10},//brightness value
    {0x7d,0x0e},//sign
  */
};

static struct regval_list sensor_ev_zero_regs[] = {
  /* FIXME
    {0xff,0x00},//register page
	{0x7c,0x00},
	{0x7d,0x04},
	{0x7c,0x09},
	{0x7d,0x00},//brightness value
    {0x7d,0x00},//sign
  */
};

static struct regval_list sensor_ev_pos1_regs[] = {
  /* FIXME
    {0xff,0x00},//register page
	{0x7c,0x00},
	{0x7d,0x04},
	{0x7c,0x09},
	{0x7d,0x00},//brightness value
    {0x7d,0x00},//sign
  */
};

static struct regval_list sensor_ev_pos2_regs[] = {
  /* FIXME
    {0xff,0x00},//register page
	{0x7c,0x00},
	{0x7d,0x04},
	{0x7c,0x09},
	{0x7d,0x10},//brightness value
    {0x7d,0x00},//sign
  */
};

static struct regval_list sensor_ev_pos3_regs[] = {
  /* FIXME
    {0xff,0x00},//register page
	{0x7c,0x00},
	{0x7d,0x04},
	{0x7c,0x09},
	{0x7d,0x20},//brightness value
    {0x7d,0x00},//sign
    */
};

static struct regval_list sensor_ev_pos4_regs[] = {
  /* FIXME
    {0xff,0x00},//register page
	{0x7c,0x00},
	{0x7d,0x04},
	{0x7c,0x09},
	{0x7d,0x30},//brightness value
    {0x7d,0x00},//sign
  */
};

/*
 * Here we'll try to encapsulate the changes for just the output
 * video format.
 *
 */


static struct regval_list sensor_fmt_yuv422_yuyv[] = {
  /* FIXME
	{0xff,0x00},
	{0xda,0x01},
  */
};

static struct regval_list sensor_fmt_yuv422_yvyu[] = {
  /* FIXME
{0xff,0x00},
{0xda,0x01},
  */
};

static struct regval_list sensor_fmt_yuv422_vyuy[] = {
  /* FIXME
{0xff,0x00},
{0xda,0x01},
  */
};

static struct regval_list sensor_fmt_yuv422_uyvy[] = {
  /* FIXME
{0xff,0x00},
{0xda,0x01},
  */
};

static struct regval_list sensor_fmt_raw[] = {
};

/*
 * Low-level register I/O.
 *
 */


static int sensor_read(struct v4l2_subdev *sd, unsigned char reg, unsigned char *value)
{
  struct i2c_client *client = v4l2_get_subdevdata(sd);
  struct i2c_msg msg;
  int ret;

  msg.addr = client->addr;
  msg.flags = 0;
  msg.len = 1;
  msg.buf = &reg;
  ret = i2c_transfer(client->adapter, &msg, 1);
  if (ret >= 0) ret = 0;
  else { vfe_dev_err("Sensor read error for register 0x%x\n", reg); return ret; }

  msg.flags = I2C_M_RD;
  msg.len = 1;
  msg.buf = value;

  ret = i2c_transfer(client->adapter, &msg, 1);
  if (ret >= 0) ret = 0;
  else vfe_dev_err("Sensor read error for register 0x%x\n", reg);

  return ret;
}

static int sensor_write(struct v4l2_subdev *sd, unsigned char reg, unsigned char value)
{
  struct i2c_client *client = v4l2_get_subdevdata(sd);
  struct i2c_msg msg;
  int ret;

  msg.addr = client->addr;
  msg.flags = 0;
  msg.len = 1;
  msg.buf = &reg;

  ret = i2c_transfer(client->adapter, &msg, 1);
  if (ret >= 0) ret = 0;
  else { vfe_dev_err("Sensor write error for register 0x%x value 0x%x\n", reg, value); return ret; }

  msg.addr = client->addr;
  msg.flags = 0;
  msg.len = 1;
  msg.buf = &value;

  ret = i2c_transfer(client->adapter, &msg, 1);
  if (ret >= 0) ret = 0;
  else { vfe_dev_err("Sensor write error for register 0x%x value 0x%x\n", reg, value); return ret; }

  return ret;
}

static int sensor_write_array(struct v4l2_subdev *sd, struct regval_list *regs, int array_size)
{
  int i = 0;

  if (!regs) return -EINVAL;

  while (i < array_size)
  {
    if (regs->addr == REG_DLY) msleep(regs->data);
    else LOG_ERR_RET(sensor_write(sd, regs->addr, regs->data));
    ++i;
    ++regs;
  }
  return 0;
}


/* stuff about exposure when capturing image and video*/
unsigned char ogain,oexposurelow,oexposuremid,oexposurehigh;
unsigned int preview_exp_line,preview_fps;
unsigned long preview_pclk;


/* *********************************************begin of ******************************************** */

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
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;

	ret = sensor_read(sd, 0x13, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_g_autoexp!\n");
		return ret;
	}

	val &= 0x01;
	if (val == 0x01) {
		*value = V4L2_EXPOSURE_AUTO;
	}
	else
	{
		*value = V4L2_EXPOSURE_MANUAL;
	}

	info->autoexp = *value;
	return 0;
}

static int sensor_s_autoexp(struct v4l2_subdev *sd,
    enum v4l2_exposure_auto_type value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;

	ret = sensor_read(sd, 0x13, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_s_autoexp!\n");
		return ret;
	}

	switch (value) {
		case V4L2_EXPOSURE_AUTO:
		  val |= 0x01;
			break;
		case V4L2_EXPOSURE_MANUAL:
			val &= 0xfe;
			break;
		case V4L2_EXPOSURE_SHUTTER_PRIORITY:
			return -EINVAL;
		case V4L2_EXPOSURE_APERTURE_PRIORITY:
			return -EINVAL;
		default:
			return -EINVAL;
	}

	ret = sensor_write(sd, 0x13, val);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_autoexp!\n");
		return ret;
	}

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
  int ret;
  struct sensor_info *info = to_state(sd);
  
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
  int ret;
  struct sensor_info *info = to_state(sd);
  
  switch (value)
  {
  case -3: ret = sensor_write_array(sd, sensor_contrast_neg3_regs, ARRAY_SIZE(sensor_contrast_neg3_regs)); break;
  case -2: ret = sensor_write_array(sd, sensor_contrast_neg2_regs, ARRAY_SIZE(sensor_contrast_neg2_regs)); break;
  case -1: ret = sensor_write_array(sd, sensor_contrast_neg1_regs, ARRAY_SIZE(sensor_contrast_neg1_regs)); break;
  case  0: ret = sensor_write_array(sd, sensor_contrast_zero_regs, ARRAY_SIZE(sensor_contrast_zero_regs)); break;
  case  1: ret = sensor_write_array(sd, sensor_contrast_pos1_regs, ARRAY_SIZE(sensor_contrast_pos1_regs)); break;
  case  2: ret = sensor_write_array(sd, sensor_contrast_pos2_regs, ARRAY_SIZE(sensor_contrast_pos2_regs)); break;
  case  3: ret = sensor_write_array(sd, sensor_contrast_pos3_regs, ARRAY_SIZE(sensor_contrast_pos3_regs)); break;
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
  int ret;
  struct sensor_info *info = to_state(sd);
  
  switch (value)
  {
  case -2: ret = sensor_write_array(sd, sensor_saturation_neg2_regs, ARRAY_SIZE(sensor_saturation_neg2_regs)); break;
  case -1: ret = sensor_write_array(sd, sensor_saturation_neg1_regs, ARRAY_SIZE(sensor_saturation_neg1_regs)); break;
  case  0: ret = sensor_write_array(sd, sensor_saturation_zero_regs, ARRAY_SIZE(sensor_saturation_zero_regs)); break;
  case  1: ret = sensor_write_array(sd, sensor_saturation_pos1_regs, ARRAY_SIZE(sensor_saturation_pos1_regs)); break;
  case  2: ret = sensor_write_array(sd, sensor_saturation_pos2_regs, ARRAY_SIZE(sensor_saturation_pos2_regs)); break;
  default: return -EINVAL;
  }
  
  if (ret < 0) { vfe_dev_err("sensor_write_array err at sensor_s_saturation!\n"); return ret;	}
  
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
  case -4: ret = sensor_write_array(sd, sensor_ev_neg4_regs, ARRAY_SIZE(sensor_ev_neg4_regs)); break;
  case -3: ret = sensor_write_array(sd, sensor_ev_neg3_regs, ARRAY_SIZE(sensor_ev_neg3_regs)); break;
  case -2: ret = sensor_write_array(sd, sensor_ev_neg2_regs, ARRAY_SIZE(sensor_ev_neg2_regs)); break;
  case -1: ret = sensor_write_array(sd, sensor_ev_neg1_regs, ARRAY_SIZE(sensor_ev_neg1_regs)); break;
  case 0: ret = sensor_write_array(sd, sensor_ev_zero_regs, ARRAY_SIZE(sensor_ev_zero_regs)); break;
  case 1: ret = sensor_write_array(sd, sensor_ev_pos1_regs, ARRAY_SIZE(sensor_ev_pos1_regs)); break;
  case 2: ret = sensor_write_array(sd, sensor_ev_pos2_regs, ARRAY_SIZE(sensor_ev_pos2_regs)); break;
  case 3: ret = sensor_write_array(sd, sensor_ev_pos3_regs, ARRAY_SIZE(sensor_ev_pos3_regs)); break;
  case 4: ret = sensor_write_array(sd, sensor_ev_pos4_regs, ARRAY_SIZE(sensor_ev_pos4_regs)); break;
  default: return -EINVAL;
  }

  if (ret < 0) { vfe_dev_err("sensor_write_array err at sensor_s_exp!\n"); return ret; }

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
  if (value == V4L2_WHITE_BALANCE_AUTO)
    return sensor_s_autowb(sd, 1);
  else
  {
    ret = sensor_s_autowb(sd, 0);
    if(ret < 0) { vfe_dev_err("sensor_s_autowb error, return %x!\n",ret); return ret; }

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
    
    
    vfe_gpio_write(sd,PWDN,CSI_STBY_ON);
    
    
    vfe_set_mclk(sd,OFF);
    break;
    
  case CSI_SUBDEV_STBY_OFF:
    vfe_dev_dbg("CSI_SUBDEV_STBY_OFF\n");
    
    vfe_set_mclk_freq(sd, MCLK);
    vfe_set_mclk(sd, ON);
    msleep(10);
    
    vfe_gpio_write(sd, PWDN, CSI_STBY_OFF);
    msleep(10);
    
    vfe_dev_print("enable oe\n");
    ret = sensor_write_array(sd, sensor_oe_enable_regs,  ARRAY_SIZE(sensor_oe_enable_regs));
    if(ret < 0) vfe_dev_err("enable oe failed\n");
    
    ret = sensor_write_array(sd, sensor_sw_stby_off_regs ,ARRAY_SIZE(sensor_sw_stby_off_regs));
    if(ret < 0) vfe_dev_err("soft stby off failed\n");
    msleep(10);
    break;
    
  case CSI_SUBDEV_PWR_ON:
    vfe_dev_dbg("CSI_SUBDEV_PWR_ON\n");
    
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
    
    break;
    
  case CSI_SUBDEV_PWR_OFF:
    vfe_dev_dbg("CSI_SUBDEV_PWR_OFF\n");
    
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
  unsigned char rdval;
  
  LOG_ERR_RET(sensor_read(sd, 0x0a, &rdval));
  if (rdval != 0x96) return -ENODEV;
  
  LOG_ERR_RET(sensor_read(sd, 0x0b, &rdval));
  if (rdval != 0x50 && rdval != 0x52 && rdval != 0x53) return -ENODEV;

  vfe_dev_print("Detected Omnivision OV965x camera: ov96%x\n", rdval);

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
  info->tpf.numerator = 1;
  info->tpf.denominator = 30;    /* 30fps */

  vfe_dev_dbg("Writing sensor default regs\n");
  ret = sensor_write_array(sd, sensor_default_regs, ARRAY_SIZE(sensor_default_regs));
  if (ret < 0) { vfe_dev_err("write sensor_default_regs error\n"); return ret; }


  if (info->stby_mode == 0) info->init_first_flag = 0;
  
  info->preview_first_flag = 1;

  return 0;
}

static long sensor_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
  int ret = 0;
  return ret;
}


static struct sensor_format_struct {
    __u8 *desc;
    enum v4l2_mbus_pixelcode mbus_code;
    struct regval_list *regs;
    int regs_size;
    int bpp;   /* Bytes per pixel */
}
  sensor_formats[] =
  {
    {
      .desc   = "YUYV 4:2:2",
      .mbus_code  = V4L2_MBUS_FMT_YUYV8_2X8,
      .regs = sensor_vga_yuv_regs,
      .regs_size = ARRAY_SIZE(sensor_vga_yuv_regs),




      .bpp    = 2,
    },
    /*
    {
      .desc   = "YVYU 4:2:2",
      .mbus_code  = V4L2_MBUS_FMT_YVYU8_2X8,
      .regs = sensor_vga_yuv_regs,
      .regs_size = ARRAY_SIZE(sensor_vga_yuv_regs),
      .bpp    = 2,
    },
    {
      .desc   = "UYVY 4:2:2",
      .mbus_code  = V4L2_MBUS_FMT_UYVY8_2X8,
      .regs = sensor_vga_yuv_regs,
      .regs_size = ARRAY_SIZE(sensor_vga_yuv_regs),
      .bpp    = 2,
    },
    {
      .desc   = "VYUY 4:2:2",
      .mbus_code  = V4L2_MBUS_FMT_VYUY8_2X8,
      .regs = sensor_vga_yuv_regs,
      .regs_size = ARRAY_SIZE(sensor_vga_yuv_regs),
      .bpp    = 2,
    },
    */
  };
#define N_FMTS ARRAY_SIZE(sensor_formats)


static struct sensor_win_size sensor_win_sizes[] = {
  /* SXGA */
  {
    .width      = SXGA_WIDTH,
    .height     = SXGA_HEIGHT,
    .regs       = sensor_sxga_regs,
    .regs_size  = ARRAY_SIZE(sensor_sxga_regs),
    .set_size   = NULL,
  },
  /* SVGA */
  /*  {
    .width      = SVGA_WIDTH,
    .height     = SVGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = sensor_svga_regs,
    .regs_size  = ARRAY_SIZE(sensor_svga_regs),
    .set_size   = NULL,
    },*/
  /* VGA */
  {
    .width      = VGA_WIDTH,
    .height     = VGA_HEIGHT,
    .regs       = sensor_vga_yuv_regs,
    .regs_size  = ARRAY_SIZE(sensor_vga_yuv_regs),
    .set_size   = NULL,
  },
};

#define N_WIN_SIZES (ARRAY_SIZE(sensor_win_sizes))

static int sensor_enum_fmt(struct v4l2_subdev *sd, unsigned index, enum v4l2_mbus_pixelcode *code)
{
  if (index >= N_FMTS) return -EINVAL;

  *code = sensor_formats[index].mbus_code;
  return 0;
}

static int sensor_try_fmt_internal(struct v4l2_subdev *sd,
                                   struct v4l2_mbus_framefmt *fmt,
                                   struct sensor_format_struct **ret_fmt,
                                   struct sensor_win_size **ret_wsize)
{
  int index;
  struct sensor_win_size *wsize;

  printk("OV9650 sensor_try_fmt_internal called with fmt code 0x%x\n", fmt->code);
  
  for (index = 0; index < N_FMTS; index++) if (sensor_formats[index].mbus_code == fmt->code) break;
  
  if (index >= N_FMTS) {
    vfe_dev_dbg("sensor_try_fmt_internal: v4l2_mbus_framefmt code %x not found\n", fmt->code);
    return -1;
  }
  
  if (ret_fmt != NULL) *ret_fmt = sensor_formats + index;
  
  fmt->field = V4L2_FIELD_NONE;
  
  for (wsize = sensor_win_sizes; wsize < sensor_win_sizes + N_WIN_SIZES; wsize++)
    if (fmt->width >= wsize->width && fmt->height >= wsize->height) break;
  
  if (wsize >= sensor_win_sizes + N_WIN_SIZES) wsize--;
  if (ret_wsize != NULL) *ret_wsize = wsize;
  
  fmt->width = wsize->width;
  fmt->height = wsize->height;
  
  return 0;
}

static int sensor_try_fmt(struct v4l2_subdev *sd, struct v4l2_mbus_framefmt *fmt)
{
  return sensor_try_fmt_internal(sd, fmt, NULL, NULL);
}

static int sensor_g_mbus_config(struct v4l2_subdev *sd, struct v4l2_mbus_config *cfg)
{
  cfg->type = V4L2_MBUS_PARALLEL;
  cfg->flags = V4L2_MBUS_MASTER | VREF_POL | HREF_POL | CLK_POL ;

  return 0;
}

static int sensor_s_fmt(struct v4l2_subdev *sd, struct v4l2_mbus_framefmt *fmt)
{
  int ret;
  struct sensor_format_struct *sensor_fmt;
  struct sensor_win_size *wsize;
  struct sensor_info *info = to_state(sd);

  ret = sensor_try_fmt_internal(sd, fmt, &sensor_fmt, &wsize);
  if (ret) return ret;
  
  vfe_dev_dbg("sensor_s_fmt: using mbus_code = %x. Writing format setup registers.\n", sensor_fmt->mbus_code);
  sensor_write_array(sd, sensor_fmt->regs, sensor_fmt->regs_size);
  
  vfe_dev_dbg("sensor_s_fmt: using wsize->width=%d, wsize->height=%d\n" ,wsize->width,wsize->height);
  if (wsize->regs)
  {
    vfe_dev_dbg("sensor_s_fmt: writing window setup registers\n");
    ret = sensor_write_array(sd, wsize->regs , wsize->regs_size);
    if (ret < 0) return ret;
  }
  
  if (wsize->set_size) { ret = wsize->set_size(sd); if (ret < 0) return ret; }
  
  info->fmt = sensor_fmt;
  info->width = wsize->width;
  info->height = wsize->height;
  
  return 0;
}

/*
 * Implement G/S_PARM.  There is a "high quality" mode we could try
 * to do someday; for now, we just do the frame rate tweak.
 */
static int sensor_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
  struct v4l2_captureparm *cp = &parms->parm.capture;
  
  if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) return -EINVAL;
  
  memset(cp, 0, sizeof(struct v4l2_captureparm));
  cp->capability = V4L2_CAP_TIMEPERFRAME;
  cp->timeperframe.numerator = 1;
  cp->timeperframe.denominator = SENSOR_FRAME_RATE;
  
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


/*
 * Code for dealing with controls.
 * fill with different sensor module
 * different sensor module has different settings here
 * if not support the follow function ,retrun -EINVAL
 */
static int sensor_queryctrl(struct v4l2_subdev *sd, struct v4l2_queryctrl *qc)
{
  /* Fill in min, max, step and default value for these controls. */
  /* see include/linux/videodev2.h for details */
  /* see sensor_s_parm and sensor_g_parm for the meaning of value */
  switch (qc->id)
  {
  case V4L2_CID_BRIGHTNESS: return v4l2_ctrl_query_fill(qc, -3, 3, 1, 1);
    
  case V4L2_CID_CONTRAST: return v4l2_ctrl_query_fill(qc, -3, 3, 1, 1);
    

  case V4L2_CID_VFLIP:
  case V4L2_CID_HFLIP: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_EXPOSURE:
  case V4L2_CID_AUTO_EXPOSURE_BIAS: return v4l2_ctrl_query_fill(qc, -4, 4, 1, 0);
  case V4L2_CID_EXPOSURE_AUTO: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_DO_WHITE_BALANCE:
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 9, 1, 0);
  case V4L2_CID_AUTO_WHITE_BALANCE: return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
  default: return -EINVAL;
  }
}

static int sensor_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
  struct sensor_info *info = to_state(sd);
  unsigned char val;
  int ret;

  vfe_dev_dbg("sensor_g_ctrl id=0x%8x\n", ctrl->id);

  switch (ctrl->id)
  {
  case V4L2_CID_BRIGHTNESS:
    return sensor_g_brightness(sd, &ctrl->value);
    
  case V4L2_CID_CONTRAST:
    return sensor_g_contrast(sd, &ctrl->value);
    
  case V4L2_CID_SATURATION:
    return sensor_g_saturation(sd, &ctrl->value);
    
  case V4L2_CID_HUE:
    return sensor_g_hue(sd, &ctrl->value);
    
  case V4L2_CID_VFLIP:
    SENSOR_READ(0x1e);
    ctrl->value = (val >> 4) & 1;
    info->vflip = ctrl->value;
    return 0;
    
  case V4L2_CID_HFLIP:
    SENSOR_READ(0x1e);
    ctrl->value = (val >> 5) & 1;
    info->hflip = ctrl->value;
    return 0;
    
  case V4L2_CID_GAIN:
    return sensor_g_gain(sd, &ctrl->value);
    
  case V4L2_CID_AUTOGAIN:
    return sensor_g_autogain(sd, &ctrl->value);

  case V4L2_CID_EXPOSURE:
  case V4L2_CID_AUTO_EXPOSURE_BIAS:
    return sensor_g_exp_bias(sd, &ctrl->value);

  case V4L2_CID_EXPOSURE_AUTO:
    return sensor_g_autoexp(sd, &ctrl->value);

  case V4L2_CID_DO_WHITE_BALANCE:
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
    return sensor_g_wb(sd, &ctrl->value);

  case V4L2_CID_AUTO_WHITE_BALANCE:
    return sensor_g_autowb(sd, &ctrl->value);
  }
  return -EINVAL;
}

static int sensor_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
  struct v4l2_queryctrl qc;
  struct sensor_info *info = to_state(sd);
  unsigned char val;
  int ret;

  vfe_dev_dbg("sensor_s_ctrl id=0x%8x\n", ctrl->id);

  qc.id = ctrl->id;
  ret = sensor_queryctrl(sd, &qc);
  if (ret < 0) return ret;

  if (qc.type == V4L2_CTRL_TYPE_MENU || qc.type == V4L2_CTRL_TYPE_INTEGER || qc.type == V4L2_CTRL_TYPE_BOOLEAN)
  {
    if (ctrl->value < qc.minimum || ctrl->value > qc.maximum) return -ERANGE;
  }

  switch (ctrl->id)
  {
  case V4L2_CID_BRIGHTNESS:
    return sensor_s_brightness(sd, ctrl->value);
    
  case V4L2_CID_CONTRAST:
    return sensor_s_contrast(sd, ctrl->value);
    
  case V4L2_CID_SATURATION:
    return sensor_s_saturation(sd, ctrl->value);
    
  case V4L2_CID_HUE:
    return sensor_s_hue(sd, ctrl->value);
    
  case V4L2_CID_VFLIP:
    SENSOR_READ(0x1e);
    switch (ctrl->value)
    {
    case 0: val |= (1 << 4); break;
    case 1: val &= ~(1 << 4); break;
    default: return -EINVAL;
    }
    SENSOR_WRITE(0x1e, val);
    msleep(50);
    info->vflip = ctrl->value;
    return 0;
    
  case V4L2_CID_HFLIP:
    SENSOR_READ(0x1e);
    switch (ctrl->value)
    {
    case 0: val |= (1 << 5); break;
    case 1: val &= ~(1 << 5); break;
    default: return -EINVAL;
    }
    SENSOR_WRITE(0x1e, val);
    msleep(50);
    info->hflip = ctrl->value;
    return 0;
    
  case V4L2_CID_GAIN:
    return sensor_s_gain(sd, ctrl->value);

  case V4L2_CID_AUTOGAIN:
    return sensor_s_autogain(sd, ctrl->value);

  case V4L2_CID_EXPOSURE:
  case V4L2_CID_AUTO_EXPOSURE_BIAS:
    return sensor_s_exp_bias(sd, ctrl->value);

  case V4L2_CID_EXPOSURE_AUTO:
    return sensor_s_autoexp(sd, (enum v4l2_exposure_auto_type) ctrl->value);

  case V4L2_CID_DO_WHITE_BALANCE:
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
    return sensor_s_wb(sd, (enum v4l2_auto_n_preset_white_balance) ctrl->value);

  case V4L2_CID_AUTO_WHITE_BALANCE:
    return sensor_s_autowb(sd, ctrl->value);
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

static int sensor_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  struct v4l2_subdev *sd;
  struct sensor_info *info;

  info = kzalloc(sizeof(struct sensor_info), GFP_KERNEL);
  if (info == NULL) return -ENOMEM;

  sd = &info->sd;
  v4l2_i2c_subdev_init(sd, client, &sensor_ops);

  info->fmt = &sensor_formats[0];
  info->af_first_flag = 1;
  info->init_first_flag = 1;
  info->auto_focus = 0;

  return 0;
}

static int sensor_remove(struct i2c_client *client)
{
  struct v4l2_subdev *sd = i2c_get_clientdata(client);

  v4l2_device_unregister_subdev(sd);
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
  return i2c_add_driver(&sensor_driver);
}

static __exit void exit_sensor(void)
{
  i2c_del_driver(&sensor_driver);
}

module_init(init_sensor);
module_exit(exit_sensor);

