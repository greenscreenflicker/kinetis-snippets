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

// Simple delay - not for real programs!
void delay(void) {
   volatile unsigned long i;
   for (i=400000; i>0; i--) {
      __asm__("nop");
   }
}


void clkout_init(void){
	//Configure PCR[3] to clkout.
	 PORTC->PCR[3] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; //FTM0_CH0
	 SIM->SOPT2&=~SIM_SOPT2_CLKOUTSEL_MASK;
	 SIM->SOPT2|=SIM_SOPT2_CLKOUTSEL(0);
}

int main(void) {

   volatile int count = 0;

   // 1 ms tick
//   SysTick_Config(SystemBusClock/1000);

   led_initialise();
   clkout_init();
//   printf("SystemBusClock  = %ld\n", SystemBusClock);
//   printf("SystemCoreClock = %ld\n", SystemCoreClock);

   // Real programs never die!
   for(;;) {
      count++;
      greenLedToggle();
//      printf("Count = %d\n", count++);
      delay();
      int i=(SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV1_MASK)>>SIM_CLKDIV1_OUTDIV1_SHIFT;
     if(i){
    	 redLedOn();
     }else{
    	 redLedOff();
     }
	 if((SIM->SDID)==0){
		 redLedOff();
	 }
   }
   return 0;
}
