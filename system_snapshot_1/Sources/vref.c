/*
 * vref.c
 *
 *  Created on: Jul 28, 2016
 *      Author: mmh
 */

#include "MK22F51212.h"

void VREF_init(void){
    uint8_t reg = 0U;

    /* Ungate clock for VREF */
    //CLOCK_EnableClock(s_vrefClocks[VREF_GetInstance(base)]);

    SIM->SCGC4|=SIM_SCGC4_VREF_MASK;
/* Configure VREF to a known state */
    /* Set chop oscillator bit */
    VREF->TRM |= VREF_TRM_CHOPEN_MASK;
    //reg = VREF->SC;
    /* Set buffer Mode selection and Regulator enable bit */
    reg |= VREF_SC_MODE_LV(1) | VREF_SC_REGEN_MASK;
    /* Set second order curvature compensation enable bit */
    reg |= VREF_SC_ICOMPEN_MASK;
    /* Enable VREF module */
    reg |= VREF_SC_VREFEN_MASK;
    /* Update bit-field from value to Status and Control register */
    VREF->SC = reg;

    // Wait until internal voltage stable
    while ((VREF->SC & VREF_SC_VREFST_MASK) == 0);
}




