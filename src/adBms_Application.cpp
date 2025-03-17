/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    adbms_Application.c
* @brief:   adbms application test cases
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/*! \addtogroup APPLICATION
*  @{
*/

/*! @addtogroup Application
*  @{
*/
// Edge Impulse
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

#include "common.h"
#include "adBms_Application.h"
#include "adBms6830CmdList.h"
#include "adBms6830GenericType.h"
#include "serialPrintResult.h"
#include "mcuWrapper.h"
#include "edge_impulse_data.h"

#include "current_sense_main.h"

/**
*******************************************************************************
* @brief Setup Variables
* The following variables can be modified to configure the software.
*******************************************************************************
*/

#define TOTAL_IC 1
cell_asic IC[TOTAL_IC];

/* ADC Command Configurations */
RD      REDUNDANT_MEASUREMENT           = RD_OFF;
CH      AUX_CH_TO_CONVERT               = GPIO1;
CONT    CONTINUOUS_MEASUREMENT          = SINGLE;
OW_C_S  CELL_OPEN_WIRE_DETECTION        = OW_OFF_ALL_CH;
OW_AUX  AUX_OPEN_WIRE_DETECTION         = AUX_OW_OFF;
PUP     OPEN_WIRE_CURRENT_SOURCE        = PUP_UP;
DCP     DISCHARGE_PERMITTED             = DCP_OFF;
RSTF    RESET_FILTER                    = RSTF_OFF;
ERR     INJECT_ERR_SPI_READ             = WITHOUT_ERR;

/* Set Under Voltage and Over Voltage Thresholds */
const float OV_THRESHOLD = 4.2;                 /* Volt */
const float UV_THRESHOLD = 3.0;                 /* Volt */
const int OWC_Threshold = 2000;                 /* Cell Open wire threshold(mili volt) */
const int OWA_Threshold = 5;                /* Aux Open wire threshold(mili volt) */
const uint32_t LOOP_MEASUREMENT_COUNT = 1;      /* Loop measurment count */
const uint16_t MEASUREMENT_LOOP_TIME  = 10;     /* milliseconds(mS)*/
uint32_t loop_count = 0;
uint32_t pladc_count;

/*Loop Measurement Setup These Variables are ENABLED or DISABLED Remember ALL CAPS*/
LOOP_MEASURMENT MEASURE_CELL            = ENABLED;        /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_AVG_CELL        = DISABLED;        /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_F_CELL          = DISABLED;        /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_S_VOLTAGE       = DISABLED;        /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_AUX             = DISABLED;       /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_RAUX            = DISABLED;        /*   This is ENABLED or DISABLED       */
LOOP_MEASURMENT MEASURE_STAT            = DISABLED;        /*   This is ENABLED or DISABLED       */

// SIMULATE SENSORS
// Global simulation variables
float simulated_voltage = 4.2f;      // Starting voltage (Volts)
float simulated_current = -1.0f;     // Starting current (Amperes, negative for discharging)
float simulated_temperature = 22.0f; // Constant temperature (°C)
// You can use constant placeholders for these if you don’t want to simulate changes:
float simulated_ah = 2.0f;           // Placeholder for ampere-hours (Ah)
float simulated_power = 5.0f;        // Placeholder for power (Watts)
float simulated_brand = 1.0f;        // Arbitrary placeholder

// Revised simulate_sensor_readings() function:
// This function updates the passed array (sensor_values) with simulated data,
// following the order required by the ML model.
void simulate_sensor_readings(float *sensor_values) {
  // Update simulated voltage based on current (a simple integration step)
  simulated_voltage += simulated_current * 0.01f; // Adjust step size as needed

  // Reverse current direction when voltage boundaries are reached
  if (simulated_voltage <= 2.5f) {
      simulated_current = 1.0f;  // Now charging
  } else if (simulated_voltage >= 4.2f) {
      simulated_current = -1.0f; // Now discharging
  }

  // Clamp voltage to the allowed range
  if (simulated_voltage > 4.2f) simulated_voltage = 4.2f;
  if (simulated_voltage < 2.5f) simulated_voltage = 2.5f;

  // (Optional) Update simulated_ah and simulated_power here if you wish.
  // For simplicity, we'll leave them constant.

  // Fill the sensor_values array with simulated data
  // The expected order is:
  // Index 0: Voltage
  // Index 1: Current
  // Index 2: Ampere-hours (Ah)
  // Index 3: Power
  // Index 4: Battery temperature
  // Index 5: Brand
  sensor_values[0] = simulated_voltage;
  sensor_values[1] = simulated_current;
  sensor_values[2] = simulated_ah;
  sensor_values[3] = simulated_power;
  sensor_values[4] = simulated_temperature;
  sensor_values[5] = simulated_brand;
}

// Edge Impulse ML

// From your model_metadata.h
// * 10 samples per inference window
// * 6 axes per sample
// * Frequency ~10.309 Hz
#define SAMPLE_COUNT        10
#define NUM_AXES            6
#define SAMPLE_FREQUENCY_HZ 10.30905633

// Derived values
#define TOTAL_SAMPLES       (SAMPLE_COUNT * NUM_AXES)     // 10×6 = 60
#define SLEEP_TIME_MS       (1000 / SAMPLE_FREQUENCY_HZ)  // ~97 ms

// 1) Create a global buffer that the classifier will read from.
//    This must match EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE (which should be 60).
float raw_data_buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

// 2) The classifier calls this function to get data
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, raw_data_buffer + offset, length * sizeof(float));
    return 0;
}

// Collect one full inference window (10 samples × 6 axes = 60 floats)
void collect_data_for_inference() {
  // We want SAMPLE_COUNT = 10 samples, each with NUM_AXES = 6 floats,
  // so that raw_data_buffer[] ends up with 10*6 = 60 floats.
  for (int sample_idx = 0; sample_idx < SAMPLE_COUNT; sample_idx++) {

      // Create a local array to hold one sample reading.
      float sensor_values[NUM_AXES] = {0};

      // If you are simulating sensor readings instead of using the real hardware,
      // call simulate_sensor_readings() to fill sensor_values.
      simulate_sensor_readings(sensor_values);

      // If you are combining simulated readings with real hardware reads,
      // you could also call your sensor read functions (e.g., read cell voltages,
      // AUX, current_sense_main, etc.) that update sensor_values.
      // For example:
      // adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
      // adBms6830_read_cell_voltages(TOTAL_IC, &IC[0]);
      // printVoltages(TOTAL_IC, &IC[0], Cell, sensor_values);
      //
      // adBms6830_start_aux_voltage_measurment(TOTAL_IC, &IC[0]);
      // adBms6830_read_aux_voltages(TOTAL_IC, &IC[0]);
      // printVoltages(TOTAL_IC, &IC[0], Aux, sensor_values);
      //
      // current_sense_main(sensor_values);

      // Now copy this sample into the global raw_data_buffer.
      // The offset is sample_idx * NUM_AXES.
      for (int j = 0; j < NUM_AXES; j++) {
          raw_data_buffer[sample_idx * NUM_AXES + j] = sensor_values[j];
      }

      // Wait for ~97 ms to maintain the sampling frequency (~10.31 Hz)
      ThisThread::sleep_for((int)SLEEP_TIME_MS);
  }
  // At this point, raw_data_buffer[] holds a full window (60 floats) for inference.
}

// We'll store classification results here
ei_impulse_result_t result;

void app_main()
{    
  //  printMenu();
  printf("=== Edge Impulse: Integrating real sensor data ===\n");
  printf("Expecting %d samples, each with %d axes (total %d floats), at %.2f Hz.\n",
        SAMPLE_COUNT, NUM_AXES, TOTAL_SAMPLES, (float)SAMPLE_FREQUENCY_HZ);
        
  adBms6830_init_config(TOTAL_IC, &IC[0]);

  while(1)
  {
    // int user_command;
    // #ifdef MBED
    //     scanf("%d", &user_command);
    //     printf("Enter cmd:%d\n", user_command);
    // #else
    //     scanf("%d", &user_command);
    //     printf("Enter cmd:%d\n", user_command);
    // #endif

    // Edge Impulse ML
    // Step A: Collect one window of data
    collect_data_for_inference();

    // Step B: Wrap the raw_data_buffer with a signal structure
    signal_t signal;
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &raw_feature_get_data;

    // Step C: Run the classifier
    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
    if (res != EI_IMPULSE_OK) {
        printf("run_classifier failed (%d)\n", res);
        continue;
    }

    // Step D: Print classification
    printf("Inference results:\n");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        printf("   %s: %.5f\n",
               result.classification[ix].label,
               result.classification[ix].value);
    }

    #if EI_CLASSIFIER_HAS_ANOMALY == 1
        printf("   anomaly: %.3f\n", result.anomaly);
    #endif

    // Step E: Delay before running again
    ThisThread::sleep_for(1000);
  }
}

void run_command(int cmd)
{
  switch(cmd)
  {

  case 1:
    adBms6830_write_read_config(TOTAL_IC, &IC[0]);
    break;

  case 2:
    adBms6830_read_config(TOTAL_IC, &IC[0]);
    break;

    case 3: 
  {  
    // // Simulate sensor readings
    // simulate_sensor_readings();

    // // Run the ML model step update
    // BatterySOCEstimation_rev_step();

    // // Print simulated inputs and ML-predicted SoC
    // printf("%f, %f, %f, %f\n", 
    //     BatterySOCEstimation_rev_U.In1,  // Simulated current
    //     BatterySOCEstimation_rev_U.In2,  // Simulated voltage
    //     BatterySOCEstimation_rev_U.In3,  // Simulated temperature
    //     BatterySOCEstimation_rev_B.ImpAsg_InsertedFor_SOC_at_inpor // Estimated SoC
    // );

    // break;
    
    // Start and read cell voltages
    adBms6830_start_adc_cell_voltage_measurment(TOTAL_IC);
    adBms6830_read_cell_voltages(TOTAL_IC, &IC[0]);

    wait_us(1000000);  // One second delay

    // Start and read aux (temperature) voltages
    adBms6830_start_aux_voltage_measurment(TOTAL_IC, &IC[0]); // GPIO Temp Reading
    adBms6830_read_aux_voltages(TOTAL_IC, &IC[0]);

    // Invoke current sense functionality
    // current_sense_main();

    // New CSV-formatted output:
    // Format: ekf_current_input, ekf_voltage_input, ekf_temp_input, ekf_soc
    // To log results, run in PlatformIO shell:
    // pio device monitor --port COM8 --baud 9600 --filter default --filter time --filter log2file

    // Replace EKF inputs with ML:

    // printf("%f, %f, %f, %f\n", 
    //   BatterySOCEstimation_rev_U.In1, 
    //   BatterySOCEstimation_rev_U.In2, 
    //   BatterySOCEstimation_rev_U.In3, 
    //   BatterySOCEstimation_rev_B.ImpAsg_InsertedFor_SOC_at_inpor);
    // break;
  }

  case 4:
    adBms6830_read_cell_voltages(TOTAL_IC, &IC[0]);
    break;

  case 5:

    adBms6830_start_adc_s_voltage_measurment(TOTAL_IC);
    break;

  case 6:
    adBms6830_read_s_voltages(TOTAL_IC, &IC[0]);
    break;

  case 7:
    adBms6830_start_avgcell_voltage_measurment(TOTAL_IC);
    break;

  case 8:
    adBms6830_read_avgcell_voltages(TOTAL_IC, &IC[0]);
    break;

  case 9:
    adBms6830_start_fcell_voltage_measurment(TOTAL_IC);
    break;

  case 10:
    adBms6830_read_fcell_voltages(TOTAL_IC, &IC[0]);
    break;

  case 11:
    adBms6830_start_aux_voltage_measurment(TOTAL_IC, &IC[0]);
    break;

  case 12:
    adBms6830_read_aux_voltages(TOTAL_IC, &IC[0]);
    break;

  case 13:
    adBms6830_start_raux_voltage_measurment(TOTAL_IC, &IC[0]);
    break;

  case 14:
    adBms6830_read_raux_voltages(TOTAL_IC, &IC[0]);
    break;

  case 15:
    adBms6830_read_status_registers(TOTAL_IC, &IC[0]);
    break;

  case 16:
    loop_count = 0;
    adBmsWakeupIc(TOTAL_IC);
    adBmsWriteData(TOTAL_IC, &IC[0], WRCFGA, Config, A);
    adBmsWriteData(TOTAL_IC, &IC[0], WRCFGB, Config, B);
    adBmsWakeupIc(TOTAL_IC);
    adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
    Delay_ms(1); // ADCs are updated at their conversion rate is 1ms
    adBms6830_Adcv(RD_ON, CONTINUOUS, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
    Delay_ms(1); // ADCs are updated at their conversion rate is 1ms
    adBms6830_Adsv(CONTINUOUS, DISCHARGE_PERMITTED, CELL_OPEN_WIRE_DETECTION);
    Delay_ms(8); // ADCs are updated at their conversion rate is 8ms
    while(loop_count < LOOP_MEASUREMENT_COUNT)
    {
      measurement_loop();
      Delay_ms(MEASUREMENT_LOOP_TIME);
      loop_count = loop_count + 1;
      printf("check\n");
    }
    printMenu();
    break;

  case 17:
    adBms6830_clear_cell_measurement(TOTAL_IC);
    break;

  case 18:
    adBms6830_clear_aux_measurement(TOTAL_IC);
    break;

  case 19:
    adBms6830_clear_spin_measurement(TOTAL_IC);
    break;

  case 20:
    adBms6830_clear_fcell_measurement(TOTAL_IC);
    break;

  case 0:
    printMenu();
    break;

  default:
#ifdef MBED
    printf("Incorrect Option\n\n");
#else
    printf("Incorrect Option\n\n");
#endif
    break;
  }
}

/**
*******************************************************************************
* @brief Set configuration register A. Refer to the data sheet
*        Set configuration register B. Refer to the data sheet
*******************************************************************************
*/
void adBms6830_init_config(uint8_t tIC, cell_asic *ic)
{
  for(uint8_t cic = 0; cic < tIC; cic++)
  {
    /* Init config A */
    ic[cic].tx_cfga.refon = PWR_UP;
//    ic[cic].cfga.cth = CVT_8_1mV;
//    ic[cic].cfga.flag_d = ConfigA_Flag(FLAG_D0, FLAG_SET) | ConfigA_Flag(FLAG_D1, FLAG_SET);
//    ic[cic].tx_cfga.gpo = ConfigA_Gpo(GPO2, GPO_SET) | ConfigA_Gpo(GPO10, GPO_SET); // set GPO2 as 3V output
    ic[cic].tx_cfga.gpo = 0X3FF; /* All GPIO pull down off */
//    ic[cic].cfga.soakon = SOAKON_CLR;
//    ic[cic].cfga.fc = IIR_FPA256;

    /* Init config B */
//    ic[cic].cfgb.dtmen = DTMEN_ON;
    ic[cic].tx_cfgb.vov = SetOverVoltageThreshold(OV_THRESHOLD);
    ic[cic].tx_cfgb.vuv = SetUnderVoltageThreshold(UV_THRESHOLD);
//    ic[cic].cfgb.dcc = ConfigB_DccBit(DCC16, DCC_BIT_SET);
//    SetConfigB_DischargeTimeOutValue(tIC, &ic[cic], RANG_0_TO_63_MIN, TIME_1MIN_OR_0_26HR);
  }
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, A);
  adBmsWriteData(tIC, &ic[0], WRCFGB, Config, B);
}

/**
*******************************************************************************
* @brief Write and Read Configuration Register A/B
*******************************************************************************
*/
void adBms6830_write_read_config(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, A);
  adBmsWriteData(tIC, &ic[0], WRCFGB, Config, B);
  adBmsReadData(tIC, &ic[0], RDCFGA, Config, A);
  adBmsReadData(tIC, &ic[0], RDCFGB, Config, B);
  printWriteConfig(tIC, &ic[0], Config, ALL_GRP);
  printReadConfig(tIC, &ic[0], Config, ALL_GRP);
}

/**
*******************************************************************************
* @brief Read Configuration Register A/B
*******************************************************************************
*/
void adBms6830_read_config(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDCFGA, Config, A);
  adBmsReadData(tIC, &ic[0], RDCFGB, Config, B);
  printReadConfig(tIC, &ic[0], Config, ALL_GRP);
}

/**
*******************************************************************************
* @brief Start ADC Cell Voltage Measurement
*******************************************************************************
*/
void adBms6830_start_adc_cell_voltage_measurment(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
  pladc_count = adBmsPollAdc(PLADC);
#ifdef MBED
  // printf("Cell conversion completed\n");
#else
  // printf("Cell conversion completed\n");
#endif
  printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read Cell Voltages
*******************************************************************************
*/
void adBms6830_read_cell_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDCVA, Cell, A);
  adBmsReadData(tIC, &ic[0], RDCVB, Cell, B);
  adBmsReadData(tIC, &ic[0], RDCVC, Cell, C);
  adBmsReadData(tIC, &ic[0], RDCVD, Cell, D);
  adBmsReadData(tIC, &ic[0], RDCVE, Cell, E);
  adBmsReadData(tIC, &ic[0], RDCVF, Cell, F);
}

/**
*******************************************************************************
* @brief Start ADC S-Voltage Measurement
*******************************************************************************
*/
void adBms6830_start_adc_s_voltage_measurment(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  adBms6830_Adsv(CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, CELL_OPEN_WIRE_DETECTION);
  pladc_count = adBmsPollAdc(PLADC);
#ifdef MBED
  printf("S-Voltage conversion completed\n");
#else
  printf("S-Voltage conversion completed\n");
#endif
  printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read S-Voltages
*******************************************************************************
*/
void adBms6830_read_s_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDSVA, S_volt, A);
  adBmsReadData(tIC, &ic[0], RDSVB, S_volt, B);
  adBmsReadData(tIC, &ic[0], RDSVC, S_volt, C);
  adBmsReadData(tIC, &ic[0], RDSVD, S_volt, D);
  adBmsReadData(tIC, &ic[0], RDSVE, S_volt, E);
  adBmsReadData(tIC, &ic[0], RDSVF, S_volt, F);
  // printVoltages(tIC, &ic[0], S_volt);
}

/**
*******************************************************************************
* @brief Start Avarage Cell Voltage Measurement
*******************************************************************************
*/
void adBms6830_start_avgcell_voltage_measurment(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  adBms6830_Adcv(RD_ON, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
  pladc_count = adBmsPollAdc(PLADC);
#ifdef MBED
  printf("Avg Cell voltage conversion completed\n");
#else
  printf("Avg Cell voltage conversion completed\n");
#endif
  printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read Avarage Cell Voltages
*******************************************************************************
*/
void adBms6830_read_avgcell_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDACA, AvgCell, A);
  adBmsReadData(tIC, &ic[0], RDACB, AvgCell, B);
  adBmsReadData(tIC, &ic[0], RDACC, AvgCell, C);
  adBmsReadData(tIC, &ic[0], RDACD, AvgCell, D);
  adBmsReadData(tIC, &ic[0], RDACE, AvgCell, E);
  adBmsReadData(tIC, &ic[0], RDACF, AvgCell, F);
  // printVoltages(tIC, &ic[0], AvgCell);
}

/**
*******************************************************************************
* @brief Start Filtered Cell Voltages Measurement
*******************************************************************************
*/
void adBms6830_start_fcell_voltage_measurment(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
  pladc_count = adBmsPollAdc(PLADC);
#ifdef MBED
  printf("F Cell voltage conversion completed\n");
#else
  printf("F Cell voltage conversion completed\n");
#endif
  printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read Filtered Cell Voltages
*******************************************************************************
*/
void adBms6830_read_fcell_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDFCA, F_volt, A);
  adBmsReadData(tIC, &ic[0], RDFCB, F_volt, B);
  adBmsReadData(tIC, &ic[0], RDFCC, F_volt, C);
  adBmsReadData(tIC, &ic[0], RDFCD, F_volt, D);
  adBmsReadData(tIC, &ic[0], RDFCE, F_volt, E);
  adBmsReadData(tIC, &ic[0], RDFCF, F_volt, F);
  // printVoltages(tIC, &ic[0], F_volt);
}

/**
*******************************************************************************
* @brief Start AUX, VMV, V+ Voltages Measurement
*******************************************************************************
*/
void adBms6830_start_aux_voltage_measurment(uint8_t tIC, cell_asic *ic)
{
  // for(uint8_t cic = 0; cic < tIC; cic++)
  // {
  //   /* Init config A */
  //   ic[cic].tx_cfga.refon = PWR_UP;
  //   ic[cic].tx_cfga.gpo = 0X3FF; /* All GPIO pull down off 0X3FF*/
  // }
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, A);
  adBms6830_Adax(AUX_OPEN_WIRE_DETECTION, OPEN_WIRE_CURRENT_SOURCE, AUX_CH_TO_CONVERT);
  pladc_count = adBmsPollAdc(PLADC);
#ifdef MBED
  // printf("Aux voltage conversion completed\n");
#else
  // printf("Aux voltage conversion completed\n");
#endif
  // printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read AUX, VMV, V+ Voltages
*******************************************************************************
*/
void adBms6830_read_aux_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDAUXA, Aux, A);
  adBmsReadData(tIC, &ic[0], RDAUXB, Aux, B);
  adBmsReadData(tIC, &ic[0], RDAUXC, Aux, C);
  adBmsReadData(tIC, &ic[0], RDAUXD, Aux, D);
}

/**
*******************************************************************************
* @brief Start Redundant GPIO Voltages Measurement
*******************************************************************************
*/
void adBms6830_start_raux_voltage_measurment(uint8_t tIC,  cell_asic *ic)
{
  for(uint8_t cic = 0; cic < tIC; cic++)
  {
    /* Init config A */
    ic[cic].tx_cfga.refon = PWR_UP;
    ic[cic].tx_cfga.gpo = 0X3FF; /* All GPIO pull down off */
  }
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, A);
  adBms6830_Adax2(AUX_CH_TO_CONVERT);
  pladc_count = adBmsPollAdc(PLADC);
#ifdef MBED
  // printf("RAux voltage conversion completed\n");
#else
  // printf("RAux voltage conversion completed\n");
#endif
  // printPollAdcConvTime(pladc_count);
}

/**
*******************************************************************************
* @brief Read Redundant GPIO Voltages
*******************************************************************************
*/
void adBms6830_read_raux_voltages(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsReadData(tIC, &ic[0], RDRAXA, RAux, A);
  adBmsReadData(tIC, &ic[0], RDRAXB, RAux, B);
  adBmsReadData(tIC, &ic[0], RDRAXC, RAux, C);
  adBmsReadData(tIC, &ic[0], RDRAXD, RAux, D);
  // printVoltages(tIC, &ic[0], RAux);
}

/**
*******************************************************************************
* @brief Read Status Reg. A, B, C, D and E.
*******************************************************************************
*/
void adBms6830_read_status_registers(uint8_t tIC, cell_asic *ic)
{
  adBmsWakeupIc(tIC);
  adBmsWriteData(tIC, &ic[0], WRCFGA, Config, A);
  adBmsWriteData(tIC, &ic[0], WRCFGB, Config, B);
  adBms6830_Adax(AUX_OPEN_WIRE_DETECTION, OPEN_WIRE_CURRENT_SOURCE, AUX_CH_TO_CONVERT);
  pladc_count = adBmsPollAdc(PLADC);
  adBms6830_Adcv(REDUNDANT_MEASUREMENT, CONTINUOUS_MEASUREMENT, DISCHARGE_PERMITTED, RESET_FILTER, CELL_OPEN_WIRE_DETECTION);
  pladc_count = pladc_count + adBmsPollAdc(PLADC);

  adBmsReadData(tIC, &ic[0], RDSTATA, Status, A);
  adBmsReadData(tIC, &ic[0], RDSTATB, Status, B);
  adBmsReadData(tIC, &ic[0], RDSTATC, Status, C);
  adBmsReadData(tIC, &ic[0], RDSTATD, Status, D);
  adBmsReadData(tIC, &ic[0], RDSTATE, Status, E);
  printPollAdcConvTime(pladc_count);
  printStatus(tIC, &ic[0], Status, ALL_GRP);
}

/**
*******************************************************************************
* @brief Loop measurment.
*******************************************************************************
*/
void measurement_loop()
{
  if(MEASURE_CELL == ENABLED)
  {
    adBmsReadData(TOTAL_IC, &IC[0], RDCVA, Cell, A);
    adBmsReadData(TOTAL_IC, &IC[0], RDCVB, Cell, B);
    adBmsReadData(TOTAL_IC, &IC[0], RDCVC, Cell, C);
    adBmsReadData(TOTAL_IC, &IC[0], RDCVD, Cell, D);
    adBmsReadData(TOTAL_IC, &IC[0], RDCVE, Cell, E);
    adBmsReadData(TOTAL_IC, &IC[0], RDCVF, Cell, F);
    // printVoltages(TOTAL_IC, &IC[0], Cell);
  }

  if(MEASURE_AVG_CELL == ENABLED)
  {
    adBmsReadData(TOTAL_IC, &IC[0], RDACA, AvgCell, A);
    adBmsReadData(TOTAL_IC, &IC[0], RDACB, AvgCell, B);
    adBmsReadData(TOTAL_IC, &IC[0], RDACC, AvgCell, C);
    adBmsReadData(TOTAL_IC, &IC[0], RDACD, AvgCell, D);
    adBmsReadData(TOTAL_IC, &IC[0], RDACE, AvgCell, E);
    adBmsReadData(TOTAL_IC, &IC[0], RDACF, AvgCell, F);
    // printVoltages(TOTAL_IC, &IC[0], AvgCell);
  }

  if(MEASURE_F_CELL == ENABLED)
  {
    adBmsReadData(TOTAL_IC, &IC[0], RDFCA, F_volt, A);
    adBmsReadData(TOTAL_IC, &IC[0], RDFCB, F_volt, B);
    adBmsReadData(TOTAL_IC, &IC[0], RDFCC, F_volt, C);
    adBmsReadData(TOTAL_IC, &IC[0], RDFCD, F_volt, D);
    adBmsReadData(TOTAL_IC, &IC[0], RDFCE, F_volt, E);
    adBmsReadData(TOTAL_IC, &IC[0], RDFCF, F_volt, F);
    // printVoltages(TOTAL_IC, &IC[0], F_volt);
  }

  if(MEASURE_S_VOLTAGE == ENABLED)
  {
    adBmsReadData(TOTAL_IC, &IC[0], RDSVA, S_volt, A);
    adBmsReadData(TOTAL_IC, &IC[0], RDSVB, S_volt, B);
    adBmsReadData(TOTAL_IC, &IC[0], RDSVC, S_volt, C);
    adBmsReadData(TOTAL_IC, &IC[0], RDSVD, S_volt, D);
    adBmsReadData(TOTAL_IC, &IC[0], RDSVE, S_volt, E);
    adBmsReadData(TOTAL_IC, &IC[0], RDSVF, S_volt, F);
    // printVoltages(TOTAL_IC, &IC[0], S_volt);
  }

  if(MEASURE_AUX == ENABLED)
  {
    adBms6830_Adax(AUX_OPEN_WIRE_DETECTION, OPEN_WIRE_CURRENT_SOURCE, AUX_CH_TO_CONVERT);
    adBmsPollAdc(PLAUX1);
    adBmsReadData(TOTAL_IC, &IC[0], RDAUXA, Aux, A);
    adBmsReadData(TOTAL_IC, &IC[0], RDAUXB, Aux, B);
    adBmsReadData(TOTAL_IC, &IC[0], RDAUXC, Aux, C);
    adBmsReadData(TOTAL_IC, &IC[0], RDAUXD, Aux, D);
    // printVoltages(TOTAL_IC, &IC[0], Aux);
  }

  if(MEASURE_RAUX == ENABLED)
  {
    adBmsWakeupIc(TOTAL_IC);
    adBms6830_Adax2(AUX_CH_TO_CONVERT);
    adBmsPollAdc(PLAUX2);
    adBmsReadData(TOTAL_IC, &IC[0], RDRAXA, RAux, A);
    adBmsReadData(TOTAL_IC, &IC[0], RDRAXB, RAux, B);
    adBmsReadData(TOTAL_IC, &IC[0], RDRAXC, RAux, C);
    adBmsReadData(TOTAL_IC, &IC[0], RDRAXD, RAux, D);
    // printVoltages(TOTAL_IC, &IC[0], RAux);
  }

  if(MEASURE_STAT == ENABLED)
  {
    adBmsReadData(TOTAL_IC, &IC[0], RDSTATA, Status, A);
    adBmsReadData(TOTAL_IC, &IC[0], RDSTATB, Status, B);
    adBmsReadData(TOTAL_IC, &IC[0], RDSTATC, Status, C);
    adBmsReadData(TOTAL_IC, &IC[0], RDSTATD, Status, D);
    adBmsReadData(TOTAL_IC, &IC[0], RDSTATE, Status, E);
    printStatus(TOTAL_IC, &IC[0], Status, ALL_GRP);
  }
}

/**
*******************************************************************************
* @brief Clear Cell measurement reg.
*******************************************************************************
*/
void adBms6830_clear_cell_measurement(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  spiSendCmd(CLRCELL);
#ifdef MBED
  printf("Cell Registers Cleared\n\n");
#else
  printf("Cell Registers Cleared\n\n");
#endif
}

/**
*******************************************************************************
* @brief Clear Aux measurement reg.
*******************************************************************************
*/
void adBms6830_clear_aux_measurement(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  spiSendCmd(CLRAUX);
#ifdef MBED
  printf("Aux Registers Cleared\n\n");
#else
  printf("Aux Registers Cleared\n\n");
#endif
}

/**
*******************************************************************************
* @brief Clear spin measurement reg.
*******************************************************************************
*/
void adBms6830_clear_spin_measurement(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  spiSendCmd(CLRSPIN);
#ifdef MBED
  printf("Spin Registers Cleared\n\n");
#else
  printf("Spin Registers Cleared\n\n");
#endif
}

/**
*******************************************************************************
* @brief Clear fcell measurement reg.
*******************************************************************************
*/
void adBms6830_clear_fcell_measurement(uint8_t tIC)
{
  adBmsWakeupIc(tIC);
  spiSendCmd(CLRFC);
#ifdef MBED
  printf("Fcell Registers Cleared\n\n");
#else
  printf("Fcell Registers Cleared\n\n");
#endif
}

/** @}*/
/** @}*/