/* Wrapper for Multi tasking.
 * 
 * Copyright 2013-2020 The MathWorks, Inc.
 */


#ifdef __MW_TARGET_USE_HARDWARE_RESOURCES_H__
#include "MW_target_hardware_resources.h"
#endif

#ifndef __NVIC_PRIO_BITS
#error "__NVIC_PRIO_BITS is undefined.  The __NVIC_PRIO_BITS is defined in hardware definition header file."
#endif

#include "mw_force_no_optimization.h"
#include "arm_cortex_m_multitasking.h"

volatile uint32_t taskToSchedule;

__attribute__ ((naked,aligned(4))) void PendSV_Handler() {
    __asm volatile (
    "   MRS R0, APSR                            \n"
    "   PUSH {R0, LR}                           \n"
    "   MRS R0, BASEPRI                         \n"
    "   PUSH {R0, R1}                           \n"
    );
#if (defined(__FPU_PRESENT) && (__FPU_PRESENT == 1)) && (defined(__FPU_USED) && (__FPU_USED == 1))
    __asm volatile (
    "   TST LR, #0x10                           \n"
    "   IT EQ                                   \n"
    "   VMOVEQ S0, S0                           \n"
    );
#endif
    __asm volatile (
    "   LDR R0, =taskToSchedule                 \n"
    "   LDR R0, [R0]                            \n"
    "   PUSH {R0, R1}                           \n"
    "   SUB SP, SP, #0x20                       \n"
    "   ADR R0,Call_isr_routine_in_thread_mode \n"
    "   STR R0,[SP, #24]                        \n"
    "   MOV R0,#0x01000000                      \n"
    "   STR R0,[SP, #28]                        \n"
    "   MVN R0,#0x06                            \n"
    "   MOV LR, R0                              \n"
    "   BX LR                                   \n"
    "Call_isr_routine_in_thread_mode:           \n"
    "   POP {R0, R1}                            \n"
    "   BLX R0                                  \n"
    "   ISB                                     \n"
    "   SVC #0                                  \n"
    "Unknown_Execution:                         \n"
    "   B Unknown_Execution                     \n"
    );
}

/* SVC Interrupt service routine to restore the context: SVC_Handler*/
__attribute__ ((naked,aligned(4))) void SVC_Handler(void) {
#if (defined(__FPU_PRESENT) && (__FPU_PRESENT == 1)) && (defined(__FPU_USED) && (__FPU_USED == 1))
    __asm volatile (
    "   TST  LR, #0x10      \n"
    "   IT EQ               \n"
    "   VMOVEQ.F32 S0, S0   \n"
    );
#endif
    __asm volatile (
    "   TST LR, #0x4            \n"
    "   ITE EQ                  \n"
    "   MRSEQ R0, MSP           \n"
    "   MRSNE R0, PSP           \n"
    "   LDR R1, [R0, #24]       \n"
    "   LDRB R0, [R1, #-2]      \n"
    "   CBZ R0, svc_service_0   \n"
    "   CMP R0, #1              \n"
    "   BEQ svc_service_1       \n"
    "   B Unknown_SVC_Request   \n"
    "svc_service_0:             \n"
    );
#if (defined(__FPU_PRESENT) && (__FPU_PRESENT == 1)) && (defined(__FPU_USED) && (__FPU_USED == 1))
     __asm volatile (
    "   TST LR, #0x10       \n"
    "   ITE EQ              \n"
    "   ADDEQ SP, SP, #104  \n"
    "   ADDNE SP, SP, #32   \n"
     );
#else
     __asm volatile ("     ADD SP, SP, #32");
#endif
     __asm volatile (
    "     POP {R0, R1}              \n"
    "     MSR BASEPRI, R0           \n"
    "     POP {R0, R1}              \n"
    "     MSR APSR_nzcvq, R0        \n"
    "     BX R1                     \n"
    "Unknown_SVC_Request:           \n"
    "     B Unknown_SVC_Request     \n"
    "svc_service_1:                 \n"
    "     MOVW R1, #0x0000ED14      \n"
    "     MOVT R1, #0xE000          \n"
    "     LDR R2,[R1]               \n"
    "     ORR R2, #1                \n"
    "     STR R2,[R1]               \n"
    "     BX LR                     \n"
    "     B Unknown_SVC_Request     \n"
    );
}

/* LocalWords:  NVIC PRIO TST VMOVEQ ADR isr MOV MVNS asm cpsie chaing arival VMOV MVN
 * LocalWords:  BLX ISB SVC ITE MRSEQ MSP MRSNE PSP LDR LDRB CBZ svc CMP BEQ
 * LocalWords:  ADDEQ ADDNE MSR BASEPRI xff APSR nzcvq MOVW MOVT
 */
