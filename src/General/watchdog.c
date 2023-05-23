
#ifdef HAL_WWDG_MODULE_ENABLED


	//https://embeddeddiaries.com/stm32-watch-dog-timer-iwdg/

	#include "stm32f0xx_hal.h"
	//#include "stm32f0xx_hal_wwdg.h"


	WWDG_HandleTypeDef hwwdg;

	void WWDG_Init(void)
	{
	/* El valor de Window debe ser menor que el valor de Counter
	 * Si se utiliza un prescaler de 8, el rango permitido es de 0 a 127.
	 * Si se utiliza un prescaler de 9, el rango permitido es de 0 a 1023.
	 * t = (Prescaler * Reload * WindowValue) / f_osc
	 * Para un oscilador de 8 MHz, el tiempo de prescaler es de 8, y el tiempo de reload es de 4096
	 * t = (8 * 4096 * 127) / 8000000 = 0.5 s
	 * */

	  hwwdg.Instance = WWDG;
	  hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
	  hwwdg.Init.Window = 127;
	  hwwdg.Init.Counter = 127;
	  //hwwdg.Init.EWIMode   = WWDG_EWI_DISABLE;

	  if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
		{
		 // Error_Handler();
		}

	}


	void HAL_WWDG_MspInit(WWDG_HandleTypeDef* hwwdg)
	{
	  if(hwwdg->Instance==WWDG)
	  {
		/* Peripheral clock enable */
		__HAL_RCC_WWDG_CLK_ENABLE();

		/* NVIC configuration for WWDG interrupt */
		HAL_NVIC_SetPriority(WWDG_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(WWDG_IRQn);
	  }
	}



	void HAL_WWDG_WakeupCallback(WWDG_HandleTypeDef* hwwdg)
	{
		 /* Reiniciar el sistema */
		  NVIC_SystemReset();
	}


	void WWDG_IRQHandler(void)
	{
	  /* WWDG interrupt management */
	  HAL_WWDG_IRQHandler(&hwwdg);
	}
 #endif


#include "stm32f0xx.h" //Incluir las definiciones del microcontrolador STM32F091


#ifdef HAL_IWDG_MODULE_ENABLED


	#include "stm32f0xx.h"

		void IWDG_Config(void)
		{
			// Habilitar el oscilador interno LSI
			//RCC->CSR |= RCC_CSR_LSIEN;
			RCC->CSR |= RCC_CSR_LSION;

			// Esperar a que el oscilador LSI esté listo
			while((RCC->CSR & RCC_CSR_LSIRDY) == RESET);

			// Configurar el prescaler del IWDG
			//IWDG->PR = IWDG_PR_PR_4;    // Prescaler = 32
			//IWDG->PR = IWDG_PR_PR_256; // Configurar el preescaler del reloj del IWDG en 1/4
			IWDG->PR = IWDG_PRESCALER_4;

			// Configurar el valor de recarga del IWDG
			IWDG->RLR = 0x0FFF;         // Recargar cada ~10ms
			//IWDG->RLR = 1000;         // Recargar cada ~10ms

			// Habilitar el módulo IWDG
			IWDG->KR = IWDG_KEY_RELOAD; // Cargar valor de recarga
			IWDG->KR = IWDG_KEY_ENABLE; // Habilitar el IWDG
		}

		/*
	int main(void)
	{

	    IWDG_Config();		// Configurar el módulo IWDG

	    while(1)
	    {

 	        //IWDG->KR = IWDG_KR_KEY_RELOAD;	// Realizar la recarga del contador del IWDG
 	        IWDG->KR = IWDG_KEY_RELOAD;	// Realizar la recarga del contador del IWDG

	        // Realizar alguna tarea del programa
	    }

	    return 0;
	}
    */

	/*
#include "stm32f0xx_hal.h"

IWDG_HandleTypeDef hiwdg; // Declaración del puntero para el IWDG

	void IWDG_Config(void){
		//  __HAL_RCC_IWDG_CLK_ENABLE(); // Habilitar el reloj para el IWDG
		 RCC->CSR |= RCC_CSR_LSION;
		 while((RCC->CSR & RCC_CSR_LSIRDY) == RESET); // Esperar a que el oscilador LSI esté listo

		  hiwdg.Instance = IWDG;
		  hiwdg.Init.Prescaler = IWDG_PRESCALER_64; // Configurar el prescaler
		  hiwdg.Init.Reload = 4095; // Configurar el valor de recarga
		  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
		   {
		    // Error_Handler();
		   }
	}



	int main(void)
	{
	  HAL_Init();

	  __HAL_RCC_IWDG_CLK_ENABLE(); // Habilitar el reloj para el IWDG

	  hiwdg.Instance = IWDG;
	  hiwdg.Init.Prescaler = IWDG_PRESCALER_4; // Configurar el prescaler
	  hiwdg.Init.Reload = 4095; // Configurar el valor de recarga
	  HAL_IWDG_Init(&hiwdg); // Inicializar el IWDG

	  while (1)
	  {
		HAL_IWDG_Refresh(&hiwdg); // Refrescar el contador del IWDG
	  }
	}
	*/

	__HAL_RCC_WWDG_CLK_ENABLE();
#endif
