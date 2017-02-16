
#ifndef __DE_FCE_TYPE_H__
#define __DE_FCE_TYPE_H__

#include "de_rtmx.h"

#define HIST_FRAME_MASK  0x00000002  
#define CE_FRAME_MASK    0x00000002   

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int en          :  1 ;   
    unsigned int res0              :  15;   
    unsigned int hist_en       :  1 ;   
    unsigned int ce_en             :  1 ;   
    unsigned int lce_en            :  1 ;   
    unsigned int res1              :  1 ;   
    unsigned int ftc_en            :  1 ;   
    unsigned int res2              :  10;   
    unsigned int win_en            :  1 ;   
  } bits;
} FCE_GCTRL_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int width        :  12;   
    unsigned int res0       :  4 ;   
    unsigned int height       :  12;   
    unsigned int res1       :  4 ;   
  } bits;
} FCE_SIZE_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int win_left     :  12;   
    unsigned int res0               :  4 ;   
    unsigned int win_top      :  12;   
    unsigned int res1               :  4 ;   
  } bits;
} FCE_WIN0_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int win_right      :  12;   
    unsigned int res0               :  4 ;   
    unsigned int win_bot      :  12;   
    unsigned int res1               :  4 ;   
  } bits;
} FCE_WIN1_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int lce_gain     :  6 ;    
    unsigned int res0       :  2 ;    
    unsigned int lce_blend      :  8 ;    
    unsigned int res1       :  16 ;   
  } bits;
} LCE_GAIN_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int sum        :  32 ;   
  } bits;
} HIST_SUM_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int hist_valid     :  1  ;   
    unsigned int res0       :  31 ;    
  } bits;
} HIST_STATUS_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int celut_access_switch :  1 ;   
    unsigned int res0       :  31;    
  } bits;
} CE_STATUS_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int ftc_gain1      :  8 ;    
    unsigned int res0       :  8 ;    
    unsigned int ftc_gain2      :  8 ;    
    unsigned int res1       :  8 ;    
  } bits;
} FTC_GAIN_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int csc_bypass     :  1 ;    
    unsigned int res0       :  31;    
  } bits;
} FTC_CSCBYPASS_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int lut0       :  8 ;    
    unsigned int lut1       :  8 ;    
    unsigned int lut2       :  8 ;    
    unsigned int lut3       :  8 ;    
  } bits;
} CE_LUT_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int hist       :  22;    
    unsigned int res0       :  10;    
  } bits;
} HIST_CNT_REG;

typedef struct
{
  FCE_GCTRL_REG   ctrl      ;
  FCE_SIZE_REG    size    ;
  FCE_WIN0_REG    win0    ;
  FCE_WIN1_REG    win1    ;
  LCE_GAIN_REG    lcegain   ;
  unsigned int        res0[3]   ;
  HIST_SUM_REG    histsum   ;
  HIST_STATUS_REG   histstauts  ;
  CE_STATUS_REG   cestatus  ;
  unsigned int        res1    ;
  FTC_GAIN_REG        ftcgain     ;  
  unsigned int        res2[3]   ;
  FTC_CSCBYPASS_REG cscbypass ;
  unsigned int        res3[47]  ;
  CE_LUT_REG      celut[64] ;
  HIST_CNT_REG    hist[256] ;
} __fce_reg_t;

typedef struct
{
  unsigned int fce_en;
  
  unsigned int ce_en;
  unsigned int up_precent_thr;
  unsigned int down_precent_thr;
  
  unsigned int lce_en;
  
  unsigned int ftc_en;
  
  unsigned int hist_en;
  
  unsigned int win_en;
  de_rect win;
} __fce_config_data;

typedef struct
{
  unsigned int Runtime;  
  unsigned int IsEnable;   
  unsigned int TwoHistReady; 
} __hist_status_t;

typedef struct
{
  unsigned int IsEnable;   
  unsigned int width;
  unsigned int height;
} __ce_status_t;


#endif
