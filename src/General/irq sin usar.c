/*
 * irq sin usar.c
 *
 *  Created on: 21/9/2018
 *      Author: Rai
 */


#include "main.h"


  void  NMI_Handler(void) {
  	while(1){
  		__asm volatile ("nop");
  	}
  }

  unsigned long hardfault_args[];

  void  HardFault_Handler(void)
  {
  //	while(1){
  		__asm volatile ("nop");
  	//}

  	    	volatile unsigned long stacked_r0 ;
  	        volatile unsigned long stacked_r1 ;
  	        volatile unsigned long stacked_r2 ;
  	        volatile unsigned long stacked_r3 ;
  	        volatile unsigned long stacked_r12 ;
  	        volatile unsigned long stacked_lr ;
  	        volatile unsigned long stacked_pc ;
  	        volatile unsigned long stacked_psr ;
  	        volatile unsigned long _CFSR ;
  	        volatile unsigned long _HFSR ;
  	        volatile unsigned long _DFSR ;
  	        volatile unsigned long _AFSR ;
  	        volatile unsigned long _BFAR ;
  	        volatile unsigned long _MMAR ;

		  ptr1--;
		  ptr2--;
  	      NVIC_SystemReset();

  	        stacked_r0 = ((unsigned long)hardfault_args[0]) ;
  	        stacked_r1 = ((unsigned long)hardfault_args[1]) ;
  	        stacked_r2 = ((unsigned long)hardfault_args[2]) ;
  	        stacked_r3 = ((unsigned long)hardfault_args[3]) ;
  	        stacked_r12 = ((unsigned long)hardfault_args[4]) ;
  	        stacked_lr = ((unsigned long)hardfault_args[5]) ;
  	        stacked_pc = ((unsigned long)hardfault_args[6]) ;
  	        stacked_psr = ((unsigned long)hardfault_args[7]) ;

  	        // Configurable Fault Status Register
  	        // Consists of MMSR, BFSR and UFSR
  	        _CFSR = (*((volatile unsigned long *)(0xE000ED28))) ;

  	        // Hard Fault Status Register
  	        _HFSR = (*((volatile unsigned long *)(0xE000ED2C))) ;

  	        // Debug Fault Status Register
  	        _DFSR = (*((volatile unsigned long *)(0xE000ED30))) ;

  	        // Auxiliary Fault Status Register
  	        _AFSR = (*((volatile unsigned long *)(0xE000ED3C))) ;

  	        // Read the Fault Address Registers. These may not contain valid values.
  	        // Check BFARVALID/MMARVALID to see if they are valid values
  	        // MemManage Fault Address Register
  	        _MMAR = (*((volatile unsigned long *)(0xE000ED34))) ;
  	        // Bus Fault Address Register
  	        _BFAR = (*((volatile unsigned long *)(0xE000ED38))) ;

  	        //__asm("BKPT #0\n") ; // Break into the debugger

  }


  void  SVC_Handler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  PendSV_Handler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }

/*
  void  SysTick_Handler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }
*/

  void  WWDG_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }

/*
  void  PVD_VDDIO2_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }
*/

 /*
  void  RTC_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }

*/
 /*
  void  FLASH_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }
*/

  void  RCC_CRS_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  EXTI0_1_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }

/*
  void  EXTI2_3_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }
*/

  /*
  void  EXTI4_15_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }
*/

  void  TSC_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  DMA1_Ch1_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  DMA1_Ch4_7_DMA2_Ch3_5_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }
/*
 void  ADC1_COMP_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }
*/

  void  TIM1_BRK_UP_TRG_COM_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  TIM1_CC_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }

/*
  void  TIM2_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }
*/

  void  TIM3_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  TIM6_DAC_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  TIM7_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  TIM14_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  TIM15_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  TIM16_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  TIM17_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  I2C1_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  I2C2_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }


  void  SPI1_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }

/*
  void  SPI2_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }
*/
/*
  void  USART1_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }
*/

  void  USART2_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }

/*
  void  USART3_8_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }
*/

  void  CEC_CAN_IRQHandler(void)
  {
  	while(1){
  		__asm volatile ("nop");
  	}
  }

