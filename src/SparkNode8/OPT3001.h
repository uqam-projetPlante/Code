/*
 * OPT3001.h
 *
 * Created: 2017-05-10 11:20:34
 *  Author: collerette_a
 */ 


#ifndef OPT3001_H_
#define OPT3001_H_

#define OPT3001_DATA_LENGTH 3
#define OPT3001_SLAVE_ADDR 0x44

//Registers
#define OPT3001_TIMEOUT 1000
#define OPT3001_RESULT 0x00
#define OPT3001_CONFIG 0x01
#define OPT3001_LOWLIM 0x02
#define OPT3001_HIGHLIM 0x03
#define OPT3001_MANID 0x7e
#define OPT3001_DEVID 0x7f

//functions
void OPT3001_init(void);
uint8_t* OPT3001_read(void);
void OPT3001_write(uint8_t reg, uint8_t byte1, uint8_t byte2);



#endif /* OPT3001_H_ */