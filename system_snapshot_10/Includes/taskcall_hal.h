/*
 * taskcall_hal.h
 *
 *  Created on: 17.07.2015
 *      Author: Michael Heidinger
 */

#ifndef PROJECT_HEADERS_TASKCALL_HAL_H_
#define PROJECT_HEADERS_TASKCALL_HAL_H_

uint32_t _taskcaller_get_time();
void _taskcaller_set_wakeup(uint64_t recall);
uint32_t _taskcaller_get_time_passive();



#endif /* PROJECT_HEADERS_TASKCALL_HAL_H_ */
