/*
 * adc2.c
 *
 *  Created on: Aug 5, 2016
 *      Author: mmh
 */
#include "MK22F51212.h"



void ADC1_init(void)
{
	// Enable the necessary clocks : 1) for the ADC0 Module ; 2) for the PORTB
	SIM_SCGC6 |= SIM_SCGC6_ADC1_MASK;	// ADC 0 clock
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;	// PTB0 clock

	//Enable grid measurement-
	PORTC_PCR11 = PORT_PCR_MUX(0);

	// Calibrate the ADC0
	//ADC_calibrate();

    //  1.2 V VREF_OUT is connected as the VALT reference option
	ADC1_SC2 |= ADC_SC2_REFSEL(1) ;


	// Reset the CFG1- register
	ADC1_CFG1 = 0;
	// Set ADC clock frequency fADCK as the bus Clock (60 Mhz)
	// Devide the clock by 4 to minimize the noise on the PWM output
	// configure the module for 16-bit resolution
	/* Sampling time configuration : sample time is the time that the internal switch is closed ans sample capacitor is charged.
	Long sample Time has been choosen for better results: This allows higher impedance inputs to be accurately sampled or to maximize
	conversion speed for lower impedance inputs. Longer sample times can also be used to lower overall
	power consumption when continuous conversions are enabled if high conversion rates are not required. */
	ADC1_CFG1 |=  ADC_CFG1_ADICLK(0)| ADC_CFG1_ADIV(3) | ADC_CFG1_MODE(3) | ADC_CFG1_ADLSMP(1) ;

	// Default longest sample time; 20 extra ADCK cycles; 24 ADCK cycles total ==> this results a very stable state compared to short sample time
	// High-speed conversion sequence selected with 2 additional ADCK cycles to total conversion time.
	/// MUX -> b required
	ADC1_CFG2 = ADC_CFG2_MUXSEL_MASK | ADC_CFG2_ADLSTS(0) | ADC_CFG2_ADHSC_MASK ;

    // SC1A is used for both software and hardware trigger modes of operation. Disable the module till the first read function
	ADC1_SC1A |= ADC_SC1_ADCH(31);
}

uint16_t ADC1_read(unsigned char channel)
{   // writes to SC1A subsequently initiate a new conversion, if SC1[ADCH] contains a value other than all 1s. (other than disabled)
	ADC1_SC1A = (channel & ADC_SC1_ADCH_MASK) ;
	// Conversion in progress. wait for it to finish
	while(ADC1_SC2 & ADC_SC2_ADACT_MASK);
	// wait for the conversion to complete
	while(!(ADC1_SC1A & ADC_SC1_COCO_MASK));
	return ADC1_RA;
}

