/*
 * adc.c
 *
 *  Created on: 3/7/2018
 *      Author: Rai
 */
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_adc.h"
#include "adc.h"
#include "main.h"
#include "display-7seg.h"




ADC_HandleTypeDef hadc;
ADC_HandleTypeDef hadc;
uint32_t valor_ADC;
uint16_t k_adc;
uint8_t         ubAnalogWatchdogStatus = RESET;  /* Set into analog watchdog interrupt callback */

void test_adc(void){

	//HAL_ADC_Start(&hadc);
	ConfigureADC_PA0();

	for (;;)
	{
		if (HAL_ADC_PollForConversion(&hadc, 1000000) == HAL_OK){
			valor_ADC = HAL_ADC_GetValue(&hadc);
			update_valor_display(valor_ADC);
			on_display_all();
		}
	}
}



void update_ADC(void){

	if (HAL_ADC_PollForConversion(&hadc, 1000000) == HAL_OK){
		takeADC_dat(&hadc);
	}

}

void takeADC_dat(ADC_HandleTypeDef* AdcHandle){
	valor_ADC = HAL_ADC_GetValue(AdcHandle);
	valor_ADC = valor_ADC*k_adc;
	valor_ADC = valor_ADC/4095;
	//update_valor_display(valor_ADC);
	//on_display_all();
}


void ConfigureADC_PA0(void){
    GPIO_InitTypeDef gpioInit;
    ADC_ChannelConfTypeDef sConfig;


    k_adc = 3185;					//constante de conversion tomado empiricamente
    hadc.Instance = ADC1;

    //hadc.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    hadc.Init.ScanConvMode = DISABLE;
    hadc.Init.ContinuousConvMode = ENABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    hadc.Init.ExternalTrigConv =  ADC_EXTERNALTRIGCONV_T1_CC4;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //hadc.Init.NbrOfConversion = 1;
    hadc.Init.DMAContinuousRequests = DISABLE;
    //hadc.Init.DMAContinuousRequests = ENABLE;
    hadc.Init.EOCSelection = DISABLE;

    HAL_ADC_Init(&hadc);

    //Configure for the selected ADC regular channel to be converted.

    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    //sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    //sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }


    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);

    // Run the ADC calibration
    if (HAL_ADCEx_Calibration_Start(&hadc) != HAL_OK)
    {
      // Calibration Error
      Error_Handler();
    }

    HAL_ADC_Start_IT(&hadc);           //se lee por interrupcion
    //HAL_ADC_Start(&hadc);				// se lee por pooling

}


void ADC_Config(void){
  ADC_ChannelConfTypeDef   sConfig;
  ADC_AnalogWDGConfTypeDef AnalogWDGConfig;

  /* Configuration of hadc init structure: ADC parameters and regular group */
  k_adc = 3185;					//constante de conversion tomado empiricamente
  hadc.Instance = ADC1;

  hadc.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution            = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;    /* Sequencer will convert the number of channels configured below, successively from the lowest to the highest channel number */
  hadc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait      = DISABLE;
  hadc.Init.LowPowerAutoPowerOff  = DISABLE;
#if defined(ADC_TRIGGER_FROM_TIMER)
  hadc.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
#else
  hadc.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode to have maximum conversion speed (no delay between conversions) */
#endif
  hadc.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
#if defined(ADC_TRIGGER_FROM_TIMER)
  hadc.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_Tx_TRGO;  /* Trig of conversion start done by external event */
  hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;
#else
  hadc.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
  hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because trig of conversion by software start (no external event) */
#endif
  hadc.Init.DMAContinuousRequests = ENABLE;                        /* ADC-DMA continuous requests to match with DMA configured in circular mode */
  hadc.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
  hadc.Init.SamplingTimeCommon    = ADC_SAMPLETIME_41CYCLES_5;

  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    /* ADC initialization error */
    Error_Handler();
  }

  /* Configuration of channel on ADCx regular group on sequencer rank 1 */
  /* Note: Considering IT occurring after each ADC conversion if ADC          */
  /*       conversion is out of the analog watchdog window selected (ADC IT   */
  /*       enabled), select sampling time and ADC clock with sufficient       */
  /*       duration to not create an overhead situation in IRQHandler.        */
  sConfig.Channel      = ADC_CHANNEL_2;
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;

  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
    Error_Handler();
  }

  /* Set analog watchdog thresholds in order to be between steps of DAC       */
  /* voltage.                                                                 */
  /*  - High threshold: between DAC steps 1/2 and 3/4 of full range:          */
  /*                    5/8 of full range (4095 <=> Vdda=3.3V): 2559<=> 2.06V */
  /*  - Low threshold:  between DAC steps 0 and 1/4 of full range:            */
  /*                    1/8 of full range (4095 <=> Vdda=3.3V): 512 <=> 0.41V */

  /* Analog watchdog 1 configuration */
  AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;
  AnalogWDGConfig.Channel = ADC_CHANNEL_2;
  AnalogWDGConfig.ITMode = ENABLE;
  AnalogWDGConfig.HighThreshold = (RANGE_12BITS * 5/8);
  AnalogWDGConfig.LowThreshold = (RANGE_12BITS * 1/8);
  if (HAL_ADC_AnalogWDGConfig(&hadc, &AnalogWDGConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
    Error_Handler();
  }

	  HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(ADC_IRQn);

	 // Run the ADC calibration
	 if (HAL_ADCEx_Calibration_Start(&hadc) != HAL_OK)
	 {
	   // Calibration Error
	   Error_Handler();
	 }

	 //HAL_ADC_Start_IT(&hadc);           //se lee por interrupcion
	 HAL_ADC_Start(&hadc);				// se lee por pooling

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hadc->Instance==ADC1)
  {
	  //Enable the GPIO clock for the pin
	  __GPIOA_CLK_ENABLE();
	  //Enable the ADC clock
	  __ADC1_CLK_ENABLE();

    // USER CODE END ADC1_MspInit 0
    // Peripheral clock enable
    __HAL_RCC_ADC1_CLK_ENABLE();

    // ADC GPIO Configuration
    //PA2     ------> ADC_IN2

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }

}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
 /* Set variable to report analog watchdog out of window status to main      */
 /* program.                                                                 */
 ubAnalogWatchdogStatus = SET;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle){
	takeADC_dat(&hadc);

}

void ADC_IRQHandler(void){
	HAL_ADC_IRQHandler(&hadc);
}


void ADC1_COMP_IRQHandler(void ){
	ADC_IRQHandler();
}
