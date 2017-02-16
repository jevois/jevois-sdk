/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * CPL <cplanxy@allwinnertech.com>
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
#include <common.h>
#include "mctl_reg.h"
#include "mctl_sys.h"

static unsigned int ccm_dram_gating_reserved;

int dram_power_save_process (void)
{

  mctl_self_refresh_entry();
  







  return 0;
}

int dram_power_up_process (void)
{


  




  mctl_self_refresh_exit();
  
  return 0;
}

void mctl_self_refresh_entry (void)
{
  unsigned int reg_val;

  ccm_dram_gating_reserved = mctl_read_w (CCM_DRAM_GATE_REG);
  mctl_write_w (CCM_DRAM_GATE_REG, 0);
  
  mctl_write_w (MC_MAER, 0);
  
  reg_val = mctl_read_w (MX_PWRCTL);
  reg_val |= 0x1 << 0;
  mctl_write_w (MX_PWRCTL, reg_val);
  
  while ( ( (mctl_read_w (MX_STATR) & 0x7) != 0x3) ) {}
  reg_val = mctl_read_w (MP_PGCR (0) );
  reg_val &= ~ (0x3 << 26);
  mctl_write_w (MP_PGCR (0), reg_val);
  reg_val = mctl_read_w (MP_ACIOCR);
  reg_val |= (0x1 << 3) | (0x1 << 8) | (0x1 << 18);
  mctl_write_w (MP_ACIOCR, reg_val);
  
  reg_val = mctl_read_w (MP_DXCCR);
  reg_val |= (0x1 << 3) | (0x1 << 4);
  mctl_write_w (MP_DXCCR, reg_val);
  
  reg_val = mctl_read_w (MP_DSGCR);
  reg_val &= ~ (0x1 << 28);
  mctl_write_w (MP_DSGCR, reg_val);
  
  reg_val = mctl_read_w (VDD_SYS_PWROFF_GATING);
  reg_val |= (0x1 << 0);
  mctl_write_w (VDD_SYS_PWROFF_GATING, reg_val);
}

void mctl_self_refresh_exit (void)
{
  unsigned int reg_val;
  
  reg_val = mctl_read_w (MP_PGCR (0) );
  reg_val &= ~ (0x3 << 26);
  reg_val |= 0x2 << 26;
  mctl_write_w (MP_PGCR (0), reg_val);
  
  reg_val = mctl_read_w (MP_DSGCR);
  reg_val |= (0x1 << 28);
  mctl_write_w (MP_DSGCR, reg_val);
  
  reg_val = mctl_read_w (MP_DXCCR);
  reg_val &= ~ ( (0x1 << 3) | (0x1 << 4) );
  mctl_write_w (MP_DXCCR, reg_val);
  
  reg_val = mctl_read_w (MP_ACIOCR);
  reg_val &= ~ ( (0x1 << 3) | (0x1 << 8) | (0x1 << 18) );
  mctl_write_w (MP_ACIOCR, reg_val);
  
  reg_val = mctl_read_w (VDD_SYS_PWROFF_GATING);
  reg_val &= ~ (0x1 << 0);
  mctl_write_w (VDD_SYS_PWROFF_GATING, reg_val);
  reg_val = mctl_read_w (MX_PWRCTL);
  reg_val &= ~ (0x1 << 0);
  mctl_write_w (MX_PWRCTL, reg_val);
  
  while ( ( (mctl_read_w (MX_STATR) & 0x7) != 0x1) ) {}
  
  
  mctl_write_w (MC_MAER, 0xFFFFFFFF);
  
  mctl_write_w (CCM_DRAM_GATE_REG, ccm_dram_gating_reserved);
}
#if 0

int soft_switch_freq (__dram_para_t * dram_para)
{
  mctl_deep_sleep_entry (dram_para);
  
  return 0;
}

void mctl_deep_sleep_entry (__dram_para_t * para)
{
  unsigned int reg_val;
  unsigned int time = 0xffffff;
  unsigned int factor;
  unsigned int i, j;
  
  mctl_self_refresh_entry();
  
  reg_val = mctl_read_w (MC_CCR);
  reg_val &= ~ (0x1 << 19);
  mctl_write_w (MC_CCR, reg_val);
  
  reg_val = mctl_read_w (CCM_AHB1_GATE0_REG);
  reg_val &= ~ (0x1 << 14);
  mctl_write_w (CCM_AHB1_GATE0_REG, reg_val);
  
  reg_val = mctl_read_w (CCM_PLL_DDR_CTRL);
  reg_val &= ~ (0x1U << 31);
  mctl_write_w (CCM_PLL_DDR_CTRL, reg_val);
  
  reg_val = mctl_read_w (CCM_PLL_DDR_CTRL);
  reg_val |= (0x1U << 20);
  mctl_write_w (CCM_PLL_DDR_CTRL, reg_val);
  
  while (mctl_read_w (CCM_PLL_DDR_CTRL) & (0x1 << 20) ) {}
  
  reg_val = mctl_read_w (CCM_DRAM_CFG_REG);
  reg_val &= ~ (0x1U << 31);
  mctl_write_w (CCM_DRAM_CFG_REG, reg_val);
  
  reg_val = mctl_read_w (CCM_DRAM_CFG_REG);
  reg_val |= (0x1u << 16);
  mctl_write_w (CCM_DRAM_CFG_REG, reg_val);
  
  while (mctl_read_w (CCM_DRAM_CFG_REG) & (0x1 << 16) ) {}
  
  if (para->dram_clk >= 336)
  {
    reg_val = ( (para->dram_clk / 24 - 1) << 8);
    reg_val |= 0 << 4;             
    reg_val |= (2 - 1) << 0;         
    reg_val |= (0x1u << 31);
  }
  else           
  {
    reg_val = ( ( (para->dram_clk << 3) / 24 - 1) << 8);
    reg_val |= 0 << 4;             
    reg_val |= (2 - 1) << 0;         
    reg_val |= (0x1u << 31);
  }
  mctl_write_w (CCM_PLL_DDR_CTRL, reg_val);
  
  reg_val = mctl_read_w (CCM_PLL_DDR_CTRL);
  reg_val |= 0x1 << 20;
  mctl_write_w (CCM_PLL_DDR_CTRL, reg_val);
  
  mctl_delay (0x1000);
  
  while (mctl_read_w (CCM_PLL_DDR_CTRL) & (0x1 << 20) ) {}
  
  while (! (mctl_read_w (CCM_PLL_DDR_CTRL) & (0x1 << 28) ) ) {}
  
  if (para->dram_clk < 336)
  {
    reg_val = mctl_read_w (MC_CCR);
    reg_val &= ~ (0xffff);
    reg_val |= 0x7 << 16;
    mctl_write_w (MC_CCR, reg_val);
    
    reg_val = mctl_read_w (MP_PIR);
    reg_val |= 0x1 << 17;
    mctl_write_w (MP_PIR, reg_val);
  }
  
  reg_val = mctl_read_w (MC_CCR);
  reg_val |= (0x1 << 19);
  mctl_write_w (MC_CCR, reg_val);
  
  reg_val = mctl_read_w (CCM_DRAM_CFG_REG);
  reg_val |= 0x1u << 31;
  mctl_write_w (CCM_DRAM_CFG_REG, reg_val);
  
  reg_val = mctl_read_w (CCM_DRAM_CFG_REG);
  reg_val |= (0x1u << 16);
  mctl_write_w (CCM_DRAM_CFG_REG, reg_val);
  
  while (mctl_read_w (CCM_DRAM_CFG_REG) & (0x1 << 16) ) {}
  
  i = (para->dram_clk == para->dram_tpr11) ? 0 : 1;
  
  mctl_write_w (MP_DXBDLR0 (0), mdfsvalue[i * 21 + 0]);
  mctl_write_w (MP_DXBDLR0 (1), mdfsvalue[i * 21 + 1]);
  
  mctl_write_w (MP_DXBDLR1 (0), mdfsvalue[i * 21 + 2]);
  mctl_write_w (MP_DXBDLR1 (1), mdfsvalue[i * 21 + 3]);
  
  mctl_write_w (MP_DXBDLR2 (0), mdfsvalue[i * 21 + 4]);
  mctl_write_w (MP_DXBDLR2 (1), mdfsvalue[i * 21 + 5]);
  
  mctl_write_w (MP_DXBDLR3 (0), mdfsvalue[i * 21 + 6]);
  mctl_write_w (MP_DXBDLR3 (1), mdfsvalue[i * 21 + 7]);
  
  mctl_write_w (MP_DXBDLR4 (0), mdfsvalue[i * 21 + 8]);
  mctl_write_w (MP_DXBDLR4 (1), mdfsvalue[i * 21 + 9]);
  
  mctl_write_w (MP_DXLCDLR0 (0), mdfsvalue[i * 21 + 10]);
  mctl_write_w (MP_DXLCDLR0 (1), mdfsvalue[i * 21 + 11]);
  
  mctl_write_w (MP_DXLCDLR1 (0), mdfsvalue[i * 21 + 12]);
  mctl_write_w (MP_DXLCDLR1 (1), mdfsvalue[i * 21 + 13]);
  
  mctl_write_w (MP_DXLCDLR2 (0), mdfsvalue[i * 21 + 14]);
  mctl_write_w (MP_DXLCDLR2 (1), mdfsvalue[i * 21 + 15]);
  
  mctl_write_w (MP_DXMDLR (0), mdfsvalue[i * 21 + 16]);
  mctl_write_w (MP_DXMDLR (1), mdfsvalue[i * 21 + 17]);
  
  mctl_write_w (MP_DXGTR (0), mdfsvalue[i * 21 + 18]);
  
  reg_val = mctl_read_w (MP_ZQCR0 (0) );
  reg_val &= ~0xFFFFFFF;
  reg_val |= (0x1 << 28) | (mdfsvalue[i * 21 + 19]);
  mctl_write_w (MP_ZQCR0 (0), reg_val);
  
  mctl_write_w (MP_ZQCR2, mdfsvalue[i * 21 + 20]);
  
  mctl_self_refresh_exit();
  
  return;
}

#endif






