/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: BatterySOCEstimation_rev.h
 *
 * Code generated for Simulink model 'BatterySOCEstimation_rev'.
 *
 * Model version                  : 13.3
 * Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
 * C/C++ source code generated on : Tue Feb 11 21:56:48 2025
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef BatterySOCEstimation_rev_h_
#define BatterySOCEstimation_rev_h_
#ifndef BatterySOCEstimation_rev_COMMON_INCLUDES_
#define BatterySOCEstimation_rev_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                           /* BatterySOCEstimation_rev_COMMON_INCLUDES_ */

#include "BatterySOCEstimation_rev_types.h"
#include <stddef.h>

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block signals for system '<Root>/SOC Estimator (Kalman Filter)' */
typedef struct {
  real_T Gain;                         /* '<S1>/Gain' */
  real_T Conversion[4];                /* '<S11>/Conversion' */
  real_T RateTransition3;              /* '<S3>/Rate Transition3' */
  real_T Conversion_m;                 /* '<S10>/Conversion' */
  real_T TmpSignalConversionAtDelayInpor[2];
  real_T Delay[2];                     /* '<S5>/Delay' */
  real_T RateTransition2;              /* '<S3>/Rate Transition2' */
  real_T R1;                           /* '<S6>/2-D Lookup Table R1' */
  real_T C1;                           /* '<S6>/2-D Lookup Table C1' */
  real_T tau1;                         /* '<S6>/Product' */
  real_T Probe[2];                     /* '<S6>/Probe' */
  real_T Ts;                           /* '<S6>/-Ts' */
  real_T Product2;                     /* '<S6>/Product2' */
  real_T MathFunction;                 /* '<S6>/Math Function' */
  real_T Assignment[4];                /* '<S6>/Assignment' */
  real_T UnitDelayP[4];                /* '<S3>/Unit Delay - P' */
  real_T Transpose[4];                 /* '<S7>/Transpose' */
  real_T Product2_l[4];                /* '<S7>/Product2' */
  real_T Sum1[4];                      /* '<S7>/Sum1' */
  real_T dV0;                          /* '<S6>/2-D Lookup Table dV0' */
  real_T Conversion_g;                 /* '<S13>/Conversion' */
  real_T TmpSignalConversionAtTranspose1[2];
  real_T Transpose1[2];                /* '<S6>/Transpose1' */
  real_T Transpose_a[2];               /* '<S4>/Transpose' */
  real_T Product[2];                   /* '<S4>/Product' */
  real_T Conversion_j;                 /* '<S9>/Conversion' */
  real_T Product2_h;                   /* '<S4>/Product2' */
  real_T Sum2;                         /* '<S4>/Sum2' */
  real_T Divide;                       /* '<S4>/Divide' */
  real_T Kk1[2];                       /* '<S4>/Product1' */
  real_T RateTransition;               /* '<S3>/Rate Transition' */
  real_T V0;                           /* '<S6>/2-D Lookup Table V0' */
  real_T R0;                           /* '<S6>/2-D Lookup Table R0' */
  real_T RateTransition1;              /* '<S3>/Rate Transition1' */
  real_T Product6;                     /* '<S6>/Product6' */
  real_T Sum2_l;                       /* '<S6>/Sum2' */
  real_T Sum;                          /* '<S3>/Sum' */
  real_T Product3[2];                  /* '<S4>/Product3' */
  real_T Product_c[2];                 /* '<S7>/Product' */
  real_T Gain_c;                       /* '<S6>/Gain' */
  real_T Conversion_a;                 /* '<S12>/Conversion' */
  real_T Sum_m;                        /* '<S6>/Sum' */
  real_T Product4;                     /* '<S6>/Product4' */
  real_T TmpSignalConversionAtProduct1In[2];
  real_T Product1[2];                  /* '<S7>/Product1' */
  real_T Sum_o[2];                     /* '<S7>/Sum' */
  real_T Sum_g[2];                     /* '<S4>/Sum' */
  real_T Conversion_o[4];              /* '<S8>/Conversion' */
  real_T Product4_o[4];                /* '<S4>/Product4' */
  real_T Sum3[4];                      /* '<S4>/Sum3' */
  real_T Product5[4];                  /* '<S4>/Product5' */
} B_CoreSubsys_BatterySOCEstima_T;

/* Block states (default storage) for system '<Root>/SOC Estimator (Kalman Filter)' */
typedef struct {
  real_T Delay_DSTATE[2];              /* '<S5>/Delay' */
  real_T UnitDelayP_DSTATE[4];         /* '<S3>/Unit Delay - P' */
  real_T Product2_DWORK1[4];           /* '<S7>/Product2' */
  real_T Divide_DWORK1;                /* '<S4>/Divide' */
  real_T Divide_DWORK3;                /* '<S4>/Divide' */
  real_T Divide_DWORK4;                /* '<S4>/Divide' */
  real_T Divide_DWORK5;                /* '<S4>/Divide' */
  int32_T Divide_DWORK2;               /* '<S4>/Divide' */
  boolean_T icLoad;                    /* '<S5>/Delay' */
} DW_CoreSubsys_BatterySOCEstim_T;

/* Block signals (default storage) */
typedef struct {
  real_T dv[4];
  real_T v1[4];
  real_T ImpAsg_InsertedFor_SOC_at_inpor;
  real_T ImpSel_InsertedFor_InitialS;
  real_T ImpSel_InsertedFor_CellTemp;
  real_T ImpSel_InsertedFor_CellVolt;
  B_CoreSubsys_BatterySOCEstima_T CoreSubsys[1];
                                    /* '<Root>/SOC Estimator (Kalman Filter)' */
} B_BatterySOCEstimation_rev_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  DW_CoreSubsys_BatterySOCEstim_T CoreSubsys[1];
                                    /* '<Root>/SOC Estimator (Kalman Filter)' */
} DW_BatterySOCEstimation_rev_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T In1;                          /* '<Root>/In1' */
  real_T In2;                          /* '<Root>/In2' */
  real_T In3;                          /* '<Root>/In3' */
} ExtU_BatterySOCEstimation_rev_T;

/* Parameters for system: '<Root>/SOC Estimator (Kalman Filter)' */
struct P_CoreSubsys_BatterySOCEstima_T_ {
  real_T Gain_Gain;                    /* Expression: -1
                                        * Referenced by: '<S1>/Gain'
                                        */
  real_T Constant_Value[4];            /* Expression: [1 0;
                                          0 0]
                                        * Referenced by: '<S6>/Constant'
                                        */
  real_T Constant1_Value;              /* Expression: 0
                                        * Referenced by: '<S5>/Constant1'
                                        */
  real_T uDLookupTableC1_tableData[63];/* Expression: C1_mat
                                        * Referenced by: '<S6>/2-D Lookup Table C1'
                                        */
  real_T Ts_Gain;                      /* Expression: -1
                                        * Referenced by: '<S6>/-Ts'
                                        */
  real_T uDLookupTabledV0_tableData[63];/* Expression: dV0_mat
                                         * Referenced by: '<S6>/2-D Lookup Table dV0'
                                         */
  real_T Constant4_Value;              /* Expression: -1
                                        * Referenced by: '<S6>/Constant4'
                                        */
  real_T Constant1_Value_b;            /* Expression: eye(size(R,1))
                                        * Referenced by: '<S4>/Constant1'
                                        */
  real_T Constant1_Value_o;            /* Expression: 1
                                        * Referenced by: '<S6>/Constant1'
                                        */
  real_T Constant2_Value[4];           /* Expression: eye(size(Q,1))
                                        * Referenced by: '<S4>/Constant2'
                                        */
  uint32_T uDLookupTableR1_maxIndex[2];
                                 /* Computed Parameter: uDLookupTableR1_maxIndex
                                  * Referenced by: '<S6>/2-D Lookup Table R1'
                                  */
  uint32_T uDLookupTableC1_maxIndex[2];
                                 /* Computed Parameter: uDLookupTableC1_maxIndex
                                  * Referenced by: '<S6>/2-D Lookup Table C1'
                                  */
  uint32_T uDLookupTabledV0_maxIndex[2];
                                /* Computed Parameter: uDLookupTabledV0_maxIndex
                                 * Referenced by: '<S6>/2-D Lookup Table dV0'
                                 */
  uint32_T uDLookupTableV0_maxIndex[2];
                                 /* Computed Parameter: uDLookupTableV0_maxIndex
                                  * Referenced by: '<S6>/2-D Lookup Table V0'
                                  */
  uint32_T uDLookupTableR0_maxIndex[2];
                                 /* Computed Parameter: uDLookupTableR0_maxIndex
                                  * Referenced by: '<S6>/2-D Lookup Table R0'
                                  */
};

/* Parameters (default storage) */
struct P_BatterySOCEstimation_rev_T_ {
  real_T AH;                           /* Variable: AH
                                        * Referenced by: '<S6>/Gain'
                                        */
  real_T P0[4];                        /* Variable: P0
                                        * Referenced by: '<S3>/Unit Delay - P'
                                        */
  real_T Q[4];                         /* Variable: Q
                                        * Referenced by: '<S7>/Constant'
                                        */
  real_T R;                            /* Variable: R
                                        * Referenced by: '<S4>/Constant'
                                        */
  real_T initialSOC;                   /* Variable: initialSOC
                                        * Referenced by: '<Root>/Constant'
                                        */
  real_T ocv_map[63];                  /* Variable: ocv_map
                                        * Referenced by: '<S6>/2-D Lookup Table V0'
                                        */
  real_T r0_mat[63];                   /* Variable: r0_mat
                                        * Referenced by: '<S6>/2-D Lookup Table R0'
                                        */
  real_T r1_mat[63];                   /* Variable: r1_mat
                                        * Referenced by: '<S6>/2-D Lookup Table R1'
                                        */
  real_T soc_range[21];                /* Variable: soc_range
                                        * Referenced by:
                                        *   '<S6>/2-D Lookup Table C1'
                                        *   '<S6>/2-D Lookup Table R0'
                                        *   '<S6>/2-D Lookup Table R1'
                                        *   '<S6>/2-D Lookup Table V0'
                                        *   '<S6>/2-D Lookup Table dV0'
                                        */
  real_T temperature[3];               /* Variable: temperature
                                        * Referenced by:
                                        *   '<S6>/2-D Lookup Table C1'
                                        *   '<S6>/2-D Lookup Table R0'
                                        *   '<S6>/2-D Lookup Table R1'
                                        *   '<S6>/2-D Lookup Table V0'
                                        *   '<S6>/2-D Lookup Table dV0'
                                        */
  P_CoreSubsys_BatterySOCEstima_T CoreSubsys;
                                    /* '<Root>/SOC Estimator (Kalman Filter)' */
};

/* Real-time Model Data Structure */
struct tag_RTM_BatterySOCEstimation__T {
  const char_T * volatile errorStatus;
};

/* Block parameters (default storage) */
extern P_BatterySOCEstimation_rev_T BatterySOCEstimation_rev_P;

/* Block signals (default storage) */
extern B_BatterySOCEstimation_rev_T BatterySOCEstimation_rev_B;

/* Block states (default storage) */
extern DW_BatterySOCEstimation_rev_T BatterySOCEstimation_rev_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_BatterySOCEstimation_rev_T BatterySOCEstimation_rev_U;

/* Model entry point functions */
extern void BatterySOCEstimation_rev_initialize(void);
extern void BatterySOCEstimation_rev_step(void);
extern void BatterySOCEstimation_rev_terminate(void);

/* Real-time Model object */
extern RT_MODEL_BatterySOCEstimation_T *const BatterySOCEstimation_rev_M;
extern volatile boolean_T stopRequested;
extern volatile boolean_T runModel;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'BatterySOCEstimation_rev'
 * '<S1>'   : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)'
 * '<S2>'   : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter'
 * '<S3>'   : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter/EKF'
 * '<S4>'   : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter/EKF/Correction'
 * '<S5>'   : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter/EKF/Delay - X'
 * '<S6>'   : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter/EKF/Jacobian'
 * '<S7>'   : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter/EKF/Prediction'
 * '<S8>'   : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter/EKF/Correction/Data Type Conversion Inherited'
 * '<S9>'   : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter/EKF/Correction/Data Type Conversion Inherited1'
 * '<S10>'  : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter/EKF/Delay - X/Data Type Conversion Inherited'
 * '<S11>'  : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter/EKF/Jacobian/Data Type Conversion Inherited'
 * '<S12>'  : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter/EKF/Jacobian/Data Type Conversion Inherited1'
 * '<S13>'  : 'BatterySOCEstimation_rev/SOC Estimator (Kalman Filter)/Kalman Filter/EKF/Jacobian/Data Type Conversion Inherited2'
 */
#endif                                 /* BatterySOCEstimation_rev_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
