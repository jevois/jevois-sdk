
#ifndef __DE_BWS_TYPE_H__
#define __DE_BWS_TYPE_H__

#include "de_rtmx.h"

#define BWS_FRAME_MASK  0x00000002 
#define BWS_DEFAULT_SLOPE 0x100

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int en          :  1 ;   
    unsigned int res         :  30; 
    unsigned int win_en            :  1 ;   
  } bits;
} BWS_CTRL_REG;

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
} BWS_SIZE_REG;

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
} BWS_WIN0_REG;

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
} BWS_WIN1_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int min        :  8 ;    
    unsigned int res0       :  8 ;    
    unsigned int black        :  8 ;    
    unsigned int res1       :  8 ;    
  } bits;
} BWS_LS_THR0_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int white        :  8 ;    
    unsigned int res0       :  8 ;    
    unsigned int max        :  8 ;    
    unsigned int res1       :  8 ;    
  } bits;
} BWS_LS_THR1_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int slope0       :  10;    
    unsigned int res0       :  6 ;    
    unsigned int slope1       :  10;    
    unsigned int res1       :  6 ;    
  } bits;
} BWS_LS_SLP0_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int slope2       :  10;    
    unsigned int res0       :  6 ;    
    unsigned int slope3       :  10;    
    unsigned int res1       :  6 ;    
  } bits;
} BWS_LS_SLP1_REG;


typedef struct
{
  BWS_CTRL_REG      ctrl      ;
  BWS_SIZE_REG      size    ;
  BWS_WIN0_REG      win0    ;
  BWS_WIN1_REG      win1    ;
  unsigned int      res0[4]   ;
  BWS_LS_THR0_REG     blkthr    ;
  BWS_LS_THR1_REG         whtthr      ;  
  BWS_LS_SLP0_REG     blkslp    ;
  BWS_LS_SLP1_REG         whtslp      ;  
} __bws_reg_t;

typedef struct
{
  unsigned int bws_en;
  unsigned int bld_high_thr;
  unsigned int bld_low_thr;
  unsigned int bld_weight_lmt;
  unsigned int present_black;
  unsigned int present_white;
  unsigned int slope_black_lmt;
  unsigned int slope_white_lmt;
  unsigned int black_prec;
  unsigned int white_prec;
  unsigned int lowest_black;
  unsigned int highest_white;
  
  unsigned int win_en;
  de_rect win;
  
} __bws_config_data;

typedef struct
{
  unsigned int IsEnable;   
  unsigned int Runtime;  
  unsigned int PreSlopeReady;
  unsigned int width;
  unsigned int height;
  unsigned int slope_black;
  unsigned int slope_white;
} __bws_status_t;
#endif
