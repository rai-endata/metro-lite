/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"
//#include "stm32f429i_discovery.h"
#include "usart7.h"
#include "usart1.h"
#include "odometro.h"
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
//extern UART_HandleTypeDef huart7;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/
//extern RTC_HandleTypeDef hrtc;

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles USART1 global interrupt.
*/

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

void USART3_8_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart7);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){

    /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
    __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
   // __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
    __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

    huart->State = HAL_UART_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(huart);
    //
	if(huart->Instance == USART1){
		errorIRQ_UART1 = 1;
		USART1_Ini();
	}else if(huart->Instance == USART7){
		errorIRQ_UART7 = 7;
		USART7_Ini();
		VA_UART_ini();
		DA_iniRx();
		DA_iniTx();
	}
}
/*
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&pulsoACCUM);
}
*/

void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&pulsoACCUM);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}



void EXTI2_3_IRQHandler(void){

	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_3)){
		//conexion BLUETOOTH
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
	}
	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_2)){
		//PANICO
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
	}

}

void EXTI4_15_IRQHandler(void)
{

	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_5)){
		//TECLA DE CAMBIO DE RELOJ
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
	}

	if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_11)){
	  //TECLA DE IMPRESION
	  //JUMPER DE PROGRAMACION
	  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
	}

}


void PVD_VDDIO2_IRQHandler(void)
{
  HAL_PWR_PVD_IRQHandler();
}

/**
  * @brief  This function handles SPI interrupt request.
  * @param  None
  * @retval None
  */
void SPIx_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&SpiHandle);
}

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
/*
void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_PIN);
}
*/
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
