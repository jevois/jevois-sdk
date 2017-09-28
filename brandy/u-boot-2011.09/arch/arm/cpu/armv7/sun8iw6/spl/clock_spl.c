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
#include "asm/armv7.h"
#include "asm/arch/cpu.h"
#include "asm/arch/ccmu.h"
#include "asm/arch/timer.h"


static void set_pll_bias(void)
{
    __u32 reg_val = 0 ;
    __u32 i = 0 ;
    __u32 reg_addr = CCMU_PLL_C0CPUX_BIAS_REG;
    for(i = 0;i<12;i++)
    {
        reg_val = readl(reg_addr + (i<<2));
        reg_val &= ~(0x1f<<16);
        reg_val |= 0x04<<16;
        writel(reg_val,reg_addr + (i<<2));
    }
}
/*******************************************************************************
*函数名称: set_pll
*函数原型：void set_pll( void )
*函数功能: 调整CPU频率
*入口参数: void
*返 回 值: void
*备    注:
*******************************************************************************/
void set_pll( void )
{
	__u32 reg_val;

	writel((1<<16 | 1<<0), CCMU_CPUX_AXI_CFG_REG);
	reg_val = 0x82001100;
	writel(reg_val, CCMU_PLL_C0CPUX_CTRL_REG);
#ifndef CONFIG_SUNXI_FPGA
	do
	{
		reg_val = readl(CCMU_PLL_STB_STATUS_REG);
	}
	while(!(reg_val & 0x1));
#endif

    reg_val = 0x82001100;
    writel(reg_val, CCMU_PLL_C1CPUX_CTRL_REG);
#ifndef CONFIG_SUNXI_FPGA
    do
    {
        reg_val = readl(CCMU_PLL_STB_STATUS_REG);
    }
    while(!(reg_val & 0x2));
#endif
    writel(0x42800, CCMU_PLL_HSIC_CTRL_REG); 
    writel(readl(CCMU_PLL_HSIC_CTRL_REG) | (1U << 31), CCMU_PLL_HSIC_CTRL_REG);
#ifndef CONFIG_SUNXI_FPGA
        do
        {
            reg_val = readl(CCMU_PLL_STB_STATUS_REG);
        }
        while(!(reg_val & (1<<8)));
#endif

    CP15DMB;
    CP15ISB;
    reg_val = readl(CCMU_CCI400_CFG_REG);
    if(!(reg_val & (0x3<<24)))
    {
        writel(0x0, CCMU_CCI400_CFG_REG);
        __usdelay(50);
    }
    writel((2<<24 | 0<<0), CCMU_CCI400_CFG_REG);
    __usdelay(100);
    CP15DMB;
    CP15ISB;
    writel((0x01 << 12) | (readl(CCMU_AHB1_APB1_CFG_REG)&(~(0x3<<12))), CCMU_AHB1_APB1_CFG_REG);
    writel( (0x32<<8) | (1U << 31), CCMU_PLL_PERIPH_CTRL_REG);
    __usdelay(10);
#ifndef CONFIG_SUNXI_FPGA
        do
        {
            reg_val = readl(CCMU_PLL_STB_STATUS_REG);
        }
        while(!(reg_val & (1<<6)));
#endif
    writel((0x02 << 12) | (1<<8) | (2<<6) | (1<<4), CCMU_AHB1_APB1_CFG_REG);
	reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
	reg_val &= ~(3 << 0);
	reg_val |=  (1 << 0);      
	reg_val |=  (1 << 12);     
	writel(reg_val, CCMU_CPUX_AXI_CFG_REG);
    reg_val = readl(CCMU_CPUX_AXI_CFG_REG);
    reg_val &= ~(3<<16);
    reg_val |= 1<<16;         
    reg_val |= 1<<28;         
    writel(reg_val, CCMU_CPUX_AXI_CFG_REG);
    __usdelay(1000);
	writel(readl(CCMU_BUS_SOFT_RST_REG0)  | (1 << 6), CCMU_BUS_SOFT_RST_REG0);
	__usdelay(20);
	writel(readl(CCMU_BUS_CLK_GATING_REG0) | (1 << 6), CCMU_BUS_CLK_GATING_REG0);
	writel(7, (SUNXI_DMA_BASE+0x20));
	writel(0x80000000, CCMU_MBUS_RST_REG);
	writel(0x81000002, CCMU_MBUS_CLK_REG);

    writel(1, 0x01720000);

	return ;
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
void reset_pll( void )
{
    writel(0x00000000, CCMU_CPUX_AXI_CFG_REG);
	writel(0x02001100, CCMU_PLL_C0CPUX_CTRL_REG);

	return ;
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
void set_gpio_gate(void)
{

}

