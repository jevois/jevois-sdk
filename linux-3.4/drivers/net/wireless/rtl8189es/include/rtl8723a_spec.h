/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *******************************************************************************/
#ifndef __RTL8723A_SPEC_H__
#define __RTL8723A_SPEC_H__

#include <rtl8192c_spec.h>


#define HAL_8723A_NAV_UPPER_UNIT	128	

#define REG_SYSON_REG_LOCK		0x001C


#define REG_FTIMR			0x0138








#define REG_EARLY_MODE_CONTROL		0x4D0



#define DIS_ATIM					BIT(0)
#define DIS_BCNQ_SUB				BIT(1)
#define DIS_TSF_UDT					BIT(4)


#define	REG_NAV_UPPER			0x0652







#define EN_WLON			BIT(16)






#define WLOCK_ALL		BIT(0)
#define WLOCK_00		BIT(1)
#define WLOCK_04		BIT(2)
#define WLOCK_08		BIT(3)
#define WLOCK_40		BIT(4)
#define WLOCK_1C_B6		BIT(5)
#define R_DIS_PRST_1		BIT(6)
#define LOCK_ALL_EN		BIT(7)

















#define HSIMR_GPIO12_0_INT_EN	BIT(0)
#define HSIMR_SPS_OCP_INT_EN	BIT(5)
#define HSIMR_RON_INT_EN		BIT(6)
#define HSIMR_PDNINT_EN		BIT(7)
#define HSIMR_GPIO9_INT_EN		BIT(25)

#define HSISR_GPIO12_0_INT		BIT(0)
#define HSISR_SPS_OCP_INT		BIT(5)
#define HSISR_RON_INT			BIT(6)
#define HSISR_PDNINT			BIT(7)
#define	HSISR_GPIO9_INT			BIT(25)

#define MASK_HSISR_CLEAR		(HSISR_GPIO12_0_INT |\
								HSISR_SPS_OCP_INT |\
								HSISR_RON_INT |\
								HSISR_PDNINT |\
								HSISR_GPIO9_INT)

#define RAM_DL_SEL				BIT7


#define RTL_ID					BIT(23)
#define SPS_SEL					BIT(24)




#define CALTMR_EN					BIT(10)















































#define SDIO_LOCAL_BASE				0x10250000
#define WLAN_IOREG_BASE				0x10260000
#define FIRMWARE_FIFO_BASE			0x10270000
#define TX_HIQ_BASE				0x10310000
#define TX_MIQ_BASE				0x10320000
#define TX_LOQ_BASE				0x10330000
#define RX_RX0FF_BASE				0x10340000

#define SDIO_LOCAL_MSK				0x0FFF
#define WLAN_IOREG_MSK				0x7FFF
#define WLAN_FIFO_MSK		      		0x1FFF
#define WLAN_RX0FF_MSK			      	0x0003

#define SDIO_WITHOUT_REF_DEVICE_ID		0
#define SDIO_LOCAL_DEVICE_ID			0
#define WLAN_TX_HIQ_DEVICE_ID			4
#define WLAN_TX_MIQ_DEVICE_ID			5
#define WLAN_TX_LOQ_DEVICE_ID			6
#define WLAN_RX0FF_DEVICE_ID			7
#define WLAN_IOREG_DEVICE_ID			8

#define HI_QUEUE_IDX				0
#define MID_QUEUE_IDX				1
#define LOW_QUEUE_IDX				2
#define PUBLIC_QUEUE_IDX			3

#define SDIO_MAX_TX_QUEUE			3	
#define SDIO_MAX_RX_QUEUE			1

#define SDIO_REG_TX_CTRL			0x0000
#define SDIO_REG_HIMR				0x0014
#define SDIO_REG_HISR				0x0018
#define SDIO_REG_HCPWM				0x0019
#define SDIO_REG_RX0_REQ_LEN			0x001C
#define SDIO_REG_FREE_TXPG			0x0020
#define SDIO_REG_HCPWM1				0x0024
#define SDIO_REG_HCPWM2				0x0026
#define SDIO_REG_HTSFR_INFO			0x0030
#define SDIO_REG_HRPWM1				0x0080
#define SDIO_REG_HRPWM2				0x0082
#define SDIO_REG_HPS_CLKR			0x0084
#define SDIO_REG_HSUS_CTRL			0x0086
#define SDIO_REG_HIMR_ON			0x0090
#define SDIO_REG_HISR_ON			0x0091

#define SDIO_HIMR_DISABLED			0

#define SDIO_HIMR_RX_REQUEST_MSK		BIT0
#define SDIO_HIMR_AVAL_MSK			BIT1
#define SDIO_HIMR_TXERR_MSK			BIT2
#define SDIO_HIMR_RXERR_MSK			BIT3
#define SDIO_HIMR_TXFOVW_MSK			BIT4
#define SDIO_HIMR_RXFOVW_MSK			BIT5
#define SDIO_HIMR_TXBCNOK_MSK			BIT6
#define SDIO_HIMR_TXBCNERR_MSK			BIT7
#define SDIO_HIMR_BCNERLY_INT_MSK		BIT16
#define SDIO_HIMR_C2HCMD_MSK			BIT17
#define SDIO_HIMR_CPWM1_MSK			BIT18
#define SDIO_HIMR_CPWM2_MSK			BIT19
#define SDIO_HIMR_HSISR_IND_MSK			BIT20
#define SDIO_HIMR_GTINT3_IND_MSK		BIT21
#define SDIO_HIMR_GTINT4_IND_MSK		BIT22
#define SDIO_HIMR_PSTIMEOUT_MSK			BIT23
#define SDIO_HIMR_OCPINT_MSK			BIT24
#define SDIO_HIMR_ATIMEND_MSK			BIT25
#define SDIO_HIMR_ATIMEND_E_MSK			BIT26
#define SDIO_HIMR_CTWEND_MSK			BIT27

#define SDIO_HISR_RX_REQUEST			BIT0
#define SDIO_HISR_AVAL				BIT1
#define SDIO_HISR_TXERR				BIT2
#define SDIO_HISR_RXERR				BIT3
#define SDIO_HISR_TXFOVW			BIT4
#define SDIO_HISR_RXFOVW			BIT5
#define SDIO_HISR_TXBCNOK			BIT6
#define SDIO_HISR_TXBCNERR			BIT7
#define SDIO_HISR_BCNERLY_INT			BIT16
#define SDIO_HISR_C2HCMD			BIT17
#define SDIO_HISR_CPWM1				BIT18
#define SDIO_HISR_CPWM2				BIT19
#define SDIO_HISR_HSISR_IND			BIT20
#define SDIO_HISR_GTINT3_IND			BIT21
#define SDIO_HISR_GTINT4_IND			BIT22
#define SDIO_HISR_PSTIMEOUT			BIT23
#define SDIO_HISR_OCPINT			BIT24
#define SDIO_HISR_ATIMEND			BIT25
#define SDIO_HISR_ATIMEND_E			BIT26
#define SDIO_HISR_CTWEND			BIT27

#define MASK_SDIO_HISR_CLEAR		(SDIO_HISR_TXERR |\
									SDIO_HISR_RXERR |\
									SDIO_HISR_TXFOVW |\
									SDIO_HISR_RXFOVW |\
									SDIO_HISR_TXBCNOK |\
									SDIO_HISR_TXBCNERR |\
									SDIO_HISR_C2HCMD |\
									SDIO_HISR_CPWM1 |\
									SDIO_HISR_CPWM2 |\
									SDIO_HISR_HSISR_IND |\
									SDIO_HISR_GTINT3_IND |\
									SDIO_HISR_GTINT4_IND |\
									SDIO_HISR_PSTIMEOUT |\
									SDIO_HISR_OCPINT)

#define HCI_RESUME_PWR_RDY			BIT1
#define HCI_SUS_CTRL				BIT0

#define SDIO_TX_FREE_PG_QUEUE			4
#define SDIO_TX_FIFO_PAGE_SZ 			128

#define SCR_TxUseBroadcastDK			BIT6
#define SCR_RxUseBroadcastDK			BIT7


#ifdef HWSET_MAX_SIZE
#undef HWSET_MAX_SIZE
#endif
#define HWSET_MAX_SIZE				256


#define	UHIMR_TIMEOUT2					BIT31
#define	UHIMR_TIMEOUT1					BIT30
#define	UHIMR_PSTIMEOUT					BIT29
#define	UHIMR_GTINT4					BIT28
#define	UHIMR_GTINT3					BIT27
#define	UHIMR_TXBCNERR					BIT26
#define	UHIMR_TXBCNOK					BIT25
#define	UHIMR_TSF_BIT32_TOGGLE			BIT24
#define	UHIMR_BCNDMAINT3				BIT23
#define	UHIMR_BCNDMAINT2				BIT22
#define	UHIMR_BCNDMAINT1				BIT21
#define	UHIMR_BCNDMAINT0				BIT20
#define	UHIMR_BCNDOK3					BIT19
#define	UHIMR_BCNDOK2					BIT18
#define	UHIMR_BCNDOK1					BIT17
#define	UHIMR_BCNDOK0					BIT16
#define	UHIMR_HSISR_IND					BIT15
#define	UHIMR_BCNDMAINT_E				BIT14
#define	UHIMR_CTW_END					BIT12
#define	UHIMR_C2HCMD					BIT10
#define	UHIMR_CPWM2					BIT9
#define	UHIMR_CPWM					BIT8
#define	UHIMR_HIGHDOK					BIT7	
#define	UHIMR_MGNTDOK					BIT6	
#define	UHIMR_BKDOK					BIT5	
#define	UHIMR_BEDOK					BIT4	
#define	UHIMR_VIDOK						BIT3	
#define	UHIMR_VODOK					BIT2	
#define	UHIMR_RDU						BIT1	
#define	UHIMR_ROK						BIT0	

#define	UHIMR_BCNDMAINT7				BIT23
#define	UHIMR_BCNDMAINT6				BIT22
#define	UHIMR_BCNDMAINT5				BIT21
#define	UHIMR_BCNDMAINT4				BIT20
#define	UHIMR_BCNDOK7					BIT19
#define	UHIMR_BCNDOK6					BIT18
#define	UHIMR_BCNDOK5					BIT17
#define	UHIMR_BCNDOK4					BIT16
#define	UHIMR_ATIMEND_E				BIT13
#define	UHIMR_ATIMEND					BIT12
#define	UHIMR_TXERR						BIT11
#define	UHIMR_RXERR						BIT10
#define	UHIMR_TXFOVW					BIT9
#define	UHIMR_RXFOVW					BIT8
#define	UHIMR_OCPINT					BIT1

#define	REG_USB_HIMR				0xFE38
#define	REG_USB_HIMRE				0xFE3C
#define	REG_USB_HISR					0xFE78
#define	REG_USB_HISRE				0xFE7C

#define	USB_INTR_CPWM_OFFSET		16
#define	USB_INTR_CONTENT_HISR_OFFSET		48
#define	USB_INTR_CONTENT_HISRE_OFFSET		52
#define	USB_INTR_CONTENT_LENGTH			56
#define	USB_C2H_CMDID_OFFSET		0
#define	USB_C2H_SEQ_OFFSET		1
#define	USB_C2H_EVENT_OFFSET		2

#ifdef CONFIG_RF_GAIN_OFFSET
#define	EEPROM_RF_GAIN_OFFSET			0x2F
#define	EEPROM_RF_GAIN_VAL				0x1F6
#endif


#endif

