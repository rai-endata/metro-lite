
#include "Bluetooth.h"
#include "stm32f0xx_hal_gpio.h"
#include "Reloj.h"

tBLUETOOH_FLAGS		_bluetoothFLAGS;
byte	Bluetooth_to_cnt;

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

void Bluetooth_Ini(void){
	GPIO_InitTypeDef gpioinitstruct;

	BLUETOOTH_STATUS_GPIO_CLK_ENABLE();

	gpioinitstruct.Pin = BLUETOOTH_STATUS_PIN;
	gpioinitstruct.Pull = GPIO_NOPULL;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;

	// Configura pin como entrada con interrupcion externa
	gpioinitstruct.Mode = GPIO_MODE_IT_RISING_FALLING;

	HAL_GPIO_Init(BLUETOOTH_STATUS_GPIO_PORT, &gpioinitstruct);

	//habilita y setea set PULSADOR IMPRESAION EXTI Interrupt a la mas baja prioridad
	HAL_NVIC_SetPriority((IRQn_Type)(BLUETOOTH_STATUS_EXTI_IRQn), 0x03, 0x00);
	HAL_NVIC_EnableIRQ((IRQn_Type)(BLUETOOTH_STATUS_EXTI_IRQn));
}

void check_pressBLUETOOTH(void){

	GPIO_PinState pinSTATE;

	pinSTATE = HAL_GPIO_ReadPin(BLUETOOTH_STATUS_GPIO_PORT, BLUETOOTH_STATUS_PIN);
	if(!pinSTATE){
		//conexion bletooth (flanco descendente)
		bluetoothCONECTADO = 1;
	}else{
		//desconexion bluetooth (flanco ascendente)
		bluetoothCONECTADO = 0;
	}
}



void Bluetooth_TimeOut (void){
     // Ejecutada cada 10mseg
     if (Bluetooth_to_cnt != 0){
    	 Bluetooth_to_cnt--;
       if (Bluetooth_to_cnt == 0){
    	   check_pressBLUETOOTH();
			if(bluetoothCONECTADO){
				//Tx_Status_RELOJ();
				if(ESTADO_RELOJ == OCUPADO || ESTADO_RELOJ== COBRANDO){
					Tx_Valor_VIAJE();
				}
			}
       }
     }
}



/*
GPIO_PinState Buetooth_Status_GetState(void)
{
  return HAL_GPIO_ReadPin(BLUETOOTH_STATUS_GPIO_PORT, BLUETOOTH_STATUS_PIN);
}*/
