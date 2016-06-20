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
#include "clock_configure.h"
#include "uart1.h"
#include "string.h"

// Simple delay - not for real programs!
void delay(void) {
   volatile unsigned long i;
   for (i=4000000; i>0; i--) {
      __asm__("nop");
   }
}

/* Example use of interrupt handler
 *
 * The standard ARM libraries provide basic support for the system timer
 * This function is used for the System Timer interrupt handler.
 *
 */
//void SysTick_Handler(void) {
//   __asm__("nop");
//}

int main(void) {

   volatile int count = 0;

   // 1 ms tick
//   SysTick_Config(SystemBusClock/1000);
   //clock_initialise();
   led_initialise();
   //hsRunMode(1);
//   printf("SystemBusClock  = %ld\n", SystemBusClock);
//   printf("SystemCoreClock = %ld\n", SystemCoreClock);

   // Real programs never die!
   char str[100]="-";
   char str2[100];
   for(;;) {
      count++;
      greenLedToggle();
//      printf("Count = %d\n", count++);
      delay();
      //UART1_dmaconfig();
      //UART1_txChar('c');

      UART1_txBulk((uint8_t*)str,(uint16_t)strlen(str));

      int size=UART1_rec_size();
      if(size){
		  while(size){
			  char ch=UART1_getch();

			  UART1_dma_complete_wait(); //we need to wait until the string to write to is not longer required.
			  sprintf(str2,"(%i|%c)\n",size,ch);
			  UART1_txBulk((uint8_t*)str2,(uint16_t)strlen(str2));
			  size--;
		  }
      }
      //UART1_rec_buf_reset();


   }
   return 0;
}
