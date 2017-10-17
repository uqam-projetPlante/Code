/*
 * LCD.c
 *
 * Created: 2017-09-17 18:59:01
 *  Author: collerette_a
 */ 
#include <asf.h>
#include "LCD.h"
#include "i2c_driver.h"
#include "delay.h"
#include <stdio.h>
#include <math.h>

static uint8_t LCD_write_buffer[LCD_DATA_LENGTH] ;
static uint8_t LCD_read_buffer[LCD_DATA_LENGTH];
char str1[40];
char str2[40];
char str3[40];
char str4[40];

void i2cLcdTime() {
	str4[0] = 'E';
	str4[1] = 'n';
	str4[2] = 't';
	str4[3] = 'e';
	str4[4] = 'r';
	str4[5] = ' ';
	str4[6] = 'T';
	str4[7] = 'i';
	str4[8] = 'm';
	str4[9] = 'e';
	i2cLcdPrint(0, 5, str4);
}

void i2cLcdTemp() {
	
	str1[0] = 'T';
	str1[1] = 'e';
	str1[2] = 'm';
	str1[3] = 'p';
	str1[4] = ':';
	str1[5] = ' ';
	str1[6] = ' ';
	str1[7] = ' ';
	str1[8] = ' ';
	str1[9] = 'C';
	i2cLcdPrint(0, 20, str1);
}

void i2cLcdRH() {
	str2[0] = 'R';
	str2[1] = 'H';
	str2[2] = ':';
	str2[3] = ' ';
	str2[4] = ' ';
	str2[5] = ' ';
	str2[6] = '%';
	i2cLcdPrint(0, 32, str2);	
}

void i2cLcdprintNum(uint8_t in, uint8_t Col, uint8_t Row)
{
	uint8_t d;
	uint8_t u;
	
	d = in / ((int) pow(10, 1)) % 10;
	u = in / ((int) pow(10, 0)) % 10;
	
	str3[0] = (d + '0');
	str3[1] = (u + '0');
	i2cLcdPrint(Col,Row,str3);
}

void i2cLcdBegin(uint8_t iRows, uint8_t iCols) {
	LCD_write_buffer[0] = 0x01;
	LCD_write_buffer[1] = iCols;
	LCD_write_buffer[2] = iRows;
	i2c_write(LCD_write_buffer,3,LCD_SLAVE_ADDR);
    timerDelay(5);
}
      
void i2cLcdClear() {
	LCD_write_buffer[0] = 0x02;
	i2c_write(LCD_write_buffer,1,LCD_SLAVE_ADDR);
	timerDelay(3);
    }
      
/*
void i2cLcdSetCursor(byte iCol, byte iRow) {
	Wire.beginTransmission(I2CLCD_ADDR);
	Wire.send(0x03);
	Wire.send(iCol);
	Wire.send(iRow);
	Wire.endTransmission();
}
*/
      
void i2cLcdPrint(uint8_t iRows, uint8_t iCols, char s[]) {
	LCD_write_buffer[0] = 0x04;
	LCD_write_buffer[1] = iCols;
	LCD_write_buffer[2] = iRows;
	int i = 0;
	char *p = s;
	while (*p) {
		i++;
		LCD_write_buffer[2+i] = *p++;
	}
	i2c_write(LCD_write_buffer,3+i,LCD_SLAVE_ADDR);
	timerDelay(1);
}
      
void i2cLcdWrite(uint8_t iRows, uint8_t iCols, uint8_t len, char s[]) {
	LCD_write_buffer[0] = 0x14;
	LCD_write_buffer[1] = iCols;
	LCD_write_buffer[2] = iRows;
    LCD_write_buffer[3] = len;
    for (uint8_t i = 0; i < len; i++)
		LCD_write_buffer[3+i] = s[i];
    i2c_write(LCD_write_buffer,(4+len),LCD_SLAVE_ADDR);
    timerDelay(3);
}
      //
      //void i2cLcdSetCustChar_P(byte slot, const byte *charDef) {
        //Wire.beginTransmission(i2cLCDAddr);
        //Wire.send(0x05);
        //Wire.send(slot);
        //for (byte i = 0; i < 8; i++) {
          //Wire.send(pgm_read_byte(charDef++));
        //}
        //Wire.endTransmission();
        //delay(5);
      //}
      //
      //void i2cLcdWriteCustChar(byte iCol, byte iRow, byte c) {
        //Wire.beginTransmission(i2cLCDAddr);
        //Wire.send(0x06);
        //Wire.send(iCol);
        //Wire.send(iRow);
        //Wire.send(c);
        //Wire.endTransmission();
        //delay(3);
      //}
      //
      void i2cSetBright(uint8_t val) {
		LCD_write_buffer[0] = 0x07;
		LCD_write_buffer[1] = val;
		i2c_write(LCD_write_buffer,2,LCD_SLAVE_ADDR);
        timerDelay(3);
      }
      
      void i2cSetContrast(uint8_t val) {
		LCD_write_buffer[0] = 0x08;
		LCD_write_buffer[1] = val;
		i2c_write(LCD_write_buffer,2,LCD_SLAVE_ADDR);
		timerDelay(3);
      }
      
      uint8_t i2cGetBright(void) {
		LCD_write_buffer[0] = 0x09;
		i2c_write(LCD_write_buffer,1,LCD_SLAVE_ADDR);
		i2c_read(LCD_read_buffer,1,LCD_SLAVE_ADDR);
			return *LCD_read_buffer;
      }
      
      uint8_t i2cGetContrast(void) {
		LCD_write_buffer[0] = 0x0A;
		i2c_write(LCD_write_buffer,1,LCD_SLAVE_ADDR);
		i2c_read(LCD_read_buffer,1,LCD_SLAVE_ADDR);
		return *LCD_read_buffer;
      }
      
      uint8_t i2cSaveConfig(void) {
		LCD_write_buffer[0] = 0x0B;
		i2c_write(LCD_write_buffer,1,LCD_SLAVE_ADDR);
        timerDelay(10);
      }
      
      uint8_t i2cLoadConfig(void) {
		LCD_write_buffer[0] = 0x0C;
		i2c_write(LCD_write_buffer,1,LCD_SLAVE_ADDR);
		timerDelay(10);
      }