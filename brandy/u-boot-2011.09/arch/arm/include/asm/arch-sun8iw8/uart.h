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

#ifndef _UART_H_
#define _UART_H_

#define CCM_UART_PORT_OFFSET          16
#define CCM_UART_ADDR_OFFSET          0x400

typedef struct serial_hw
{
	volatile unsigned int rbr;		/* 0 */
	volatile unsigned int ier;		/* 1 */
	volatile unsigned int fcr;		/* 2 */
	volatile unsigned int lcr;		/* 3 */
	volatile unsigned int mcr;		/* 4 */
	volatile unsigned int lsr;		/* 5 */
	volatile unsigned int msr;		/* 6 */
	volatile unsigned int sch;		/* 7 */
}serial_hw_t;

#define thr rbr
#define dll rbr
#define dlh ier
#define iir fcr



#define   UART_BAUD    115200     
#define   PARITY       0          
#define   STOP         0          
#define   DLEN         3          


void sunxi_serial_init(int uart_port, void *gpio_cfg, int gpio_max);
void sunxi_serial_putc (char c);
char sunxi_serial_getc (void);
int sunxi_serial_tstc (void);


#endif    /*  #ifndef _UART_H_  */
