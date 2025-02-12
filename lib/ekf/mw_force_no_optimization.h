/* Forcing optimization to -O0
 * 
 * Copyright 2015-2022 The MathWorks, Inc.
 */
 
/* Header file for multitasking */
#ifndef _MW_FORCE_NO_OPTIMIZATION_H_
#define _MW_FORCE_NO_OPTIMIZATION_H_

#if defined (__GNUC__) && !defined (__clang__)
	#if defined (__TI_GNU_ATTRIBUTE_SUPPORT__)
		#pragma FUNCTION_OPTIONS ( arm_cortex_m_call_thread_with_context_switch, "--opt_level=0" )
		#pragma FUNCTION_OPTIONS ( SysTick_Handler, "--opt_level=0" )
		#pragma FUNCTION_OPTIONS ( SVC_Handler, "--opt_level=0" )		
	#else
		/* Forcing the optimization level to -O0 due to the bug as described 
       in https://bugs.launchpad.net/gcc-arm-embedded/+bug/1447063 */
		#pragma GCC optimize ("O0")
	#endif	   
#endif

#endif
