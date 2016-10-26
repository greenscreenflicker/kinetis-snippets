/*
 * regulation.c
 *
 *  Created on: Aug 22, 2016
 *      Author: mmh
 */

#include <math.h>
#include "mcu_tracer.h"
#include "MK22F51212.h"
#include "flexpwm.h"
#include "adc.h"
#include "adcdma1.h"

float global_regul_d;
float global_target_iled;

float regulator_current_ki=0;
float Ki_factor=90;
float pwm_duty_cycle=0.5;


float regul_calcd(float Ud, float Uout,float tc, float D){
	float C=100e-9;
	float L=100e-6;
	float d;
	float umrechnung=1e9;
	Uout=3.78f*(1.4f+Uout);
	//d=(sqrt(C*L)*atan(((Ud-Uout-Ud*D)*sin(tc/sqrt(C*L)))/((Ud-Uout+Ud*D)*cos(tc/sqrt(C*L))-Ud*D+Uout+Ud)))/tc;
	//d=(sqrt(C*L)*atan(((-Ud+Uout-Ud*D)*sin(tc/sqrt(C*L)))/((-Ud+Uout-Ud*D)*cos(tc/sqrt(C*L))-Ud*D-Uout-Ud)))/umrechnung;
	//d=(sqrt(C*L)*atan(((-Ud+Uout+Ud*D)*sin(tc/sqrt(C*L)))/((-Ud+Uout+Ud*D)*cos(tc/sqrt(C*L))+Ud*D-Uout-Ud)))/tc;
	d=(tc*(-Ud+Uout+Ud*D))/(2*Ud*(D-1));
	return d*umrechnung;
}

//Calculates the charge transfered
float regul_calC(float tc, float Imax){
	return (tc*Imax)/2;
}

float regul_estimateI(float Udc, float Uout, float tc){
	float L=100e-6;
	Uout=3.78f*(0.8f+Uout);
	return ((Udc/2-Uout)*tc)/(2*L);
}

float regul_caltc(float Udc, float Uout, float Itarget){
	float L=470e-6;
	float I=Itarget;
	float tc;
	Uout=3.78f*(Uout);
	tc=(2*I*L)/(Udc-2*Uout);
	//tc=tc+1.666666667e-7;
	return tc;
}

void regul_regulate(void){
	//does the regulation

	float Duty=global_pwm_ton;
	Duty=Duty/global_pwm_tcy;

	float umrechnung=(1.0f)/(60000000.0f);
/*
	global_regul_d=regul_calcd(global_adc_dclink,
			global_si8900_uled,
			(global_pwm_tcy/2)*umrechnung,
			1-Duty);
*/
	global_regul_d=regul_caltc(global_adc_dclink,
			global_si8900_uled,
			global_target_iled*0.001f)*60000000.0f;

	if(global_regul_d>600) global_regul_d=600;
	/*
	global_pwm_tp=1;
	global_pwm_tpsc=1;
	global_pwm_ton=global_regul_d;
	global_pwm_tcy=2*global_pwm_ton;
	*/
	FTM_setpwm(global_pwm_ton,global_pwm_tcy,global_pwm_tp,global_pwm_tpsc);
	FTM_deadtime(global_deadtime);
}

void regulator_convert_measurement(void){
	char buf[50];
	uint16_t adcvalue=ADC_read(3);

	double Rb=100.0;
	double Rt=1000.0;
	double Uref=1.2;
	double Bits=65535;

	float convert=((Rb+Rt)*Uref)/(Rb*Bits);
	float voltage;
	voltage=convert*adcvalue;
	global_adc_voltage=voltage;
	//sprintf(buf,"ADC: %f",1.234);
	//mcu_tracer_msg(buf);

	uint16_t grid_adc=adc_result[2];
	convert=(4000*Uref)/(5.5*Bits);
	global_adc_grid=(grid_adc-32768)*convert;

	Rb=2.2;
	Rt=2000;
	convert=((Rb+Rt)*Uref)/(Rb*Bits);
	float dclink=convert*adc_result[0];
	global_adc_dclink=dclink;







	//if(CMP1_SCR & CMP_SCR_COUT_MASK){
	if(CMP0->SCR & CMP_SCR_COUT_MASK){
		global_overcurrent=1;
	}else{
		global_overcurrent=0;
	}
	if(global_clear_error){
		FTM_clear_error();
		global_clear_error=0;
	}

	//SI8900_manual_req(0);
	Rb=39.0;
	Rt=1000.0;
	Uref=2.495;
	Bits=1024;
	convert=((Rb+Rt)*Uref)/(Rb*Bits);

	float convercur=(Uref)/(Bits);
	global_si8900_uled=SI8900_get_ch(0)*convert;
	global_si8900_iled=SI8900_get_ch(1)*convercur;

	Rb=150.0;
	convert=((Rb+Rt)*Uref)/(Rb*Bits);

	global_si8900_secsup=SI8900_get_ch(2)*convert;
	//Pwm updates

}


void regulator_current(float Itarget){
	float error=(Itarget-global_si8900_iled);
	float deff=pwm_duty_cycle;
	float dlimit;

	if(deff>1) deff=1.0f;
	if(deff<0) deff=0;
	if(deff>0.5) deff=1.0f-deff;
	dlimit=deff*3600.0f;

	regulator_current_ki+=error;
	if(regulator_current_ki>25) regulator_current_ki=25;
	if(regulator_current_ki<0) regulator_current_ki=0;
	global_regul_d=regulator_current_ki*Ki_factor;


	if(global_regul_d>dlimit) global_regul_d=dlimit;
	if(global_regul_d<10) global_regul_d=10;
	global_pwm_tp=1;
	global_pwm_tpsc=1;
	while((global_regul_d*global_pwm_tpsc)<100){
		global_pwm_tpsc=global_pwm_tpsc+1;
	}

	global_pwm_ton=pwm_duty_cycle*(global_regul_d*global_pwm_tpsc);
	global_pwm_tcy=(global_regul_d*global_pwm_tpsc);

	FTM_setpwm(global_pwm_ton,global_pwm_tcy,global_pwm_tp,global_pwm_tpsc);
	FTM_deadtime(global_deadtime);
}

void regulator(void){
	regulator_current(global_target_iled);
	GPIOB_PTOR=(1<<3);
}
uint32_t global_deadtime;
