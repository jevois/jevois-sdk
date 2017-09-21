
#include "de_fce_type.h"
#include "de_rtmx.h"
#include "de_enhance.h"

#define FCE_OFST	0xA0000	

#ifndef get_wvalue
#define get_wvalue(addr)	(*((volatile unsigned long  *)(addr)))
#endif

#ifndef put_wvalue
#define put_wvalue(addr, v)	(*((volatile unsigned long  *)(addr)) = (unsigned long)(v))
#endif

static volatile __fce_reg_t *fce_dev[DEVICE_NUM][CHN_NUM];

static de_reg_blocks fce_para_block[DEVICE_NUM][CHN_NUM];
static de_reg_blocks fce_celut_block[DEVICE_NUM][CHN_NUM];
static de_reg_blocks fce_hist_block[DEVICE_NUM][CHN_NUM];

static unsigned int fce_hw_base[DEVICE_NUM][CHN_NUM] = {{0}};

extern unsigned int *g_hist[DEVICE_NUM][CHN_NUM];
extern unsigned int *g_hist_p[DEVICE_NUM][CHN_NUM];
extern unsigned int g_sum[DEVICE_NUM][CHN_NUM];
__hist_status_t *g_hist_status[DEVICE_NUM][CHN_NUM];

__ce_status_t *g_ce_status[DEVICE_NUM][CHN_NUM];
static unsigned char *g_celut[DEVICE_NUM][CHN_NUM];

int de_fce_set_reg_base(unsigned int sel, unsigned int chno, unsigned int base)
{
	fce_dev[sel][chno] = (__fce_reg_t *)base;

	return 0;
}

int de_fce_update_regs(unsigned int sel, unsigned int chno)
{
	if(fce_para_block[sel][chno].dirty == 0x1){
		memcpy((void *)fce_para_block[sel][chno].off,fce_para_block[sel][chno].val,fce_para_block[sel][chno].size);
		fce_para_block[sel][chno].dirty = 0x0;
	}

	return 0;
}

int de_fce_init(unsigned int sel, unsigned int chno, unsigned int reg_base)
{
	unsigned int base;
	void *memory;

	base = reg_base + (sel+1)*0x00100000 + FCE_OFST;
	fce_hw_base[sel][chno] = base;

	__inf("sel %d, fce_base[%d]=0x%x\n", sel, chno, base);

	memory = disp_sys_malloc(sizeof(__fce_reg_t));
	if(NULL == memory) {
		__wrn("malloc fce[%d][%d] memory fail! size=0x%x\n", sel, chno, sizeof(__fce_reg_t));
		return -1;
	}

	fce_para_block[sel][chno].off			= base;
	fce_para_block[sel][chno].val			= memory;
	fce_para_block[sel][chno].size			= 0x34;
	fce_para_block[sel][chno].dirty 		= 0;

	fce_celut_block[sel][chno].off			= base + 0x100;
	fce_celut_block[sel][chno].val			= memory + 0x100;
	fce_celut_block[sel][chno].size		    = 0x100;
	fce_celut_block[sel][chno].dirty 		= 0;

	fce_hist_block[sel][chno].off			= base + 0x200;
	fce_hist_block[sel][chno].val			= memory + 0x200;
	fce_hist_block[sel][chno].size			= 0x400;
	fce_hist_block[sel][chno].dirty 		= 0;

	de_fce_set_reg_base(sel, chno, (unsigned int)memory);

	g_hist_status[sel][chno] = disp_sys_malloc(sizeof(__hist_status_t));
	if(NULL == g_hist_status[sel][chno]) {
		__wrn("malloc g_hist_status[%d][%d] memory fail! size=0x%x\n", sel, chno, sizeof(__hist_status_t));
		return -1;
	}
	__inf("g_hist_status[%d][%d]=0x%x\n", sel, chno, (unsigned int)g_hist_status[sel][chno]);

	g_hist_status[sel][chno]->Runtime = 0;
	g_hist_status[sel][chno]->IsEnable = 0;
	g_hist_status[sel][chno]->TwoHistReady = 0;

	g_hist[sel][chno] = disp_sys_malloc(1024);	
	if(NULL == g_hist[sel][chno]) {
		__wrn("malloc hist[%d][%d] memory fail! size=0x%x\n", sel, chno, 1024);
		return -1;
	}

	g_hist_p[sel][chno] = disp_sys_malloc(1024);
	if(NULL == g_hist_p[sel][chno]) {
		__wrn("malloc hist_p[%d][%d] memory fail! size=0x%x\n", sel, chno, 1024);
		return -1;
	}

	g_ce_status[sel][chno] = disp_sys_malloc(sizeof(__ce_status_t));
	if(NULL == g_ce_status[sel][chno]) {
		__wrn("malloc g_ce_status[%d][%d] memory fail! size=0x%x\n", sel, chno, sizeof(__ce_status_t));
		return -1;
	}

	g_ce_status[sel][chno]->IsEnable = 0;

	g_celut[sel][chno] = disp_sys_malloc(256);
	if(NULL == g_celut[sel][chno]) {
		__wrn("malloc celut[%d][%d] memory fail! size=0x%x\n", sel, chno, 256);
		return -1;
	}

	return 0;
}

int de_fce_enable(unsigned int sel, unsigned int chno, unsigned int en)
{
	fce_dev[sel][chno]->ctrl.bits.en = en;
	fce_para_block[sel][chno].dirty 		= 1;
	return 0;
}

int de_fce_set_size(unsigned int sel, unsigned int chno, unsigned int width, unsigned int height)
{
	fce_dev[sel][chno]->size.bits.width = width - 1;
	fce_dev[sel][chno]->size.bits.height = height - 1;
	fce_para_block[sel][chno].dirty 		= 1;
	return 0;
}

int de_fce_set_window(unsigned int sel, unsigned int chno, unsigned int win_enable, de_rect window)
{
	fce_dev[sel][chno]->ctrl.bits.win_en = win_enable;

	if(win_enable)
	{
		fce_dev[sel][chno]->win0.bits.win_left = window.x;
		fce_dev[sel][chno]->win0.bits.win_top = window.y;
		fce_dev[sel][chno]->win1.bits.win_right = window.x + window.w - 1;
		fce_dev[sel][chno]->win1.bits.win_bot = window.y + window.h - 1;
	}
	fce_para_block[sel][chno].dirty 		= 1;
	return 0;
}

int de_fce_csc_en(unsigned int sel, unsigned int chno, unsigned int csc_enable)
{
	fce_dev[sel][chno]->cscbypass.bits.csc_bypass = csc_enable;
	fce_para_block[sel][chno].dirty 		= 1;

	return 0;
}

int de_fce_set_para(unsigned int sel, unsigned int chno,
						unsigned int lce_en, unsigned int ftc_en, unsigned int ce_en, unsigned int hist_en)
{
	fce_dev[sel][chno]->ctrl.bits.lce_en = lce_en;
	fce_dev[sel][chno]->ctrl.bits.ftc_en = ftc_en;
	fce_dev[sel][chno]->ctrl.bits.ce_en = ce_en;
	fce_dev[sel][chno]->ctrl.bits.hist_en = hist_en;

	fce_dev[sel][chno]->lcegain.bits.lce_gain = 15;
	fce_dev[sel][chno]->lcegain.bits.lce_blend = 84;
	fce_dev[sel][chno]->ftcgain.bits.ftc_gain1 = 10;
	fce_dev[sel][chno]->ftcgain.bits.ftc_gain2 = 5;
	fce_para_block[sel][chno].dirty 		= 1;

	return 0;
}

int de_fce_get_hist(unsigned int sel, unsigned int chno, unsigned int hist[256], unsigned int *sum)
{
	unsigned int base;
	base = fce_hw_base[sel][chno];
#if 0
	*sum = 0x2EE0000;
	for(i=0; i<256; i++) {
		hist[i] = 0x5DC;
	}
	return 0;
#endif

	memcpy((unsigned char *)hist, (unsigned char *)fce_hist_block[sel][chno].off, sizeof(unsigned int)*256);

	*sum = get_wvalue(base + 0x20);

	return 0;
}

int de_fce_set_ce(unsigned int sel, unsigned int chno, unsigned char ce_lut[256])
{
	unsigned int base;
	base = fce_hw_base[sel][chno];

	put_wvalue(base + 0x28, 0x1);
	memcpy((void*)fce_celut_block[sel][chno].off, (unsigned char *)ce_lut, sizeof(unsigned char)*256);
	put_wvalue(base + 0x28, 0x0);

	return 0;
}

int de_fce_info2para(unsigned int sharp, unsigned int auto_contrast, unsigned int auto_color, de_rect window, __fce_config_data *para)
{
	int fce_en, hist_en;

	hist_en = (auto_contrast==2 || auto_contrast==3)?1:0;
	fce_en = (hist_en!=0 || auto_color!=0 || auto_contrast==1 || auto_contrast==3)?1:0;

	para->fce_en = fce_en;
	para->hist_en = hist_en;

	para->ce_en = (auto_contrast==2 || auto_contrast==3)?1:0;
	para->up_precent_thr = 5;
	para->down_precent_thr = 5;

	para->lce_en = (auto_contrast==1 || auto_contrast==3)?1:0;
	para->ftc_en = (auto_color!=0)?1:0;

	para->win_en = 1;
	para->win.x = window.x;
	para->win.y = window.y;
	para->win.w = window.w;
	para->win.h = window.h;

	return 0;
}

void auto_ce_model(unsigned int width, unsigned height, unsigned int sumcnt, unsigned int hist[256],
						unsigned int up_precent_thr, unsigned int down_precent_thr, unsigned char celut[256])
{
	static __u32 hist_r[256], p[256];
	__u32 i;
	__u32 mean;
	__u32 total_pixel, total_pixel_r;
	__s32 uthr, lthr;
	__u32 rate;
	int tmp;

	if (height % 2 == 0)
	{
		total_pixel = (width * height)>>1;
	}
	else
	{
		total_pixel = (width * (height -1) + (width+1))>>1;
	}
	total_pixel_r = 0;

	mean = total_pixel/512;

	if (mean==0)
	{
		for(i=0; i<256; i++)
		{
			celut[i] = i;
		}
	}
	else
	{

		uthr = mean + mean * 2 * up_precent_thr / 100;
		lthr = mean - mean * 2 * down_precent_thr / 100;

		if(hist[0]<=uthr && hist[0]>=lthr)
		{
			hist_r[0] = hist[0];
		}
		else if(hist[0]>uthr)
		{
			hist_r[0] = uthr;
		}
		else
		{
			hist_r[0] = lthr;
		}

		total_pixel_r = total_pixel_r + hist_r[0];

		p[0] = hist_r[0];

		for(i=1; i<256; i++)
		{
			if(hist[i]<=uthr && hist[i]>=lthr)
			{
				hist_r[i] = hist[i];
			}
			else if(hist[i]>uthr)
			{
				hist_r[i] = uthr;
			}
			else
			{
				hist_r[i] = lthr;
			}

			total_pixel_r = total_pixel_r + hist_r[i];

			p[i] = p[i-1] + hist_r[i];
		}

		rate = (total_pixel<<8)/total_pixel_r;

		for(i=0; i<256; i++)
		{
			tmp = (p[i] * rate + 128)/total_pixel;
			celut[i] = (tmp < 256) ? tmp : 255;
		}
	}
	return ;
}

int de_hist_apply(unsigned int screen_id, unsigned int chno, unsigned int hist_en, unsigned int auto_contrast_dirty)
{
	if(hist_en==1 && auto_contrast_dirty)
	{
		memset((unsigned char *)g_hist[screen_id][chno], 0, 1024);
		memset((unsigned char *)g_hist_p[screen_id][chno], 0, 1024);
		g_hist_status[screen_id][chno]->Runtime = 0;
		g_hist_status[screen_id][chno]->TwoHistReady = 0;
		g_hist_status[screen_id][chno]->IsEnable = 1;
	}
	else if(hist_en==0 && auto_contrast_dirty)
	{
		g_hist_status[screen_id][chno]->Runtime = 0;
		g_hist_status[screen_id][chno]->TwoHistReady = 0;
		g_hist_status[screen_id][chno]->IsEnable = 0;
	}

	return 0;
}

int de_hist_sync(unsigned int screen_id, unsigned int chno, unsigned int frame_cnt)
{
	if((g_hist_status[screen_id][chno]->IsEnable) && ((HIST_FRAME_MASK==(frame_cnt%2)) || (HIST_FRAME_MASK==0x2)))
	{
		memcpy((unsigned char *)g_hist_p[screen_id][chno], (unsigned char *)g_hist[screen_id][chno], 1024);
		de_fce_get_hist(screen_id, chno, g_hist[screen_id][chno], &g_sum[screen_id][chno]);

		if(g_hist_status[screen_id][chno]->Runtime < 2)
		{
			g_hist_status[screen_id][chno]->Runtime ++;
		}
		else
		{
			g_hist_status[screen_id][chno]->TwoHistReady = 1;
		}
	}
	return 0;

}

int de_ce_apply(unsigned int screen_id, unsigned int chno, unsigned int ce_en, unsigned int auto_contrast_dirty)
{
	if(ce_en==1 && auto_contrast_dirty)
	{
		g_ce_status[screen_id][chno]->IsEnable = 1;
	}
	else if(ce_en==0 && auto_contrast_dirty)
	{
		g_ce_status[screen_id][chno]->IsEnable = 0;
	}
	return 0;

}

int de_ce_sync(unsigned int screen_id, unsigned int chno, unsigned int frame_cnt)
{
	int i;
	if(g_ce_status[screen_id][chno]->IsEnable  && ((CE_FRAME_MASK==(frame_cnt%2)) || (CE_FRAME_MASK==0x2)))
	{
		if(g_hist_status[screen_id][chno]->TwoHistReady)
		{
			auto_ce_model(g_ce_status[screen_id][chno]->width, g_ce_status[screen_id][chno]->height, g_sum[screen_id][chno],
				g_hist[screen_id][chno], 5, 5, g_celut[screen_id][chno]);
		}
		else
		{
			for(i=0;i<256;i++)
			{
				g_celut[screen_id][chno][i] = i;
			}
		}
			de_fce_set_ce(screen_id, chno, g_celut[screen_id][chno]);
	}
	return 0;

}

