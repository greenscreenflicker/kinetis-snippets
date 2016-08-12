/*
 * Si8900.h
 *
 *  Created on: Aug 10, 2016
 *      Author: mmh
 */

#ifndef INCLUDES_SI8900_H_
#define INCLUDES_SI8900_H_

#include <stdint.h>

void Si8900_init(uint32_t baudrate);
void Si8900_txChar(int ch);
void SI8900_manual_req(int ch);
void SI8900_sync(void);

#define SI8900_CNFG_MX1_MSK 		(1<<5)
#define SI8900_CNFG_MX0_MSK 		(1<<4)
#define SI8900_CNFG_MX_SHIFT		(4)
#define SI8900_CNFG_VREF_MSK 		(1<<3)
#define SI8900_CNFG_MODE_MSK 		(1<<1)
#define SI8900_CNFG_PGA_MSK			(1<<0)

#define SI8900_ADC_L_DATA_MSK		(0b111111<<1)
#define SI8900_ADC_H_DATA_MSK		(0b1111)

#endif /* SOURCES_SI8900_H_ */
