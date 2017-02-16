
#include "de_lti_type.h"
#include "de_enhance.h"

#define LTI_OFST  0xA4000  

static volatile __lti_reg_t * lti_dev[DEVICE_NUM][CHN_NUM];
static de_reg_blocks lti_block[DEVICE_NUM][CHN_NUM];

int de_lti_set_reg_base (unsigned int sel, unsigned int chno, unsigned int base)
{
  lti_dev[sel][chno] = (__lti_reg_t *) base;
  
  return 0;
}

int de_lti_update_regs (unsigned int sel, unsigned int chno)
{
  if (lti_block[sel][chno].dirty == 0x1) {
    memcpy ( (void *) lti_block[sel][chno].off, lti_block[sel][chno].val, lti_block[sel][chno].size);
    lti_block[sel][chno].dirty = 0x0;
  }
  
  return 0;
}

int de_lti_init (unsigned int sel, unsigned int chno, unsigned int reg_base)
{
  unsigned int base;
  void * memory;
  
  base = reg_base + (sel + 1) * 0x00100000 + LTI_OFST;
  __inf ("sel %d, lti_base[%d]=0x%x\n", sel, chno, base);
  
  memory = disp_sys_malloc (sizeof (__lti_reg_t) );
  if (NULL == memory) {
    __wrn ("malloc lti[%d][%d] memory fail! size=0x%x\n", sel, chno, sizeof (__lti_reg_t) );
    return -1;
  }
  
  lti_block[sel][chno].off      = base;
  lti_block[sel][chno].val      = memory;
  lti_block[sel][chno].size     = 0x40;
  lti_block[sel][chno].dirty      = 0;
  
  de_lti_set_reg_base (sel, chno, (unsigned int) memory);
  
  return 0;
}

int de_lti_enable (unsigned int sel, unsigned int chno, unsigned int en)
{
  lti_dev[sel][chno]->ctrl.bits.en = en;
  lti_block[sel][chno].dirty    = 1;
  return 0;
}

int de_lti_set_size (unsigned int sel, unsigned int chno, unsigned int width, unsigned int height)
{
  lti_dev[sel][chno]->size.bits.width = width - 1;
  lti_dev[sel][chno]->size.bits.height = height - 1;
  lti_block[sel][chno].dirty    = 1;
  return 0;
}

int de_lti_set_window (unsigned int sel, unsigned int chno, unsigned int win_enable, de_rect window)
{
  lti_dev[sel][chno]->ctrl.bits.win_en = win_enable;
  
  if (win_enable)
  {
    lti_dev[sel][chno]->win0.bits.win_left = window.x;
    lti_dev[sel][chno]->win0.bits.win_top = window.y;
    lti_dev[sel][chno]->win1.bits.win_right = window.x + window.w - 1;
    lti_dev[sel][chno]->win1.bits.win_bot = window.y + window.h - 1;
  }
  lti_block[sel][chno].dirty    = 1;
  return 0;
}

int de_lti_set_para (unsigned int sel, unsigned int chno, unsigned int gain)
{
  lti_dev[sel][chno]->gain.bits.lti_fil_gain = 3;
  
  lti_dev[sel][chno]->ctrl.bits.sel = 0;
  lti_dev[sel][chno]->ctrl.bits.nonl_en = 0;
  
  lti_dev[sel][chno]->coef0.bits.c0 = 127;
  lti_dev[sel][chno]->coef0.bits.c1 = 64;
  lti_dev[sel][chno]->coef1.bits.c2 =  0xe0;
  lti_dev[sel][chno]->coef1.bits.c3 = 0xc0;
  lti_dev[sel][chno]->coef2.bits.c4 = 0xe0;
  
  lti_dev[sel][chno]->corth.bits.lti_cor_th = 4;
  lti_dev[sel][chno]->diff.bits.offset = 32;
  lti_dev[sel][chno]->diff.bits.slope = 4;
  lti_dev[sel][chno]->edge_gain.bits.edge_gain = 1;
  lti_dev[sel][chno]->os_con.bits.core_x = 0;
  lti_dev[sel][chno]->os_con.bits.clip = 40;
  lti_dev[sel][chno]->os_con.bits.peak_limit = 1;
  
  lti_dev[sel][chno]->win_range.bits.win_range = 2;
  lti_dev[sel][chno]->elvel_th.bits.elvel_th = 32;
  
  lti_block[sel][chno].dirty    = 1;
  return 0;
}

int de_lti_info2para (unsigned int sharp, de_rect window, __lti_config_data * para)
{
  para->lti_en = (sharp == 2 || sharp == 3) ? 1 : 0;
  
  para->win_en = 1;
  para->win.x = window.x;
  para->win.y = window.y;
  para->win.w = window.w;
  para->win.h = window.h;
  
  return 0;
}

