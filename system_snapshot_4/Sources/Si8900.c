/*
 * Si8900.c
 *
 *  Created on: Aug 10, 2016
 *      Author: mmh
 *
 *  Interfaces to Si8900 10bit ADC
 *
 *  Uses UART2 Pins 59 and 60
 */

#include <stdio.h>
#include "MK22F51212.h"
#include "system_MK22F51212.h"
#include "Si8900.h"
#include "mcu_tracer.h"


float global_Si8900_var;

#define UART_CLOCK (SystemCoreClock/2)

void Si8900_init(uint32_t baudrate){
   // Enable clock to UART
   SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;

   // Enable clock to port pins used by UART
   SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

   // Set Tx & Rx Pin function
   PORTD->PCR[2] = PORT_PCR_MUX(3);
   PORTD->PCR[3] = PORT_PCR_MUX(3);


	// Disable UART before changing registers
	UART2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

	// Calculate UART clock setting (5-bit fraction at right)
	int scaledBaudValue = (2*UART_CLOCK)/(baudrate);


	// Set Baud rate register
	UART2->BDH = (UART2->BDH&~UART_BDH_SBR_MASK) | UART_BDH_SBR((scaledBaudValue>>(8+5)));
	UART2->BDL = UART_BDL_SBR(scaledBaudValue>>5);
	// Fractional divider to get closer to the baud rate
	UART2->C4 = (UART2->C4&~UART_C4_BRFA_MASK) | UART_C4_BRFA(scaledBaudValue);

	UART2->C1 = 0;

	// Enable UART Tx & Rx
	UART2->C2 = UART_C2_TE_MASK|UART_C2_RE_MASK;
	//send sync byte, so that Si8900 can sync
	UART2->PFIFO |= (UART_PFIFO_TXFE_MASK|UART_PFIFO_RXFE_MASK);
}


void Si8900_txChar(int ch) {
   while ((UART2->S1 & UART_S1_TDRE_MASK) == 0) {
      // Wait for Tx buffer empty
      __asm__("nop");
   }
   UART2->D = ch;
}

void SI8900_sync(void){
	int rec;
	uint32_t countdown=1000;
	while(countdown--){
		Si8900_txChar(0xAA);
		uint16_t delay=10000;
		while(delay--){
			__asm__ volatile ("");
		}
	}
}

int SI8900_timeout_read(){
	uint16_t countdown=1000;
	uint32_t rec;
	do{
		rec=((UART2_S1 & UART_S1_RDRF_MASK));
		if(rec){
			return UART2->D;
		}
		countdown--;
		if(countdown<1) return -1;
	}while(1);

}

void SI8900_clear_buffer(){
	uint32_t rec;
	do{
		rec=((UART2_S1 & UART_S1_RDRF_MASK));
		if(!rec) return;
		(void)UART2->D;
	}while(1);
}

void SI8900_manual_req(int ch){
	uint8_t data;
	data=(1<<7)|(1<<6)|(0b10<<SI8900_CNFG_MX_SHIFT)|(SI8900_CNFG_MODE_MSK)|(SI8900_CNFG_PGA_MSK);
	SI8900_clear_buffer();
	Si8900_txChar(data);
	int rec=SI8900_timeout_read();
	if(rec==data){
		int rec2=SI8900_timeout_read();
		int rec3=SI8900_timeout_read();
		if(rec2<0 || rec3<0){
			global_Si8900_var=-1;
			return;
		}
		uint16_t binary=((rec2&SI8900_ADC_H_DATA_MSK)<<6)+((rec3&SI8900_ADC_L_DATA_MSK)>>1);
		global_Si8900_var=binary;
	}else{
		SI8900_clear_buffer();
		global_Si8900_var=-1;
	}
	//	mcu_tracer_msg(buf);

}
