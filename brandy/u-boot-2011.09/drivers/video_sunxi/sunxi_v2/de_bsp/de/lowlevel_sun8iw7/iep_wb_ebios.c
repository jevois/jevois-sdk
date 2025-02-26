
#include "iep_wb_ebios.h"
#define WB_MODULE_NUMBER 1

static volatile __iep_wb_dev_t *wb_dev[WB_MODULE_NUMBER] = {0};
static __u32 g_bufoff0[WB_MODULE_NUMBER] = {0};
static __u32 g_bufoff1[WB_MODULE_NUMBER] = {0};

__s32 WB_EBIOS_Set_Reg_Base(__u32 sel, __u32 base)
{
	wb_dev[sel] = (__iep_wb_dev_t *)base;

	return 0;
}

__u32 WB_EBIOS_Get_Reg_Base(__u32 sel)
{
	__u32 ret = 0;

	ret = (__u32)wb_dev[sel];

	return ret;
}

__s32 WB_EBIOS_Init(__u32 sel)
{

	memset((void*)wb_dev[sel], 0, sizeof(__iep_wb_dev_t));

    wb_dev[sel]->dma.bits.burst_len = 0;
    wb_dev[sel]->intp.bits.wb_end_int_timing = 0;

    WB_EBIOS_Set_Capture_Mode(sel);

	return 0;
}

__s32 WB_EBIOS_Enable(__u32 sel)
{
	wb_dev[sel]->gctrl.bits.en = 1;

	return 0;
}

__s32 WB_EBIOS_Reset(__u32 sel)
{
	wb_dev[sel]->gctrl.bits.en = 0;

	return 0;
}

__s32 WB_EBIOS_Set_Reg_Rdy(__u32 sel)
{
	wb_dev[sel]->gctrl.bits.reg_rdy_en = 1;

	return 0;
}

__s32 WB_EBIOS_Set_Capture_Mode(__u32 sel)
{
	wb_dev[sel]->gctrl.bits.wb_mode = 0;
	wb_dev[sel]->gctrl.bits.in_port_sel = 0;

	return 0;
}

__s32 WB_EBIOS_Writeback_Enable(__u32 sel)
{
	wb_dev[sel]->gctrl.bits.wb_en = 1;

	return 0;
}

__s32 WB_EBIOS_Writeback_Disable(__u32 sel)
{
	wb_dev[sel]->gctrl.bits.wb_en = 0;

	return 0;
}

#if 0
#endif

__s32 WB_EBIOS_Set_Para(__u32 sel,  disp_size insize, disp_window cropwin, disp_window outwin, disp_fb_info outfb)
{
	__u32 sclfac;
	__u32 xoff, yoff, bufoff0, bufoff1;

	wb_dev[sel]->size.bits.width = insize.width - 1;
	wb_dev[sel]->size.bits.height = insize.height - 1;

	wb_dev[sel]->crop_coord.bits.crop_left = (__u32)cropwin.x;
	wb_dev[sel]->crop_coord.bits.crop_top = (__u32)cropwin.y;
	wb_dev[sel]->crop_size.bits.crop_width = cropwin.width - 1;
	wb_dev[sel]->crop_size.bits.crop_height = cropwin.height - 1;

	if(cropwin.width/outwin.width != cropwin.height/outwin.height)
	{
		return -1;
	}

	sclfac = cropwin.width/outwin.width;

	if(sclfac * outwin.width - cropwin.width != 0)
	{
		return -1;
	}

	if(sclfac * outwin.height - cropwin.height != 0)
	{
		return -1;
	}

	switch(sclfac)
	{
		case 1:	sclfac = 0;	break;
		case 2:	sclfac = 1;	break;
		case 4:	sclfac = 2;	break;
		default:sclfac = 0;	break;
	}
	wb_dev[sel]->resizer.bits.factor = sclfac;

	if(outfb.size.width < (outwin.width + outwin.x))
	{
		return -1;
	}

	if(outfb.size.height < (outwin.height+ outwin.y))
	{
		return -1;
	}

	if(outfb.format == DISP_FORMAT_ARGB_8888)
	{
		wb_dev[sel]->lstrd0.bits.lstrd = outfb.size.width * 4;
		wb_dev[sel]->format.bits.format = 0;
		wb_dev[sel]->format.bits.ps = 1;

		wb_dev[sel]->cscyrcoef[0].dwval = 0x400;
		wb_dev[sel]->cscyrcoef[1].dwval = 0;
		wb_dev[sel]->cscyrcoef[2].dwval = 0;
		wb_dev[sel]->cscyrcon.dwval = 0;
		wb_dev[sel]->cscugcoef[0].dwval = 0;
		wb_dev[sel]->cscugcoef[1].dwval = 0x400;
		wb_dev[sel]->cscugcoef[2].dwval = 0;
		wb_dev[sel]->cscugcon.dwval = 0;
		wb_dev[sel]->cscvbcoef[0].dwval = 0;
		wb_dev[sel]->cscvbcoef[1].dwval = 0;
		wb_dev[sel]->cscvbcoef[2].dwval = 0x400;
		wb_dev[sel]->cscvbcon.dwval = 0;

		xoff = outwin.x;
		yoff = outwin.y;

		bufoff0 = outfb.size.width * yoff + xoff;
		bufoff1 = 0;

	}
	else if(outfb.format == DISP_FORMAT_YUV420_P)
	{
		wb_dev[sel]->lstrd0.bits.lstrd = outfb.size.width;
		wb_dev[sel]->lstrd1.bits.lstrd = outfb.size.width;
		wb_dev[sel]->format.bits.format = 1;
		wb_dev[sel]->format.bits.ps = 0;

		if(outfb.size.width > 720 || outfb.size.height > 720)
		{
			wb_dev[sel]->cscyrcoef[0].dwval = 0x00bb;
			wb_dev[sel]->cscyrcoef[1].dwval = 0x0275;
			wb_dev[sel]->cscyrcoef[2].dwval = 0x003f;
			wb_dev[sel]->cscyrcon.dwval 	= 0x0100;
			wb_dev[sel]->cscugcoef[0].dwval = 0xff99;
			wb_dev[sel]->cscugcoef[1].dwval = 0xfea6;
			wb_dev[sel]->cscugcoef[2].dwval = 0x01c2;
			wb_dev[sel]->cscugcon.dwval 	= 0x0800;
			wb_dev[sel]->cscvbcoef[0].dwval = 0x01c2;
			wb_dev[sel]->cscvbcoef[1].dwval = 0xfe67;
			wb_dev[sel]->cscvbcoef[2].dwval = 0xffd7;
			wb_dev[sel]->cscvbcon.dwval 	= 0x0800;
		}
		else
		{
			wb_dev[sel]->cscyrcoef[0].dwval = 0x107;
			wb_dev[sel]->cscyrcoef[1].dwval = 0x204;
			wb_dev[sel]->cscyrcoef[2].dwval = 0x64;
			wb_dev[sel]->cscyrcon.dwval = 0x100;
			wb_dev[sel]->cscugcoef[0].dwval = 0xff68;
			wb_dev[sel]->cscugcoef[1].dwval = 0xfed6;
			wb_dev[sel]->cscugcoef[2].dwval = 0x1c2;
			wb_dev[sel]->cscugcon.dwval = 0x800;
			wb_dev[sel]->cscvbcoef[0].dwval = 0x1c2;
			wb_dev[sel]->cscvbcoef[1].dwval = 0xfe87;
			wb_dev[sel]->cscvbcoef[2].dwval = 0xffb7;
			wb_dev[sel]->cscvbcon.dwval = 0x800;
		}

		xoff = (outwin.x>>1)<<1;
		yoff = (outwin.y>>1)<<1;

		bufoff0 = outfb.size.width * yoff + xoff;
		bufoff1 = outfb.size.width * (yoff>>1) + xoff;
	}
	else
	{
		return -1;
	}

	wb_dev[sel]->addr0.bits.addr = outfb.addr[0] + bufoff0;
	wb_dev[sel]->addr1.bits.addr = outfb.addr[1] + bufoff1;

	g_bufoff0[sel] = bufoff0;
	g_bufoff1[sel] = bufoff1;

	return 0;
}

__s32 WB_EBIOS_Set_Addr(__u32 sel, __u32 addr[3])
{
	wb_dev[sel]->addr0.bits.addr = addr[0] + g_bufoff0[sel];
	wb_dev[sel]->addr1.bits.addr = addr[1] + g_bufoff1[sel];

	return 0;
}

__u32 WB_EBIOS_Get_Status(__u32 sel)
{
	__u32 status;

	status = wb_dev[sel]->status.dwval & 0x71;
	wb_dev[sel]->status.dwval = 0x71;

	if(status & 0x10)
	{
		return 1;
	}
	else if(status & 0x20)
	{
		return 2;
	}/*
	else if(status & 0x40)
	{
		return 3;
	}
	*/
	else if(status & 0x01)
	{
		return 0;
	}
	else
	{
		return 4;
	}

}

__s32 WB_EBIOS_EnableINT(__u32 sel)
{
	wb_dev[sel]->intp.dwval |= (WB_END_IE | WB_FIFO_EMPTY_ERROR_IE | WB_FIFO_OVF_ERROR_IE | WB_UNFINISH_ERROR_IE);

	return 0;
}

__s32 WB_EBIOS_DisableINT(__u32 sel)
{
	wb_dev[sel]->intp.dwval &= ~(WB_END_IE | WB_FIFO_EMPTY_ERROR_IE | WB_FIFO_OVF_ERROR_IE | WB_UNFINISH_ERROR_IE);

	return 0;
}

__u32 WB_EBIOS_QueryINT(__u32 sel)
{
	return wb_dev[sel]->status.dwval;
}

__u32 WB_EBIOS_ClearINT(__u32 sel)
{
	wb_dev[sel]->status.dwval |= (WB_END_IE | WB_FIFO_EMPTY_ERROR_IE | WB_FIFO_OVF_ERROR_IE | WB_UNFINISH_ERROR_IE);

	return 0;
}

