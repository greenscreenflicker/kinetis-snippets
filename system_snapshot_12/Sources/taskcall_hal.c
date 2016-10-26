/*
 * taskcall_hal.c
 *
 *  Created on: 17.07.2015
 *      Author: Michael Heidinger
 */

#include <stdlib.h>
#include <stdint.h>
#include "MK22F51212.h"
#include "taskcall.h"

//#include "core_cm0plus.h"

uint32_t time_comulator=0;


uint32_t _taskcaller_get_time(){
	uint32_t returnval;
	if((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)){
		//overflow occured
		//SysTick->CTRL &=~(SysTick_CTRL_COUNTFLAG_Msk);
		returnval=time_comulator+2*SysTick->LOAD-SysTick->VAL;
		SysTick->VAL=0;
		time_comulator=0;
		return returnval;
	}
	returnval=time_comulator+SysTick->LOAD-SysTick->VAL;
	SysTick->VAL=0;
	time_comulator=0;
	return returnval;
}

uint32_t _taskcaller_get_time_passive(){
	//gets time (no resting of value)
	uint32_t returnval;
	uint32_t control=SysTick->CTRL;
	if((control & SysTick_CTRL_COUNTFLAG_Msk)){
		//overflow occured
		//flag gets cleared, as we are passive, we have to set it again
		//SysTick->CTRL &=~(SysTick_CTRL_COUNTFLAG_Msk);
		returnval=2*SysTick->LOAD-SysTick->VAL;
		SysTick->VAL=0;
		time_comulator=time_comulator+returnval;
		return time_comulator;
	}
	returnval=SysTick->LOAD-SysTick->VAL;
	SysTick->VAL=0;
	time_comulator=time_comulator+returnval;
	return time_comulator;
}

void _taskcaller_set_wakeup(uint64_t recall){

	if(recall>SysTick_LOAD_RELOAD_Msk){
		recall=SysTick_LOAD_RELOAD_Msk;
	}
	time_comulator=time_comulator+(SysTick->LOAD-SysTick->VAL);
	SysTick->LOAD=recall-1;
	SysTick->VAL=0;

}

