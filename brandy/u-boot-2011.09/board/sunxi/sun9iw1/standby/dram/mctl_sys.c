/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include "dram_i.h"
#include "mctl_reg.h"
#include "mctl_hal.h"

static __u32 ccm_dram_gating_reserved;

void mctl_self_refresh_entry(unsigned int ch_index)
{
	unsigned int reg_val;
	unsigned int ch_id;

  	ccm_dram_gating_reserved = mctl_read_w(CCM_DRAM_GATING);
  	mctl_write_w(CCM_DRAM_GATING, 0);

  	mctl_write_w(SDR_COM_MAER, 0);

	if(ch_index == 1)
		ch_id = 0x1000;
	else
		ch_id = 0x0;
	reg_val = 0x3;
	mctl_write_w(ch_id + SDR_SCTL, reg_val);
	while(  (mctl_read_w(ch_id + SDR_SSTAT)&0x7) != 0x5 ) {};

	reg_val = mctl_read_w(ch_id + SDR_ACIOCR);
	reg_val |= (0x1<<3)|(0x1<<8)|(0x3<<18);
	mctl_write_w(ch_id + SDR_ACIOCR, reg_val);

	reg_val = mctl_read_w(ch_id + SDR_DXCCR);
	reg_val |= (0x1<<2)|(0x1<<3);
	mctl_write_w(ch_id + SDR_DXCCR, reg_val);

	reg_val = mctl_read_w(ch_id + SDR_DSGCR);
	reg_val &= ~(0x1<<28);
	mctl_write_w(ch_id + SDR_DSGCR, reg_val);

}

void mctl_self_refresh_exit(unsigned int ch_index)
{
	unsigned int reg_val;
	unsigned int ch_id;

	if(ch_index == 1)
		ch_id = 0x1000;
	else
		ch_id = 0x0;

	reg_val = mctl_read_w(ch_id + SDR_DSGCR);
	reg_val |= (0x1<<28);
	mctl_write_w(ch_id + SDR_DSGCR, reg_val);

	reg_val = mctl_read_w(ch_id + SDR_DXCCR);
	reg_val &= ~((0x1<<2)|(0x1<<3));
	mctl_write_w(ch_id + SDR_DXCCR, reg_val);

	reg_val = mctl_read_w(ch_id + SDR_ACIOCR);
	reg_val &= ~((0x1<<3)|(0x1<<8)|(0x3<<18));
	mctl_write_w(ch_id + SDR_ACIOCR, reg_val);

	reg_val = 0x4;
	mctl_write_w(ch_id + SDR_SCTL, reg_val);

	while(  (mctl_read_w(ch_id + SDR_SSTAT)&0x7) != 0x3 ) {};

	mctl_write_w(SDR_COM_MAER, 0xFFFFFFFF);

  	mctl_write_w(CCM_DRAM_GATING, ccm_dram_gating_reserved);
}

void mctl_deep_sleep_entry(void)
{
	unsigned int reg_val;

	mctl_self_refresh_entry(0);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_self_refresh_entry(1);
	}


	reg_val = mctl_read_w(R_VDD_SYS_PWROFF_GATE);
	reg_val |= (0x1<<1);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
		reg_val |=0x1;
	mctl_write_w(R_VDD_SYS_PWROFF_GATE, reg_val);

	standby_serial_putc('q');
	standby_serial_putc('\n');



 	reg_val = mctl_read_w(SDR_COM_CCR);
 	reg_val &= ~(0x7<<0);
 	mctl_write_w(SDR_COM_CCR, reg_val);


 	reg_val = mctl_read_w(CCM_AHB1_GATE0_CTRL);
 	reg_val &=~(0x1<<14);
 	mctl_write_w(CCM_AHB1_GATE0_CTRL, reg_val);

 	reg_val = mctl_read_w(CCM_MDFS_CLK_CTRL);
 	reg_val &= ~(0x1U<<31);
 	mctl_write_w(CCM_MDFS_CLK_CTRL, reg_val);

	standby_serial_putc('6');

}

void mctl_deep_sleep_exit(__dram_para_t *para)
{
	unsigned int reg_val;
  	standby_serial_putc('c');
	standby_serial_putc('\n');


 	reg_val = mctl_read_w(CCM_MDFS_CLK_CTRL);
 	reg_val |= (0x1U<<31);
 	mctl_write_w(CCM_MDFS_CLK_CTRL, reg_val);

 	reg_val = mctl_read_w(CCM_AHB1_GATE0_CTRL);
 	reg_val |= (0x1<<14);
 	mctl_write_w(CCM_AHB1_GATE0_CTRL, reg_val);

 	reg_val = mctl_read_w(SDR_COM_CCR);
 	reg_val |= (0x7<<0);
 	mctl_write_w(SDR_COM_CCR, reg_val);

	reg_val = mctl_read_w(R_VDD_SYS_PWROFF_GATE);
	reg_val &= ~(0x1<<1);
	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
		reg_val &= ~(0x1);
	mctl_write_w(R_VDD_SYS_PWROFF_GATE, reg_val);

	standby_serial_putc('1');

	mctl_self_refresh_exit(0);

	standby_serial_putc('2');

	if(mctl_read_w(SDR_COM_CR) & (0x1<<19))
	{
		mctl_self_refresh_exit(1);
	}

  	standby_serial_putc('z');
  	standby_serial_putc('f');
	standby_serial_putc('\n');

}

