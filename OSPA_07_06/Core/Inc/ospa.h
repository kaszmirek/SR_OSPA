//--------------------------------------------------------------
// File     : ospa.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef OSPA_H
#define OSPA_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "main.h"
#include "stm32_ub_led.h"
#include "stm32_ub_lcd_ili9341.h"
#include "stm32_ub_graphic2d.h"
#include "stm32_ub_font.h"
#include "stm32_ub_touch_stmpe811.h"
#include "stm32_ub_uart.h"
#include "stm32_ub_button.h"
#include "adc.h"
#include "fft.h"
#include <stdio.h>



//--------------------------------------------------------------
//--------------------------------------------------------------
#define  SCALE_START_X      13   // dont change
#define  SCALE_START_Y      9    // dont change
#define  FFT_START_Y        31   // dont change
#define  FFT_START_X        20   // dont change

#define  SCALE_SPACE        25   // dont change
#define  SCALE_ANZ_Y        9    // dont change
#define  SCALE_ANZ_X        13   // dont change

#define  SCALE_W            SCALE_SPACE*(SCALE_ANZ_X-1)
#define  SCALE_H            SCALE_SPACE*(SCALE_ANZ_Y-1)

#define  SCALE_MX_PIXEL     SCALE_START_X+SCALE_H
#define  SCALE_MY_PIXEL     SCALE_START_Y+SCALE_W

#define  SCALE_X_MITTE      SCALE_W/2
#define  SCALE_Y_MITTE      SCALE_H/2




//--------------------------------------------------------------
// factor scales
//--------------------------------------------------------------
#define  FAKTOR_5V          SCALE_SPACE/( ADC_MAX*5/VDDA)
#define  FAKTOR_2V          SCALE_SPACE/( ADC_MAX*2/VDDA)
#define  FAKTOR_1V          SCALE_SPACE/( ADC_MAX*1/VDDA)
#define  FAKTOR_0V5         SCALE_SPACE/( ADC_MAX*0.5/VDDA)
#define  FAKTOR_0V2         SCALE_SPACE/( ADC_MAX*0.2/VDDA)
#define  FAKTOR_0V1         SCALE_SPACE/( ADC_MAX*0.1/VDDA)
#define  FAKTOR_0V05        SCALE_SPACE/( ADC_MAX*0.05/VDDA)

//--------------------------------------------------------------
//
//--------------------------------------------------------------
#define  FAKTOR_T           SCALE_W/4095



//--------------------------------------------------------------
// FFT-data -> Pixel-data
//--------------------------------------------------------------
#define  FAKTOR_F           FFT_VISIBLE_LENGTH/4095





//--------------------------------------------------------------
// color defines
//--------------------------------------------------------------
#define  BACKGROUND_COL     RGB_COL_BLACK
#define  FONT_COL           RGB_COL_BLUE
#define  ADC_CH1_COL        RGB_COL_CYAN
#define  ADC_CH2_COL        RGB_COL_YELLOW
#define  SCALE_COL          RGB_COL_GREY
#define  CURSOR_COL         RGB_COL_GREEN
#define  FFT_COL            RGB_COL_MAGENTA


//--------------------------------------------------------------
#define  BMP_HEADER_LEN   54 // Header = 54 Bytes (dont change)
//--------------------------------------------------------------
#define INTERVALL_MS     50 //for mai  loop
//--------------------------------------------------------------
void ospa_init(void);
//--------------------------------------------------------------
uint32_t ospa_hw_init(void);
void ospa_start(void);

void ospa_clear_all(void);
void ospa_draw_background(void);
void ospa_draw_scale(void);
void ospa_draw_line_h(uint16_t xp, uint32_t color, uint16_t m);
void ospa_draw_line_v(uint16_t yp, uint32_t color, uint16_t m);
void ospa_send_uart(uint8_t *ptr);
int16_t ospa_adc2pixel(uint16_t adc, uint32_t faktor);
void ospa_sort_adc(void);
void ospa_fill_fft(void);
void   setup_init(void);
void ospa_menu_show(void);
//Setup struct
typedef struct {
  uint8_t factor_value;//1
  uint8_t trigger_mode;//0
  uint8_t trigger_source;//0
  uint16_t trigger_treshold;//1
  uint8_t v_position;//50
  uint8_t trigger_edge;
  uint8_t trigger_state;
  uint8_t fft_mode;
  uint8_t timebase_value;
  //Channel_t ch1;             // "Channel-1"
  //Timebase_t timebase;       // "Timebase"
  //Trigger_t trigger;         //  "Trigger"
  //FFT_t fft;                 //  "FFT"
}Menu_t;
Menu_t setup;

//--------------------------------------------------------------
#endif
