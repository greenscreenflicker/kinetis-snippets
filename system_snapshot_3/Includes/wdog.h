/*
 * wdog.h
 *
 *  Created on: Jul 4, 2016
 *      Author: mmh
 */

#ifndef INCLUDES_WDOG_H_
#define INCLUDES_WDOG_H_

#include "taskcall.h"

void wdog_init(void);
void wdog_refresh(void);
void wdog_refresh_func(int64_t delay);
void startup_reason_report(void);

extern taskcall_t task_watchdog_reset;

#endif /* INCLUDES_WDOG_H_ */
