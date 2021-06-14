//--------------------------------------------------------------
// File     : adc.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef _ADC_H
#define _ADC_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "main.h"
#include "stm32_ub_led.h"

#define  ADC1d_ANZ   1
#define  ADC_ARRAY_LEN         300 // Number of samples
#define  ADC_HALF_ARRAY_LEN    ADC_ARRAY_LEN/2 

#define ADC_BITS				12
#define 	VDDA					3.3
#define  ADC_MAX				4095
#define  FAKTOR_ADC            	VDDA/4095

#define  VFAKTOR_5V             5.0/25
#define  VFAKTOR_2V             2.0/25
#define  VFAKTOR_1V             1.0/25
#define  VFAKTOR_0V5            0.5/25
#define  VFAKTOR_0V2            0.2/25
#define  VFAKTOR_0V1            0.1/25
#define  VFAKTOR_0V05           0.05/25
//--------------------------------------------------------------
// Buffer for data. Size of screen width.
// (300 Pixel x ADC1d_ANZ x 16bit)
//--------------------------------------------------------------
volatile uint16_t ADC_DMA_Buffer_A[ADC1d_ANZ*ADC_ARRAY_LEN];  //  (A) from DMA
volatile uint16_t ADC_DMA_Buffer_B[ADC1d_ANZ*ADC_ARRAY_LEN];  //  (B) from DMA
volatile uint16_t ADC_DMA_Buffer_C[ADC1d_ANZ*ADC_ARRAY_LEN];  //  (C) for fft

//define ADC Timer for easy change
#define ADC_TIM 			htim2
#define ADC1_DMA_STREAM            DMA2_Stream0
#define ADC1_DMA_CHANNEL           DMA_Channel_0

//--------------------------------------------------------------
// Timer-2
//--------------------------------------------------------------
#define OSPA_TIM2_PERIODE      299//check if still correct
#define OSPA_TIM2_PRESCALE     83
#define DMA_MODE_SingleBF 2

//--------------------------------------------------------------
typedef enum {
  ADC_FF = 0,
  ADC_RUNNING,
  ADC_PRE_TRIGGER,
  ADC_TRIGGER_OK,
  ADC_READY
}ADC_Status_t;

//--------------------------------------------------------------
typedef struct {
  ADC_Status_t status;
  uint32_t trigger_pos;
  uint32_t trigger_quarter;
  uint32_t dma_status;// 0 for normal operation , 1 for stop of conversion

}ADC_t;
ADC_t OSPA_ADC;

//--------------------------------------------------------------
void ADC_Init_ALL(void);
void ADC_change_Frq(uint8_t n);
void ADC_change_Mode(uint8_t mode);
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
//--------------------------------------------------------------
//--------------------------------------------------------------
void P_ADC_InitADC(void);//ok
void P_ADC_Start(void);//ok
void P_ADC_Clear(void);//ok
//trigger search in buffor
void ADC_searchTrigger_A1(void);
void ADC_searchTrigger_B1(void);
void ADC_searchTrigger_A2(void);
void ADC_searchTrigger_B2(void);
//--------------------------------------------------------------

#endif
