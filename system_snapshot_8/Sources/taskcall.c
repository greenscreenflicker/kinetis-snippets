/*
 * taskcall.c
 *
 *  Created on: 17.07.2015
 *      Author: Michael Heidinger
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <taskcall.h>
#include "taskcall_hal.h"
#include "wdog.h"
#include "MK22F51212.h"

taskcall_t* taskcall_head=NULL;

uint64_t _taskcall_active_time;
uint64_t _taskcall_sleep_time;

void _taskcall_taskname(taskcall_t* task){
	printf("Taskname: %s\n",task->taskname);
}

void _taskcall_task_register_time(taskcall_t* task_ptr, int64_t time){
	task_ptr->time=time;
	_taskcall_task_register(task_ptr);
}

/* Registers a task in the link list according to the priority */
void _taskcall_task_register(taskcall_t* task_ptr){
	taskcall_t* iterate=taskcall_head; // the current iteration
	taskcall_t* iterate_minus_one=NULL; // the element before the current iteration
	while(iterate){
		if((task_ptr->time)<(iterate->time)){
			//We have found our place
			break;
		}
		//Position important!
		iterate_minus_one=iterate;
		iterate=iterate->next;
	}

	if(iterate_minus_one==NULL){
		//The first element we insert
		task_ptr->next=taskcall_head;
		taskcall_head=task_ptr;
	}else{
		//second or following element
		iterate_minus_one->next=task_ptr;
		task_ptr->next=iterate;
	}
/*
	printf("Task %s registered: %s->%s->%s\b\n",
			task_ptr->taskname,
			(iterate_minus_one)?(iterate_minus_one->taskname):("[ne]"),
			task_ptr->taskname,
			(iterate)?(iterate->taskname):("[ne]"));
*/

}

void _taskcall_task_remove(taskcall_t* task2rem){
	taskcall_t* iterate=taskcall_head; // the current iteration
	taskcall_t* iterate_minus_one=NULL; // the element before the current iteration
	while(iterate){
		if(iterate==task2rem){
			if(iterate_minus_one){
				iterate_minus_one->next=task2rem->next;
			}else{
				taskcall_head=task2rem->next;
			}
			return;
		}
		//Position important!
		iterate_minus_one=iterate;
		iterate=iterate->next;
	}

}

void _taskcall_task_first_remove(void){
	taskcall_t* second=taskcall_head->next;
	taskcall_head->next=NULL; // Just for safety...
	taskcall_head=second;
}


void _taskcall_substract_time(uint32_t subst_time){
	taskcall_t* iterate=taskcall_head; // the current iteration
	while(iterate){
		iterate->time=iterate->time-subst_time;
		iterate=iterate->next;
	}
}


void _taskcall_task_list_dump(void){
	taskcall_t* iterate=taskcall_head; // the current iteration
	printf("TaskList follows:\n");
	while(iterate){
		printf("->%s: t=%ld\n",iterate->taskname,iterate->time);
		iterate=iterate->next;
	}
}

void _taskcall_start(void){
	__enable_irq();
	SysTick_Config(100);
}

void _taskcall_exec(void){
	while(taskcall_head){
		int64_t time2execute;
		taskcall_t *execute=NULL;
		//uint32_t passivetime=_taskcaller_get_time_passive();
		//avoids looping in when multiple tasks need to be exectuted
		//(==> Speed improvement)
		_taskcall_substract_time(_taskcaller_get_time());
		//
		time2execute=taskcall_head->time;

		if(time2execute<1){
			//if element is ready to execute, we need to execute
			//Calculate Time
			//remove executed task
			execute=taskcall_head; // Working copy
			_taskcall_task_first_remove();
			//execute it!
			execute->task_ptr(-time2execute);
			//wdog_refresh();
			_taskcall_active_time+=_taskcaller_get_time_passive();
		}else if(taskcall_head->time>10){
			//Is it really worth going to sleep?
			_taskcall_sleep_time+=taskcall_head->time;
			_taskcaller_set_wakeup(taskcall_head->time);
			return;
		}
	}

	//Prepare to sleep
	//ToDO

}

void _print64_bit(void){
	/*
	  static char bfr[20+1];
	  char* p = bfr + sizeof(bfr);
	  *(--p) = '\0';
	  for (bool first = true; v || first; first = false) {
	    const uint32_t digit = v % 10;
	    const char c = '0' + digit;
	    *(--p) = c;
	    v = v / 10;
	  }*/
}

void _taskcall_cpu_load(void){
	//

	printf("CPU Load: %i active %lf sleep\n",(long long)_taskcall_active_time,(long long)_taskcall_sleep_time);
	_taskcall_active_time=0;
	_taskcall_sleep_time=0;

}
void SysTick_Handler(void) {
	_taskcall_exec();
}

