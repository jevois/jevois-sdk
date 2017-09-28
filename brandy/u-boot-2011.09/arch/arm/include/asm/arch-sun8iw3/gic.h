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

#ifndef __GIC_H
#define __GIC_H

#include "platform.h"
#include "intc.h"

/* GIC registers */
#define GIC_DIST_CON		(ARMA9_GIC_BASE + 0x0000)
#define GIC_CON_TYPE		(ARMA9_GIC_BASE + 0x0004)
#define GIC_CON_IIDR		(ARMA9_GIC_BASE + 0x0008)

#define GIC_CON_IGRP		(ARMA9_GIC_BASE + 0x0080)

#define GIC_SET_EN(_n)		(ARMA9_GIC_BASE + 0x100 + 4 * (_n))
#define GIC_SET_EN0			GIC_SET_EN(0)
#define GIC_SET_EN1			GIC_SET_EN(1)  
#define GIC_SET_EN2			GIC_SET_EN(2)  
#define GIC_SET_EN3			GIC_SET_EN(3)  
#define GIC_SET_EN4			GIC_SET_EN(4)  

#define GIC_CLR_EN(_n)		(ARMA9_GIC_BASE + 0x180 + 4 * (_n))
#define GIC_CLR_EN0			GIC_CLR_EN(0)
#define GIC_CLR_EN1			GIC_CLR_EN(1)  
#define GIC_CLR_EN2			GIC_CLR_EN(2)  
#define GIC_CLR_EN3			GIC_CLR_EN(3)  
#define GIC_CLR_EN4			GIC_CLR_EN(4)  

#define GIC_PEND_SET(_n)	(ARMA9_GIC_BASE + 0x200 + 4 * (_n))
#define GIC_PEND_SET0		GIC_PEND_SET(0)
#define GIC_PEND_SET1		GIC_PEND_SET(1)
#define GIC_PEND_SET2		GIC_PEND_SET(2)
#define GIC_PEND_SET3		GIC_PEND_SET(3)
#define GIC_PEND_SET4		GIC_PEND_SET(4)

#define GIC_PEND_CLR(_n)	(ARMA9_GIC_BASE + 0x280 + 4 * (_n))
#define GIC_PEND_CLR0		GIC_PEND_CLR(0)
#define GIC_PEND_CLR1		GIC_PEND_CLR(1)
#define GIC_PEND_CLR2		GIC_PEND_CLR(2)
#define GIC_PEND_CLR3		GIC_PEND_CLR(3)
#define GIC_PEND_CLR4		GIC_PEND_CLR(4)

#define GIC_ACT_SET(_n)		(ARMA9_GIC_BASE + 0x300 + 4 * (_n))
#define GIC_ACT_SET0		GIC_ACT_SET(0)
#define GIC_ACT_SET1		GIC_ACT_SET(1) 
#define GIC_ACT_SET2		GIC_ACT_SET(2) 
#define GIC_ACT_SET3		GIC_ACT_SET(3) 
#define GIC_ACT_SET4		GIC_ACT_SET(4) 

#define GIC_ACT_CLR(_n)		(ARMA9_GIC_BASE + 0x380 + 4 * (_n))
#define GIC_ACT_CLR0		GIC_ACT_CLR(0)
#define GIC_ACT_CLR1		GIC_ACT_CLR(1) 
#define GIC_ACT_CLR2		GIC_ACT_CLR(2) 
#define GIC_ACT_CLR3		GIC_ACT_CLR(3) 
#define GIC_ACT_CLR4		GIC_ACT_CLR(4) 

#define GIC_SGI_PRIO(_n)	(ARMA9_GIC_BASE + 0x400 + 4 * (_n))
#define GIC_SGI_PRIO0		GIC_SGI_PRIO(0)
#define GIC_SGI_PRIO1		GIC_SGI_PRIO(1)
#define GIC_SGI_PRIO2		GIC_SGI_PRIO(2)
#define GIC_SGI_PRIO3		GIC_SGI_PRIO(3)

#define GIC_PPI_PRIO(_n)	(ARMA9_GIC_BASE + 0x410 + 4 * (_n))
#define GIC_PPI_PRIO0		GIC_PPI_PRIO(0)
#define GIC_PPI_PRIO1		GIC_PPI_PRIO(1)
#define GIC_PPI_PRIO2		GIC_PPI_PRIO(2)
#define GIC_PPI_PRIO3		GIC_PPI_PRIO(3)

#define GIC_SPI_PRIO(_n)	(ARMA9_GIC_BASE + 0x420 + 4 * (_n))
#define GIC_SPI_PRIO0		GIC_SPI_PRIO(0 )
#define GIC_SPI_PRIO1 		GIC_SPI_PRIO(1 )   
#define GIC_SPI_PRIO2 		GIC_SPI_PRIO(2 )   
#define GIC_SPI_PRIO3 		GIC_SPI_PRIO(3 )   
#define GIC_SPI_PRIO4 		GIC_SPI_PRIO(4 )   
#define GIC_SPI_PRIO5 		GIC_SPI_PRIO(5 )   
#define GIC_SPI_PRIO6 		GIC_SPI_PRIO(6 )   
#define GIC_SPI_PRIO7 		GIC_SPI_PRIO(7 )   
#define GIC_SPI_PRIO8 		GIC_SPI_PRIO(8 )   
#define GIC_SPI_PRIO9 		GIC_SPI_PRIO(9 )   
#define GIC_SPI_PRIO10		GIC_SPI_PRIO(10)   
#define GIC_SPI_PRIO11		GIC_SPI_PRIO(11)   
#define GIC_SPI_PRIO12		GIC_SPI_PRIO(12)   
#define GIC_SPI_PRIO13		GIC_SPI_PRIO(13)   
#define GIC_SPI_PRIO14		GIC_SPI_PRIO(14)   
#define GIC_SPI_PRIO15		GIC_SPI_PRIO(15)   
#define GIC_SPI_PRIO16		GIC_SPI_PRIO(16)   
#define GIC_SPI_PRIO17		GIC_SPI_PRIO(17)   
#define GIC_SPI_PRIO18		GIC_SPI_PRIO(18)   
#define GIC_SPI_PRIO19		GIC_SPI_PRIO(19)   
#define GIC_SPI_PRIO20		GIC_SPI_PRIO(20)   
#define GIC_SPI_PRIO21		GIC_SPI_PRIO(21)   
#define GIC_SPI_PRIO22		GIC_SPI_PRIO(22)   
#define GIC_SPI_PRIO23		GIC_SPI_PRIO(23)   
#define GIC_SPI_PRIO24		GIC_SPI_PRIO(24)   
#define GIC_SPI_PRIO25		GIC_SPI_PRIO(25)   
#define GIC_SPI_PRIO26		GIC_SPI_PRIO(26)   
#define GIC_SPI_PRIO27		GIC_SPI_PRIO(27)   
#define GIC_SPI_PRIO28		GIC_SPI_PRIO(28)   
#define GIC_SPI_PRIO29		GIC_SPI_PRIO(29)   
#define GIC_SPI_PRIO30		GIC_SPI_PRIO(30)   
#define GIC_SPI_PRIO31		GIC_SPI_PRIO(31)   

#define GIC_SGI_PROC_TARG(_n)	(ARMA9_GIC_BASE + 0x800 + 4 * (_n))
#define GIC_SGI_PROC_TARG0		GIC_SGI_PROC_TARG(0)
#define GIC_SGI_PROC_TARG1		GIC_SGI_PROC_TARG(1)   
#define GIC_SGI_PROC_TARG2		GIC_SGI_PROC_TARG(2)   
#define GIC_SGI_PROC_TARG3		GIC_SGI_PROC_TARG(3)   

#define GIC_PPI_PROC_TARG(_n)	(ARMA9_GIC_BASE + 0x810 + 4 * (_n))
#define GIC_PPI_PROC_TARG0		GIC_PPI_PROC_TARG(0)
#define GIC_PPI_PROC_TARG1		GIC_PPI_PROC_TARG(1)   
#define GIC_PPI_PROC_TARG2		GIC_PPI_PROC_TARG(2)   
#define GIC_PPI_PROC_TARG3		GIC_PPI_PROC_TARG(3)   

#define GIC_SPI_PROC_TARG(_n)	(ARMA9_GIC_BASE + 0x820 + 4 * (_n))
#define GIC_SPI_PROC_TARG0 		GIC_SPI_PROC_TARG(0 )
#define GIC_SPI_PROC_TARG1 		GIC_SPI_PROC_TARG(1 )  
#define GIC_SPI_PROC_TARG2 		GIC_SPI_PROC_TARG(2 )  
#define GIC_SPI_PROC_TARG3 		GIC_SPI_PROC_TARG(3 )  
#define GIC_SPI_PROC_TARG4 		GIC_SPI_PROC_TARG(4 )  
#define GIC_SPI_PROC_TARG5 		GIC_SPI_PROC_TARG(5 )  
#define GIC_SPI_PROC_TARG6 		GIC_SPI_PROC_TARG(6 )  
#define GIC_SPI_PROC_TARG7 		GIC_SPI_PROC_TARG(7 )  
#define GIC_SPI_PROC_TARG8 		GIC_SPI_PROC_TARG(8 )  
#define GIC_SPI_PROC_TARG9 		GIC_SPI_PROC_TARG(9 )  
#define GIC_SPI_PROC_TARG10		GIC_SPI_PROC_TARG(10)  
#define GIC_SPI_PROC_TARG11		GIC_SPI_PROC_TARG(11)  
#define GIC_SPI_PROC_TARG12		GIC_SPI_PROC_TARG(12)  
#define GIC_SPI_PROC_TARG13		GIC_SPI_PROC_TARG(13)  
#define GIC_SPI_PROC_TARG14		GIC_SPI_PROC_TARG(14)  
#define GIC_SPI_PROC_TARG15		GIC_SPI_PROC_TARG(15)  
#define GIC_SPI_PROC_TARG16		GIC_SPI_PROC_TARG(16)  
#define GIC_SPI_PROC_TARG17		GIC_SPI_PROC_TARG(17)  
#define GIC_SPI_PROC_TARG18		GIC_SPI_PROC_TARG(18)  
#define GIC_SPI_PROC_TARG19		GIC_SPI_PROC_TARG(19)  
#define GIC_SPI_PROC_TARG20		GIC_SPI_PROC_TARG(20)  
#define GIC_SPI_PROC_TARG21		GIC_SPI_PROC_TARG(21)  
#define GIC_SPI_PROC_TARG22		GIC_SPI_PROC_TARG(22)  
#define GIC_SPI_PROC_TARG23		GIC_SPI_PROC_TARG(23)  
#define GIC_SPI_PROC_TARG24		GIC_SPI_PROC_TARG(24)  
#define GIC_SPI_PROC_TARG25		GIC_SPI_PROC_TARG(25)  
#define GIC_SPI_PROC_TARG26		GIC_SPI_PROC_TARG(26)  
#define GIC_SPI_PROC_TARG27		GIC_SPI_PROC_TARG(27)  
#define GIC_SPI_PROC_TARG28		GIC_SPI_PROC_TARG(28)  
#define GIC_SPI_PROC_TARG29		GIC_SPI_PROC_TARG(29)  
#define GIC_SPI_PROC_TARG30		GIC_SPI_PROC_TARG(30)  
#define GIC_SPI_PROC_TARG31		GIC_SPI_PROC_TARG(31)  

#define GIC_IRQ_MOD_CFG(_n)		(ARMA9_GIC_BASE + 0xc00 + 4 * (_n))
#define GIC_IRQ_MOD_CFG0		GIC_IRQ_MOD_CFG(0)	
#define GIC_IRQ_MOD_CFG1		GIC_IRQ_MOD_CFG(1)  
#define GIC_IRQ_MOD_CFG2		GIC_IRQ_MOD_CFG(2)  
#define GIC_IRQ_MOD_CFG3		GIC_IRQ_MOD_CFG(3)  
#define GIC_IRQ_MOD_CFG4		GIC_IRQ_MOD_CFG(4)  
#define GIC_IRQ_MOD_CFG5		GIC_IRQ_MOD_CFG(5)  
#define GIC_IRQ_MOD_CFG6		GIC_IRQ_MOD_CFG(6)  
#define GIC_IRQ_MOD_CFG7		GIC_IRQ_MOD_CFG(7)  
#define GIC_IRQ_MOD_CFG8		GIC_IRQ_MOD_CFG(8)  
#define GIC_IRQ_MOD_CFG9		GIC_IRQ_MOD_CFG(9)  

#define GIC_SOFT_IRQ_GEN		(ARMA9_GIC_BASE + 0xf00)
#define GIC_SGI_PEND_SET(_n)	(ARMA9_GIC_BASE + 0xf10 + 4 * (_n))
#define GIC_SGI_PEND_SET0		GIC_SGI_PEND_SET(0)	
#define GIC_SGI_PEND_SET1		GIC_SGI_PEND_SET(1)	
#define GIC_SGI_PEND_SET2		GIC_SGI_PEND_SET(2)	
#define GIC_SGI_PEND_SET3		GIC_SGI_PEND_SET(3)	
#define GIC_SGI_PEND_CLR(_n)	(ARMA9_GIC_BASE + 0xf10 + 4 * (_n))
#define GIC_SGI_PEND_CLR0		GIC_SGI_PEND_CLR(0)	
#define GIC_SGI_PEND_CLR1		GIC_SGI_PEND_CLR(1)	
#define GIC_SGI_PEND_CLR2		GIC_SGI_PEND_CLR(2)	
#define GIC_SGI_PEND_CLR3		GIC_SGI_PEND_CLR(3)	


#define GIC_CPU_IF_CTRL			(ARMA9_CPUIF_BASE + 0x000)
#define GIC_INT_PRIO_MASK		(ARMA9_CPUIF_BASE + 0x004)
#define GIC_BINARY_POINT		(ARMA9_CPUIF_BASE + 0x008)
#define GIC_INT_ACK_REG			(ARMA9_CPUIF_BASE + 0x00c)
#define GIC_END_INT_REG			(ARMA9_CPUIF_BASE + 0x010)
#define GIC_RUNNING_PRIO		(ARMA9_CPUIF_BASE + 0x014)
#define GIC_HIGHEST_PENDINT		(ARMA9_CPUIF_BASE + 0x018)
#define GIC_DEACT_INT_REG		(ARMA9_CPUIF_BASE + 0x1000)// 0x1000

/* gic source list */
/* software generated interrupt */
#define GIC_SRC_SGI(_n)		(_n)
#define GIC_SRC_SGI0		GIC_SRC_SGI(0 ) 
#define GIC_SRC_SGI1        GIC_SRC_SGI(1 ) 
#define GIC_SRC_SGI2        GIC_SRC_SGI(2 ) 
#define GIC_SRC_SGI3        GIC_SRC_SGI(3 ) 
#define GIC_SRC_SGI4        GIC_SRC_SGI(4 ) 
#define GIC_SRC_SGI5        GIC_SRC_SGI(5 ) 
#define GIC_SRC_SGI6        GIC_SRC_SGI(6 ) 
#define GIC_SRC_SGI7        GIC_SRC_SGI(7 ) 
#define GIC_SRC_SGI8        GIC_SRC_SGI(8 ) 
#define GIC_SRC_SGI9        GIC_SRC_SGI(9 ) 
#define GIC_SRC_SGI10       GIC_SRC_SGI(10) 
#define GIC_SRC_SGI11       GIC_SRC_SGI(11) 
#define GIC_SRC_SGI12       GIC_SRC_SGI(12) 
#define GIC_SRC_SGI13       GIC_SRC_SGI(13) 
#define GIC_SRC_SGI14       GIC_SRC_SGI(14) 
#define GIC_SRC_SGI15       GIC_SRC_SGI(15) 
/* private peripheral interrupt */
#define GIC_SRC_PPI(_n)		(16 + (_n))
#define GIC_SRC_PPI0		GIC_SRC_PPI(0 ) 
#define GIC_SRC_PPI1        GIC_SRC_PPI(1 ) 
#define GIC_SRC_PPI2        GIC_SRC_PPI(2 ) 
#define GIC_SRC_PPI3        GIC_SRC_PPI(3 ) 
#define GIC_SRC_PPI4        GIC_SRC_PPI(4 ) 
#define GIC_SRC_PPI5        GIC_SRC_PPI(5 ) 
#define GIC_SRC_PPI6        GIC_SRC_PPI(6 ) 
#define GIC_SRC_PPI7        GIC_SRC_PPI(7 ) 
#define GIC_SRC_PPI8        GIC_SRC_PPI(8 ) 
#define GIC_SRC_PPI9        GIC_SRC_PPI(9 ) 
#define GIC_SRC_PPI10       GIC_SRC_PPI(10) 
#define GIC_SRC_PPI11       GIC_SRC_PPI(11) 
#define GIC_SRC_PPI12       GIC_SRC_PPI(12) 
#define GIC_SRC_PPI13       GIC_SRC_PPI(13) 
#define GIC_SRC_PPI14       GIC_SRC_PPI(14) 
#define GIC_SRC_PPI15       GIC_SRC_PPI(15) 
/* external peripheral interrupt */
#define GIC_SRC_SPI(_n)		(32 + (_n))


#endif
