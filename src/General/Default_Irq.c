/*
 * Default_Irq.c
 *
 *  Created on: 29/1/2018
 *      Author: Rai
 */

#include "stm32f0xx_hal.h"

__weak   void NMI_Handler (void){while(1){__NOP();}}

__weak   void HardFault_Handler (void){
	//illegal memory accesses and
	//several incorrect program conditions.
	//HardFault: is the default exception and can be triggered because of an error during exception
	//processing, or because an exception cannot be managed by any other exception mechanism.
	while(1){
		__NOP();
	}
}


__weak   void MemManage_Handler		          (void){while(1){__NOP();}}
__weak   void BusFault_Handler				  (void){while(1){__NOP();}}
__weak   void UsageFault_Handler		      (void){while(1){__NOP();}}
__weak   void SVC_Handler				      (void){while(1){__NOP();}}
__weak   void DebugMon_Handler				  (void){while(1){__NOP();}}
__weak   void PendSV_Handler                  (void){while(1){__NOP();}}
__weak   void SysTick_Handler                 (void){while(1){__NOP();}}
__weak   void WWDG_IRQHandler                 (void){while(1){__NOP();}}
__weak   void PVD_IRQHandler                  (void){while(1){__NOP();}}
__weak   void TAMP_STAMP_IRQHandler           (void){while(1){__NOP();}}
__weak   void RTC_WKUP_IRQHandler             (void){while(1){__NOP();}}
__weak   void FLASH_IRQHandler                (void){while(1){__NOP();}}
__weak   void RCC_IRQHandler                  (void){while(1){__NOP();}}
__weak   void EXTI0_IRQHandler                (void){while(1){__NOP();}}
__weak   void EXTI1_IRQHandler                (void){while(1){__NOP();}}
__weak   void EXTI2_IRQHandler                (void){while(1){__NOP();}}
__weak   void EXTI3_IRQHandler                (void){while(1){__NOP();}}
__weak   void EXTI4_IRQHandler                (void){while(1){__NOP();}}
__weak   void DMA1_Stream0_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA1_Stream1_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA1_Stream2_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA1_Stream3_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA1_Stream4_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA1_Stream5_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA1_Stream6_IRQHandler         (void){while(1){__NOP();}}
__weak   void ADC_IRQHandler                  (void){while(1){__NOP();}}
__weak   void CAN1_TX_IRQHandler              (void){while(1){__NOP();}}
__weak   void CAN1_RX0_IRQHandler             (void){while(1){__NOP();}}
__weak   void CAN1_RX1_IRQHandler             (void){while(1){__NOP();}}
__weak   void CAN1_SCE_IRQHandler             (void){while(1){__NOP();}}
__weak   void EXTI9_5_IRQHandler      		  (void){while(1){__NOP();}}
__weak   void TIM1_BRK_TIM9_IRQHandler 		  (void){while(1){__NOP();}}
__weak   void TIM1_UP_TIM10_IRQHandler 		  (void){while(1){__NOP();}}
__weak   void TIM1_TRG_COM_TIM11_IRQHandler 	(void){while(1){__NOP();}}
__weak   void TIM1_CC_IRQHandler            	(void){while(1){__NOP();}}
__weak   void TIM2_IRQHandler               	(void){while(1){__NOP();}}
__weak   void TIM3_IRQHandler               	(void){while(1){__NOP();}}
__weak   void TIM4_IRQHandler               	(void){while(1){__NOP();}}
__weak   void I2C1_EV_IRQHandler            	(void){while(1){__NOP();}}
__weak   void I2C1_ER_IRQHandler   		  		(void){while(1){__NOP();}}
__weak   void I2C2_EV_IRQHandler            	(void){while(1){__NOP();}}
__weak   void I2C2_ER_IRQHandler            	(void){while(1){__NOP();}}
__weak   void SPI1_IRQHandler               	(void){while(1){__NOP();}}
__weak   void SPI2_IRQHandler               	(void){while(1){__NOP();}}
__weak   void USART1_IRQHandler             	(void){while(1){__NOP();}}
__weak   void USART2_IRQHandler			   		(void){while(1){__NOP();}}
__weak   void USART3_IRQHandler              	(void){while(1){__NOP();}}
__weak   void EXTI15_10_IRQHandler           	(void){while(1){__NOP();}}
__weak   void RTC_Alarm_IRQHandler           	(void){while(1){__NOP();}}
__weak   void OTG_FS_WKUP_IRQHandler         	(void){while(1){__NOP();}}
__weak   void TIM8_BRK_TIM12_IRQHandler      	(void){while(1){__NOP();}}
__weak   void TIM8_UP_TIM13_IRQHandler       	(void){while(1){__NOP();}}
__weak   void TIM8_TRG_COM_TIM14_IRQHandler  	(void){while(1){__NOP();}}
__weak   void TIM8_CC_IRQHandler             	(void){while(1){__NOP();}}
__weak   void DMA1_Stream7_IRQHandler        	(void){while(1){__NOP();}}
__weak   void FMC_IRQHandler                 	(void){while(1){__NOP();}}
__weak   void SDIO_IRQHandler                	(void){while(1){__NOP();}}
__weak   void TIM5_IRQHandler            		(void){while(1){__NOP();}}
__weak   void SPI3_IRQHandler                 (void){while(1){__NOP();}}
__weak   void UART4_IRQHandler                (void){while(1){__NOP();}}
__weak   void UART5_IRQHandler                (void){while(1){__NOP();}}
__weak   void TIM6_DAC_IRQHandler             (void){while(1){__NOP();}}
__weak   void TIM7_IRQHandler                 (void){while(1){__NOP();}}
__weak   void DMA2_Stream0_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA2_Stream1_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA2_Stream2_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA2_Stream3_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA2_Stream4_IRQHandler         (void){while(1){__NOP();}}
__weak   void ETH_IRQHandler                  (void){while(1){__NOP();}}
__weak   void ETH_WKUP_IRQHandler             (void){while(1){__NOP();}}
__weak   void CAN2_TX_IRQHandler              (void){while(1){__NOP();}}
__weak   void CAN2_RX0_IRQHandler             (void){while(1){__NOP();}}
__weak   void CAN2_RX1_IRQHandler             (void){while(1){__NOP();}}
__weak   void CAN2_SCE_IRQHandler             (void){while(1){__NOP();}}
__weak   void OTG_FS_IRQHandler               (void){while(1){__NOP();}}
__weak   void DMA2_Stream5_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA2_Stream6_IRQHandler         (void){while(1){__NOP();}}
__weak   void DMA2_Stream7_IRQHandler         (void){while(1){__NOP();}}
__weak   void USART6_IRQHandler               (void){while(1){__NOP();}}
__weak   void I2C3_EV_IRQHandler              (void){while(1){__NOP();}}
__weak   void I2C3_ER_IRQHandler              (void){while(1){__NOP();}}
__weak   void OTG_HS_EP1_OUT_IRQHandler       (void){while(1){__NOP();}}
__weak   void OTG_HS_EP1_IN_IRQHandler        (void){while(1){__NOP();}}
__weak   void OTG_HS_WKUP_IRQHandler          (void){while(1){__NOP();}}
__weak   void OTG_HS_IRQHandler               (void){while(1){__NOP();}}
__weak   void DCMI_IRQHandler                 (void){while(1){__NOP();}}
__weak   void HASH_RNG_IRQHandler             (void){while(1){__NOP();}}
__weak   void FPU_IRQHandler                  (void){while(1){__NOP();}}
__weak   void UART7_IRQHandler                (void){while(1){__NOP();}}
__weak   void UART8_IRQHandler                (void){while(1){__NOP();}}
__weak   void SPI4_IRQHandler                 (void){while(1){__NOP();}}
__weak   void SPI5_IRQHandler                 (void){while(1){__NOP();}}
__weak   void SPI6_IRQHandler                 (void){while(1){__NOP();}}
__weak   void SAI1_IRQHandler            	  (void){while(1){__NOP();}}
__weak   void LTDC_IRQHandler                 (void){while(1){__NOP();}}
__weak   void LTDC_ER_IRQHandler              (void){while(1){__NOP();}}
__weak   void DMA2D_IRQHandler                (void){while(1){__NOP();}}
