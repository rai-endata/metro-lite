/**
  ******************************************************************************
  * @file    UART/UART_TwoBoards_ComIT/Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.2.4
  * @date    13-November-2015 
  * @brief   HAL MSP module.    
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
#include "main.h"
#include "spi.h"
//#include "stm32f091xc.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

 /** @defgroup UART_TwoBoards_ComIT
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for UART interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */

void HAL_UART_MspInit(UART_HandleTypeDef *huart){

	if(huart==&huart1){
		HAL_UART1_MspInit(huart);
	}else if(huart==&huart7){
		HAL_UART7_MspInit(huart);
	}else if(huart==&huart7){
		HAL_UART7_MspInit(huart);
	}

}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
	if(huart==&huart1){
		HAL_UART1_MspDeInit(huart);
	}else if(huart==&huart7){
		HAL_UART7_MspDeInit(huart);
	}

}

/*
void HAL_UART1_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  // Enable GPIO TX/RX clock
  USART1_TX_GPIO_CLK_ENABLE();
  USART1_RX_GPIO_CLK_ENABLE();

  // Enable USART1 clock
  USART1_CLK_ENABLE();

  //USART1 GPIO Configuration
  //PA9     ------> USART1_TX
  //PA10     ------> USART1_RX

  GPIO_InitStruct.Pin = USART1_TX_PIN|USART1_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = USART1_RX_AF;


  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  // NVIC for USART1
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}
*/

void HAL_UART1_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
      // Peripheral clock enable
    __HAL_RCC_USART1_CLK_ENABLE();

    //**USART1 GPIO Configuration
    //PA9     ------> USART1_TX
    //PA10     ------> USART1_RX
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}


/*
void HAL_UART7_MspInit(UART_HandleTypeDef *huart)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  // Enable GPIO TX/RX clock
  USART7_TX_GPIO_CLK_ENABLE();
  USART7_RX_GPIO_CLK_ENABLE();

  // Enable USART6 clock
  USART7_CLK_ENABLE();
  
  // UART TX GPIO pin configuration
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pin       = USART7_TX_PIN;
  GPIO_InitStruct.Alternate = USART7_TX_AF;
  HAL_GPIO_Init(USART7_TX_GPIO_PORT, &GPIO_InitStruct);
    
  // UART RX GPIO pin configuration
  GPIO_InitStruct.Pin = USART7_RX_PIN;
  GPIO_InitStruct.Alternate = USART7_RX_AF;
  HAL_GPIO_Init(USART7_RX_GPIO_PORT, &GPIO_InitStruct);
    
  // NVIC for USART1
  HAL_NVIC_SetPriority(USART7_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USART7_IRQn);
}
*/

void HAL_UART7_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct;
      //Peripheral clock enable
    __HAL_RCC_USART7_CLK_ENABLE();

    //USART7 GPIO Configuration
    //PC6     ------> USART7_TX
    //PC7     ------> USART7_RX

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART7;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // USART7 interrupt Init
    HAL_NVIC_SetPriority(USART3_8_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_8_IRQn);


}


/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */


void HAL_UART7_MspDeInit(UART_HandleTypeDef* huart)
{
	// Peripheral clock disable
    __HAL_RCC_USART7_CLK_DISABLE();

    //USART7 GPIO Configuration
    //PC6     ------> USART7_TX
    //PC7     ------> USART7_RX

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7);

    /* USART7 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART3_8_IRQn);

}

void HAL_UART1_MspDeInit(UART_HandleTypeDef* huart)
{
      // Peripheral clock disable
    __HAL_RCC_USART1_CLK_DISABLE();

    //**USART1 GPIO Configuration
    //PA9     ------> USART1_TX
    //PA10     ------> USART1_RX

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    // USART1 interrupt DeInit
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }


/**
  * @}
  */



void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_base->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /**TIM2 GPIO Configuration
    PA0/WKUP     ------> TIM2_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    //GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }

}


void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{

  if(htim_base->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __TIM2_CLK_DISABLE();

    /**TIM2 GPIO Configuration
    PA0/WKUP     ------> TIM2_CH1
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

  }
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */

}



/*
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{

  if(hrtc->Instance==RTC)
  {
  // USER CODE BEGIN RTC_MspInit 0/
	 __HAL_RCC_BACKUPRESET_FORCE();
	 __HAL_RCC_BACKUPRESET_RELEASE();

  // USER CODE END RTC_MspInit 0
    /* Peripheral clock enable
    __HAL_RCC_RTC_ENABLE();
  // Peripheral interrupt init
    HAL_NVIC_SetPriority(TAMP_STAMP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TAMP_STAMP_IRQn);
  // USER CODE BEGIN RTC_MspInit 1

  // USER CODE END RTC_MspInit 1
  }
}
*/



/*
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  //##-1- Configue LSE as RTC clock soucre ###################################
#ifdef RTC_CLOCK_SOURCE_LSE

  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    // Initialization Error
	  _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    // Initialization Error
	  _Error_Handler(__FILE__, __LINE__);
  }
#elif defined (RTC_CLOCK_SOURCE_LSI)
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    // Initialization Error
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    // Initialization Error
    Error_Handler();
  }
#else
#error Please select the RTC Clock source inside the main.h file
#endif //RTC_CLOCK_SOURCE_LSE

  //##-2- Enable RTC peripheral Clocks #######################################
  // Enable RTC Clock
  __HAL_RCC_RTC_ENABLE();

  //##-3- Configure the NVIC for RTC WakeUp Timer ############################
  //HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x0F, 0);
  //HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
  HAL_NVIC_SetPriority(TAMP_STAMP_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TAMP_STAMP_IRQn);

}


void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc){
  if(hrtc->Instance==RTC){
    __HAL_RCC_RTC_DISABLE();				// Peripheral clock disable
    HAL_NVIC_DisableIRQ(TAMP_STAMP_IRQn);	// Peripheral interrupt DeInit
  }
}
*/

/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param htim: TIM handle pointer
  * @retval None
  */
/*
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef   GPIO_InitStruct;

  //*##-1- Enable peripherals and GPIO Clocks #################################
  // TIMx Peripheral clock enable
  TIMx_CLK_ENABLE();

  // Enable GPIO channels Clock/
  TIMx_CHANNEL_GPIO_PORT();

  // Configure  (TIMx_Channel) in Alternate function, push-pull and 100MHz speed
  GPIO_InitStruct.Pin = GPIO_PIN_CHANNEL2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF_TIMx;
  HAL_GPIO_Init(GPIO_PORT, &GPIO_InitStruct);

  //##-2- Configure the NVIC for TIMx #########################################
  HAL_NVIC_SetPriority(TIMx_IRQn, 0, 1);

  // Enable the TIM4 global Interrupt
  HAL_NVIC_EnableIRQ(TIMx_IRQn);

}

*/

/*
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
  if(hrtc->Instance==RTC)
  {
  // USER CODE BEGIN RTC_MspInit 0

  // USER CODE END RTC_MspInit 0
    // Peripheral clock enable
    __HAL_RCC_RTC_ENABLE();
  // USER CODE BEGIN RTC_MspInit 1

  // USER CODE END RTC_MspInit 1
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{

  if(hrtc->Instance==RTC)
  {
  // USER CODE BEGIN RTC_MspDeInit 0

  // USER CODE END RTC_MspDeInit 0
    // Peripheral clock disable
    __HAL_RCC_RTC_DISABLE();
  // USER CODE BEGIN RTC_MspDeInit 1

  // USER CODE END RTC_MspDeInit 1
  }

}
*/

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  //##-1- Configue LSE as RTC clock soucre ###################################
#ifdef RTC_CLOCK_SOURCE_LSE
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    /* Initialization Error */
	  _Error_Handler(__FILE__, __LINE__);
  }
#elif defined (RTC_CLOCK_SOURCE_LSI)
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    // Initialization Error
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    // Initialization Error
    Error_Handler();
  }

  #else
#error Please select the RTC Clock source inside the main.h file
#endif /*RTC_CLOCK_SOURCE_LSE*/

  //##-2- Enable RTC peripheral Clocks #######################################
  // Enable RTC Clock
  __HAL_RCC_RTC_ENABLE();

  //##-3- Configure the NVIC for RTC WakeUp Timer ############################
  //HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x0F, 0);
  //HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
  //HAL_NVIC_SetPriority(TAMP_STAMP_IRQn, 0, 0);
  //HAL_NVIC_EnableIRQ(TAMP_STAMP_IRQn);
}


void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc){
  if(hrtc->Instance==RTC){
    __HAL_RCC_RTC_DISABLE();				// Peripheral clock disable
    //HAL_NVIC_DisableIRQ(TAMP_STAMP_IRQn);	// Peripheral interrupt DeInit
  }
}

/**
  * @brief SPI MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - NVIC configuration for SPI interrupt request enable
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
GPIO_InitTypeDef  GPIO_InitStruct;

  if (hspi->Instance == SPIx)
  {
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    SPIx_SCK_GPIO_CLK_ENABLE();
    SPIx_MISO_GPIO_CLK_ENABLE();
    SPIx_MOSI_GPIO_CLK_ENABLE();
    /* Enable SPI clock */
    SPIx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    //GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = SPIx_SCK_AF;
    HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_MISO_PIN;
    GPIO_InitStruct.Alternate = SPIx_MISO_AF;
    //GPIO_InitStruct.Pull      = GPIO_PULLUP;           //BY RAI
    HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
    GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
    //GPIO_InitStruct.Pull      = GPIO_PULLUP;           //BY RAI
    HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);

    /*##-3- Configure the NVIC for SPI #########################################*/
    /* NVIC for SPI */
    HAL_NVIC_SetPriority(SPIx_IRQn, 1, 0);
    //HAL_NVIC_EnableIRQ(SPIx_IRQn);
    //ENABLE_SPI_byIRQ();
    ENABLE_SPI_byPOLLING();
  }
}

/**
  * @brief SPI MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  if(hspi->Instance == SPIx)
  {
    /*##-1- Reset peripherals ##################################################*/
    SPIx_FORCE_RESET();
    SPIx_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* Deconfigure SPI SCK */
    HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
    /* Deconfigure SPI MISO */
    HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
    /* Deconfigure SPI MOSI */
    HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);

    /*##-3- Disable the NVIC for SPI ###########################################*/
    HAL_NVIC_DisableIRQ(SPIx_IRQn);
  }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
