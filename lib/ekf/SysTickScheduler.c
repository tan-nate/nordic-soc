/* SysTick scheduler
 * 
 * Copyright 2013-2023 The MathWorks, Inc.
 */
 
#include "arm_cortex_m_multitasking.h"

unsigned long schdl_counter;
unsigned long schdl_counter_max;

extern volatile uint32_t taskToSchedule;

static void SysTick_Isr(void)
{
    __DSB();
    __ISB();
	
	if (schdl_counter >= schdl_counter_max)
	{
		schdl_counter = 0;
	}
	
    if (0 == schdl_counter++)	/* Compare with zero before increment */
	{
		/* The rt_OneStep() function expects interrupts to be disabled. 
           Hence disable interrupt before rt_OneStep(). */
        __disable_irq();
        rt_OneStep();
        /* The rt_OneStep() function disables interrupt. 
           Hence enable interrupt immediately after rt_OneStep(). */
        __enable_irq();
	}
}

#if (MW_ARMv6M_AND_PLUS == 0)
    void SysTick_Handler(){
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
#else
MW_RENTRANT_ISR_FUNCTION_BEGIN(SysTick_Handler, -1)
    MW_CALL_ISR_FUNCTION(SysTick_Isr);
MW_RENTRANT_ISR_FUNCTION_END()
#endif

void ARMCM_SysTick_Config(float modelBaseRate)
{
#if !defined(MW_DONT_USE_SYSTICK)    
#if defined(MW_SCHEDULER_INTERRUPT_SOURCE) && (MW_SCHEDULER_INTERRUPT_SOURCE == 0)
	/* SysTick Scheduler is selected */
	unsigned long tick_counter;
	
	tick_counter = (unsigned long)(SystemCoreClock * modelBaseRate);
	
	if (tick_counter > SysTick_LOAD_RELOAD_Msk)
	{
		schdl_counter_max = (unsigned long)((float)tick_counter / (float)SysTick_LOAD_RELOAD_Msk) + 1;
		SysTick_Config(tick_counter/schdl_counter_max);		
	}
	else
	{
		schdl_counter_max = 0;
		SysTick_Config(tick_counter);
	}
	
	schdl_counter = 0;
	
	/* Set the SysTick priority */
	#ifdef MW_SCHEDULER_PRIORITY
		NVIC_SetPriority((IRQn_Type)(-1), MW_SCHEDULER_PRIORITY);
	#endif
    #if (MW_ARMv6M_AND_PLUS == 0)
        taskToSchedule = (uint32_t) SysTick_Isr;
    #endif
#endif
#endif
    NVIC_SetPriority((IRQn_Type)(-5), MW_MAX_PRIORITY_VALUE);
    NVIC_SetPriority((IRQn_Type)(-2), MW_MAX_PRIORITY_VALUE);
}

void ARMCM_SysTick_Stop(void)
{
#if !defined(MW_DONT_USE_SYSTICK)    
#if defined(MW_SCHEDULER_INTERRUPT_SOURCE) && (MW_SCHEDULER_INTERRUPT_SOURCE == 0)
	/* SysTick Scheduler is Stopped */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
#endif
#endif
}