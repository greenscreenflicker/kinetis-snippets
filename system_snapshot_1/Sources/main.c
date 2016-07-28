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
#include "uart1.h"
#include "mcu_tracer.h"
#include "leds.h"
#include "taskcall.h"
#include "wdog.h"
#include "lowvoltagewarning.h"

void led_count_task(int64_t delay);
taskcall_t task_led_lauflicht={"leds",0,NULL,&led_count_task,NULL,NULL};


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

	GPIOB_PSOR=(1<<count);
	count++;
	if((CMP1->SCR & CMP_SCR_COUT_MASK)){
		if(count>1) count=0;
	}else{
		if(count>3) count=0;
	}
	_taskcall_task_register_time(&task_led_lauflicht,(120000000/4));
}

int cmp_init(void){
	//Clock to the comparator
	SIM->SCGC4|=SIM_SCGC4_CMP_MASK;
	CMP1->CR1=CMP_CR1_EN_MASK; // Enable the comparator
	//Enable Dac, VIn1, Voltage 19/32*Vref
	CMP1->DACCR=CMP_DACCR_DACEN(1)|CMP_DACCR_VRSEL(0)|CMP_DACCR_VOSEL(40);
	CMP1->MUXCR=CMP_MUXCR_PSEL(1)|CMP_MUXCR_MSEL(7); //+=Sense -=DAC

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
	cmp_init();

	// printf("SystemBusClock  = %ld\n", SystemBusClock);
	//printf("SystemCoreClock = %ld\n", SystemCoreClock);
	tracer_green=1;
	_taskcall_task_register_time(&task_watchdog_reset,(120000000*2));
	_taskcall_task_register_time(&task_led_lauflicht,(120000000*3));
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
