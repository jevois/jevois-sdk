
#ifndef __DE_GSU_TYPE_H__
#define __DE_GSU_TYPE_H__

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int en                :  1 ;   
    unsigned int res0              :  3 ;   
    unsigned int coef_switch_rdy   :  1 ;   
    unsigned int res1              :  25;   
    unsigned int reset             :  1 ;   
    unsigned int res2              :  1 ;   
  } bits;
} GSU_CTRL_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int res0              :  4 ;   
    unsigned int busy              :  1 ;   
    unsigned int res1              :  11;   
    unsigned int line_cnt          :  12;   
    unsigned int res2              :  4 ;   
  } bits;
} GSU_STATUS_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int vphase_sel_en     :  1 ;   
    unsigned int res0              :  31;   
  } bits;
} GSU_FIELD_CTRL_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int bist_en           :  1 ;     
    unsigned int res0              :  15;    
    unsigned int bist_sel          :  5 ;     
    unsigned int res1              :  11;    
  } bits;
} GSU_BIST_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int width             :  13;    
    unsigned int res0              :  3 ;    
    unsigned int height            :  13;     
    unsigned int res1              :  3 ;    
  } bits;
} GSU_OUTSIZE_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int width             :  13;    
    unsigned int res0              :  3 ;    
    unsigned int height            :  13;     
    unsigned int res1              :  3 ;    
  } bits;
} GSU_INSIZE_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int res0              :  2 ;    
    unsigned int frac              :  18;    
    unsigned int integer           :  5 ;    
    unsigned int res1              :  7;    
  } bits;
} GSU_HSTEP_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int res0              :  2 ;    
    unsigned int frac              :  18;    
    unsigned int integer           :  5 ;    
    unsigned int res1              :  7 ;    
  } bits;
} GSU_VSTEP_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int res0              :  2 ;    
    unsigned int frac              :  18;    
    unsigned int integer           :  5 ;    
    unsigned int res1              :  7;    
  } bits;
} GSU_HPHASE_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int res0              :  2 ;    
    unsigned int frac              :  18;    
    unsigned int integer           :  5 ;    
    unsigned int res1              :  7;    
  } bits;
} GSU_VPHASE0_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int res0              :  2 ;    
    unsigned int frac              :  18;    
    unsigned int integer           :  4 ;    
    unsigned int res1              :  8;    
  } bits;
} GSU_VPHASE1_REG;

typedef union
{
  unsigned int dwval;
  struct
  {
    unsigned int coef0             :   8 ;    
    unsigned int coef1             :   8 ;    
    unsigned int coef2             :   8 ;    
    unsigned int coef3             :   8 ;    
  } bits;
} GSU_HCOEFF_REG;

typedef struct
{
  GSU_CTRL_REG        ctrl      ;
  unsigned int    res0    ;
  GSU_STATUS_REG    status    ;
  GSU_FIELD_CTRL_REG  field   ;
  GSU_BIST_REG    bist    ;
  unsigned int        res1[11]  ;
  GSU_OUTSIZE_REG   outsize   ;
  unsigned int        res5[15]    ;  
  GSU_INSIZE_REG    insize    ;
  unsigned int        res2        ;  
  GSU_HSTEP_REG   hstep   ;
  GSU_VSTEP_REG   vstep   ;
  GSU_HPHASE_REG    hphase    ;
  unsigned int        res3    ;
  GSU_VPHASE0_REG   vphase0   ;
  GSU_VPHASE1_REG   vphase1   ;
  unsigned int    res4[88]  ;
  GSU_HCOEFF_REG    hcoeff[16]  ;
} __gsu_reg_t;

#endif
