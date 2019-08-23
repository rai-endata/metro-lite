/*
 * uart3.c
 *
 *  Created on: 15/9/2017
 *      Author: Rai
 */
#include "usart1.h"
#include "main.h"
#include "stm32f0xx_hal_uart.h"
#include "Definicion de tipos.h"
#include "Print UART.h"
#include "- PRINT Config -.h"

#define	dim_noIMPORTA1	1
byte noIMPORTA1[dim_noIMPORTA1];

uint8_t choice_device_uart1;

static void MX_USART1_UART_Init(void);



/* INICIALIZACION UART1  */
/*************************/
/*
	void USART1_ini (void){
		MX_USART1_UART_Init();
		HAL_UART_Receive_IT(&huart1,noIMPORTA1,dim_noIMPORTA1);	//pongo cualquiera porque defino el buffer de rx en otro lugar
	}
*/
	 void USART1_Ini (void){

		//MX_USART1_UART_Init();									//inicializo uart
		USART_Ini(&huart1, 4800);
		HAL_UART_Receive_IT(&huart1,PRINT_RxBuffer,dimPRINT_Rx);	//pongo en recepcion UART1 (Obs: PRINT_RxBuffer,dimATRx no son usados por ahora)
	 }

	static void MX_USART1_UART_Init(void)
	{

	  huart1.Instance = USART1;
	  //huart1.Init.BaudRate = 9600;
	  huart1.Init.BaudRate = 4800;
	  huart1.Init.WordLength = UART_WORDLENGTH_8B;
	  huart1.Init.StopBits = UART_STOPBITS_1;
	  huart1.Init.Parity = UART_PARITY_NONE;
	  huart1.Init.Mode = UART_MODE_TX_RX;
	  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	  HAL_UART_Init(&huart1);

	}


