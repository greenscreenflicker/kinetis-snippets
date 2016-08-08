/*
 * flexpwm.c
 *
 *  Created on: Jul 27, 2016
 *      Author: mmh
 */

#include "MK22F51212.h"

void FTM_init(void){
	// Enable clock to TIMER FTM0
   SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;

   // Enable clock to port pins used by FTM0, we use port C
   SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

   //Set pin to right output
   PORTC->PCR[1] = PORT_PCR_MUX(4)| PORT_PCR_DSE_MASK; //FTM0_CH0
   PORTC->PCR[2] = PORT_PCR_MUX(4)| PORT_PCR_DSE_MASK; //FTM0_CH1

   FTM0->MODE&=~(FTM_MODE_WPDIS_MASK);
   FTM0->MODE|=(FTM_MODE_FTMEN_MASK);

   /*	FTM0->MOD = 79;
	FTM0->CONTROLS[0].CnSC= (FTM_CnSC_MS(0b10) | FTM_CnSC_ELS(0b10));   // No Interrupts; High True pulses on Edge Aligned PWM
	FTM0->CONTROLS[1].CnSC= (FTM_CnSC_MS(0b01) | FTM_CnSC_ELS(0b01));
	FTM0->CONTROLS[0].CnV=40;  // 50% pulse width
	FTM0->CONTROLS[1].CnV=40;
	FTM0->SC&=~FTM_SC_PS_MASK; //check prescaler is zero
	FTM0->QDCTRL&=~FTM_QDCTRL_QUADEN_MASK; //check that quadrature encoder is disabled
	FTM0->SC&=~FTM_SC_CPWMS_MASK; // upcounting mode
	FTM0->CNTIN=0;
	FTM0->CNT=0;
	FTM0->SC=FTM_SC_CLKS(0b01)|FTM_SC_PS(0);   //  Edge Aligned PWM running from System Clock / 1
*/
	//Idiotentest ansatz
	FTM0->CONF=0xC0; //set up BDM in 11
	FTM0->FMS=0x00; //clear the WPEN so that WPDIS is set in FTM0_MOD reg

	FTM0->MODE|=0x05; //enable write the FTM CnV register
	FTM0->MOD=79;
	FTM0->CONTROLS[0].CnSC=0x28; //edge-alignment,	PWM initial state is High, becomes low
	FTM0->CONTROLS[1].CnSC=0x28;
	FTM0->COMBINE=0x02; //complementary mode for CH0&CH1 of FTM0
	FTM0->COMBINE|=0x10; //dead timer insertion enabled in complementary mode for //CH0&CH1 of FTM0
	FTM0->CONTROLS[0].CnV=40;
	FTM0->CONTROLS[1].CnV=40;
	//FTM0_C2SC=0x28;
	//FTM0_C3SC=0x28;
	//FTM0->COMBINE|=0x0200;
	//FTM0->COMBINE|=0x1000;
	FTM0->DEADTIME=5;
	FTM0->CNTIN=0x00;
	//FTM0->SC=0x08; //PWM edge_alignment, system clock driving, dividing by 1

	//PIN28 - PORTA.12 - ALT3
	//Enable clock to FTM1
	SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
	// Enable clock to port pins used by FTM1, we use port A
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	//Set pin to right output Porta , ALT3, Drive Strength
	PORTA->PCR[12] = PORT_PCR_MUX(3)| PORT_PCR_DSE_MASK; //FTM0_CH0


	FTM1->MODE&=~(FTM_MODE_WPDIS_MASK);
	FTM1->MODE|=(FTM_MODE_FTMEN_MASK);
	//Idiotentest ansatz
	FTM1->CONF=0xC0; //set up BDM in 11
	FTM1->FMS=0x00; //clear the WPEN so that WPDIS is set in FTM0_MOD reg

	FTM1->MODE|=0x05; //enable write the FTM CnV register
	FTM1->MOD=80*10-1;
	FTM1->CONTROLS[0].CnSC=0x28; //edge-alignment,	PWM initial state is High, becomes low
	FTM1->CONTROLS[0].CnV=80*3;
	FTM1->CONTROLS[1].CnSC=0x28; //edge-alignment,	PWM initial state is High, becomes low
	FTM1->CONTROLS[1].CnV=80*3;
	FTM1->CNTIN=0x00;
	//FTM1->SC=0x08; //PWM edge_alignment, system clock driving, dividing by 1

	//Lets sync' em;
	//setting the GTBEEN bit for both FTM0/FTM1, FTM0/FTM1 waiting the GTB signal for
	FTM0->CONF|=FTM_CONF_GTBEEN_MASK;
	FTM1->CONF|=FTM_CONF_GTBEEN_MASK;

	//enable both FTM0 and FTM1
	FTM0->SC=0x08;
	FTM1->SC=0x08;

	FTM0->CONF|=FTM_CONF_GTBEOUT_MASK;

	SIM->SOPT8|=SIM_SOPT8_FTM0OCH0SRC_MASK|SIM_SOPT8_FTM0OCH1SRC_MASK;
}
