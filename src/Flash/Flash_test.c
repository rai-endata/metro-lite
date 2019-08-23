/*
 * Flash.c
 *
 *  Created on: 16/2/2018
 *      Author: Rai
 */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_EraseProgram/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a description of how to erase and program the
  *          STM32F0xx FLASH.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#include "Flash_test.h"
#include "Flash.h"

/** @addtogroup STM32F0xx_HAL_Examples
  * @{
  */

/** @addtogroup FLASH_EraseProgram
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Address = 0, PageError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;
__IO uint16_t data16 = 1;
uint16_t DATA[300];
byte* ptrDATA;
uint32_t finDIR;

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;


void flash_test(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void flash_test(void){

	uint16_t n=0;

	while(n < 100){
		n++;
		DATA[n-1]=n;
	}

	/* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  // BORRADO
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_USER_START_ADDR_ERASE;
  EraseInitStruct.NbPages = (FLASH_USER_END_ADDR_ERASE - FLASH_USER_START_ADDR_ERASE) / FLASH_PAGE_SIZE;
  //if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){
  if (FLASH_eraseSector(FLASH_USER_START_ADDR_ERASE) != FLASH_ERR_NONE){

    while (1){
		//user can call function 'HAL_FLASH_GetError()'
		BSP_LED_Toggle(LED3);;
		HAL_Delay(100);
    }
  }

  Address = FLASH_USER_START_ADDR-10;
  finDIR  = (FLASH_USER_START_ADDR-10) + (100);

  if(FLASH_updateSector((uint32_t*) Address, &DATA, (uint32_t) (100)) == FLASH_ERR_NONE ){
	  BSP_LED_Toggle(LED3);
	  while (1){};
  }else{
      while (1){
        BSP_LED_Toggle(LED3);
        HAL_Delay(2000);
      }
  };

  //PROGRAMACION
  ptrDATA = &data16;
  DATA[0]= *(ptrDATA+1);
  DATA[1]= *(ptrDATA+0);


  while (Address < FLASH_USER_START_ADDR){
  //while (Address < FLASH_USER_END_ADDR){
    //if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) == HAL_OK)
   	//if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Address, DATA_16) == HAL_OK){
	//if (FLASH_writeWord(Address, DATA_16) == FLASH_ERR_NONE){
	if (FLASH_updateSector((uint32_t*) Address, &DATA, (uint32_t) 300)== FLASH_ERR_NONE){
      //Address = Address + 4;
      Address = Address + 2;
      data16  = data16 +  1;
      DATA[0]= *(ptrDATA+1);
      DATA[1]= *(ptrDATA+0);

      if(Address == FLASH_USER_END_ADDR){
    	  BSP_LED_Toggle(LED3);
      }
   	}else{
      while (1){
       // BSP_LED_Toggle(LED3);;
        HAL_Delay(2000);
      }
    }
  }
  HAL_FLASH_Lock();

  //VERIFICACION
  Address = FLASH_USER_START_ADDR;
  MemoryProgramStatus = 0x0;
  while (Address < FLASH_USER_END_ADDR){
    //data32 = *(__IO uint32_t *)Address;
    data16 = *(__IO uint16_t *)Address;
    //if (data32 != DATA_32)
   	if (data16 != DATA_16) {
      MemoryProgramStatus++;
    }
    //Address = Address + 4;
    Address = Address + 2;
  }

  //CHECK ERROR
  if (MemoryProgramStatus == 0){
    // No error detected.
	 // BSP_LED_On(LED3);
  }else{
    // Error detected.
    while (1){
      //BSP_LED_Toggle(LED3);;
      HAL_Delay(1000);
    }
  }

}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
/*
static void Error_Handler(void)
{
  while(1)
  {
  }
}
*/

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

