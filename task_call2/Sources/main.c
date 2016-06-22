/*
 ============================================================================
 * main.c
 *
 *  Created on: 04/12/2012
 *      Author: podonoghue
 ============================================================================
 */
#include <stdio.h>
#include "system.h"
#include "derivative.h"
#include "utilities.h"
#include "leds.h"
#include "uart.h"
#include "taskcall.h"

void task_toogle_blue_led(int64_t delay);
void task_toogle_red_led(int64_t delay);
void task_toogle_green_led(int64_t delay);
// Wir definieren hier die Tasks die registriert werden sollen (holen uns also hier den memory)
taskcall_t blink_blue ={"blue_b",0,NULL,&task_toogle_blue_led,NULL,NULL};
taskcall_t blink_red  ={"red_b",0,NULL,&task_toogle_red_led,NULL,NULL};
taskcall_t blink_green={"green_b",0,NULL,&task_toogle_green_led,NULL,NULL};
/*taskcall_t blink_green={"green_b",5,NULL,NULL,NULL,NULL};
taskcall_t blink_red={"red_b",17,NULL,NULL,NULL,NULL};
taskcall_t blink_pink={"pink",10,NULL,NULL,NULL,NULL};
taskcall_t blink_yel1={"yel1",14,NULL,NULL,NULL,NULL};
taskcall_t blink_yel2={"yel2",14,NULL,NULL,NULL,NULL};*/

// Simple delay - not for real programs!
void delay(void) {
   volatile unsigned long i;
   for (i=4800000; i>0; i--) {
      __asm__("nop");
   }
}

/* Example use of interrupt handler
 *
 * The standard ARM libraries provide basic support for the system timer
 * This function is used for the System Timer interrupt handler.
 *
 */
void SysTick_Handler(void) {
	//blueLedToggle();

	_taskcall_exec();
	//greenLedToggle();
}

void task_toogle_blue_led(int64_t delay){
	blueLedToggle();
	_taskcall_task_register_time(&blink_blue,1000000-delay);
}

void task_toogle_red_led(int64_t delay){
	//redLedToggle();
	_taskcall_task_register_time(&blink_red, 1000000-delay);
}


void task_toogle_green_led(int64_t delay){
	greenLedToggle();
	_taskcall_task_register_time(&blink_green,2000000-delay);
}

int main(void) {

   volatile int count = 0;


   led_initialise();



   printf("SystemBusClock  = %ld\n", SystemBusClock);
   printf("SystemCoreClock = %ld\n", SystemCoreClock);

   _taskcall_task_register(&blink_blue);
   _taskcall_task_register(&blink_red);
   _taskcall_task_register(&blink_green);
   __enable_irq();
   SysTick_Config(100);

   while(1){
	   _taskcall_task_list_dump();
	   _taskcall_cpu_load();
	   delay();
   }
   return 0;
}

/* old testing code
 *    _taskcall_task_register(&blink_blue);
   _taskcall_task_list_dump();
   _taskcall_task_register(&blink_green);
   _taskcall_task_list_dump();
   _taskcall_task_register(&blink_red);
   _taskcall_task_list_dump();
   _taskcall_task_register(&blink_pink);
   _taskcall_task_list_dump();
   _taskcall_task_register(&blink_yel1);
   _taskcall_task_list_dump();
   _taskcall_task_register(&blink_yel1);
   _taskcall_task_list_dump();
   _taskcall_task_register(&blink_red);
   _taskcall_task_list_dump();
   _taskcall_task_register(&blink_yel2);
   _taskcall_task_list_dump();
   printf("Testing removing...\n");
   _taskcall_task_remove(&blink_green);
   _taskcall_task_list_dump();
   printf("Subtracting TIme...\n");
   _taskcall_substract_time(13);
   _taskcall_task_list_dump();
   _taskcall_task_first_remove();
   _taskcall_task_list_dump();
   // Real programs never die!
   printf("Completed.\n");
   SysTick_Config(1500000); */
