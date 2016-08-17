/*
 * leds.c
 *
 *  Created on: Jun 20, 2016
 *      Author: mmh
 */


#include "MK22F51212.h"

void init_leds(void){
	//PORTA1 red
	SIM_SCGC5|=SIM_SCGC5_PORTA_MASK|SIM_SCGC5_PORTD_MASK;
	GPIOA_PDDR|=(1<<1);
	PORTA_PCR1 = PORT_PCR_MUX(1)|PORT_PCR_DSE_MASK;
	//PORTA2 green
	GPIOA_PDDR|=(1<<2);
	PORTA_PCR2 = PORT_PCR_MUX(1)|PORT_PCR_DSE_MASK;


	//OIRTD5 blue
	GPIOD_PDDR|=(1<<5);
	PORTD_PCR5 = PORT_PCR_MUX(1)|PORT_PCR_DSE_MASK;

	//turn all leds off
	GPIOA_PCOR=(1<<1);
	GPIOD_PCOR=(1<<3)|(1<<13);
}

void red_toogle(void){
	GPIOA_PTOR=(1<<1);
}


void red_on(void){
	GPIOA_PCOR=(1<<1);
}

void red_off(void){
	GPIOA_PSOR=(1<<1);
}


void blue_on(void){
	GPIOD_PCOR=(1<<5);
}

void blue_off(void){
	GPIOD_PSOR=(1<<5);
}

void blue_toggle(void){
	GPIOD_PTOR=(1<<5);
}

void green_on(void){
	GPIOA_PCOR=(1<<2);
}

void green_off(void){
	GPIOA_PSOR=(1<<2);
}

void green_toggle(void){
	GPIOA_PTOR=(1<<2);
}


