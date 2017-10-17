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
#include "Keypad.h"
#include <stdio.h>
#include <math.h>

char key;
struct rtc_calendar_time mySchedule[NUM_SCHEDULED_TASKS];
struct rtc_calendar_time myTime;
int temp1;
int RH1;
char str[];
bool timeset = true;
uint8_t currentTask;

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

void Get_time(){
	key = keypadScan();
}

void TC3_Handler(void)
{	
	if(!timeset)
	{
			
	}
	else
	{
		//Print Data to LCD screen
		if(TC3->COUNT16.INTFLAG.bit.MC0)
		{
			i2cLcdprintNum(currentTask,0,26);
			i2cLcdprintNum(RH1,0,36);
			rtc_calendar_get_time(&myTime);
			i2cLcdprintNum(myTime.hour,0,6);
			i2cLcdprintNum(myTime.minute,0,9);
			//i2cLcdprintNum(mytime.second, 0, 13);
			if(myTime.pm)
			{
				str[0]='A';
				str[1]='M';
				i2cLcdPrint(0,12,str);
			}
			else
			{
				str[0]='P';
				str[1]='M';
				i2cLcdPrint(0,12,str);
			}
		}
	}
	
	// Get values from Sensors
	if(TC3->COUNT16.INTFLAG.bit.OVF)
	{
		if(!timeset)
			Get_time();
		else{
		temp1 =  Si7021_GetTemp();
		RH1 =    Si7021_GetRH();
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
	//Configure your schedule here, Use as many tasks as 
	mySchedule[0].hour = 8;
	mySchedule[0].minute = 0;
	mySchedule[0].pm = false;
	event_cb_register(TurnLightOn,0);

	mySchedule[1].hour = 5;
	mySchedule[1].minute = 0;
	mySchedule[1].pm = true;
	event_cb_register(TurnLightOff,1);
	
	mySchedule[2].hour = 8;
	mySchedule[2].minute = 0;
	mySchedule[2].pm = true;
	event_cb_register(TurnLightOn1,2);

	mySchedule[3].hour = 10;
	mySchedule[3].minute = 0;
	mySchedule[3].pm = true;
	event_cb_register(TurnLightOff1,3);
}

void configApp(){
		//keypadInit();
		while(!timeset)
		{
				
		}
		
		//temp:-- RH:--
		i2cLcdTemp();
		i2cLcdRH();
		LED_On(LED_0_PIN);
		//set time
		myTime.hour      = 9;
		myTime.minute    = 0;
		myTime.pm		= false;
		rtc_calendar_set_time(&myTime);	
		
		//configure Schedule
		Schedule_config();
		
		//Find the first task to execute (next scheduled task scheduled within 1 hour of current time)
		currentTask = 255;
		for(uint8_t i = (myTime.hour + 1); i <= 12; i++)
		{
			for(uint8_t j = 0; j <=(NUM_SCHEDULED_TASKS-1); j++)
			{	
				if((mySchedule[j].hour == i) && (mySchedule[j].pm == myTime.pm))
				{
					currentTask = j;
				}
			}
		}	
		if(currentTask == 255)
		{
			for(int i = 0; i < 12; i++)
			{
				for(uint8_t j = 0; j <=(NUM_SCHEDULED_TASKS-1); j++)
				{
					if((mySchedule[j].hour == i) && (mySchedule[j].pm != myTime.pm))
					{
						currentTask = j;
					}
				}
			}
		}
					
		rtc_calendar_set_alarm(&(mySchedule[currentTask]));	
}