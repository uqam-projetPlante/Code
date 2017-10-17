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


#include <asf.h>
#include "MMA8491Q.h"
#include "i2c_driver.h"

static uint8_t MMA8491Q_write_buffer[MMA8491Q_DATA_LENGTH] ;
static uint8_t MMA8491Q_read_buffer[MMA8491Q_DATA_LENGTH];

void MMA8491Q_enable(){

	port_pin_set_output_level(EXT1_PIN_8, true);
	//PORT->Group[1].OUTSET[PIN_PB03].bit = 1;
}

void MMA8491Q_disable(){

	port_pin_set_output_level(EXT1_PIN_8, false);
	//PORT->Group[1].OUTSET[PIN_PB03].bit = 0;
}

void MMA8491Q_init(){
	//configure enable pin
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(EXT1_PIN_8, &pin_conf);
	
	//const PORT_WRCONFIG_Type wrconfig = {
		//.bit.WRPINCFG = 1,
		//.bit.WRPMUX = 1,
		//.bit.PMUX = MUX_PA16C_SERCOM1_PAD0,
		//.bit.PMUXEN = 1,
		//.bit.HWSEL = 1,
		//.bit.PINMASK = (uint16_t)((PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19) >> 16)
	//};
	//PORT->Group[0].WRCONFIG.reg = wrconfig.reg;	
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

