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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/cpu.h>

struct core_pll_freq_tbl {
    int FactorN;
    int FactorK;
    int FactorM;
    int pading;
};

static struct core_pll_freq_tbl pll1_table[] =
{ 
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{4,	    1,	1,	0},   
	{4,		1,	1,	0},   
	{4,		1,	1,	0},   
	{5,	    1,	1,	0},   
	{5,		1,	1,	0},   
	{5,		1,	1,	0},   
	{5,		1,	1,	0},   
	{6,	    1,	1,	0},   
	{6,		1,	1,	0},   
	{6,		1,	1,	0},   
	{6,		1,	1,	0},   
	{7,		1,	1,	0},   
	{7,	    1,	1,	0},   
	{7,	    1,	1,	0},   
	{7,	    1,	1,	0},   
	{8,		1,	1,	0},   
	{8,		1,	1,	0},   
	{8,		1,	1,	0},   
	{8,		1,	1,	0},   
	{9,		1,	1,	0},   
	{9,		1,	1,	0},   
	{9, 	1,	1,	0},   
	{9, 	1,	1,	0},   
	{10,	1,	1,	0},   
	{10,	1,	1,	0},   
	{10,	1,	1,	0},   
	{10,	1,	1,	0},   
	{11,	1,	1,	0},   
	{11,	1,	1,	0},   
	{11,	1,	1,	0},   
	{11,	1,	1,	0},   
	{12,	1,	1,	0},   
	{12,	1,	1,	0},   
	{12,	1,	1,	0},   
	{12,	1,	1,	0},   
	{13,	1,	1,	0},   
	{13,	1,	1,	0},   
	{13,	1,	1,	0},   
	{13,	1,	1,	0},   
	{14,	1,	1,	0},   
	{14,	1,	1,	0},   
	{14,	1,	1,	0},   
	{14,	1,	1,	0},   
	{15,    1,	1,	0},   
	{15,	1,	1,	0},   
	{15,	1,	1,	0},   
	{15,	1,	1,	0},   
	{16,	1,	1,	0},   
	{16,	1,	1,	0},   
	{16,	1,	1,	0},   
	{16,	1,	1,	0},   
	{17,	1,	1,	0},   
	{17,	1,	1,	0},   
	{17,	1,	1,	0},   
	{17,	1,	1,	0},   
	{18,	1,	1,	0},   
	{18,	1,	1,	0},   
	{18,	1,	1,	0},   
	{18,	1,	1,	0},   
	{19,	1,	1,	0},   
	{19,	1,	1,	0},   
	{19,	1,	1,	0},   
	{19,	1,	1,	0},   
	{20,	1,	1,	0},   
	{20,	1,	1,	0},   
	{20,	1,	1,	0},   
	{20,	1,	1,	0},   
	{21,	1,	1,	0},   
	{21,	1,	1,	0},   
	{21,	1,	1,	0},   
	{21,	1,	1,	0},   
	{22,	1,	1,	0},   
	{22,	1,	1,	0},   
	{22,	1,	1,	0},   
	{22,	1,	1,	0},   
	{23,	1,	1,	0},   
	{23,	1,	1,	0},   
	{23,	1,	1,	0},   
	{23,	1,	1,	0},   
	{24,	1,	1,	0},   
	{24,	1,	1,	0},   
	{24,	1,	1,	0},   
	{24,	1,	1,	0},   
	{25,	1,	1,	0},   
	{25,	1,	1,	0},   
	{25,	1,	1,	0},   
	{25,	1,	1,	0},   
	{26,	1,	1,	0},   
	{26,	1,	1,	0},   
	{26,	1,	1,	0},   
	{26,	1,	1,	0},   
	{27,	1,	1,	0},   
	{27,	1,	1,	0},   
	{27,	1,	1,	0},   
	{27,	1,	1,	0},   
	{28,	1,	1,	0},   
	{28,	1,	1,	0},   
	{28,	1,	1,	0},   
	{28,	1,	1,	0},   
	{29,	1,	1,	0},   
	{29,	1,	1,	0},   
	{29,	1,	1,	0},   
	{29,	1,	1,	0},   
	{30,	1,	1,	0},   
	{30,	1,	1,	0},   
	{30,	1,	1,	0},   
	{30,	1,	1,	0},   
	{31,	1,	1,	0},   
	{31,	1,	1,	0},   
	{31,	1,	1,	0},   
	{31,	1,	1,	0},   
	{21,	2,	1,	0},   
	{21,	2,	1,	0},   
	{21,	2,	1,	0},   
	{21,	2,	1,	0},   
	{21,	2,	1,	0},   
	{21,	2,	1,	0},   
	{22,	2,	1,	0},   
	{22,	2,	1,	0},   
	{22,	2,	1,	0},   
	{22,	2,	1,	0},   
	{22,	2,	1,	0},   
	{22,	2,	1,	0},   
	{23,	2,	1,	0},   
	{23,	2,	1,	0},   
	{23,	2,	1,	0},   
	{23,	2,	1,	0},   
	{23,	2,	1,	0},   
	{23,	2,	1,	0},   
	{24,	2,	1,	0},   
	{24,	2,	1,	0},   
	{24,	2,	1,	0},   
	{24,	2,	1,	0},   
	{24,	2,	1,	0},   
	{24,	2,	1,	0},   
	{25,	2,	1,	0},   
	{25,	2,	1,	0},   
	{25,	2,	1,	0},   
	{25,	2,	1,	0},   
	{25,	2,	1,	0},   
	{25,	2,	1,	0},   
	{26,	2,	1,	0},   
	{26,	2,	1,	0},   
	{26,	2,	1,	0},   
	{26,	2,	1,	0},   
	{26,	2,	1,	0},   
	{26,	2,	1,	0},   
	{27,	2,	1,	0},   
	{27,	2,	1,	0},   
	{27,	2,	1,	0},   
	{27,	2,	1,	0},   
	{27,	2,	1,	0},   
	{27,	2,	1,	0},   
	{28,	2,	1,	0},   
	{28,	2,	1,	0},   
	{28,	2,	1,	0},   
	{28,	2,	1,	0},   
	{28,	2,	1,	0},   
	{28,	2,	1,	0},   
	{29,	2,	1,	0},   
	{29,	2,	1,	0},   
	{29,	2,	1,	0},   
	{29,	2,	1,	0},   
	{29,	2,	1,	0},   
	{29,	2,	1,	0},   
	{30,	2,	1,	0},   
	{30,	2,	1,	0},   
	{30,	2,	1,	0},   
	{30,	2,	1,	0},   
	{30,	2,	1,	0},   
	{30,	2,	1,	0},   
	{31,	2,	1,	0},   
	{31,	2,	1,	0},   
	{31,	2,	1,	0},   
	{31,	2,	1,	0},   
	{31,	2,	1,	0},   
	{31,	2,	1,	0},   
	{31,	2,	1,	0},   
	{31,	2,	1,	0},   
	{24,	3,	1,	0},   
	{24,	3,	1,	0},   
	{24,	3,	1,	0},   
	{24,	3,	1,	0},   
	{24,	3,	1,	0},   
	{24,	3,	1,	0},   
	{24,	3,	1,	0},   
	{24,	3,	1,	0},   
	{25,	3,	1,	0},   
	{25,	3,	1,	0},   
	{25,	3,	1,	0},   
	{25,	3,	1,	0},   
	{25,	3,	1,	0},   
	{25,	3,	1,	0},   
	{25,	3,	1,	0},   
	{25,	3,	1,	0},   
	{26,	3,	1,	0},   
	{26,	3,	1,	0},   
	{26,	3,	1,	0},   
	{26,	3,	1,	0},   
	{26,	3,	1,	0},   
	{26,	3,	1,	0},   
	{26,	3,	1,	0},   
	{26,	3,	1,	0},   
	{27,	3,	1,	0},   
	{27,	3,	1,	0},   
	{27,	3,	1,	0},   
	{27,	3,	1,	0},   
	{27,	3,	1,	0},   
	{27,	3,	1,	0},   
	{27,	3,	1,	0},   
	{27,	3,	1,	0},   
	{28,	3,	1,	0},   
	{28,	3,	1,	0},   
	{28,	3,	1,	0},   
	{28,	3,	1,	0},   
	{28,	3,	1,	0},   
	{28,	3,	1,	0},   
	{28,	3,	1,	0},   
	{28,	3,	1,	0},   
	{29,	3,	1,	0},   
	{29,	3,	1,	0},   
	{29,	3,	1,	0},   
	{29,	3,	1,	0},   
	{29,	3,	1,	0},   
	{29,	3,	1,	0},   
	{29,	3,	1,	0},   
	{29,	3,	1,	0},   
	{30,	3,	1,	0},   
	{30,	3,	1,	0},   
	{30,	3,	1,	0},   
	{30,	3,	1,	0},   
	{30,	3,	1,	0},   
	{30,	3,	1,	0},   
	{30,	3,	1,	0},   
	{30,	3,	1,	0},   
};
static int clk_get_pll_para(struct core_pll_freq_tbl *factor, int rate);
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_corepll(void)
{
	unsigned int reg_val;
	int 	div_m;
	int 	factor_k, factor_n;
	int 	clock;

	reg_val  = readl(CCM_PLL1_CPUX_CTRL);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x3) + 1;
	div_m    = ((reg_val >> 0) & 0x3) + 1;

	clock = 24 * factor_n * factor_k/div_m;

	return clock;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_axi(void)
{
	int clock;
	unsigned int reg_val;
	int clock_src, factor;

	reg_val   = readl(CCM_CPU_L2_AXI_CTRL);
	clock_src = (reg_val >> 16) & 0x03;
	factor    = (reg_val >> 0) & 0x07;

	if(factor >= 3)
	{
		factor = 4;
	}
	else
	{
		factor ++;
	}

	switch(clock_src)
	{
		case 0:
			clock = 32000;
			break;
		case 1:
			clock = 24;
			break;
		case 2:
			clock =  sunxi_clock_get_corepll();
			break;
		default:
			return 0;
	}

	return clock/factor;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_ahb(void)
{
	unsigned int reg_val;
	int factor;
	int clock;

	reg_val = readl(CCM_AHB1_APB1_CTRL);
	factor  = (reg_val >> 4) & 0x03;
	clock   = sunxi_clock_get_axi()>>factor;

	return clock;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_apb1(void)
{
	unsigned int reg_val;
	int          clock, factor;

	reg_val = readl(CCM_AHB1_APB1_CTRL);
	factor  = (reg_val >> 8) & 0x03;
	clock   = sunxi_clock_get_ahb();

	if(factor)
	{
		clock >>= factor;
	}
	else
	{
		clock >>= 1;
	}

	return clock;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_apb2(void)
{
	return 24;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：只限于调整COREPLL，固定分频比，4:2:1
*
*
************************************************************************************************************
*/
static int clk_get_pll_para(struct core_pll_freq_tbl *factor, int pll_clk)
{
    int                       index;
	struct core_pll_freq_tbl *target_factor;

    index = pll_clk / 6;
	target_factor = &pll1_table[index];

    factor->FactorN = target_factor->FactorN;
    factor->FactorK = target_factor->FactorK;
    factor->FactorM = target_factor->FactorM;

    return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
static int clk_set_divd(int pll)
{
	unsigned int reg_val;

	reg_val = readl(CCM_CPU_L2_AXI_CTRL);
	reg_val &= ~(0x03 << 0);
	reg_val |=  (0x02 << 0);
	writel(reg_val, CCM_CPU_L2_AXI_CTRL);
	reg_val = readl(CCM_AHB1_APB1_CTRL);;
	reg_val &= ~((0x03 << 12) | (0x03 << 8) | (0x03 << 4));
	reg_val |=   (0x02 << 12);
	if(pll <= 400)
	{
		;
	}
	else if(pll <= 800)
	{
		reg_val |= (1 << 4);
	}
	else if(pll <= 1600)
	{
		reg_val |= (2 << 4);
	}
	else
	{
		reg_val |= (3 << 4);
	}
	writel(reg_val, CCM_AHB1_APB1_CTRL);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*
*
*    返回值  ：
*
*    说明    ：只限于调整COREPLL，固定分频比，4:2:1
*
*
************************************************************************************************************
*/
int sunxi_clock_set_corepll(int frequency, int core_vol)
{
    unsigned int reg_val;
    unsigned int i;
    struct core_pll_freq_tbl  pll_factor;
    if(!frequency)
    {
        frequency = 408;
    }
    else if(frequency > 1500)
    {
    	frequency = 1500;
    }
    else if(frequency < 24)
    {
		frequency = 24;
    }
    reg_val = readl(CCM_CPU_L2_AXI_CTRL);
    reg_val &= ~(0x03 << 16);
    reg_val |=  (0x01 << 16);
    writel(reg_val, CCM_CPU_L2_AXI_CTRL);
    for(i=0; i<0x400; i++);
	clk_get_pll_para(&pll_factor, frequency);
    reg_val = readl(CCM_PLL1_CPUX_CTRL);
    reg_val &= ~((0x1f << 8) | (0x03 << 4) | (0x03 << 0));
	reg_val |=  ((pll_factor.FactorN)<<8) | ((pll_factor.FactorK)<<4) | ((pll_factor.FactorM) << 0);
    writel(reg_val, CCM_PLL1_CPUX_CTRL);
	do
	{
		reg_val = readl(CCM_PLL1_CPUX_CTRL);
	}
	while(!(reg_val & (0x1 << 28)));
    clk_set_divd(frequency);
    reg_val = readl(CCM_CPU_L2_AXI_CTRL);
    reg_val &= ~(0x03 << 16);
    reg_val |=  (0x02 << 16);
    writel(reg_val, CCM_CPU_L2_AXI_CTRL);

    return  0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_set_pll6(void)
{
    int i;
	unsigned int reg_val;

    /* set voltage and ldo for pll */
    reg_val = readl((SUNXI_R_PRCM_REGS_BASE+0x44));
    reg_val &= ~(0xff << 24);
    reg_val |= 0xa7 << 24;
    writel(reg_val, (SUNXI_R_PRCM_REGS_BASE+0x44));
    reg_val = readl((SUNXI_R_PRCM_REGS_BASE+0x44));
    reg_val &= ~(0x1 << 15);
    reg_val &= ~(0x7 << 16);
    reg_val |= 0x7 << 16;
    writel(reg_val, (SUNXI_R_PRCM_REGS_BASE+0x44));
    /* delaly some time*/
    for(i=0; i<100000; i++);

    /* set pll6 frequency to 600Mhz, and enable it */
	writel(0x80041811, CCM_PLL6_MOD_CTRL);
	do
	{
		reg_val = readl(CCM_PLL6_MOD_CTRL);
	}
	while(!(reg_val & (0x1 << 28)));

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_set_mbus(void)
{
	int factor_n, factor_k, pll6;
	unsigned int reg_val;

    /* set voltage and ldo for pll */
	reg_val = readl(CCM_PLL6_MOD_CTRL);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x03) + 1;
	pll6 = 24 * factor_n * factor_k/2;

    if(pll6 > 300 * 4) {
        factor_n = 5;
    } else if(pll6 > 300*3){
        factor_n = 4;
    } else if(pll6 > 300*2){
        factor_n = 3;
    } else if(pll6 > 300*1){
        factor_n = 2;
    } else {
        factor_n = 1;
    }

    /* config mbus0 */
    writel((0x81000000|(factor_n-1)), CCM_MBUS_SCLK_CTRL0);
    writel((0x81000000|(factor_n-1)), CCM_MBUS_SCLK_CTRL1);

    return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    函数名称：
*
*    参数列表：
*
*    返回值  ：
*
*    说明    ：
*
*
************************************************************************************************************
*/
int sunxi_clock_get_pll6(void)
{
	unsigned int reg_val;
	int factor_n, factor_k, pll6;

	reg_val = readl(CCM_PLL6_MOD_CTRL);
	factor_n = ((reg_val >> 8) & 0x1f) + 1;
	factor_k = ((reg_val >> 4) & 0x03) + 1;
	pll6 = 24 * factor_n * factor_k/2;

	return pll6;
}
