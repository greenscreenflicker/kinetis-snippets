/*
 * vrefv1.c
 *
 *  Created on: 09.05.2016
 *      Author: Michael
 */
//Includes
#include "FRDM_K22F.h"
#include "system.h"
#include "clock_configure.h"
//delete sometime
#include "leds.h"
#include <stdio.h>

//Configures voltage refence to output 1.2V


/*******************************************************************************
 * Code
 ******************************************************************************/


void VREF_Init(void){
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

/*
void VREF_Deinit(VREF_Type *base)
{
    // Gate clock for VREF /
    CLOCK_DisableClock(s_vrefClocks[VREF_GetInstance(base)]);
}*/


/*
void VREF_SetTrimVal(VREF_Type *base, uint8_t trimValue)
{
    uint8_t reg = 0U;

    // Set TRIM bits value in voltage reference
    reg = base->TRM;
    reg = ((reg & ~VREF_TRM_TRIM_MASK) | VREF_TRM_TRIM(trimValue));
    base->TRM = reg;
    // Wait until internal voltage stable
    while ((base->SC & VREF_SC_VREFST_MASK) == 0)
    {
    }
*/
