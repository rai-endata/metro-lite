#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stdint.h"
#define BLUETOOTH_STATUS_PIN                         GPIO_PIN_3
#define BLUETOOTH_STATUS_GPIO_PORT                   GPIOA
#define BLUETOOTH_STATUS_GPIO_CLK_ENABLE()           __GPIOA_CLK_ENABLE()
#define BLUETOOTH_STATUS_GPIO_CLK_DISABLE()          __GPIOA_CLK_DISABLE()
#define BLUETOOTH_STATUS_EXTI_IRQn 		                EXTI2_3_IRQn



	typedef union {
	      uint8_t uint8_t;
	      struct{
			  uint8_t bluetoothCONNECTED		:1;
			  uint8_t :1;
			  uint8_t :1;
			  uint8_t :1;
			  uint8_t :1;
			  uint8_t :1;
			  uint8_t :1;
			  uint8_t :1;
	      }Bits;
	 }tBLUETOOH_FLAGS;


extern tBLUETOOH_FLAGS			_bluetoothFLAGS;

#define bluetoothCONECTADO		_bluetoothFLAGS.Bits.bluetoothCONNECTED

#define TO_BLUETOOTH_1		10 //cuando detecto cambio de nivel en la linea de bluetooth bajo el timeout de bluetooth a 1 seg
#define TO_BLUETOOTH_2		30 //Normalmente el timeout de bluetooth es cada 3 segundos
#define TO_BLUETOOTH_3		50 //cuando recibo datos del celular pateo 5 segundos el timeout para mirar bluetooth


extern void Bluetooth_Ini(void);
extern void check_pressBLUETOOTH(void);
extern void check_pressBLUETOOTH(void);
extern void Bluetooth_TimeOut (void);
extern uint8_t	Bluetooth_to_cnt;

extern void setTO_Bluetooth(uint8_t time );

//extern GPIO_PinState Buetooth_Status_GetState(void);

#endif
