/*
 * Si8900.h
 *
 *  Created on: Aug 10, 2016
 *      Author: mmh
 */

#ifndef INCLUDES_SI8900_H_
#define INCLUDES_SI8900_H_

#include <stdint.h>

void Si8900_init(void);
void Si8900_hw_init(uint32_t baudrate);
void SI8900_tx_dma_init(void);
void SI8900_rec_dma_init(void);
void SI8900_autobaudrate_detection(void);
uint16_t SI8900_get_ch(uint8_t ch);


#define SI8900_CNFG_MX1_MSK 		(1<<5)
#define SI8900_CNFG_MX0_MSK 		(1<<4)
#define SI8900_CNFG_MX_SHIFT		(4)
#define SI8900_CNFG_VREF_MSK 		(1<<3)
#define SI8900_CNFG_MODE_MSK 		(1<<1)
#define SI8900_CNFG_PGA_MSK			(1<<0)

#define SI8900_ADC_L_DATA_MSK		(0b111111<<1)
#define SI8900_ADC_H_DATA_MSK		(0b1111)

#define SI8900_REC_BUF_SIZE 		9
#define SI8900_TX_BUF_SIZE			3
extern uint8_t SI8900_rec_buf[SI8900_REC_BUF_SIZE];
extern float global_Si8900_var;
extern uint8_t SI8900_sync_flag;


#endif /* SOURCES_SI8900_H_ */
