/*
 * testbench.c
 *
 * Created: 2017-05-17 17:04:12
 *  Author: collerette_a
 */ 
#include <asf.h>
#include "conf_usb.h"
#include "ui.h"
#include "OPT3001.h"
#include "Si7021.h"
#include "MMA8491Q.h"
#include "WM8731.h"
#include "flash.h"
#include "testbench.h"
#include "spi_sercom1.h"
#include "delay.h"
#include "RTC.h"
#include "LCD.h"
#include "usart.h"
#include <stdio.h>
#include <math.h>

uint8_t uart_data_tx1 = 0x07;
uint8_t uart_data_rx1;
uint8_t string1[] = "Bluetooth!\r\n";
//uint8_t string1[] = {0x07,0x08,0x0a,0x02};

static volatile bool main_b_cdc_enable = false;
struct rtc_calendar_time mytime;
uint8_t mysecond;

//CDC RX
static uint8_t rx_data;

//sensor data
uint8_t* OPT3001_data;
uint8_t* Si7021_data;
uint8_t* MMA8491Q_data;
int temp1;
int RH1;
uint8_t	iCols = 20;
uint8_t	iRows = 4;
char str[];
uint8_t bright;
uint8_t contrast;

uint8_t Codec_data;

int timer = 0;

//flash buffer
uint8_t page_buffer[NVMCTRL_PAGE_SIZE];
enum status_code error_code;
uint32_t NVM_i = 0;
int n;

void printNum(uint8_t in, uint8_t Col, uint8_t Row);

void test_components(void){			
	
			/* Data ready to be sent */
			if (udi_cdc_is_rx_ready()) {
				rx_data = udi_cdc_getc();
			}
		switch(rx_data){
			case 'l' :
				OPT3001_data = OPT3001_read();
				udi_cdc_putc(OPT3001_data[0] + '0');
				break;
			case 't' ://print temperature
				temp1 =  Si7021_GetTemp();
				i2cLcdprintNum(temp1,0,26);
				break;
			case 'h' ://print Humidity
				RH1 = Si7021_GetRH();
				i2cLcdprintNum(RH1,0,36);
			break;
			case 'r' :
				do
				{
					error_code = nvm_read_buffer(
					500 * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE,
					page_buffer, NVMCTRL_PAGE_SIZE);
				} while (error_code == STATUS_BUSY);
				
				for (uint32_t i = 0; i < NVMCTRL_PAGE_SIZE; i++) {
					udi_cdc_putc(page_buffer[i] + '0');
				}
				break;
			case 'w' :
				for (uint32_t i = 0; i < NVMCTRL_PAGE_SIZE; i++) {
					page_buffer[i] = i;
				}
				do
				{
					error_code = nvm_erase_row(
					500 * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE);
				} while (error_code == STATUS_BUSY);
				do
				{
					error_code = nvm_write_buffer(
					500 * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE,
					page_buffer, NVMCTRL_PAGE_SIZE);
				} while (error_code == STATUS_BUSY);
				break;
			case 'e' :
				do
				{
					error_code = nvm_erase_row(
					500 * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE);
				} while (error_code == STATUS_BUSY);
				break;	
			case 'P':
				REG_PORT_OUTSET0 = PORT_PA27;
				break;
			case 'T':
				rtc_calendar_get_time(&mytime);
				i2cLcdprintNum(mytime.hour,0,4);
				i2cLcdprintNum(mytime.minute,0,7);
				i2cLcdprintNum(mytime.second, 0, 10);
				if(mytime.pm)
				{
					str[0]='A';
					str[1]='M';
					i2cLcdPrint(0,14,str);
				}
				else
				{
					str[0]='P';
					str[1]='M';
					i2cLcdPrint(0,14,str);					
				}			
				break;
			case 'c':
				uart_sendBuffer(string1, sizeof(string1));
				while(uart_data_rx1 != 'f'){
					uart_data_rx1 = uart_read();	
				}
				i2cLcdPrint(0,14,&uart_data_rx1);
				//uart_read(&uart_data_rx1);
				//uart_send(&uart_data_tx1);
				//uart_data_tx1 ++;
				//uart_send(uart_data_tx1);
				//uart_data_tx1 ++;
				//uart_send(uart_data_tx1);
				//uart_data_tx1 ++;				
				
			break;				
		}
			
			rx_data = 0;
}

