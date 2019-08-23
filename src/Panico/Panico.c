
#include "Panico.h"
#include "stm32f0xx_hal_gpio.h"

tPANICO_FLAGS			_panicoFLAGS;

/*
void Buetooth_Ini(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

    // Enable the BUTTON Clock
  BLUETOOTH_STATUS_GPIO_CLK_ENABLE();

    // Configure Button pin as input
          GPIO_InitStruct.Pin = BLUETOOTH_STATUS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BLUETOOTH_STATUS_GPIO_PORT, &GPIO_InitStruct);

}
*/

void Panico_Ini(void){
	GPIO_InitTypeDef gpioinitstruct;

	PANICO_GPIO_CLK_ENABLE();

	gpioinitstruct.Pin = PANICO_PIN;
	//gpioinitstruct.Pull = GPIO_NOPULL;
	gpioinitstruct.Pull = GPIO_PULLDOWN;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;


	// Configura pin como entrada con interrupcion externa
	//gpioinitstruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	gpioinitstruct.Mode = GPIO_MODE_EVT_RISING;

	HAL_GPIO_Init(PANICO_GPIO_PORT, &gpioinitstruct);

	//habilita y setea set PULSADOR IMPRESAION EXTI Interrupt a la mas baja prioridad
	HAL_NVIC_SetPriority((IRQn_Type)(PANICO_EXTI_IRQn), 0x03, 0x00);
	HAL_NVIC_EnableIRQ((IRQn_Type)(PANICO_EXTI_IRQn));
}


void check_pressPANICO(void){

	GPIO_PinState pinSTATE;

	pinSTATE = HAL_GPIO_ReadPin(PANICO_GPIO_PORT, PANICO_PIN);
	if(!pinSTATE){
		//deteccion Panico (flanco descendente)
		panicoDETECTADO = 0;
	}else{
		//deteccion Panico (flanco ascendente)
		if(!panicoDETECTADO){
			panicoDETECTADO = 1;
			Tx_Boton_EMERGENCIA();
		}
	}
}



/*
GPIO_PinState Buetooth_Status_GetState(void)
{
  return HAL_GPIO_ReadPin(BLUETOOTH_STATUS_GPIO_PORT, BLUETOOTH_STATUS_PIN);
}*/
