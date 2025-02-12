/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: BatterySOCEstimation_rev.c
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

#include "BatterySOCEstimation_rev.h"
#include <math.h>
#include "rtwtypes.h"
#include "BatterySOCEstimation_rev_private.h"

/* Block signals (default storage) */
B_BatterySOCEstimation_rev_T BatterySOCEstimation_rev_B;

/* Block states (default storage) */
DW_BatterySOCEstimation_rev_T BatterySOCEstimation_rev_DW;

/* External inputs (root inport signals with default storage) */
ExtU_BatterySOCEstimation_rev_T BatterySOCEstimation_rev_U;

/* Real-time model */
static RT_MODEL_BatterySOCEstimation_T BatterySOCEstimation_rev_M_;
RT_MODEL_BatterySOCEstimation_T *const BatterySOCEstimation_rev_M =
  &BatterySOCEstimation_rev_M_;
real_T look2_binlxpw(real_T u0, real_T u1, const real_T bp0[], const real_T bp1[],
                     const real_T table[], const uint32_T maxIndex[], uint32_T
                     stride)
{
  real_T fractions[2];
  real_T frac;
  real_T yL_0d0;
  real_T yL_0d1;
  uint32_T bpIndices[2];
  uint32_T bpIdx;
  uint32_T iLeft;
  uint32_T iRght;

  /* Column-major Lookup 2-D
     Search method: 'binary'
     Use previous index: 'off'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Linear'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Linear'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    iLeft = 0U;
    frac = (u0 - bp0[0U]) / (bp0[1U] - bp0[0U]);
  } else if (u0 < bp0[maxIndex[0U]]) {
    /* Binary Search */
    bpIdx = maxIndex[0U] >> 1U;
    iLeft = 0U;
    iRght = maxIndex[0U];
    while (iRght - iLeft > 1U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  } else {
    iLeft = maxIndex[0U] - 1U;
    frac = (u0 - bp0[maxIndex[0U] - 1U]) / (bp0[maxIndex[0U]] - bp0[maxIndex[0U]
      - 1U]);
  }

  fractions[0U] = frac;
  bpIndices[0U] = iLeft;

  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Linear'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u1 <= bp1[0U]) {
    iLeft = 0U;
    frac = (u1 - bp1[0U]) / (bp1[1U] - bp1[0U]);
  } else if (u1 < bp1[maxIndex[1U]]) {
    /* Binary Search */
    bpIdx = maxIndex[1U] >> 1U;
    iLeft = 0U;
    iRght = maxIndex[1U];
    while (iRght - iLeft > 1U) {
      if (u1 < bp1[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u1 - bp1[iLeft]) / (bp1[iLeft + 1U] - bp1[iLeft]);
  } else {
    iLeft = maxIndex[1U] - 1U;
    frac = (u1 - bp1[maxIndex[1U] - 1U]) / (bp1[maxIndex[1U]] - bp1[maxIndex[1U]
      - 1U]);
  }

  /* Column-major Interpolation 2-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'portable wrapping'
   */
  bpIdx = iLeft * stride + bpIndices[0U];
  yL_0d0 = table[bpIdx];
  yL_0d0 += (table[bpIdx + 1U] - yL_0d0) * fractions[0U];
  bpIdx += stride;
  yL_0d1 = table[bpIdx];
  return (((table[bpIdx + 1U] - yL_0d1) * fractions[0U] + yL_0d1) - yL_0d0) *
    frac + yL_0d0;
}

/* Model step function */
void BatterySOCEstimation_rev_step(void)
{
  /* local scratch DWork variables */
  int32_T ForEach_itr;
  real_T Transpose1;
  real_T tmp;
  real_T tmp_0;
  real_T tmp_1;
  real_T tmp_2;
  real_T v1_idx_0;
  real_T v1_idx_1;
  int32_T i;

  /* Outputs for Iterator SubSystem: '<Root>/SOC Estimator (Kalman Filter)' incorporates:
   *  ForEach: '<S1>/For Each'
   */
  for (ForEach_itr = 0; ForEach_itr < 1; ForEach_itr++) {
    /* ForEachSliceSelector generated from: '<S1>/InitialSOC' incorporates:
     *  Constant: '<Root>/Constant'
     */
    BatterySOCEstimation_rev_B.ImpSel_InsertedFor_InitialS =
      BatterySOCEstimation_rev_P.initialSOC;

    /* ForEachSliceSelector generated from: '<S1>/CellTemperature' incorporates:
     *  Inport: '<Root>/In3'
     */
    BatterySOCEstimation_rev_B.ImpSel_InsertedFor_CellTemp =
      BatterySOCEstimation_rev_U.In3;

    /* ForEachSliceSelector generated from: '<S1>/CellVoltage' incorporates:
     *  Inport: '<Root>/In2'
     */
    BatterySOCEstimation_rev_B.ImpSel_InsertedFor_CellVolt =
      BatterySOCEstimation_rev_U.In2;

    /* Gain: '<S1>/Gain' incorporates:
     *  Inport: '<Root>/In1'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Gain =
      BatterySOCEstimation_rev_P.CoreSubsys.Gain_Gain *
      BatterySOCEstimation_rev_U.In1;

    /* DataTypeConversion: '<S11>/Conversion' incorporates:
     *  Constant: '<S6>/Constant'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion[0] =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant_Value[0];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion[1] =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant_Value[1];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion[2] =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant_Value[2];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion[3] =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant_Value[3];

    /* RateTransition: '<S3>/Rate Transition3' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition3 =
      BatterySOCEstimation_rev_B.ImpSel_InsertedFor_InitialS;

    /* DataTypeConversion: '<S10>/Conversion' incorporates:
     *  Constant: '<S5>/Constant1'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_m =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant1_Value;

    /* SignalConversion generated from: '<S5>/Delay' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      TmpSignalConversionAtDelayInpor[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition3;
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      TmpSignalConversionAtDelayInpor[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_m;

    /* Delay: '<S5>/Delay' */
    if (BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].icLoad) {
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].Delay_DSTATE[0] =
        BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
        TmpSignalConversionAtDelayInpor[0];
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].Delay_DSTATE[1] =
        BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
        TmpSignalConversionAtDelayInpor[1];
    }

    /* Delay: '<S5>/Delay' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Delay[0] =
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].Delay_DSTATE[0];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Delay[1] =
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].Delay_DSTATE[1];

    /* RateTransition: '<S3>/Rate Transition2' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition2 =
      BatterySOCEstimation_rev_B.ImpSel_InsertedFor_CellTemp;

    /* Lookup_n-D: '<S6>/2-D Lookup Table R1' incorporates:
     *  RateTransition: '<S3>/Rate Transition2'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].R1 = look2_binlxpw
      (BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Delay[0],
       BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition2,
       BatterySOCEstimation_rev_P.soc_range,
       BatterySOCEstimation_rev_P.temperature, BatterySOCEstimation_rev_P.r1_mat,
       BatterySOCEstimation_rev_P.CoreSubsys.uDLookupTableR1_maxIndex, 21U);

    /* Lookup_n-D: '<S6>/2-D Lookup Table C1' incorporates:
     *  RateTransition: '<S3>/Rate Transition2'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].C1 = look2_binlxpw
      (BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Delay[0],
       BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition2,
       BatterySOCEstimation_rev_P.soc_range,
       BatterySOCEstimation_rev_P.temperature,
       BatterySOCEstimation_rev_P.CoreSubsys.uDLookupTableC1_tableData,
       BatterySOCEstimation_rev_P.CoreSubsys.uDLookupTableC1_maxIndex, 21U);

    /* Product: '<S6>/Product' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].tau1 =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].R1 *
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].C1;

    /* Gain: '<S6>/-Ts' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Ts =
      BatterySOCEstimation_rev_P.CoreSubsys.Ts_Gain *
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Probe[0];

    /* Product: '<S6>/Product2' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product2 = 1.0 /
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].tau1 *
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Ts;

    /* Math: '<S6>/Math Function'
     *
     * About '<S6>/Math Function':
     *  Operator: exp
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].MathFunction = exp
      (BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product2);

    /* Assignment: '<S6>/Assignment' incorporates:
     *  DataTypeConversion: '<S11>/Conversion'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion[0];

    /* UnitDelay: '<S3>/Unit Delay - P' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].UnitDelayP[0] =
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[0];

    /* Assignment: '<S6>/Assignment' incorporates:
     *  DataTypeConversion: '<S11>/Conversion'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion[1];

    /* UnitDelay: '<S3>/Unit Delay - P' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].UnitDelayP[1] =
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[1];

    /* Assignment: '<S6>/Assignment' incorporates:
     *  DataTypeConversion: '<S11>/Conversion'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[2] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion[2];

    /* UnitDelay: '<S3>/Unit Delay - P' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].UnitDelayP[2] =
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[2];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].UnitDelayP[3] =
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[3];

    /* Assignment: '<S6>/Assignment' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[3] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].MathFunction;

    /* Math: '<S7>/Transpose' incorporates:
     *  Assignment: '<S6>/Assignment'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[0];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[2];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose[2] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[1];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose[3] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[3];

    /* Product: '<S7>/Product2' incorporates:
     *  Assignment: '<S6>/Assignment'
     *  Math: '<S7>/Transpose'
     *  UnitDelay: '<S3>/Unit Delay - P'
     */
    BatterySOCEstimation_rev_B.dv[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].UnitDelayP[0];
    tmp = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose[0];
    BatterySOCEstimation_rev_B.dv[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].UnitDelayP[1];
    tmp_0 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose[1];
    BatterySOCEstimation_rev_B.dv[2] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].UnitDelayP[2];
    tmp_1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose[2];
    BatterySOCEstimation_rev_B.dv[3] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].UnitDelayP[3];
    tmp_2 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose[3];
    for (i = 0; i < 2; i++) {
      Transpose1 = BatterySOCEstimation_rev_B.dv[i];
      v1_idx_1 = Transpose1 * tmp;
      v1_idx_0 = BatterySOCEstimation_rev_B.dv[i + 2];
      v1_idx_1 += v1_idx_0 * tmp_0;
      BatterySOCEstimation_rev_B.v1[i] = v1_idx_1;
      v1_idx_1 = Transpose1 * tmp_1;
      v1_idx_1 += v1_idx_0 * tmp_2;
      BatterySOCEstimation_rev_B.v1[i + 2] = v1_idx_1;
    }

    BatterySOCEstimation_rev_B.dv[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[0];
    BatterySOCEstimation_rev_B.dv[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[1];
    BatterySOCEstimation_rev_B.dv[2] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[2];
    BatterySOCEstimation_rev_B.dv[3] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[3];
    v1_idx_1 = BatterySOCEstimation_rev_B.v1[0];
    tmp = BatterySOCEstimation_rev_B.v1[1];
    tmp_0 = BatterySOCEstimation_rev_B.v1[2];
    tmp_1 = BatterySOCEstimation_rev_B.v1[3];
    for (i = 0; i < 2; i++) {
      /* Product: '<S7>/Product2' */
      Transpose1 = BatterySOCEstimation_rev_B.dv[i];

      /* Product: '<S7>/Product2' */
      tmp_2 = Transpose1 * v1_idx_1;

      /* Product: '<S7>/Product2' */
      v1_idx_0 = BatterySOCEstimation_rev_B.dv[i + 2];

      /* Product: '<S7>/Product2' */
      tmp_2 += v1_idx_0 * tmp;

      /* Product: '<S7>/Product2' */
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product2_l[i] = tmp_2;

      /* Product: '<S7>/Product2' */
      tmp_2 = Transpose1 * tmp_0;
      tmp_2 += v1_idx_0 * tmp_1;

      /* Product: '<S7>/Product2' */
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product2_l[i + 2] =
        tmp_2;
    }

    /* Sum: '<S7>/Sum1' incorporates:
     *  Constant: '<S7>/Constant'
     *  Product: '<S7>/Product2'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product2_l[0] +
      BatterySOCEstimation_rev_P.Q[0];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product2_l[1] +
      BatterySOCEstimation_rev_P.Q[1];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[2] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product2_l[2] +
      BatterySOCEstimation_rev_P.Q[2];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[3] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product2_l[3] +
      BatterySOCEstimation_rev_P.Q[3];

    /* Lookup_n-D: '<S6>/2-D Lookup Table dV0' incorporates:
     *  RateTransition: '<S3>/Rate Transition2'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].dV0 = look2_binlxpw
      (BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Delay[0],
       BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition2,
       BatterySOCEstimation_rev_P.soc_range,
       BatterySOCEstimation_rev_P.temperature,
       BatterySOCEstimation_rev_P.CoreSubsys.uDLookupTabledV0_tableData,
       BatterySOCEstimation_rev_P.CoreSubsys.uDLookupTabledV0_maxIndex, 21U);

    /* DataTypeConversion: '<S13>/Conversion' incorporates:
     *  Constant: '<S6>/Constant4'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_g =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant4_Value;

    /* SignalConversion generated from: '<S6>/Transpose1' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      TmpSignalConversionAtTranspose1[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].dV0;
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      TmpSignalConversionAtTranspose1[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_g;

    /* Math: '<S6>/Transpose1' */
    Transpose1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      TmpSignalConversionAtTranspose1[0];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose1[0] =
      Transpose1;

    /* Math: '<S4>/Transpose' incorporates:
     *  Math: '<S6>/Transpose1'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose_a[0] =
      Transpose1;

    /* Math: '<S6>/Transpose1' */
    Transpose1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      TmpSignalConversionAtTranspose1[1];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose1[1] =
      Transpose1;

    /* Math: '<S4>/Transpose' incorporates:
     *  Math: '<S6>/Transpose1'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose_a[1] =
      Transpose1;

    /* Product: '<S4>/Product' incorporates:
     *  Math: '<S4>/Transpose'
     *  Product: '<S4>/Product2'
     *  Sum: '<S7>/Sum1'
     */
    BatterySOCEstimation_rev_B.dv[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[0];
    BatterySOCEstimation_rev_B.dv[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[1];
    BatterySOCEstimation_rev_B.dv[2] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[2];
    BatterySOCEstimation_rev_B.dv[3] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[3];
    tmp = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose_a[0];
    tmp_0 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose_a[1];
    Transpose1 = BatterySOCEstimation_rev_B.dv[0] * tmp;

    /* Product: '<S4>/Product' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product[0] = Transpose1;

    /* Product: '<S4>/Product' incorporates:
     *  Product: '<S4>/Product2'
     */
    v1_idx_1 = BatterySOCEstimation_rev_B.dv[2] * tmp_0 +
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product[0];

    /* Product: '<S4>/Product' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product[0] = v1_idx_1;

    /* Product: '<S4>/Product' incorporates:
     *  Product: '<S4>/Product2'
     */
    Transpose1 = BatterySOCEstimation_rev_B.dv[1] * tmp;

    /* Product: '<S4>/Product' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product[1] = Transpose1;

    /* Product: '<S4>/Product' incorporates:
     *  Product: '<S4>/Product2'
     */
    Transpose1 = BatterySOCEstimation_rev_B.dv[3] * tmp_0 +
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product[1];

    /* Product: '<S4>/Product' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product[1] = Transpose1;

    /* DataTypeConversion: '<S9>/Conversion' incorporates:
     *  Constant: '<S4>/Constant1'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_j =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant1_Value_b;

    /* Product: '<S4>/Product2' */
    v1_idx_0 = v1_idx_1;
    v1_idx_1 = Transpose1;

    /* Product: '<S4>/Divide' */
    Transpose1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_j;

    /* Product: '<S4>/Product2' incorporates:
     *  Math: '<S6>/Transpose1'
     */
    tmp_0 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose1[0];
    v1_idx_0 *= tmp_0;

    /* Product: '<S4>/Product1' incorporates:
     *  Product: '<S4>/Product'
     */
    tmp_0 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product[0];
    tmp = tmp_0;

    /* Product: '<S4>/Product2' incorporates:
     *  Math: '<S6>/Transpose1'
     */
    tmp_0 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose1[1];
    v1_idx_0 += tmp_0 * v1_idx_1;

    /* Product: '<S4>/Product1' incorporates:
     *  Product: '<S4>/Product'
     */
    tmp_0 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product[1];

    /* Product: '<S4>/Product2' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product2_h = v1_idx_0;

    /* Sum: '<S4>/Sum2' incorporates:
     *  Constant: '<S4>/Constant'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum2 =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product2_h +
      BatterySOCEstimation_rev_P.R;

    /* Product: '<S4>/Divide' */
    v1_idx_0 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum2;

    /* Product: '<S4>/Divide' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Divide = Transpose1 /
      v1_idx_0;

    /* Product: '<S4>/Product1' */
    Transpose1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Divide;

    /* RateTransition: '<S3>/Rate Transition' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition =
      BatterySOCEstimation_rev_B.ImpSel_InsertedFor_CellVolt;

    /* Lookup_n-D: '<S6>/2-D Lookup Table V0' incorporates:
     *  RateTransition: '<S3>/Rate Transition2'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].V0 = look2_binlxpw
      (BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Delay[0],
       BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition2,
       BatterySOCEstimation_rev_P.soc_range,
       BatterySOCEstimation_rev_P.temperature,
       BatterySOCEstimation_rev_P.ocv_map,
       BatterySOCEstimation_rev_P.CoreSubsys.uDLookupTableV0_maxIndex, 21U);

    /* Lookup_n-D: '<S6>/2-D Lookup Table R0' incorporates:
     *  RateTransition: '<S3>/Rate Transition2'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].R0 = look2_binlxpw
      (BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Delay[0],
       BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition2,
       BatterySOCEstimation_rev_P.soc_range,
       BatterySOCEstimation_rev_P.temperature, BatterySOCEstimation_rev_P.r0_mat,
       BatterySOCEstimation_rev_P.CoreSubsys.uDLookupTableR0_maxIndex, 21U);

    /* RateTransition: '<S3>/Rate Transition1' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition1 =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Gain;

    /* Product: '<S6>/Product6' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product6 =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].R0 *
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition1;

    /* Sum: '<S6>/Sum2' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum2_l =
      (BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].V0 -
       BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product6) -
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Delay[1];

    /* Sum: '<S3>/Sum' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].RateTransition -
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum2_l;

    /* Product: '<S4>/Product1' */
    v1_idx_0 = tmp * Transpose1;
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Kk1[0] = v1_idx_0;

    /* Product: '<S4>/Product3' incorporates:
     *  Product: '<S4>/Product1'
     */
    tmp = v1_idx_0;

    /* Product: '<S4>/Product1' */
    v1_idx_0 = tmp_0 * Transpose1;
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Kk1[1] = v1_idx_0;

    /* Product: '<S4>/Product3' incorporates:
     *  Product: '<S4>/Product1'
     */
    tmp_0 = v1_idx_0;
    Transpose1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum;

    /* Product: '<S4>/Product3' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product3[0] = tmp *
      Transpose1;
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product3[1] = tmp_0 *
      Transpose1;

    /* Product: '<S7>/Product' incorporates:
     *  Assignment: '<S6>/Assignment'
     */
    BatterySOCEstimation_rev_B.dv[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[0];
    BatterySOCEstimation_rev_B.dv[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[1];
    BatterySOCEstimation_rev_B.dv[2] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[2];
    BatterySOCEstimation_rev_B.dv[3] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Assignment[3];
    tmp = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Delay[0];
    tmp_0 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Delay[1];
    v1_idx_1 = BatterySOCEstimation_rev_B.dv[0] * tmp;
    v1_idx_1 += BatterySOCEstimation_rev_B.dv[2] * tmp_0;

    /* Product: '<S7>/Product' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product_c[0] = v1_idx_1;

    /* Product: '<S7>/Product' */
    v1_idx_1 = BatterySOCEstimation_rev_B.dv[1] * tmp;
    v1_idx_1 += BatterySOCEstimation_rev_B.dv[3] * tmp_0;

    /* Product: '<S7>/Product' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product_c[1] = v1_idx_1;

    /* Gain: '<S6>/Gain' */
    Transpose1 = 1.0 / (BatterySOCEstimation_rev_P.AH * 3600.0);

    /* Gain: '<S6>/Gain' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Gain_c = Transpose1 *
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Ts;

    /* DataTypeConversion: '<S12>/Conversion' incorporates:
     *  Constant: '<S6>/Constant1'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_a =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant1_Value_o;

    /* Sum: '<S6>/Sum' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum_m =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_a -
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].MathFunction;

    /* Product: '<S6>/Product4' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product4 =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum_m *
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].R1;

    /* SignalConversion generated from: '<S7>/Product1' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      TmpSignalConversionAtProduct1In[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Gain_c;
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      TmpSignalConversionAtProduct1In[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product4;

    /* Product: '<S7>/Product1' */
    Transpose1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      RateTransition1;
    tmp = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      TmpSignalConversionAtProduct1In[0];
    tmp *= Transpose1;

    /* Product: '<S7>/Product1' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product1[0] = tmp;

    /* Sum: '<S7>/Sum' */
    tmp += BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product_c[0];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum_o[0] = tmp;

    /* Sum: '<S4>/Sum' incorporates:
     *  Product: '<S4>/Product3'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum_g[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product3[0] + tmp;

    /* Product: '<S7>/Product1' */
    tmp = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      TmpSignalConversionAtProduct1In[1];
    tmp *= Transpose1;

    /* Product: '<S7>/Product1' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product1[1] = tmp;

    /* Sum: '<S7>/Sum' */
    tmp += BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product_c[1];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum_o[1] = tmp;

    /* Sum: '<S4>/Sum' incorporates:
     *  Product: '<S4>/Product3'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum_g[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product3[1] + tmp;

    /* DataTypeConversion: '<S8>/Conversion' incorporates:
     *  Constant: '<S4>/Constant2'
     */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_o[0] =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant2_Value[0];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_o[1] =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant2_Value[1];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_o[2] =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant2_Value[2];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Conversion_o[3] =
      BatterySOCEstimation_rev_P.CoreSubsys.Constant2_Value[3];

    /* Product: '<S4>/Product4' incorporates:
     *  Math: '<S6>/Transpose1'
     *  Product: '<S4>/Product1'
     */
    tmp = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Kk1[0];
    v1_idx_0 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose1[0];
    tmp_0 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Kk1[1];
    v1_idx_1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Transpose1[1];

    /* Product: '<S4>/Product4' */
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product4_o[0] = tmp *
      v1_idx_0;
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product4_o[1] = tmp_0 *
      v1_idx_0;
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product4_o[2] = tmp *
      v1_idx_1;
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product4_o[3] = tmp_0 *
      v1_idx_1;

    /* Sum: '<S4>/Sum3' incorporates:
     *  DataTypeConversion: '<S8>/Conversion'
     *  Product: '<S4>/Product4'
     */
    Transpose1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr]
      .Conversion_o[0] - BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      Product4_o[0];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum3[0] = Transpose1;

    /* Product: '<S4>/Product5' incorporates:
     *  Sum: '<S4>/Sum3'
     *  Sum: '<S7>/Sum1'
     */
    BatterySOCEstimation_rev_B.dv[0] = Transpose1;
    tmp = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[0];

    /* Sum: '<S4>/Sum3' incorporates:
     *  DataTypeConversion: '<S8>/Conversion'
     *  Product: '<S4>/Product4'
     */
    Transpose1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr]
      .Conversion_o[1] - BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      Product4_o[1];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum3[1] = Transpose1;

    /* Product: '<S4>/Product5' incorporates:
     *  Sum: '<S4>/Sum3'
     *  Sum: '<S7>/Sum1'
     */
    BatterySOCEstimation_rev_B.dv[1] = Transpose1;
    tmp_0 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[1];

    /* Sum: '<S4>/Sum3' incorporates:
     *  DataTypeConversion: '<S8>/Conversion'
     *  Product: '<S4>/Product4'
     */
    Transpose1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr]
      .Conversion_o[2] - BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      Product4_o[2];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum3[2] = Transpose1;

    /* Product: '<S4>/Product5' incorporates:
     *  Sum: '<S4>/Sum3'
     *  Sum: '<S7>/Sum1'
     */
    BatterySOCEstimation_rev_B.dv[2] = Transpose1;
    tmp_1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[2];

    /* Sum: '<S4>/Sum3' incorporates:
     *  DataTypeConversion: '<S8>/Conversion'
     *  Product: '<S4>/Product4'
     */
    Transpose1 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr]
      .Conversion_o[3] - BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].
      Product4_o[3];
    BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum3[3] = Transpose1;

    /* Product: '<S4>/Product5' incorporates:
     *  Sum: '<S4>/Sum3'
     *  Sum: '<S7>/Sum1'
     */
    BatterySOCEstimation_rev_B.dv[3] = Transpose1;
    tmp_2 = BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum1[3];

    /* Update for Delay: '<S5>/Delay' */
    BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].icLoad = false;
    for (i = 0; i < 2; i++) {
      /* Product: '<S4>/Product5' */
      Transpose1 = BatterySOCEstimation_rev_B.dv[i];

      /* Product: '<S4>/Product5' */
      v1_idx_1 = Transpose1 * tmp;

      /* Product: '<S4>/Product5' */
      v1_idx_0 = BatterySOCEstimation_rev_B.dv[i + 2];

      /* Product: '<S4>/Product5' */
      v1_idx_1 += v1_idx_0 * tmp_0;

      /* Product: '<S4>/Product5' */
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product5[i] = v1_idx_1;

      /* Product: '<S4>/Product5' */
      v1_idx_1 = Transpose1 * tmp_1;
      v1_idx_1 += v1_idx_0 * tmp_2;

      /* Product: '<S4>/Product5' */
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product5[i + 2] =
        v1_idx_1;

      /* Update for Delay: '<S5>/Delay' */
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].Delay_DSTATE[i] =
        BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum_g[i];
    }

    /* Update for UnitDelay: '<S3>/Unit Delay - P' incorporates:
     *  Product: '<S4>/Product5'
     */
    BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[0] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product5[0];
    BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[1] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product5[1];
    BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[2] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product5[2];
    BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[3] =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Product5[3];

    /* ForEachSliceAssignment generated from: '<S1>/SOC' */
    BatterySOCEstimation_rev_B.ImpAsg_InsertedFor_SOC_at_inpor =
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Sum_g[0];
  }

  /* End of Outputs for SubSystem: '<Root>/SOC Estimator (Kalman Filter)' */
}

/* Model initialize function */
void BatterySOCEstimation_rev_initialize(void)
{
  {
    /* local scratch DWork variables */
    int32_T ForEach_itr;

    /* SystemInitialize for Iterator SubSystem: '<Root>/SOC Estimator (Kalman Filter)' */
    for (ForEach_itr = 0; ForEach_itr < 1; ForEach_itr++) {
      /* Start for Probe: '<S6>/Probe' */
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Probe[0] = 1.0;
      BatterySOCEstimation_rev_B.CoreSubsys[ForEach_itr].Probe[1] = 0.0;

      /* InitializeConditions for Delay: '<S5>/Delay' */
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].icLoad = true;

      /* InitializeConditions for UnitDelay: '<S3>/Unit Delay - P' */
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[0] =
        BatterySOCEstimation_rev_P.P0[0];
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[1] =
        BatterySOCEstimation_rev_P.P0[1];
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[2] =
        BatterySOCEstimation_rev_P.P0[2];
      BatterySOCEstimation_rev_DW.CoreSubsys[ForEach_itr].UnitDelayP_DSTATE[3] =
        BatterySOCEstimation_rev_P.P0[3];
    }

    /* End of SystemInitialize for SubSystem: '<Root>/SOC Estimator (Kalman Filter)' */
  }
}

/* Model terminate function */
void BatterySOCEstimation_rev_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
