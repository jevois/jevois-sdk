#include "de_feat.h"
#include "de_gsu_type.h"
#include "de_scaler.h"
#include "de_scaler_table.h"

#define GSU_OFST 0x40000
static volatile __gsu_reg_t * gsu_dev[DEVICE_NUM][UI_CHN_NUM];
static de_reg_blocks gsu_glb_block[DEVICE_NUM][UI_CHN_NUM];
static de_reg_blocks gsu_out_block[DEVICE_NUM][UI_CHN_NUM];
static de_reg_blocks gsu_scale_block[DEVICE_NUM][UI_CHN_NUM];
static de_reg_blocks gsu_coeff_block[DEVICE_NUM][UI_CHN_NUM];

int de_gsu_init (unsigned int sel, unsigned int reg_base)
{
  int j, chno;
  unsigned int gsu_base;
  void * memory;
  int vi_chno;
  
  chno = de_feat_is_support_scale (sel);
  vi_chno = de_feat_get_num_vi_chns (sel);
  chno = chno - vi_chno;
  
  for (j = 0; j < chno; j++)
  {
    gsu_base = reg_base + (sel + 1) * 0x00100000 + GSU_OFST + j * 0x10000;
    __inf ("sel %d, gsu_base[%d]=0x%x\n", sel, j, gsu_base);
    
    memory = disp_sys_malloc (sizeof (__gsu_reg_t) );
    if (NULL == memory) {
      __wrn ("malloc ui scaler[%d][%d] memory fail! size=0x%x\n", sel, j, sizeof (__gsu_reg_t) );
      return -1;
    }
    
    gsu_glb_block[sel][j].off     = gsu_base;
    gsu_glb_block[sel][j].val     = memory;
    gsu_glb_block[sel][j].size      = 0x14;
    gsu_glb_block[sel][j].dirty     = 0;
    
    gsu_out_block[sel][j].off     = gsu_base + 0x40;
    gsu_out_block[sel][j].val     = memory + 0x40;
    gsu_out_block[sel][j].size      = 0x4;
    gsu_out_block[sel][j].dirty     = 0;
    
    gsu_scale_block[sel][j].off   = gsu_base + 0x80;
    gsu_scale_block[sel][j].val   = memory + 0x80;
    gsu_scale_block[sel][j].size    = 0x20;
    gsu_scale_block[sel][j].dirty     = 0;
    
    gsu_coeff_block[sel][j].off   = gsu_base + 0x200;
    gsu_coeff_block[sel][j].val   = memory + 0x200;
    gsu_coeff_block[sel][j].size    = 0x40;
    gsu_coeff_block[sel][j].dirty     = 0;
    
    
    de_gsu_set_reg_base (sel, j, memory);
  }
  
  return 0;
}

int de_gsu_update_regs (unsigned int sel)
{
  int i, chno;
  
  chno = de_feat_is_support_scale (sel);
  
  for (i = 0; i < chno; i++)
  {
    if (gsu_glb_block[sel][i].dirty == 0x1) {
      memcpy ( (void *) gsu_glb_block[sel][i].off, gsu_glb_block[sel][i].val, gsu_glb_block[sel][i].size);
      gsu_glb_block[sel][i].dirty = 0x0;
    }
    if (gsu_out_block[sel][i].dirty == 0x1) {
      memcpy ( (void *) gsu_out_block[sel][i].off, gsu_out_block[sel][i].val, gsu_out_block[sel][i].size);
      gsu_out_block[sel][i].dirty = 0x0;
    }
    if (gsu_scale_block[sel][i].dirty == 0x1) {
      memcpy ( (void *) gsu_scale_block[sel][i].off, gsu_scale_block[sel][i].val, gsu_scale_block[sel][i].size);
      gsu_scale_block[sel][i].dirty = 0x0;
    }
    if (gsu_coeff_block[sel][i].dirty == 0x1) {
      memcpy ( (void *) gsu_coeff_block[sel][i].off, gsu_coeff_block[sel][i].val, gsu_coeff_block[sel][i].size);
      gsu_coeff_block[sel][i].dirty = 0x0;
    }
    
  }
  
  return 0;
}

int de_gsu_set_reg_base (unsigned int sel, unsigned int chno, void * base)
{
  __inf ("sel=%d, chno=%d, base=0x%x\n", sel, chno, (u32) base);
  gsu_dev[sel][chno] = (__gsu_reg_t *) base;
  
  return 0;
}

int de_gsu_enable (unsigned int sel, unsigned int chno, unsigned int en)
{
  gsu_dev[sel][chno - VI_CHN_NUM]->ctrl.bits.en = en;
  gsu_glb_block[sel][chno - VI_CHN_NUM].dirty   = 1;
  
  return 0;
}


static unsigned int de_gsu_calc_fir_coef (unsigned int step)
{
  unsigned int pt_coef;
  unsigned int scale_ratio, int_part, float_part, fir_coef_ofst;
  
  scale_ratio = step >> (GSU_PHASE_FRAC_BITWIDTH - 3);
  int_part = scale_ratio >> 3;
  float_part = scale_ratio & 0x7;
  fir_coef_ofst = (int_part == 0)  ? GSU_ZOOM0_SIZE :
                  (int_part == 1)  ? GSU_ZOOM0_SIZE + float_part :
                  (int_part == 2)  ? GSU_ZOOM0_SIZE + GSU_ZOOM1_SIZE + (float_part >> 1) :
                  (int_part == 3)  ? GSU_ZOOM0_SIZE + GSU_ZOOM1_SIZE + GSU_ZOOM2_SIZE :
                  (int_part == 4)  ? GSU_ZOOM0_SIZE + GSU_ZOOM1_SIZE + GSU_ZOOM2_SIZE + GSU_ZOOM3_SIZE :
                  GSU_ZOOM0_SIZE + GSU_ZOOM1_SIZE + GSU_ZOOM2_SIZE + GSU_ZOOM3_SIZE + GSU_ZOOM4_SIZE;
                  
  pt_coef = fir_coef_ofst * GSU_PHASE_NUM ;
  
  return pt_coef;
}


int de_gsu_set_para (unsigned int sel, unsigned int chno, unsigned int enable,
                     unsigned int in_w, unsigned int in_h, unsigned int out_w, unsigned int out_h, scaler_para * para)
{
  unsigned int pt_coef;
  unsigned int chno_t;
  unsigned int vi_chno;
  
  vi_chno = de_feat_get_num_vi_chns (sel);
  
  chno_t = chno - vi_chno;
  
  gsu_dev[sel][chno_t]->ctrl.bits.en = enable;
  
  if (0 == enable)
  {
    gsu_glb_block[sel][chno_t].dirty   = 1;
    return 0;
  }
  
  gsu_dev[sel][chno_t]->outsize.dwval = ( (out_h - 1) << 16) | (out_w - 1);
  gsu_dev[sel][chno_t]->insize.dwval = ( (in_h - 1) << 16) | (in_w - 1);
  gsu_dev[sel][chno_t]->hstep.dwval = para->hstep << GSU_PHASE_FRAC_REG_SHIFT;
  gsu_dev[sel][chno_t]->vstep.dwval = para->vstep << GSU_PHASE_FRAC_REG_SHIFT;
  gsu_dev[sel][chno_t]->hphase.dwval = para->hphase << GSU_PHASE_FRAC_REG_SHIFT;
  gsu_dev[sel][chno_t]->vphase0.dwval = para->vphase << GSU_PHASE_FRAC_REG_SHIFT;
  
  pt_coef = de_gsu_calc_fir_coef (para->vstep);
  memcpy (&gsu_dev[sel][chno_t]->hcoeff, lan2coefftab16 + pt_coef, sizeof (unsigned int) *GSU_PHASE_NUM);
  gsu_dev[sel][chno_t]->ctrl.bits.coef_switch_rdy = 1;
  gsu_glb_block[sel][chno_t].dirty   = 1;
  gsu_out_block[sel][chno_t].dirty   = 1;
  gsu_scale_block[sel][chno_t].dirty = 1;
  gsu_coeff_block[sel][chno_t].dirty = 1;
  
  return 0;
}

int de_gsu_calc_scaler_para (de_rect64 crop, de_rect frame, de_rect * crop_fix, scaler_para * para)
{
  unsigned long long tmp = 0;
  
  tmp = (N2_POWER (crop.w, GSU_PHASE_FRAC_BITWIDTH) );
  do_div (tmp, frame.w);
  para->hstep = (unsigned int) (tmp >> GSU_FB_FRAC_BITWIDTH);
  
  tmp = (N2_POWER (crop.h, GSU_PHASE_FRAC_BITWIDTH) );
  do_div (tmp, frame.h);
  para->vstep = (unsigned int) (tmp >> GSU_FB_FRAC_BITWIDTH);
  
  para->hphase = ( (crop.x & 0xffffffff) >> (32 - GSU_PHASE_FRAC_BITWIDTH) );
  para->vphase = ( (crop.y & 0xffffffff) >> (32 - GSU_PHASE_FRAC_BITWIDTH) );;
  
  crop_fix->w = (unsigned int) ( ( (crop.w & 0xffffffff) + (crop.x & 0xffffffff) ) >= N2_POWER (1, GSU_FB_FRAC_BITWIDTH) ?
                                 ( (crop.w >> GSU_FB_FRAC_BITWIDTH) + 1) : (crop.w >> GSU_FB_FRAC_BITWIDTH) );
                                 
  crop_fix->h = (unsigned int) ( ( (crop.h & 0xffffffff) + (crop.y & 0xffffffff) ) >= N2_POWER (1, GSU_FB_FRAC_BITWIDTH) ?
                                 ( (crop.h >> GSU_FB_FRAC_BITWIDTH) + 1) : (crop.h >> GSU_FB_FRAC_BITWIDTH) );
                                 
  crop_fix->x = (int) (crop.x >> GSU_FB_FRAC_BITWIDTH);
  crop_fix->y = (int) (crop.y >> GSU_FB_FRAC_BITWIDTH);
  
  return 0;
}

int de_calc_ovl_coord (unsigned int frame_coord, unsigned int scale_step, int gsu_sel)
{
  int shift, half, layer_coord;
  
  shift = (gsu_sel == 0) ? VSU_PHASE_FRAC_BITWIDTH : GSU_PHASE_FRAC_BITWIDTH;
  half = (1 << (shift - 1) );
  layer_coord = (frame_coord * scale_step + half) >> shift;
  __inf ("half_shift_xy=<%x,%d,%d>\n", half, shift, layer_coord);
  
  return layer_coord;
}

int de_gsu_sel_ovl_scaler_para (unsigned char * en, scaler_para * layer_scale_para, scaler_para * ovl_scale_para)
{
  int i, j, layer_en_num;
  int used_layer_idx[LAYER_MAX_NUM_PER_CHN];
  
  layer_en_num = 0;
  j = 0;
  for (i = 0; i < LAYER_MAX_NUM_PER_CHN; i++)
  {
    if (* (en + i) == 1)
    {
      layer_en_num++;
      used_layer_idx[j++] = i;
    }
  }
  
  if (layer_en_num == 1)
  {
    ovl_scale_para->hphase = layer_scale_para[used_layer_idx[0]].hphase;
    ovl_scale_para->vphase = layer_scale_para[used_layer_idx[0]].vphase;
    ovl_scale_para->hstep = layer_scale_para[used_layer_idx[0]].hstep;
    ovl_scale_para->vstep = layer_scale_para[used_layer_idx[0]].vstep;
  }
  else
    if (layer_en_num > 1)
    {
      ovl_scale_para->hstep = layer_scale_para[used_layer_idx[0]].hstep;
      ovl_scale_para->vstep = layer_scale_para[used_layer_idx[0]].vstep;
      
      for (i = 0; i < layer_en_num; i++)
      {
        if (layer_scale_para[used_layer_idx[i]].hphase != 0)
        {
          ovl_scale_para->hphase = layer_scale_para[used_layer_idx[i]].hphase;
          break;
        }
      }
      
      if (i == layer_en_num)
      {
        ovl_scale_para->hphase = layer_scale_para[used_layer_idx[0]].hphase;
      }
      
      for (i = 0; i < layer_en_num; i++)
      {
        if (layer_scale_para[used_layer_idx[i]].vphase != 0)
        {
          ovl_scale_para->vphase = layer_scale_para[used_layer_idx[i]].vphase;
          break;
        }
      }
      
      if (i == layer_en_num)
      {
        ovl_scale_para->vphase = layer_scale_para[used_layer_idx[0]].vphase;
      }
      
    }
    else
    {
      ovl_scale_para->hphase = 0;
      ovl_scale_para->vphase = 0;
      ovl_scale_para->hstep = (unsigned int) N2_POWER (1, GSU_PHASE_FRAC_BITWIDTH);
      ovl_scale_para->vstep = (unsigned int) N2_POWER (1, GSU_PHASE_FRAC_BITWIDTH);
    }
    
  return 0;
}
