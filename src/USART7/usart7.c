

#include "usart7.h"
#include "main.h"
#include "Manejo de Buffers.h"
#include "DTE - Tx.h"
#include "stm32f0xx_hal_uart.h"


	/* *************************
	* DEFINICION DE PROTOTIPOS *
	***************************/

static void MX_USART7_UART_Init(void);




	/* INICIALIZACION UART7  */
	/*************************/

	void USART7_Ini (void){
		//MX_USART7_UART_Init();									//inicializo uart
		USART_Ini(&huart7, 9600);
		HAL_UART_Receive_IT(&huart7,va_rxDATA_buffer,dim_va_rxDATA);					//pongo en recepcion UART1 (Obs: PRINT_RxBuffer,dimATRx no son usados por ahora)
	 }


	/* USART7 FUNCION INIT *
	 ***********************/

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


	/* INICIALIZACION DE PUNTEROS DEL SCI *
	**************************************/


