//--------------------------------------------------------------
// File     : stm32_ub_fft.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_FFT_H
#define __STM32F4_UB_FFT_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "main.h"
#include <math.h>
#include <stdint.h>
#include "arm_math.h"

#define DECIMATION_FACTOR       64
#define OUT_FREQ                32000
#define PDM_Input_Buffer_SIZE   ((OUT_FREQ/1000)*DECIMATION_FACTOR/8)
#define PCM_Output_Buffer_SIZE  (OUT_FREQ/1000)

#define SPECTRUM_BG_COLOR       Black
#define SPECTRUM_FREQ_S_kHz     32.0
#define SPECTRUM_HEIGHT         150
#define SPECTRUM_NR_SAMPLES     512
#define SPECTRUM_X_LABEL        "[kHz]"




// sample signals for 29 board on taurus
// 30 not work same code

//--------------------------------------------------------------
// FFT Buffer size
//--------------------------------------------------------------
#define FFT_LENGTH              512 // dont change
#define FFT_CMPLX_LENGTH        FFT_LENGTH*2
#define FFT_VISIBLE_LENGTH      FFT_LENGTH/2

//--------------------------------------------------------------
#define FFT_UINT_MAXdata        100  // max = 100 Pixels / max = 200 Pixels


//--------------------------------------------------------------
// bufor dla fft
//--------------------------------------------------------------
float FFT_DATA_IN[FFT_LENGTH];
uint16_t FFT_UINT_DATA[FFT_VISIBLE_LENGTH];


//--------------------------------------------------------------

uint32_t fft_init(void);
void fft_calc(void);



//--------------------------------------------------------------
#endif // __STM32F4_UB_FFT_H
