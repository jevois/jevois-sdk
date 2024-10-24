/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include "common.h"
#include "asm/io.h"
#include "asm/arch/ccmu.h"
#include "asm/arch/ss.h"

DECLARE_GLOBAL_DATA_PTR;

static int ss_base_mode = 0;
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static u32 __aw_endian4(u32 data)
{
	u32 d1, d2, d3, d4;
	d1= (data&0xff)<<24;
	d2= (data&0xff00)<<8;
	d3= (data&0xff0000)>>8;
	d4= (data&0xff000000)>>24;

	return (d1|d2|d3|d4);
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static u32 __sha256_padding(u32 data_size, u8* text)
{
	u32 i;
	u32 k, q;
	u32 size;
	u32 padding_buf[16];
    u8 *ptext;

	k = data_size/64;
	q = data_size%64;

	ptext = (u8*)padding_buf;
	if(q==0){
		for(i=0; i<16; i++){
			padding_buf[i] = 0x0;
		}
		padding_buf[0] = 0x00000080;

		padding_buf[14] = data_size>>29;
		padding_buf[15] = data_size<<3;
		padding_buf[14] = __aw_endian4(padding_buf[14]);
		padding_buf[15] = __aw_endian4(padding_buf[15]);

		for(i=0; i<64; i++){
			text[k*64 + i] = ptext[i];
		}
		size = (k + 1)*64;
	}else if(q<56){
		for(i=0; i<16; i++){
			padding_buf[i] = 0x0;
		}
		for(i=0; i<q; i++){
			ptext[i] = text[k*64 + i];
		}
		ptext[q] = 0x80;


		padding_buf[14] = data_size>>29;
		padding_buf[15] = data_size<<3;
		padding_buf[14] = __aw_endian4(padding_buf[14]);
		padding_buf[15] = __aw_endian4(padding_buf[15]);

		for(i=0; i<64; i++){
			text[k*64 + i] = ptext[i];
		}
		size = (k + 1)*64;
	}else{
		for(i=0; i<16; i++){
			padding_buf[i] = 0x0;
		}
		for(i=0; i<q; i++){
			ptext[i] = text[k*64 + i];
		}
		ptext[q] = 0x80;
		for(i=0; i<64; i++){
			text[k*64 + i] = ptext[i];
		}

		for(i=0; i<16; i++){
			padding_buf[i] = 0x0;
		}
		padding_buf[14] = data_size>>29;
		padding_buf[15] = data_size<<3;
		padding_buf[14] = __aw_endian4(padding_buf[14]);
		padding_buf[15] = __aw_endian4(padding_buf[15]);

		for(i=0; i<64; i++){
			text[(k + 1)*64 + i] = ptext[i];
		}
		size = (k + 2)*64;
	}

	return size;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static void __ss_encry_decry_end(uint task_id)
{
	uint int_en;

	int_en = readl(SS_ICR) & 0xf;
	int_en = int_en&(0x01<<task_id);
	if(int_en!=0)
	{

	   while((readl(SS_ISR)&(0x01<<task_id))==0) {};
	}
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
static void __rsa_padding(u8 *dst_buf, u8 *src_buf, u32 data_len, u32 group_len)
{
	int i = 0;

	memset(dst_buf, 0, group_len);
	for(i = group_len - data_len; i < group_len; i++)
	{
		dst_buf[i] = src_buf[group_len - 1 - i];
	}
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void sunxi_ss_open(void)
{
	u32  reg_val;

	reg_val = readl(CCM_SS_SCLK_CTRL);
	reg_val &= ~(0xf<<24);
	reg_val |= 0x1<<24;
	reg_val &= ~(0x3<<16);
	reg_val |= 0x0<<16;		
	reg_val &= ~(0xf);
	reg_val |= (4 -1);		
	reg_val |= 0x1U<<31;
	writel(reg_val,CCM_SS_SCLK_CTRL);
	reg_val = readl(CCM_AHB1_GATE0_CTRL);
	reg_val |= 0x1<<5;	
	writel(reg_val,CCM_AHB1_GATE0_CTRL);
	reg_val = readl(CCM_AHB1_RST_REG0);
	reg_val |= 0x1<<5;	
	writel(reg_val,CCM_AHB1_RST_REG0);

	if(gd->securemode == SUNXI_NORMAL_MODE)
	{
		ss_base_mode = 1;
	}
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
void sunxi_ss_close(void)
{
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int  sunxi_sha_calc(u8 *dst_addr, u32 dst_len,
					u8 *src_addr, u32 src_len)
{
	u32 reg_val = 0;
	u32 total_len = 0;
	u32 md_size = 32;
	s32 i = 0;
	u8  sign_buff[32 + 32], *p_sign;
	task_queue task0;

	memset(sign_buff, 0, sizeof(sign_buff));
	memset((u8 *)&task0 , 0 ,sizeof(task_queue));
	p_sign =  (u8 *)(((u32)sign_buff + 31)&(~31));

	total_len = __sha256_padding(src_len, (u8 *)src_addr)/4;	//计算明文长度

    task0.task_id = 0;
	task0.common_ctl = (19)|(1U << 31);
	task0.symmetric_ctl = 0;
	task0.asymmetric_ctl = 0;
	task0.key_descriptor = 0;
	task0.iv_descriptor = 0;
	task0.ctr_descriptor = 0;
	task0.data_len = total_len;

	task0.source[0].addr = (uint)src_addr;
	task0.source[0].length = total_len;
	for(i=1;i<8;i++)
		task0.source[i].length = 0;

	task0.destination[0].addr = (uint)p_sign;
	task0.destination[0].length = 32/4;
	for(i=1;i<8;i++)
		 task0.destination[i].length = 0;
	task0.next_descriptor = 0;

	flush_cache((uint)&task0, sizeof(task0));
	flush_cache((uint)sign_buff, 64);
	flush_cache((uint)src_addr, total_len * 4);

	writel((uint)&task0, SS_TDQ);
	writel(0x1<<(task0.task_id), SS_ICR);
	writel(0x1, SS_TLR);
	__ss_encry_decry_end(task0.task_id);
	for(i=0; i< md_size; i++)
	{
	    dst_addr[i] = p_sign[i];   //从目的地址读生成的消息摘要
	}
	reg_val = readl(SS_ISR);
	if((reg_val&(0x01<<task0.task_id))==(0x01<<task0.task_id))
	{
	   reg_val &= ~(0x0f);
	   reg_val |= (0x01<<task0.task_id);
	}
	writel(reg_val, SS_ISR);
	writel(readl(SS_TLR) & (~0x1), SS_TLR);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
s32 sunxi_rsa_calc(u8 * n_addr,   u32 n_len,
				   u8 * e_addr,   u32 e_len,
				   u8 * dst_addr, u32 dst_len,
				   u8 * src_addr, u32 src_len)
{
#define	TEMP_BUFF_LEN	((2048>>3) + 32)
	uint   i;
	task_queue task0;
	u32 reg_val = 0;
	u8	temp_n_addr[TEMP_BUFF_LEN],   *p_n;
	u8	temp_e_addr[TEMP_BUFF_LEN],   *p_e;
	u8	temp_src_addr[TEMP_BUFF_LEN], *p_src;
	u8	temp_dst_addr[TEMP_BUFF_LEN], *p_dst;
	u32 mod_bit_size = 2048;

	u32 mod_size_len_inbytes = mod_bit_size/8;

	p_n = (u8 *)(((u32)temp_n_addr + 31)&(~31));
	p_e = (u8 *)(((u32)temp_e_addr + 31)&(~31));
	p_src = (u8 *)(((u32)temp_src_addr + 31)&(~31));
	p_dst = (u8 *)(((u32)temp_dst_addr + 31)&(~31));

	__rsa_padding(p_src, src_addr, src_len, mod_size_len_inbytes);
	__rsa_padding(p_n, n_addr, n_len, mod_size_len_inbytes);
	memset(p_e, 0, mod_size_len_inbytes);
	memcpy(p_e, e_addr, e_len);

	task0.task_id = 0;
	task0.common_ctl = (32 | (1U<<31));     
	task0.symmetric_ctl = 0;
	task0.asymmetric_ctl = (2<<28);
	task0.key_descriptor = (uint)p_e;
	task0.iv_descriptor = (uint)p_n;
	task0.ctr_descriptor = 0;
	task0.data_len = mod_size_len_inbytes/4;    
	task0.source[0].addr= (uint)p_src;
	task0.source[0].length = mod_size_len_inbytes/4;
	for(i=1;i<8;i++)
		task0.source[i].length = 0;
	task0.destination[0].addr= (uint)p_dst;
	task0.destination[0].length = mod_size_len_inbytes/4;
	for(i=1;i<8;i++)
		task0.destination[i].length = 0;
	task0.next_descriptor = 0;

	flush_cache((uint)&task0, sizeof(task0));
	flush_cache((uint)temp_n_addr, TEMP_BUFF_LEN);
	flush_cache((uint)temp_e_addr, TEMP_BUFF_LEN);
	flush_cache((uint)temp_src_addr, TEMP_BUFF_LEN);
	flush_cache((uint)temp_dst_addr, TEMP_BUFF_LEN);

	writel((uint)&task0, SS_TDQ);
	writel(0x1<<(task0.task_id), SS_ICR);
	writel(0x1, SS_TLR);
	__ss_encry_decry_end(task0.task_id);

	__rsa_padding(dst_addr, p_dst, mod_bit_size/64, mod_bit_size/64);
	reg_val = readl(SS_ISR);
	if((reg_val&(0x01<<task0.task_id))==(0x01<<task0.task_id))
	{
	   reg_val &= ~(0x0f);
	   reg_val |= (0x01<<task0.task_id);
	}
	writel(reg_val, SS_ISR);
	writel(readl(SS_TLR) & (~0x1), SS_TLR);

	return 0;
}
