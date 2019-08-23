#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H
#include "Definicion de tipos.h"

#define BLUETOOTH_STATUS_PIN                         GPIO_PIN_3
#define BLUETOOTH_STATUS_GPIO_PORT                   GPIOA
#define BLUETOOTH_STATUS_GPIO_CLK_ENABLE()           __GPIOA_CLK_ENABLE()
#define BLUETOOTH_STATUS_GPIO_CLK_DISABLE()          __GPIOA_CLK_DISABLE()
#define BLUETOOTH_STATUS_EXTI_IRQn 		                EXTI2_3_IRQn



	typedef union {
	      byte Byte;
	      struct{
			  byte bluetoothCONNECTED		:1;
			  byte :1;
			  byte :1;
			  byte :1;
			  byte :1;
			  byte :1;
			  byte :1;
			  byte :1;
	      }Bits;
	 }tBLUETOOH_FLAGS;


extern tBLUETOOH_FLAGS			_bluetoothFLAGS;

#define bluetoothCONECTADO		_bluetoothFLAGS.Bits.bluetoothCONNECTED

#define BLUETOOTH_TIMEOUT		15


extern void Bluetooth_Ini(void);
extern void check_pressBLUETOOTH(void);
extern void check_pressBLUETOOTH(void);
extern void Bluetooth_TimeOut (void);
extern byte	Bluetooth_to_cnt;

//extern GPIO_PinState Buetooth_Status_GetState(void);

#endif
