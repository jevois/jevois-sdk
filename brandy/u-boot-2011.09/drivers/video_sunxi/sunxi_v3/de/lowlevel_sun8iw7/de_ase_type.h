
#ifndef __DE_ASE_TYPE_H__
#define __DE_ASE_TYPE_H__

#include "de_rtmx.h"

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int en          :  1 ;   
    unsigned int win_en            :  1 ;   
    unsigned int res0              :  30;   
  } bits;
} ASE_CTRL_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int width        : 12;
    unsigned int res0       : 4;
    unsigned int height       : 12;
    unsigned int res1       : 4;
  } bits;
} ASE_SIZE_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int left       : 12;
    unsigned int res0       : 4;
    unsigned int top        : 12;
    unsigned int res1       : 4;
  } bits;
} ASE_WIN0_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int right        : 12;
    unsigned int res0       : 4;
    unsigned int bot        : 12;
    unsigned int res1       : 4;
  } bits;
} ASE_WIN1_REG;


typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int gain       : 5; 
    unsigned int res0       : 27;
  } bits;
} ASE_GAIN_REG;

typedef struct
{
  ASE_CTRL_REG    ctrl;    
  ASE_SIZE_REG    size;    
  ASE_WIN0_REG    win0;    
  ASE_WIN1_REG    win1;    
  ASE_GAIN_REG    gain;    
  
} __ase_reg_t;

typedef struct
{
  unsigned int ase_en;
  unsigned int gain;
  
  unsigned int win_en;
  de_rect win;
} __ase_config_data;
#endif
