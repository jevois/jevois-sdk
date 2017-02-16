/*
* A V4L2 driver for Superpix  sp2519 cameras.
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


MODULE_AUTHOR ("raymonxiu");
MODULE_DESCRIPTION ("A low-level driver for Superpix  SP2519 sensors");
MODULE_LICENSE ("GPL");



#define DEV_DBG_EN      1
#if(DEV_DBG_EN == 1)
#define vfe_dev_dbg(x,arg...) printk("[CSI_DEBUG][SP2519]"x,##arg)
#else
#define vfe_dev_dbg(x,arg...)
#endif

#define vfe_dev_err(x,arg...) printk("[CSI_ERR][SP2519]"x,##arg)
#define vfe_dev_print(x,arg...) printk("[CSI][SP2519]"x,##arg)

#define LOG_ERR_RET(x)  { \
    int ret;  \
    ret = x; \
    if(ret < 0) {\
      vfe_dev_err("error at %s\n",__func__);  \
      return ret; \
    } \
  }

#define MCLK              (24*1000*1000)
#define VREF_POL          V4L2_MBUS_VSYNC_ACTIVE_HIGH
#define HREF_POL          V4L2_MBUS_HSYNC_ACTIVE_HIGH
#define CLK_POL           V4L2_MBUS_PCLK_SAMPLE_RISING
#define V4L2_IDENT_SENSOR  0x2519

#define CSI_STBY_ON     1
#define CSI_STBY_OFF            0
#define CSI_RST_ON      0
#define CSI_RST_OFF     1
#define CSI_PWR_ON      1
#define CSI_PWR_OFF     0


#define regval_list reg_list_a8_d8
#define REG_DLY  0xff

/*
 * Our nominal (default) frame rate.
 */
#define SENSOR_FRAME_RATE  6

/*
 * The sp2519 sits on i2c with ID 0x60
 */
#define I2C_ADDR      0x60
#define SENSOR_NAME "sp2519"
#define  SP2519_P0_0xde  0x95
#define  SP2519_P0_0xd4  0x77
#define  SP2519_P0_0xd9  0x77
#define SP2519_NORMAL_Y0ffset  0x10
#define SP2519_LOWLIGHT_Y0ffset  0x20

/*
 * Information we maintain about a known sensor.
 */
struct sensor_format_struct;  /* coming later */

struct cfg_array { /* coming later */
  struct regval_list * regs;
  int size;
};

static inline struct sensor_info * to_state (struct v4l2_subdev * sd)
{
  return container_of (sd, struct sensor_info, sd);
}



/*
 * The default register settings
 *
 */

static struct regval_list sensor_default_regs[] = {

  {0xfd, 0x01},
  {0x36, 0x02},
  {0xfd, 0x00},
  {0x0c, 0x55},
  {0x30, 0x00},
  {0x2f, 0x09},
  {0x11, 0x40},
  {0x18, 0x00},
  {0x1a, 0x49},
  {0x1c, 0x1f},
  {0x1d, 0x01},
  {0x1e, 0x01},
  {0x2e, 0x80},
  {0x20, 0x2f},
  {0x21, 0x10},
  {0x22, 0x2a},
  {0x25, 0xad},
  {0x27, 0xa1},
  {0x1f, 0xc0},
  {0x28, 0x0b},
  {0x2b, 0x8c},
  {0x26, 0x09},
  {0x2c, 0x45},
  {0x37, 0x00},
  {0x16, 0x01},
  {0x17, 0x2f},
  
  {0x69, 0x01},
  {0x6a, 0x2d},
  {0x13, 0x4f},
  {0x6b, 0x50},
  {0x6c, 0x50},
  {0x6f, 0x50},
  {0x73, 0x51},
  
  {0x7a, 0x41},
  {0x70, 0x41},
  {0x7d, 0x40},
  {0x74, 0x40},
  {0x75, 0x40},
  {0x14, 0x01},
  {0x15, 0x20},
  {0x71, 0x22},
  {0x76, 0x22},
  {0x7c, 0x22},
  
  {0x7e, 0x21},
  {0x72, 0x21},
  {0x77, 0x20},
  
  {0xfd, 0x00},
  
  {0x92, 0x00},
  
  {0xfd, 0x01},
  {0x32, 0x00},
  {0xfb, 0x21},
  {0xfd, 0x02},
  {0x85, 0x00},
  {0x00, 0x82},
  {0x01, 0x82},
  
  {0xfd, 0x00},
  {0x03, 0x02},
  {0x04, 0xa0},
  {0x05, 0x00},
  {0x06, 0x00},
  {0x07, 0x00},
  {0x08, 0x00},
  {0x09, 0x00},
  {0x0a, 0x95},
  {0xfd, 0x01},
  {0xf0, 0x00},
  {0xf7, 0x70},
  {0xf8, 0x5d},
  {0x02, 0x0b},
  {0x03, 0x01},
  {0x06, 0x70},
  {0x07, 0x00},
  {0x08, 0x01},
  {0x09, 0x00},
  {0xfd, 0x02},
  {0x3d, 0x0d},
  {0x3e, 0x5d},
  {0x3f, 0x00},
  {0x88, 0x92},
  {0x89, 0x81},
  {0x8a, 0x54},
  {0xfd, 0x02},
  {0xbe, 0xd0},
  {0xbf, 0x04},
  {0xd0, 0xd0},
  {0xd1, 0x04},
  {0xc9, 0xd0},
  {0xca, 0x04},
  
  {0xb8, 0x90},
  {0xba, 0x30},
  {0xbb, 0x45},
  {0xbc, 0x90},
  {0xbd, 0x50},
  {0xfd, 0x03},
  {0x77, 0x50}, 
  
  {0xfd, 0x01},
  {0xe0, 0x48},
  {0xe1, 0x38},
  {0xe2, 0x30},
  {0xe3, 0x2c},
  {0xe4, 0x2c},
  {0xe5, 0x2a},
  {0xe6, 0x2a},
  {0xe7, 0x28},
  {0xe8, 0x28},
  {0xe9, 0x28},
  {0xea, 0x26},
  {0xf3, 0x26},
  {0xf4, 0x26},
  {0xfd, 0x01},
  {0x04, 0xa0},
  {0x05, 0x26},
  {0x0a, 0x48},
  {0x0b, 0x26},
  
  {0xfd, 0x01},
  {0xeb, 0x7a},
  {0xec, 0x7a},
  {0xed, 0x04},
  {0xee, 0x08},
  
  
  {0xfd, 0x03},
  {0x52, 0xff},
  {0x53, 0x60},
  {0x94, 0x20},
  {0x54, 0x00},
  {0x55, 0x00},
  
  {0x56, 0x80},
  {0x57, 0x80},
  {0x95, 0x80},
  {0x58, 0x00},
  {0x59, 0x00},
  
  {0x5a, 0xf6},
  {0x5b, 0x00},
  {0x5c, 0x88},
  {0x5d, 0x00},
  {0x96, 0x68},
  
  {0xfd, 0x03},
  {0x8a, 0x00},
  {0x8b, 0x00},
  {0x8c, 0xff},
  
  {0x22, 0xff},
  {0x23, 0xff},
  {0x24, 0xff},
  {0x25, 0xff},
  
  {0x5e, 0xff},
  {0x5f, 0xff},
  {0x60, 0xff},
  {0x61, 0xff},
  {0x62, 0x00},
  {0x63, 0x00},
  {0x64, 0x00},
  {0x65, 0x00},
  
  
  {0xfd, 0x01},
  {0x21, 0x00},
  {0x22, 0x00},
  {0x26, 0xa0},
  {0x27, 0x14},
  {0x28, 0x05},
  {0x29, 0x20},
  {0x2a, 0x01},
  
  {0xfd, 0x01},
  {0xa1, 0x15},
  {0xa2, 0x15},
  {0xa3, 0x15},
  {0xa4, 0x15},
  {0xa5, 0x12},
  {0xa6, 0x14},
  {0xa7, 0x0e},
  {0xa8, 0x0d},
  {0xa9, 0x11},
  {0xaa, 0x16},
  {0xab, 0x0d},
  {0xac, 0x0b},
  {0xad, 0x13},
  {0xae, 0x08},
  {0xaf, 0x05},
  {0xb0, 0x04},
  {0xb1, 0x10},
  {0xb2, 0x0a},
  {0xb3, 0x05},
  {0xb4, 0x07},
  {0xb5, 0x10},
  {0xb6, 0x0a},
  {0xb7, 0x07},
  {0xb8, 0x07},
  
  
  {0xfd, 0x02},
  {0x26, 0xac},
  {0x27, 0x91},
  {0x28, 0xcc},
  {0x29, 0x01},
  {0x2a, 0x02},
  {0x2b, 0x16},
  {0x2c, 0x20},
  {0x2d, 0xdc},
  {0x2e, 0x20},
  {0x2f, 0x96},
  {0x1b, 0x80},
  {0x1a, 0x80},
  {0x18, 0x16},
  {0x19, 0x26},
  
  
  {0x66, 0x35},
  {0x67, 0x5f},
  {0x68, 0xb5},
  {0x69, 0xdb},
  {0x6a, 0xa5},
  
  
  {0x7c, 0x33},
  {0x7d, 0x58},
  {0x7e, 0xe7},
  {0x7f, 0x0c},
  {0x80, 0xa6},
  
  
  {0x70, 0x20},
  {0x71, 0x45},
  {0x72, 0x08},
  {0x73, 0x2a},
  {0x74, 0xaa},
  
  
  {0x6b, 0x00},
  {0x6c, 0x20},
  {0x6d, 0x12},
  {0x6e, 0x36},
  {0x6f, 0xaa},
  
  
  {0x61, 0xeb},
  {0x62, 0x13},
  {0x63, 0x29},
  {0x64, 0x4e},
  {0x65, 0x6a},
  
  {0x75, 0x00},
  {0x76, 0x09},
  {0x77, 0x02},
  {0x0e, 0x16},
  {0x3b, 0x09},
  {0xfd, 0x02},
  {0x02, 0x00},
  {0x03, 0x10},
  {0x04, 0xf0},
  {0xf5, 0xb3},
  {0xf6, 0x80},
  {0xf7, 0xe0},
  {0xf8, 0x89},
  
  
  {0xfd, 0x02},
  {0x08, 0x00},
  {0x09, 0x04},
  
  {0xfd, 0x02},
  {0xdd, 0x0f},
  {0xde, 0x0f},
  
  {0xfd, 0x02},
  {0x57, 0x1a},
  {0x58, 0x10},
  {0x59, 0xe0},
  {0x5a, 0x20},
  {0x5b, 0x20},
  
  {0xcb, 0x10},
  {0xcc, 0x10},
  {0xcd, 0x28},
  {0xce, 0x80},
  
  {0xfd, 0x03},
  {0x87, 0x0a},    
  {0x88, 0x14},    
  {0x89, 0x18},    
  
  {0xfd, 0x02},
  {0xe8, 0x30}, 
  {0xec, 0x40}, 
  {0xe9, 0x30}, 
  {0xed, 0x40}, 
  {0xea, 0x28}, 
  {0xee, 0x38}, 
  {0xeb, 0x20}, 
  {0xef, 0x30}, 
  
  {0xfd, 0x02},  
  {0xdc, 0x03},  
  {0x05, 0x00},  
  
  
  
  {0xfd, 0x02},
  {0xf4, 0x30},
  {0xfd, 0x03},
  {0x97, 0x80},
  {0x98, 0x80},
  {0x99, 0x80},
  {0x9a, 0x80},
  {0xfd, 0x02},
  {0xe4, 0xff},
  {0xe5, 0xff},
  {0xe6, 0xff},
  {0xe7, 0xff},
  
  {0xfd, 0x03},
  {0x72, 0x00},
  {0x73, 0x2d},
  {0x74, 0x2d},
  {0x75, 0x2d},
  
  
  {0xfd, 0x02},
  {0x78, 0x20},
  {0x79, 0x20},
  {0x7a, 0x14},
  {0x7b, 0x08},
  
  
  {0x81, 0x40},
  {0x82, 0x40},
  {0x83, 0x50},
  {0x84, 0x50},
  
  {0xfd, 0x03},
  {0x7e, 0x10},   
  {0x7f, 0x18},   
  {0x80, 0x20},   
  {0x81, 0x30},   
  {0x7c, 0xff},   
  {0x82, 0x44},   
  {0x83, 0x22},   
  {0x84, 0x08},    
  {0x85, 0x40},    
  {0x86, 0x80},  
  
  
  {0xfd, 0x03},
  {0x66, 0x18},
  {0x67, 0x28},
  {0x68, 0x20},
  {0x69, 0x88},
  {0x9b, 0x18},
  {0x9c, 0x28},
  {0x9d, 0x20},
  
  
  {0xfd, 0x01},
  {0x8b, 0x00},
  {0x8c, 0x0f},
  {0x8d, 0x21},
  {0x8e, 0x2c},
  {0x8f, 0x37},
  {0x90, 0x46},
  {0x91, 0x53},
  {0x92, 0x5e},
  {0x93, 0x6a},
  {0x94, 0x7d},
  {0x95, 0x8d},
  {0x96, 0x9e},
  {0x97, 0xac},
  {0x98, 0xba},
  {0x99, 0xc6},
  {0x9a, 0xd1},
  {0x9b, 0xda},
  {0x9c, 0xe4},
  {0x9d, 0xeb},
  {0x9e, 0xf2},
  {0x9f, 0xf9},
  {0xa0, 0xff},
  
  
  
  {0xfd, 0x02},
  {0x15, 0xC8},
  {0x16, 0x95},
  
  {0xa0, 0x80},
  {0xa1, 0xf4},
  {0xa2, 0x0c},
  {0xa3, 0xe7},
  {0xa4, 0x99},
  {0xa5, 0x00},
  {0xa6, 0xf4},
  {0xa7, 0xda},
  {0xa8, 0xb3},
  {0xa9, 0x0c},
  {0xaa, 0x03},
  {0xab, 0x0f},
  
  {0xac, 0x99},
  {0xad, 0xf4},
  {0xae, 0xf4},
  {0xaf, 0xf4},
  {0xb0, 0x99},
  {0xb1, 0xf4},
  {0xb2, 0xf4},
  {0xb3, 0xda},
  {0xb4, 0xb3},
  {0xb5, 0x3c},
  {0xb6, 0x33},
  {0xb7, 0x0f},
  
  {0xfd, 0x01},  
  {0xd2, 0x2f},  
  {0xd1, 0x38},  
  {0xdd, 0x1b},
  {0xde, 0x1b},
  
  
  
  {0xfd, 0x02},
  {0xc1, 0x40},
  {0xc2, 0x40},
  {0xc3, 0x40},
  {0xc4, 0x40},
  {0xc5, 0x80},
  {0xc6, 0x80},
  {0xc7, 0x80},
  {0xc8, 0x80},
  
  
  {0xfd, 0x01},
  {0xd3, 0x70},
  {0xd4, 0x70},
  {0xd5, 0x70},
  {0xd6, 0x70},
  
  {0xd7, 0x70},
  {0xd8, 0x70},
  {0xd9, 0x70},
  {0xda, 0x70},
  
  {0xfd, 0x03},
  {0x76, 0x10},
  {0x7a, 0x40},
  {0x7b, 0x40},
  
  {0xfd, 0x01},
  {0xc2, 0xff},
  {0xc3, 0xff},
  {0xc4, 0xaa},
  {0xc5, 0xaa},
  
  
  
  {0xfd, 0x01},
  {0xcd, 0x09},
  {0xce, 0x10},
  
  
  {0xfd, 0x02},
  {0x35, 0x6f},
  {0x37, 0x13},
  
  
  
  {0xfd, 0x01},
  {0xdb, 0x00},
  
  {0x10, 0x80},
  {0x11, 0x80},
  {0x12, 0x80},
  {0x13, 0x80},
  {0x14, 0xa0},
  {0x15, 0xa0},
  {0x16, 0xa0},
  {0x17, 0xa0},
  
  {0xfd, 0x03},
  {0x00, 0x80},
  {0x03, 0x30},
  {0x06, 0xf0},
  {0x07, 0x08},
  {0x0a, 0x00},
  {0x01, 0x00},
  {0x02, 0x00},
  {0x04, 0x00},
  {0x05, 0x00},
  {0x0b, 0x00},
  {0x0c, 0x00},
  {0x0d, 0x00},
  {0x0e, 0x00},
  {0x08, 0x10},
  {0x09, 0x10},
  
  {0xfd, 0x02},
  {0x8e, 0x0a},
  {0x8f, 0x03},
  {0x90, 0x40},
  {0x91, 0x40},
  {0x92, 0x60},
  {0x93, 0x80},
  {0x94, 0x80},
  {0x95, 0x80},
  {0x96, 0x80},
  {0x97, 0x80},
  
  {0x98, 0x80},
  {0x99, 0x80},
  {0x9a, 0x80},
  {0x9b, 0x80},
  
  {0x9e, 0x44},
  {0x9f, 0x44},
  
  {0xfd, 0x02},
  {0x85, 0x00},
  {0xfd, 0x01},
  {0x00, 0x00},
  {0x32, 0x15},
  {0x33, 0xef},
  {0x34, 0xef},  
  {0x35, 0x40},  
  {0xfd, 0x00},
  {0x31, 0x00},  
  {0x3f, 0x00},
  
  {0xfd, 0x01},
  {0x50, 0x00}, 
  {0x66, 0x00},  
  {0xfd, 0x02},
  {0xd6, 0x0f},
  {0xfd, 0x00},
  {0x1b, 0x30},
  {0xfd, 0x01},
  {0x36, 0x00},
};

static struct regval_list sensor_uxga_regs[] = {
  {0xfd, 0x00},
  {0x19, 0x00},
  {0x30, 0x00},
  {0x31, 0x00},
  {0x33, 0x00},
  {0xfd, 0x00},
  {0x95, 0x06},
  {0x94, 0x40},
  {0x97, 0x04},
  {0x96, 0xb0},
};

static struct regval_list sensor_720p_regs[] = {

  {0xfd, 0x00},
  
};

static struct regval_list sensor_svga_regs[] = {

  {0xfd, 0x00},
  {0x19, 0x03},
  {0x30, 0x00},
  {0x31, 0x04},
  {0x33, 0x01},
  
  {0xfd, 0x00},
  {0x95, 0x03},
  {0x94, 0x20},
  {0x97, 0x02},
  {0x96, 0x58},
  
};

static struct regval_list sensor_vga_regs[] = {
  {0xfd, 0x00},
  
};

/*
 * The white balance settings
 * Here only tune the R G B channel gain.
 * The white balance enalbe bit is modified in sensor_s_autowb and sensor_s_wb
 */
static struct regval_list sensor_wb_manual[] = {
};

static struct regval_list sensor_wb_auto_regs[] = {
  {0xfd, 0x02},
  {0x26, 0xac},
  {0x27, 0x91},
  {0xfd, 0x01},
  {0x32, 0x15},
  {0xfd, 0x00},
};


static struct regval_list sensor_wb_incandescence_regs[] = {
  {0xfd, 0x01},
  {0x32, 0x05},
  {0xfd, 0x02},
  {0x26, 0x7b},
  {0x27, 0xd3},
  {0xfd, 0x00},
};

static struct regval_list sensor_wb_fluorescent_regs[] = {
  {0xfd, 0x01},
  {0x32, 0x05},
  {0xfd, 0x02},
  {0x26, 0xb4},
  {0x27, 0xc4},
  {0xfd, 0x00},
};

static struct regval_list sensor_wb_tungsten_regs[] = {
  {0xfd, 0x01},
  {0x32, 0x05},
  {0xfd, 0x02},
  {0x26, 0xae},
  {0x27, 0xcc},
  {0xfd, 0x00},
};

static struct regval_list sensor_wb_horizon[] = {
};

static struct regval_list sensor_wb_daylight_regs[] = {
  {0xfd, 0x01},
  {0x32, 0x05},
  {0xfd, 0x02},
  {0x26, 0xc1},
  {0x27, 0x88},
  {0xfd, 0x00},
};

static struct regval_list sensor_wb_flash[] = {
};

static struct regval_list sensor_wb_cloud_regs[] = {
  {0xfd, 0x01},
  {0x32, 0x05},
  {0xfd, 0x02},
  {0x26, 0xe2},
  {0x27, 0x82},
  {0xfd, 0x00},
};

static struct regval_list sensor_wb_shade[] = {
};

static struct cfg_array sensor_wb[] = {
  {
    .regs = sensor_wb_manual,            
    .size = ARRAY_SIZE (sensor_wb_manual),
  },
  {
    .regs = sensor_wb_auto_regs,         
    .size = ARRAY_SIZE (sensor_wb_auto_regs),
  },
  {
    .regs = sensor_wb_incandescence_regs,
    .size = ARRAY_SIZE (sensor_wb_incandescence_regs),
  },
  {
    .regs = sensor_wb_fluorescent_regs,  
    .size = ARRAY_SIZE (sensor_wb_fluorescent_regs),
  },
  {
    .regs = sensor_wb_tungsten_regs,     
    .size = ARRAY_SIZE (sensor_wb_tungsten_regs),
  },
  {
    .regs = sensor_wb_horizon,           
    .size = ARRAY_SIZE (sensor_wb_horizon),
  },
  {
    .regs = sensor_wb_daylight_regs,     
    .size = ARRAY_SIZE (sensor_wb_daylight_regs),
  },
  {
    .regs = sensor_wb_flash,             
    .size = ARRAY_SIZE (sensor_wb_flash),
  },
  {
    .regs = sensor_wb_cloud_regs,        
    .size = ARRAY_SIZE (sensor_wb_cloud_regs),
  },
  {
    .regs = sensor_wb_shade,             
    .size = ARRAY_SIZE (sensor_wb_shade),
  },
};


/*
 * The color effect settings
 */
static struct regval_list sensor_colorfx_none_regs[] = {

};

static struct regval_list sensor_colorfx_bw_regs[] = {

};

static struct regval_list sensor_colorfx_sepia_regs[] = {

};

static struct regval_list sensor_colorfx_negative_regs[] = {

};

static struct regval_list sensor_colorfx_emboss_regs[] = {

};

static struct regval_list sensor_colorfx_sketch_regs[] = {

};

static struct regval_list sensor_colorfx_sky_blue_regs[] = {

};

static struct regval_list sensor_colorfx_grass_green_regs[] = {

};

static struct regval_list sensor_colorfx_skin_whiten_regs[] = {
};

static struct regval_list sensor_colorfx_vivid_regs[] = {
};

static struct regval_list sensor_colorfx_aqua_regs[] = {
};

static struct regval_list sensor_colorfx_art_freeze_regs[] = {
};

static struct regval_list sensor_colorfx_silhouette_regs[] = {
};

static struct regval_list sensor_colorfx_solarization_regs[] = {
};

static struct regval_list sensor_colorfx_antique_regs[] = {
};

static struct regval_list sensor_colorfx_set_cbcr_regs[] = {
};

static struct cfg_array sensor_colorfx[] = {
  {
    .regs = sensor_colorfx_none_regs,        
    .size = ARRAY_SIZE (sensor_colorfx_none_regs),
  },
  {
    .regs = sensor_colorfx_bw_regs,          
    .size = ARRAY_SIZE (sensor_colorfx_bw_regs),
  },
  {
    .regs = sensor_colorfx_sepia_regs,       
    .size = ARRAY_SIZE (sensor_colorfx_sepia_regs),
  },
  {
    .regs = sensor_colorfx_negative_regs,    
    .size = ARRAY_SIZE (sensor_colorfx_negative_regs),
  },
  {
    .regs = sensor_colorfx_emboss_regs,      
    .size = ARRAY_SIZE (sensor_colorfx_emboss_regs),
  },
  {
    .regs = sensor_colorfx_sketch_regs,      
    .size = ARRAY_SIZE (sensor_colorfx_sketch_regs),
  },
  {
    .regs = sensor_colorfx_sky_blue_regs,    
    .size = ARRAY_SIZE (sensor_colorfx_sky_blue_regs),
  },
  {
    .regs = sensor_colorfx_grass_green_regs, 
    .size = ARRAY_SIZE (sensor_colorfx_grass_green_regs),
  },
  {
    .regs = sensor_colorfx_skin_whiten_regs, 
    .size = ARRAY_SIZE (sensor_colorfx_skin_whiten_regs),
  },
  {
    .regs = sensor_colorfx_vivid_regs,       
    .size = ARRAY_SIZE (sensor_colorfx_vivid_regs),
  },
  {
    .regs = sensor_colorfx_aqua_regs,        
    .size = ARRAY_SIZE (sensor_colorfx_aqua_regs),
  },
  {
    .regs = sensor_colorfx_art_freeze_regs,  
    .size = ARRAY_SIZE (sensor_colorfx_art_freeze_regs),
  },
  {
    .regs = sensor_colorfx_silhouette_regs,  
    .size = ARRAY_SIZE (sensor_colorfx_silhouette_regs),
  },
  {
    .regs = sensor_colorfx_solarization_regs,
    .size = ARRAY_SIZE (sensor_colorfx_solarization_regs),
  },
  {
    .regs = sensor_colorfx_antique_regs,     
    .size = ARRAY_SIZE (sensor_colorfx_antique_regs),
  },
  {
    .regs = sensor_colorfx_set_cbcr_regs,    
    .size = ARRAY_SIZE (sensor_colorfx_set_cbcr_regs),
  },
};



/*
 * The brightness setttings
 */
static struct regval_list sensor_brightness_neg4_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0xc0},
};

static struct regval_list sensor_brightness_neg3_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0xd0},
};

static struct regval_list sensor_brightness_neg2_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0xe0},
};

static struct regval_list sensor_brightness_neg1_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0xf0},
};

static struct regval_list sensor_brightness_zero_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0x00},
};

static struct regval_list sensor_brightness_pos1_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0x10},
};

static struct regval_list sensor_brightness_pos2_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0x20},
};

static struct regval_list sensor_brightness_pos3_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0x30},
};

static struct regval_list sensor_brightness_pos4_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0x40},
};

static struct cfg_array sensor_brightness[] = {
  {
    .regs = sensor_brightness_neg4_regs,
    .size = ARRAY_SIZE (sensor_brightness_neg4_regs),
  },
  {
    .regs = sensor_brightness_neg3_regs,
    .size = ARRAY_SIZE (sensor_brightness_neg3_regs),
  },
  {
    .regs = sensor_brightness_neg2_regs,
    .size = ARRAY_SIZE (sensor_brightness_neg2_regs),
  },
  {
    .regs = sensor_brightness_neg1_regs,
    .size = ARRAY_SIZE (sensor_brightness_neg1_regs),
  },
  {
    .regs = sensor_brightness_zero_regs,
    .size = ARRAY_SIZE (sensor_brightness_zero_regs),
  },
  {
    .regs = sensor_brightness_pos1_regs,
    .size = ARRAY_SIZE (sensor_brightness_pos1_regs),
  },
  {
    .regs = sensor_brightness_pos2_regs,
    .size = ARRAY_SIZE (sensor_brightness_pos2_regs),
  },
  {
    .regs = sensor_brightness_pos3_regs,
    .size = ARRAY_SIZE (sensor_brightness_pos3_regs),
  },
  {
    .regs = sensor_brightness_pos4_regs,
    .size = ARRAY_SIZE (sensor_brightness_pos4_regs),
  },
};

/*
 * The contrast setttings
 */
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

static struct cfg_array sensor_contrast[] = {
  {
    .regs = sensor_contrast_neg4_regs,
    .size = ARRAY_SIZE (sensor_contrast_neg4_regs),
  },
  {
    .regs = sensor_contrast_neg3_regs,
    .size = ARRAY_SIZE (sensor_contrast_neg3_regs),
  },
  {
    .regs = sensor_contrast_neg2_regs,
    .size = ARRAY_SIZE (sensor_contrast_neg2_regs),
  },
  {
    .regs = sensor_contrast_neg1_regs,
    .size = ARRAY_SIZE (sensor_contrast_neg1_regs),
  },
  {
    .regs = sensor_contrast_zero_regs,
    .size = ARRAY_SIZE (sensor_contrast_zero_regs),
  },
  {
    .regs = sensor_contrast_pos1_regs,
    .size = ARRAY_SIZE (sensor_contrast_pos1_regs),
  },
  {
    .regs = sensor_contrast_pos2_regs,
    .size = ARRAY_SIZE (sensor_contrast_pos2_regs),
  },
  {
    .regs = sensor_contrast_pos3_regs,
    .size = ARRAY_SIZE (sensor_contrast_pos3_regs),
  },
  {
    .regs = sensor_contrast_pos4_regs,
    .size = ARRAY_SIZE (sensor_contrast_pos4_regs),
  },
};

/*
 * The saturation setttings
 */
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

static struct cfg_array sensor_saturation[] = {
  {
    .regs = sensor_saturation_neg4_regs,
    .size = ARRAY_SIZE (sensor_saturation_neg4_regs),
  },
  {
    .regs = sensor_saturation_neg3_regs,
    .size = ARRAY_SIZE (sensor_saturation_neg3_regs),
  },
  {
    .regs = sensor_saturation_neg2_regs,
    .size = ARRAY_SIZE (sensor_saturation_neg2_regs),
  },
  {
    .regs = sensor_saturation_neg1_regs,
    .size = ARRAY_SIZE (sensor_saturation_neg1_regs),
  },
  {
    .regs = sensor_saturation_zero_regs,
    .size = ARRAY_SIZE (sensor_saturation_zero_regs),
  },
  {
    .regs = sensor_saturation_pos1_regs,
    .size = ARRAY_SIZE (sensor_saturation_pos1_regs),
  },
  {
    .regs = sensor_saturation_pos2_regs,
    .size = ARRAY_SIZE (sensor_saturation_pos2_regs),
  },
  {
    .regs = sensor_saturation_pos3_regs,
    .size = ARRAY_SIZE (sensor_saturation_pos3_regs),
  },
  {
    .regs = sensor_saturation_pos4_regs,
    .size = ARRAY_SIZE (sensor_saturation_pos4_regs),
  },
};

/*
 * The exposure target setttings
 */
static struct regval_list sensor_ev_neg4_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0xc0},
};

static struct regval_list sensor_ev_neg3_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0xd0},
};

static struct regval_list sensor_ev_neg2_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0xe0},
};

static struct regval_list sensor_ev_neg1_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0xf0},
};

static struct regval_list sensor_ev_zero_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0x00},
};

static struct regval_list sensor_ev_pos1_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0x10},
};

static struct regval_list sensor_ev_pos2_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0x20},
};

static struct regval_list sensor_ev_pos3_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0x30},
};

static struct regval_list sensor_ev_pos4_regs[] = {
  {0xfd, 0x00},
  {0xf1, 0x40},
};

static struct cfg_array sensor_ev[] = {
  {
    .regs = sensor_ev_neg4_regs,
    .size = ARRAY_SIZE (sensor_ev_neg4_regs),
  },
  {
    .regs = sensor_ev_neg3_regs,
    .size = ARRAY_SIZE (sensor_ev_neg3_regs),
  },
  {
    .regs = sensor_ev_neg2_regs,
    .size = ARRAY_SIZE (sensor_ev_neg2_regs),
  },
  {
    .regs = sensor_ev_neg1_regs,
    .size = ARRAY_SIZE (sensor_ev_neg1_regs),
  },
  {
    .regs = sensor_ev_zero_regs,
    .size = ARRAY_SIZE (sensor_ev_zero_regs),
  },
  {
    .regs = sensor_ev_pos1_regs,
    .size = ARRAY_SIZE (sensor_ev_pos1_regs),
  },
  {
    .regs = sensor_ev_pos2_regs,
    .size = ARRAY_SIZE (sensor_ev_pos2_regs),
  },
  {
    .regs = sensor_ev_pos3_regs,
    .size = ARRAY_SIZE (sensor_ev_pos3_regs),
  },
  {
    .regs = sensor_ev_pos4_regs,
    .size = ARRAY_SIZE (sensor_ev_pos4_regs),
  },
};

/*
 * Here we'll try to encapsulate the changes for just the output
 * video format.
 *
 */


static struct regval_list sensor_fmt_yuv422_yuyv[] = {

  {0xfd, 0x01},
  {0x35, 0x40},
};


static struct regval_list sensor_fmt_yuv422_yvyu[] = {

  {0xfd, 0x01},
  {0x35, 0x41},
};

static struct regval_list sensor_fmt_yuv422_vyuy[] = {

  {0xfd, 0x01},
  {0x35, 0x01},
};

static struct regval_list sensor_fmt_yuv422_uyvy[] = {

  {0xfd, 0x01},
  {0x35, 0x00},
};




/*
 * Low-level register I/O.
 *
 */


/*
 * On most platforms, we'd rather do straight i2c I/O.
 */
static int sensor_read (struct v4l2_subdev * sd, unsigned short reg,
                        unsigned char * value)
{
  int ret = 0;
  int cnt = 0;
  
  ret = cci_read_a8_d8 (sd, reg, value);
  while (ret != 0 && cnt < 2)
  {
    ret = cci_read_a8_d8 (sd, reg, value);
    cnt++;
  }
  if (cnt > 0)
  { vfe_dev_dbg ("sensor read retry=%d\n", cnt); }
  
  return ret;
}

static int sensor_write (struct v4l2_subdev * sd, unsigned short reg,
                         unsigned char value)
{
  int ret = 0;
  int cnt = 0;
  
  ret = cci_write_a8_d8 (sd, reg, value);
  while (ret != 0 && cnt < 2)
  {
    ret = cci_write_a8_d8 (sd, reg, value);
    cnt++;
  }
  if (cnt > 0)
  { vfe_dev_dbg ("sensor write retry=%d\n", cnt); }
  
  return ret;
}

/*
 * Write a list of register settings;
 */
static int sensor_write_array (struct v4l2_subdev * sd, struct regval_list * regs, int array_size)
{
  int i = 0;
  
  if (!regs)
  { return -EINVAL; }
  
  while (i < array_size)
  {
    if (regs->addr == REG_DLY) {
      msleep (regs->data);
    }
    else {
      LOG_ERR_RET (sensor_write (sd, regs->addr, regs->data) )
    }
    i++;
    regs++;
  }
  return 0;
}

static int sensor_s_hflip_vflip (struct v4l2_subdev * sd, int h_value, int v_value)
{
  struct sensor_info * info = to_state (sd);
  unsigned char rdval;
  LOG_ERR_RET (sensor_write (sd, 0xfd, 0x00) )
  LOG_ERR_RET (sensor_read (sd, 0x3f, &rdval) )
  switch (h_value) {
  case 0:
    rdval &= 0xfe;
    break;
  case 1:
    rdval |= 0x01;
    break;
  default:
    return -EINVAL;
  }
  
  switch (v_value) {
  case 0:
    rdval &= 0xfd;
    break;
  case 1:
    rdval |= 0x02;
    break;
  default:
    return -EINVAL;
  }
  LOG_ERR_RET (sensor_write (sd, 0x3f, rdval) )
  
  usleep_range (10000, 12000);
  info->hflip = h_value;
  info->vflip = v_value;
  return 0;
}

static int sensor_g_hflip (struct v4l2_subdev * sd, __s32 * value)
{
  struct sensor_info * info = to_state (sd);
  unsigned char rdval;
  
  LOG_ERR_RET (sensor_write (sd, 0xfd, 0x00) )
  LOG_ERR_RET (sensor_read (sd, 0x3f, &rdval) )
  
  rdval &= (1 << 0);
  *value = (rdval >> 0);
  
  info->vflip = *value;
  return 0;
}

static int sensor_s_hflip (struct v4l2_subdev * sd, int value)
{
  struct sensor_info * info = to_state (sd);
  unsigned char rdval;
  
  LOG_ERR_RET (sensor_write (sd, 0xfd, 0x00) )
  LOG_ERR_RET (sensor_read (sd, 0x3f, &rdval) )
  
  switch (value) {
  case 0:
    rdval &= 0xfe;
    break;
  case 1:
    rdval |= 0x01;
    break;
  default:
    return -EINVAL;
  }
  
  LOG_ERR_RET (sensor_write (sd, 0x3f, rdval) )
  
  usleep_range (10000, 12000);
  info->hflip = value;
  return 0;
}

static int sensor_g_vflip (struct v4l2_subdev * sd, __s32 * value)
{
  struct sensor_info * info = to_state (sd);
  unsigned char rdval;
  
  LOG_ERR_RET (sensor_write (sd, 0xfd, 0x00) )
  LOG_ERR_RET (sensor_read (sd, 0x3f, &rdval) )
  
  rdval &= (1 << 1);
  *value = (rdval >> 1);
  
  info->vflip = *value;
  return 0;
}

static int sensor_s_vflip (struct v4l2_subdev * sd, int value)
{
  struct sensor_info * info = to_state (sd);
  unsigned char rdval;
  
  LOG_ERR_RET (sensor_write (sd, 0xfd, 0x00) )
  LOG_ERR_RET (sensor_read (sd, 0x3f, &rdval) )
  
  switch (value) {
  case 0:
    rdval &= 0xfd;
    break;
  case 1:
    rdval |= 0x02;
    break;
  default:
    return -EINVAL;
  }
  
  LOG_ERR_RET (sensor_write (sd, 0x3f, rdval) )
  
  usleep_range (10000, 12000);
  info->vflip = value;
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
  /*  int ret;
    struct sensor_info *info = to_state(sd);
    struct regval_list regs;
  */
  return 0;
}

static int sensor_s_autoexp (struct v4l2_subdev * sd,
                             enum v4l2_exposure_auto_type value)
{
  /*  int ret;
    struct sensor_info *info = to_state(sd);
    struct regval_list regs;
  */
  return 0;
}

static int sensor_g_autowb (struct v4l2_subdev * sd, int * value)
{
  /*  int ret;
    struct sensor_info *info = to_state(sd);
    struct regval_list regs;
  */
  return 0;
}

static int sensor_s_autowb (struct v4l2_subdev * sd, int value)
{
  return 0;
  /*
    int ret;
    unsigned char val;
    struct sensor_info *info = to_state(sd);
  
    ret = sensor_write_array(sd, sensor_wb_auto_regs, ARRAY_SIZE(sensor_wb_auto_regs));
    if (ret < 0) {
      vfe_dev_err("sensor_write_array err at sensor_s_autowb!\n");
      return ret;
    }
  
    ret = sensor_read(sd, 0x32, &val);
    if (ret < 0) {
      vfe_dev_err("sensor_read err at sensor_s_autowb!\n");
      return ret;
    }
  
    switch(value) {
    case 0:
      val &= 0xef;
      break;
    case 1:
      val |= 0x10;
      break;
    default:
      break;
    }
    ret = sensor_write(sd, 0x32, val);
    if (ret < 0) {
      vfe_dev_err("sensor_write err at sensor_s_autowb!\n");
      return ret;
    }
  
    usleep_range(10000,12000);
  
    info->autowb = value;
    return 0;*/
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
  struct sensor_info * info = to_state (sd);
  
  if (info->brightness == value)
  { return 0; }
  
  if (value < -4 || value > 4)
  { return -ERANGE; }
  
  LOG_ERR_RET (sensor_write_array (sd, sensor_brightness[value + 4].regs, sensor_brightness[value + 4].size) )
  
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
  struct sensor_info * info = to_state (sd);
  
  if (info->contrast == value)
  { return 0; }
  
  if (value < -4 || value > 4)
  { return -ERANGE; }
  
  LOG_ERR_RET (sensor_write_array (sd, sensor_contrast[value + 4].regs, sensor_contrast[value + 4].size) )
  
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
  struct sensor_info * info = to_state (sd);
  
  if (info->saturation == value)
  { return 0; }
  
  if (value < -4 || value > 4)
  { return -ERANGE; }
  
  LOG_ERR_RET (sensor_write_array (sd, sensor_saturation[value + 4].regs, sensor_saturation[value + 4].size) )
  
  info->saturation = value;
  return 0;
}

static int sensor_g_exp_bias (struct v4l2_subdev * sd, __s32 * value)
{
  struct sensor_info * info = to_state (sd);
  
  *value = info->exp_bias;
  return 0;
}

static int sensor_s_exp_bias (struct v4l2_subdev * sd, int value)
{
  struct sensor_info * info = to_state (sd);
  
  if (info->exp_bias == value)
  { return 0; }
  
  if (value < -4 || value > 4)
  { return -ERANGE; }
  
  LOG_ERR_RET (sensor_write_array (sd, sensor_ev[value + 4].regs, sensor_ev[value + 4].size) )
  
  info->exp_bias = value;
  return 0;
}

static int sensor_g_wb (struct v4l2_subdev * sd, int * value)
{
  struct sensor_info * info = to_state (sd);
  enum v4l2_auto_n_preset_white_balance * wb_type = (enum v4l2_auto_n_preset_white_balance *) value;
  
  *wb_type = info->wb;
  
  return 0;
}

static int sensor_s_wb (struct v4l2_subdev * sd,
                        enum v4l2_auto_n_preset_white_balance value)
{
  struct sensor_info * info = to_state (sd);
  
  if (info->capture_mode == V4L2_MODE_IMAGE)
  { return 0; }
  
  LOG_ERR_RET (sensor_write_array (sd, sensor_wb[value].regs , sensor_wb[value].size) )
  
  if (value == V4L2_WHITE_BALANCE_AUTO)
  { info->autowb = 1; }
  else
  { info->autowb = 0; }
  
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
  struct sensor_info * info = to_state (sd);
  
  if (info->clrfx == value)
  { return 0; }
  
  LOG_ERR_RET (sensor_write_array (sd, sensor_colorfx[value].regs, sensor_colorfx[value].size) )
  
  info->clrfx = value;
  return 0;
}

static int sensor_g_flash_mode (struct v4l2_subdev * sd,
                                __s32 * value)
{
  struct sensor_info * info = to_state (sd);
  enum v4l2_flash_led_mode * flash_mode = (enum v4l2_flash_led_mode *) value;
  
  *flash_mode = info->flash_mode;
  return 0;
}

static int sensor_s_flash_mode (struct v4l2_subdev * sd,
                                enum v4l2_flash_led_mode value)
{
  struct sensor_info * info = to_state (sd);
  

  info->flash_mode = value;
  return 0;
}


/*
 * Stuff that knows about the sensor.
 */

static int sensor_power (struct v4l2_subdev * sd, int on)
{
  
  cci_lock (sd);
  
  switch (on)
  {
  case CSI_SUBDEV_STBY_ON:
    vfe_dev_dbg ("CSI_SUBDEV_STBY_ON\n");
    vfe_gpio_write (sd, PWDN, CSI_RST_OFF);
    usleep_range (10000, 12000);
    vfe_gpio_write (sd, PWDN, CSI_STBY_ON);
    msleep (100);
    vfe_set_mclk (sd, OFF);
    usleep_range (10000, 12000);
    break;
  case CSI_SUBDEV_STBY_OFF:
    vfe_dev_dbg ("CSI_SUBDEV_STBY_OFF\n");
    vfe_set_mclk_freq (sd, MCLK);
    vfe_set_mclk (sd, ON);
    usleep_range (30000, 31000);
    vfe_gpio_write (sd, PWDN, CSI_STBY_OFF);
    msleep (100);
    break;
  case CSI_SUBDEV_PWR_ON:
    vfe_dev_dbg ("CSI_SUBDEV_PWR_ON\n");
    vfe_gpio_set_status (sd, PWDN, 1);
    vfe_gpio_set_status (sd, RESET, 1);
    vfe_gpio_write (sd, PWDN, CSI_STBY_ON);
    vfe_gpio_write (sd, RESET, CSI_RST_ON);
    usleep_range (1000, 1200);
    vfe_set_mclk_freq (sd, MCLK);
    vfe_set_mclk (sd, ON);
    usleep_range (10000, 12000);
    vfe_gpio_write (sd, POWER_EN, CSI_PWR_ON);
    vfe_set_pmu_channel (sd, AVDD, ON);
    usleep_range (10000, 12000);
    vfe_set_pmu_channel (sd, IOVDD, ON);
    usleep_range (10000, 12000);
    
    vfe_set_pmu_channel (sd, DVDD, ON);
    usleep_range (10000, 12000);
    vfe_set_pmu_channel (sd, AFVDD, ON);
    usleep_range (10000, 12000);
    vfe_gpio_write (sd, PWDN, CSI_STBY_OFF);
    usleep_range (5000, 10000);
    vfe_gpio_write (sd, PWDN, CSI_STBY_ON);
    usleep_range (10000, 12000);
    vfe_gpio_write (sd, PWDN, CSI_STBY_OFF);
    usleep_range (10000, 12000);
    vfe_gpio_write (sd, RESET, CSI_RST_OFF);
    usleep_range (30000, 31000);
    vfe_gpio_write (sd, RESET, CSI_RST_ON);
    usleep_range (30000, 31000);
    vfe_gpio_write (sd, RESET, CSI_RST_OFF);
    usleep_range (30000, 31000);
    break;
  case CSI_SUBDEV_PWR_OFF:
    vfe_dev_dbg ("CSI_SUBDEV_PWR_OFF\n");
    vfe_gpio_write (sd, PWDN, CSI_STBY_ON);
    usleep_range (10000, 12000);
    vfe_gpio_write (sd, RESET, CSI_RST_ON);
    usleep_range (10000, 12000);
    vfe_gpio_write (sd, POWER_EN, CSI_PWR_OFF);
    vfe_set_pmu_channel (sd, AFVDD, OFF);
    usleep_range (10000, 12000);
    vfe_set_pmu_channel (sd, DVDD, OFF);
    usleep_range (10000, 12000);
    vfe_set_pmu_channel (sd, AVDD, OFF);
    usleep_range (10000, 12000);
    vfe_set_pmu_channel (sd, IOVDD, OFF);
    usleep_range (10000, 12000);
    vfe_set_mclk (sd, OFF);
    vfe_gpio_set_status (sd, RESET, 0);
    vfe_gpio_set_status (sd, PWDN, 0);
    break;
  default:
    return -EINVAL;
  }
  
  cci_unlock (sd);
  return 0;
}

static int sensor_reset (struct v4l2_subdev * sd, u32 val)
{
  switch (val)
  {
  case 0:
    vfe_gpio_write (sd, RESET, CSI_RST_OFF);
    usleep_range (10000, 12000);
    break;
  case 1:
    vfe_gpio_write (sd, RESET, CSI_RST_ON);
    usleep_range (10000, 12000);
    break;
  default:
    return -EINVAL;
  }
  
  return 0;
}

static int sensor_detect (struct v4l2_subdev * sd)
{
  int ret;
  unsigned char val;
  
  ret = sensor_read (sd, 0x02, &val);
  if (ret < 0) {
    vfe_dev_err ("sensor_read err at sensor_detect!\n");
    return ret;
  }
  
  if (val != 0x25)
  { return -ENODEV; }
  
  return 0;
}

static int sensor_init (struct v4l2_subdev * sd, u32 val)
{
  int ret;
  vfe_dev_dbg ("sensor_init\n");
  /*Make sure it is a target sensor*/
  ret = sensor_detect (sd);
  if (ret) {
    vfe_dev_err ("chip found is not an target chip.\n");
    return ret;
  }
  
  return sensor_write_array (sd, sensor_default_regs , ARRAY_SIZE (sensor_default_regs) );
}

static long sensor_ioctl (struct v4l2_subdev * sd, unsigned int cmd, void * arg)
{
  int ret = 0;
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


static struct sensor_win_size
    sensor_win_sizes[] = {
  /* UXGA */
  {
    .width      = UXGA_WIDTH,
    .height     = UXGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = sensor_uxga_regs,
    .regs_size  = ARRAY_SIZE (sensor_uxga_regs),
    .set_size   = NULL,
  },
  /* 720p */
  {
    .width      = HD720_WIDTH,
    .height     = HD720_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = sensor_720p_regs,
    .regs_size  = ARRAY_SIZE (sensor_720p_regs),
    .set_size   = NULL,
  },
  /* SVGA */
  {
    .width      = SVGA_WIDTH,
    .height     = SVGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = sensor_svga_regs,
    .regs_size  = ARRAY_SIZE (sensor_svga_regs),
    .set_size   = NULL,
  },
  /* VGA */
  {
    .width      = VGA_WIDTH,
    .height     = VGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = sensor_vga_regs,
    .regs_size  = ARRAY_SIZE (sensor_vga_regs),
    .set_size   = NULL,
  },
};

#define N_WIN_SIZES (ARRAY_SIZE(sensor_win_sizes))

static int sensor_enum_fmt (struct v4l2_subdev * sd, unsigned index,
                            enum v4l2_mbus_pixelcode * code)
{
  if (index >= N_FMTS)
  { return -EINVAL; }
  
  *code = sensor_formats[index].mbus_code;
  return 0;
}

static int sensor_enum_size (struct v4l2_subdev * sd,
                             struct v4l2_frmsizeenum * fsize)
{
  if (fsize->index > N_WIN_SIZES - 1)
  { return -EINVAL; }
  
  fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
  fsize->discrete.width = sensor_win_sizes[fsize->index].width;
  fsize->discrete.height = sensor_win_sizes[fsize->index].height;
  
  return 0;
}

static int sensor_try_fmt_internal (struct v4l2_subdev * sd,
                                    struct v4l2_mbus_framefmt * fmt,
                                    struct sensor_format_struct ** ret_fmt,
                                    struct sensor_win_size ** ret_wsize)
{
  int index;
  struct sensor_win_size * wsize;
  
  for (index = 0; index < N_FMTS; index++)
    if (sensor_formats[index].mbus_code == fmt->code)
    { break; }
    
  if (index >= N_FMTS)
  { return -EINVAL; }
  
  if (ret_fmt != NULL)
  { *ret_fmt = sensor_formats + index; }
  
  /*
   * Fields: the sensor devices claim to be progressive.
   */
  
  fmt->field = V4L2_FIELD_NONE;
  
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
  fmt->width = wsize->width;
  fmt->height = wsize->height;
  
  return 0;
}

static int sensor_try_fmt (struct v4l2_subdev * sd,
                           struct v4l2_mbus_framefmt * fmt)
{
  return sensor_try_fmt_internal (sd, fmt, NULL, NULL);
}

static int sensor_g_mbus_config (struct v4l2_subdev * sd,
                                 struct v4l2_mbus_config * cfg)
{
  cfg->type = V4L2_MBUS_PARALLEL;
  cfg->flags = V4L2_MBUS_MASTER | VREF_POL | HREF_POL | CLK_POL ;
  
  return 0;
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
  vfe_dev_dbg ("sensor_s_fmt\n");
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
  sensor_s_hflip_vflip (sd, info->hflip, info->vflip);
  
  return 0;
}

/*
 * Implement G/S_PARM.  There is a "high quality" mode we could try
 * to do someday; for now, we just do the frame rate tweak.
 */
static int sensor_g_parm (struct v4l2_subdev * sd, struct v4l2_streamparm * parms)
{
  struct v4l2_captureparm * cp = &parms->parm.capture;
  struct sensor_info * info = to_state (sd);
  
  if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
  { return -EINVAL; }
  
  memset (cp, 0, sizeof (struct v4l2_captureparm) );
  cp->capability = V4L2_CAP_TIMEPERFRAME;
  cp->timeperframe.numerator = 1;
  
  if (info->width > SVGA_WIDTH && info->height > SVGA_HEIGHT) {
    cp->timeperframe.denominator = SENSOR_FRAME_RATE / 2;
  }
  else {
    cp->timeperframe.denominator = SENSOR_FRAME_RATE;
  }
  
  return 0;
}

static int sensor_s_parm (struct v4l2_subdev * sd, struct v4l2_streamparm * parms)
{


  return 0;
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
  case V4L2_CID_VFLIP:
  case V4L2_CID_HFLIP:
    return v4l2_ctrl_query_fill (qc, 0, 1, 1, 0);
  case V4L2_CID_EXPOSURE:
  case V4L2_CID_AUTO_EXPOSURE_BIAS:
    return v4l2_ctrl_query_fill (qc, -4, 4, 1, 0);
  case V4L2_CID_EXPOSURE_AUTO:
    return v4l2_ctrl_query_fill (qc, 0, 1, 1, 0);
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
    return v4l2_ctrl_query_fill (qc, 0, 9, 1, 1);
  case V4L2_CID_AUTO_WHITE_BALANCE:
    return v4l2_ctrl_query_fill (qc, 0, 1, 1, 1);
  case V4L2_CID_COLORFX:
    return v4l2_ctrl_query_fill (qc, 0, 15, 1, 0);
  case V4L2_CID_FLASH_LED_MODE:
    return v4l2_ctrl_query_fill (qc, 0, 4, 1, 0);
  }
  return -EINVAL;
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
  case V4L2_CID_AUTO_EXPOSURE_BIAS:
    return sensor_g_exp_bias (sd, &ctrl->value);
  case V4L2_CID_EXPOSURE_AUTO:
    return sensor_g_autoexp (sd, &ctrl->value);
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
    return sensor_g_wb (sd, &ctrl->value);
  case V4L2_CID_AUTO_WHITE_BALANCE:
    return sensor_g_autowb (sd, &ctrl->value);
  case V4L2_CID_COLORFX:
    return sensor_g_colorfx (sd, &ctrl->value);
  case V4L2_CID_FLASH_LED_MODE:
    return sensor_g_flash_mode (sd, &ctrl->value);
  }
  return -EINVAL;
}

static int sensor_s_ctrl (struct v4l2_subdev * sd, struct v4l2_control * ctrl)
{
  struct v4l2_queryctrl qc;
  int ret;
  
  qc.id = ctrl->id;
  ret = sensor_queryctrl (sd, &qc);
  if (ret < 0) {
    return ret;
  }
  
  if (qc.type == V4L2_CTRL_TYPE_MENU ||
      qc.type == V4L2_CTRL_TYPE_INTEGER ||
      qc.type == V4L2_CTRL_TYPE_BOOLEAN)
  {
    if (ctrl->value < qc.minimum || ctrl->value > qc.maximum) {
      return -ERANGE;
    }
  }
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
  case V4L2_CID_AUTO_EXPOSURE_BIAS:
    return sensor_s_exp_bias (sd, ctrl->value);
  case V4L2_CID_EXPOSURE_AUTO:
    return sensor_s_autoexp (sd,
                             (enum v4l2_exposure_auto_type) ctrl->value);
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
    return sensor_s_wb (sd,
                        (enum v4l2_auto_n_preset_white_balance) ctrl->value);
  case V4L2_CID_AUTO_WHITE_BALANCE:
    return sensor_s_autowb (sd, ctrl->value);
  case V4L2_CID_COLORFX:
    return sensor_s_colorfx (sd,
                             (enum v4l2_colorfx) ctrl->value);
  case V4L2_CID_FLASH_LED_MODE:
    return sensor_s_flash_mode (sd,
                                (enum v4l2_flash_led_mode) ctrl->value);
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
  .enum_mbus_fmt = sensor_enum_fmt,
  .enum_framesizes = sensor_enum_size,
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

static int sensor_probe (struct i2c_client * client,
                         const struct i2c_device_id * id)
{
  struct v4l2_subdev * sd;
  struct sensor_info * info;

  info = kzalloc (sizeof (struct sensor_info), GFP_KERNEL);
  if (info == NULL)
  { return -ENOMEM; }
  sd = &info->sd;
  cci_dev_probe_helper (sd, client, &sensor_ops, &cci_drv);
  
  
  info->fmt = &sensor_formats[0];
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
  struct v4l2_subdev * sd;
  
  sd = cci_dev_remove_helper (client, &cci_drv);
  kfree (to_state (sd) );
  return 0;
}

static const struct i2c_device_id sensor_id[] = {
  { SENSOR_NAME, 0 },
  { }
};
MODULE_DEVICE_TABLE (i2c, sensor_id);

static struct i2c_driver sensor_driver = {
  .driver = {
    .owner = THIS_MODULE,
    .name = SENSOR_NAME,
  },
  .probe = sensor_probe,
  .remove = sensor_remove,
  .id_table = sensor_id,
};

static __init int init_sensor (void)
{
  return cci_dev_init_helper (&sensor_driver);
}

static __exit void exit_sensor (void)
{
  cci_dev_exit_helper (&sensor_driver);
}

module_init (init_sensor);
module_exit (exit_sensor);
