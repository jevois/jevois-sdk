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

#ifndef __SUNXI_INTC_H__
#define __SUNXI_INTC_H__

#define AW_IRQ_GIC_START    (32)

#ifndef CONFIG_SUN6I_FPGA

#define AW_IRQ_NMI			(AW_IRQ_GIC_START + 0)  
#define AW_IRQ_UART0		(AW_IRQ_GIC_START + 1)  
#define AW_IRQ_UART1		(AW_IRQ_GIC_START + 2)  
#define AW_IRQ_UART2		(AW_IRQ_GIC_START + 3)  
#define AW_IRQ_UART3		(AW_IRQ_GIC_START + 4)  
#define AW_IRQ_CIR0		(AW_IRQ_GIC_START + 5)  
#define AW_IRQ_CIR1		(AW_IRQ_GIC_START + 6  )
#define AW_IRQ_TWI0		(AW_IRQ_GIC_START + 7  )
#define AW_IRQ_TWI1		(AW_IRQ_GIC_START + 8  )
#define AW_IRQ_TWI2		(AW_IRQ_GIC_START + 9  )
#define AW_IRQ_SPI0		(AW_IRQ_GIC_START + 10 )
#define AW_IRQ_SPI1		(AW_IRQ_GIC_START + 11 )
#define AW_IRQ_SPI2		(AW_IRQ_GIC_START + 12 )
#define AW_IRQ_SPDIF	(AW_IRQ_GIC_START + 13 )
#define AW_IRQ_AC97		(AW_IRQ_GIC_START + 14 )
#define AW_IRQ_TS			(AW_IRQ_GIC_START + 15 )
#define AW_IRQ_IIS			(AW_IRQ_GIC_START + 16 )
#define AW_IRQ_UART4		(AW_IRQ_GIC_START + 17 )
#define AW_IRQ_UART5		(AW_IRQ_GIC_START + 18 )
#define AW_IRQ_UART6		(AW_IRQ_GIC_START + 19 )
#define AW_IRQ_UART7		(AW_IRQ_GIC_START + 20 )
#define AW_IRQ_KP				(AW_IRQ_GIC_START + 21 )
#define AW_IRQ_TIMER0		(AW_IRQ_GIC_START + 22 )
#define AW_IRQ_TIMER1		(AW_IRQ_GIC_START + 23 )
#define AW_IRQ_TIMER2		(AW_IRQ_GIC_START + 24 )
#define AW_IRQ_TIMER3		(AW_IRQ_GIC_START + 25 )
#define AW_IRQ_CAN			(AW_IRQ_GIC_START + 26 )
#define AW_IRQ_DMA			(AW_IRQ_GIC_START + 27 )
#define AW_IRQ_PIO_EINT	(AW_IRQ_GIC_START + 28 )
#define AW_IRQ_TP			(AW_IRQ_GIC_START + 29 )
#define AW_IRQ_CODEC	(AW_IRQ_GIC_START + 30 )
#define AW_IRQ_LRADC	(AW_IRQ_GIC_START + 31 )
#define AW_IRQ_MMC0		(AW_IRQ_GIC_START + 32 )
#define AW_IRQ_MMC1		(AW_IRQ_GIC_START + 33 )
#define AW_IRQ_MMC2		(AW_IRQ_GIC_START + 34 )
#define AW_IRQ_MMC3		(AW_IRQ_GIC_START + 35 )
#define AW_IRQ_MS			(AW_IRQ_GIC_START + 36 )
#define AW_IRQ_NAND		(AW_IRQ_GIC_START + 37 )
#define AW_IRQ_USB_OTG		(AW_IRQ_GIC_START + 38 )
#define AW_IRQ_USB1		(AW_IRQ_GIC_START + 39 )
#define AW_IRQ_USB2		(AW_IRQ_GIC_START + 40 )
#define AW_IRQ_SCR		(AW_IRQ_GIC_START + 41 )
#define AW_IRQ_CSI0		(AW_IRQ_GIC_START + 42 )
#define AW_IRQ_CSI1		(AW_IRQ_GIC_START + 43 )
#define AW_IRQ_LCD0		(AW_IRQ_GIC_START + 44 )
#define AW_IRQ_LCD1		(AW_IRQ_GIC_START + 45 )
#define AW_IRQ_MP			(AW_IRQ_GIC_START + 46 )
#define AW_IRQ_DEBE0	(AW_IRQ_GIC_START + 47 )
#define AW_IRQ_DEFE0	(AW_IRQ_GIC_START + 47 )
#define AW_IRQ_DEBE1	(AW_IRQ_GIC_START + 48 )
#define AW_IRQ_DEFE1	(AW_IRQ_GIC_START + 48 )
#define AW_IRQ_PMU		(AW_IRQ_GIC_START + 49 )
#define GIC_SRC_SPI3	(AW_IRQ_GIC_START + 50 )

#define AW_IRQ_VE			(AW_IRQ_GIC_START + 53 )
#define AW_IRQ_SS			(AW_IRQ_GIC_START + 54 )
#define AW_IRQ_EMAC		(AW_IRQ_GIC_START + 55 )
#define AW_IRQ_SATA		(AW_IRQ_GIC_START + 56 )
#define AW_IRQ_HDMI		(AW_IRQ_GIC_START + 58 )
#define AW_IRQ_TVE01	(AW_IRQ_GIC_START + 59 )
#define AW_IRQ_ACE		(AW_IRQ_GIC_START + 60 )
#define AW_IRQ_TVD		(AW_IRQ_GIC_START + 61 )
#define AW_IRQ_PS2_0	(AW_IRQ_GIC_START + 62 )
#define AW_IRQ_PS2_1	(AW_IRQ_GIC_START + 63 )
#define AW_IRQ_USB3		(AW_IRQ_GIC_START + 64 )
#define AW_IRQ_USB4		(AW_IRQ_GIC_START + 65 )
#define AW_IRQ_PLE_PERFMU	(AW_IRQ_GIC_START + 66 )
#define AW_IRQ_TIMER4			(AW_IRQ_GIC_START + 67 )
#define AW_IRQ_TIMER5			(AW_IRQ_GIC_START + 68 )
#define AW_IRQ_GPU_GP			(AW_IRQ_GIC_START + 69 )
#define AW_IRQ_GPU_GPMMU	(AW_IRQ_GIC_START + 70 )
#define AW_IRQ_GPU_PP0		(AW_IRQ_GIC_START + 71 )
#define AW_IRQ_GPU_PPMMU0	(AW_IRQ_GIC_START + 72 )
#define AW_IRQ_GPU_PMU		(AW_IRQ_GIC_START + 73 )
#define AW_IRQ_GPU_PP1		(AW_IRQ_GIC_START + 74 )
#define AW_IRQ_GPU_PPMMU1	(AW_IRQ_GIC_START + 75 )
#define AW_IRQ_GPU_RSV0	(AW_IRQ_GIC_START + 76 )
#define AW_IRQ_GPU_RSV1	(AW_IRQ_GIC_START + 77 )
#define AW_IRQ_GPU_RSV2	(AW_IRQ_GIC_START + 78 )
#define AW_IRQ_GPU_RSV3	(AW_IRQ_GIC_START + 79 )
#define AW_IRQ_GPU_RSV4	(AW_IRQ_GIC_START + 80 )
#define AW_IRQ_HSTMR0		(AW_IRQ_GIC_START + 81 )
#define AW_IRQ_HSTMR1		(AW_IRQ_GIC_START + 82 )
#define AW_IRQ_HSTMR2		(AW_IRQ_GIC_START + 83 )
#define AW_IRQ_HSTMR3		(AW_IRQ_GIC_START + 84 )
#define AW_IRQ_GMAC			(AW_IRQ_GIC_START + 85 )
#define AW_IRQ_MIPIDSI		(AW_IRQ_GIC_START + 89)		/*	MIPI DSI	    */
#define AW_IRQ_DRC01		(AW_IRQ_GIC_START + 91)		/*	DRC 0/1		*/
#define AW_IRQ_DEU01		(AW_IRQ_GIC_START + 92)		/*	DEU	0/1		*/
#define	AW_IRQ_AXI_ERROR	(AW_IRQ_GIC_START + 124)	/*	AXI_ERROR	*/
#define GIC_IRQ_NUM			(AW_IRQ_AXI_ERROR + 1)

#elif   defined (AW_FPGA_V4_PLATFORM) 			   /* S4 820 */
#define	AW_IRQ_NMI			(AW_IRQ_GIC_START + 0) /*	NMI			*/
#define AW_IRQ_UART0		(AW_IRQ_GIC_START + 1) /*	UART0		*/
#define AW_IRQ_UART1    	(AW_IRQ_GIC_START + 1) /*   UART1   	*/
#define AW_IRQ_TWI0			(AW_IRQ_GIC_START + 2) /*	TWI0		*/
#define AW_IRQ_TWI1			(AW_IRQ_GIC_START + 2) /*	TWI1		*/
#define AW_IRQ_EINTA		(AW_IRQ_GIC_START + 2) /*	EINTA		*/
#define AW_IRQ_SPDIF		(AW_IRQ_GIC_START + 3) /*	SPDIF		*/
#define AW_IRQ_IIS			(AW_IRQ_GIC_START + 3) /*	IIS			*/
#define AW_IRQ_MP			(AW_IRQ_GIC_START + 3) /*	MP			*/
#define AW_IRQ_CSI0			(AW_IRQ_GIC_START + 3) /*	CSI0		*/
#define AW_IRQ_CSI1			(AW_IRQ_GIC_START + 3) /*	CSI1		*/
#define	AW_IRQ_MIPIDSI		(AW_IRQ_GIC_START + 3) /*	MIPIDSI		*/
#define	AW_IRQ_MIPICSI		(AW_IRQ_GIC_START + 3) /*	MIPICSI		*/
#define	AW_IRQ_TIMER0		(AW_IRQ_GIC_START + 4) /*	TIMER0		*/
#define AW_IRQ_TIMER1		(AW_IRQ_GIC_START + 5) /*	TIMER1		*/
#define AW_IRQ_TIMER2		(AW_IRQ_GIC_START + 5) /*	TIMER2		*/
#define AW_IRQ_TIMER3		(AW_IRQ_GIC_START + 5) /*	TIMER3		*/
#define AW_IRQ_TIMER4		(AW_IRQ_GIC_START + 5) /*	TIMER4		*/
#define AW_IRQ_TIMER5		(AW_IRQ_GIC_START + 5) /*	TIMER5		*/
#define AW_IRQ_DMA			(AW_IRQ_GIC_START + 6) /*	DMA			*/
#define	AW_IRQ_WATCHDOG1	(AW_IRQ_GIC_START + 7) /*	WATCHDOG1	*/
#define	AW_IRQ_WATCHDOG2	(AW_IRQ_GIC_START + 8) /*	WATCHDOG2	*/
#define	AW_IRQ_RTIMER0		(AW_IRQ_GIC_START + 9) /*	RTIMER0		*/
#define	AW_IRQ_RTIMER1		(AW_IRQ_GIC_START + 9) /*	RTIMER1		*/
#define	AW_IRQ_RWATCHDOG	(AW_IRQ_GIC_START + 9) /*	RWATCHDOG	*/
#define	AW_IRQ_SPINLOCK		(AW_IRQ_GIC_START + 10)/*	SPINLOCK	*/
#define AW_IRQ_MBOX			(AW_IRQ_GIC_START + 11)/*	MBOX		*/
#define	AW_IRQ_HSTIMER0		(AW_IRQ_GIC_START + 12)/*	HSTIMER		*/
#define	AW_IRQ_MMC0			(AW_IRQ_GIC_START + 13)/*	MMC0		*/
#define AW_IRQ_MMC1			(AW_IRQ_GIC_START + 13)/*	MMC1		*/
#define AW_IRQ_SPI0			(AW_IRQ_GIC_START + 13)/*	SPI0		*/
#define AW_IRQ_NAND0		(AW_IRQ_GIC_START + 14)/*	NAND0		*/
#define AW_IRQ_NAND1		(AW_IRQ_GIC_START + 15)/*	NAND1		*/
#define AW_IRQ_MMC2			(AW_IRQ_GIC_START + 15)/*	MMC2		*/
#define AW_IRQ_MMC3			(AW_IRQ_GIC_START + 15)/*	MMC3		*/
#define AW_IRQ_RCIR			(AW_IRQ_GIC_START + 16)/*	RCIR		*/
#define AW_IRQ_RUART		(AW_IRQ_GIC_START + 16)/*	RUART		*/
#define AW_IRQ_EINTL		(AW_IRQ_GIC_START + 17)/*	EINTL		*/
#define AW_IRQ_EINTM		(AW_IRQ_GIC_START + 17)/*	EINTM		*/
#define AW_IRQ_RALARM0		(AW_IRQ_GIC_START + 18)/*	RALARM0		*/
#define AW_IRQ_RALARM1		(AW_IRQ_GIC_START + 18)/*	RALARM1		*/
#define AW_IRQ_RP2TWI		(AW_IRQ_GIC_START + 19)/*	RP2WI		*/
#define AW_IRQ_RTWI			(AW_IRQ_GIC_START + 19)/*	RTWI		*/
#define AW_IRQ_VE			(AW_IRQ_GIC_START + 20)/*	VE			*/
#define AW_IRQ_USB_OTG		(AW_IRQ_GIC_START + 21)/*	USB_OTG		*/
#define AW_IRQ_USB_HCI		(AW_IRQ_GIC_START + 21)/*	USB_HCI		*/
#define AW_IRQ_USB_EHCI0	(AW_IRQ_GIC_START + 22)/*	USB_EHCI0	*/
#define AW_IRQ_USB_OHCI0	(AW_IRQ_GIC_START + 23)/*	USB_OHCI0	*/
#define AW_IRQ_SS			(AW_IRQ_GIC_START + 24)/*	SS			*/
#define AW_IRQ_TS			(AW_IRQ_GIC_START + 24)/*	TS			*/
#define AW_IRQ_GMAC			(AW_IRQ_GIC_START + 24)/*	GMAC		*/
#define AW_IRQ_CODEC		(AW_IRQ_GIC_START + 24)/*	CODEC		*/
#define AW_IRQ_TP			(AW_IRQ_GIC_START + 24)/*	TP			*/
#define AW_IRQ_LRADC		(AW_IRQ_GIC_START + 24)/*	LRADC		*/
#define AW_IRQ_MTCACC		(AW_IRQ_GIC_START + 24)/*	MTCACC		*/
#define AW_IRQ_LCD0			(AW_IRQ_GIC_START + 25)/*	LCD0		*/
#define AW_IRQ_TZASC		(AW_IRQ_GIC_START + 26)/*	TZASC		*/
#define AW_IRQ_DEFE0		(AW_IRQ_GIC_START + 27)/*	DEFE0		*/
#define AW_IRQ_DEBE0		(AW_IRQ_GIC_START + 28)/*	DEFB0		*/
#define AW_IRQ_DRC01		(AW_IRQ_GIC_START + 29)/*	DRC01		*/
#define AW_IRQ_DEU01		(AW_IRQ_GIC_START + 30)/*	DEU01		*/
#define AW_IRQ_1WIRE		(AW_IRQ_GIC_START + 31)/*	R_1WIRE		*/
#define GIC_IRQ_NUM			(AW_IRQ_1WIRE + 1)

#elif defined (AW_FPGA_V7_PLATFORM)

#define AW_IRQ_NMI			(AW_IRQ_GIC_START + 0) /*	NMI			*/
#define AW_IRQ_UART0		(AW_IRQ_GIC_START + 1) /*	UART0		*/
#define AW_IRQ_TWI0			(AW_IRQ_GIC_START + 2) /*	TWI0		*/
#define AW_IRQ_TWI1			(AW_IRQ_GIC_START + 2) /*	TWI1		*/
#define AW_IRQ_SPDIF		(AW_IRQ_GIC_START + 3) /*	SPDIF		*/
#define AW_IRQ_IIS			(AW_IRQ_GIC_START + 3) /*	IIS			*/
#define AW_IRQ_CSI0			(AW_IRQ_GIC_START + 3) /*	CSI0		*/
#define AW_IRQ_CSI1			(AW_IRQ_GIC_START + 3) /*	CSI1		*/
#define	AW_IRQ_MIPIDSI		(AW_IRQ_GIC_START + 3) /*	MIPIDSI		*/
#define	AW_IRQ_MIPICSI		(AW_IRQ_GIC_START + 3) /*	MIPICSI		*/
#define	AW_IRQ_TIMER0		(AW_IRQ_GIC_START + 4) /*	TIMER0		*/
#define AW_IRQ_TIMER1		(AW_IRQ_GIC_START + 5) /*	TIMER1		*/
#define AW_IRQ_DMA			(AW_IRQ_GIC_START + 6) /*	DMA			*/
#define	AW_IRQ_WATCHDOG1	(AW_IRQ_GIC_START + 7) /*	WATCHDOG1	*/
#define	AW_IRQ_WATCHDOG2	(AW_IRQ_GIC_START + 8) /*	WATCHDOG2	*/
#define	AW_IRQ_RTIMER0		(AW_IRQ_GIC_START + 9) /*	RTIMER0		*/
#define	AW_IRQ_RTIMER1		(AW_IRQ_GIC_START + 9) /*	RTIMER1		*/
#define	AW_IRQ_RWATCHDOG	(AW_IRQ_GIC_START + 9) /*	RWATCHDOG	*/
#define	AW_IRQ_SPINLOCK		(AW_IRQ_GIC_START + 10)/*	SPINLOCK	*/
#define AW_IRQ_MBOX			(AW_IRQ_GIC_START + 11)/*	MBOX		*/
#define	AW_IRQ_HSTIMER0		(AW_IRQ_GIC_START + 12)/*	HSTIMER		*/
#define	AW_IRQ_MMC0			(AW_IRQ_GIC_START + 13)/*	MMC0		*/
#define	AW_IRQ_MMC1			(AW_IRQ_GIC_START + 13)/*	MMC1		*/
#define	AW_IRQ_MMC2			(AW_IRQ_GIC_START + 13)/*	MMC2		*/
#define	AW_IRQ_MMC3			(AW_IRQ_GIC_START + 13)/*	MMC3		*/
#define AW_IRQ_SPI0			(AW_IRQ_GIC_START + 13)/*	SPI0		*/
#define AW_IRQ_NAND0		(AW_IRQ_GIC_START + 14)/*	NAND0		*/
#define AW_IRQ_NAND1		(AW_IRQ_GIC_START + 15)/*	NAND1		*/
#define AW_IRQ_RCIR			(AW_IRQ_GIC_START + 16)/*	RCIR		*/
#define AW_IRQ_RUART		(AW_IRQ_GIC_START + 16)/*	RUART		*/
#define AW_IRQ_RGPIOL		(AW_IRQ_GIC_START + 17)/*	RGPIOL		*/
#define AW_IRQ_RALARM0		(AW_IRQ_GIC_START + 18)/*	RALARM0		*/
#define AW_IRQ_RALARM1		(AW_IRQ_GIC_START + 18)/*	RALARM1		*/
#define AW_IRQ_RP2TWI		(AW_IRQ_GIC_START + 19)/*	RP2WI		*/
#define AW_IRQ_RTWI			(AW_IRQ_GIC_START + 19)/*	RTWI		*/
#define AW_IRQ_VE			(AW_IRQ_GIC_START + 20)/*	VE			*/
#define AW_IRQ_USB_OTG		(AW_IRQ_GIC_START + 21)/*	USB_OTG		*/
#define AW_IRQ_USB_HCI		(AW_IRQ_GIC_START + 21)/*	USB_HCI		*/
#define AW_IRQ_USB_EHCI0	(AW_IRQ_GIC_START + 22)/*	USB_EHCI0	*/
#define AW_IRQ_USB_OHCI0	(AW_IRQ_GIC_START + 23)/*	USB_OHCI0	*/
#define AW_IRQ_SS			(AW_IRQ_GIC_START + 24)/*	SS			*/
#define AW_IRQ_TS			(AW_IRQ_GIC_START + 24)/*	TS			*/
#define AW_IRQ_GMAC			(AW_IRQ_GIC_START + 24)/*	GMAC		*/
#define AW_IRQ_CODEC		(AW_IRQ_GIC_START + 24)/*	CODEC		*/
#define AW_IRQ_TP			(AW_IRQ_GIC_START + 24)/*	TP			*/
#define AW_IRQ_LCD0			(AW_IRQ_GIC_START + 25)/*	LCD0		*/
#define AW_IRQ_TZASC		(AW_IRQ_GIC_START + 26)/*	TZASC		*/
#define AW_IRQ_GPU			(AW_IRQ_GIC_START + 26)/*	GPU			*/
#define AW_IRQ_DEFE0		(AW_IRQ_GIC_START + 27)/*	DEFE0		*/
#define AW_IRQ_DEBE0		(AW_IRQ_GIC_START + 28)/*	DEFB0		*/
#define AW_IRQ_DRC01		(AW_IRQ_GIC_START + 29)/*	DRC01		*/
#define AW_IRQ_DEU01		(AW_IRQ_GIC_START + 30)/*	DEU01		*/
#define AW_IRQ_1WIRE		(AW_IRQ_GIC_START + 31)/*	R_1WIRE		*/

#define GIC_IRQ_NUM			(AW_IRQ_1WIRE + 1)

#endif

/* processer target */
#define GIC_CPU_TARGET(_n)	(1 << (_n))
#define GIC_CPU_TARGET0		GIC_CPU_TARGET(0)
#define GIC_CPU_TARGET1		GIC_CPU_TARGET(1)
#define GIC_CPU_TARGET2		GIC_CPU_TARGET(2)
#define GIC_CPU_TARGET3		GIC_CPU_TARGET(3)
#define GIC_CPU_TARGET4		GIC_CPU_TARGET(4)
#define GIC_CPU_TARGET5		GIC_CPU_TARGET(5)
#define GIC_CPU_TARGET6		GIC_CPU_TARGET(6)
#define GIC_CPU_TARGET7		GIC_CPU_TARGET(7)
/* trigger mode */
#define GIC_SPI_LEVEL_TRIGGER	(0)
#define GIC_SPI_EDGE_TRIGGER	(2)

extern void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data);
extern void irq_free_handler(int irq);
extern int irq_enable(int irq_no);
extern int irq_disable(int irq_no);

int arch_interrupt_init (void);

int arch_interrupt_exit (void);


#endif
