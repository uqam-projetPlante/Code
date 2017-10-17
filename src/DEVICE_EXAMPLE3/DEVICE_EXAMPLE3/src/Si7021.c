/*
 * Si7021.c
 *
 * Created: 2017-05-10 16:03:26
 *  Author: collerette_a
 */ 


#include <asf.h>
#include "Si7021.h"
#include "i2c_driver.h"
#include "delay.h"

static uint8_t Si7021_write_buffer[SI7021_DATA_LENGTH] ;
static uint8_t Si7021_read_buffer[SI7021_DATA_LENGTH];
uint8_t* Si7021_data;
uint16_t temp_data;
int temp1;
uint16_t RH_data;
int RH1;


int Si7021_GetTemp(){
	
	Si7021_command(SI7021_MEAS_T_HMH);
	timerDelay(10);
	Si7021_data = Si7021_read();
	temp_data = (Si7021_data[0]<<8) | Si7021_data[1];
	temp1 = ((175.72*temp_data)/65535)-46.85;	
	
	return temp1;
}


int Si7021_GetRH(){
	
	Si7021_command(SI7021_MEAS_RH_HMH);
	timerDelay(10);
	Si7021_data = Si7021_read();
	RH_data = (Si7021_data[0]<<8) | Si7021_data[1];
	RH1 = ((175.72*RH_data)/65535)-46.85;
	
	return RH1;
}

void Si7021_init(){
	
	
}

uint8_t* Si7021_read(){
	
	i2c_read(Si7021_read_buffer,2,SI7021_SLAVE_ADDR);
	return Si7021_read_buffer;
}

void Si7021_writereg(uint8_t reg, uint8_t byte1){
	
}

void Si7021_command(uint8_t command){
	
	Si7021_write_buffer[0] = command;	
	i2c_write(Si7021_write_buffer,1,SI7021_SLAVE_ADDR);

}

