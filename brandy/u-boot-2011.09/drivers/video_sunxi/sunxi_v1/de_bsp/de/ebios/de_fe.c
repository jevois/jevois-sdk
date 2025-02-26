

#include "de_fe.h"

static volatile __de_scal_dev_t *scal_dev[2];
static __u32 de_scal_ch0_offset;
static __u32 de_scal_ch1_offset;
static __u32 de_scal_ch2_offset;
static __u32 de_scal_trd_fp_en = 0;
static __u32 de_scal_trd_itl_en = 0;
static __u32 de_scal_ch0r_offset;
static __u32 de_scal_ch1r_offset;
static __u32 de_scal_ch2r_offset;

__s32 DE_SCAL_Set_Reg_Base(__u8 sel, __u32 base)
{
	scal_dev[sel]=(__de_scal_dev_t *)base;

	return 0;
}

__u32 DE_SCAL_Get_Reg_Base(__u8 sel)
{
	__u32 ret = 0;

	ret = (__u32)(scal_dev[sel]);

	return ret;
}

__s32 DE_SCAL_Config_Src(__u8 sel, __scal_buf_addr_t *addr, __scal_src_size_t *size,
                         __scal_src_type_t *type, __u8 field, __u8 dien)
{
    __u8 w_shift, h_shift;
	__u32 image_w0, image_w1;
	__u32 x_off0, y_off0, x_off1, y_off1;
	__u32 in_w0, in_h0, in_w1, in_h1;
	__u8 rgb16mode = 0;

	image_w0 = size->src_width;
	in_w0 = size->scal_width;
	in_h0 = size->scal_height;
	x_off0 = size->x_off;
	y_off0 = (field | dien) ? (size->y_off & 0xfffffffe) : size->y_off; 

    {
        if(type->fmt == DE_SCAL_INYUV422 || type->fmt == DE_SCAL_INYUV420)
        {
            w_shift = 1;
        	image_w1 = (image_w0 + 0x1)>>w_shift;
        	in_w1 = (in_w0 + 0x1)>>w_shift;
        	x_off1 = (x_off0)>>w_shift;
        	if(type->mod == DE_SCAL_INTERLEAVED)
        	{
        	    image_w0 = (image_w0 + 0x1) & 0xfffffffe;
                image_w1 = image_w0>>1;
                in_w0 &= 0xfffffffe;
                in_w1 = in_w0>>0x1;
        	    x_off0 &= 0xfffffffe;
        		x_off1 = x_off0>>w_shift;
        	}
        }
        else if(type->fmt == DE_SCAL_INYUV411)
        {
            w_shift = 2;
        	image_w1 = (image_w0 + 0x3)>>w_shift;
        	in_w1 = (in_w0 + 0x3)>>w_shift;
        	x_off1 = (x_off0)>>w_shift;
        }
		else if(type->fmt == DE_SCAL_INRGB565 || type->fmt == DE_SCAL_INRGB4444 || type->fmt == DE_SCAL_INRGB1555)
		{
			w_shift = 0;
	        rgb16mode = 1;
	        image_w1 = image_w0;
	        in_w1 = in_w0;
	        x_off1 = x_off0;
		}
        else
        {
            w_shift = 0;
        	image_w1 = image_w0;
        	in_w1 = in_w0;
        	x_off1 = x_off0;
        }
        if(type->fmt == DE_SCAL_INYUV420)
        {
            h_shift = 1;
        	in_h1 = (in_h0 + 0x1)>>h_shift;
        	y_off1 = (y_off0)>>h_shift;
        }
        else
        {
            h_shift = 0;
        	in_h1 = in_h0;
        	y_off1 = y_off0;
        }
    }
    in_h0 = (in_h0!=0) ? in_h0 : 1;
	in_h1 = (in_h1!=0) ? in_h1 : 1;
	in_w0 = (in_w0!=0) ? in_w0 : 1;
	in_w1 = (in_w1!=0) ? in_w1 : 1;

	if(type->mod == DE_SCAL_PLANNAR)
	{
	    scal_dev[sel]->linestrd0.dwval = image_w0;
		scal_dev[sel]->linestrd1.dwval = image_w1;
		scal_dev[sel]->linestrd2.dwval = image_w1;

        if(type->fmt == DE_SCAL_INYUV420)
        {
            scal_dev[sel]->linestrd0.dwval = ((image_w0+15)>>4)<<4;
            scal_dev[sel]->linestrd1.dwval = ((image_w1+7)>>3)<<3;
            scal_dev[sel]->linestrd2.dwval = ((image_w1+7)>>3)<<3;
        }
        de_scal_ch0_offset = image_w0 * y_off0 + x_off0;
        de_scal_ch1_offset = image_w1 * y_off1 + x_off1;
        de_scal_ch2_offset = image_w1 * y_off1 + x_off1;
		scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
		scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
		scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;
	}
	else if(type->mod == DE_SCAL_INTERLEAVED)
	{
	    scal_dev[sel]->linestrd0.dwval = image_w0<<(0x2 - w_shift  - rgb16mode);
		scal_dev[sel]->linestrd1.dwval = 0x0;
		scal_dev[sel]->linestrd2.dwval = 0x0;

        de_scal_ch0_offset = ((image_w0 * y_off0 + x_off0)<<(0x2 - w_shift - rgb16mode));
        de_scal_ch1_offset = 0x0;
        de_scal_ch2_offset = 0x0;
		scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
		scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr;
		scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr;
	}
	else if(type->mod == DE_SCAL_UVCOMBINED)
	{
	    scal_dev[sel]->linestrd0.dwval = image_w0;
		scal_dev[sel]->linestrd1.dwval = image_w1<<1;
		scal_dev[sel]->linestrd2.dwval = 0x0;

        de_scal_ch0_offset = image_w0 * y_off0 + x_off0;
        de_scal_ch1_offset = (((image_w1) * (y_off1) + (x_off1))<<1);
        de_scal_ch2_offset = 0x0;
		scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
		scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
		scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr;
	}
	else if(type->mod == DE_SCAL_PLANNARMB)
	{
	    image_w0 = (image_w0 + 0xf)&0xfff0;
        image_w1 = (image_w1 + (0xf>>w_shift)) & (~(0xf>>w_shift));

        scal_dev[sel]->tb_off0.bits.x_offset0 = (x_off0 & 0x0f);
        scal_dev[sel]->tb_off0.bits.y_offset0 = (y_off0 & 0x0f);
        scal_dev[sel]->tb_off0.bits.x_offset1 = (((x_off0 & 0x0f) & (0x0f)) + in_w0 + 0x0f) & 0x0f;
        scal_dev[sel]->tb_off1.bits.x_offset0 = ((x_off1)&(0x0f>>w_shift));
        scal_dev[sel]->tb_off1.bits.y_offset0 = ((y_off1)&(0x0f>>h_shift));
        scal_dev[sel]->tb_off1.bits.x_offset1 = ((((x_off1)&(0x0f>>w_shift)) & (0x0f>>w_shift)) + (in_w1) + (0x0f>>w_shift))&(0x0f>>w_shift);
		scal_dev[sel]->tb_off2.bits.x_offset0 = scal_dev[sel]->tb_off1.bits.x_offset0;
		scal_dev[sel]->tb_off2.bits.y_offset0 = scal_dev[sel]->tb_off1.bits.y_offset0;
		scal_dev[sel]->tb_off2.bits.x_offset1 = scal_dev[sel]->tb_off1.bits.x_offset1;

		scal_dev[sel]->linestrd0.dwval = (image_w0 - 0xf)<<4;
		scal_dev[sel]->linestrd1.dwval = ((image_w1) <<(0x04-h_shift)) - ((0xf>>h_shift)<<(0x04-w_shift));
		scal_dev[sel]->linestrd2.dwval = scal_dev[sel]->linestrd1.dwval;

        de_scal_ch0_offset = ((image_w0 + 0x0f)&0xfff0) * (y_off0&0xfff0) + ((y_off0&0x00f)<<4) + ((x_off0&0xff0)<<4);
        de_scal_ch1_offset = (((image_w1) + (0x0f>>w_shift)) &(0xfff0>>w_shift)) * ((y_off1) & (0xfff0>>h_shift)) +
                             ((((y_off1) & (0x00f>>h_shift))<<(0x4 - w_shift))) + (((x_off1) & (0xfff0>>w_shift))<<(0x4 - h_shift));
        de_scal_ch2_offset = de_scal_ch1_offset;
		scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
		scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
		scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;
	}
	else if(type->mod == DE_SCAL_UVCOMBINEDMB)
	{
	    image_w0 = (image_w0 + 0x1f)&0xffffffe0;
		image_w1 = (image_w1 + 0x0f)&0xfffffff0;
		scal_dev[sel]->tb_off0.bits.x_offset0 = (x_off0 & 0x1f);
        scal_dev[sel]->tb_off0.bits.y_offset0 = (y_off0 & 0x1f);
		scal_dev[sel]->tb_off0.bits.x_offset1 = (((x_off0 & 0x1f) & 0x1f) + in_w0 + 0x1f) &0x1f;
		scal_dev[sel]->tb_off1.bits.x_offset0 = (((x_off1)<<1)&0x1f);
        scal_dev[sel]->tb_off1.bits.y_offset0 = ((y_off1)&0x1f);
        scal_dev[sel]->tb_off1.bits.x_offset1 = (((((x_off1)<<1)&0x1f) & 0x1e) + ((in_w1)<<1) + 0x1f) & 0x1f;

		scal_dev[sel]->linestrd0.dwval = (((image_w0 + 0x1f)&0xffe0) - 0x1f)<<0x05;
        scal_dev[sel]->linestrd1.dwval = (((((image_w1)<<1)+0x1f)&0xffe0) - 0x1f) << 0x05;
        scal_dev[sel]->linestrd2.dwval = 0x00;

        de_scal_ch0_offset = ((image_w0 + 0x1f) &0xffe0) * (y_off0& 0xffe0) + ((y_off0& 0x01f)<<5) + ((x_off0& 0xffe0)<<5);
        de_scal_ch1_offset = (((image_w1<< 0x01)+0x1f)&0xffe0) * ((y_off1) & 0xffe0) + (((y_off1) & 0x01f)<<5) + (((x_off1<<0x01) & 0xffe0)<<5);
        de_scal_ch2_offset = 0x0;
		scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
        scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
        scal_dev[sel]->buf_addr2.dwval = 0x0;
	}

	scal_dev[sel]->input_fmt.bits.byte_seq = type->byte_seq;
	scal_dev[sel]->input_fmt.bits.data_mod = type->mod;
	scal_dev[sel]->input_fmt.bits.data_fmt = type->fmt;
	scal_dev[sel]->input_fmt.bits.data_ps = type->ps;

	scal_dev[sel]->ch0_insize.bits.in_width = in_w0 - 1;
	scal_dev[sel]->ch0_insize.bits.in_height = in_h0 - 1;
	scal_dev[sel]->ch1_insize.bits.in_width = in_w1 - 1;
	scal_dev[sel]->ch1_insize.bits.in_height = in_h1 - 1;

	scal_dev[sel]->trd_ctrl.dwval = 0;
    return 0;
}


__s32 DE_SCAL_Set_Fb_Addr(__u8 sel, __scal_buf_addr_t *addr)
{
    scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
    scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
    scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

    return 0;
}


__s32 DE_SCAL_Set_Init_Phase(__u8 sel, __scal_scan_mod_t *in_scan, __scal_src_size_t *in_size,
                             __scal_src_type_t *in_type, __scal_scan_mod_t *out_scan,
                             __scal_out_size_t *out_size, __scal_out_type_t *out_type, __u8 dien)
{
     __s32 ch0_h_phase=0, ch0_v_phase0=0, ch0_v_phase1=0, ch12_h_phase=0, ch12_v_phase0=0, ch12_v_phase1=0;
	 __u8 h_shift=0, w_shift=0;
     __s32 in_h0, in_h1, out_h0, out_h1;


	 scal_dev[sel]->output_fmt.bits.scan_mod = out_scan->field;
     scal_dev[sel]->input_fmt.bits.scan_mod = out_scan->field ? 0x0 : in_scan->field; 
     if(de_scal_trd_itl_en == 0)  
 	{
		 scal_dev[sel]->field_ctrl.bits.field_loop_mod = 0x0;
		 scal_dev[sel]->field_ctrl.bits.valid_field_cnt = 0x1-0x1;
		 scal_dev[sel]->field_ctrl.bits.field_cnt = in_scan->bottom;
 	}


	 if(in_type->fmt == DE_SCAL_INYUV420)
	 {
	     if(in_type->sample_method == 0x0) 
	     {
	         ch0_h_phase = 0x400;
			 ch0_v_phase0 = 0x400;
			 ch0_v_phase1 = 0x400;
			 ch12_h_phase = 0xfc400;  
			 ch12_v_phase0 = 0xfc400; 
			 ch12_v_phase1 = 0xfc400; 
	     }
		 else
		 {
		     ch0_h_phase = 0x400;
			 ch0_v_phase0 = 0x400;
			 ch0_v_phase1 = 0x400;
			 ch12_h_phase = 0x400;      
			 ch12_v_phase0 = 0xfc400; 
			 ch12_v_phase1 = 0xfc400; 
		 }
	 }
	 else 
	 {
	     ch0_h_phase = 0x400;
		 ch0_v_phase0 = 0x400;
		 ch0_v_phase1 = 0x400;
		 ch12_h_phase = 0x400;
		 ch12_v_phase0 = 0x400;
		 ch12_v_phase1 = 0x400;
	 }

     w_shift = (in_type->fmt == DE_SCAL_INYUV420 || in_type->fmt == DE_SCAL_INYUV422) ? 0x1 : ((in_type->fmt == DE_SCAL_INYUV411) ? 0x2 : 0x0);
	 h_shift = (in_type->fmt == DE_SCAL_INYUV420) ? 0x1 : 0x0;

     if(((dien == 0x01) || (in_scan->field== 0x1)) && (in_size->y_off & 0x1)&& (in_scan->bottom == 0x0)) 
     {
         ch0_v_phase0 = (ch0_v_phase0 + 0x10000) & SCALINITPASELMT;
         ch12_v_phase0 = (ch12_v_phase0 + 0x10000) & SCALINITPASELMT;
     }
     else
     {
         ch12_v_phase0 = (ch12_v_phase0 + (in_size->y_off & ((1<<h_shift)-1))*(0x10000>>h_shift)) & SCALINITPASELMT;
         ch12_v_phase1 = ch12_v_phase0;
     }

	 scal_dev[sel]->ch0_horzphase.bits.phase = ch0_h_phase;
	 scal_dev[sel]->ch1_horzphase.bits.phase = (ch12_h_phase + (in_size->x_off & ((1<<w_shift) - 1)) * (0x10000>>w_shift)) & SCALINITPASELMT;

     if(out_scan->field == 0x1) 
     {
         in_h0 = in_size->scal_height;
         in_h1 = (in_type->fmt == DE_SCAL_INYUV420) ? (in_h0+0x1)>>1: in_h0;
         out_h0 = out_size->height;
         out_h1 = (out_type->fmt == DE_SCAL_OUTPYUV420) ? (out_h0+0x1)>>1 : out_h0;

		in_h0 = (in_h0!=0) ? in_h0 : 1;
		in_h1 = (in_h1!=0) ? in_h1 : 1;
		out_h0 = (out_h0!=0) ? out_h0 : 1;
		out_h1 = (out_h1!=0) ? out_h1 : 1;

         if(in_scan->bottom == 0x0)
         {
	         scal_dev[sel]->ch0_vertphase0.bits.phase = ch0_v_phase0;
             scal_dev[sel]->ch0_vertphase1.bits.phase = ch0_v_phase0 + ((in_h0>>in_scan->field)<<16)/(out_h0);
             scal_dev[sel]->ch1_vertphase0.bits.phase = ch12_v_phase0;
             scal_dev[sel]->ch1_vertphase1.bits.phase = ch12_v_phase0 + ((in_h1>>in_scan->field)<<16)/(out_h1);
         }
         else
         {
             scal_dev[sel]->ch0_vertphase0.bits.phase = ch0_v_phase1;
             scal_dev[sel]->ch0_vertphase1.bits.phase = ch0_v_phase1 + ((in_h0>>in_scan->field)<<16)/(out_h0);
             scal_dev[sel]->ch1_vertphase0.bits.phase = ch12_v_phase1;
             scal_dev[sel]->ch1_vertphase1.bits.phase = ch12_v_phase1 + ((in_h1>>in_scan->field)<<16)/(out_h1);
         }
     }
     else 
     {
         scal_dev[sel]->ch0_vertphase0.bits.phase = ch0_v_phase0;
         scal_dev[sel]->ch0_vertphase1.bits.phase = ch0_v_phase1;
         scal_dev[sel]->ch1_vertphase0.bits.phase = ch12_v_phase0;
         scal_dev[sel]->ch1_vertphase1.bits.phase = ch12_v_phase1;

     }

	 return 0;
}

__s32 DE_SCAL_Agth_Config(__u8 sel, __scal_src_type_t *in_type,__scal_src_size_t *in_size,__scal_out_size_t *out_size,
                          __u8 dien,__u8 trden,__scal_3d_outmode_t outmode)
{
	__u32 inwidth,outwidth, linebuflen,cibuflen;
	inwidth = in_size->scal_width;
	outwidth = out_size->width;

	if(trden)
	{
		switch(outmode)
		{
			case DE_SCAL_3DOUT_HDMI_FPP:break;
			case DE_SCAL_3DOUT_HDMI_TB: break;
			case DE_SCAL_3DOUT_HDMI_FPI:break;
			case DE_SCAL_3DOUT_HDMI_FA: break;
			default: inwidth *=2; outwidth *=2; break;
		}
	}
	if((inwidth > 8192) || (outwidth > 8192))
		return -1;
	else if((inwidth > 4096) && (outwidth > 4096))
		return -1;
	else if((inwidth>2048) && (outwidth>2048))
	{
		if(dien)
		{
			return -1;
		}
		else if(in_type->fmt == DE_SCAL_INYUV420)
		{
			scal_dev[sel]->bypass.bits.sram_map_sel = 1;
			linebuflen = 4096;
			cibuflen = 2048;
		}
		else if((inwidth>2560)&&(outwidth>2560))
		{
			return -1;
		}
		else
		{
			scal_dev[sel]->bypass.bits.sram_map_sel = 2;
			linebuflen = 2560;
			cibuflen = 0;
		}
	}
	else
	{
		scal_dev[sel]->bypass.bits.sram_map_sel = 0;
		linebuflen = 2048;
		cibuflen = dien ? 1280:2048;
	}

	if(trden & (outmode<DE_SCAL_3DOUT_LIRGB))
	{
		if(outwidth>cibuflen)
			return -1;
	}

	if(linebuflen == 4096)
	{
		scal_dev[sel]->agth_sel.bits.linebuf_agth = 1;
	}
	else if(outwidth>linebuflen)
	{
		scal_dev[sel]->agth_sel.bits.linebuf_agth = 1;
	}
	else if(inwidth > linebuflen)
	{
		scal_dev[sel]->agth_sel.bits.linebuf_agth = 0;
	}

	return 0;
}


__s32 DE_SCAL_Set_Scaling_Factor(__u8 sel, __scal_scan_mod_t *in_scan, __scal_src_size_t *in_size,
                                 __scal_src_type_t *in_type, __scal_scan_mod_t *out_scan,
                                 __scal_out_size_t *out_size, __scal_out_type_t *out_type)
{
    __s32 in_w0, in_h0, out_w0, out_h0;
    __s32 ch0_hstep, ch0_vstep ;
	__s8 w_shift, h_shift;

    in_w0 = in_size->scal_width;
    in_h0 = in_size->scal_height;

    out_w0 = out_size->width;
    out_h0 = out_size->height + (out_scan->field & 0x1);

	if((in_type->mod == DE_SCAL_INTERLEAVED) && (in_type->fmt == DE_SCAL_INYUV422))
    {
        in_w0 &=0xfffffffe;
    }
    /*//algorithm select
    if(out_w0 > SCALLINEMAX)
    {
	    scal_dev[sel]->agth_sel.bits.linebuf_agth = 0x1;
        if(in_w0>SCALLINEMAX) 
        {
            in_w0 = SCALLINEMAX;
        }
    }
    else
    {
        scal_dev[sel]->agth_sel.bits.linebuf_agth= 0x0;
    }*/

    w_shift = (in_type->fmt == DE_SCAL_INYUV411) ? 2 : ((in_type->fmt == DE_SCAL_INYUV420)||(in_type->fmt == DE_SCAL_INYUV422)) ? 1 : 0;
	h_shift = ((in_type->fmt == DE_SCAL_INYUV420)) ? 1 : 0;


    if((out_type->fmt == DE_SCAL_OUTPYUV420) || (out_type->fmt == DE_SCAL_OUTPYUV422))
    {
        w_shift -= 1 ;
    }
    else if(out_type->fmt == DE_SCAL_OUTPYUV411)
    {
        w_shift -= 2 ;
    }
    else
    {
        w_shift -= 0 ;;
    }
    if(out_type->fmt == DE_SCAL_OUTPYUV420)
    {
        h_shift -= 1;
    }
    else
    {
        h_shift -= 0;
    }
    in_h0 = (in_h0!=0) ? in_h0 : 1;
	in_w0 = (in_w0!=0) ? in_w0 : 1;
	out_h0 = (out_h0!=0) ? out_h0 : 1;
	out_w0 = (out_w0!=0) ? out_w0 : 1;

    ch0_hstep = (in_w0<<16)/out_w0;
    ch0_vstep = (out_scan->field)? ((in_h0<<16)/(out_h0)) : (((in_h0>>in_scan->field)<<16)/( out_h0 ));

	scal_dev[sel]->ch0_horzfact.dwval = ch0_hstep;
    scal_dev[sel]->ch0_vertfact.dwval = ch0_vstep<<(out_scan->field);
    scal_dev[sel]->ch1_horzfact.dwval = (w_shift>0) ? (ch0_hstep>>w_shift) : ch0_hstep<<(0-w_shift);
    scal_dev[sel]->ch1_vertfact.dwval = (h_shift>0) ? (ch0_vstep>>h_shift)<<(out_scan->field) : (ch0_vstep<<(0-h_shift))<<(out_scan->field);

	return 0;
}


__s32 DE_SCAL_Set_Scaling_Coef(__u8 sel, __scal_scan_mod_t *in_scan, __scal_src_size_t *in_size,
                               __scal_src_type_t *in_type, __scal_scan_mod_t *out_scan,
                               __scal_out_size_t *out_size, __scal_out_type_t *out_type, __u8 smth_mode)
{
    __s32 in_w0, in_h0, in_w1, in_h1, out_w0, out_h0, out_w1, out_h1;
    __s32 ch0h_smth_level, ch0v_smth_level, ch1h_smth_level, ch1v_smth_level;
    __u32 int_part, float_part;
    __u32 zoom0_size, zoom1_size, zoom2_size, zoom3_size, zoom4_size, zoom5_size, al1_size;
    __u32 ch0h_sc, ch0v_sc, ch1h_sc, ch1v_sc;
#if defined(CONFIG_ARCH_SUN8IW1P1) || defined(CONFIG_ARCH_SUN8IW3P1) || defined(CONFIG_ARCH_SUN8IW5P1)|| defined(CONFIG_ARCH_SUN8IW6P1)
    __u32 ch0v_fir_coef_addr, ch0h_fir_coef_addr, ch1v_fir_coef_addr, ch1h_fir_coef_addr;
    __u32 i;
#endif
    __u32 ch0v_fir_coef_ofst, ch0h_fir_coef_ofst, ch1v_fir_coef_ofst, ch1h_fir_coef_ofst;
#if defined(CONFIG_ARCH_SUN8IW1P1)
	__u32 ch3h_fir_coef_addr, ch3v_fir_coef_addr;
#endif
    __s32 fir_ofst_tmp;
#if defined(CONFIG_ARCH_SUN8IW3P1) || defined(CONFIG_ARCH_SUN8IW5P1)|| defined(CONFIG_ARCH_SUN8IW6P1)
    __u32 loop_count = 0;
#endif

    in_w0 = in_size->scal_width;
    in_h0 = in_size->scal_height;

    out_w0 = out_size->width;
    out_h0 = out_size->height;

    zoom0_size = 1;
    zoom1_size = 8;
    zoom2_size = 4;
    zoom3_size = 1;
    zoom4_size = 1;
    zoom5_size = 1;
    al1_size = zoom0_size + zoom1_size + zoom2_size + zoom3_size + zoom4_size + zoom5_size;

    if((in_type->mod == DE_SCAL_INTERLEAVED) && (in_type->fmt == DE_SCAL_INYUV422))
    {
        in_w0 &=0xfffffffe;
    }

    if((in_type->fmt == DE_SCAL_INYUV420) || (in_type->fmt == DE_SCAL_INYUV422))
    {
        in_w1 = (in_w0 + 0x1)>>0x1;
    }
    else if(in_type->fmt == DE_SCAL_INYUV411)
    {
        in_w1 = (in_w0 + 0x3)>>0x2;
    }
    else
    {
        in_w1 = in_w0;
    }
    if((in_type->fmt == DE_SCAL_INYUV420))
    {
        in_h1 = (in_h0 + 0x1)>>0x1;
    }
    else
    {
        in_h1 = in_h0;
    }
    if((out_type->fmt == DE_SCAL_OUTPYUV420) || (out_type->fmt == DE_SCAL_OUTPYUV422))
    {
        out_w1 = (out_w0 + 0x1)>>0x1;
    }
    else if(out_type->fmt == DE_SCAL_OUTPYUV411)
    {
        out_w1 = (out_w0 + 0x3)>>0x2;
    }
    else
    {
        out_w1 = out_w0;
    }
    if(out_type->fmt == DE_SCAL_OUTPYUV420)
    {
        out_h1 = (out_h0+ 0x1)>>0x1;
    }
    else
    {
        out_h1 = out_h0;
    }

    in_h0 = (in_h0!=0) ? in_h0 : 1;
	in_h1 = (in_h1!=0) ? in_h1 : 1;
	in_w0 = (in_w0!=0) ? in_w0 : 1;
	in_w1 = (in_w1!=0) ? in_w1 : 1;
	out_h0 = (out_h0!=0) ? out_h0 : 1;
	out_h1 = (out_h1!=0) ? out_h1 : 1;
	out_w0 = (out_w0!=0) ? out_w0 : 1;
	out_w1 = (out_w1!=0) ? out_w1 : 1;

    ch0h_smth_level = (smth_mode&0x40)  ?  0 - (smth_mode&0x3f) : smth_mode&0x3f;
    ch0v_smth_level = ch0h_smth_level;
    if((smth_mode>>7) &0x01)
    {
      ch0v_smth_level = (smth_mode&0x4000) ? 0 - ((smth_mode&0x3f00)>>8) : ((smth_mode&0x3f00)>>8);
    }
    if((smth_mode>>31)&0x01)
    {
      ch1h_smth_level = (smth_mode&0x400000) ? 0 - ((smth_mode&0x3f0000)>>16) : ((smth_mode&0x3f0000)>>16);
      ch1v_smth_level = ch1h_smth_level;
      if((smth_mode >> 23)&0x1)
      {
        ch1v_smth_level = (smth_mode&0x40000000) ? 0 - ((smth_mode&0x3f000000)>>24) : ((smth_mode&0x3f000000)>>24);
      }
    }
    ch0h_sc = (in_w0<<3)/out_w0;
    ch0v_sc = (in_h0<<(3-in_scan->field))/(out_h0);
    ch1h_sc = (in_w1<<3)/out_w1;
    ch1v_sc = (in_h1<<(3-in_scan->field))/(out_h1);

    if(((smth_mode>>31)&0x01)==0x0)
    {
      if(!ch1h_sc)
      {
        ch1h_smth_level = 0;
      }
      else
      {
        ch1h_smth_level = ch0h_smth_level>>(ch0h_sc/ch1h_sc);
      }

      if(!ch1v_sc)
      {
        ch1v_smth_level = 0;
      }
      else
      {
        ch1v_smth_level = ch0v_smth_level>>(ch0v_sc/ch1v_sc);
      }
    }

      int_part = ch0v_sc>>3;
      float_part = ch0v_sc & 0x7;
      ch0v_fir_coef_ofst = (int_part==0)  ? zoom0_size :
                           (int_part==1)  ? zoom0_size + float_part :
                           (int_part==2)  ? zoom0_size + zoom1_size + (float_part>>1) :
                           (int_part==3)  ? zoom0_size + zoom1_size + zoom2_size :
                           (int_part==4)  ? zoom0_size + zoom1_size + zoom2_size +zoom3_size :
                           zoom0_size + zoom1_size + zoom2_size + zoom3_size + zoom4_size;
      int_part = ch0h_sc>>3;
      float_part = ch0h_sc & 0x7;
      ch0h_fir_coef_ofst = (int_part==0)  ? zoom0_size :
                           (int_part==1)  ? zoom0_size + float_part :
                           (int_part==2)  ? zoom0_size + zoom1_size + (float_part>>1) :
                           (int_part==3)  ? zoom0_size + zoom1_size + zoom2_size :
                           (int_part==4)  ? zoom0_size + zoom1_size + zoom2_size +zoom3_size :
                           zoom0_size + zoom1_size + zoom2_size + zoom3_size + zoom4_size;
      int_part = ch1v_sc>>3;
      float_part = ch1v_sc & 0x7;
      ch1v_fir_coef_ofst = (int_part==0)  ? zoom0_size :
                           (int_part==1)  ? zoom0_size + float_part :
                           (int_part==2)  ? zoom0_size + zoom1_size + (float_part>>1) :
                           (int_part==3)  ? zoom0_size + zoom1_size + zoom2_size :
                           (int_part==4)  ? zoom0_size + zoom1_size + zoom2_size +zoom3_size :
                           zoom0_size + zoom1_size + zoom2_size + zoom3_size + zoom4_size;
      int_part = ch1h_sc>>3;
      float_part = ch1h_sc & 0x7;
      ch1h_fir_coef_ofst =  (int_part==0)  ? zoom0_size :
                            (int_part==1)  ? zoom0_size + float_part :
                            (int_part==2)  ? zoom0_size + zoom1_size + (float_part>>1) :
                            (int_part==3)  ? zoom0_size + zoom1_size + zoom2_size :
                            (int_part==4)  ? zoom0_size + zoom1_size + zoom2_size +zoom3_size :
                            zoom0_size + zoom1_size + zoom2_size + zoom3_size + zoom4_size;
    fir_ofst_tmp = ch0v_fir_coef_ofst + ch0v_smth_level;
    ch0v_fir_coef_ofst = (fir_ofst_tmp<0) ? 0 : fir_ofst_tmp;
    fir_ofst_tmp = ch0h_fir_coef_ofst + ch0h_smth_level;
    ch0h_fir_coef_ofst = (fir_ofst_tmp<0) ? 0 : fir_ofst_tmp;
    fir_ofst_tmp = ch1v_fir_coef_ofst + ch1v_smth_level;
    ch1v_fir_coef_ofst = (fir_ofst_tmp<0) ? 0 : fir_ofst_tmp;
    fir_ofst_tmp = ch1h_fir_coef_ofst + ch1h_smth_level;
    ch1h_fir_coef_ofst = (fir_ofst_tmp<0) ? 0 : fir_ofst_tmp;
    ch0v_fir_coef_ofst = (ch0v_fir_coef_ofst > (al1_size - 1)) ? (al1_size - 1) : ch0v_fir_coef_ofst;
    ch1v_fir_coef_ofst = (ch1v_fir_coef_ofst > (al1_size - 1)) ? (al1_size - 1) : ch1v_fir_coef_ofst;
    ch0h_fir_coef_ofst = (ch0h_fir_coef_ofst > (al1_size - 1)) ? (al1_size - 1) : ch0h_fir_coef_ofst;
    ch1h_fir_coef_ofst = (ch1h_fir_coef_ofst > (al1_size - 1)) ? (al1_size - 1) : ch1h_fir_coef_ofst;

#if defined CONFIG_ARCH_SUN8IW3P1 || defined(CONFIG_ARCH_SUN8IW5P1)|| defined(CONFIG_ARCH_SUN8IW6P1)
    /* for  single buffer */
    ch0v_fir_coef_addr = (ch0v_fir_coef_ofst<<7);
    ch0h_fir_coef_addr = (ch0h_fir_coef_ofst<<7);
    ch1v_fir_coef_addr = (ch1v_fir_coef_ofst<<7);
    ch1h_fir_coef_addr = (ch1h_fir_coef_ofst<<7);
    scal_dev[sel]->frm_ctrl.bits.coef_access_ctrl= 1;
    while((scal_dev[sel]->status.bits.coef_access_status == 0) && (loop_count < 20)) {
        loop_count ++;
    }

    for(i=0; i<32; i++)
    {
      scal_dev[sel]->ch0_horzcoef0[i].dwval = fir_tab[(ch0h_fir_coef_addr>>2) + i];
      scal_dev[sel]->ch0_vertcoef[i].dwval  = fir_tab[(ch0v_fir_coef_addr>>2) + i];
      scal_dev[sel]->ch1_horzcoef0[i].dwval = fir_tab[(ch1h_fir_coef_addr>>2) + i];
      scal_dev[sel]->ch1_vertcoef[i].dwval  = fir_tab[(ch1v_fir_coef_addr>>2) + i];
    }

    scal_dev[sel]->frm_ctrl.bits.coef_access_ctrl = 0x0;
#elif defined CONFIG_ARCH_SUN8IW1P1
    ch0v_fir_coef_addr = (ch0v_fir_coef_ofst<<7);
    ch0h_fir_coef_addr = ((al1_size)<<7) + (ch0h_fir_coef_ofst<<8);
    ch1v_fir_coef_addr = (ch1v_fir_coef_ofst<<7);
    ch1h_fir_coef_addr = ((al1_size)<<7) + (ch1h_fir_coef_ofst<<8);
    ch3h_fir_coef_addr = out_type->alpha_coef_type==0 ? ch0h_fir_coef_addr : ((al1_size)<<7);
    ch3v_fir_coef_addr = out_type->alpha_coef_type==0 ? ch0v_fir_coef_addr : (0)<<7;
    for(i=0; i<32; i++)
    {
      scal_dev[sel]->ch0_horzcoef0[i].dwval = fir_tab[(ch0h_fir_coef_addr>>2) + 2*i];
      scal_dev[sel]->ch0_horzcoef1[i].dwval = fir_tab[(ch0h_fir_coef_addr>>2) + 2*i + 1];
      scal_dev[sel]->ch0_vertcoef[i].dwval  = fir_tab[(ch0v_fir_coef_addr>>2) + i];
      scal_dev[sel]->ch1_horzcoef0[i].dwval = fir_tab[(ch1h_fir_coef_addr>>2) + 2*i];
      scal_dev[sel]->ch1_horzcoef1[i].dwval = fir_tab[(ch1h_fir_coef_addr>>2) + 2*i + 1];
      scal_dev[sel]->ch1_vertcoef[i].dwval  = fir_tab[(ch1v_fir_coef_addr>>2) + i];
    }
	if(out_type->alpha_en && (in_type->mod == DE_SCAL_INTERLEAVED))
	{
		for(i=0;i<32;i++)
		{
			scal_dev[sel]->ch3_horzcoef0[i].dwval = fir_tab[(ch3h_fir_coef_addr>>2) + 2*i];
			scal_dev[sel]->ch3_horzcoef1[i].dwval = fir_tab[(ch3h_fir_coef_addr>>2) + 2*i + 1];
	        scal_dev[sel]->ch3_vertcoef[i].dwval  = fir_tab[(ch3v_fir_coef_addr>>2) + i];
		}
	}

    scal_dev[sel]->frm_ctrl.bits.coef_rdy_en = 0x1;
#endif

    return 0;
}

#if 0
__s32 DE_SCAL_Set_Scaling_Coef_for_video(__u8 sel, __scal_scan_mod_t *in_scan, __scal_src_size_t *in_size,
                               __scal_src_type_t *in_type, __scal_scan_mod_t *out_scan,
                               __scal_out_size_t *out_size, __scal_out_type_t *out_type, __u32 smth_mode)
{
    __s32 in_w0, in_h0, in_w1, in_h1, out_w0, out_h0, out_w1, out_h1;
    __s32 ch0h_smth_level=0, ch0v_smth_level=0, ch1h_smth_level=0, ch1v_smth_level=0;
    __u32 int_part, float_part;
    __u32 zoom0_size, zoom1_size, zoom2_size, zoom3_size, zoom4_size, zoom5_size, al1_size;
    __u32 ch0h_sc, ch0v_sc, ch1h_sc, ch1v_sc;
    __u32 ch0v_fir_coef_addr, ch0h_fir_coef_addr, ch1v_fir_coef_addr, ch1h_fir_coef_addr;
    __u32 ch0v_fir_coef_ofst, ch0h_fir_coef_ofst, ch1v_fir_coef_ofst, ch1h_fir_coef_ofst;
    __s32 fir_ofst_tmp;
    __u32 i;

    in_w0 = in_size->scal_width;
    in_h0 = in_size->scal_height;

    out_w0 = out_size->width;
    out_h0 = out_size->height;

    zoom0_size = 1;
    zoom1_size = 8;
    zoom2_size = 4;
    zoom3_size = 1;
    zoom4_size = 1;
    zoom5_size = 1;
    al1_size = zoom0_size + zoom1_size + zoom2_size + zoom3_size + zoom4_size + zoom5_size;

    if((in_type->mod == DE_SCAL_INTERLEAVED) && (in_type->fmt == DE_SCAL_INYUV422))
    {
        in_w0 &=0xfffffffe;
    }

    if((in_type->fmt == DE_SCAL_INYUV420) || (in_type->fmt == DE_SCAL_INYUV422))
    {
        in_w1 = (in_w0 + 0x1)>>0x1;
    }
    else if(in_type->fmt == DE_SCAL_INYUV411)
    {
        in_w1 = (in_w0 + 0x3)>>0x2;
    }
    else
    {
        in_w1 = in_w0;
    }
    if((in_type->fmt == DE_SCAL_INYUV420))
    {
        in_h1 = (in_h0 + 0x1)>>0x1;
    }
    else
    {
        in_h1 = in_h0;
    }
    if((out_type->fmt == DE_SCAL_OUTPYUV420) || (out_type->fmt == DE_SCAL_OUTPYUV422))
    {
        out_w1 = (out_w0 + 0x1)>>0x1;
    }
    else if(out_type->fmt == DE_SCAL_OUTPYUV411)
    {
        out_w1 = (out_w0 + 0x3)>>0x2;
    }
    else
    {
        out_w1 = out_w0;
    }
    if(out_type->fmt == DE_SCAL_OUTPYUV420)
    {
        out_h1 = (out_h0+ 0x1)>>0x1;
    }
    else
    {
        out_h1 = out_h0;
    }

    in_h0 = (in_h0!=0) ? in_h0 : 1;
	in_h1 = (in_h1!=0) ? in_h1 : 1;
	in_w0 = (in_w0!=0) ? in_w0 : 1;
	in_w1 = (in_w1!=0) ? in_w1 : 1;
	out_h0 = (out_h0!=0) ? out_h0 : 1;
	out_h1 = (out_h1!=0) ? out_h1 : 1;
	out_w0 = (out_w0!=0) ? out_w0 : 1;
	out_w1 = (out_w1!=0) ? out_w1 : 1;

    ch0h_smth_level = (smth_mode&0x40)  ?  0 - (smth_mode&0x3f) : smth_mode&0x3f;
    ch0v_smth_level = ch0h_smth_level;
    if((smth_mode>>7) &0x01)
    {
      ch0v_smth_level = (smth_mode&0x4000) ? 0 - ((smth_mode&0x3f00)>>8) : ((smth_mode&0x3f00)>>8);
    }
    if((smth_mode>>31)&0x01)
    {
      ch1h_smth_level = (smth_mode&0x400000) ? 0 - ((smth_mode&0x3f0000)>>16) : ((smth_mode&0x3f0000)>>16);
      ch1v_smth_level = ch1h_smth_level;
      if((smth_mode >> 23)&0x1)
      {
        ch1v_smth_level = (smth_mode&0x40000000) ? 0 - ((smth_mode&0x3f000000)>>24) : ((smth_mode&0x3f000000)>>24);
      }
    }
    ch0h_sc = (in_w0<<3)/out_w0;
    ch0v_sc = (in_h0<<(3-in_scan->field))/(out_h0);
    ch1h_sc = (in_w1<<3)/out_w1;
    ch1v_sc = (in_h1<<(3-in_scan->field))/(out_h1);

    if(((smth_mode>>31)&0x01)==0x0)
    {
      if(!ch1h_sc)
      {
        ch1h_smth_level = 0;
      }
      else
      {
        ch1h_smth_level = ch0h_smth_level>>(ch0h_sc/ch1h_sc);
      }

      if(!ch1v_sc)
      {
        ch1v_smth_level = 0;
      }
      else
      {
        ch1v_smth_level = ch0v_smth_level>>(ch0v_sc/ch1v_sc);
      }
    }

      int_part = ch0v_sc>>3;
      float_part = ch0v_sc & 0x7;
      ch0v_fir_coef_ofst = (int_part==0)  ? zoom0_size :
                           (int_part==1)  ? zoom0_size + float_part :
                           (int_part==2)  ? zoom0_size + zoom1_size + (float_part>>1) :
                           (int_part==3)  ? zoom0_size + zoom1_size + zoom2_size :
                           (int_part==4)  ? zoom0_size + zoom1_size + zoom2_size +zoom3_size :
                           zoom0_size + zoom1_size + zoom2_size + zoom3_size + zoom4_size;
      int_part = ch0h_sc>>3;
      float_part = ch0h_sc & 0x7;
      ch0h_fir_coef_ofst = (int_part==0)  ? zoom0_size :
                           (int_part==1)  ? zoom0_size + float_part :
                           (int_part==2)  ? zoom0_size + zoom1_size + (float_part>>1) :
                           (int_part==3)  ? zoom0_size + zoom1_size + zoom2_size :
                           (int_part==4)  ? zoom0_size + zoom1_size + zoom2_size +zoom3_size :
                           zoom0_size + zoom1_size + zoom2_size + zoom3_size + zoom4_size;
      int_part = ch1v_sc>>3;
      float_part = ch1v_sc & 0x7;
      ch1v_fir_coef_ofst = (int_part==0)  ? zoom0_size :
                           (int_part==1)  ? zoom0_size + float_part :
                           (int_part==2)  ? zoom0_size + zoom1_size + (float_part>>1) :
                           (int_part==3)  ? zoom0_size + zoom1_size + zoom2_size :
                           (int_part==4)  ? zoom0_size + zoom1_size + zoom2_size +zoom3_size :
                           zoom0_size + zoom1_size + zoom2_size + zoom3_size + zoom4_size;
      int_part = ch1h_sc>>3;
      float_part = ch1h_sc & 0x7;
      ch1h_fir_coef_ofst =  (int_part==0)  ? zoom0_size :
                            (int_part==1)  ? zoom0_size + float_part :
                            (int_part==2)  ? zoom0_size + zoom1_size + (float_part>>1) :
                            (int_part==3)  ? zoom0_size + zoom1_size + zoom2_size :
                            (int_part==4)  ? zoom0_size + zoom1_size + zoom2_size +zoom3_size :
                            zoom0_size + zoom1_size + zoom2_size + zoom3_size + zoom4_size;
    fir_ofst_tmp = ch0v_fir_coef_ofst + ch0v_smth_level;
    ch0v_fir_coef_ofst = (fir_ofst_tmp<0) ? 0 : fir_ofst_tmp;
    fir_ofst_tmp = ch0h_fir_coef_ofst + ch0h_smth_level;
    ch0h_fir_coef_ofst = (fir_ofst_tmp<0) ? 0 : fir_ofst_tmp;
    fir_ofst_tmp = ch1v_fir_coef_ofst + ch1v_smth_level;
    ch1v_fir_coef_ofst = (fir_ofst_tmp<0) ? 0 : fir_ofst_tmp;
    fir_ofst_tmp = ch1h_fir_coef_ofst + ch1h_smth_level;
    ch1h_fir_coef_ofst = (fir_ofst_tmp<0) ? 0 : fir_ofst_tmp;
    ch0v_fir_coef_ofst = (ch0v_fir_coef_ofst > (al1_size - 1)) ? (al1_size - 1) : ch0v_fir_coef_ofst;
    ch1v_fir_coef_ofst = (ch1v_fir_coef_ofst > (al1_size - 1)) ? (al1_size - 1) : ch1v_fir_coef_ofst;
    ch0h_fir_coef_ofst = (ch0h_fir_coef_ofst > (al1_size - 1)) ? (al1_size - 1) : ch0h_fir_coef_ofst;
    ch1h_fir_coef_ofst = (ch1h_fir_coef_ofst > (al1_size - 1)) ? (al1_size - 1) : ch1h_fir_coef_ofst;

    ch0v_fir_coef_addr = (ch0v_fir_coef_ofst<<7);
    ch0h_fir_coef_addr = ((al1_size)<<7) + (ch0h_fir_coef_ofst<<8);
    ch1v_fir_coef_addr = (ch1v_fir_coef_ofst<<7);
    ch1h_fir_coef_addr = ((al1_size)<<7) + (ch1h_fir_coef_ofst<<8);

    for(i=0; i<32; i++)
    {
	    scal_dev[sel]->ch0_horzcoef0[i].dwval = fir_tab_video[(ch0h_fir_coef_addr>>2) + 2*i];
		scal_dev[sel]->ch0_horzcoef1[i].dwval = fir_tab_video[(ch0h_fir_coef_addr>>2) + 2*i + 1];
        scal_dev[sel]->ch0_vertcoef[i].dwval  = fir_tab_video[(ch0v_fir_coef_addr>>2) + i];
		scal_dev[sel]->ch1_horzcoef0[i].dwval = fir_tab_video[(ch1h_fir_coef_addr>>2) + 2*i];
		scal_dev[sel]->ch1_horzcoef1[i].dwval = fir_tab_video[(ch1h_fir_coef_addr>>2) + 2*i + 1];
        scal_dev[sel]->ch1_vertcoef[i].dwval  = fir_tab_video[(ch1v_fir_coef_addr>>2) + i];
    }

    scal_dev[sel]->frm_ctrl.bits.coef_rdy_en = 0x1;

    return 0;
}
#endif

__s32 DE_SCAL_Set_CSC_Coef(__u8 sel, __u8 in_csc_mode, __u8 out_csc_mode, __u8 incs, __u8 outcs, __u32  in_br_swap, __u32 out_br_swap)
{
    __u8  csc_pass;
    __u32 csc_coef_addr;
    __u32 i;


    if(incs == 0x0) 
    {
        if(outcs == 0x0)
        {
            csc_pass = 0x01;
            csc_coef_addr = (((in_csc_mode&0x3)<<7) + ((in_csc_mode&0x3)<<6)) + 0x60;
        }
        else
        {
        	csc_pass = 0x0;
            csc_coef_addr = (((in_csc_mode&0x3)<<7) + ((in_csc_mode&0x3)<<6)) + 0x60 + 0x30;
        }
    }
    else
    {
    	if(outcs == 0x0)
        {
            csc_pass = 0x00;
            csc_coef_addr = (((in_csc_mode&0x3)<<7) + ((in_csc_mode&0x3)<<6));
        }
        else
        {
            csc_pass = 0x01;
            csc_coef_addr = (((in_csc_mode&0x3)<<7) + ((in_csc_mode&0x3)<<6)) + 0x30;
        }
    }

    if(in_br_swap || out_br_swap)
   	{
   		csc_pass = 0;
   	}
   	if(!csc_pass)
    {
        for(i=0; i<4; i++)
        {
            scal_dev[sel]->csc_coef[i].dwval = csc_tab[(csc_coef_addr>>2) + i];
			scal_dev[sel]->csc_coef[i+4 + out_br_swap * 4].dwval = csc_tab[(csc_coef_addr>>2) + i + 4 + in_br_swap * 4];
			scal_dev[sel]->csc_coef[i+8 - out_br_swap * 4].dwval = csc_tab[(csc_coef_addr>>2) + i + 8 - in_br_swap * 4];

        }
    }
    scal_dev[sel]->bypass.bits.csc_bypass_en = csc_pass;


    return 0;
}


__s32 DE_SCAL_Set_Out_Format(__u8 sel, __scal_out_type_t *out_type)
{
	scal_dev[sel]->output_fmt.bits.byte_seq = out_type->byte_seq;
	scal_dev[sel]->output_fmt.bits.data_fmt = out_type->fmt;
	if(out_type->fmt == DE_SCAL_OUTI0RGB888 || out_type->fmt == DE_SCAL_OUTI1RGB888)
	{
		scal_dev[sel]->output_fmt.bits.alpha_en = out_type->alpha_en;
	}
	else
	{
		scal_dev[sel]->output_fmt.bits.alpha_en = 0x0;
	}

    return 0;
}


__s32 DE_SCAL_Set_Out_Size(__u8 sel, __scal_scan_mod_t *out_scan, __scal_out_type_t *out_type,
                           __scal_out_size_t *out_size)
{
    __u32 out_w1, out_h1, out_w0, out_h0;
    if((out_type->fmt == DE_SCAL_OUTPYUV420) || (out_type->fmt == DE_SCAL_OUTPYUV422))
    {
        out_w1 = (out_size->width+ 0x1) >> 1;
    }
    else if(out_type->fmt == DE_SCAL_OUTPYUV411)
    {
        out_w1 = (out_size->width+ 0x3) >> 2;
    }
    else
    {
        out_w1 = out_size->width;
    }

    if(out_type->fmt == DE_SCAL_OUTPYUV420)
    {
        out_h1 = (out_size->height + 0x1) >> 1;
    }
    else
    {
        out_h1 = out_size->height;
    }
	out_h0 = out_size->height;
	out_w0 = out_size->width;
    out_h0 = (out_h0!=0) ? out_h0 : 1;
	out_h1 = (out_h1!=0) ? out_h1 : 1;
	out_w0 = (out_w0!=0) ? out_w0 : 1;
	out_w1 = (out_w1!=0) ? out_w1 : 1;

	scal_dev[sel]->ch0_outsize.bits.out_height = ((out_h0 + (out_scan->field & 0x1))>>out_scan->field) - 1;
    scal_dev[sel]->ch0_outsize.bits.out_width = out_w0 - 1;
    scal_dev[sel]->ch1_outsize.bits.out_height = ((out_h1 + (out_scan->field & 0x1)) >>out_scan->field) - 1;
    scal_dev[sel]->ch1_outsize.bits.out_width = out_w1 - 1;
    return 0;
}


__s32 DE_SCAL_Set_Trig_Line(__u8 sel, __u32 line)
{
	scal_dev[sel]->lint_ctrl.bits.field_sel = 0x0;
    scal_dev[sel]->lint_ctrl.bits.trig_line = line;
    return 0;
}

__s32 DE_SCAL_Set_Int_En(__u8 sel, __u32 int_num)
{
    if(int_num == 7)
    {
        scal_dev[sel]->int_en.bits.wb_en = 0x1;
    }
    else if(int_num == 9)
    {
        scal_dev[sel]->int_en.bits.line_en = 0x1;
    }
    else if(int_num == 10)
    {
        scal_dev[sel]->int_en.bits.reg_load_en = 0x1;
    }

    return 0;
}

__s32 DE_SCAL_Set_Di_Ctrl(__u8 sel, __u8 en, __u8 mode, __u8 diagintp_en, __u8 tempdiff_en)
{
	scal_dev[sel]->di_ctrl.bits.en = en;
	scal_dev[sel]->di_ctrl.bits.flag_out_en = 0x0; 
    scal_dev[sel]->di_ctrl.bits.mod = mode;
    scal_dev[sel]->di_ctrl.bits.diagintp_en = diagintp_en;
    scal_dev[sel]->di_ctrl.bits.tempdiff_en = tempdiff_en;
    return 0;
}


__s32 DE_SCAL_Set_Di_PreFrame_Addr(__u8 sel, __u32 luma_addr, __u32 chroma_addr)
{
    scal_dev[sel]->di_preluma.dwval = luma_addr + de_scal_ch0_offset;
	scal_dev[sel]->di_prechroma.dwval = chroma_addr + de_scal_ch1_offset;

	return 0;
}

__s32 DE_SCAL_Set_Di_MafFlag_Src(__u8 sel, __u32 cur_addr, __u32 pre_addr, __u32 stride)
{
    scal_dev[sel]->di_tileflag0.dwval = pre_addr;
	scal_dev[sel]->di_tileflag1.dwval = cur_addr;
    scal_dev[sel]->di_flaglinestrd.dwval = stride;
    return 0;
}


__s32 DE_SCAL_Start(__u8 sel)
{
	scal_dev[sel]->frm_ctrl.bits.frm_start = 0x1;

    return 0;
}

__s32 DE_SCAL_Set_Filtercoef_Ready(__u8 sel)
{
    scal_dev[sel]->frm_ctrl.bits.coef_rdy_en = 0x1;

    return 0;
}

__s32 DE_SCAL_Set_Reg_Rdy(__u8 sel)
{
    scal_dev[sel]->frm_ctrl.bits.reg_rdy_en = 0x1;

    return 0;
}


__s32 DE_SCAL_Reset(__u8 sel)
{
    scal_dev[sel]->frm_ctrl.bits.frm_start = 0x0;

    scal_dev[sel]->status.bits.wb_err_status = 0x0;
    scal_dev[sel]->status.bits.wb_err_losedata = 0x0;
    scal_dev[sel]->status.bits.wb_err_sync = 0x0;

    return 0;
}

__s32 DE_SCAL_Input_Select(__u8 sel, __u32 source)
{
    scal_dev[sel]->frm_ctrl.bits.in_ctrl = source;
    return 0;
}


__s32 DE_SCAL_Output_Select(__u8 sel, __u8 out)
{
    if(out == 3)//write back
    {
        scal_dev[sel]->frm_ctrl.bits.out_ctrl = 1;//disable scaler output to be
        scal_dev[sel]->frm_ctrl.bits.out_port_sel = 0;
    }
    else if(out < 3)
    {
        scal_dev[sel]->frm_ctrl.bits.out_ctrl = 0;//enable scaler output to be
        scal_dev[sel]->frm_ctrl.bits.out_port_sel = 0x0; 
    }

    return 0;
}

__s32 DE_SCAL_Writeback_Enable(__u8 sel)
{
    scal_dev[sel]->frm_ctrl.bits.wb_en = 0x1;

    return 0;
}

__s32 DE_SCAL_Writeback_Disable(__u8 sel)
{
	scal_dev[sel]->frm_ctrl.bits.wb_en = 0x0;

	return 0;
}

__s32 DE_SCAL_Writeback_Linestride_Enable(__u8 sel)
{
	scal_dev[sel]->wb_linestrd_en.dwval = 0x1;

	return 0;
}

__s32 DE_SCAL_Writeback_Linestride_Disable(__u8 sel)
{
	scal_dev[sel]->wb_linestrd_en.dwval = 0x0;

	return 0;
}


__s32 DE_SCAL_Enable(__u8 sel)
{
	de_scal_trd_fp_en = 0;
	de_scal_trd_itl_en = 0;
    scal_dev[sel]->modl_en.bits.en = 0x1;

    return 0;
}

__s32 DE_SCAL_Disable(__u8 sel)
{
    scal_dev[sel]->modl_en.bits.en = 0x0;

    return 0;
}


__s32 DE_SCAL_Set_Writeback_Addr(__u8 sel, __scal_buf_addr_t *addr)
{
    scal_dev[sel]->wb_addr0.dwval = addr->ch0_addr;
    scal_dev[sel]->wb_addr1.dwval = addr->ch1_addr;
    scal_dev[sel]->wb_addr2.dwval = addr->ch2_addr;


    return 0;
}


__u8 DE_SCAL_Get_Input_Format(__u8 sel)
{
    __u8 fmt_ret;
    fmt_ret = scal_dev[sel]->input_fmt.bits.data_fmt;

    return fmt_ret;
}

__u8 DE_SCAL_Get_Input_Mode(__u8 sel)
{
    __u8 mode_ret;
    mode_ret = scal_dev[sel]->input_fmt.bits.data_mod;

    return mode_ret;
}



__u8 DE_SCAL_Get_Output_Format(__u8 sel)
{
    __u8 fmt_ret;
    fmt_ret = scal_dev[sel]->output_fmt.bits.data_fmt;

    return fmt_ret;
}


__u16 DE_SCAL_Get_Input_Width(__u8 sel)
{
    __u16 in_w;
    in_w = scal_dev[sel]->ch0_insize.bits.in_width + 0x1;

    return in_w;
}

__u16 DE_SCAL_Get_Input_Height(__u8 sel)
{
    __u16 in_h;
    in_h = scal_dev[sel]->ch0_insize.bits.in_height + 0x1;

    return in_h;
}

__u16 DE_SCAL_Get_Output_Width(__u8 sel)
{
  __u16 out_w;
  out_w = scal_dev[sel]->ch0_outsize.bits.out_width + 0x1;

  return out_w;
}

__u16 DE_SCAL_Get_Output_Height(__u8 sel)
{
    __u16 out_h;
    out_h = scal_dev[sel]->ch0_outsize.bits.out_height + 0x1;

    return out_h;
}

__s32 DE_SCAL_Get_Start_Status(__u8 sel)
{
     if(scal_dev[sel]->modl_en.bits.en  && scal_dev[sel]->frm_ctrl.bits.frm_start)
     {
         return 0;
     }
     else
     {
         return -1;
     }
}

__s32 DE_SCAL_Get_Field_Status(__u8 sel)
{
	return scal_dev[sel]->status.bits.lcd_field;
}


__s32 iDE_SCAL_Matrix_Mul(__scal_matrix4x4 in1, __scal_matrix4x4 in2, __scal_matrix4x4 *result)
{
	__scal_matrix4x4 tmp;

	tmp.x00 = (in1.x00 * in2.x00 + in1.x01 * in2.x10 + in1.x02 * in2.x20 + in1.x03 * in2.x30) >> 10;
	tmp.x01 = (in1.x00 * in2.x01 + in1.x01 * in2.x11 + in1.x02 * in2.x21 + in1.x03 * in2.x31) >> 10;
	tmp.x02 = (in1.x00 * in2.x02 + in1.x01 * in2.x12 + in1.x02 * in2.x22 + in1.x03 * in2.x32) >> 10;
	tmp.x03 = (in1.x00 * in2.x03 + in1.x01 * in2.x13 + in1.x02 * in2.x23 + in1.x03 * in2.x33) >> 10;
	tmp.x10 = (in1.x10 * in2.x00 + in1.x11 * in2.x10 + in1.x12 * in2.x20 + in1.x13 * in2.x30) >> 10;
	tmp.x11 = (in1.x10 * in2.x01 + in1.x11 * in2.x11 + in1.x12 * in2.x21 + in1.x13 * in2.x31) >> 10;
	tmp.x12 = (in1.x10 * in2.x02 + in1.x11 * in2.x12 + in1.x12 * in2.x22 + in1.x13 * in2.x32) >> 10;
	tmp.x13 = (in1.x10 * in2.x03 + in1.x11 * in2.x13 + in1.x12 * in2.x23 + in1.x13 * in2.x33) >> 10;
	tmp.x20 = (in1.x20 * in2.x00 + in1.x21 * in2.x10 + in1.x22 * in2.x20 + in1.x23 * in2.x30) >> 10;
	tmp.x21 = (in1.x20 * in2.x01 + in1.x21 * in2.x11 + in1.x22 * in2.x21 + in1.x23 * in2.x31) >> 10;
	tmp.x22 = (in1.x20 * in2.x02 + in1.x21 * in2.x12 + in1.x22 * in2.x22 + in1.x23 * in2.x32) >> 10;
	tmp.x23 = (in1.x20 * in2.x03 + in1.x21 * in2.x13 + in1.x22 * in2.x23 + in1.x23 * in2.x33) >> 10;
	tmp.x30 = (in1.x30 * in2.x00 + in1.x31 * in2.x10 + in1.x32 * in2.x20 + in1.x33 * in2.x30) >> 10;
	tmp.x31 = (in1.x30 * in2.x01 + in1.x31 * in2.x11 + in1.x32 * in2.x21 + in1.x33 * in2.x31) >> 10;
	tmp.x32 = (in1.x30 * in2.x02 + in1.x31 * in2.x12 + in1.x32 * in2.x22 + in1.x33 * in2.x32) >> 10;
	tmp.x33 = (in1.x30 * in2.x03 + in1.x31 * in2.x13 + in1.x32 * in2.x23 + in1.x33 * in2.x33) >> 10;


	*result = tmp;

	return 0;
}


__s32 iDE_SCAL_Csc_Lmt(__s64 *value, __s32 min, __s32 max, __s32 shift, __s32 validbit)
{
    __s64 tmp;
    tmp = (*value)>>shift;
   if(tmp < min)
    *value = min & validbit;
   else if(tmp > max)
     *value = max & validbit;
   else
     *value = tmp & validbit;

   return 0;
}

__s32 DE_SCAL_Set_CSC_Coef_Enhance(__u8 sel, __u8 in_csc_mode, __u8 out_csc_mode, __u8 incs, __u8 outcs,
                                                   __s32  bright, __s32 contrast, __s32 saturaion, __s32 hue,
                                                   __u32  in_br_swap, __u32 out_br_swap)
{
	__scal_matrix4x4 matrixEn;
	__scal_matrix4x4 matrixconv, *ptmatrix;
	__scal_matrix4x4 matrixresult, tmpcoeff;
    __s64 *pt;
	__u32 i;
	__s32 sinv, cosv;  

	bright = bright*64/100;
	saturaion = saturaion*64/100;
	contrast = contrast*64/100;
	hue = hue*64/100;

	sinv = image_enhance_tab[8*16 + (hue&0x3f)];
	cosv = image_enhance_tab[8*16 + 8*8 + (hue&0x3f)];

	matrixEn.x00 = contrast << 5;
	matrixEn.x01 = 0;
	matrixEn.x02 = 0;
	matrixEn.x03 = (((bright - 32) + 16) <<10) - ( contrast << 9);
	matrixEn.x10 = 0;
	matrixEn.x11 = (contrast * saturaion * cosv) >> 7;
	matrixEn.x12 = (contrast * saturaion * sinv) >> 7;
	matrixEn.x13 = (1<<17) - ((matrixEn.x11 + matrixEn.x12)<<7);
	matrixEn.x20 = 0;
	matrixEn.x21 = (-contrast * saturaion * sinv)>>7;
	matrixEn.x22 = (contrast * saturaion * cosv) >> 7;
	matrixEn.x23 = (1<<17) - ((matrixEn.x22 + matrixEn.x21)<<7);
	matrixEn.x30 = 0;
	matrixEn.x31 = 0;
	matrixEn.x32 = 0;
	matrixEn.x33 = 1024;

	if((incs == 0) && (outcs == 0)) 
	{

		for(i=0; i<16; i++)
		{
			*((__s64 *)(&tmpcoeff.x00) + i) = ((__s64)*(image_enhance_tab + (in_csc_mode<<5) + i) <<32)>>32;	

		}

		ptmatrix = &tmpcoeff;

		iDE_SCAL_Matrix_Mul(matrixEn, *ptmatrix, &matrixconv);

		for(i=0; i<16; i++)
		{
			*((__s64 *)(&tmpcoeff.x00) + i) = ((__s64)*(image_enhance_tab + (in_csc_mode<<5) + 0x10 + i) <<32)>>32;
		}

		ptmatrix = &tmpcoeff;

		iDE_SCAL_Matrix_Mul(*ptmatrix, matrixconv, &matrixconv);

        matrixresult.x00 = (matrixconv.x11+8)/16;  matrixresult.x01 = (matrixconv.x10+8)/16;
        matrixresult.x02 = (matrixconv.x12+8)/16;  matrixresult.x03 = (matrixconv.x13+512)/1024;
        matrixresult.x10 = (matrixconv.x01+8)/16;  matrixresult.x11 = (matrixconv.x00+8)/16;
        matrixresult.x12 = (matrixconv.x02+8)/16;  matrixresult.x13 = (matrixconv.x03+512)/1024;
        matrixresult.x20 = (matrixconv.x21+8)/16;  matrixresult.x21 = (matrixconv.x20+8)/16;
        matrixresult.x22 = (matrixconv.x22+8)/16;  matrixresult.x23 = (matrixconv.x23+512)/1024;
        matrixresult.x30 = (matrixconv.x31+8)/16;  matrixresult.x31 = (matrixconv.x30+8)/16;
        matrixresult.x32 = (matrixconv.x32+8)/16;  matrixresult.x33 = (matrixconv.x33+8)/16;

	}
	else if((incs == 1) && (outcs == 0))
	{
		for(i=0; i<16; i++)
		{
			*((__s64 *)(&tmpcoeff.x00) + i) = ((__s64)*(image_enhance_tab + (in_csc_mode<<5) + 0x10 + i) <<32)>>32;
		}

		ptmatrix = &tmpcoeff;

		iDE_SCAL_Matrix_Mul(*ptmatrix, matrixEn, &matrixconv);
        matrixresult.x00 = matrixconv.x10/4;  matrixresult.x01 = matrixconv.x11/4;
        matrixresult.x02 = matrixconv.x12/4;  matrixresult.x03 = matrixconv.x13/256;
        matrixresult.x10 = matrixconv.x00/4;  matrixresult.x11 = matrixconv.x01/4;
        matrixresult.x12 = matrixconv.x02/4;  matrixresult.x13 = matrixconv.x03/256;
        matrixresult.x20 = matrixconv.x20/4;  matrixresult.x21 = matrixconv.x21/4;
        matrixresult.x22 = matrixconv.x22/4;  matrixresult.x23 = matrixconv.x23/256;
        matrixresult.x30 = matrixconv.x30/4;  matrixresult.x31 = matrixconv.x31/4;
        matrixresult.x32 = matrixconv.x32/4;  matrixresult.x33 = matrixconv.x33/4;

	}
	else if((incs == 0) && (outcs == 1))
	{
		for(i=0; i<16; i++)
		{
			*((__s64 *)(&tmpcoeff.x00) + i) = ((__s64)*(image_enhance_tab + (in_csc_mode<<5) + i) <<32)>>32;
		}

		ptmatrix = &tmpcoeff;

		iDE_SCAL_Matrix_Mul(matrixEn, *ptmatrix, &matrixconv);
        matrixresult.x00 = matrixconv.x01/4;  matrixresult.x01 = matrixconv.x00/4;
        matrixresult.x02 = matrixconv.x02/4;  matrixresult.x03 = matrixconv.x03/256;
        matrixresult.x10 = matrixconv.x11/4;  matrixresult.x11 = matrixconv.x10/4;
        matrixresult.x12 = matrixconv.x12/4;  matrixresult.x13 = matrixconv.x13/256;
        matrixresult.x20 = matrixconv.x21/4;  matrixresult.x21 = matrixconv.x20/4;
        matrixresult.x22 = matrixconv.x22/4;  matrixresult.x23 = matrixconv.x23/256;
        matrixresult.x30 = matrixconv.x31/4;  matrixresult.x31 = matrixconv.x30/4;
        matrixresult.x32 = matrixconv.x32/4;  matrixresult.x33 = matrixconv.x33/4;
	}
	else 
	{
		matrixresult = matrixEn;
	}

    iDE_SCAL_Csc_Lmt(&matrixresult.x00, -4095, 4095, 0, 8191);
    iDE_SCAL_Csc_Lmt(&matrixresult.x01, -4095, 4095, 0, 8191);
    iDE_SCAL_Csc_Lmt(&matrixresult.x02, -4095, 4095, 0, 8191);
    iDE_SCAL_Csc_Lmt(&matrixresult.x03, -8191, 8191, 0, 16383);
    iDE_SCAL_Csc_Lmt(&matrixresult.x10, -4095, 4095, 0, 8191);
    iDE_SCAL_Csc_Lmt(&matrixresult.x11, -4095, 4095, 0, 8191);
    iDE_SCAL_Csc_Lmt(&matrixresult.x12, -4095, 4095, 0, 8191);
    iDE_SCAL_Csc_Lmt(&matrixresult.x13, -8191, 8191, 0, 16383);
    iDE_SCAL_Csc_Lmt(&matrixresult.x20, -4095, 4095, 0, 8191);
    iDE_SCAL_Csc_Lmt(&matrixresult.x21, -4095, 4095, 0, 8191);
    iDE_SCAL_Csc_Lmt(&matrixresult.x22, -4095, 4095, 0, 8191);
    iDE_SCAL_Csc_Lmt(&matrixresult.x23, -8191, 8191, 0, 16383);

    pt = (__s64 *)&(matrixresult.x00);
    for(i=0; i<4; i++)
    {
        scal_dev[sel]->csc_coef[i].dwval = *(pt + i);
		scal_dev[sel]->csc_coef[i+4 + out_br_swap * 4].dwval =  *(pt + i + 4 + in_br_swap * 4);
		scal_dev[sel]->csc_coef[i+8 - out_br_swap * 4].dwval =  *(pt + i + 8 - in_br_swap * 4);
	}

    scal_dev[sel]->bypass.bits.csc_bypass_en = 0;

	return 0;
}

__s32 DE_SCAL_Get_3D_In_Single_Size(__scal_3d_inmode_t inmode, __scal_src_size_t *fullsize,__scal_src_size_t *singlesize)
{
	switch(inmode)
	{
		case DE_SCAL_3DIN_TB:
			singlesize->src_width = fullsize->src_width;
			singlesize->src_height = fullsize->src_height>>1;
			singlesize->scal_width = fullsize->scal_width;
			singlesize->scal_height = fullsize->scal_height>>1;
			singlesize->x_off = fullsize->x_off;
			singlesize->y_off = fullsize->y_off;
			break;
		case DE_SCAL_3DIN_SSF:
		case DE_SCAL_3DIN_SSH:
			singlesize->src_width = fullsize->src_width>>1;
			singlesize->src_height = fullsize->src_height;
			singlesize->scal_width = fullsize->scal_width>>1;
			singlesize->scal_height = fullsize->scal_height;
			singlesize->x_off = fullsize->x_off;
			singlesize->y_off = fullsize->y_off;
			break;
		case DE_SCAL_3DIN_LI:
			singlesize->src_width = fullsize->src_width;
			singlesize->src_height = fullsize->src_height>>1;
			singlesize->scal_width = fullsize->scal_width;
			singlesize->scal_height = fullsize->scal_height>>1;
			singlesize->x_off = fullsize->x_off;
			singlesize->y_off = fullsize->y_off>>1;
			break;
		case DE_SCAL_3DIN_FP:
			singlesize->src_width = fullsize->src_width;
			singlesize->src_height = fullsize->src_height;
			singlesize->scal_width = fullsize->scal_width;
			singlesize->scal_height = fullsize->scal_height;
			singlesize->x_off = fullsize->x_off;
			singlesize->y_off = fullsize->y_off;
			break;
		default:
			break;
	}

	return 0;
}

__s32 DE_SCAL_Get_3D_Out_Single_Size(__scal_3d_outmode_t outmode, __scal_out_size_t *singlesize,__scal_out_size_t *fullsize)
{
	switch(outmode)
	{
		case DE_SCAL_3DOUT_CI_1:
		case DE_SCAL_3DOUT_CI_2:
		case DE_SCAL_3DOUT_CI_3:
		case DE_SCAL_3DOUT_CI_4:
		case DE_SCAL_3DOUT_HDMI_SSF:
		case DE_SCAL_3DOUT_HDMI_SSH:
			singlesize->height = fullsize->height;
			singlesize->width  = fullsize->width>>1;
			break;
		case DE_SCAL_3DOUT_LIRGB:
		case DE_SCAL_3DOUT_HDMI_TB:
		case DE_SCAL_3DOUT_HDMI_FPP:
		case DE_SCAL_3DOUT_HDMI_LI:
			singlesize->height = fullsize->height>>1;
			singlesize->width  = fullsize->width;
			break;
		case DE_SCAL_3DOUT_HDMI_FPI:
			singlesize->height = fullsize->height>>2;
			singlesize->width  = fullsize->width;
			break;
		case DE_SCAL_3DOUT_HDMI_FA: 
			singlesize->height = fullsize->height;
			singlesize->width  = fullsize->width;
		default:
			break;

	}
	return 0;
}

__s32 DE_SCAL_Get_3D_Out_Full_Size(__scal_3d_outmode_t outmode, __scal_out_size_t *singlesize,__scal_out_size_t *fullsize)
{
	switch(outmode)
	{
		case DE_SCAL_3DOUT_CI_1:
		case DE_SCAL_3DOUT_CI_2:
		case DE_SCAL_3DOUT_CI_3:
		case DE_SCAL_3DOUT_CI_4:
		case DE_SCAL_3DOUT_HDMI_SSF:
		case DE_SCAL_3DOUT_HDMI_SSH:
			fullsize->height = singlesize->height;
			fullsize->width  = singlesize->width<<1;
			break;
		case DE_SCAL_3DOUT_LIRGB:
		case DE_SCAL_3DOUT_HDMI_TB:
		case DE_SCAL_3DOUT_HDMI_FPP:
		case DE_SCAL_3DOUT_HDMI_LI:
			fullsize->height = singlesize->height<<1;
			fullsize->width  = singlesize->width;
			break;
		case DE_SCAL_3DOUT_HDMI_FPI:
			fullsize->height = singlesize->height<<2;
			fullsize->width  = singlesize->width;
			break;
		case DE_SCAL_3DOUT_HDMI_FA: 
			fullsize->height = singlesize->height;
			fullsize->width  = singlesize->width;
		default:
			break;

	}
	return 0;
}


__s32 DE_SCAL_Set_3D_Fb_Addr(__u8 sel, __scal_buf_addr_t *addr, __scal_buf_addr_t *addrtrd)
{
    scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
    scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
    scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;
	if(de_scal_trd_fp_en)
	{
		scal_dev[sel]->trd_buf_addr0.dwval = addrtrd->ch0_addr + de_scal_ch0r_offset;
		scal_dev[sel]->trd_buf_addr1.dwval = addrtrd->ch1_addr + de_scal_ch1r_offset;
		scal_dev[sel]->trd_buf_addr2.dwval = addrtrd->ch2_addr + de_scal_ch2r_offset;
	}
	else
	{
		scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
		scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
		scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
	}

    return 0;
}

__s32 DE_SCAL_Set_3D_Di_PreFrame_Addr(__u8 sel, __scal_buf_addr_t *addr, __scal_buf_addr_t *addrtrd)
{
    scal_dev[sel]->di_preluma.dwval = addr->ch0_addr+ de_scal_ch0_offset;
	scal_dev[sel]->di_prechroma.dwval = addr->ch1_addr+ de_scal_ch1_offset;
	if(de_scal_trd_fp_en)
	{
		scal_dev[sel]->trd_preluma.dwval = addrtrd->ch0_addr + de_scal_ch0r_offset;
		scal_dev[sel]->trd_prechroma.dwval = addrtrd->ch1_addr + de_scal_ch1r_offset;
	}
	else
	{
		scal_dev[sel]->trd_preluma.dwval = addr->ch0_addr + de_scal_ch0r_offset;
		scal_dev[sel]->trd_prechroma.dwval = addr->ch1_addr + de_scal_ch1r_offset;
	}
    return 0;
}


__s32 DE_SCAL_Set_3D_Ctrl(__u8 sel, __u8 trden, __scal_3d_inmode_t inmode,
								__scal_3d_outmode_t outmode)
{
	__u8 in_li_en=0;
	__u8 out_ci_en=0, out_tb_en=0, out_ss_en=0, out_itl_en=0;
	__u8 model_sel=0;
	__u8 ci_mod=0;

	switch(inmode)
	{
		case DE_SCAL_3DIN_LI:;
			in_li_en = 1;
			break;
		default:
			in_li_en = 0;
			break;
	}

    if(trden)
    {
    	switch(outmode)
    	{
    		case DE_SCAL_3DOUT_CI_1:;
    			ci_mod = 0;out_ci_en = 1;break;
    		case DE_SCAL_3DOUT_CI_2:
    			ci_mod = 1;out_ci_en = 1;break;
    		case DE_SCAL_3DOUT_CI_3:
    			ci_mod = 2;out_ci_en = 1;break;
    		case DE_SCAL_3DOUT_CI_4:
    			ci_mod = 3;out_ci_en = 1;break;
    		case DE_SCAL_3DOUT_HDMI_SSF:;
    		case DE_SCAL_3DOUT_HDMI_SSH:
    			out_ss_en = 1;
    			break;
    		case DE_SCAL_3DOUT_HDMI_TB:;
    		case DE_SCAL_3DOUT_HDMI_FPP:
    			out_tb_en = 1;
    			break;
    		case DE_SCAL_3DOUT_HDMI_FPI:
    			out_tb_en = 1;
    			out_itl_en = 1;
    			break;
    		case DE_SCAL_3DOUT_HDMI_FA: 
    			break;
    		default:
    			break;
    	}
	}
	model_sel = trden? (out_tb_en ? 2 :1 ) : 0;

	scal_dev[sel]->trd_ctrl.bits.mod_sel = model_sel;
	scal_dev[sel]->trd_ctrl.bits.ci_out_en = out_ci_en;
	scal_dev[sel]->trd_ctrl.bits.ss_out_en = out_ss_en;
	scal_dev[sel]->trd_ctrl.bits.li_in_en = in_li_en;
	scal_dev[sel]->trd_ctrl.bits.tb_out_scan_mod = out_itl_en;
	scal_dev[sel]->trd_ctrl.bits.ci_out_mod = ci_mod;
	scal_dev[sel]->trd_ctrl.bits.tb_out_mod_field = out_tb_en ? (out_itl_en ? 3 : 1) : 0;
	scal_dev[sel]->field_ctrl.bits.valid_field_cnt = out_tb_en ? (out_itl_en ? 3 : 1) : 0;
	scal_dev[sel]->field_ctrl.bits.field_cnt = out_tb_en ? (out_itl_en ? 0xC : 2) : 0;
	de_scal_trd_itl_en = out_itl_en;
	return 0;
}

__s32 DE_SCAL_Config_3D_Src(__u8 sel, __scal_buf_addr_t *addr, __scal_src_size_t *size,
                           __scal_src_type_t *type, __scal_3d_inmode_t trdinmode, __scal_buf_addr_t *addrtrd)
{
    __u8 w_shift, h_shift;
	__u32 image_w0, image_w1, image_h0, image_h1;
	__u32 x_off0, y_off0, x_off1, y_off1;
	__u32 in_w0, in_h0, in_w1, in_h1;
	__u8  de_scal_ch0_dx0,de_scal_ch0_dx1,de_scal_ch0_dy0;
	__u8  de_scal_ch1_dx0,de_scal_ch1_dx1,de_scal_ch1_dy0;

	image_w0 = size->src_width;
	image_h0 = size->src_height;  
	in_w0 = size->scal_width;
	in_h0 = size->scal_height;
	x_off0 = size->x_off;
	y_off0 = size->y_off;

	de_scal_trd_fp_en = 0;

    if(type->fmt == DE_SCAL_INYUV422 || type->fmt == DE_SCAL_INYUV420)
    {
        w_shift = 1;
        image_w0 = (image_w0 + 1)&0xfffffffe;
    	image_w1 = (image_w0)>>w_shift;
        in_w0 = in_w0 & 0xfffffffe;
    	in_w1 = (in_w0 + 0x1)>>w_shift;
        x_off0 = x_off0 & 0xfffffffe;
    	x_off1 = (x_off0)>>w_shift;
    }
    else if(type->fmt == DE_SCAL_INYUV411)
    {
        w_shift = 2;
    	image_w1 = (image_w0 + 0x3)>>w_shift;
        in_w0 &= 0xfffffffc;
    	in_w1 = (in_w0 + 0x3)>>w_shift;
        x_off0 &= 0xfffffffc;
    	x_off1 = (x_off0)>>w_shift;
    }
    else
    {
        w_shift = 0;
    	image_w1 = image_w0;
    	in_w1 = in_w0;
    	x_off1 = x_off0;
    }
    if(type->fmt == DE_SCAL_INYUV420)
    {
        h_shift = 1;
		image_h0 &= 0xfffffffe;
		image_h1 = ((image_h0 + 0x1) >> h_shift);
        in_h0 &= 0xfffffffe;
    	in_h1 = (in_h0 + 0x1)>>h_shift;
        y_off0 &= 0xfffffffe;
    	y_off1 = (y_off0)>>h_shift;
    }
    else
    {
        h_shift = 0;
		image_h1 = image_h0;
    	in_h1 = in_h0;
    	y_off1 = y_off0;
    }

    in_h0 = (in_h0!=0) ? in_h0 : 1;
	in_h1 = (in_h1!=0) ? in_h1 : 1;
	in_w0 = (in_w0!=0) ? in_w0 : 1;
	in_w1 = (in_w1!=0) ? in_w1 : 1;

	if(type->mod == DE_SCAL_PLANNAR)
	{
		if((trdinmode == DE_SCAL_3DIN_TB))
		{
		    scal_dev[sel]->linestrd0.dwval = image_w0;
			scal_dev[sel]->linestrd1.dwval = image_w1;
			scal_dev[sel]->linestrd2.dwval = image_w1;

	        de_scal_ch0_offset = image_w0 * y_off0 + x_off0;
	        de_scal_ch1_offset = image_w1 * y_off1 + x_off1;
	        de_scal_ch2_offset = image_w1 * y_off1 + x_off1;

			de_scal_ch0r_offset = image_w0 * image_h0 + de_scal_ch0_offset;
			de_scal_ch1r_offset = image_w1 * image_h1 + de_scal_ch1_offset;
			de_scal_ch2r_offset = image_w1 * image_h1 + de_scal_ch2_offset;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;

		}
		else if((trdinmode == DE_SCAL_3DIN_FP))
		{
		    scal_dev[sel]->linestrd0.dwval = image_w0;
			scal_dev[sel]->linestrd1.dwval = image_w1;
			scal_dev[sel]->linestrd2.dwval = image_w1;

	        de_scal_ch0_offset = image_w0 * y_off0 + x_off0;
	        de_scal_ch1_offset = image_w1 * y_off1 + x_off1;
	        de_scal_ch2_offset = image_w1 * y_off1 + x_off1;

			de_scal_ch0r_offset = de_scal_ch0_offset;
			de_scal_ch1r_offset = de_scal_ch1_offset;
			de_scal_ch2r_offset = de_scal_ch2_offset;

			de_scal_trd_fp_en = 1;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addrtrd->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addrtrd->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addrtrd->ch2_addr + de_scal_ch2r_offset;
		}
		else if((trdinmode == DE_SCAL_3DIN_SSF) || (trdinmode == DE_SCAL_3DIN_SSH))
		{
		    scal_dev[sel]->linestrd0.dwval = image_w0<<1;
			scal_dev[sel]->linestrd1.dwval = image_w1<<1;
			scal_dev[sel]->linestrd2.dwval = image_w1<<1;

	        de_scal_ch0_offset = (image_w0<<1) * y_off0 + x_off0;
	        de_scal_ch1_offset = (image_w1<<1) * y_off1 + x_off1;
	        de_scal_ch2_offset = (image_w1<<1) * y_off1 + x_off1;

			de_scal_ch0r_offset = image_w0 + de_scal_ch0_offset;
			de_scal_ch1r_offset = image_w1 + de_scal_ch1_offset;
			de_scal_ch2r_offset = image_w1 + de_scal_ch2_offset;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
		}
		else if((trdinmode == DE_SCAL_3DIN_LI))
		{
		    scal_dev[sel]->linestrd0.dwval = image_w0;
			scal_dev[sel]->linestrd1.dwval = image_w1;
			scal_dev[sel]->linestrd2.dwval = image_w1;

	        de_scal_ch0_offset = (image_w0) * (y_off0<<1) + x_off0;
	        de_scal_ch1_offset = (image_w1) * (y_off1<<1) + x_off1;
	        de_scal_ch2_offset = (image_w1) * (y_off1<<1) + x_off1;

			de_scal_ch0r_offset = image_w0 + de_scal_ch0_offset;
			de_scal_ch1r_offset = image_w1 + de_scal_ch1_offset;
			de_scal_ch2r_offset = image_w1 + de_scal_ch2_offset;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
		}
	}
	else if(type->mod == DE_SCAL_INTERLEAVED)
	{
		if((trdinmode == DE_SCAL_3DIN_FP))
		{
			scal_dev[sel]->linestrd0.dwval = image_w0<<(2-w_shift);
			scal_dev[sel]->linestrd1.dwval = 0;
			scal_dev[sel]->linestrd2.dwval = 0;

	        de_scal_ch0_offset = (image_w0<<(2-w_shift)) * y_off0 + x_off0;
	        de_scal_ch1_offset = 0;
	        de_scal_ch2_offset = 0;

			de_scal_ch0r_offset = de_scal_ch0_offset;
			de_scal_ch1r_offset = de_scal_ch1_offset;
			de_scal_ch2r_offset = de_scal_ch2_offset;

			de_scal_trd_fp_en = 1;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addrtrd->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addrtrd->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addrtrd->ch2_addr + de_scal_ch2r_offset;
		}
		else if((trdinmode == DE_SCAL_3DIN_TB))
		{
			scal_dev[sel]->linestrd0.dwval = image_w0<<(2-w_shift);
			scal_dev[sel]->linestrd1.dwval = 0;
			scal_dev[sel]->linestrd2.dwval = 0;

	        de_scal_ch0_offset = (image_w0<<(2-w_shift)) * y_off0 + x_off0;
	        de_scal_ch1_offset = 0;
	        de_scal_ch2_offset = 0;

			de_scal_ch0r_offset = (image_w0<<(2-w_shift)) * image_h0 + de_scal_ch0_offset;
			de_scal_ch1r_offset = de_scal_ch1_offset;
			de_scal_ch2r_offset = de_scal_ch2_offset;

			de_scal_trd_fp_en = 0;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
		}
		else if((trdinmode == DE_SCAL_3DIN_SSF)||(trdinmode == DE_SCAL_3DIN_SSH))
		{
			scal_dev[sel]->linestrd0.dwval = image_w0<<(3-w_shift);
			scal_dev[sel]->linestrd1.dwval = 0;
			scal_dev[sel]->linestrd2.dwval = 0;

	        de_scal_ch0_offset = (image_w0<<(3-w_shift)) * y_off0 + x_off0;
	        de_scal_ch1_offset = 0;
	        de_scal_ch2_offset = 0;

			de_scal_ch0r_offset = (image_w0<<(2-w_shift)) + de_scal_ch0_offset;
			de_scal_ch1r_offset = de_scal_ch1_offset;
			de_scal_ch2r_offset = de_scal_ch2_offset;

			de_scal_trd_fp_en = 0;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
		}
		else if((trdinmode == DE_SCAL_3DIN_LI))
		{
			scal_dev[sel]->linestrd0.dwval = image_w0<<(2-w_shift);
			scal_dev[sel]->linestrd1.dwval = 0;
			scal_dev[sel]->linestrd2.dwval = 0;

	        de_scal_ch0_offset = (image_w0<<(2-w_shift)) * (y_off0<<1) + x_off0;
	        de_scal_ch1_offset = 0;
	        de_scal_ch2_offset = 0;

			de_scal_ch0r_offset = (image_w0<<(2-w_shift)) + de_scal_ch0_offset;
			de_scal_ch1r_offset = de_scal_ch1_offset;
			de_scal_ch2r_offset = de_scal_ch2_offset;

			de_scal_trd_fp_en = 0;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
		}
	}
	else if(type->mod == DE_SCAL_UVCOMBINED)
	{
		if((trdinmode == DE_SCAL_3DIN_SSF) || (trdinmode == DE_SCAL_3DIN_SSH))
		{
			scal_dev[sel]->linestrd0.dwval = image_w0 * 2;
			scal_dev[sel]->linestrd1.dwval = image_w1 * 2 * 2;
			scal_dev[sel]->linestrd2.dwval = 0;

	        de_scal_ch0_offset = (2 * image_w0) * y_off0 + x_off0;
	        de_scal_ch1_offset = 2 * (2 * image_w1) * y_off1 + 2 * x_off1;
	        de_scal_ch2_offset = 0;

			de_scal_ch0r_offset = image_w0;
			de_scal_ch1r_offset = image_w1 * 2;
			de_scal_ch2r_offset = 0;

			de_scal_trd_fp_en = 0;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
		}
		else if(trdinmode == DE_SCAL_3DIN_TB)
		{
			scal_dev[sel]->linestrd0.dwval = image_w0;
			scal_dev[sel]->linestrd1.dwval = image_w1 * 2;
			scal_dev[sel]->linestrd2.dwval = 0;

	        de_scal_ch0_offset = (image_w0) * y_off0 + x_off0;
	        de_scal_ch1_offset = (image_w1 * 2) * y_off1 + 2 * x_off1;
	        de_scal_ch2_offset = 0;

			de_scal_ch0r_offset = (image_w0) * image_h0 + de_scal_ch0_offset;
			de_scal_ch1r_offset = (image_w1 * 2) * image_h1 + de_scal_ch1_offset;
			de_scal_ch2r_offset = 0;

			de_scal_trd_fp_en = 0;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
		}
		else if(trdinmode == DE_SCAL_3DIN_FP)
		{
			scal_dev[sel]->linestrd0.dwval = image_w0;
			scal_dev[sel]->linestrd1.dwval = image_w1 * 2;
			scal_dev[sel]->linestrd2.dwval = 0;

	        de_scal_ch0_offset = (image_w0) * y_off0 + x_off0;
	        de_scal_ch1_offset = (image_w1 * 2) * y_off1 + 2 * x_off1;
	        de_scal_ch2_offset = 0;

			de_scal_ch0r_offset = de_scal_ch0_offset;
			de_scal_ch1r_offset = de_scal_ch1_offset;
			de_scal_ch2r_offset = de_scal_ch2_offset;

			de_scal_trd_fp_en = 1;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addrtrd->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addrtrd->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addrtrd->ch2_addr + de_scal_ch2r_offset;
		}
		else if(trdinmode == DE_SCAL_3DIN_LI)
		{
			scal_dev[sel]->linestrd0.dwval = image_w0;
			scal_dev[sel]->linestrd1.dwval = image_w1 * 2;
			scal_dev[sel]->linestrd2.dwval = 0;

	        de_scal_ch0_offset = (image_w0 * 2) * y_off0 + x_off0;
	        de_scal_ch1_offset = 2 * (image_w1 * 2) * y_off1 + 2 * x_off1;
	        de_scal_ch2_offset = 0;

			de_scal_ch0r_offset = de_scal_ch0_offset + image_w0;
			de_scal_ch1r_offset = de_scal_ch1_offset + image_w1 * 2;
			de_scal_ch2r_offset = de_scal_ch2_offset;

			de_scal_trd_fp_en = 0;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
		}
	}
	else if(type->mod == DE_SCAL_UVCOMBINEDMB)
	{
		if((trdinmode == DE_SCAL_3DIN_TB))
		{
		    scal_dev[sel]->linestrd0.dwval = (((image_w0+0x1f)&0xffe0) - 0x1f)<<0x05;;
			scal_dev[sel]->linestrd1.dwval = (((((image_w1)<<1)+0x1f)&0xffe0) - 0x1f) << 0x05;
			scal_dev[sel]->linestrd2.dwval = 0x00;

			de_scal_ch0_dx0 = (x_off0 & 0x1f);
			de_scal_ch0_dy0 = (y_off0 & 0x1f);
			de_scal_ch0_dx1 = ((de_scal_ch0_dx0 & 0x1f) + in_w0 + 0x1f) &0x1f;
			de_scal_ch1_dx0 = (((x_off1)<<1)&0x1f);
			de_scal_ch1_dy0 = ((y_off1)&0x1f);
			de_scal_ch1_dx1 = ((de_scal_ch1_dx0 & 0x1e) + ((in_w1)<<1) + 0x1f) & 0x1f;

			scal_dev[sel]->tb_off0.bits.x_offset0 = de_scal_ch0_dx0;
			scal_dev[sel]->tb_off0.bits.y_offset0 = de_scal_ch0_dy0;
			scal_dev[sel]->tb_off0.bits.x_offset1 = de_scal_ch0_dx1;
			scal_dev[sel]->tb_off1.bits.x_offset0 = de_scal_ch1_dx0;
			scal_dev[sel]->tb_off1.bits.y_offset0 = de_scal_ch1_dy0;
			scal_dev[sel]->tb_off1.bits.x_offset1 = de_scal_ch1_dx1;
			scal_dev[sel]->trd_tb_off0.bits.x_offset0 = de_scal_ch0_dx0;
			scal_dev[sel]->trd_tb_off0.bits.y_offset0 = (image_h0 + y_off0) & 0x1f;
			scal_dev[sel]->trd_tb_off0.bits.x_offset1 = de_scal_ch0_dx1;
			scal_dev[sel]->trd_tb_off1.bits.x_offset0 = de_scal_ch1_dx0;
			scal_dev[sel]->trd_tb_off1.bits.y_offset0 = (image_h1 + y_off1) & 0x1f;
			scal_dev[sel]->trd_tb_off1.bits.x_offset1 = de_scal_ch1_dx1;

			de_scal_ch0_offset = ((image_w0 + 0x1f) &0xffe0) * (y_off0& 0xffe0) + ((y_off0& 0x01f)<<5) +
	                              ((x_off0& 0xffe0)<<5);
	        de_scal_ch1_offset = (((image_w1<< 0x01)+0x1f)&0xffe0) * ((y_off1) & 0xffe0) +
	                              (((y_off1) & 0x01f)<<5) + (((x_off1<<0x01) & 0xffe0)<<5);
	        de_scal_ch2_offset = 0x0;

			de_scal_ch0r_offset = ((image_w0 + 0x1f) &0xffe0) * ((y_off0+ image_h0) & 0xffe0) +
							(((y_off0+ image_h0)& 0x01f)<<5) + ((x_off0& 0xffe0)<<5);
			de_scal_ch1r_offset = (((image_w1<< 0x01)+0x1f)&0xffe0) * ((y_off1+ image_h1) & 0xffe0) +
	                       (((y_off1+ image_h1) & 0x01f)<<5) + (((x_off1<<0x01) & 0xffe0)<<5);
			de_scal_ch2r_offset = 0x0;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
		}
		else if((trdinmode == DE_SCAL_3DIN_FP))
		{
			de_scal_trd_fp_en = 1;
		    scal_dev[sel]->linestrd0.dwval = (((image_w0+0x1f)&0xffe0) - 0x1f)<<0x05;;
			scal_dev[sel]->linestrd1.dwval = (((((image_w1)<<1)+0x1f)&0xffe0) - 0x1f) << 0x05;
			scal_dev[sel]->linestrd2.dwval = 0x00;

			de_scal_ch0_dx0 = (x_off0 & 0x1f);
			de_scal_ch0_dy0 = (y_off0 & 0x1f);
			de_scal_ch0_dx1 = ((de_scal_ch0_dx0 & 0x1f) + in_w0 + 0x1f) &0x1f;
			de_scal_ch1_dx0 = (((x_off1)<<1)&0x1f);
			de_scal_ch1_dy0 = ((y_off1)&0x1f);
			de_scal_ch1_dx1 = ((de_scal_ch1_dx0 & 0x1e) + ((in_w1)<<1) + 0x1f) & 0x1f;

			scal_dev[sel]->tb_off0.bits.x_offset0 = de_scal_ch0_dx0;
			scal_dev[sel]->tb_off0.bits.y_offset0 = de_scal_ch0_dy0;
			scal_dev[sel]->tb_off0.bits.x_offset1 = de_scal_ch0_dx1;
			scal_dev[sel]->tb_off1.bits.x_offset0 = de_scal_ch1_dx0;
			scal_dev[sel]->tb_off1.bits.y_offset0 = de_scal_ch1_dy0;
			scal_dev[sel]->tb_off1.bits.x_offset1 = de_scal_ch1_dx1;
			scal_dev[sel]->trd_tb_off0.bits.x_offset0 = de_scal_ch0_dx0;
			scal_dev[sel]->trd_tb_off0.bits.y_offset0 = de_scal_ch0_dy0;
			scal_dev[sel]->trd_tb_off0.bits.x_offset1 = de_scal_ch0_dx1;
			scal_dev[sel]->trd_tb_off1.bits.x_offset0 = de_scal_ch1_dx0;
			scal_dev[sel]->trd_tb_off1.bits.y_offset0 = de_scal_ch1_dy0;
			scal_dev[sel]->trd_tb_off1.bits.x_offset1 = de_scal_ch1_dx1;

			de_scal_ch0_offset = ((image_w0 + 0x1f) &0xffe0) * (y_off0& 0xffe0) + ((y_off0& 0x01f)<<5) +
	                              ((x_off0& 0xffe0)<<5);
	        de_scal_ch1_offset = (((image_w1<< 0x01)+0x1f)&0xffe0) * ((y_off1) & 0xffe0) +
	                              (((y_off1) & 0x01f)<<5) + (((x_off1<<0x01) & 0xffe0)<<5);
	        de_scal_ch2_offset = 0x0;

			de_scal_ch0r_offset = de_scal_ch0_offset;
			de_scal_ch1r_offset = de_scal_ch1_offset;
			de_scal_ch2r_offset = 0x0;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addrtrd->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addrtrd->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addrtrd->ch2_addr + de_scal_ch2r_offset;
		}
		else if(((trdinmode == DE_SCAL_3DIN_SSF) ||(trdinmode == DE_SCAL_3DIN_SSH)))
		{
		    scal_dev[sel]->linestrd0.dwval = (((2*image_w0+0x1f)&0xffe0) - 0x1f)<<0x05;;
			scal_dev[sel]->linestrd1.dwval = (((((2*image_w1)<<1)+0x1f)&0xffe0) - 0x1f) << 0x05;
			scal_dev[sel]->linestrd2.dwval = 0x00;

			de_scal_ch0_dx0 = (x_off0 & 0x1f);
			de_scal_ch0_dy0 = (y_off0 & 0x1f);
			de_scal_ch0_dx1 = ((de_scal_ch0_dx0 & 0x1f) + in_w0 + 0x1f) &0x1f;
			de_scal_ch1_dx0 = (((x_off1)<<1)&0x1f);
			de_scal_ch1_dy0 = ((y_off1)&0x1f);
			de_scal_ch1_dx1 = ((de_scal_ch1_dx0 & 0x1e) + ((in_w1)<<1) + 0x1f) & 0x1f;

			scal_dev[sel]->tb_off0.bits.x_offset0 = de_scal_ch0_dx0;
			scal_dev[sel]->tb_off0.bits.y_offset0 = de_scal_ch0_dy0;
			scal_dev[sel]->tb_off0.bits.x_offset1 = de_scal_ch0_dx1;
			scal_dev[sel]->tb_off1.bits.x_offset0 = de_scal_ch1_dx0;
			scal_dev[sel]->tb_off1.bits.y_offset0 = de_scal_ch1_dy0;
			scal_dev[sel]->tb_off1.bits.x_offset1 = de_scal_ch1_dx1;
			scal_dev[sel]->trd_tb_off0.bits.x_offset0 = (image_w0 + x_off0) & 0x1f;
			scal_dev[sel]->trd_tb_off0.bits.y_offset0 = de_scal_ch0_dy0;
			scal_dev[sel]->trd_tb_off0.bits.x_offset1 = (((image_w0 + x_off0) & 0x1f) + in_w0 + 0x1f)&0x1f;
			scal_dev[sel]->trd_tb_off1.bits.x_offset0 = ((2 * (image_w1 + x_off1)) & 0x1f);
			scal_dev[sel]->trd_tb_off1.bits.y_offset0 = de_scal_ch1_dy0;
			scal_dev[sel]->trd_tb_off1.bits.x_offset1 = (((2 * (image_w1 + x_off1)) & 0x1f) + (in_w1<<1) + 0x1f)&0x1f;

			de_scal_ch0_offset = ((2 *image_w0 + 0x1f) &0xffe0) * (y_off0& 0xffe0) + ((y_off0& 0x01f)<<5) +
	                              ((x_off0& 0xffe0)<<5);
	        de_scal_ch1_offset = ((((2 * image_w1)<< 0x01)+0x1f)&0xffe0) * ((y_off1) & 0xffe0) +
	                              (((y_off1) & 0x01f)<<5) + (((x_off1<<0x01) & 0xffe0)<<5);
	        de_scal_ch2_offset = 0x0;

			de_scal_ch0r_offset = ((2 *image_w0 + 0x1f) &0xffe0) * (y_off0& 0xffe0) + ((y_off0& 0x01f)<<5) +
	                              (((image_w0 + x_off0) & 0xffe0)<<5);
			de_scal_ch1r_offset = ((((2 * image_w1)<< 0x01)+0x1f)&0xffe0) * ((y_off1) & 0xffe0) +
	                              (((y_off1) & 0x01f)<<5) + ((((image_w1 + x_off1)<<0x01) & 0xffe0)<<5);
			de_scal_ch2r_offset = 0x0;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
		}
		else if((trdinmode == DE_SCAL_3DIN_LI))
		{
		    scal_dev[sel]->linestrd0.dwval = ((((image_w0+0x1f)&0xffe0) - 0x1f)<<0x05);
			scal_dev[sel]->linestrd1.dwval = ((((((image_w1)<<1)+0x1f)&0xffe0) - 0x1f) << 0x05);
			scal_dev[sel]->linestrd2.dwval = 0x00;

			de_scal_ch0_dx0 = (x_off0 & 0x1f);
			de_scal_ch0_dy0 = ((2*y_off0) & 0x1f);
			de_scal_ch0_dx1 = ((de_scal_ch0_dx0 & 0x1f) + in_w0 + 0x1f) &0x1f;
			de_scal_ch1_dx0 = (((x_off1)<<1)&0x1f);
			de_scal_ch1_dy0 = ((2*y_off1)&0x1f);
			de_scal_ch1_dx1 = ((de_scal_ch1_dx0 & 0x1e) + ((in_w1)<<1) + 0x1f) & 0x1f;

			scal_dev[sel]->tb_off0.bits.x_offset0 = de_scal_ch0_dx0;
			scal_dev[sel]->tb_off0.bits.y_offset0 = de_scal_ch0_dy0;
			scal_dev[sel]->tb_off0.bits.x_offset1 = de_scal_ch0_dx1;
			scal_dev[sel]->tb_off1.bits.x_offset0 = de_scal_ch1_dx0;
			scal_dev[sel]->tb_off1.bits.y_offset0 = de_scal_ch1_dy0;
			scal_dev[sel]->tb_off1.bits.x_offset1 = de_scal_ch1_dx1;
			scal_dev[sel]->trd_tb_off0.bits.x_offset0 = de_scal_ch0_dx0;
			scal_dev[sel]->trd_tb_off0.bits.y_offset0 = (2*y_off0 + 1) & 0x1f;
			scal_dev[sel]->trd_tb_off0.bits.x_offset1 = de_scal_ch0_dx1;
			scal_dev[sel]->trd_tb_off1.bits.x_offset0 = de_scal_ch1_dx0;
			scal_dev[sel]->trd_tb_off1.bits.y_offset0 = (2*y_off1 + 1) & 0x1f;
			scal_dev[sel]->trd_tb_off1.bits.x_offset1 = de_scal_ch1_dx1;

			de_scal_ch0_offset = ((image_w0 + 0x1f) &0xffe0) * ((2*y_off0) & 0xffe0) + (((2*y_off0) & 0x01f)<<5) +
	                              ((x_off0& 0xffe0)<<5);
	        de_scal_ch1_offset = (((image_w1<< 0x01)+0x1f)&0xffe0) * ((2*y_off1) & 0xffe0) +
	                              (((2*y_off1) & 0x01f)<<5) + (((x_off1<<0x01) & 0xffe0)<<5);
	        de_scal_ch2_offset = 0x0;

			de_scal_ch0r_offset = de_scal_ch0_offset + 32;
			de_scal_ch1r_offset = de_scal_ch1_offset + 32;
			de_scal_ch2r_offset = 0x0;

			scal_dev[sel]->buf_addr0.dwval = addr->ch0_addr+ de_scal_ch0_offset;
			scal_dev[sel]->buf_addr1.dwval = addr->ch1_addr+ de_scal_ch1_offset;
			scal_dev[sel]->buf_addr2.dwval = addr->ch2_addr+ de_scal_ch2_offset;

			scal_dev[sel]->trd_buf_addr0.dwval = addr->ch0_addr + de_scal_ch0r_offset;
			scal_dev[sel]->trd_buf_addr1.dwval = addr->ch1_addr + de_scal_ch1r_offset;
			scal_dev[sel]->trd_buf_addr2.dwval = addr->ch2_addr + de_scal_ch2r_offset;
		}
	}


	scal_dev[sel]->input_fmt.bits.byte_seq = type->byte_seq;
	scal_dev[sel]->input_fmt.bits.data_mod = type->mod;
	scal_dev[sel]->input_fmt.bits.data_fmt = type->fmt;
	scal_dev[sel]->input_fmt.bits.data_ps = type->ps;

	scal_dev[sel]->ch0_insize.bits.in_width = in_w0 - 1;
	scal_dev[sel]->ch0_insize.bits.in_height = in_h0 - 1;
	scal_dev[sel]->ch1_insize.bits.in_width = in_w1 - 1;
	scal_dev[sel]->ch1_insize.bits.in_height = in_h1 - 1;


    return 0;
}

__s32 DE_SCAL_EnableINT(__u8 sel,__u32 irqsrc)
{
	scal_dev[sel]->int_en.dwval |= irqsrc;

	return 0;
}

__s32 DE_SCAL_DisableINT(__u8 sel, __u32 irqsrc)
{
	scal_dev[sel]->int_en.dwval &= (~irqsrc);

	return 0;
}

__u32 DE_SCAL_QueryINT(__u8 sel)
{
	return scal_dev[sel]->int_status.dwval;
}

__u32 DE_SCAL_ClearINT(__u8 sel,__u32 irqsrc)
{
	scal_dev[sel]->int_status.dwval |= irqsrc;
	return 0;
}


__s32 DE_SCAL_Set_Writeback_Addr_ex(__u32 sel, __scal_buf_addr_t *addr, __scal_out_size_t *size,
                         __scal_out_type_t *type)
{
    __u8 w_shift, h_shift;
	__u32 image_w0, image_w1;
	__u32 x_off0, y_off0, x_off1, y_off1;
	__u32 in_w0, in_h0, in_w1, in_h1;

	image_w0 = size->fb_width;
	in_w0 = size->width;
	in_h0 = size->height;
	x_off0 = size->x_off;
	y_off0 = size->y_off; 

    {
        if(type->fmt == DE_SCAL_OUTPYUV422 || type->fmt == DE_SCAL_OUTPYUV420)
        {
            w_shift = 1;
        	image_w1 = (image_w0 + 0x1)>>w_shift;
        	in_w1 = (in_w0 + 0x1)>>w_shift;
        	x_off1 = (x_off0)>>w_shift;
        }
        else if(type->fmt == DE_SCAL_OUTPYUV411)
        {
            w_shift = 2;
        	image_w1 = (image_w0 + 0x3)>>w_shift;
        	in_w1 = (in_w0 + 0x3)>>w_shift;
        	x_off1 = (x_off0)>>w_shift;
        }
        else
        {
            w_shift = 0;
        	image_w1 = image_w0;
        	in_w1 = in_w0;
        	x_off1 = x_off0;
        }
        if(type->fmt == DE_SCAL_OUTPYUV420)
        {
            h_shift = 1;
        	in_h1 = (in_h0 + 0x1)>>h_shift;
        	y_off1 = (y_off0)>>h_shift;
        }
        else
        {
            h_shift = 0;
        	in_h1 = in_h0;
        	y_off1 = y_off0;
        }
    }
    in_h0 = (in_h0!=0) ? in_h0 : 1;
	in_h1 = (in_h1!=0) ? in_h1 : 1;
	in_w0 = (in_w0!=0) ? in_w0 : 1;
	in_w1 = (in_w1!=0) ? in_w1 : 1;

	if((type->fmt == DE_SCAL_OUTPRGB888) || (type->fmt == DE_SCAL_OUTPYUV444) || (type->fmt == DE_SCAL_OUTPYUV420) || (type->fmt == DE_SCAL_OUTPYUV422) || (type->fmt == DE_SCAL_OUTPYUV411 ))
	{
	    scal_dev[sel]->wb_linestrd0.dwval = image_w0;
		scal_dev[sel]->wb_linestrd1.dwval = image_w1;
		scal_dev[sel]->wb_linestrd2.dwval = image_w1;

        de_scal_ch0_offset = image_w0 * y_off0 + x_off0;
        de_scal_ch1_offset = image_w1 * y_off1 + x_off1;
        de_scal_ch2_offset = image_w1 * y_off1 + x_off1;

        addr->ch0_addr = addr->ch0_addr+ de_scal_ch0_offset;
        addr->ch1_addr = addr->ch1_addr+ de_scal_ch1_offset;
        addr->ch2_addr = addr->ch2_addr+ de_scal_ch2_offset;

        DE_SCAL_Set_Writeback_Addr(sel,addr);
	}
	else if((type->fmt == DE_SCAL_OUTI0RGB888) || (type->fmt == DE_SCAL_OUTI1RGB888) )
	{
        scal_dev[sel]->wb_linestrd0.dwval = image_w0<<(0x2 - w_shift);
		scal_dev[sel]->wb_linestrd1.dwval = 0;
		scal_dev[sel]->wb_linestrd2.dwval = 0;

        de_scal_ch0_offset = ((image_w0 * y_off0 + x_off0)<<(0x2 - w_shift));
        de_scal_ch1_offset = 0x0;
        de_scal_ch2_offset = 0x0;

        addr->ch0_addr = addr->ch0_addr+ de_scal_ch0_offset;

        DE_SCAL_Set_Writeback_Addr(sel,addr);
	}

    return 0;
}

