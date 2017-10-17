/*
* OPT3001.c
*
* Created: 2017-05-10 11:21:00
*  Author: collerette_a
*/

#include <asf.h>
#include "OPT3001.h"
#include "i2c_driver.h"

static uint8_t OPT3001_write_buffer[OPT3001_DATA_LENGTH] ;
static uint8_t OPT3001_read_buffer[OPT3001_DATA_LENGTH];

void OPT3001_init(){
	
	//Set config register
	OPT3001_write(OPT3001_CONFIG,0xc4,0x10);
	//Write to result register
	OPT3001_write(OPT3001_RESULT,0x00,0x00);
}


//Reads the register that was last written to
uint8_t* OPT3001_read(){
	
	i2c_read(OPT3001_read_buffer, 3, OPT3001_SLAVE_ADDR);
	return OPT3001_read_buffer;
}

void OPT3001_write(uint8_t reg, uint8_t byte1, uint8_t byte2){
	
	OPT3001_write_buffer[0] = reg;
	OPT3001_write_buffer[1] = byte1;
	OPT3001_write_buffer[2] = byte2;
	
	i2c_write(OPT3001_write_buffer, 3, OPT3001_SLAVE_ADDR);

}