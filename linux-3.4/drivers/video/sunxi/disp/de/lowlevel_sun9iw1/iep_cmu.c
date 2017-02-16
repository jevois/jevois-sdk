

#include "iep_cmu.h"

u32 cmu_reg_base[3] = {0, 0};

#if defined(CONFIG_ARCH_SUN8IW1P1) || defined(CONFIG_ARCH_SUN6I)
u32 hsv_range_par[21] =
{
  0x01550eaa, 0x06aa0400, 0x0c000955, 0x095506aa, 0x0eaa0c00, 0x04000155, 0x02380000,
  0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000,
  0x0fff0001, 0x0fff0000, 0x0fff0000, 0x0fff0000, 0x0fff0000, 0x0fff0000, 0x0ae101a0
};

u32 hsv_adjust_vivid_par[16] =
{
  0x00000000, 0x068a0000,
  0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};


u32 hsv_adjust_flesh_par[16] =
{
  0x00000000, 0x00000080,
  0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000,
  0x00e80004, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00e80004
};

u32 hsv_adjust_scenery_par[16] =
{
  0x00000000, 0x068a0080,
  0xff000000, 0xff000000, 0xff000000, 0xff000f8f, 0xff000000, 0xff0000cc, 0xff000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0102000a, 0x00000000
};
#elif defined(CONFIG_ARCH_SUN8IW3P1) || defined(CONFIG_ARCH_SUN9IW1P1)
u32 hsv_range_par[21] =
{
  0x01550eaa, 0x06aa0400, 0x0c000955, 0x095506aa, 0x0eaa0c00, 0x04000155, 0x02380000,
  0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000,
  0x00ff0001, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00ae001a
};

u32 hsv_adjust_vivid_par[16] =
{
  0x00000000, 0x00690000,
  0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};


u32 hsv_adjust_flesh_par[16] =
{
  0x00000000, 0x00000080,
  0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000,
  0x000f0004, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000f0004
};

u32 hsv_adjust_scenery_par[16] =
{
  0x00000000, 0x00690080,
  0xff000000, 0xff000000, 0xff000000, 0xff000f8f, 0xff000000, 0xff0000cc, 0xff000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0010000a, 0x00000000
};
#else
u32 hsv_range_par[21] =
{
  0x01550eaa, 0x06aa0400, 0x0c000955, 0x095506aa, 0x0eaa0c00, 0x04000155, 0x02380000,
  0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000,
  0x00ff0001, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00ff0000, 0x00ae001a
};

u32 hsv_adjust_vivid_par[16] =
{
  0x00000000, 0x00690000,
  0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};


u32 hsv_adjust_flesh_par[16] =
{
  0x00000000, 0x00000080,
  0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000,
  0x000f0004, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000f0004
};

u32 hsv_adjust_scenery_par[16] =
{
  0x00000000, 0x00690080,
  0xff000000, 0xff000000, 0xff000000, 0xff000f8f, 0xff000000, 0xff0000cc, 0xff000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0010000a, 0x00000000
};
#endif

s32 IEP_CMU_Set_Reg_Base (u32 sel, u32 address)
{
  cmu_reg_base[sel] = address;
  
  return 0;
}

s32 IEP_CMU_Reg_Init (u32 sel)
{
  memset ( (void *) (cmu_reg_base[sel]), 0, 0x100);
  
  return 0;
}

s32 IEP_CMU_Set_Data_Flow (__u8 sel, u32 data_flow)
{
  u32 reg_val;
  
  reg_val = CMU_RUINT32 (sel, IMGEHC_CMU_CTL_REG_OFF);
  reg_val |= (data_flow & 0x3) << 16;
  CMU_WUINT32 (sel, IMGEHC_CMU_CTL_REG_OFF, reg_val);
  
  return 0;
}

s32 IEP_CMU_Set_Window (__u8 sel, disp_window * window)
{
  s32 Lx, Ty, Rx, By;
  u32 width, height, reg_val;
  
  Lx = window->x;
  Ty = window->y;
  Rx = window->x + window->width - 1;
  By = window->y + window->height - 1;
  reg_val = CMU_RUINT32 (sel, IMGEHC_CMU_INSIZE_REG_OFF);
  width = (reg_val & 0x1fff) + 1;
  height = ( (reg_val >> 16) & 0x1fff) + 1;
  
  if ( (Lx > Rx) || (Ty > By) || (Lx > width - 1) || (Ty > height - 1) || (Rx < 1) || (By < 1) )
  {
    return -1;
  }
  
  if (Lx < 0) { Lx = 0; }
  if (Ty < 0) { Ty = 0; }
  if ( (width > 1) && (Rx > width - 1) ) { Rx = width - 1; }
  if ( (height > 1) && (By > height - 1) ) { By = height - 1; }
  CMU_WUINT32 (sel, IMGEHC_CMU_OWP_REG0_OFF, ( (Ty & 0x1fff) << 16) | (Lx & 0x1fff) );
  CMU_WUINT32 (sel, IMGEHC_CMU_OWP_REG1_OFF, ( (By & 0x1fff) << 16) | (Rx & 0x1fff) );
  reg_val = (CMU_RUINT32 (sel, IMGEHC_CMU_CTL_REG_OFF) & 0xfffffffb) | (0x1 << 2);
  CMU_WUINT32 (sel, IMGEHC_CMU_CTL_REG_OFF, reg_val);
  
  return 0;
}

s32 IEP_CMU_Set_Imgsize (__u8 sel, u32 width, u32 height)
{
  CMU_WUINT32 (sel, IMGEHC_CMU_INSIZE_REG_OFF, ( ( (height - 1) & 0x1fff) << 16) | ( (width - 1) & 0x1fff) );
  
  return 0;
}

s32 IEP_CMU_Set_Par (__u8 sel, u32 hue, u32 saturaion, u32 brightness, u32 mode)
{
  char primary_key[20];
  u32 i, j, reg_val;
  s32 ret = 0;
  u32 hue_g, hue_l, i_hue, i_saturaion, i_brightness;
  
  hue_l = 41 * hue - 0x7FF;
  hue_l = ( (s32) hue_l > 0x7FF) ? 0x7FF : hue_l;
  hue_g = hue > 50 ? hue_l : ( ( (41 * hue + 0x7FF) > 0xFFF) ? 0xFFF : (41 * hue + 0x7FF) );
  
  i_hue = mode == 7 ? hue_g : hue_l;
  #if defined(CONFIG_ARCH_SUN8IW1P1) || defined(CONFIG_ARCH_SUN6I)
  i_saturaion = 82 * saturaion - 0xFFF;
  #elif defined(CONFIG_ARCH_SUN8IW3P1) || defined(CONFIG_ARCH_SUN9IW1P1)
  i_saturaion = 5 * saturaion - 0xFF;
  #else
  i_saturaion = 5 * saturaion - 0xFF;
  #endif
  i_brightness = 5 * brightness - 0xFF;
  
  i_hue = hue == 50 ? 0 : i_hue;                     
  #if defined(CONFIG_ARCH_SUN8IW1P1) || defined(CONFIG_ARCH_SUN6I)
  i_saturaion = saturaion == 50 ? 0 : ( (s32) i_saturaion > 0xFFF ? 0xFFF : i_saturaion);
  #elif defined(CONFIG_ARCH_SUN8IW3P1) || defined(CONFIG_ARCH_SUN9IW1P1)
  i_saturaion = saturaion == 50 ? 0 : ( (s32) i_saturaion > 0xFF ? 0xFF : i_saturaion);
  #else
  i_saturaion = saturaion == 50 ? 0 : ( (s32) i_saturaion > 0xFF ? 0xFF : i_saturaion);
  #endif
  i_brightness = brightness == 50 ? 0 : ( (s32) i_brightness > 0xFF ? 0xFF : i_brightness);
  

  for (i = 0, j = 0; i < 7; i++, j += 4)
  { CMU_WUINT32 (sel, IMGEHC_CMU_HRANGE_REG0_OFF + j, hsv_range_par[i] & 0x0FFF0FFF); }
  
  for (i = 7, j = 0; i < 14; i++, j += 4)
  {
    reg_val = CMU_RUINT32 (sel, IMGEHC_CMU_VRANGE_HLGAIN_REG0_OFF) & 0xFFF;
    CMU_WUINT32 (sel, IMGEHC_CMU_VRANGE_HLGAIN_REG0_OFF + j, (hsv_range_par[i] & 0xFFFF0000) | reg_val);
  }
  
  for (i = 14, j = 0; i < 21; i++, j += 4)
  { CMU_WUINT32 (sel, IMGEHC_CMU_SRANGE_REG0_OFF + j, hsv_range_par[i] & 0x0FFF0FFF); }
  
  switch (mode)
  {
  case 0x0://red
  case 0x1://green
  case 0x2://blue
  case 0x3://cyan
  case 0x4://magenta
  case 0x5://yellow
  case 0x6://flesh
    CMU_WUINT32 (sel, IMGEHC_CMU_VRANGE_HLGAIN_REG0_OFF + mode * 4, (0xFF << 24) | (i_hue & 0xFFF) );
    CMU_WUINT32 (sel, IMGEHC_CMU_LOCAL_SVGAIN_REG0_OFF + mode * 4, ( (i_saturaion & 0x1FFF) << 16) | (i_brightness & 0x1FF) );
    break;
    
  case 0x7://global adjust
    CMU_WUINT32 (sel, IMGEHC_CMU_HGGAIN_REG_OFF, i_hue & 0xFFF);
    CMU_WUINT32 (sel, IMGEHC_CMU_GLOBAL_SVGAIN_REG_OFF, ( (i_saturaion & 0x1FFF) << 16) | (i_brightness & 0x1FF) );
    break;
    
  case 0x8://vivid mode
    CMU_WUINT32 (sel, IMGEHC_CMU_HGGAIN_REG_OFF, hsv_adjust_vivid_par[0] & 0xFFF);
    CMU_WUINT32 (sel, IMGEHC_CMU_GLOBAL_SVGAIN_REG_OFF, hsv_adjust_vivid_par[1] & 0x1FFF01FF);
    for (i = 2, j = 0; i < 9; i++, j += 4) { CMU_WUINT32 (sel, IMGEHC_CMU_VRANGE_HLGAIN_REG0_OFF + j, hsv_adjust_vivid_par[i] & 0xFFFF0FFF); }
    for (i = 9, j = 0; i < 16; i++, j += 4) { CMU_WUINT32 (sel, IMGEHC_CMU_LOCAL_SVGAIN_REG0_OFF + j, hsv_adjust_vivid_par[i] & 0x1FFF01FF); }
    sprintf (primary_key, "lcd%d_para", sel);
    ret = OSAL_Script_FetchParser_Data (primary_key, "smart_color", &reg_val, 1);
    
    if (ret < 0) { break; }
    else
    {
      #if defined(CONFIG_ARCH_SUN8IW1P1) || defined(CONFIG_ARCH_SUN6I)
      i_saturaion = 41 * reg_val;
      i_saturaion = reg_val == 50 ? 0 : (reg_val > 50 ? (i_saturaion - 0x7FF) : ( (i_saturaion << 1) - 0xFFF) );
      CMU_WUINT32 (sel, IMGEHC_CMU_GLOBAL_SVGAIN_REG_OFF, (i_saturaion & 0x1FFF) << 16);
      #elif defined(CONFIG_ARCH_SUN8IW3P1) || defined(CONFIG_ARCH_SUN9IW1P1)
      i_saturaion = 5 * reg_val;
      i_saturaion = reg_val == 50 ? 0 : (reg_val > 50 ? (i_saturaion - 0xFF) : ( (i_saturaion << 1) - 0xFF) );
      CMU_WUINT32 (sel, IMGEHC_CMU_GLOBAL_SVGAIN_REG_OFF, (i_saturaion & 0x1FF) << 16);
      #else
      i_saturaion = 5 * reg_val;
      i_saturaion = reg_val == 50 ? 0 : (reg_val > 50 ? (i_saturaion - 0xFF) : ( (i_saturaion << 1) - 0xFF) );
      CMU_WUINT32 (sel, IMGEHC_CMU_GLOBAL_SVGAIN_REG_OFF, (i_saturaion & 0x1FF) << 16);
      #endif
    }
    break;
    
  case 0x9://flesh mode
    CMU_WUINT32 (sel, IMGEHC_CMU_HRANGE_REG0_OFF, 0x02380fa4);
    CMU_WUINT32 (sel, IMGEHC_CMU_SRANGE_REG0_OFF, 0x00ae001a);
    #if defined(CONFIG_ARCH_SUN8IW1P1) || defined(CONFIG_ARCH_SUN6I)
    CMU_WUINT32 (sel, IMGEHC_CMU_SRANGE_REG0_OFF, 0x0ae001a0);
    #endif
    CMU_WUINT32 (sel, IMGEHC_CMU_HGGAIN_REG_OFF, hsv_adjust_flesh_par[0] & 0xFFF);
    CMU_WUINT32 (sel, IMGEHC_CMU_GLOBAL_SVGAIN_REG_OFF, hsv_adjust_flesh_par[1] & 0x1FFF01FF);
    for (i = 2, j = 0; i < 9; i++, j += 4) { CMU_WUINT32 (sel, IMGEHC_CMU_VRANGE_HLGAIN_REG0_OFF + j, hsv_adjust_flesh_par[i] & 0xFFFF0FFF); }
    for (i = 9, j = 0; i < 16; i++, j += 4) { CMU_WUINT32 (sel, IMGEHC_CMU_LOCAL_SVGAIN_REG0_OFF + j, hsv_adjust_flesh_par[i] & 0x1FFF01FF); }
    break;
    
  case 0xa://scenery mode
    CMU_WUINT32 (sel, IMGEHC_CMU_HRANGE_REG3_OFF, 0x080006aa);
    CMU_WUINT32 (sel, IMGEHC_CMU_HRANGE_REG5_OFF, 0x040002aa);
    CMU_WUINT32 (sel, IMGEHC_CMU_SRANGE_REG3_OFF, 0x00FF0018);
    CMU_WUINT32 (sel, IMGEHC_CMU_SRANGE_REG5_OFF, 0x00FF0018);
    #if defined(CONFIG_ARCH_SUN8IW1P1) || defined(CONFIG_ARCH_SUN6I)
    CMU_WUINT32 (sel, IMGEHC_CMU_SRANGE_REG3_OFF, 0x0FFF0180);
    CMU_WUINT32 (sel, IMGEHC_CMU_SRANGE_REG5_OFF, 0x0FFF0180);
    #endif
    CMU_WUINT32 (sel, IMGEHC_CMU_HGGAIN_REG_OFF, hsv_adjust_scenery_par[0] & 0xFFF);
    CMU_WUINT32 (sel, IMGEHC_CMU_GLOBAL_SVGAIN_REG_OFF, hsv_adjust_scenery_par[1] & 0x1FFF01FF);
    for (i = 2, j = 0; i < 9; i++, j += 4) { CMU_WUINT32 (sel, IMGEHC_CMU_VRANGE_HLGAIN_REG0_OFF + j, hsv_adjust_scenery_par[i] & 0xFFFF0FFF); }
    for (i = 9, j = 0; i < 16; i++, j += 4) { CMU_WUINT32 (sel, IMGEHC_CMU_LOCAL_SVGAIN_REG0_OFF + j, hsv_adjust_scenery_par[i] & 0x1FFF01FF); }
    break;
    
  default:
    break;
  }
  
  return 0;
}

s32 IEP_CMU_Operation_In_Vblanking (u32 sel)
{
  u32 reg_val;
  
  CMU_WUINT32 (sel, IMGEHC_CMU_REGBUFFCTL_REG_OFF, 0x2);
  reg_val = CMU_RUINT32 (sel, IMGEHC_CMU_REGBUFFCTL_REG_OFF);
  CMU_WUINT32 (sel, IMGEHC_CMU_REGBUFFCTL_REG_OFF, reg_val | 0x1);
  reg_val = 0x3;
  
  return 0;
}

s32 IEP_CMU_Enable (u32 sel, u32 enable)
{
  u32 reg_val;
  
  reg_val = CMU_RUINT32 (sel, IMGEHC_CMU_CTL_REG_OFF) & 0xfffffffe;
  reg_val |= enable;
  CMU_WUINT32 (sel, IMGEHC_CMU_CTL_REG_OFF, reg_val);
  
  return 0;
}

s32 IEP_CMU_Disable (u32 sel)
{
  u32 reg_val;
  
  reg_val = CMU_RUINT32 (sel, IMGEHC_CMU_CTL_REG_OFF) & 0xfffffffe;
  CMU_WUINT32 (sel, IMGEHC_CMU_CTL_REG_OFF, reg_val);
  
  return 0;
}

s32 iep_cmu_early_suspend (u32 sel)
{
  
  return 0;
}

s32 iep_cmu_suspend (u32 sel)
{
  return 0;
}

s32 iep_cmu_resume (u32 sel)
{
  return 0;
}

s32 IEP_CMU_Late_Resume (u32 sel)
{
  
  return 0;
}

