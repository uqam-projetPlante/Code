/*
 * application.c
 *
 * Created: 2017-09-19 16:37:00
 *  Author: collerette_a
 */ 
#include <asf.h>
#include "conf_usb.h"
#include "ui.h"
#include "OPT3001.h"
#include "Si7021.h"
#include "MMA8491Q.h"
#include "clockout.h"
#include "spi_sercom1.h"
#include "flash.h"
#include "testbench.h"
#include "clockgen.h"
#include "i2c_driver.h"
#include "delay.h"
#include "RTC.h"
#include "LCD.h"
#include "application.h"
#include "Timer1.h"
#include "keypad.h"
#include "usart.h"

#include <stdio.h>
#include <math.h>

struct rtc_calendar_time mySchedule[NUM_SCHEDULED_TASKS];
struct rtc_calendar_time myTime;
int temp1;
int RH1;
char str[];
char str1[];
uint8_t currentTask;
char key;
uint8_t hour;
uint8_t minute;
uint8_t uart_data_rx;

struct schedule_cb;

typedef void (*schedule_cb_t)(const struct schedule_cb *evt);

struct schedule_cb
{
	schedule_cb_t cb;
};

static struct schedule_cb myTask1[NUM_SCHEDULED_TASKS];

void event_cb_register(schedule_cb_t cb, int index)
{
	myTask1[index].cb = cb;
}

static void TurnLightOn(const struct schedule_cb *evt)
{
	LED_On(LED_0_PIN);
}

static void TurnLightOff(const struct schedule_cb *evt)
{
	LED_Off(LED_0_PIN);
}

static void TurnLightOn1(const struct schedule_cb *evt)
{
	LED_On(LED_0_PIN);
}

static void TurnLightOff1(const struct schedule_cb *evt)
{
	LED_Off(LED_0_PIN);
}

uint8_t char2num(char in){
	switch(in)
	{
		case '1' :
			return 1;
		break;
		case '2' :
			return 2;
		break;
		case '3' :
			return 3;
		break;
		case '4' :
			return 4;
		break;
		case '5' :
			return 5;
		break;
		case '6' :
			return 6;
		break;
		case '7' :
			return 7;
		break;
		case '8' :
			return 8;
		break;
		case '9' :
			return 9;
		break;
		case '0' :
			return 0;
		break;			
	}
}


void Get_time(){
	uint8_t d1;
	uint8_t d2;
	uint8_t u1;
	uint8_t u2;
	i2cLcdTime();
	key = 'H';
	i2cLcdPrint(0,27,&key);
	i2cLcdPrint(0,28,&key);
	key = 'M';
	i2cLcdPrint(0,30,&key);
	i2cLcdPrint(0,31,&key);
	
	while(keypadScan() == 0xFF){}
	key = keypadScan();
	d1 = char2num(key);
	i2cLcdPrint(0,27,&key);
	timerDelay(200);	
	while(keypadScan() == 0xFF){}
	key = keypadScan();
	u1 = char2num(key);
	i2cLcdPrint(0,28,&key);
	timerDelay(200);
	hour = (10*d1)+u1;
	while(keypadScan() == 0xFF){}
	key = keypadScan();
	d2 = char2num(key);
	i2cLcdPrint(0,30,&key);
	timerDelay(200);
	while(keypadScan() == 0xFF){}
	key = keypadScan();
	u2 = char2num(key);
	i2cLcdPrint(0,31,&key);
	timerDelay(200);
	minute = (10*d2)+u2;
	
	i2cLcdClear();
}

void TC3_Handler(void)
{
	//Print Data to LCD screen
	if(TC3->COUNT16.INTFLAG.bit.MC0)
	{
		i2cLcdprintNum(temp1,0,26);
		i2cLcdprintNum(RH1,0,36);
		rtc_calendar_get_time(&myTime);
		i2cLcdprintNum(myTime.hour,0,7);
		i2cLcdprintNum(myTime.minute,0,10);
	}
	
	// Chek for bluetooth commands
	if(TC3->COUNT16.INTFLAG.bit.OVF)
	{
		uart_data_rx = uart_read();
		if(uart_data_rx == 'f')
		{
			//list of commands
			uart_data_rx = uart_read();
			switch(uart_data_rx){
				case '1' :
					uart_sendBuffer("Growbox V1.0\n", 14);
				break;
				case '2' :
					uart_sendBuffer("Temp: ", 6);
					sprintf(str1, "%d", temp1);
					uart_sendBuffer(str1, 2);
					uart_sendBuffer(" C\n", 3);
					uart_sendBuffer("RH: ", 4);
					sprintf(str1, "%d", RH1);
					uart_sendBuffer(str1, 2);
					uart_sendBuffer(" %\n", 3);
				break;
			}			
		}
	}
	
}

void RTC_Handler(void)
{
	//Execute task
	myTask1[currentTask].cb(&myTask1[currentTask]);
	
	//Set next alarm for task
	currentTask++;
	rtc_calendar_set_alarm(&(mySchedule[currentTask]));
	
	//if all task completed: go back to first task
	if (currentTask == NUM_SCHEDULED_TASKS)
	{
		currentTask = 0;	
		rtc_calendar_set_alarm(&(mySchedule[currentTask]));
	}
	
	//clear interupt alarm0 flag
	RTC->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM0;
}

void Schedule_config(){
	//Configure your schedule here, Use as many tasks as defined NUM_SCHEDULED_TASKS 
	mySchedule[0].hour = 8;
	mySchedule[0].minute = 0;
	event_cb_register(TurnLightOn,0);

	mySchedule[1].hour = 10;
	mySchedule[1].minute = 0;
	event_cb_register(TurnLightOff,1);
	
	mySchedule[2].hour = 13;
	mySchedule[2].minute = 0;
	event_cb_register(TurnLightOn1,2);

	mySchedule[3].hour = 16;
	mySchedule[3].minute = 0;
	event_cb_register(TurnLightOff1,3);
}

void configApp(){
	// initialize keypad
	keypadInit();
	//get current time
	Get_time();
	//initialize real time counter
	RTC_init();
	//set time
	myTime.hour      = hour;
	myTime.minute    = minute;
	rtc_calendar_set_time(&myTime);	
	
	//configure Schedule
	Schedule_config();
		
	//Find the first task to execute (next scheduled task scheduled within 1 hour of current time)
	currentTask = 255;
	for(uint8_t i = (myTime.hour + 1); i <= 24; i++)
	{
		for(uint8_t j = 0; j <=(NUM_SCHEDULED_TASKS-1); j++)
		{	
			if(mySchedule[j].hour == i)
			{
				currentTask = j;
				break;
			}
			if(currentTask!=255)
				break;
		}
	}	
	if(currentTask == 255)
	{
		for(uint8_t i = 0; i <= myTime.hour; i++)
		{
			for(uint8_t j = 0; j <=(NUM_SCHEDULED_TASKS-1); j++)
			{
				if(mySchedule[j].hour == i)
				{
					currentTask = j;
					break;
				}
				if(currentTask!=255)
				break;
			}
	}
		
	}	
	//set first task to execute
	rtc_calendar_set_alarm(&(mySchedule[currentTask]));	
	
	//initialize timer
	timerInit1();
	//Display temp:-- RH:--
	i2cLcdTemp();
	i2cLcdRH();	
}