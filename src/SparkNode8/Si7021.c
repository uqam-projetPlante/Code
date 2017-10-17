/*
 * Si7021.c
 *
 * Created: 2017-05-10 16:03:26
 *  Author: collerette_a
 */ 

#include "hardware.h"
#include "Si7021.h"
#include "i2c_driver.h"


static uint8_t Si7021_write_buffer[SI7021_DATA_LENGTH] ;
static uint8_t Si7021_read_buffer[SI7021_DATA_LENGTH];

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

