/*
 * Si7021.h
 *
 * Created: 2017-05-10 16:03:08
 *  Author: collerette_a
 */ 


#ifndef SI7021_H_
#define SI7021_H_


#define SI7021_DATA_LENGTH 2
#define SI7021_SLAVE_ADDR 0x40

//Commands
#define SI7021_MEAS_RH_HMH 0xe5
#define SI7021_MEAS_RH_NHMH 0xf5
#define SI7021_MEAS_T_HMH 0xe3
#define SI7021_MEAS_T_NHMH 0xf3
#define SI7021_READ_T_FROMRH 0xe0
#define SI7021_RESET 0xfe
#define SI7021_WRITE_RHT_REG1 0xe6
#define SI7021_READ_RHT_REG1 0xe7
#define SI7021_WRITE_HEAT_REG1 0x51
#define SI7021_READ_HEAT_REG1 0x11

void Si7021_init(void);
uint8_t* Si7021_read(void);
void Si7021_writereg(uint8_t reg, uint8_t byte1);
void Si7021_command(uint8_t command);
int Si7021_GetTemp(void);
int Si7021_GetRH(void);



#endif /* SI7021_H_ */