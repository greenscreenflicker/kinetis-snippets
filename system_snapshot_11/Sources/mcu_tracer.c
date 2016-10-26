/*
 * mcu_tracer.c
 *
 *  Created on: Jun 20, 2016
 *      Author: mmh
 */

#include <stdint.h>
#include "mcu_tracer.h"
#include "uart1.h"
#include "string.h"


mcu_tracer_t monitorvars[20];
#define MONITOR_ELEMENTS (sizeof(monitorvars)/sizeof(mcu_tracer_t))

int global_checksum;
 uint8_t mcu_tracer_checksum;
 int32_t debug1, debug2, debugbefore;
 float debug3;

uint8_t mcu_tracer_reject_data=0;



extern float global_regul_d;

uint32_t led_lauflicht=1;

uint32_t mainloop_iterations;

float testfloat;
float testfloat_original;
float testfloat_approx;

char sendbuf[1000];
uint16_t sendbuf_pos=0;

void mcu_tracer_config(){
	//UART1_startup(115200);
	UART1_startup(1000000);
	mcu_tracer_fill();
}

void mcu_tracer_fill(void){
	uint16_t fill=0;
/*
	monitorvars[fill].type=1;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_l=&tracer_red;
	strcpy(monitorvars[fill].varname,"red");
	monitorvars[fill].data_lmin=0;
	monitorvars[fill].data_lmax=1;
	fill++;
	monitorvars[fill].type=1;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_l=&tracer_green;
	strcpy(monitorvars[fill].varname,"green");
	monitorvars[fill].data_lmin=0;
	monitorvars[fill].data_lmax=1;
	fill++;
	monitorvars[fill].type=1;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_l=&tracer_blue;
	strcpy(monitorvars[fill].varname,"blue");
	monitorvars[fill].data_lmin=0;
	monitorvars[fill].data_lmax=1;
	fill++;
*/

	monitorvars[fill].type=1;
	monitorvars[fill].rw=1;
	monitorvars[fill].data_l=&mainloop_iterations;
	strcpy(monitorvars[fill].varname,"Mainloop Iter");
	monitorvars[fill].data_lmin=INT32_MIN;
	monitorvars[fill].data_lmax=INT32_MAX;
	fill++;

	monitorvars[fill].type=2;
	monitorvars[fill].rw=1;
	monitorvars[fill].data_f=&global_overcurrent;
	strcpy(monitorvars[fill].varname,"Overcurrent");
	monitorvars[fill].data_fmin=0;
	monitorvars[fill].data_fmax=100;
	fill++;

	monitorvars[fill].type=1;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_f=&global_clear_error;
	strcpy(monitorvars[fill].varname,"PWM Error Reset");
	monitorvars[fill].data_lmin=0;
	monitorvars[fill].data_lmax=1;
	fill++;


	monitorvars[fill].type=1;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_f=&global_pwm_ton;
	strcpy(monitorvars[fill].varname,"PWM ton");
	monitorvars[fill].data_lmin=0;
	monitorvars[fill].data_lmax=2000;
	fill++;

	monitorvars[fill].type=1;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_f=&global_pwm_tcy;
	strcpy(monitorvars[fill].varname,"PWM tcycle");
	monitorvars[fill].data_lmin=0;
	monitorvars[fill].data_lmax=2000;
	fill++;
/*
	monitorvars[fill].type=1;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_f=&global_pwm_tp;
	strcpy(monitorvars[fill].varname,"pulses");
	monitorvars[fill].data_lmin=0;
	monitorvars[fill].data_lmax=1000;
	fill++;

	monitorvars[fill].type=1;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_f=&global_pwm_tpsc;
	strcpy(monitorvars[fill].varname,"pulse cy");
	monitorvars[fill].data_lmin=0;
	monitorvars[fill].data_lmax=1000;
	fill++;
*/
	monitorvars[fill].type=MCU_TRACER_DATA_TYPE_FLOAT;
	monitorvars[fill].rw=1;
	monitorvars[fill].data_f=&global_adc_voltage;
	strcpy(monitorvars[fill].varname,"Usupply");
	monitorvars[fill].data_fmin=0;
	monitorvars[fill].data_fmax=100;
	fill++;

	monitorvars[fill].type=2;
	monitorvars[fill].rw=1;
	monitorvars[fill].data_f=&global_adc_grid;
	strcpy(monitorvars[fill].varname,"Ugrid");
	monitorvars[fill].data_fmin=-300;
	monitorvars[fill].data_fmax=300;
	fill++;

	monitorvars[fill].type=2;
	monitorvars[fill].rw=1;
	monitorvars[fill].data_f=&global_adc_dclink;
	strcpy(monitorvars[fill].varname,"Udclink");
	monitorvars[fill].data_fmin=0;
	monitorvars[fill].data_fmax=1000;
	fill++;

	monitorvars[fill].type=2;
	monitorvars[fill].rw=1;
	monitorvars[fill].data_f=&global_si8900_uled;
	strcpy(monitorvars[fill].varname,"Uled");
	monitorvars[fill].data_fmin=0;
	monitorvars[fill].data_fmax=1000;
	fill++;

	monitorvars[fill].type=2;
	monitorvars[fill].rw=1;
	monitorvars[fill].data_f=&global_si8900_iled;
	strcpy(monitorvars[fill].varname,"Iled");
	monitorvars[fill].data_fmin=0;
	monitorvars[fill].data_fmax=1000;
	fill++;

	monitorvars[fill].type=2;
	monitorvars[fill].rw=1;
	monitorvars[fill].data_f=&global_si8900_secsup;
	strcpy(monitorvars[fill].varname,"Usecsup");
	monitorvars[fill].data_fmin=0;
	monitorvars[fill].data_fmax=1000;
	fill++;

	monitorvars[fill].type=2;
	monitorvars[fill].rw=1;
	monitorvars[fill].data_f=&global_regul_d;
	strcpy(monitorvars[fill].varname,"Regul. d");
	monitorvars[fill].data_fmin=0;
	monitorvars[fill].data_fmax=1000;
	fill++;

	monitorvars[fill].type=1;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_f=&global_target_iled;
	strcpy(monitorvars[fill].varname,"Target I");
	monitorvars[fill].data_lmin=0;
	monitorvars[fill].data_lmax=10000;
	fill++;

	monitorvars[fill].type=1;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_f=&global_deadtime;
	strcpy(monitorvars[fill].varname,"Deadtime");
	monitorvars[fill].data_lmin=0;
	monitorvars[fill].data_lmax=63;
	fill++;

	monitorvars[fill].type=MCU_TRACER_DATA_TYPE_FLOAT;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_f=&testfloat;
	strcpy(monitorvars[fill].varname,"testfloat");
	monitorvars[fill].data_fmin=0.0f;
	monitorvars[fill].data_fmax=7.0f;
	fill++;

	monitorvars[fill].type=MCU_TRACER_DATA_TYPE_FLOAT;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_f=&testfloat_original;
	strcpy(monitorvars[fill].varname,"original");
	monitorvars[fill].data_fmin=0.0f;
	monitorvars[fill].data_fmax=63.0f;
	fill++;


	monitorvars[fill].type=MCU_TRACER_DATA_TYPE_FLOAT;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_f=&testfloat_approx;
	strcpy(monitorvars[fill].varname,"approx");
	monitorvars[fill].data_fmin=0.0f;
	monitorvars[fill].data_fmax=63.0f;
	fill++;

/*
	monitorvars[fill].type=1;
	monitorvars[fill].rw=0;
	monitorvars[fill].data_l=&led_lauflicht;
	strcpy(monitorvars[fill].varname,"LED lauflicht");
	monitorvars[fill].data_lmin=INT32_MIN;
	monitorvars[fill].data_lmax=INT32_MAX;
	fill++;*/

	mcu_tracer_reject_data=0;
 }

uint8_t rec_char(void){
	if(mcu_tracer_reject_data) return 0;
	int timeout=1000;
	while(!(UART1_buffercontent())){
		timeout--;
		if(timeout<1){
			mcu_tracer_reject_data=1;
			return 0;
		}
	}
	uint8_t ch;
	ch=UART1_getch();
	mcu_tracer_checksum=ch^mcu_tracer_checksum;
	return ch;
}

uint8_t rec_checksum(void){
	if(mcu_tracer_reject_data){
		UART1_rec_buf_reset();
		return 0;
	}
	while(!(UART1_buffercontent()));
	uint8_t ch;
	ch=UART1_getch();
	if(mcu_tracer_checksum==ch){
	  if(led_lauflicht==2) led_lauflicht=1;
	  return 1;
	}else{
	  //Let's empty buffer, and wait for a new correct string.
		return 0;
	}
}

void mcu_tracer_process(void){
  while(1){
    mcu_tracer_checksum=0;
    if(!(UART1_buffercontent())){
      //We have no data, we're done
      return;
    }
    uint8_t startbyte=rec_char();
    if(!(startbyte==MCU_TRACER_STARTBYTE)){
      //we have no startbyte
      mcu_tracer_checksum=0;
      //Serial.println("not a start byte");
      //Serial.print(startbyte,HEX);
      continue;
    }else{
      //Serial.println("Recieved startbyte");
    }
    //Now we synced to the startbyte

    uint8_t order=rec_char();
    //Serial.print("Order:");
    //Serial.print(order,HEX);
    //Serial.print("\n");
    if(order==1){
      //Serial.println("Order one");
      //we have a Init request
      //we need to check mcu_tracer_checksum now

      if(rec_checksum()){
        // call the init routine;
        //erial.println("Checksum ok");
        mcu_tracer_init();
      }
    }else if(order==0){
        if(rec_checksum()){
        	mcu_tracer_ping();
        }
     }

    else if(order==2){
      if(rec_checksum()){
        mcu_tracer_vals();
      }
    }else if(order==3){
      uint32_t data=0;
      uint16_t arraynumber=0;
      uint8_t rec;
      int i;
      //load arrynumber
      for(i=2;i>0;i--){
        arraynumber=(arraynumber<<8);
        rec=rec_char();
        arraynumber=(rec+arraynumber);
      }
      //load data
      for(i=4;i>0;i--){
        data=(data<<8);
        rec=rec_char();
        data=(rec+data);
      }
      if(rec_checksum()){
        mcu_tracer_update(arraynumber,data);
      }
    }else if(order=0xFF){
      if(rec_checksum()){
        //execute emergency function
        mcu_tracer_emergency();
        mcu_tracer_emergency_reply();
      }
    }else{
      mcu_tracer_msg("recieved unkown order");
    }
    mcu_tracer_flush_buffer();
    mcu_tracer_reject_data_check();
  }
}

void mcu_tracer_reject_data_check(void){
	if(mcu_tracer_reject_data){
		UART1_rec_buf_reset();
		mcu_tracer_reject_data=0;
	}
}
void mcu_tracer_write_serial(char data){
  global_checksum=global_checksum^data;
  UART1_dma_complete_wait();
  sendbuf[sendbuf_pos++]=data;
}

void mcu_tracer_flush_buffer(void){
	UART1_txBulk(sendbuf,sendbuf_pos);
	sendbuf_pos=0;
}

void mcu_tracer_write_string(const char* data){
   while( *data != '\0' ){
      mcu_tracer_write_serial(*data++ );
   }
   mcu_tracer_write_serial(1);
}

void mcu_tracer_send_checksum(void){
	UART1_dma_complete_wait();
	sendbuf[sendbuf_pos++]=global_checksum;
	global_checksum=0;
}

void mcu_tracer_init(void){
  int i;
  mcu_tracer_write_serial(MCU_TRACER_STARTBYTE);
  mcu_tracer_write_serial(1); //Order type 1: Sending init
  for(i=0; i<MONITOR_ELEMENTS; i++){
    //check if we have a terminator =>if, break
    if(monitorvars[i].type==0) break;
    mcu_tracer_write_serial(monitorvars[i].type);
    mcu_tracer_write_serial(monitorvars[i].rw);
    mcu_tracer_write_string(monitorvars[i].varname);
  }

  //sending terminating byte
  mcu_tracer_write_serial(0); //last byte
  mcu_tracer_write_serial(0); //read only
  mcu_tracer_write_string("");

  mcu_tracer_send_checksum();
}

//Sends actual values to pc
void mcu_tracer_vals(void){
  mcu_tracer_write_serial(MCU_TRACER_STARTBYTE);
  mcu_tracer_write_serial(2); //order two, we transfer variables
  char i;
  for(i=0; i<MONITOR_ELEMENTS; i++){
    //check if we have a terminator =>if, break
    if(monitorvars[i].type==0) break;
    uint32_t data;
    data=*(monitorvars[i].data_l);
    mcu_tracer_write_serial(data>>(3*8));
    mcu_tracer_write_serial(data>>(2*8));
    mcu_tracer_write_serial(data>>(1*8));
    mcu_tracer_write_serial(data>>(0*8));
  }
  mcu_tracer_send_checksum();
}

union dataconvert {
   int32_t i;
   float f;
};

//Updates the value in the register
void mcu_tracer_update(uint16_t addr, int32_t val){

  if(addr>MONITOR_ELEMENTS){
    //error, we do not have this addr
    return;
  }

  if(monitorvars[addr].type==1||monitorvars[addr].type==3){
    //Integer
    int32_t *toupdate;
    if(val>monitorvars[addr].data_lmax){
      val=monitorvars[addr].data_lmax;
    }
    if(val<monitorvars[addr].data_lmin){
      val=monitorvars[addr].data_lmin;
    }

    //memcpy(monitorvars[addr].data_l,&val,4);


    *(monitorvars[addr].data_l)=val;
     /*
    *(monitorvars[addr].data_l+1)=(uint8_t)(val<<8);
    *(monitorvars[addr].data_l+1)=(uint8_t)(val<<8);*/
  }else if(monitorvars[addr].type==2){
	float *toupdate;
	union dataconvert dataconv;
	dataconv.i=val;
	float val_f=dataconv.f;
	if((val_f)>monitorvars[addr].data_fmax){
	  val=monitorvars[addr].data_fmax;
	}
	if((val_f)<monitorvars[addr].data_fmin){
	  val=monitorvars[addr].data_fmin;
	}
	*(monitorvars[addr].data_f)=val_f;
  }else{
    mcu_tracer_msg("Data type not yet supported");
  }
  mcu_tracer_inform(addr);
}

void mcu_tracer_msg(const char* msg){
  mcu_tracer_write_serial(MCU_TRACER_STARTBYTE);
  mcu_tracer_write_serial(0xFE);
  mcu_tracer_write_string(msg);
  mcu_tracer_send_checksum();
}

void mcu_tracer_inform(uint16_t addr){
  if(addr>MONITOR_ELEMENTS) return; //we do not have this.
  int32_t val=*(monitorvars[addr].data_l);

  mcu_tracer_write_serial(MCU_TRACER_STARTBYTE);
  mcu_tracer_write_serial(3); //order 3, we transfer single value
  mcu_tracer_write_serial(addr>>(1*8));
  mcu_tracer_write_serial(addr>>(0*8));
  mcu_tracer_write_serial(val>>(3*8));
  mcu_tracer_write_serial(val>>(2*8));
  mcu_tracer_write_serial(val>>(1*8));
  mcu_tracer_write_serial(val>>(0*8));
  mcu_tracer_send_checksum();
}

void mcu_tracer_ping(void){
  mcu_tracer_write_serial(MCU_TRACER_STARTBYTE);
  mcu_tracer_write_serial(0);
  mcu_tracer_send_checksum();
}

void mcu_tracer_emergency_reply(void){
  mcu_tracer_write_serial(MCU_TRACER_STARTBYTE);
  mcu_tracer_write_serial(0xFF);
  mcu_tracer_send_checksum();
}

void mcu_tracer_emergency(void){
  mcu_tracer_msg("Place here your emergency code");
}
/*
char msg[40];
void loop() {
  // put your main code here, to run repeatedly:
  mcu_tracer_process();
  if(debug1==1){
    digitalWrite(13, HIGH);
  }else{
    digitalWrite(13, LOW);
  }
  if(debugbefore!=debug1){
    itoa(debug1,&msg[0],10);
    strcat(msg,"=Debug one");
    mcu_tracer_msg(msg);
  }
  debugbefore=debug1;
  debug2=analogRead(A0);
  debug3=debug2 * (5.0 / 1023.0);
}*/