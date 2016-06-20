/**
 * @file delay.h
 * @brief Simple busy-waiting delay routines using SYST counter
 *
 *  Created on: 5 Oct 2015
 *      Author: podonoghue
 */
#ifndef INCLUDE_USBDM_DELAY_H_
#define INCLUDE_USBDM_DELAY_H_

#include <stdint.h>
#include "derivative.h"

#ifdef __cplusplus
namespace USBDM {
#endif

// Mask for maximum timer value
static uint32_t TIMER_MASK = ((1UL<<24)-1UL);

/**
 * Convert Milliseconds to timer ticks
 *
 * @return Time value in timer ticks
 */
static inline int64_t convertMSToTicks(uint32_t ms) {
   return ((uint64_t)ms * SystemCoreClock) / 1000;
}

/**
 * Enable timer
 *
 * Note this is only required if getTicks() is used by itself
 */
static inline void enableTimer() {
   // Reload with maximum value 2^24
   SYST->RVR = -1;

   // Enable without interrupts
   SYST->CSR = SYST_CSR_CLKSOURCE_MASK| // Processor clock
               SYST_CSR_ENABLE_MASK;    // Enabled
}

/**
 * Gets the number of ticks since the timer was started
 *
 * Note: the timer rolls over at TIMER_MASK ticks
 *
 * @return Number of ticks
 */
static inline uint32_t getTicks() {
   return SYST->CVR;
}

/**
 * Simple delay routine
 *
 * @param usToWait How many microseconds to busy-wait
 */
void waitUS(uint32_t usToWait);

/**
 * Simple delay routine
 *
 * @param msToWait How many milliseconds to busy-wait
 */
void waitMS(uint32_t msToWait);

#ifdef __cplusplus
} // End namespace USBDM
#endif

#endif /* INCLUDE_USBDM_DELAY_H_ */
