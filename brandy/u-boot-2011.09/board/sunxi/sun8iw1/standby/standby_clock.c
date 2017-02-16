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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include "standby_i.h"
#include <asm/arch/ccmu.h>


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
static int standby_set_divd (int pll)
{
  __u32 reg_val;
  
  reg_val = readl (CCM_CPU_L2_AXI_CTRL);
  reg_val &= ~ (0x03 << 0);
  reg_val |=  (0x02 << 0);
  writel (reg_val, CCM_CPU_L2_AXI_CTRL);
  reg_val = readl (CCM_AHB1_APB1_CTRL);
  reg_val &= ~ ( (0x03 << 12) | (0x03 << 8) | (0x03 << 4) );
  reg_val |=   (0x02 << 12);
  if (pll <= 400)
  {
    ;
  }
  else
    if (pll <= 800)
    {
      reg_val |= (1 << 4);
    }
    else
      if (pll <= 1600)
      {
        reg_val |= (2 << 4);
      }
      else
      {
        reg_val |= (3 << 4);
      }
  writel (reg_val, CCM_AHB1_APB1_CTRL);
  
  return 0;
}

__s32 standby_clock_to_24M (void)
{
  __u32 reg_val;
  int   i;
  
  standby_set_divd (24);
  
  reg_val = readl (CCM_CPU_L2_AXI_CTRL);
  reg_val &= ~ (0x03 << 16);
  reg_val |=  (0x01 << 16);
  writel (reg_val, CCM_CPU_L2_AXI_CTRL);
  
  for (i = 0; i < 0x4000; i++);
  
  return 0;
}

__s32 standby_clock_to_pll1 (void)
{
  __u32 pll1;
  __u32 reg_val;
  __u32 factor_n;
  __u32 factor_k, div_m;
  
  reg_val  = readl (CCM_PLL1_CPUX_CTRL);
  factor_n = ( (reg_val >> 8) & 0x1f) + 1;
  factor_k = ( (reg_val >> 4) & 0x3) + 1;
  div_m = ( (reg_val >> 0) & 0x3) + 1;
  
  pll1 = 24 * factor_n * factor_k / div_m;
  
  standby_set_divd (pll1);
  
  reg_val = readl (CCM_CPU_L2_AXI_CTRL);
  reg_val &= ~ (0x03 << 16);
  reg_val |=  (0x02 << 16);
  writel (reg_val, CCM_CPU_L2_AXI_CTRL);
  
  return 0;
}


void standby_clock_plldisable (void)
{
  uint reg_val;
  
  reg_val = readl (CCM_PLL1_CPUX_CTRL);
  reg_val &= ~ (1U << 31);
  writel (reg_val, CCM_PLL1_CPUX_CTRL);
  
  reg_val = readl (CCM_PLL3_VIDEO_CTRL);
  reg_val &= ~ (1U << 31);
  writel (reg_val, CCM_PLL3_VIDEO_CTRL);
  
  reg_val = readl (CCM_PLL6_MOD_CTRL);
  reg_val &= ~ (1U << 31);
  writel (reg_val, CCM_PLL6_MOD_CTRL);
  
  reg_val = readl (CCM_PLL7_VIDEO1_CTRL);
  reg_val &= ~ (1U << 31);
  writel (reg_val, CCM_PLL7_VIDEO1_CTRL);
}

void standby_clock_pllenable (void)
{
  __u32 reg_val;
  
  reg_val = readl (CCM_PLL1_CPUX_CTRL);
  reg_val |= (1U << 31);
  writel (reg_val, CCM_PLL1_CPUX_CTRL);
  do
  {
    reg_val = readl (CCM_PLL1_CPUX_CTRL);
  }
  while (! (reg_val & (0x1 << 28) ) );
  
  
  reg_val = readl (CCM_PLL3_VIDEO_CTRL);
  reg_val |= (1U << 31);
  writel (reg_val, CCM_PLL3_VIDEO_CTRL);
  do
  {
    reg_val = readl (CCM_PLL3_VIDEO_CTRL);
  }
  while (! (reg_val & (0x1 << 28) ) );
  
  reg_val = readl (CCM_PLL6_MOD_CTRL);
  reg_val |= (1U << 31);
  writel (reg_val, CCM_PLL6_MOD_CTRL);
  do
  {
    reg_val = readl (CCM_PLL6_MOD_CTRL);
  }
  while (! (reg_val & (0x1 << 28) ) );
  
  reg_val = readl (CCM_PLL7_VIDEO1_CTRL);
  reg_val |= (1U << 31);
  writel (reg_val, CCM_PLL7_VIDEO1_CTRL);
  do
  {
    reg_val = readl (CCM_PLL7_VIDEO1_CTRL);
  }
  while (! (reg_val & (0x1 << 28) ) );
}


/*
*********************************************************************************************************
*                           standby_tmr_enable_watchdog
*
*Description: enable watch-dog.
*
*Arguments  : none.
*
*Return     : none;
*
*Notes      :
*
*********************************************************************************************************
*/

void standby_tmr_enable_watchdog (void)
{
  /* set watch-dog reset, timeout is 2 seconds */
}


/*
*********************************************************************************************************
*                           standby_tmr_disable_watchdog
*
*Description: disable watch-dog.
*
*Arguments  : none.
*
*Return     : none;
*
*Notes      :
*
*********************************************************************************************************
*/
void standby_tmr_disable_watchdog (void)
{
  /* disable watch-dog reset */
}

void standby_clock_apb1_to_source (int clock)
{
}


void standby_clock_24m_op (int op)
{

  return;
}

