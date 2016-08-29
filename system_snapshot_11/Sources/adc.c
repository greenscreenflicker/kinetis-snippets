/*
 * adc.c
 *
 *  Created on: Aug 1, 2016
 *      Author: mmh
 */

#include "MK22F51212.h"
#include "mcu_tracer.h"


//The calibration function sets the offset calibration value, the minus-side calibration values, and the plus-side calibration values.
/* For best calibration results: (See page 795 of the reference manual)
• Set hardware averaging to maximum, that is, SC3[AVGE]=1 and SC3[AVGS]=11 for an average of 32
• Set ADC clock frequency fADCK less than or equal to 4 MHz
• VREFH=VDDA
• Calibrate at nominal voltage and temperature
return 1 if failed ==> ADC accuracy specifications are not guaranteed, otherweise return 0 */
int ADC_calibrate(void){
	// Set ADC clock frequency fADCK less than or equal to 4 MHz ==> input clock is the ALTCLK OSCERCLK (see page 105) [8 Mhz]
	// Devide the clock by 4 ==>  fADK = 2Mhz (this clock is just used for the calibration process)
	// choose the 16-bits mode
	ADC0_CFG1 = ADC_CFG1_ADICLK(0)| ADC_CFG1_ADIV(3) | ADC_CFG1_MODE(3);
	//  VREFH=VDDA reference option
	ADC0_SC2  = ADC_SC2_REFSEL(1);
	ADC0_SC3 &=~ADC_SC3_CAL_MASK;

	// Set hardware averaging to maximum, that is, SC3[AVGE]=1 and SC3[AVGS]=11 (3) for an average of 32
	// and then start the calibration

	ADC0_SC3 = ADC_SC3_AVGS(3) | ADC_SC3_AVGE_MASK | ADC_SC3_CAL_MASK;
	// Wait for the calibration to be done
	while(ADC0_SC3 & ADC_SC3_CAL_MASK);
	// Check if any error occured while calibrating and return 1 if failed ==> ADC accuracy specifications are not guaranteed, otherweise return 0
    if(ADC0_SC3 & ADC_SC3_CALF_MASK){
    	mcu_tracer_msg("ADC Calibration error");
    	while(1);
    }

	// follow the steps on page 796:

    // Initialize or clear a 16-bit variable in RAM.
    uint16_t variable = 0;
    // Add the plus-side calibration results CLP0, CLP1, CLP2, CLP3, CLP4, and CLPS to the variable.
    variable += ADC0_CLPS +  ADC0_CLP0 + ADC0_CLP1 + ADC0_CLP2 + ADC0_CLP3 + ADC0_CLP4;
    // Divide the variable by two.
    variable /= 2;
    // Set the MSB of the variable.
    variable |= (1<<15);
	// Store the value in the plus-side gain calibration register PG.
    ADC0_PG = variable;
	// Repeat the procedure for the minus-side gain calibration value.
	variable = 0;
	variable += ADC0_CLMS + ADC0_CLM0 + ADC0_CLM1 + ADC0_CLM2 + ADC0_CLM3 + ADC0_CLM4;
	variable /= 2;
    variable |= (1<<15);
	ADC0_MG = variable;

	return 0;
}

void ADC_init(void)
{
	// Enable the necessary clocks : 1) for the ADC0 Module ; 2) for the PORTB
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;	// ADC 0 clock
//	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;	// PTB0 clock


	// Calibrate the ADC0
	ADC_calibrate();
	ADC_calibrate(); //second calibration needed, as first is typically void.
	ADC_calibrate();

    //  1.2 V VREF_OUT is connected as the VALT reference option
	ADC0_SC2 |= ADC_SC2_REFSEL(1) ;


	// Reset the CFG1- register
	ADC0_CFG1 = 0;
	// Set ADC clock frequency fADCK as the bus Clock (60 Mhz)
	// Devide the clock by 4 to minimize the noise on the PWM output
	// configure the module for 16-bit resolution
	/* Sampling time configuration : sample time is the time that the internal switch is closed ans sample capacitor is charged.
	Long sample Time has been choosen for better results: This allows higher impedance inputs to be accurately sampled or to maximize
	conversion speed for lower impedance inputs. Longer sample times can also be used to lower overall
	power consumption when continuous conversions are enabled if high conversion rates are not required. */
	ADC0_CFG1 |=  ADC_CFG1_ADICLK(0)| ADC_CFG1_ADIV(3) | ADC_CFG1_MODE(3) | ADC_CFG1_ADLSMP(1) ;

	// Default longest sample time; 20 extra ADCK cycles; 24 ADCK cycles total ==> this results a very stable state compared to short sample time
	// High-speed conversion sequence selected with 2 additional ADCK cycles to total conversion time.
	ADC0_CFG2 = ADC_CFG2_ADLSTS(0) | ADC_CFG2_ADHSC_MASK ;

    // SC1A is used for both software and hardware trigger modes of operation. Disable the module till the first read function
	ADC0_SC1A |= ADC_SC1_ADCH(31);
}

/* This function reads the channel given in the parantheses and returns the result saved in the ADCx_RA register
   The channel number is listed under ADCH in the ADCx_SC1n registerFor our case we used the channel ADC0_SE8  */
unsigned short ADC_read(unsigned char channel)
{   // writes to SC1A subsequently initiate a new conversion, if SC1[ADCH] contains a value other than all 1s. (other than disabled)
	ADC0_SC1A = (channel & ADC_SC1_ADCH_MASK) ;
	// Conversion in progress. wait for it to finish
	while(ADC0_SC2 & ADC_SC2_ADACT_MASK);
	// wait for the conversion to complete
	while(!(ADC0_SC1A & ADC_SC1_COCO_MASK));
	return ADC0_RA;
}
