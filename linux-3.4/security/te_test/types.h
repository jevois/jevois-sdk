/* 
 * 
 * Header for common Data types
 *
 */

#ifndef __TYPES_H__
#define __TYPES_H__

#define	OK	(0)
#define	FAIL	(-1)
#define TRUE	(1)
#define	FALSE	(0)
#define true     1
#define false    0

#ifndef NULL
#define NULL    0
#endif

typedef	void * 		    HANDLE;
typedef unsigned long long  u64;
typedef unsigned int        u32;
typedef unsigned short      u16;
typedef unsigned char       u8;
typedef signed long long    s64;
typedef signed int          s32;
typedef signed short        s16;
typedef signed char         s8;
typedef unsigned int        size_t;

typedef u32             pa_t;
typedef u32             va_t;
typedef unsigned int    irq_flags_t;

typedef s32  (* __pCBK_t)(void *p_arg);	
typedef s32  (* __pISR_t)(void *p_arg);	
typedef void (* __pTASK_t)(void *p_arg);
typedef s32  (* __pNotifier_t)(u32 message, void *arg);//notifer call-back
typedef	s32  (* __pExceptionHandler)(void);		

#endif /* __TYPES_H__ */
