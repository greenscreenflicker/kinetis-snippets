/*
 * flexpwm.h
 *
 *  Created on: Aug 15, 2016
 *      Author: mmh
 */

#ifndef INCLUDES_FLEXPWM_H_
#define INCLUDES_FLEXPWM_H_

void FTM_init(void);
void FTM_setpwm(uint32_t ton, uint32_t tcy, uint32_t npulse, uint32_t nskip);
void FTM_clear_error(void);
void FTM_deadtime(uint32_t deadtime);
void FTM_mask(void);
void FTM_softstart_terminate(void);
void FTM_softstart(void);

#endif /* INCLUDES_FLEXPWM_H_ */
