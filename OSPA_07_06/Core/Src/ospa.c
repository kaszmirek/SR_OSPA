

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ospa.h"

extern TIM_HandleTypeDef htim2;



//--------------------------------------------------------------
// initialization
//--------------------------------------------------------------
void ospa_init(void)
{
  uint32_t check;

  //---------------------------------------------
  // Hardware init
  //--------------------------------------------- 
  check=ospa_hw_init();
  ospa_send_uart((uint8_t *)"Oscyloskop OSPA\r\n");
  if(check==1) {
    // Touch init error - may be not checked
    UB_LCD_FillLayer(BACKGROUND_COL);
  }
  else if(check==2) {

    UB_LCD_FillLayer(BACKGROUND_COL);
    UB_Font_DrawString(10,10,"Wrong ADC Array-LEN",&Arial_7x10,FONT_COL,BACKGROUND_COL);
    while(1);
  }

  UB_Font_DrawString(50,100,"TEST OSCYLOSKOPU",&Arial_7x10,FONT_COL,BACKGROUND_COL);

 // fft_init();

 // ADC_change_Frq(Menu.timebase.value);
}




//--------------------------------------------------------------
// init screen and adc
//--------------------------------------------------------------
uint32_t ospa_hw_init(void)
{
  uint32_t return_val=0;

  // init Touch
  if(UB_Touch_Init()!=SUCCESS) {
    return_val=1; // Touch error
  }

  // Check Defines
  if(ADC_ARRAY_LEN!=SCALE_W) {
    return_val=2; // define error
  }

  // init CD (SD-RAM)
  UB_LCD_Init();
  UB_LCD_LayerInit_Fullscreen();
  UB_LCD_SetMode(LANDSCAPE);

  ospa_clear_all();

  // init ADC structure and start measurments
  ADC_Init_ALL();

  return(return_val);
}



//--------------------------------------------------------------
//aviable
// layer1
// layer2
// layer_menu
// layer adc
// layer back
//--------------------------------------------------------------
void ospa_clear_all(void)
{
  UB_LCD_SetLayer_1();
  UB_LCD_FillLayer(BACKGROUND_COL);
  UB_LCD_SetLayer_2();
  UB_LCD_SetTransparency(255);
  UB_LCD_FillLayer(BACKGROUND_COL);
  //UB_LCD_Copy_Layer2_to_Layer1();
  UB_LCD_SetLayer_Menu();
  UB_LCD_FillLayer(BACKGROUND_COL);
  UB_LCD_SetLayer_ADC();
  UB_LCD_FillLayer(BACKGROUND_COL);
  UB_LCD_SetLayer_Back();
}


//--------------------------------------------------------------
//--------------------------------------------------------------
void ospa_draw_background(void)
{

  UB_LCD_SetLayer_Menu();
  UB_LCD_FillLayer(BACKGROUND_COL);

  //draw scale and cursor
  ospa_draw_scale();
  UB_LCD_SetLayer_Back();
}


//--------------------------------------------------------------
// draws the scale  from the oscillator
//--------------------------------------------------------------
void ospa_draw_scale(void)
{
  uint32_t n,m;
  uint16_t xs,ys;
  int16_t signed_int;

  xs=SCALE_START_X;
  ys=SCALE_START_Y;

  //---------------------------------------------
  // grid of individual points
  //---------------------------------------------
  for(m=0;m<=SCALE_H;m+=SCALE_SPACE) {
    for(n=0;n<=SCALE_W;n+=SCALE_SPACE) {
      UB_Graphic2D_DrawPixelNormal(m+xs,n+ys,SCALE_COL);
    }
  }

  //---------------------------------------------
  // X-axis (horizontal center line)
  //---------------------------------------------
  signed_int=SCALE_Y_MITTE+xs;
  ospa_draw_line_h(signed_int,SCALE_COL,0);

  //---------------------------------------------
  // Y-axis (vertical center line)
  //---------------------------------------------
  signed_int=SCALE_X_MITTE+ys;
  ospa_draw_line_v(signed_int,SCALE_COL,0);
 
  //---------------------------------------------
  // border
  //---------------------------------------------


  UB_Graphic2D_DrawStraightDMA(xs-1,ys-1,SCALE_W+2,LCD_DIR_HORIZONTAL,SCALE_COL);

  UB_Graphic2D_DrawStraightDMA(xs+SCALE_H+1,ys-1,SCALE_W+2,LCD_DIR_HORIZONTAL,SCALE_COL);
  // left
  UB_Graphic2D_DrawStraightDMA(xs-1,ys-1,SCALE_H+2,LCD_DIR_VERTICAL,SCALE_COL);
  // right
  UB_Graphic2D_DrawStraightDMA(xs-1,ys+SCALE_W+1,SCALE_H+2,LCD_DIR_VERTICAL,SCALE_COL);


  //---------------------------------------------
  // trigger line (always visible)
  //---------------------------------------------
  if(setup.trigger_source == 0) {
    signed_int=ospa_adc2pixel(setup.trigger_treshold, setup.factor_value);
    signed_int+=SCALE_Y_MITTE+SCALE_START_X+setup.v_position;
    if(signed_int<SCALE_START_X) signed_int=SCALE_START_X;
    if(signed_int>SCALE_MX_PIXEL) signed_int=SCALE_MX_PIXEL;

    ospa_draw_line_h(signed_int,ADC_CH1_COL,1);
    UB_Font_DrawString(signed_int-3,0,"T",&Arial_7x10,ADC_CH1_COL,BACKGROUND_COL);
  }



}

//--------------------------------------------------------------
// Convert adc-data into pixel position
//--------------------------------------------------------------
int16_t ospa_adc2pixel(uint16_t adc, uint32_t faktor)
{
  int16_t return_val=0;

  switch(faktor) {
    case 0 : return_val=adc*FAKTOR_5V;break;
    case 1 : return_val=adc*FAKTOR_2V;break;
    case 2 : return_val=adc*FAKTOR_1V;break;
    case 3 : return_val=adc*FAKTOR_0V5;break;
    case 4 : return_val=adc*FAKTOR_0V2;break;
    case 5 : return_val=adc*FAKTOR_0V1;break;
    case 6 : return_val=adc*FAKTOR_0V05;break;
  }

  return(return_val);
}

//--------------------------------------------------------------
// draws a horizontal line on the ospa grid
// at "xp", with color "c" and mode "m"
//--------------------------------------------------------------
void ospa_draw_line_h(uint16_t xp, uint32_t color, uint16_t m)
{
  uint32_t n,t;

  if(m==0) {
    // "X----X----X----X----X----X"
    for(n=0;n<=SCALE_W;n+=5) {
      UB_Graphic2D_DrawPixelNormal(xp,n+SCALE_START_Y,color);
    } 
  }
  else if(m==1) {
    //  "X-X-X-X-X-X-X-X-X"
    for(n=0;n<=SCALE_W;n+=2) {
      UB_Graphic2D_DrawPixelNormal(xp,n+SCALE_START_Y,color);
    }
  }
  else if(m==2) {
    //"XX---XX---XX---XX---XX"
    t=0;
    for(n=0;n<=SCALE_W;n++) {
      if(t<2) UB_Graphic2D_DrawPixelNormal(xp,n+SCALE_START_Y,color);
      t++;
      if(t>4) t=0;
    }
  }
}


//--------------------------------------------------------------
// draws a vertical line on the ospa grid at "yp",
// with color "c" and mode "m"
//--------------------------------------------------------------
void ospa_draw_line_v(uint16_t yp, uint32_t color, uint16_t m)
{
  uint32_t n,t;

  if(m==0) {
    // "X----X----X----X----X----X"
    for(n=0;n<=SCALE_H;n+=5) {
      UB_Graphic2D_DrawPixelNormal(n+SCALE_START_X,yp,color);
    } 
  }
  else if(m==1) {
    //  "X-X-X-X-X-X-X-X-X"
    for(n=0;n<=SCALE_H;n+=2) {
      UB_Graphic2D_DrawPixelNormal(n+SCALE_START_X,yp,color);
    }
  }
  else if(m==2) {
    // "XX---XX---XX---XX---XX"
    t=0;
    for(n=0;n<=SCALE_H;n++) {
      if(t<2) UB_Graphic2D_DrawPixelNormal(n+SCALE_START_X,yp,color);
      t++;
      if(t>4) t=0;
    }
  }
}


//--------------------------------------------------------------
// sedn strigg via uart
//--------------------------------------------------------------
void ospa_send_uart(uint8_t *ptr)
{
  UB_Uart_SendString(ptr);
}



