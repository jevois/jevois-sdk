/*
 * sunxi sensor header file
 * Author:raymonxiu
 */
#ifndef __CAMERA_CFG_H__
#define __CAMERA_CFG_H__

typedef enum tag_CAMERA_IO_CMD {
  GET_CURRENT_WIN_CFG,
  SET_FPS,
  SET_FLASH_CTRL,
  ISP_SET_EXP_GAIN,
  GET_SENSOR_EXIF,
} __camera_cmd_t;

struct sensor_exp_gain {
  int exp_val;
  int gain_val;
};

struct sensor_exif_attribute {
  __u32 exposure_time_num;
  __u32 exposure_time_den;
  __u32 fnumber;
  __u32 focal_length;
  __u32 iso_speed;
  __u32 flash_fire;
  __u32 brightness;
};

struct sensor_win_size {
  int width;
  int height;
  unsigned int hoffset;    
  unsigned int voffset;    
  unsigned int hts;        
  unsigned int vts;        
  unsigned int pclk;       
  unsigned int mipi_bps;   
  unsigned int fps_fixed;  
  unsigned int bin_factor; 
  unsigned int intg_min;   
  unsigned int intg_max;   
  unsigned int gain_min;   
  unsigned int gain_max;   
  int width_input;
  int height_input;
  
  void * regs; /* Regs to tweak */
  int regs_size;
  int (*set_size) (struct v4l2_subdev * sd);
  /* h/vref stuff */
};


#endif