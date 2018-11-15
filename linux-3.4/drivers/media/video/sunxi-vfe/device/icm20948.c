/* Copyright (2018) by JeVois Inc, all right reserved */


#define IMU_ADDR 0x68

static const uint8_t dmp3_image[] = {
	#include "icm20948_img.dmp3a.h"
};

#define DMP_LOAD_START 0x90

#define IMU_WRITE(reg)                                               \
  ret = imu_write_a8_d8(sd, reg, imuval);                                     \
  if (ret < 0) { vfe_dev_err("Error writing %u to sensor register %u in %s\n", val, reg, __func__); return ret; }

#define IMU_READ(reg)                                                \
  ret = imu_read_a8_d8(sd, reg, &imuval);                                     \
  if (ret < 0) { vfe_dev_err("Error reading from sensor register %u in %s\n", reg, __func__); return ret;	}

int imu_read_a8_d8(struct v4l2_subdev *sd, unsigned char addr, unsigned char *value)
{
  unsigned char data;
  struct i2c_msg msg;
  int ret;
  struct i2c_client *client = v4l2_get_subdevdata(sd);
  
  data = addr;

  msg.addr = IMU_ADDR;
  msg.flags = 0;
  msg.len = 1;
  msg.buf = &data;
  ret = i2c_transfer(client->adapter, &msg, 1);
  
  if (ret < 0)
  {
    vfe_dev_err("%s error sending register address! slave = 0x%x, addr = 0x%2x\n ",__func__, IMU_ADDR, addr);
    return ret;
  }

  msg.addr = IMU_ADDR;
  msg.flags = I2C_M_RD;
  msg.len = 1;
  msg.buf = &data;
  
  ret = i2c_transfer(client->adapter, &msg, 1);
  if (ret >= 0) {
    *value = data;
    ret = 0;
  } else {
    vfe_dev_err("%s error reading register value! slave = 0x%x, addr = 0x%2x, value = 0x%2x\n ",__func__, IMU_ADDR, addr,*value);
  }
  return ret;
}


int imu_read_a8_d8_array(struct v4l2_subdev *sd, unsigned char addr, unsigned int siz, unsigned char * values)
{
  struct i2c_msg msg[2];
  int ret;
  struct i2c_client *client = v4l2_get_subdevdata(sd);
  
  /*
   * Send out the register address...
   */
  msg[0].addr = IMU_ADDR;
  msg[0].flags = 0;
  msg[0].len = 1;
  msg[0].buf = &addr;
  /*
   * ...then read back the results.
   */
  msg[1].addr = IMU_ADDR;
  msg[1].flags = I2C_M_RD;
  msg[1].len = siz;
  msg[1].buf = &values[0];
  
  ret = i2c_transfer(client->adapter, msg, 2);
  if (ret >= 0) ret = 0;
  else vfe_dev_err("%s error reading register values! slave = 0x%x, addr = 0x%2x, siz = %u\n ",
                   __func__, IMU_ADDR, addr, siz);

  return ret;
}

int imu_write_a8_d8(struct v4l2_subdev *sd, unsigned char addr, unsigned char value)
{
  struct i2c_msg msg;
  unsigned char data[2];
  int ret;
  struct i2c_client *client = v4l2_get_subdevdata(sd);
  
  data[0] = addr;
  data[1] = value;
  
  msg.addr = IMU_ADDR;
  msg.flags = 0;
  msg.len = 2;
  msg.buf = data;
  
  ret = i2c_transfer(client->adapter, &msg, 1);
  if (ret >= 0) {
    ret = 0;
  } else {
    vfe_dev_err("%s error! slave = 0x%x, addr = 0x%2x, value = 0x%2x\n",__func__, IMU_ADDR, addr, value);
  }
  return ret;
}

int imu_write_a8_d8_array(struct v4l2_subdev *sd, unsigned char addr, unsigned int siz, unsigned char const * values)
{
  struct i2c_msg msg;
  int ret; unsigned char * data; unsigned int i;
  struct i2c_client *client = v4l2_get_subdevdata(sd);

  data = (unsigned char *)kmalloc(siz + 1, GFP_KERNEL);
  data[0] = addr;
  for (i = 0; i < siz; ++i) data[i+1] = values[i];
  
  msg.addr = IMU_ADDR;
  msg.flags = 0;
  msg.len = siz + 1;
  msg.buf = data;

  ret = i2c_transfer(client->adapter, &msg, 1);

  if (ret >= 0) ret = 0;
  else vfe_dev_err("%s error! slave = 0x%x, addr = 0x%2x, siz = %u\n ",__func__, IMU_ADDR, addr, siz);

  kfree(data);
  
  return ret;
}

static int imu_write_array(struct v4l2_subdev *sd, struct reg_list_a8_d8 *regs, int array_size)
{
  int i = 0;

  if (!regs) return -EINVAL;

  while (i < array_size)
  {

    if (imu_write_a8_d8(sd, regs->addr, regs->data)) return -EINVAL;
    ++i; ++regs;
  }
  return 0;
}

static int imu_load_dmp(struct v4l2_subdev *sd)
{
  int ret; unsigned char * data;
  int const siz = sizeof(dmp3_image);



  

  
  vfe_dev_err("%s loading %d bytes of DMP code...\n", __func__, siz);
  
  ret = imu_write_a8_d8(sd, 0x7c, 0x90);
  if (ret) { vfe_dev_err("%s set DMP start addr failed\n", __func__); return ret; }

  ret = imu_write_a8_d8_array(sd, 0x7d, siz, &dmp3_image[0]);
  if (ret) { vfe_dev_err("%s DMP code upload failed\n", __func__); return ret; }
  
  vfe_dev_err("%s loaded %d bytes of DMP code.\n", __func__, siz);
  /*
  ret = imu_write_a8_d8(sd, 0x7c, 0x90);
  if (ret) { vfe_dev_err("%s set DMP start addr failed\n", __func__); return ret; }

  data = (unsigned char *)kmalloc(siz, GFP_KERNEL);

  ret = imu_read_a8_d8_array(sd, 0x7d, siz, data);
  if (ret) vfe_dev_err("%s failed to read DMP data back from IMU\n", __func__);
  else if (memcmp(data, &dmp3_image[0], siz)) vfe_dev_err("%s write verify failed!\n", __func__);
  */
  kfree(data);
  
  return ret;
}
