/*
 * regulation.c
 *
 *  Created on: Aug 22, 2016
 *      Author: mmh
 */

#include <math.h>
#include "mcu_tracer.h"

float global_regul_d;
uint32_t global_target_iled;


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
	float L=95e-6;
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
	global_pwm_tp=1;
	global_pwm_tpsc=1;
	global_pwm_ton=global_regul_d;
	global_pwm_tcy=2*global_pwm_ton;
	FTM_setpwm(global_pwm_ton,global_pwm_tcy,global_pwm_tp,global_pwm_tpsc);
}

