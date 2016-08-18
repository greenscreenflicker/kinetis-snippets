/*
 * adcdma1.h
 *
 *  Created on: Aug 17, 2016
 *      Author: mmh
 */

#ifndef INCLUDES_ADCDMA1_H_
#define INCLUDES_ADCDMA1_H_

#define TOTAL_CHANNEL_NUMBER 3
#define ADC1_DMA_CHMUX 		12
#define ADC1_DMA_REC 		13

extern uint16_t adc_result[TOTAL_CHANNEL_NUMBER];
extern uint8_t adc_mux[TOTAL_CHANNEL_NUMBER];

void ADC_dma1_init(void);
void ADC_dma1_init1(void);
void ADC_dma1_init2(void);

#endif /* INCLUDES_ADCDMA1_H_ */
