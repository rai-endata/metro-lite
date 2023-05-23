/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

//#include "stm32f091xc.h"
//#include "stm32f0xx_hal.h"
//#include "stm32f0xx_hal_tim.h"
//#include "- metroBLUE Config -.h"
#include "main.h"
#include "Reloj.h"
#include "usart7.h"
#include "usart1.h"
#include "DA Tx.h"
#include "Odometro.h"
#include "Tarifacion Reloj.h"
#include "Timer.h"
#include "Calculos.h"
#include "rtc.h"
#include "display-7seg.h"
//#include "stm32f0xx_Re_Blue.h"
#include "teclas.h"
#include "test_eeprom.h"
#include "driver_eeprom.h"
#include "eeprom.h"
#include "spi.h"
#include "Crc.h"
#include "inicio.h"
#include "Bluetooth.h"
#include "Panico.h"
#include "Comandos sin conexion a central.h"
#include "Reportes.h"
//#include "watchdog.h"
#include "DA Define Commands.h"
#include "Comandos sin conexion a central.h"


//#include "file aux1.h"

/* Private function prototypes -----------------------------------------------*/
//static void SystemClockEX_Config(void);
//void SystemClock_Config(void);
//void SystemClock_Config1(void);
//void MX_RTC_Init(void);
void watchDOG (void);
void set_TIMEandDATE (void);

void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void PVD_Config(void);
static void InterruptPVD_When_VDD_ON_Config(void);
static void InterruptPVD_When_VDD_OFF_Config(void);
//static void clrRAM (byte* ini, byte* fin);

PWR_PVDTypeDef sConfigPVD;



RTC_HandleTypeDef	hrtc;
UART_HandleTypeDef	huart1;
UART_HandleTypeDef	huart7;
RTC_HandleTypeDef	hrtc;

uint8_t rx_buff[200];
uint8_t tx_buff[200];

/* USER CODE END 0 */
uint32_t ESTADO_BUTTON;

byte buffPULSE_ACCUM_CNT[20];
uint32_t auxCNT;
uint32_t cntIC;
uint32_t cntIC_anterior;

mainFLAGS1	_VISOR_F1;			// Variables Generales
byte 		MOTIVO_RESET;		// Motivo por el cual se reseteo el equipo


//#define		sbss 0x20000438
//#define		ebss 0x20000468
/*
#define		sdata	0x20000000
#define		edata	0x200007b8
#define		sidata	0x80206d0
#define		sbss	0x200007b8
#define		ebss	0x2000517c
*/



void SystemInit_test(void);

// VARIABLES DE FLASH
//#define valorPrimerEncendido      0x5555



byte* ptrESTADO_RELOJ;
byte* ptr1;
byte* ptr2;



int main(void)
{
	HAL_Init();				//definiciones necesarias para el uso del HAL
	ini_display_7seg();
	SystemClock_Config();
	set_tipo_de_equipo();
	//IWDG_Config();		// Configurar el m�dulo IWDG
	MX_SPIx_Init();			//ini spi
	EPROM_CS_Init();
	Ini_portBANDERITA();
	MX_GPIO_Init();

	//set_choice_device_uart1();
	choice_device_uart1 = PROG_DEVICE;
	USART1_Ini();
	USART7_Ini();
	MX_TIM2_Init4();
	HAL_TIM_IC_Start_IT(&pulsoACCUM,TIM_CHANNEL_1);
	MX_RTC_Init();

	VA_UART_ini();
	DA_iniRx();
	DA_iniTx();
	PRINT_ptrIni ();												//inicializo punteros de comandos
	buzzer_ini();
	PROGRAMADOR_ini();
	SOURCE_DATE_Ini();
	levantar_variablesEEPROM();
	ini_pulsador_cambio_reloj();
	JUMPER_PROG_Init();

	Bluetooth_Ini();
	Panico_Ini();
	ini_puerto_sensor_asiento();
	BanderaOut_Ini();

	clr_BANDERA();

	//inicializar asiento luego de tarifa (setea carrera_bandera)

	ASIENTO_ini();
	determinePrimerEncendido();         	//Determinar si se trata del 1er Encendido

	//Configuraci�n de contador de pulsos
	DISTANCIA_setPulsosCntOld(0);
	__HAL_TIM_SET_COUNTER(&pulsoACCUM,0);
	PULSE_ACCUM_CNT = __HAL_TIM_GetCounter(&pulsoACCUM);
	auxCNT = PULSE_ACCUM_CNT;

	#ifdef VISOR_REPORTES
		iniREPORTES();           //Inicializacion de Reportes y Reporte de 30 Dias
	#endif

	check_corte_alimentacion();

	Tx_Encendido_EQUIPO();		    			//encendido de EQUIPO
	PVD_Config();
	InterruptPVD_When_VDD_OFF_Config();

	ini_acumular_cmdSC();

#ifdef RELOJ_DEBUG
	iniPRINT_DEBUG();
#endif

	DISTANCIA_iniCalculo_PULSE_ACCUM();


// *****  Lazo Principal *************

	for(;;){
		//IWDG->KR = IWDG_KEY_RELOAD;	// Realizar la recarga del contador del IWDG
		//procesar_datosSC();				//procesa datos sin conexion

		check_datosSC();

		// REPORTES
		#ifdef VISOR_REPORTES
		  REPORTES_grabarFlash();      	// Grabacion de reportes en FLASH
		#endif

		check_relojBANDERITA();
		
		check_pressPANICO();
		check_pressTECLA();
		//rtc__actDATE();
		set_TIMEandDATE();
		TMR_GRAL_LOOP();
		relojINTERNO_updateCHOFER();
		//cntIC = __HAL_TIM_GetCompare(&pulsoACCUM,TIM_CHANNEL_1);
		PULSE_ACCUM_CNT = __HAL_TIM_GetCounter(&pulsoACCUM);
		if(ESTADO_RELOJ != COBRANDO){
			(void)calcularDISTANCIA_acumulada;			//actualiza calculo de distancia
			DISTANCIAkm = DISTANCIAm/100; 				//con un decimal
		}

		firstDATE();
		tarifarRELOJ();

		//RECEPCION DE DA ************
		guardarRxDA_BaxFORMAT();			//toma datos recibidos del DA, y los guarda con protocolo BAX en rxVA_baxFORMAT.RxBuffer
		RxDA_BaxFORMAT_toAIR_RxBuffer();	//toma datos de rxDA_baxFORMAT.RxBuffer con protocolo BAX y los guarda AIR_RxBuffer
		RxDA_process();					     //toma datos de AIR_RxBuffer y los procesa
		//TRANSMISION A DA
		pasarCMDS_BUFFER_to_TxBUFFER();		//pasa datos del buffer del comando a transmitir al buffer de transmisi�n
		DA_Tx();							//pasa datos del buffer de transmisi�n al buffer de salida e inicia la transmisi�n
		//****************************

		ASIENTO_consultaSensor();       	// Consulta sensor presionado o no
		RELOJ_a_pagar_to_libre();
		//grabar_enFLASH();
	    //PROGRAMADOR
	    PROGRAMADOR_chkRx();              //Chequeo de Recepcion de datos de Programacion
		PROGRAMADOR_chkTx();			  //
		PROGRAMADOR_fin();                //Fin de Programacion de Parametros aka Fin Grabacion EEPROM
		EEPROM_chkRequest(1);			  //
	    chkProgMode();
	    ModoPROGRAMACION();
	    //finTxRta_actions();               // Ejecuto Acciones que esperan el fin de Tx Rta (se recomienda que este debajo de proceso de IRQ MODEM)
	    AIR_UPDATE_update ();             // Actualizacion por AIRE

#ifdef RELOJ_DEBUG
	    PRINT_DEBUG_imprimir();
#endif
	}
}

// ********* fin Lazo principal *****************


void ModoPROGRAMACION (void){
	while(prog_mode){
	    PROGRAMADOR_chkRx();              //Chequeo de Recepcion de datos de Programacion
	    PROGRAMADOR_chkTx();			  //
	    PROGRAMADOR_fin();                //Fin de Programacion de Parametros aka Fin Grabacion EEPROM
	    EEPROM_chkRequest(1);			  //
	    TMR_GRAL_LOOP();

		//RECEPCION DE DA ************
		guardarRxDA_BaxFORMAT();			//toma datos recibidos del DA, y los guarda con protocolo BAX en rxVA_baxFORMAT.RxBuffer
		RxDA_BaxFORMAT_toAIR_RxBuffer();	//toma datos de rxDA_baxFORMAT.RxBuffer con protocolo BAX y los guarda AIR_RxBuffer
		RxDA_process();					     //toma datos de AIR_RxBuffer y los procesa
		//TRANSMISION A DA
		pasarCMDS_BUFFER_to_TxBUFFER();		//pasa datos del buffer del comando a transmitir al buffer de transmisi�n
		DA_Tx();							//pasa datos del buffer de transmisi�n al buffer de salida e inicia la transmisi�n
		//****************************
	}
}



void set_TIMEandDATE (void){

	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	getDate();

    if(RTC_Date.fecha[2]==0x00){
    	sTime.Hours = 13;
    	sTime.Minutes = 38;
    	sTime.Seconds = 00;
    	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}

    	sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
    	sDate.Month = RTC_MONTH_APRIL;
    	sDate.Date = 17;
    	sDate.Year = 18;
		if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
		{
		_Error_Handler(__FILE__, __LINE__);
		}
    }

}



	void watchDOG (void) {
		//HAL_Delay(1000);
		BSP_LED_Toggle(LED4);
	}





	void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
		uint32_t diff_pulsos,diff_tiempo;

		if (htim->Instance==TIM2){ 											//check if the interrupt comes from TIM2
			cntIC = __HAL_TIM_GetCounter(&pulsoACCUM);	// read capture value

			if((t_pulsos - t_pulsos_anterior) > 0){
				diff_tiempo = t_pulsos - t_pulsos_anterior;
			}else if ((t_pulsos - t_pulsos_anterior) < 0 ){
				diff_tiempo = t_pulsos - (0xffffffff - t_pulsos_anterior);
			}else {
				diff_tiempo = 0;
			}

			if(diff_tiempo >= 1000){

				//diferencia de pulsos
				if(cntIC > cntIC_anterior){
					diff_pulsos = cntIC -cntIC_anterior;
				}else{
					diff_pulsos = cntIC +(0xffffffff-cntIC_anterior);
				}

				frecuencia = (100*1000*diff_pulsos)/diff_tiempo;	//con dos decimales

				t_pulsos_anterior = t_pulsos;
				cntIC_anterior   = cntIC;
			}
			VELOCIDAD = (3600*frecuencia)/PULSOS_x_KM;        //con dos decimales

			if(ESTADO_RELOJ != COBRANDO){
				if (VELOCIDAD_MAX < (VELOCIDAD/100)){
					VELOCIDAD_MAX = (VELOCIDAD/100);  // Divido x100 para dejarla sin los 2 decimales
				}
			}
			timeOut_FRECUENCIA_cnt = 3;
		}
	}

	/**
	  * @brief  This function is executed in case of error occurrence.
	  * @param  None
	  * @retval None
	  */
	void _Error_Handler(char * file, int line)
	{
	  /* USER CODE BEGIN Error_Handler_Debug */
	  /* User can add his own implementation to report the HAL error return state */
	  uint8_t salir;
	  salir=1;
	  while(salir)
	  {
		  //BSP_LED_On(LED4);
	  }
	  /* USER CODE END Error_Handler_Debug */
	}

/*
	static void SystemClockEX_Config(void)
	{

	  RCC_OscInitTypeDef RCC_OscInitStruct;
	  RCC_ClkInitTypeDef RCC_ClkInitStruct;

	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	  HAL_RCC_OscConfig(&RCC_OscInitStruct);

	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

	  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	  // SysTick_IRQn interrupt configuration
	  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	}

	// RTC init function
	void MX_RTC_Init(void)
	{
	    //Initialize RTC and set the Time and Date
	  hrtc.Instance = RTC;
	  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	  hrtc.Init.AsynchPrediv = 127;
	  hrtc.Init.SynchPrediv = 255;
	  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	  HAL_RTC_Init(&hrtc);

	    //Enable the WakeUp
	  HAL_RTCEx_SetWakeUpTimer(&hrtc, 0, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

	    //Enable the TimeStamp
	  HAL_RTCEx_SetTimeStamp_IT(&hrtc, RTC_TIMESTAMPEDGE_RISING, RTC_TIMESTAMPPIN_PC13);

	}
	*/

	void SystemClock_Config(void)
	{

	  RCC_OscInitTypeDef RCC_OscInitStruct;
	  RCC_ClkInitTypeDef RCC_ClkInitStruct;

	  //Configure LSE Drive Capability
	  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);

      //Initializes the CPU, AHB and APB busses clocks
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_HSI14;
	  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
	  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
	  //adc
	  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
	  RCC_OscInitStruct.HSI14CalibrationValue = 16;

	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    _Error_Handler(__FILE__, __LINE__);
	  }

	  //Initializes the CPU, AHB and APB busses clocks
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	  {
	    _Error_Handler(__FILE__, __LINE__);
	  }

	 //Configure the Systick interrupt time
	  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	  //Configure the Systick
	  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	  //SysTick_IRQn interrupt configuration
	  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	}

	// RTC init function
	void MX_RTC_Init(void)
	{

	  RTC_TimeTypeDef sTime;
	  RTC_DateTypeDef sDate;

	    //Initialize RTC Only

	  hrtc.Instance = RTC;
	  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	  hrtc.Init.AsynchPrediv = 127;
	  hrtc.Init.SynchPrediv = 255;
	  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	  if (HAL_RTC_Init(&hrtc) != HAL_OK)
	  {
	    _Error_Handler(__FILE__, __LINE__);
	  }


	 // if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2){
	 //   HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x32F2);
	 // }

	}

/*
	// TIM2 init function
	static void MX_TIM2_Init(void)
	{

	  TIM_MasterConfigTypeDef sMasterConfig;
	  TIM_IC_InitTypeDef sConfigIC;

	  htim2.Instance = TIM2;
	  htim2.Init.Prescaler = 0;
	  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	  htim2.Init.Period = 0xffffffff;
	  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
	  {
	    _Error_Handler(__FILE__, __LINE__);
	  }

	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	  {
	    _Error_Handler(__FILE__, __LINE__);
	  }

	  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	  sConfigIC.ICFilter = 15;
	  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
	  {
	    _Error_Handler(__FILE__, __LINE__);
	  }

	}

	// USART1 init function
	static void MX_USART1_UART_Init(void)
	{

	  huart1.Instance = USART1;
	  huart1.Init.BaudRate = 9600;
	  huart1.Init.WordLength = UART_WORDLENGTH_8B;
	  huart1.Init.StopBits = UART_STOPBITS_1;
	  huart1.Init.Parity = UART_PARITY_NONE;
	  huart1.Init.Mode = UART_MODE_TX_RX;
	  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	  if (HAL_UART_Init(&huart1) != HAL_OK)
	  {
	    _Error_Handler(__FILE__, __LINE__);
	  }

	}

	// USART7 init function
	static void MX_USART7_UART_Init(void)
	{

	  huart7.Instance = USART7;
	  huart7.Init.BaudRate = 9600;
	  huart7.Init.WordLength = UART_WORDLENGTH_8B;
	  huart7.Init.StopBits = UART_STOPBITS_1;
	  huart7.Init.Parity = UART_PARITY_NONE;
	  huart7.Init.Mode = UART_MODE_TX_RX;
	  huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart7.Init.OverSampling = UART_OVERSAMPLING_16;
	  huart7.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	  huart7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	  if (HAL_UART_Init(&huart7) != HAL_OK)
	  {
	    _Error_Handler(__FILE__, __LINE__);
	  }

	}
*/

	/** Configure pins as
	        * Analog
	        * Input
	        * Output
	        * EVENT_OUT
	        * EXTI
	     PA2   ------> USART2_TX
	     PA3   ------> USART2_RX
	*/
	static void MX_GPIO_Init(void)
	{

	  GPIO_InitTypeDef GPIO_InitStruct;

	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOF_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();


	  /*Configure GPIO pin : B1_Pin */
	  //GPIO_InitStruct.Pin = B1_Pin;
	  //GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	  //GPIO_InitStruct.Pull = GPIO_NOPULL;
	  //HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
	  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	  /*Configure GPIO pin : LD2_Pin */

	  //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET); //Configure GPIO pin Output Level

	  ////GPIO_InitStruct.Pin = LD2_Pin;
	  ////GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  ////GPIO_InitStruct.Pull = GPIO_NOPULL;
	  ///GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  ////HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

	}

	void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

		GPIO_PinState pinSTATE;


		if(GPIO_Pin == GPIO_PIN_5){//la interrupcion se debe al pin 5 ?
			check_pressT1();
		}else if(GPIO_Pin == GPIO_PIN_11){//la interrupcion se debe al pin 11 ?
			//FUERA_SERVICIO/LIBRE - IMPRESION EN COBRANDO
			check_pressT2();
			//JUMPER DE PROGRAMACION
			//check_jumperPROG();

		}else if(GPIO_Pin == GPIO_PIN_3){//la interrupcion se debe al pin 3 ?
			//CONEXION BLUETOOTH
			//check_pressBLUETOOTH();
			setTO_Bluetooth(TO_BLUETOOTH_1);
			//Bluetooth_to_cnt = BLUETOOTH_TIMEOUT;
			/*
			if(bluetoothCONECTADO){
				//Tx_Status_RELOJ();
				if(ESTADO_RELOJ == OCUPADO || ESTADO_RELOJ== COBRANDO){
					Tx_Valor_VIAJE();
				}
			}*/

		}else if(GPIO_Pin == GPIO_PIN_2){//la interrupcion se debe al pin 2 ?
			//panico
			check_pressPANICO();
		}
	}

	static void PVD_Config(void)
	{

/*
PVD Out		        VDD > VPDthreshold        VDD < VPDthreshold
	--------------------+                         +---------------
						+                         +
						+                         +
						+                         +
						+-------------------------+
						fallling                rising

Table 27. Programmable voltage detector characteristics
							Min    Typ    Max Unit

VPVD0 PVD threshold 0
Rising edge 				2.1    2.18   2.26 V
Falling edge 				2 	   2.08   2.16 V

VPVD1 PVD threshold 1
Rising edge 				2.19   2.28   2.37 V
Falling edge 				2.09   2.18   2.27 V
VPVD2 PVD threshold 2
Rising	edge 				2.28   2.38   2.48 V
Falling edge 				2.18   2.28   2.38 V
VPVD3 PVD threshold 3
Rising edge 				2.38   2.48   2.58 V
Falling edge 				2.28   2.38   2.48 V
VPVD4 PVD threshold 4
Rising edge 				2.47   2.58   2.69 V
Falling edge 				2.37   2.48   2.59 V
VPVD5 PVD threshold 5
Rising edge 				2.57   2.68   2.79 V
Falling edge 				2.47   2.58   2.69 V
VPVD6 PVD threshold 6
Rising edge 				2.66   2.78   2.9 V
Falling edge 				2.56   2.68   2.8 V
VPVD7 PVD threshold 7
Rising edge 				2.76   2.88   3 V
Falling edge 				2.66   2.78   2.9 V
VPVDhyst
(1) PVD hysteresis - - 100 - mV
IDD(PVD) PVD current consumption - - 0.15 0.26(1) microA
*/

		/*##-1- Enable Power Clock #################################################*/
	  __HAL_RCC_PWR_CLK_ENABLE();

	  /*##-2- Configure the NVIC for PVD #########################################*/
	  HAL_NVIC_SetPriority(PVD_VDDIO2_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(PVD_VDDIO2_IRQn);

	}

	static void InterruptPVD_When_VDD_ON_Config(void){
		  sConfigPVD.PVDLevel = PWR_PVDLEVEL_7;
		  sConfigPVD.Mode = PWR_PVD_MODE_EVENT_FALLING;  //interrumpe cuando VDD > VPDthreshold
		  HAL_PWR_ConfigPVD(&sConfigPVD);
		  HAL_PWR_EnablePVD();							 // Enable the PVD Output
		  PVD_OFF=0;
	}

	static void InterruptPVD_When_VDD_OFF_Config(void){
		  sConfigPVD.PVDLevel = PWR_PVDLEVEL_7;
		  sConfigPVD.Mode = PWR_PVD_MODE_IT_RISING;  //interrumpe cuando VDD < VPDthreshold
		  HAL_PWR_ConfigPVD(&sConfigPVD);
		  HAL_PWR_EnablePVD();						//Enable the PVD Output
		  PVD_OFF=1;
	}

	void HAL_PWR_PVDCallback(void)
	{
		//if(PVD_OFF==0){
		//	InterruptPVD_When_VDD_OFF_Config();
		//}else{
			//BSP_LED_On(LED3);	//PA5
		    ENABLE_SPI_byPOLLING();
		    EEPROM_Protect(EEPROM_ProtectNone);
		    write_backup_eeprom();
			EEPROM_Protect(EEPROM_ProtectAll);
		//}
	}
	 //PRUEBA LED1
	  //BSP_LED_Init(0);
	  //BSP_LED_On(0);
	  //BSP_LED_Off(0);
	  //PRUEBA LED2
	  //BSP_LED_Init(1);
	  //BSP_LED_On(1);
	  //BSP_LED_Off(1);
	  //PRUEBA BOTTON COMO ENTRADA
	  //BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
	  //ESTADO_BUTTON = BSP_PB_GetState(BUTTON_KEY);
	  //ESTADO_BUTTON = BSP_PB_GetState(BUTTON_KEY);

	  //PRUEBA BOTTON COMO INTERRRUPCION
	  //Adem�s de inicializar hay que agregar las funciones para
	  //manejar la interrupci�n estas son
	  //void EXTI0_IRQHandler(void) ;Atenci�n de IRQ en archivo stm32f4x_it.c
	  //HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin); en archivo stm32xx_gpio.c
	  //HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)(); Funci�n de atenci�n en archivo que corresponda

	 // BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

	  /* USER CODE BEGIN 2 */
	   //  HAL_UART_Receive_IT(&huart1,rx_buff,200);
	     //HAL_UART_Transmit_IT(&huart1,tx_buff,10);
	     //__HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);


	void USART_Ini(UART_HandleTypeDef	*huart, uint16_t baudrate){

			  if(huart == &huart1){
				  huart->Instance = USART1;
			  }else{
				  huart->Instance = USART7;
			  }

			  huart->Init.BaudRate = baudrate;
			  huart->Init.WordLength = UART_WORDLENGTH_8B;
			  huart->Init.StopBits = UART_STOPBITS_1;
			  huart->Init.Parity = UART_PARITY_NONE;
			  huart->Init.Mode = UART_MODE_TX_RX;
			  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
			  huart->Init.OverSampling = UART_OVERSAMPLING_16;
			  if (HAL_UART_Init(huart) != HAL_OK)
				{
				_Error_Handler(__FILE__, __LINE__);
			  }
		}
void checkUART_ERROR (void){

	if(errorIRQ_UART1){
		errorIRQ_UART1 = 0;
		DisablefLAGS_UART1();
		USART1_Ini();

	}else if(errorIRQ_UART7){
		errorIRQ_UART7 = 0;
		DisablefLAGS_UART7();
		USART7_Ini();
		VA_UART_ini();
		DA_iniRx();
		DA_iniTx();
	}
}



void SystemInit_test(void)
{
  /* Reset the RCC clock configuration to the default reset state ------------*/
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001U;

#if defined (STM32F051x8) || defined (STM32F058x8)
  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE and MCOSEL[2:0] bits */
  RCC->CFGR &= (uint32_t)0xF8FFB80CU;
#else
  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE, MCOSEL[2:0], MCOPRE[2:0] and PLLNODIV bits */
  RCC->CFGR &= (uint32_t)0x08FFB80CU;
#endif /* STM32F051x8 or STM32F058x8 */

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFFU;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFFU;

  /* Reset PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
  RCC->CFGR &= (uint32_t)0xFFC0FFFFU;

  /* Reset PREDIV[3:0] bits */
  RCC->CFGR2 &= (uint32_t)0xFFFFFFF0U;

#if defined (STM32F072xB) || defined (STM32F078xx)
  /* Reset USART2SW[1:0], USART1SW[1:0], I2C1SW, CECSW, USBSW and ADCSW bits */
  RCC->CFGR3 &= (uint32_t)0xFFFCFE2CU;
#elif defined (STM32F071xB)
  /* Reset USART2SW[1:0], USART1SW[1:0], I2C1SW, CECSW and ADCSW bits */
  RCC->CFGR3 &= (uint32_t)0xFFFFCEACU;
#elif defined (STM32F091xC) || defined (STM32F098xx)
  /* Reset USART3SW[1:0], USART2SW[1:0], USART1SW[1:0], I2C1SW, CECSW and ADCSW bits */
  RCC->CFGR3 &= (uint32_t)0xFFF0FEACU;
#elif defined (STM32F030x6) || defined (STM32F030x8) || defined (STM32F031x6) || defined (STM32F038xx) || defined (STM32F030xC)
  /* Reset USART1SW[1:0], I2C1SW and ADCSW bits */
  RCC->CFGR3 &= (uint32_t)0xFFFFFEECU;
#elif defined (STM32F051x8) || defined (STM32F058xx)
  /* Reset USART1SW[1:0], I2C1SW, CECSW and ADCSW bits */
  RCC->CFGR3 &= (uint32_t)0xFFFFFEACU;
#elif defined (STM32F042x6) || defined (STM32F048xx)
  /* Reset USART1SW[1:0], I2C1SW, CECSW, USBSW and ADCSW bits */
  RCC->CFGR3 &= (uint32_t)0xFFFFFE2CU;
#elif defined (STM32F070x6) || defined (STM32F070xB)
  /* Reset USART1SW[1:0], I2C1SW, USBSW and ADCSW bits */
  RCC->CFGR3 &= (uint32_t)0xFFFFFE6CU;
  /* Set default USB clock to PLLCLK, since there is no HSI48 */
  RCC->CFGR3 |= (uint32_t)0x00000080U;
#else
 #warning "No target selected"
#endif

  /* Reset HSI14 bit */
  RCC->CR2 &= (uint32_t)0xFFFFFFFEU;

  /* Disable all interrupts */
  //RCC->CIR = 0x00000000U;

}

//TST_segmentos_display_7seg();
//TEST_DISPLAY_7SEG();
//BSP_LED_Init(LED3);
//BSP_LED_Off(LED3);
//NVIC_SystemReset();
//chk_crc();
//BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
//ini spi
//MX_SPIx_Init();
//EPROM_CS_Init();

//test_backup_registers();
//write_backup_rtc();
//read_backup_registers();
//clean_backup_rtc();
//write_backup_rtc();
//read_backup_registers();

//test_spi();
//LC_EepromTest();

//test_size();
//read_backup_eeprom();
//read_progRELOJ_eeprom();

//write_backup_eeprom();
//corteALIMENTACION = NO_HUBO_CORTE;		//se debe reseatear luego de leer el backup de eeprom

//chk_crc_itu();
//HAL_PWR_PVDCallback();
//read_backup_eeprom();

//for(;;){
 //TEST_DISPLAY_7SEG();
//}
//	flash_test();
//test_adc();

//ConfigureADC_PA0();
//ADC_Config();

//if(checkTIME == 0){
//	  checkTIME = 60;
	  //RTC_actDate();
		  //if(RTC_Date.fecha[2]==0x00){
			// set_TIMEandDATE();
			// RTC_actDate();
		 //}
//}


	//******* PRUEBAS **************************
/*
if(auxCNT==1){Tx_Encendido_EQUIPO();}
		if(auxCNT==2){Tx_Boton_EMERGENCIA();}
		if(auxCNT==3){Tx_Valor_VIAJE();}
		if(auxCNT==4){Tx_Resumen_VIAJE();}
		if(auxCNT==5){Tx_TARIFAS();}
		if(auxCNT==6){Tx_Status_RELOJ();}
		if(auxCNT==7){Tx_Pase_a_LIBRE();}
		if(auxCNT==8){Tx_Pase_a_OCUPADO();}
		if(auxCNT==9){Tx_Pase_a_COBRANDO();}
		if(auxCNT==10){Tx_Pase_a_FUERA_SERVICIO();}
		if(auxCNT==11){Tx_Comando_MENSAJE();}
		if(auxCNT==12){Tx_Comando_TRANSPARENTE();}
		auxCNT=0;
*/
	//******************************************




//while(PVD_OFF==0){
	//espero hasta que VDD > VPDthreshold
	//};


/*
 *
 //static void MX_USART7_UART_Init(void);
//static void MX_USART1_UART_Init(void);
//static void MX_TIM2_Init(void);

  if(RELOJ_OCUPADO){
	 // __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);

	  SystemInit();
	  __libc_init_array();
	  HAL_Init();
	  SystemClock_Config();
	  MX_TIM2_Init4();
	  ini_display_7seg();
	  choice_device_uart1 = PROG_DEVICE;

	  USART1_Ini();
	  USART7_Ini();

	  	  	  ptr1 = (byte*) ebss-1;
	 		  ptr2 = (byte*) ebss;
	 		  while(USART1->CR1==0){
	 			  clrRAM (ptr1, ptr2);
	 			  USART1_Ini();
	 			  USART7_Ini();
	 			  ptr1--;
	 			  ptr2--;
	 			  if(ptr1 == (byte*)0x20002838){
	 				  ptr1--;
	 				  ptr2--;
	 			  }

	 			  if(ptr1< sbss){
	 				  break;
	 			  }
	 		  }

	  goto LOOP;

	    HAL_Init();
	    SystemClock_Config();

	    MX_TIM2_Init4();
		ini_display_7seg();
		set_tipo_de_equipo();
		MX_SPIx_Init();
		EPROM_CS_Init();
		Ini_portBANDERITA();
		clr_BANDERA();
		MX_GPIO_Init();
		choice_device_uart1 = PROG_DEVICE;
		USART1_Ini();
		USART7_Ini();
		HAL_TIM_IC_Start_IT(&pulsoACCUM,TIM_CHANNEL_1);
		VA_UART_ini();
		//MX_RTC_Init();
		//VA_UART_ini();
		Bluetooth_Ini();
		Panico_Ini();
		ini_puerto_sensor_asiento();
		BanderaOut_Ini();
		clr_BANDERA();
		buzzer_ini();
		ini_pulsador_cambio_reloj();
		JUMPER_PROG_Init();
		PVD_Config();
		InterruptPVD_When_VDD_OFF_Config();
		goto LOOP;
  }
INI:
*/

/*
byte estaOCUPADO(void ){

	byte estado;
	estado = 0;

	if(RELOJ_OCUPADO){
		estado = 1;
	}
	return(estado);

}

static void clrRAM (byte* ini, byte* fin){

	byte tmp;

	while(ini < fin){
		//tmp = *ini;
		*ini = 0;
		//*ini = tmp;
		ini++;
	}
}

*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
