/*
 * MMA8491Q.h
 *
 * Created: 2017-05-11 14:29:07
 *  Author: collerette_a
 */ 


#ifndef MMA8491Q_H_
#define MMA8491Q_H_


#define MMA8491Q_DATA_LENGTH 2
#define MMA8491Q_SLAVE_ADDR 0x55

//registers
#define MMA8491Q_STATUS	   0x00
#define MMA8491Q_OUT_X_MSB 0x01
#define MMA8491Q_OUT_X_LSB 0x02
#define MMA8491Q_OUT_Y_MSB 0x03
#define MMA8491Q_OUT_Y_LSB 0x04
#define MMA8491Q_OUT_Z_MSB 0x05
#define MMA8491Q_OUT_Z_LSB 0x06

//Commands

void MMA8491Q_init(void);
void MMA8491Q_enable(void);
void MMA8491Q_disable(void);
uint8_t* MMA8491Q_read(void);
void MMA8491Q_writereg(uint8_t reg, uint8_t byte1);
void MMA8491Q_command(uint8_t command);


#endif /* MMA8491Q_H_ */