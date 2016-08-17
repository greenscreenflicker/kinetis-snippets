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

void Si8900_init(void){
	//Init hardware interface
	Si8900_hw_init(115200);
	//Init transmitter (required to send out pulsed data)
	SI8900_tx_dma_init();
	//Init recieving DMA which stores recived data to memory
	SI8900_rec_dma_init();
	//Ensures that the Si8900 finds our boundrate.
	SI8900_autobaudrate_detection();
}

void Si8900_hw_init(uint32_t baudrate){
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
	UART2->C2 = UART_C2_TE_MASK|UART_C2_RE_MASK|UART_C2_RIE_MASK;
	// Enable Recieve DMA Request
	UART2->C5 = UART_C5_RDMAS_MASK;

	//send sync byte, so that Si8900 can sync
	UART2->PFIFO |= (UART_PFIFO_TXFE_MASK|UART_PFIFO_RXFE_MASK);
}




void SI8900_clear_buffer(){
	uint32_t rec;
	do{
		rec=((UART2_S1 & UART_S1_RDRF_MASK));
		if(!rec) return;
		(void)UART2->D;
	}while(1);
}



uint8_t si8900_transmit_data[SI8900_TX_BUF_SIZE];

void SI8900_tx_dma_init(void){
	uint8_t datalen=SI8900_TX_BUF_SIZE;

	//Turn on clocks for periodic interrupt timer
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; //enables PIT clock gate
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;//turns on PIT
	//Configure Channel 3
	PIT->CHANNEL[3].LDVAL=22000;
	PIT->CHANNEL[3].TCTRL|= PIT_TCTRL_TIE_MASK;//enables PIT timer interrupt
	PIT->CHANNEL[3].TCTRL|= PIT_TCTRL_TEN_MASK;//enables timer0

	//Configure DMA3
	// Use channel 3 for rec
	SIM_SCGC6|=SIM_SCGC6_DMAMUX_MASK;
	SIM_SCGC7|=SIM_SCGC7_DMA_MASK;

	DMAMUX_CHCFG3=0; //set to zero during configuration
	//Enable request signal for channel 1
	DMA0->ERQ|=DMA_ERQ_ERQ3_MASK;

	//Set memory address
	DMA0->TCD[3].SADDR=(uint32_t) &si8900_transmit_data;
	DMA0->TCD[3].DADDR=(uint32_t) &(UART2->D);

	//Offset for source and destination
	DMA0->TCD[3].SOFF=1; //We increment the source address by one, after each cycle
	DMA0->TCD[3].DOFF=0; //We increment the destination address by one

	//Source and destination data transfer size
	DMA0->TCD[3].ATTR=DMA_ATTR_SSIZE(0)|DMA_ATTR_DSIZE(0);

	//Source and destination adress adjustments
	DMA0->TCD[3].SLAST = -datalen;
	DMA0->TCD[3].DLAST_SGA=-0;

	DMA0->TCD[3].NBYTES_MLNO=1; //1 byte only in the minor loop.

	DMA0->TCD[3].CITER_ELINKNO=DMA_CITER_ELINKNO_CITER(datalen);
	DMA0->TCD[3].BITER_ELINKNO=DMA_BITER_ELINKNO_BITER(datalen);



	DMAMUX_CHCFG3=DMAMUX_CHCFG_ENBL_MASK|DMAMUX_CHCFG_TRIG_MASK|DMAMUX_CHCFG_SOURCE(60); //page 79 for requests
	DMA0->TCD[3].CSR=0;
/*
	if(DMA0->ES){
		//Page 396
		if(DMA0->ES & (1<<0)) {
			//Destination Bus error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<1)) {
			//Source Bus error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<2)) {
			//Scatter/Gather Configuration Error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<3)) {
			//NBYTES/CITER configuration Error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<4)) {
			//Destination Offset error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<5)) {
			//Destination Address Error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<6)) {
			//Source offset error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<7)) {
			//Source address error
			__asm__("NOP");
		}
	}else{
		__asm__("NOP");
	}*/

}

uint8_t SI8900_rec_buf[SI8900_REC_BUF_SIZE];



void SI8900_rec_dma_init(void){
	DMAMUX_CHCFG4=0; //set to zero during configuration

	//Enable request signal for channel 1
	DMA0->ERQ|=DMA_ERQ_ERQ4_MASK;

	//Set memory address
	DMA0->TCD[4].SADDR=(uint32_t) &(UART2->D);
	DMA0->TCD[4].DADDR=(uint32_t) &SI8900_rec_buf[0];

	//Offset for source and destination
	DMA0->TCD[4].SOFF=0; //We increment the address by one, after each cycle
	DMA0->TCD[4].DOFF=1;

	//Source and destination data transfer size
	DMA0->TCD[4].ATTR=DMA_ATTR_SSIZE(0)|DMA_ATTR_DSIZE(0);

	DMA0->TCD[4].NBYTES_MLNO=1;

	DMA0->TCD[4].CITER_ELINKNO=DMA_CITER_ELINKNO_CITER(SI8900_REC_BUF_SIZE);
	DMA0->TCD[4].BITER_ELINKNO=DMA_BITER_ELINKNO_BITER(SI8900_REC_BUF_SIZE);

	DMAMUX_CHCFG4=DMAMUX_CHCFG_ENBL_MASK|DMAMUX_CHCFG_SOURCE(6); //page 77

	DMA0->TCD[4].CSR=DMA_CSR_INTMAJOR_MASK; //end of major loop mask
	DMA0->TCD[4].SLAST=0;
	DMA0->TCD[4].DLAST_SGA=-SI8900_REC_BUF_SIZE;


/*
	if(DMA0->ES){
		//Page 396
		if(DMA0->ES & (1<<0)) {
			//Destination Bus error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<1)) {
			//Source Bus error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<2)) {
			//Scatter/Gather Configuration Error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<3)) {
			//NBYTES/CITER configuration Error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<4)) {
			//Destination Offset error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<5)) {
			//Destination Address Error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<6)) {
			//Source offset error
			__asm__("NOP");
		}
		if(DMA0->ES & (1<<7)) {
			//Source address error
			__asm__("NOP");
		}
	}else{
		__asm__("NOP");
	}*/
}
uint8_t SI8900_sync_flag;

void SI8900_autobaudrate_detection(void){
	uint8_t fillcounter=SI8900_TX_BUF_SIZE;
	do{
		si8900_transmit_data[--fillcounter]=0xAA;
	}while(fillcounter);
	SI8900_sync_flag=0;
	NVIC_EnableIRQ(DMA4_IRQn);
}

uint8_t SI8900_rec_buf_sync_test(void){
	uint8_t fillcounter=SI8900_REC_BUF_SIZE;
	do{
		if(SI8900_rec_buf[--fillcounter]!=0xAA) return 0;
	}while(fillcounter);
	return 1;
}


uint8_t SI8900_req_adc(void){
	si8900_transmit_data[0]=(1<<7)|(1<<6)|(0b00<<SI8900_CNFG_MX_SHIFT)|(SI8900_CNFG_MODE_MSK)|(SI8900_CNFG_PGA_MSK);
	si8900_transmit_data[1]=(1<<7)|(1<<6)|(0b01<<SI8900_CNFG_MX_SHIFT)|(SI8900_CNFG_MODE_MSK)|(SI8900_CNFG_PGA_MSK);
	si8900_transmit_data[2]=(1<<7)|(1<<6)|(0b10<<SI8900_CNFG_MX_SHIFT)|(SI8900_CNFG_MODE_MSK)|(SI8900_CNFG_PGA_MSK);
}


//ensures that channel 0,1,2 lineup is correct and not mixed up eg. 1,2,0
uint8_t SI8900_data_lineup(void){
	DMAMUX_CHCFG3=0;
	SI8900_clear_buffer(); //might be unescesarry, but kept for security.
	SI8900_rec_dma_init();
	DMAMUX_CHCFG3=DMAMUX_CHCFG_ENBL_MASK|DMAMUX_CHCFG_TRIG_MASK|DMAMUX_CHCFG_SOURCE(60); //page 79 for requests
}

uint16_t SI8900_get_ch(uint8_t ch){
	if(si8900_transmit_data[ch]==SI8900_rec_buf[3*ch]){
		//data is not currupt
		uint8_t rec2=SI8900_rec_buf[3*ch+1];
		uint8_t rec3=SI8900_rec_buf[3*ch+2];
		uint16_t binary=((rec2&SI8900_ADC_H_DATA_MSK)<<6)+((rec3&SI8900_ADC_L_DATA_MSK)>>1);
		return binary;
	}else{
		SI8900_data_lineup();
		return 0xFFFF;
	}
}

//when we recieve data this is called.
void DMA4_IRQHandler(void){
	//recieve data flag is zero, we need to test for
	SI8900_sync_flag=SI8900_rec_buf_sync_test();
	if(SI8900_sync_flag){
		SI8900_req_adc();
		SI8900_data_lineup();
		//reset transmit function, so that we can clear buffer
		NVIC_DisableIRQ(DMA4_IRQn);
	}
}

//Old functions, outdated, left for checking proposes.
/*
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

}*/
