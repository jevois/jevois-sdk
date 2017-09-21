
#include "de_ase_type.h"
#include "de_rtmx.h"
#include "de_enhance.h"

#define ASE_OFST	0xA8000	

static volatile __ase_reg_t *ase_dev[DEVICE_NUM][CHN_NUM];
static de_reg_blocks ase_block[DEVICE_NUM][CHN_NUM];

int de_ase_set_reg_base(unsigned int sel, unsigned int chno, unsigned int base)
{
	ase_dev[sel][chno] = (__ase_reg_t *)base;

	return 0;
}

int de_ase_update_regs(unsigned int sel, unsigned int chno)
{
	if(ase_block[sel][chno].dirty == 0x1){
		memcpy((void *)ase_block[sel][chno].off,ase_block[sel][chno].val,ase_block[sel][chno].size);
		ase_block[sel][chno].dirty = 0x0;}

	return 0;
}

int de_ase_init(unsigned int sel, unsigned int chno, unsigned int reg_base)
{
	unsigned int base;
	void *memory;

	base = reg_base + (sel+1)*0x00100000 + ASE_OFST;
	__inf("sel %d, ase_base[%d]=0x%x\n", sel, chno, base);

	memory = disp_sys_malloc(sizeof(__ase_reg_t));
	if(NULL == memory) {
		__wrn("malloc ase[%d][%d] memory fail! size=0x%x\n", sel, chno, sizeof(__ase_reg_t));
		return -1;
	}

	ase_block[sel][chno].off			= base;
	ase_block[sel][chno].val			= memory;
	ase_block[sel][chno].size			= 0x14;
	ase_block[sel][chno].dirty 			= 0;

	de_ase_set_reg_base(sel, chno, (unsigned int)memory);

	return 0;
}

int de_ase_enable(unsigned int sel, unsigned int chno, unsigned int en)
{
	ase_dev[sel][chno]->ctrl.bits.en = en;
	ase_block[sel][chno].dirty 		 = 1;
	return 0;
}

int de_ase_set_size(unsigned int sel, unsigned int chno, unsigned int width, unsigned int height)
{
	ase_dev[sel][chno]->size.bits.width = width - 1;
	ase_dev[sel][chno]->size.bits.height = height - 1;
	ase_block[sel][chno].dirty 		 = 1;
	return 0;
}

int de_ase_set_window(unsigned int sel, unsigned int chno, unsigned int win_enable, de_rect window)
{
	ase_dev[sel][chno]->ctrl.bits.win_en = win_enable;

	if(win_enable)
	{
		ase_dev[sel][chno]->win0.bits.left = window.x;
		ase_dev[sel][chno]->win0.bits.top = window.y;
		ase_dev[sel][chno]->win1.bits.right = window.x + window.w - 1;
		ase_dev[sel][chno]->win1.bits.bot = window.y + window.h - 1;
	}
	ase_block[sel][chno].dirty 		 = 1;
	return 0;
}

int de_ase_set_para(unsigned int sel, unsigned int chno, unsigned int gain)
{
	ase_dev[sel][chno]->gain.bits.gain = gain;
	ase_block[sel][chno].dirty 		 = 1;
	return 0;
}

int de_ase_info2para(unsigned int auto_color, de_rect window, __ase_config_data *para)
{
	int ase_mode;
	int ase_para[ASE_PARA_NUM][ASE_MODE_NUM] = {
	{0, 12, 16, 20}
	};

	ase_mode = ((auto_color>>4)&0xf)?1:0;
	para->ase_en = ase_mode?1:0;
	para->gain = ase_para[0][ase_mode];

	return 0;
}

