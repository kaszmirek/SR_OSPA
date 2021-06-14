

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ospa.h"

extern TIM_HandleTypeDef htim2;

volatile uint32_t  Timer_ms=0;

uint8_t stop_flag=0;
uint8_t frequency =10;
uint8_t last_freq = 200;
uint8_t voltage_amp = 33 ;
uint8_t last_voltage_amp = 33 ;
int16_t max_value_noconv=0;
int16_t min_value_noconv=0;
float min_value_conv=0;
float max_value_conv=0;
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
  //ospa_send_uart((uint8_t *)"Oscyloskop OSPA test\r\n");
  printf("Oscyloskop OSPA test numer 3\r\n");
  //ospa_send_uart((uint8_t *)"Oscyloskop OSPA test numer 2\r\n");
  if(check==1) {
    // Touch init error - may be not checked
    UB_LCD_FillLayer(BACKGROUND_COL);
  }
  else if(check==2) {

    UB_LCD_FillLayer(BACKGROUND_COL);
    UB_Font_DrawString(10,10,"Wrong ADC Array-LEN",&Arial_7x10,FONT_COL,BACKGROUND_COL);
    while(1);
  }

  //UB_Font_DrawString(50,100,"TEST OSCYLOSKOPU",&Arial_7x10,FONT_COL,BACKGROUND_COL);
  setup_init();
  fft_init();
  //UB_Uart_Init();
  ADC_change_Frq(setup.timebase_value);

  for(int i=0; i<300; ++i)
  	  {
  	  ADC_DMA_Buffer_A[i] = (uint16_t)  voltage_amp*2048/33*sin(2*PI*frequency*i*0.001)+2048;
      }
  ospa_menu_show();
}


void setup_init(void)
{
	setup.factor_value =  2 ;//1
	setup.trigger_mode = 0;//0
	setup.trigger_source = 0;//0
	setup.trigger_treshold = 3000;//1
	setup.v_position = 0;//50
	setup.trigger_edge=0;
	setup.trigger_state = 1;
	setup.fft_mode=1;
	setup.timebase_value = 4;
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
  //ADC_Init_ALL();
  __HAL_TIM_DISABLE(&ADC_TIM);

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

  //UB_LCD_SetLayer_Menu();
	UB_LCD_SetLayer_2();
	UB_LCD_FillLayer(BACKGROUND_COL);

  //draw scale and cursor
  ospa_draw_scale();
  UB_LCD_SetLayer_Back();
}


//--------------------------------------------------------------
// draws the scale
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
    if(signed_int<SCALE_START_X)
    	signed_int=SCALE_START_X;
    if(signed_int>SCALE_MX_PIXEL)
    	signed_int=SCALE_MX_PIXEL;

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
//--------------------------------------------------------------
void ospa_send_uart(uint8_t *ptr)
{
  UB_Uart_SendString(ptr);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void ospa_draw_adc(void)
{
  uint32_t n=0;
  int16_t ch1_data1,ch1_data2;

  int16_t fft_data1,fft_data2;

  ospa_draw_background();
  //UB_LCD_SetLayer_Menu();
  UB_LCD_SetLayer_2();


  ch1_data1=ospa_adc2pixel(ADC_DMA_Buffer_C[0], setup.factor_value);
  ch1_data1+=SCALE_Y_MITTE+SCALE_START_X+setup.v_position;
  if(ch1_data1<SCALE_START_X)
	  ch1_data1=SCALE_START_X;
  if(ch1_data1>SCALE_MX_PIXEL)
	  ch1_data1=SCALE_MX_PIXEL;



  fft_data1=FFT_UINT_DATA[0];
  fft_data1+=FFT_START_X;
  if(fft_data1<SCALE_START_X)
	  fft_data1=SCALE_START_X;
  if(fft_data1>SCALE_MX_PIXEL)
	  fft_data1=SCALE_MX_PIXEL;

min_value_noconv=5000;
max_value_noconv=0;
  for(n=1;n<SCALE_W;n++) {
	  int16_t data =ADC_DMA_Buffer_C[n];
	  if(data<min_value_noconv)
		  min_value_noconv=data;
	  if(data>max_value_noconv)
		  max_value_noconv=data;
      ch1_data2=ospa_adc2pixel(data, setup.factor_value);
      ch1_data2+=SCALE_Y_MITTE+SCALE_START_X+setup.v_position;
      if(ch1_data2<SCALE_START_X) ch1_data2=SCALE_START_X;
      if(ch1_data2>SCALE_MX_PIXEL) ch1_data2=SCALE_MX_PIXEL;
      UB_Graphic2D_DrawLineNormal(ch1_data1,SCALE_START_Y+n,ch1_data2,SCALE_START_Y+n+1,ADC_CH1_COL);
      ch1_data1=ch1_data2;

  }

min_value_conv=min_value_noconv*3.3/4095;
max_value_conv=max_value_noconv*3.3/4095;

    for(n=1;n<FFT_VISIBLE_LENGTH;n++) {
      fft_data2=FFT_UINT_DATA[n];
      fft_data2+=FFT_START_X;
      if(fft_data2<SCALE_START_X) fft_data2=SCALE_START_X;
      if(fft_data2>SCALE_MX_PIXEL) fft_data2=SCALE_MX_PIXEL;
      UB_Graphic2D_DrawLineNormal(fft_data1,FFT_START_Y+n,fft_data2,FFT_START_Y+n+1,FFT_COL);
      fft_data1=fft_data2;
    }

    //UB_Font_DrawString(20,200,"FREQ:10Hz",&Arial_7x10,ADC_CH1_COL,BACKGROUND_COL);

  UB_LCD_SetLayer_Back();
}




//--------------------------------------------------------------
//--------------------------------------------------------------
void ospa_start(void)
{


  ospa_draw_background();


  while(1)
  {


	  	  	  if(stop_flag==0){

	     	 if(Timer_ms==0)
	     	 {
	     	Timer_ms=INTERVALL_MS;

	     	//-----------------------------DATA_GENERATION------------------------------

	     	  if(last_freq!= frequency||last_voltage_amp!=voltage_amp)
	     		  {
	     			  for(int i=0; i<300; ++i)
	     			  	  {
	     			  	  ADC_DMA_Buffer_A[i] = (uint16_t)  voltage_amp*2048/33*sin(2*PI*frequency*i*0.001)+2048;
	     			      }
	     			  last_freq=frequency;
	     			  last_voltage_amp=voltage_amp;
	     		  }
	     	//--------------------------------CODE------------------------------
	     	 ospa_draw_background();

	         	// ospa_draw_background();

	            //ospa_draw_background();
	            // ospa_draw_adc();



	            //OSPA_ADC.status=ADC_READY;
	            OSPA_ADC.status=ADC_RUNNING;
	            ADC_searchTrigger_A1();
	            if(OSPA_ADC.status!=ADC_TRIGGER_OK)
	            ADC_searchTrigger_A2();



	            if(OSPA_ADC.status==ADC_TRIGGER_OK) {
	            	OSPA_ADC.status=ADC_READY;
	            }
	            	else
	            	OSPA_ADC.status=ADC_FF;


	        if(setup.trigger_mode==0) {
	           //--------------------------------------

	           //--------------------------------------
	           if(setup.trigger_state==1) {
	             if(OSPA_ADC.status==ADC_READY) {
	               UB_Led_Toggle(LED_RED);
	               ospa_sort_adc();
	               ospa_fill_fft();
	               fft_calc();
	               ospa_draw_adc();
	               OSPA_ADC.status=ADC_FF;
	               __HAL_TIM_ENABLE(&htim2);
	             }
	             else {
	               //do not draw anything, trigger did not reach treshold
	                //ospa_draw_adc();
	            	 ospa_draw_background();
	             }
	           }
	           else if(setup.trigger_state==0) {
	             // Button "STOP" wurde gedr�ckt
	             // Timer analten
	             __HAL_TIM_DISABLE(&htim2);
	             ospa_draw_adc();
	           }
	           else if(setup.trigger_state==2) {

	        	   setup.trigger_state=1;
	             OSPA_ADC.status=ADC_FF;
	             __HAL_TIM_ENABLE(&htim2);
	              ospa_draw_adc();
	           }
	         }

	         UB_LCD_Refresh();


	       }



	  }


  }
  }




void ospa_sort_adc(void)
{
  uint32_t n=0;
  uint32_t start=0,anz1=0,anz2=0;
  uint16_t data;

  OSPA_ADC.trigger_quarter=1;

  if(OSPA_ADC.trigger_quarter==1) {
    //-------------------------------
    // The trigger point is in Q1
    //-------------------------------
    anz1=(SCALE_X_MITTE-OSPA_ADC.trigger_pos);
    start=SCALE_X_MITTE-anz1;

    //-------------------------------
    // copy left part
    //-------------------------------
    for(n=0;n<anz1;n++) {
      data=ADC_DMA_Buffer_A[(start+n)];
      ADC_DMA_Buffer_C[n]=data;
      //data=ADC_DMA_Buffer_A[((start+n))+1];
      //ADC_DMA_Buffer_C[(n*2)+1]=data;
    }
    //-------------------------------
    // copy right part
    //-------------------------------
    anz2=SCALE_W-anz1;
    start=0;
    for(n=0;n<SCALE_X_MITTE;n++) {
      data=ADC_DMA_Buffer_A[(SCALE_X_MITTE+n)];
      ADC_DMA_Buffer_C[(n+anz1)]=data;
     // data=ADC_DMA_Buffer_A[((start+n)*2)+1];
     // ADC_DMA_Buffer_C[((n+anz1)*2)+1]=data;
    }

    for(n=0;n<SCALE_X_MITTE-anz1;n++)
    {
    	data=ADC_DMA_Buffer_A[n];
    	ADC_DMA_Buffer_C[(SCALE_X_MITTE+anz1+n)]=data;
    }
  }
  else if(OSPA_ADC.trigger_quarter==2) {
    //-------------------------------
    // The trigger point is in Q2
    //-------------------------------
    anz1=SCALE_W-((OSPA_ADC.trigger_pos-SCALE_X_MITTE));
    start=SCALE_W-anz1;

    //-------------------------------
    // copy left part
    //-------------------------------
    for(n=0;n<anz1;n++) {
      data=ADC_DMA_Buffer_A[(start+n)*2];
      ADC_DMA_Buffer_C[n*2]=data;
      data=ADC_DMA_Buffer_A[((start+n)*2)+1];
      ADC_DMA_Buffer_C[(n*2)+1]=data;
    }
    //-------------------------------
    // copy right part
    //-------------------------------
    anz2=SCALE_W-anz1;
    start=0;
    for(n=0;n<anz2;n++) {
      data=ADC_DMA_Buffer_B[(start+n)*2];
      ADC_DMA_Buffer_C[(n+anz1)*2]=data;
      data=ADC_DMA_Buffer_B[((start+n)*2)+1];
      ADC_DMA_Buffer_C[((n+anz1)*2)+1]=data;
    }
  }

  }




//--------------------------------------------------------------
// fills the FFT input buffer with the
// sample data from CH1
//--------------------------------------------------------------
void ospa_fill_fft(void)
{
  uint32_t n,m;

  if(setup.fft_mode==1) {
    m=0;
    for(n=0;n<FFT_LENGTH;n++) {
      if(m<SCALE_W) {
        FFT_DATA_IN[n]=(float)((ADC_DMA_Buffer_C[(m)]-2048.0)/1000.0);
      }
      else {
        FFT_DATA_IN[n]=0.0;
      }
      m++;
    }
  }

}

void ospa_menu_show(void)
{
	printf("********************************\r\n");
	printf("*   OSCYLOSKOP OSPA WITA!!!    *\r\n");
	printf("********************************\r\n");
	printf("* Dostępne opcje:              *\r\n");
	printf("* 1.Zmien poziom wyzwalania    *\r\n");
	printf("* 2.Zmien skale pionowa        *\r\n");
	printf("* 3.Zmien skale pozioma        *\r\n");
	printf("* 4.Zmien czestotliwosc sygnalu*\r\n");
	printf("* 5.Zmien amplitude sygnalu    *\r\n");
	printf("* 6. START/STOP                *\r\n");
	printf("* 7. Wyswietl pomiar           *\r\n");
	printf("********************************\r\n");
	printf("Wybierz opcje: ");
	fflush(stdout);
}

