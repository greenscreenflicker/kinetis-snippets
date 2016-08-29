/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <stdio.h>
#include "MK22F51212.h"
#include "adc.h"
#include "adcdma1.h"
#include "uart1.h"
#include "mcu_tracer.h"
#include "leds.h"
#include "taskcall.h"
#include "wdog.h"
#include "lowvoltagewarning.h"
#include "Si8900.h"
#include "flexpwm.h"
#include "vref.h"


void led_count_task(int64_t delay);
void adc_conv_test(int64_t delay);

taskcall_t task_led_lauflicht={"leds",0,NULL,&led_count_task,NULL,NULL};
taskcall_t task_adc_conv_test={"adct",0,NULL,&adc_conv_test,NULL,NULL};

void led_sample_init(void){
	SIM_SCGC5|=SIM_SCGC5_PORTB_MASK;
	GPIOB_PDDR|=(1<<0)|(1<<1)|(1<<2)|(1<<3);
	PORTB_PCR0 = PORT_PCR_MUX(1)|PORT_PCR_DSE_MASK;
	PORTB_PCR1 = PORT_PCR_MUX(1)|PORT_PCR_DSE_MASK;
	PORTB_PCR2 = PORT_PCR_MUX(1)|PORT_PCR_DSE_MASK;
	PORTB_PCR3 = PORT_PCR_MUX(1)|PORT_PCR_DSE_MASK;
	GPIOB_PSOR=(1<<0)|(1<<1)|(1<<2)|(1<<3);
}


int count=0;
void led_count_task(int64_t delay){
	GPIOB_PCOR=(1<<0)|(1<<1)|(1<<2)|(1<<3);
	if(led_lauflicht==1){
		GPIOB_PSOR=(1<<count);
		count++;
		if(((UART1->S1 & (UART_S1_FE_MASK|UART_S1_OR_MASK|UART_S1_PF_MASK|UART_S1_NF_MASK)) != 0)){
			if(count>1) count=0;
		}else{
			if(count>3) count=0;
		}
	}else if(led_lauflicht==2){
		GPIOB_PSOR=(1<<3)|(1<<0);
	}
	_taskcall_task_register_time(&task_led_lauflicht,(120000000/4));
}

/*
int cmp_init(void){
	//Clock to the comparator
	SIM->SCGC4|=SIM_SCGC4_CMP_MASK;
	CMP1->CR1=CMP_CR1_EN_MASK; // Enable the comparator
	//Enable Dac, VIn1, Voltage 19/32*Vref
	CMP1->DACCR=CMP_DACCR_DACEN(1)|CMP_DACCR_VRSEL(0)|CMP_DACCR_VOSEL(40);
	CMP1->MUXCR=CMP_MUXCR_PSEL(1)|CMP_MUXCR_MSEL(7); //+=Sense -=DAC

}*/

float global_adc_voltage;
float global_adc_grid;
float global_overcurrent;
uint32_t global_clear_error;

uint32_t global_pwm_ton=40;
uint32_t global_pwm_tcy=80;
uint32_t global_pwm_tp=1;
uint32_t global_pwm_tpsc=1;


uint32_t global_clear_error;
float global_adc_dclink;

float global_si8900_uled;
float global_si8900_iled;
float global_si8900_secsup;


void adc_conv_test(int64_t delay){
	char buf[50];
	uint16_t adcvalue=ADC_read(3);

	double Rb=100.0;
	double Rt=1000.0;
	double Uref=1.1972;
	double Bits=65535;

	float convert=((Rb+Rt)*Uref)/(Rb*Bits);
	float voltage;
	voltage=convert*adcvalue;
	global_adc_voltage=voltage;
	//sprintf(buf,"ADC: %f",1.234);
	//mcu_tracer_msg(buf);

	uint16_t grid_adc=adc_result[2];
	convert=(4000*Uref)/(5.5*Bits);
	global_adc_grid=(grid_adc-32768)*convert;

	Rb=2.2;
	Rt=2000;
	convert=((Rb+Rt)*Uref)/(Rb*Bits);
	float dclink=convert*adc_result[0];
	global_adc_dclink=dclink;







	//if(CMP1_SCR & CMP_SCR_COUT_MASK){
	if(CMP0->SCR & CMP_SCR_COUT_MASK){
		global_overcurrent=1;
	}else{
		global_overcurrent=0;
	}
	if(global_clear_error){
		FTM_clear_error();
		global_clear_error=0;
	}

	//SI8900_manual_req(0);
	Rb=150.0;
	Rt=1000.0;
	Uref=2.495;
	Bits=1024;
	convert=((Rb+Rt)*Uref)/(Rb*Bits);

	float convercur=(Uref)/(Bits);
	global_si8900_uled=SI8900_get_ch(0)*convert;
	global_si8900_iled=SI8900_get_ch(1)*convercur;

	global_si8900_secsup=SI8900_get_ch(2)*convert;
	//Pwm updates

	SI8900_void_data();

	//mcu_tracer_msg("asd");
	//global_pwm_ton
	//FTM_setpwm(global_pwm_ton,global_pwm_tcy,global_pwm_tp,global_pwm_tpsc);

	regul_regulate();

	_taskcall_task_register_time(&task_adc_conv_test,(120000000/29));

}

int main(void)
{

    /* Write your code here */
	SystemCoreClockUpdate();
	lvd_init();

	led_sample_init();


	mcu_tracer_config();
	startup_reason_report();

	//Inits
	FTM_init();
	VREF_init();
	ADC_init();
	//ADC1_init();
	//ADC1_dmaconfig();
	ADC_dma1_init();

	Si8900_init();

	//SI8900_sync();
	//led_sample_init();

	// printf("SystemBusClock  = %ld\n", SystemBusClock);
	//printf("SystemCoreClock = %ld\n", SystemCoreClock);


	_taskcall_task_register_time(&task_watchdog_reset,(120000000*2));
	_taskcall_task_register_time(&task_led_lauflicht,(120000000*3));
	_taskcall_task_register_time(&task_adc_conv_test,(120000000));
	_taskcall_start();

   while(1){
	   //put here low priority tasks, like communication
       mcu_tracer_process();
       mainloop_iterations=mainloop_iterations+1;
       if(mainloop_iterations>1000000) mainloop_iterations=0;
   }

    /* This for loop should be replaced. By default this loop allows a single stepping. */
	init_leds();
	while(1);
    /* Never leave main */
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
