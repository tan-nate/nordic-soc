/* SysTick scheduler
 * 
 * Copyright 2013-2020 The MathWorks, Inc.
 */

#ifndef _SYSTICKSCHEDULER_H_
#define _SYSTICKSCHEDULER_H_

extern void rt_OneStep(void);
extern void SysTick_Handler(void);
extern void ARMCM_SysTick_Config(float modelBaseRate);
extern void ARMCM_SysTick_Stop(void);

#endif
