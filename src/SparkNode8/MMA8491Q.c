/*
 * MMA8491Q.c
 *
 * Created: 2017-05-11 14:29:38
 *  Author: collerette_a
 */ 
/*
 * MMA8491Q.c
 *
 * Created: 2017-05-10 16:03:26
 *  Author: collerette_a
 */ 

#include "hardware.h"
#include "MMA8491Q.h"
#include "i2c_driver.h"

static uint8_t MMA8491Q_write_buffer[MMA8491Q_DATA_LENGTH] ;
static uint8_t MMA8491Q_read_buffer[MMA8491Q_DATA_LENGTH];

void MMA8491Q_enable(){
	REG_PORT_OUTSET1 = PORT_PB11;
}

void MMA8491Q_disable(){
	REG_PORT_OUTCLR1 = PORT_PB11;
}

void MMA8491Q_init(){
	REG_PORT_DIRSET1 = PORT_PB11;
	REG_PORT_OUTCLR1 = PORT_PB11;	
}

uint8_t* MMA8491Q_read(){
	
	i2c_read(MMA8491Q_read_buffer,2,MMA8491Q_SLAVE_ADDR);
	return MMA8491Q_read_buffer;
}

void MMA8491Q_writereg(uint8_t reg, uint8_t byte1){
	

}

void MMA8491Q_command(uint8_t command){
	
	MMA8491Q_write_buffer[0] = command;
	i2c_write(MMA8491Q_write_buffer,1,MMA8491Q_SLAVE_ADDR);
}

