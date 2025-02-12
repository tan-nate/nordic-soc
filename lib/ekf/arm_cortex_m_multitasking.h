/* Wrapper for Multi tasking.
 * 
 * Copyright 2013-2020 The MathWorks, Inc.
 */
 
/* Header file for multitasking */
#ifndef _ARM_CORTEX_M_MULTITASKING_
#define _ARM_CORTEX_M_MULTITASKING_

#ifdef __MW_TARGET_USE_HARDWARE_RESOURCES_H__
 #include "MW_target_hardware_resources.h"
#endif

#define MW_MAX_PRIORITY_VALUE ((0x01 << __NVIC_PRIO_BITS) - 1)

#if defined (__GNUC__)
	#define MW_ASM	__asm volatile
#elif defined (__TMS470__)
	#define MW_ASM	__asm
#endif


/* Inline syntax */
#if defined (__GNUC__)
    #define MW_ASM    __asm volatile
  #define MW_STATIC_INLINE  static inline
  #define MW_NAKED_ATTRIBUTE  __attribute__ ((naked))
#elif defined (__TMS470__)
    #define MW_ASM    __asm
  #define MW_STATIC_INLINE  static inline
  #define MW_NAKED_ATTRIBUTE  
#endif


/* TI ARM Compiler equivalents to CMSIS */
#if defined (__TMS470__)
unsigned long  __get_BASEPRI(void);
void __set_BASEPRI(unsigned long basePri);

#define __DSB() __asm (" DSB\n");
#define __ISB() __asm (" ISB\n");

#endif

#if (__CORTEX_M == 0) || (__CORTEX_M == 1) || (__CORTEX_M == 23)
    #define MW_ARMv6M_AND_PLUS 1U
#else
    #define MW_ARMv6M_AND_PLUS 0U
#endif

/* BASEPRI register is available from Cortex-M3 */
/* The preprocessor __CORTEX_M comes from CMSIS files.  If not, __CORTEX_M should be explicitly defined based on the Cortex-M architecture of the processor. */
#if (MW_ARMv6M_AND_PLUS == 0) 
 #define ARM_CORTEX_M_PUSH_BASEPRI()  MW_ASM (" MRS R0, BASEPRI \n"      \
                                              " PUSH {R0, R1}   \n");        /* Dummy R1 for alignment */
#else
 #define ARM_CORTEX_M_PUSH_BASEPRI()
#endif


/* BASEPRI register management to suppress lower priority tasks */
#if (MW_ARMv6M_AND_PLUS == 0)
/* Function to block lower priority tasks */
MW_STATIC_INLINE void mw_blockLowerPriorityTasks(unsigned long isrPri)
{
    isrPri = (isrPri+1) << (8U - __NVIC_PRIO_BITS);
    
    __set_BASEPRI(isrPri);
}
#else
/* For M0, M0+ not managed.  Hence, the timer task should be running at lowest priority always */
#define mw_blockLowerPriorityTasks(x)
#endif


/* In GCC, functions with 'naked' attributes doesn't include prolog. */ 
/* For TI ARM toolchain, the below prolog is added only in DEBUG (-g) mode. This prolog needs to be 
   added in non-debug mode. */
/*This macro should be called before calling arm_cortex_m_call_thread_with_context_switch(). */
#if defined (__GNUC__) //|| (defined (__TMS470__) && !defined (_DEBUG))
#define ARM_CORTEX_M_PUSH_THREADS_PSR_AND_RET_ADDR()  MW_ASM (" MRS R0, APSR    \n"      \
                                                              " PUSH {R0, LR}   \n");
#else
#define ARM_CORTEX_M_PUSH_THREADS_PSR_AND_RET_ADDR()
#endif


/* Preamble for Reentrant ISR */
#if defined(MW_MULTI_TASKING_MODE) && (MW_MULTI_TASKING_MODE == 1)
#define MW_RENTRANT_ISR_FUNCTION_BEGIN(MW_ISR_NAME, MW_IRQ_NUMBER)            \
MW_NAKED_ATTRIBUTE void MW_ISR_NAME(void) {                                   \
    ARM_CORTEX_M_PUSH_THREADS_PSR_AND_RET_ADDR();                             \
    ARM_CORTEX_M_PUSH_BASEPRI();                                              \
    mw_blockLowerPriorityTasks(NVIC_GetPriority((IRQn_Type)(MW_IRQ_NUMBER)));

#define MW_CALL_ISR_FUNCTION(MW_ISR_FUNCTION_NAME)                            \
    arm_cortex_m_call_thread_with_context_switch(MW_ISR_FUNCTION_NAME);
#else
#define MW_RENTRANT_ISR_FUNCTION_BEGIN(MW_ISR_NAME, MW_IRQ_NUMBER)            \
void MW_ISR_NAME(void) {

#define MW_CALL_ISR_FUNCTION(MW_ISR_FUNCTION_NAME)                            \
    MW_ISR_FUNCTION_NAME();
#endif

/* Postable for Reentrant ISR */
#define MW_RENTRANT_ISR_FUNCTION_END()                \
}

                                                              
#ifdef __cplusplus
extern "C" {
#endif                                                                      
    void arm_cortex_m_call_thread_with_context_switch(void (*isr_routine_ptr)(void));
#ifdef __cplusplus
}
#endif   

#endif /* _ARM_CORTEX_M_MULTITASKING_ */

/*
 * LocalWords:  DSB ISB BASEPRI prolog toolchain TMS APSR ISR Postable
 */
