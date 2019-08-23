/*
 * adc.h
 *
 *  Created on: 3/7/2018
 *      Author: Rai
 */

#ifndef ADC_ADC_H_
#define ADC_ADC_H_

#define RANGE_12BITS                   ((uint32_t) 4095)    /* Max digital value with a full range of 12 bits */
#define ADC_IRQn ADC1_COMP_IRQn

extern void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc);
extern void ConfigureADC_PA0(void);
extern void ADC_Config(void);
extern void test_adc(void);
extern void update_ADC(void);
extern void takeADC_dat(ADC_HandleTypeDef* AdcHandle);

#endif /* ADC_ADC_H_ */
