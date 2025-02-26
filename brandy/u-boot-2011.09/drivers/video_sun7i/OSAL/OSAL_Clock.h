/*
*************************************************************************************
*                         			eBsp
*					   Operation System Adapter Layer
*
*				(c) Copyright 2006-2010, All winners Co,Ld.
*							All	Rights Reserved
*
* File Name 	: OSAL_Clock.h
*
* Author 		: javen
*
* Description 	: 操作系统适配层
*
* History 		:
*      <author>    		<time>       	<version >    		<desc>
*       javen     	   	2010-09-07          1.0         create this word
*		holi			2010-12-03			1.1			实现了具体的接口
*************************************************************************************
*/

#ifndef  __OSAL_CLOCK_H__
#define  __OSAL_CLOCK_H__

#include "OSAL.h"

/* define module clock id       */
typedef enum
{
	CLK_NONE = 0,

	SYS_CLK_PLL3 = 1,
	SYS_CLK_PLL7 = 2,
	SYS_CLK_PLL8 = 3,
	SYS_CLK_PLL9 = 4,
	SYS_CLK_PLL10 = 5,
	SYS_CLK_PLL3X2 = 6,
	SYS_CLK_PLL6 = 7,
	SYS_CLK_PLL6x2 = 8,
	SYS_CLK_PLL7X2 = 9,
	SYS_CLK_MIPIPLL = 10,
	SYS_CLK_PLL5P =11,

	MOD_CLK_DEBE0 = 16,
	MOD_CLK_DEBE1 = 17,
	MOD_CLK_DEFE0 = 18,
	MOD_CLK_DEFE1 = 19,
	MOD_CLK_LCD0CH0 = 20,
	MOD_CLK_LCD0CH1 = 21,
	MOD_CLK_LCD1CH0 = 22,
	MOD_CLK_LCD1CH1 = 23,
	MOD_CLK_HDMI = 24,
	MOD_CLK_HDMI_DDC = 25,
	MOD_CLK_MIPIDSIS = 26,
	MOD_CLK_MIPIDSIP = 27,
	MOD_CLK_IEPDRC0 = 28,
	MOD_CLK_IEPDRC1 = 29,
	MOD_CLK_IEPDEU0 = 30,
	MOD_CLK_IEPDEU1 = 31,
	MOD_CLK_LVDS = 32,
	MOD_CLK_LCD0CH1_S1=33,
	MOD_CLK_LCD1CH1_S1=34,
	MOD_CLK_LCD0CH1_S2=35,
	MOD_CLK_LCD1CH1_S2=36,

	AHB_CLK_MIPIDSI = 48,
	AHB_CLK_LCD0 = 49,
	AHB_CLK_LCD1 = 50,
	AHB_CLK_HDMI = 51,
	AHB_CLK_DEBE0 =52,
	AHB_CLK_DEBE1 =53,
	AHB_CLK_DEFE0 =54,
	AHB_CLK_DEFE1 = 55,
	AHB_CLK_DEU0 = 56,
	AHB_CLK_DEU1 = 57,
	AHB_CLK_DRC0 = 58,
	AHB_CLK_DRC1 = 59,
	AHB_CLK_TVE0 = 60,
	AHB_CLK_TVE1 = 61,

	DRAM_CLK_DRC0 = 80,
	DRAM_CLK_DRC1 = 81,
	DRAM_CLK_DEU0 = 82,
	DRAM_CLK_DEU1 = 83,
	DRAM_CLK_DEFE0 = 84,
	DRAM_CLK_DEFE1 = 85,
	DRAM_CLK_DEBE0 = 86,
	DRAM_CLK_DEBE1 = 87,
}__disp_clk_id_t;

#if (defined (CONFIG_ARCH_SUN7I) || (defined CONFIG_ARCH_SUN5I))

#define CLK_BE_SRC SYS_CLK_PLL5P
#define CLK_LCD_CH0_SRC SYS_CLK_PLL7
#else 
#define CLK_BE_SRC SYS_CLK_PLL10
#define CLK_LCD_CH0_SRC SYS_CLK_MIPIPLL

#endif

#if ((defined CONFIG_ARCH_SUN8IW1P1)||(defined CONFIG_ARCH_SUN7I) || (defined CONFIG_ARCH_SUN5I))
#define CLK_LCD_CH1_SRC SYS_CLK_PLL7
#define CLK_FE_SRC SYS_CLK_PLL7
#define CLK_DSI_SRC SYS_CLK_PLL7
/* 0:pll3, 1:pll7 */
#define CLK_MIPI_PLL_SRC 1
#elif (defined(CONFIG_ARCH_SUN8IW3P1) || defined(CONFIG_ARCH_SUN9IW1P1))
#define CLK_FE_SRC SYS_CLK_PLL10
#define CLK_LCD_CH1_SRC SYS_CLK_PLL3
#define CLK_DSI_SRC SYS_CLK_PLL3
/* 0:pll3, 1:pll7 */
#define CLK_MIPI_PLL_SRC 0
#endif
#ifndef __OSAL_CLOCK_MASK__
#define RESET_OSAL
#define RST_INVAILD 0
#define RST_VAILD   1
#endif

typedef struct
{
	__disp_clk_id_t       id;     /* clock id         */
	char        *name;  /* clock name       */
	struct clk  *hdl;
}__disp_clk_t;

/*
*********************************************************************************************************
*                                   SET SOURCE CLOCK FREQUENCY
*
* Description: 
*		set source clock frequency;
*
* Arguments  : 
*		nSclkNo  	:	source clock number;
*       nFreq   	:	frequency, the source clock will change to;
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
__s32 OSAL_CCMU_SetSrcFreq(__u32 nSclkNo, __u32 nFreq);



/*
*********************************************************************************************************
*                                   GET SOURCE CLOCK FREQUENCY
*
* Description: 
*		get source clock frequency;
*
* Arguments  : 
*		nSclkNo  	:	source clock number need get frequency;
*
* Returns    : 
*		frequency of the source clock;
*
* Note       :
*********************************************************************************************************
*/
__u32 OSAL_CCMU_GetSrcFreq(__u32 nSclkNo);



/*
*********************************************************************************************************
*                                   OPEN MODULE CLK
* Description: 
*		open module clk;
*
* Arguments  : 
*		nMclkNo	:	number of module clock which need be open;
*
* Returns    : 
*		EBSP_TRUE/EBSP_FALSE
*
* Note       :
*********************************************************************************************************
*/
__hdle OSAL_CCMU_OpenMclk(__s32 nMclkNo);


/*
*********************************************************************************************************
*                                    CLOSE MODULE CLK
* Description: 
*		close module clk;
*
* Arguments  : 
*		hMclk	:	handle
*
* Returns    : 
*		EBSP_TRUE/EBSP_FALSE
*
* Note       :
*********************************************************************************************************
*/
__s32  OSAL_CCMU_CloseMclk(__hdle hMclk);

/*
*********************************************************************************************************
*                                   GET MODULE SRC
* Description: 
*		set module src;
*
* Arguments  : 
*		nMclkNo	:	number of module clock which need be open;
*       nSclkNo	:	call-back function for process clock change;
*
* Returns    : 
*		EBSP_TRUE/EBSP_FALSE
*
* Note       :
*********************************************************************************************************
*/
__s32 OSAL_CCMU_SetMclkSrc(__hdle hMclk, __u32 nSclkNo);





/*
*********************************************************************************************************
*                                  GET MODULE SRC
*
* Description: 
*		get module src;
*
* Arguments  : 
*		nMclkNo	:	handle of the module clock;
*
* Returns    : 
*		src no
*
* Note       :
*********************************************************************************************************
*/
__s32 OSAL_CCMU_GetMclkSrc(__hdle hMclk);




/*
*********************************************************************************************************
*                                   SET MODUEL CLOCK FREQUENCY
*
* Description: 
*		set module clock frequency;
*
* Arguments  : 
*		nSclkNo  :	number of source clock which the module clock will use;
*		nDiv     :	division for the module clock;
*
* Returns    : 
*		EBSP_TRUE/EBSP_FALSE
*
* Note       :
*********************************************************************************************************
*/
__s32 OSAL_CCMU_SetMclkDiv(__hdle hMclk, __s32 nDiv);



/*
*********************************************************************************************************
*                                   GET MODUEL CLOCK FREQUENCY
*
* Description: 
*		get module clock requency;
*
* Arguments  : 
*		hMclk    	:	module clock handle;
*
* Returns    : 
*		frequency of the module clock;
*
* Note       :
*********************************************************************************************************
*/
__u32 OSAL_CCMU_GetMclkDiv(__hdle hMclk);



/*
*********************************************************************************************************
*                                   MODUEL CLOCK ON/OFF
*
* Description: 
*		module clock on/off;
*
* Arguments  : 
*		nMclkNo		:	module clock handle;
*       bOnOff   	:	on or off;
*
* Returns    : 
*		EBSP_TRUE/EBSP_FALSE
*
* Note       :
*********************************************************************************************************
*/
__s32 OSAL_CCMU_MclkOnOff(__hdle hMclk, __s32 bOnOff);

__s32 OSAL_CCMU_MclkReset(__hdle hMclk, __s32 bReset);


/*
__s32  esCLK_SetSrcFreq(__s32 nSclkNo, __u32 nFreq);
__u32  esCLK_GetSrcFreq(__s32 nSclkNo);

__hdle esCLK_OpenMclk(__s32 nMclkNo, __pCB_ClkCtl_t pCb);
__s32  esCLK_CloseMclk(__hdle hMclk);

__s32  esCLK_SetMclkSrc(__s32 nMclkNo, __s32 nSclkNo);
__s32  esCLK_GetMclkSrc(__s32 nMclkNo);

__s32  esCLK_SetMclkDiv(__s32 nMclkNo, __s32 nDiv);
__u32  esCLK_GetMclkDiv(__s32 nMclkNo);

__s32  esCLK_MclkOnOff(__s32 nMclkNo, __s32 bOnOff);


__s32 esCLK_reg_cb(__s32 nMclkNo, __pCB_ClkCtl_t pCb);
__s32  esCLK_unreg_cb(__s32 nMclkNo);				


					__s32  esCLK_SetSrcFreq(__s32 nSclkNo, __u32 nFreq);
					__u32  esCLK_GetSrcFreq(__s32 nSclkNo);


__hdle esCLK_OpenMclk(__s32 nMclkNo);
__s32  esCLK_CloseMclk(__hdle hMclk);



__s32  esCLK_SetMclkSrc(__hdle hMclk, __s32 nSclkNo);
__s32  esCLK_GetMclkSrc(__hdle hMclk);				

__s32  esCLK_SetMclkDiv(__hdle hMclk, __s32 nDiv);
__u32  esCLK_GetMclkDiv(__hdle hMclk);

__s32  esCLK_MclkOnOff(__hdle hMclk, __s32 bOnOff);


*/

#endif  

