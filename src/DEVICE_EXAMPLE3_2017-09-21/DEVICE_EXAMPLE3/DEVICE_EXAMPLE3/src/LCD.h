/*
 * LCD.h
 *
 * Created: 2017-09-17 18:58:46
 *  Author: collerette_a
 */ 


#ifndef LCD_H_
#define LCD_H_

#define LCD_DATA_LENGTH 40
#define LCD_SLAVE_ADDR 0x01

//Commands
#define LCD_MEAS_RH_HMH 0xe5
#define LCD_MEAS_RH_NHMH 0xf5
#define LCD_MEAS_T_HMH 0xe3
#define LCD_MEAS_T_NHMH 0xf3
#define LCD_READ_T_FROMRH 0xe0
#define LCD_RESET 0xfe
#define LCD_WRITE_RHT_REG1 0xe6
#define LCD_READ_RHT_REG1 0xe7
#define LCD_WRITE_HEAT_REG1 0x51
#define LCD_READ_HEAT_REG1 0x11

void LCD_init(void);
uint8_t* LCD_read(void);
void LCD_writereg(uint8_t reg, uint8_t byte1);
void LCD_command(uint8_t command);
void i2cLcdBegin(uint8_t iRows, uint8_t iCols);
void i2cLcdClear(void);
void i2cLcdPrint(uint8_t iRows, uint8_t iCols, char s[]);
void i2cLcdWrite(uint8_t iRows, uint8_t iCols, uint8_t len, char s[]);
void i2cSetBright(uint8_t val);
void i2cSetContrast(uint8_t val);
uint8_t i2cGetBright(void);
uint8_t i2cGetContrast(void);
uint8_t i2cSaveConfig(void);
uint8_t i2cLoadConfig(void);
void i2cLcdTemp(void);
void i2cLcdRH(void);
void i2cLcdprintNum(uint8_t in, uint8_t Col, uint8_t Row);
void i2cLcdTime();




#endif /* LCD_H_ */