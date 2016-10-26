/*
 * flexpwm.c
 *
 *  Created on: Jul 27, 2016
 *      Author: mmh
 */

#include <stdio.h>
#include "MK22F51212.h"
#include "mcu_tracer.h"
#include "taskcall.h"
#include "flexpwm.h"

taskcall_t task_softstat_terminate={"ssterm",0,NULL,&FTM_softstart_terminate,NULL,NULL};


uint8_t FTM_insoftstart=0;

//Set up protection functions
//required write enable.
void FTM_protect_init(){
	//Clock to the comparator
	SIM->SCGC4|=SIM_SCGC4_CMP_MASK;
	SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;

	// Init Over current protection
	CMP1->CR1=CMP_CR1_EN_MASK; // Enable the comparator
	CMP1->CR0=CMP_CR0_FILTER_CNT(7); //Set filter to react after 7 consequent errors.
	//Enable Dac, VIn1, Voltage 40/60*Vref
	CMP1->DACCR=CMP_DACCR_DACEN(1)|CMP_DACCR_VRSEL(0)|CMP_DACCR_VOSEL(48);
	CMP1->MUXCR=CMP_MUXCR_PSEL(1)|CMP_MUXCR_MSEL(7); //+=Sense -=DAC

	//Init OV-Protection#
	//config port
	 PORTC->PCR[8]= PORT_PCR_MUX(0);

	CMP0->CR1=CMP_CR1_EN_MASK; // Enable the comparator
	CMP0->CR0=CMP_CR0_FILTER_CNT(15);//7
	//Enable Dac, VIn1, Voltage 40/60*Vref

	//Overvoltage Level Calculation:
	//(0.5+Value)*17V
	//800V: 47
	CMP0->DACCR=CMP_DACCR_DACEN(1)|CMP_DACCR_VRSEL(0)|CMP_DACCR_VOSEL(28);


	//select input source (CMP0_In2)
	CMP0->MUXCR=CMP_MUXCR_PSEL(2)|CMP_MUXCR_MSEL(7); //+=Sense -=DAC

	//Select comparator 1 as fault source
	SIM->SOPT4|=(SIM_SOPT4_FTM0FLT1_MASK)|(SIM_SOPT4_FTM0FLT0_MASK);

	//Enables fault mode
	FTM0->MODE|=FTM_MODE_FAULTM(2); // faults on all channels, manual disable
	FTM0->FLTCTRL=FTM_FLTCTRL_FAULT0EN_MASK|FTM_FLTCTRL_FAULT1EN_MASK;
	FTM0->COMBINE|=FTM_COMBINE_FAULTEN0_MASK|FTM_COMBINE_FAULTEN1_MASK|FTM_COMBINE_FAULTEN2_MASK|FTM_COMBINE_FAULTEN3_MASK;
	FTM0->FMS&=(~FTM_FMS_FAULTF_MASK);

	//Masks channels so that they're inactive.
	FTM0->OUTMASK|=(FTM_OUTMASK_CH0OM_MASK|FTM_OUTMASK_CH1OM_MASK);
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
		char buf[50];
		sprintf(buf,"Overvoltage: %iV - PWM off.",(int)global_adc_dclink);
		mcu_tracer_msg(buf);
	}else{
		mcu_tracer_msg("OverX: Unknown error. Fix me!");
	}
}

void FTM_clear_error(void){
	//Clears overcurrent error
	uint32_t fms=FTM0->FMS;
	if(!(fms & FTM_FMS_FAULTIN_MASK)){
		FTM_softstart();
		fms&=(~FTM_FMS_FAULTF_MASK);
		FTM0->FMS=fms;
		//Enable Interrupts
		FTM_fault_interrupt_enable();
		//disable output mask.
		FTM0->OUTMASK&=~(FTM_OUTMASK_CH0OM_MASK|FTM_OUTMASK_CH1OM_MASK);
		mcu_tracer_msg("OverX: Errors reseted. Softstarting.");

	}else{
		if(FTM0->FMS & FTM_FMS_FAULTF1_MASK){
			mcu_tracer_msg("OverX: Error still persistend. Cannot reset. Overcurrent");
		}else if(FTM0->FMS & FTM_FMS_FAULTF0_MASK){
			mcu_tracer_msg("OverX: Error still persistend. Cannot reset. Overvoltage");
		}else{
			mcu_tracer_msg("OverX: Reset - Unknown error. Fix me!");
		}
	}
}

//Disables PWM to save position.
void FTM_mask(void){
	//Masks channels so that they are inactive.
	FTM0->OUTMASK|=(FTM_OUTMASK_CH0OM_MASK|FTM_OUTMASK_CH1OM_MASK);
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
	FTM0->COMBINE|=FTM_COMBINE_SYNCEN0_MASK;
	FTM0->CONTROLS[0].CnV=40;
	FTM0->CONTROLS[1].CnV=40;
	//FTM0_C2SC=0x28;
	//FTM0_C3SC=0x28;
	//FTM0->COMBINE|=0x0200;
	//FTM0->COMBINE|=0x1000;
	FTM0->DEADTIME=10;
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

	//Sync FTM0
	FTM0->SYNC|= FTM_SYNC_CNTMIN_MASK | FTM_SYNC_SWSYNC_MASK ;
	// The enhanced PWM synchronization mode is recommended for motor control and power conversion applications ==> SYNCONF[SYNCMODE] = 1
	// If the SWRSTCNT bit is set, the FTM counter restarts with FTM_CNTIN register value and the FTM_MOD and FTM_CnV registers are updated immediately.
	//   If the SWRSTCNT bit is cleared, the FTM counter continues to count normally and the FTM_MOD and FTM_CnV register update at the next loading point.
	// SWWRBUF The software trigger activates MOD, CNTIN, and CV registers synchronization.
	FTM0->SYNCONF|= FTM_SYNCONF_SYNCMODE_MASK |  FTM_SYNCONF_SWWRBUF_MASK | FTM_SYNCONF_SWRSTCNT_MASK ;

	// PWM Synchronization mode : no restrictions
	FTM0->MODE &= ~FTM_MODE_PWMSYNC_MASK ;

	//Sync options FTM1
	FTM1->SYNC|= FTM_SYNC_CNTMIN_MASK | FTM_SYNC_SWSYNC_MASK;
	FTM1->SYNCONF|= FTM_SYNCONF_SYNCMODE_MASK |  FTM_SYNCONF_SWWRBUF_MASK | FTM_SYNCONF_SWRSTCNT_MASK ;
	FTM1->MODE &= ~FTM_MODE_PWMSYNC_MASK ;
	FTM1->COMBINE|=FTM_COMBINE_SYNCEN0_MASK;
	FTM_fault_interrupt_enable();
}


void FTM_setpwm(uint32_t ton, uint32_t tcy, uint32_t npulse, uint32_t nskip){
	if(FTM_insoftstart)return;
	if(tcy*nskip>65535){
		mcu_tracer_msg("SetPwm: Value too high.");
		return;
	}
	//Loading updated values is enabled.
	FTM0->PWMLOAD=0;
	FTM1->PWMLOAD=0;

	//  match value for the output modes ( CH0 and CH1 )
	FTM0_C1V=ton; //  match value for the output modes (CH0)
	FTM0_C0V=ton; //  match value for the output modes (CH1)
	FTM0->MOD=tcy-1;


	FTM1->MOD=tcy*nskip-1;
	FTM1_C1V=tcy*npulse;
	FTM1_C0V=tcy*npulse;

	FTM0->PWMLOAD= FTM_PWMLOAD_LDOK_MASK ;
	FTM1->PWMLOAD= FTM_PWMLOAD_LDOK_MASK ;
}

void FTM_deadtime(uint32_t deadtime){
	if(FTM_insoftstart)return;
	FTM0->DEADTIME=FTM_DEADTIME_DTVAL(deadtime);
}

void FTM_softstart(void){
	FTM_insoftstart=0;
	//Precharges C so that current limit does not hit.
	FTM_deadtime(63); //Set very long dead time
	FTM_setpwm(100,200,1,1); //10 additional cycles for softstarting
	_taskcall_task_register_time(&task_softstat_terminate,42000);
	FTM_insoftstart=1;
}

void FTM_softstart_terminate(void){
	FTM_insoftstart=0;
	FTM_setpwm(global_pwm_ton,global_pwm_tcy,global_pwm_tp,global_pwm_tpsc);
	FTM_deadtime(global_deadtime);
}
