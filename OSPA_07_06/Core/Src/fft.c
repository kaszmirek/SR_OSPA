


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "fft.h"


//--------------------------------------------------------------
static float FFT_CMPLX_DATA[FFT_CMPLX_LENGTH];
static float FFT_MAG_DATA[FFT_LENGTH];

arm_cfft_radix4_instance_f32 S_CFFT;
arm_rfft_instance_f32 S;



//--------------------------------------------------------------
// 1 for ok
// other value for error
//--------------------------------------------------------------
uint32_t fft_init(void)
{
  uint32_t return_val=0;
  arm_status status;
  uint32_t doBitReverse = 1;
  uint32_t ifftFlag = 0;

  status = ARM_MATH_SUCCESS;
  // FFT init
  status = arm_rfft_init_f32(&S, &S_CFFT, FFT_LENGTH, ifftFlag, doBitReverse);
  if(status!=ARM_MATH_SUCCESS) return(0);

  return_val=1;

  return(return_val);
}


//--------------------------------------------------------------
// calculate fft on given buffer
//--------------------------------------------------------------
void fft_calc(void)
{
  float maxValue;
  uint32_t n;


  arm_rfft_f32(&S, FFT_DATA_IN, FFT_CMPLX_DATA);
  arm_cmplx_mag_f32(FFT_CMPLX_DATA, FFT_MAG_DATA, FFT_LENGTH);

  maxValue=0.1;
  for(n=2;n<FFT_VISIBLE_LENGTH;n++) {
    if(FFT_MAG_DATA[n]>maxValue) maxValue=FFT_MAG_DATA[n];
  }

  FFT_UINT_DATA[0]=0;
  FFT_UINT_DATA[1]=0;
  for(n=2;n<FFT_VISIBLE_LENGTH;n++) {
    FFT_UINT_DATA[n]=(uint16_t)(FFT_UINT_MAXdata*FFT_MAG_DATA[n]/maxValue);
  }
}



