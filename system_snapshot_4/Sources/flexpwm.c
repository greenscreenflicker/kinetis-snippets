/*
 * flexpwm.c
 *
 *  Created on: Jul 27, 2016
 *      Author: mmh
 */

#include "MK22F51212.h"
#include "mcu_tracer.h"

//Set up protection functions
//required write enable.
void FTM_protect_init(){
	//Clock to the comparator
	SIM->SCGC4|=SIM_SCGC4_CMP_MASK;
	SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;

	// Init Over current protection
	CMP1->CR1=CMP_CR1_EN_MASK; // Enable the comparator
	//Enable Dac, VIn1, Voltage 40/60*Vref
	CMP1->DACCR=CMP_DACCR_DACEN(1)|CMP_DACCR_VRSEL(0)|CMP_DACCR_VOSEL(40);
	CMP1->MUXCR=CMP_MUXCR_PSEL(1)|CMP_MUXCR_MSEL(7); //+=Sense -=DAC

	//Init OV-Protection#
	//config port
	 PORTC->PCR[8]= PORT_PCR_MUX(0);

	CMP0->CR1=CMP_CR1_EN_MASK; // Enable the comparator
	CMP0->CR0=CMP_CR0_FILTER_CNT(7);
	//Enable Dac, VIn1, Voltage 40/60*Vref

	//Overvoltage Level Calculation:
	//(0.5+Value)*17V
	//800V: 47
	CMP0->DACCR=CMP_DACCR_DACEN(1)|CMP_DACCR_VRSEL(0)|CMP_DACCR_VOSEL(10);


	//select input source (CMP0_In2)
	CMP0->MUXCR=CMP_MUXCR_PSEL(2)|CMP_MUXCR_MSEL(7); //+=Sense -=DAC

	//Select comparator 1 as fault source
	SIM->SOPT4|=(SIM_SOPT4_FTM0FLT1_MASK)|(SIM_SOPT4_FTM0FLT0_MASK);

	//Enables fault mode
	FTM0->MODE|=FTM_MODE_FAULTM(2); // faults on all channels, manual disable
	FTM0->FLTCTRL=FTM_FLTCTRL_FAULT0EN_MASK|FTM_FLTCTRL_FAULT1EN_MASK;
	FTM0->COMBINE|=FTM_COMBINE_FAULTEN0_MASK|FTM_COMBINE_FAULTEN1_MASK|FTM_COMBINE_FAULTEN2_MASK|FTM_COMBINE_FAULTEN3_MASK;
	FTM0->FMS&=(~FTM_FMS_FAULTF_MASK);
}

void FTM_fault_interrupt_enable(){
	//inits the fault interrupt function
	FTM0->MODE|=FTM_MODE_FAULTIE_MASK;
	NVIC_EnableIRQ(FTM0_IRQn);
}

void FTM0_IRQHandler(void){
	FTM0->MODE&=~FTM_MODE_FAULTIE_MASK;
	//CMP1 works on Overcurrent
	if(FTM0->FMS & FTM_FMS_FAULTF1_MASK){
		mcu_tracer_msg("Overcurrent detected. PWM off.");
	}else if(FTM0->FMS & FTM_FMS_FAULTF0_MASK){
		mcu_tracer_msg("Overvoltage detected. PWM off.");
	}else{
		mcu_tracer_msg("OverX: Unkown error. Fix me!");
	}
}

void FTM_clear_error(){
	//Clears overcurrent error
	uint32_t fms=FTM0->FMS;
	if(!(fms & FTM_FMS_FAULTIN_MASK)){
		fms&=(~FTM_FMS_FAULTF_MASK);
		FTM0->FMS=fms;
		//Enable Interrupts
		FTM_fault_interrupt_enable();
		mcu_tracer_msg("OverX: Errors reseted.");
	}else{
		mcu_tracer_msg("OverX: Error still persistend. Cannot reset.");
	}
}



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


   FTM_protect_init();



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
	FTM0->COMBINE|=0x02; //complementary mode for CH0&CH1 of FTM0
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

	FTM_fault_interrupt_enable();
}
