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
#include <malloc.h>
#include <spi.h>
#include <asm/io.h>
#include <asm/arch/spi.h>
#include <asm/arch/ccmu.h>
#include <asm/arch/dma.h>
#define SUNXI_NOR_FLASH_DEBUG 1
#ifdef  SUNXI_NOR_FLASH_DEBUG
#define SUNXI_DEBUG(fmt,args...)	printf(fmt ,##args)
#else
#define SUNXI_DEBUG(fmt,args...) do {} while(0)
#endif

static u32 g_cfg_mclk = 0;
static sunxi_dma_setting_t *spi_tx_dma;
static sunxi_dma_setting_t *spi_rx_dma;
static  uint  spi_tx_dma_hd;
static  uint  spi_rx_dma_hd;

void ccm_module_disable_bak(u32 clk_id)
{
	switch(clk_id>>8) {
		case AHB1_BUS0:
			clr_wbit(CCM_AHB1_RST_REG0, 0x1U<<(clk_id&0xff));
			SUNXI_DEBUG("\nread CCM_AHB1_RST_REG0[0x%x]\n",readl(CCM_AHB1_RST_REG0));
			break;
	}
}

void ccm_module_enable_bak(u32 clk_id)
{
	switch(clk_id>>8) {
		case AHB1_BUS0:
			set_wbit(CCM_AHB1_RST_REG0, 0x1U<<(clk_id&0xff));
			SUNXI_DEBUG("\nread enable CCM_AHB1_RST_REG0[0x%x]\n",readl(CCM_AHB1_RST_REG0));
			break;
	}
}
void ccm_clock_enable_bak(u32 clk_id)
{
	switch(clk_id>>8) {
		case AXI_BUS:
			set_wbit(CCM_AXI_GATE_CTRL, 0x1U<<(clk_id&0xff));
			break;
		case AHB1_BUS0:
			set_wbit(CCM_AHB1_GATE0_CTRL, 0x1U<<(clk_id&0xff));
			SUNXI_DEBUG("read s CCM_AHB1_GATE0_CTRL[0x%x]\n",readl(CCM_AHB1_GATE0_CTRL));
			break;
	}
}

void ccm_clock_disable_bak(u32 clk_id)
{
	switch(clk_id>>8) {
		case AXI_BUS:
			clr_wbit(CCM_AXI_GATE_CTRL, 0x1U<<(clk_id&0xff));
			break;
		case AHB1_BUS0:
			clr_wbit(CCM_AHB1_GATE0_CTRL, 0x1U<<(clk_id&0xff));
			SUNXI_DEBUG("read dis CCM_AHB1_GATE0_CTRL[0x%x]\n",readl(CCM_AHB1_GATE0_CTRL));
			break;
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
static int spi_dma_recv_start(uint spi_no, uchar* pbuf, uint byte_cnt)
{
	flush_cache((uint)pbuf, byte_cnt);

	sunxi_dma_start(spi_rx_dma_hd, SPI_RXD, (uint)pbuf, byte_cnt);

	return 0;
}
static int spi_wait_dma_recv_over(uint spi_no)
{
	return  sunxi_dma_querystatus(spi_rx_dma_hd);
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
static int spi_dma_send_start(uint spi_no, uchar* pbuf, uint byte_cnt)
{
	flush_cache((uint)pbuf, byte_cnt);

	sunxi_dma_start(spi_tx_dma_hd, (uint)pbuf, SPI_TXD, byte_cnt);

	return 0;
}
static int spi_wait_dma_send_over(uint spi_no)
{
	return sunxi_dma_querystatus(spi_tx_dma_hd);
}

#define set_wbit(addr, v)   (*((volatile unsigned long  *)(addr)) |=  (unsigned long)(v))
#define clr_wbit(addr, v)   (*((volatile unsigned long  *)(addr)) &= ~(unsigned long)(v))

void ccm_module_reset_bak(u32 clk_id)
{
	ccm_module_disable_bak(clk_id);
	ccm_module_disable_bak(clk_id);
	ccm_module_enable_bak(clk_id);
}

u32 ccm_get_pll_periph_clk(void)
{
    u32 rval = 0;
	u32 n, k;
	rval = readl(CCM_PLL6_MOD_CTRL);
	n = (0x1f & (rval >> 8)) + 1;
	k = (0x3 & (rval >> 4)) + 1;
	return (24000000 * n * k)>>1;
}

void pattern_goto(int pos)
{
}

u32 spi_cfg_mclk(u32 spi_no, u32 src, u32 mclk)
{
#ifdef CONFIG_FPGA
	g_cfg_mclk = 24000000;
	return g_cfg_mclk;
#else
    u32 mclk_base = CCM_SPI0_SCLK_CTRL;
	u32 source_clk;
	u32 rval;
	u32 m, n, div;
	src  = 1;

    switch (src) {
	case 0:
		source_clk = 24000000;
		break;
	case 1:
		source_clk = ccm_get_pll_periph_clk();
		break;
	default :
		SUNXI_DEBUG("Wrong SPI clock source :%x\n", src);
	}
	SUNXI_DEBUG("SPI clock source :0x%x\n", source_clk);
	div = (source_clk + mclk - 1) / mclk;
	div = div==0 ? 1 : div;
	if (div > 128) {
		m = 1;
		n = 0;
		SUNXI_DEBUG("Source clock is too high\n");
	} else if (div > 64) {
		n = 3;
		m = div >> 3;
	} else if (div > 32) {
		n = 2;
		m = div >> 2;
	} else if (div > 16) {
		n = 1;
		m = div >> 1;
	} else {
		n = 0;
		m = div;
	}
	
	rval = (1U << 31) | (src << 24) | (n << 16) | (m - 1);
	writel(rval, mclk_base);
	g_cfg_mclk = source_clk / (1 << n) / (m - 1);
	SUNXI_DEBUG("spi spic->sclk =0x%x\n",g_cfg_mclk );
	return g_cfg_mclk;
#endif
}

u32 spi_get_mlk(u32 spi_no)
{
	return g_cfg_mclk ;//spicinfo[spi_no].sclk;
}

void spi_gpio_cfg(int spi_no)
{
	uint reg_val = 0;
	writel(0x3333, (0x1c20800 + 0x48));  
	reg_val = readl(0x1c20800 + 0x64);	 
	reg_val &= ~(0x03 << 4);
	reg_val |= (0x01 << 4); 
	writel(reg_val, (0x1c20800 + 0x64));
	
	SUNXI_DEBUG("Reg pull reg_val=0x%x,read=0x%x\n",reg_val,readl((0x1c20800 + 0x64)));
}

void spi_onoff(u32 spi_no, u32 onoff)
{
	u32 clkid[] = {SPI0_CKID, SPI1_CKID};
	spi_no = 0;
	switch (spi_no) {
	case 0:
            spi_gpio_cfg(0);	
            break;
	}
	ccm_module_reset_bak(clkid[spi_no]);
	if (onoff)
		ccm_clock_enable_bak(clkid[spi_no]);
	else
		ccm_clock_disable_bak(clkid[spi_no]);

}

void spic_set_clk(u32 spi_no, u32 clk)
{
	u32 mclk = spi_get_mlk(spi_no);
	u32 div;
	u32 cdr1 = 0;
	u32 cdr2 = 0;
	u32 cdr_sel = 0;
	
	div = mclk/(clk<<1);
	
	if (div==0) {
		cdr1 = 0;
	
		cdr2 = 0;
		cdr_sel = 0;
	} else if (div<=0x100) {
		cdr1 = 0;
	
		cdr2 = div-1;
		cdr_sel = 1;
	} else {
		div = 0;
		while (mclk > clk) {
		    div++;
		    mclk >>= 1;
		}
		cdr1 = div;
	
		cdr2 = 0;
		cdr_sel = 0;
	}
	
	writel((cdr_sel<<12)|(cdr1<<8)|cdr2, SPI_CCR);
	SUNXI_DEBUG("spic_set_clk:mclk=0x%x\n",mclk);
}

int spic_init(u32 spi_no)
{
	u32 rval;
	spi_rx_dma = malloc_noncache(sizeof(sunxi_dma_setting_t));
	spi_tx_dma = malloc_noncache(sizeof(sunxi_dma_setting_t));
	if(!(spi_rx_dma) || !(spi_tx_dma))
	{
		printf("no enough memory to malloc \n");
		return -1;
	}
	memset(spi_tx_dma , 0 , sizeof(sunxi_dma_setting_t));
	memset(spi_rx_dma , 0 , sizeof(sunxi_dma_setting_t));
	spi_rx_dma_hd = sunxi_dma_request(DMAC_DMATYPE_NORMAL);
	spi_tx_dma_hd = sunxi_dma_request(DMAC_DMATYPE_NORMAL);
	

	if((spi_tx_dma_hd == 0) || (spi_rx_dma_hd == 0))
	{
		printf("spi request dma failed\n");

		return -1;
	}
	spi_rx_dma->cfg.src_drq_type     = DMAC_CFG_TYPE_SPI0; 
	spi_rx_dma->cfg.src_addr_mode    = DMAC_CFG_SRC_ADDR_TYPE_IO_MODE;
	spi_rx_dma->cfg.src_burst_length = DMAC_CFG_SRC_1_BURST;
	spi_rx_dma->cfg.src_data_width   = DMAC_CFG_SRC_DATA_WIDTH_8BIT;

	spi_rx_dma->cfg.dst_drq_type     = DMAC_CFG_TYPE_DRAM; 
	spi_rx_dma->cfg.dst_addr_mode    = DMAC_CFG_DEST_ADDR_TYPE_LINEAR_MODE;
	spi_rx_dma->cfg.dst_burst_length = DMAC_CFG_DEST_1_BURST;
	spi_rx_dma->cfg.dst_data_width   = DMAC_CFG_DEST_DATA_WIDTH_8BIT;

	spi_tx_dma->cfg.src_drq_type     = DMAC_CFG_TYPE_SRAM; 
	spi_tx_dma->cfg.src_addr_mode    = DMAC_CFG_SRC_ADDR_TYPE_LINEAR_MODE;
	spi_tx_dma->cfg.src_burst_length = DMAC_CFG_SRC_1_BURST;
	spi_tx_dma->cfg.src_data_width   = DMAC_CFG_SRC_DATA_WIDTH_8BIT;
	
	spi_tx_dma->cfg.dst_drq_type     = DMAC_CFG_TYPE_SPI0; 
	spi_tx_dma->cfg.dst_addr_mode    = DMAC_CFG_DEST_ADDR_TYPE_IO_MODE;
	spi_tx_dma->cfg.dst_burst_length = DMAC_CFG_DEST_1_BURST;
	spi_tx_dma->cfg.dst_data_width   = DMAC_CFG_DEST_DATA_WIDTH_8BIT;
	spi_tx_dma->wait_cyc = 0x10;

	sunxi_dma_setting(spi_rx_dma_hd, (void *)spi_rx_dma);
	sunxi_dma_setting(spi_tx_dma_hd, (void *)spi_tx_dma);
	spi_no = 0;
	spi_onoff(spi_no, 1);
	
	spi_cfg_mclk(spi_no, SPI_CLK_SRC, SPI_MCLK);
	spic_set_clk(spi_no, SPI_DEFAULT_CLK);
	
	rval = SPI_SOFT_RST|SPI_TXPAUSE_EN|SPI_MASTER|SPI_ENABLE;
	writel(rval, SPI_GCR);
	rval = SPI_SET_SS_1|SPI_DHB|SPI_SS_ACTIVE0;  
	writel(rval, SPI_TCR);
    writel(SPI_TXFIFO_RST|(SPI_TX_WL<<16)|(SPI_RX_WL), SPI_FCR);
    return 0;    
}

int spic_rw( u32 tcnt, void* txbuf, u32 rcnt, void* rxbuf) 
{
	u32 i = 0,fcr;
	int timeout = 0xfffff;

	u8 *tx_buffer = txbuf ;
	u8 *rx_buffer = rxbuf;
	writel(SPI_IER, 0);
	writel(SPI_ISR, 0xffffffff);//clear status register

	writel(tcnt, SPI_MTC);
	writel(tcnt+rcnt, SPI_MBC);
	writel(readl(SPI_TCR)|SPI_EXCHANGE, SPI_TCR);
	if(tcnt)
	{
		if(tcnt < 64 )
    	{
			i = 0;
			while (i < tcnt)
			{
				while(((readl(SPI_FSR)>>16)==SPI_FIFO_SIZE) );
				writeb(*(tx_buffer+i),SPI_TXD);
				i++;
			}
		}
		else
		{
			writel((readl(SPI_FCR)|SPI_TXDMAREQ_EN), SPI_FCR);
			spi_dma_send_start(0, txbuf, tcnt);
            /* wait DMA finish */
			while ((timeout-- > 0) && spi_wait_dma_send_over(0));
			if (timeout <= 0)
			{
				printf("tx wait_dma_send_over fail\n");
				return -1;
			}
		}
	}
	timeout = 0xfffff;
	/* start transmit */
	if(rcnt)
	{
		if(rcnt < 64)
		{
			i = 0;
			while(i < rcnt)
			{
				while(((readl(SPI_FSR))&0x7f)==0);
				*(rx_buffer+i)=readb(SPI_RXD);
				i++;
			}
		}
		else
		{
            pattern_goto(115);
			timeout = 0xfffff;
			writel((readl(SPI_FCR)|SPI_RXDMAREQ_EN), SPI_FCR);
			spi_dma_recv_start(0, rxbuf, rcnt);
            /* wait DMA finish */
			while ((timeout-- > 0) && spi_wait_dma_recv_over(0));
			if (timeout <= 0)
			{
				printf("rx wait_dma_recv_over fail\n");
				return -1;
			}
		}
	}
#if 1
    timeout = 0xfffff;
	while(!(readl(SPI_ISR)&(0x1<<12)))//wait transfer complete
	{
		timeout--;
		if (!timeout)
		{
			printf("SPI_ISR time_out \n");
			break;
		}
	}
	timeout = 0xfffff;
    while(DMA_CHAN_STA_REG & 0x1)
    {
		timeout--;
		if (!timeout)
		{
			printf("DMA_CHAN_STA_REG timeout\n");
			break;
		}
	}
	pattern_goto(121);
#endif

    fcr = readl(SPI_FCR);
    fcr &= ~(SPI_TXDMAREQ_EN|SPI_RXDMAREQ_EN);
	writel(fcr, SPI_FCR);
	if ((readl(SPI_ISR) & (0xf << 8))|| (timeout==0))	/* (1U << 11) | (1U << 10) | (1U << 9) | (1U << 8)) */
			return RET_FAIL;

	if(readl(SPI_TCR)&SPI_EXCHANGE)
	{
		printf("XCH Control Error!!\n");
	}

	writel(0xfffff,SPI_ISR);  /* clear  flag */
	return RET_OK;

}

int spic_exit(u32 spi_no)
{
	if(spi_tx_dma)
		free(spi_tx_dma);
	if(spi_rx_dma)
		free(spi_rx_dma);
	sunxi_dma_release(spi_tx_dma_hd);
	sunxi_dma_release(spi_rx_dma_hd);
	return 0;
}

