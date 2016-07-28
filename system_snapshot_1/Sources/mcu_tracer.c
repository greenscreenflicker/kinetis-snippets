/*
 * mcu_tracer.c
 *
 *  Created on: Jun 20, 2016
 *      Author: mmh
 */

#include "mcu_tracer.h"
#include "uart1.h"
#include "string.h"


mcu_tracer_t monitorvars[10];
#define MONITOR_ELEMENTS (sizeof(monitorvars)/sizeof(mcu_tracer_t))

int global_checksum;
 uint8_t mcu_tracer_checksum;
 int32_t debug1, debug2, debugbefore;
 float debug3;


uint32_t tracer_red, tracer_green, tracer_blue;

uint32_t mainloop_iterations;


char sendbuf[1000];
uint16_t sendbuf_pos=0;

void mcu_tracer_config(){
	UART1_startup(115200);
	mcu_tracer_fill();
}

void mcu_tracer_fill(void){
   monitorvars[0].type=1;
   monitorvars[0].rw=0;
   monitorvars[0].data_l=&tracer_red;
   strcpy(monitorvars[0].varname,"red");
   monitorvars[0].data_lmin=0;
   monitorvars[0].data_lmax=1;

   monitorvars[1].type=1;
   monitorvars[1].rw=0;
   monitorvars[1].data_l=&tracer_green;
   strcpy(monitorvars[1].varname,"green");
   monitorvars[1].data_lmin=0;
   monitorvars[1].data_lmax=1;

   monitorvars[2].type=1;
   monitorvars[2].rw=0;
   monitorvars[2].data_l=&tracer_blue;
   strcpy(monitorvars[2].varname,"blue");
   monitorvars[2].data_lmin=0;
   monitorvars[2].data_lmax=1;



   monitorvars[3].type=1;
   monitorvars[3].rw=1;
   monitorvars[3].data_l=&mainloop_iterations;
   strcpy(monitorvars[3].varname,"Mainloop Iter");
   monitorvars[3].data_lmin=INT32_MIN;
   monitorvars[3].data_lmax=INT32_MAX;
 }

uint8_t rec_char(void){
      while(!(UART1_buffercontent()));
      uint8_t ch;
      ch=UART1_getch();
      mcu_tracer_checksum=ch^mcu_tracer_checksum;
      return ch;
}

uint8_t rec_checksum(void){
  while(!(UART1_buffercontent()));
  uint8_t ch;
  ch=UART1_getch();
  if(mcu_tracer_checksum==ch){
    return 1;
  }else{
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

//Updates the value in the register
void mcu_tracer_update(uint16_t addr, int32_t val){
  if(addr>MONITOR_ELEMENTS){
    //error, we do not have this addr
    return;
  }

  if(monitorvars[addr].type==1||monitorvars[addr].type==3){
    //Integer
    int32_t *toupdate;
    toupdate=monitorvars[addr].data_l; //getting mcu address of variable
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
