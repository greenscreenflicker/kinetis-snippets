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


#endif /* INCLUDES_FLEXPWM_H_ */
