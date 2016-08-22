/*
 * lowvoltagewarning.c
 *
 *  Created on: Jul 5, 2016
 *      Author: mmh
 */
#include <stdio.h>
#include "MK22F51212.h"
#include "mcu_tracer.h"
#include "taskcall.h"


void lvd_reset(void);

taskcall_t reset_lvd={"res_lvd",0,NULL,&lvd_reset,NULL,NULL};



void lvd_init(void){
	//Configure low voltage reset
	PMC->LVDSC1=
			PMC_LVDSC1_LVDRE(1) | //force a reset on low voltage
			PMC_LVDSC1_LVDV(1) | //high trip point;
			0;

	PMC->LVDSC2=
			PMC_LVDSC2_LVWV(0b11) | // highest warning point
			PMC_LVDSC2_LVWIE_MASK | //enable low voltage warning interrupt
			PMC_LVDSC2_LVWACK_MASK; //remove warning byte
	NVIC_EnableIRQ(LVD_LVW_IRQn);
}


void LVD_LVW_IRQHandler(){
	uint8_t warningpoint;
	warningpoint=((PMC->LVDSC2 & PMC_LVDSC2_LVWV_MASK)>>PMC_LVDSC2_LVWV_SHIFT);

	char buf[50];
	sprintf(buf,"Low Voltage %i",warningpoint);
	mcu_tracer_msg(buf);
	//Set warning point to 3
	if(warningpoint>0){
		PMC->LVDSC2=
				PMC_LVDSC2_LVWV(warningpoint-1) | // highest warning point
				PMC_LVDSC2_LVWIE_MASK | //enable low voltage warning interrupt
				PMC_LVDSC2_LVWACK_MASK; //remove warning byte
	}else{
		PMC->LVDSC2=PMC_LVDSC2_LVWACK_MASK; //remove warning byte
	}
	_taskcall_task_remove(&reset_lvd);
	_taskcall_task_register_time(&reset_lvd,120000000);
	//PMC->LVDSC2|=PMC_LVDSC2_LVWACK_MASK;
}

void lvd_reset(void){
	PMC->LVDSC2=
			PMC_LVDSC2_LVWV(0b11) | // highest warning point
			PMC_LVDSC2_LVWIE_MASK | //enable low voltage warning interrupt
			PMC_LVDSC2_LVWACK_MASK; //remove warning byte

	mcu_tracer_msg("Low Voltage reset");
}

