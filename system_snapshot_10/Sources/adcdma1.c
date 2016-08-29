/*
 * adcdma1.c
 *
 *  Created on: Aug 17, 2016
 *      Author: mmh
 */

#include <stdio.h>
#include "MK22F51212.h"
#include "adcdma1.h"


uint16_t adc_result[TOTAL_CHANNEL_NUMBER];
uint8_t adc_mux[TOTAL_CHANNEL_NUMBER];


void ADC_dma1_init(void){
	//Init ports
	//TODO: ADD ports
	//not required here, as they're default in the right state at our board.
	//Init adc itself
	ADC_dma1_init1();
	//Init DMA
	ADC_dma1_init2();
}

void ADC1_calibrate(void)
{
	// Set ADC clock frequency fADCK less than or equal to 4 MHz ==> input clock is the ALTCLK OSCERCLK (see page 105) [8 Mhz]
	// Devide the clock by 4 ==>  fADK = 2Mhz (this clock is just used for the calibration process)
	// choose the 16-bits mode
	ADC1->CFG1 |= ADC_CFG1_ADICLK(2)| ADC_CFG1_ADIV(2) | ADC_CFG1_MODE(3);
	// Set hardware averaging to maximum, that is, SC3[AVGE]=1 and SC3[AVGS]=11 (3) for an average of 32
	// and then start the calibration
	ADC1->SC3 |= ADC_SC3_AVGS(3) | ADC_SC3_AVGE_MASK | ADC_SC3_CAL_MASK;
	// Wait for the calibration to be done
	while(ADC1->SC3 & ADC_SC3_CAL_MASK);
	// Check if any error occured while calibrating and return 1 if failed ==> ADC accuracy specifications are not guaranteed, otherweise return 0
    if(ADC1->SC3 & ADC_SC3_CALF_MASK)
		    return 1;

	// follow the steps on page 796:

    // Initialize or clear a 16-bit variable in RAM.
    uint16_t variable = 0;
    // Add the plus-side calibration results CLP0, CLP1, CLP2, CLP3, CLP4, and CLPS to the variable.
    variable += ADC1_CLPS +  ADC1_CLP0 + ADC1_CLP1 + ADC1_CLP2 + ADC1_CLP3 + ADC1_CLP4;
    // Divide the variable by two.
    variable /= 2;
    // Set the MSB of the variable.
    variable |= 0b1000000000000000;
	// Store the value in the plus-side gain calibration register PG.
    ADC0_PG = variable;
	// Repeat the procedure for the minus-side gain calibration value.
	variable = 0;
	variable += ADC1_CLMS + ADC1_CLM0 + ADC1_CLM1 + ADC1_CLM2 + ADC1_CLM3 + ADC1_CLM4;
	variable /= 2;
	variable |= 0b1000000000000000;
	ADC1_MG = variable;

	return 0;
}


void ADC_dma1_init1(void)
{

	// Enable the necessary clocks : 1) for the ADC0 Module ; 2) for the PORTB
	SIM_SCGC6 |= SIM_SCGC6_ADC1_MASK;	// ADC 0 clock
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK;	// PTB clock and PORT C

	ADC1_calibrate();
	// pin muxing for pin number 35 (PTB0/LLWU_P5); choose the alternative 0 : ADC0_SE8
    //  1.2 V VREF_OUT is connected as the VALT reference option
	ADC1_SC2 |= ADC_SC2_REFSEL(1) ;


	// Reset the CFG1- register
	ADC1_CFG1 = 0;
	// Set ADC clock frequency fADCK as the bus Clock (60 Mhz)
	// Devide the clock by 1
	// configure the module for 16-bit resolution
	/* Sampling time configuration : sample time is the time that the internal switch is closed ans sample capacitor is charged.
	Long sample Time has been choosen for better results: This allows higher impedance inputs to be accurately sampled or to maximize
	conversion speed for lower impedance inputs. Longer sample times can also be used to lower overall
	power consumption when continuous conversions are enabled if high conversion rates are not required. */
	ADC1_CFG1 |=  ADC_CFG1_ADICLK(0)| ADC_CFG1_ADIV(3) | ADC_CFG1_MODE(3) | ADC_CFG1_ADLSMP(1) ;

	// DMA is enabled and will assert the ADC DMA request during an ADC conversion complete event noted
	// when any of the SC1n[COCO] flags is asserted
	// ADC Software trigger mode: means that the ADC conersions are started by writing the channel number in the ADCx_SC1A register
	ADC1_SC2 |= ADC_SC2_DMAEN_MASK | ADC_SC2_ADTRG(0);


	// Default longest sample time; 20 extra ADCK cycles; 24 ADCK cycles total ==> this results a very stable state compared to short sample time
	// High-speed conversion sequence selected with 2 additional ADCK cycles to total conversion time.
	ADC1_CFG2 = ADC_CFG2_MUXSEL_MASK | ADC_CFG2_ADLSTS(0) | ADC_CFG2_ADHSC_MASK ;


	// SC1A is used for both software and hardware trigger modes of operation.
	// Disable the module till the first read function == writing the number 31 in ADCH
	ADC1_SC1A = ADC_SC1_ADCH(31);

	// Start the conversion of Channel 8 to start the DMA cycle

}

void ADC_dma1_init2(void){
   adc_mux[0] = 4;
   adc_mux[1] = 5;
   adc_mux[2] = 7;


   // Enable clock for DMAMUX and DMA
   SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
   SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;


   //Channel 0 transfers next ADC0 input setting from constant buffer to ADC0_SC1A.
   //**** DMA transfer request source – always requestor
   DMAMUX_CHCFG_REG(DMAMUX,ADC1_DMA_CHMUX) = DMAMUX_CHCFG_ENBL_MASK |DMAMUX_CHCFG_SOURCE(63) ;

   //**** Source address, constant buffer in SRAM
   DMA0->TCD[ADC1_DMA_CHMUX].SADDR = (uint32_t)&adc_mux[0];
   //**** Destination address, ADC0 control register
   DMA0->TCD[ADC1_DMA_CHMUX].DADDR = (uint32_t)&ADC1_SC1A;

   //**** Source address increment, data is 8-bit, 1 byte
   DMA0->TCD[ADC1_DMA_CHMUX].SOFF = 0x01;

   //**** Destination address increment in bytes, no increment needed
   DMA0->TCD[ADC1_DMA_CHMUX].DOFF = 0x00;

   //**** Source and destination data width specification, both source and destination are 8-bit
   DMA0->TCD[ADC1_DMA_CHMUX].ATTR = DMA_ATTR_SSIZE(0)| DMA_ATTR_DSIZE(0);

   //**** Number of bytes for minor loop (one data transfer), ADC0 input setting value is
   //**** 8 bits long, so 1-byte transfer
   DMA0->TCD[ADC1_DMA_CHMUX].NBYTES_MLNO = 0x01;

   //**** Channel linking and major loop setting reload value after major loop finish,
   //**** no linking after minor loop, major loop transfers number 0x03
   DMA0->TCD[ADC1_DMA_CHMUX].CITER_ELINKNO = TOTAL_CHANNEL_NUMBER;

   //**** Channel linking and major loop setting, no linking after minor loop,
   //**** major loop transfers number 0x03
   DMA0->TCD[ADC1_DMA_CHMUX].BITER_ELINKNO = TOTAL_CHANNEL_NUMBER;

   //**** Source address reload after major loop finish, must be subtracted from last
   //**** pointer value, sampling channel number is 3 each and 1 byte long, 1 × 3 = 3
   //**** and must be subtract -3
   DMA0->TCD[ADC1_DMA_CHMUX].SLAST = -TOTAL_CHANNEL_NUMBER;

   //**** Destination address reload after major loop finish, no address reload needed
   DMA0->TCD[ADC1_DMA_CHMUX].DLAST_SGA = 0x00;

   //**** Common channel setting, no linking after major loop, no IRQ request enable
   DMA0->TCD[ADC1_DMA_CHMUX].CSR = 0x00;


// ******************************************************************************************************************

	//Channel 21 transfers ADC0 result data from ADC0_RA to SRAM buffer.
	// Enable Channel 1 and set ADC0 as DMA request source (source number 40)
   DMAMUX_CHCFG_REG(DMAMUX,ADC1_DMA_REC)  = DMAMUX_CHCFG_ENBL_MASK |DMAMUX_CHCFG_SOURCE(41);


	//**** Source address, ADC0_RA
	DMA0->TCD[ADC1_DMA_REC].SADDR = (uint32_t) &ADC1_RA;
	//**** Destination address, SRAM buffer [0]
	DMA0->TCD[ADC1_DMA_REC].DADDR = (uint32_t)&adc_result[0];

	//**** Source address increment; data is still read for the same address, no increment needed
	DMA0->TCD[ADC1_DMA_REC].SOFF = 0x00;

	//**** Destination address increment in bytes, increment for next buffer address
	//**** 16 bit => 2 bytes
	DMA0->TCD[ADC1_DMA_REC].DOFF = 0x02;

	//**** Source and destination data width specification, both source and destination is 16-bit
	DMA0->TCD[ADC1_DMA_REC].ATTR = DMA_ATTR_SSIZE(1)| DMA_ATTR_DSIZE(1);

	//**** Number of bytes for minor loop (one data transfer), ADC0 result is 16 bits long, so
	//**** 2-byte transfer
	DMA0->TCD[ADC1_DMA_REC].NBYTES_MLNO = 0x02;

	//**** Channel linking and major loop setting reload value after major loop finishes,
	//**** linking after minor loop is enabled, major loop transfers number 12 (0x0C).
	//DMA_TCD1_CITER_ELINKNO = (DMA_CITER_ELINKNO_ELINK_MASK|0x0C);
	//DMA_TCD1_CITER_ELINKNO = (DMA_CITER_ELINKNO_ELINK_MASK | 0x0C);
	DMA0->TCD[ADC1_DMA_REC].CITER_ELINKYES= (DMA_CITER_ELINKYES_ELINK_MASK | DMA_CITER_ELINKYES_LINKCH(ADC1_DMA_CHMUX) | DMA_CITER_ELINKYES_CITER(TOTAL_CHANNEL_NUMBER));


	//**** Channel linking and major loop setting, linking after minor loop is enabled to
	//**** channel 0 (0x0000), major loop transfers number 12 (0x0C)
	//DMA_TCD1_BITER_ELINKNO = (DMA_BITER_ELINKNO_ELINK_MASK|0x0C);
	//DMA_TCD1_BITER_ELINKNO  =(DMA_BITER_ELINKNO_ELINK_MASK|0x0000|0x0C);
	DMA0->TCD[ADC1_DMA_REC].BITER_ELINKYES= (DMA_BITER_ELINKYES_ELINK_MASK | DMA_BITER_ELINKYES_LINKCH(ADC1_DMA_CHMUX) | DMA_BITER_ELINKYES_BITER(TOTAL_CHANNEL_NUMBER));

	//**** Source address reload after major loop finishes, no reload needed

	DMA0->TCD[ADC1_DMA_REC].SLAST = 0x00;

	//**** Destination address reload after major loop finishes,
	//**** must be subtracted from last pointer value, sample number is 12 each and 2 bytes long,
	//**** 2 × 12 = 24 and must be subtract -24
	DMA0->TCD[ADC1_DMA_REC].DLAST_SGA = -(2*TOTAL_CHANNEL_NUMBER);

	//**** Common channel setting, linking after major loop enable to channel 0,
	//**** IRQ request is generated after major loop complete
	DMA0->TCD[ADC1_DMA_REC].CSR = DMA_CSR_INTMAJOR_MASK |DMA_CSR_MAJORELINK_MASK | DMA_CSR_MAJORLINKCH(ADC1_DMA_CHMUX);

	// Enable request signal for channel
	DMA_ERQ|= (1<<ADC1_DMA_REC);

	//Start the conversion
	DMA0->TCD[ADC1_DMA_CHMUX].CSR|=DMA_CSR_START_MASK;
	ADC1_SC1A = ADC_SC1_ADCH(adc_mux[0]);

}
