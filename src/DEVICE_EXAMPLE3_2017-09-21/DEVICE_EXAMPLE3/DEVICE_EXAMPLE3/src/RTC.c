/*
 * RTC.c
 *
 * Created: 2017-09-15 15:44:40
 *  Author: collerette_a
 */ 
#include <asf.h>
#include "RTC.h"

void RTC_init(void)
{
	/* Turn on the digital interface clock */
	PM->APBAMASK.reg |= PM_APBAMASK_RTC;
	
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(RTC_GCLK_ID) |
	GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(4);
	
	RTC->MODE2.CTRL.reg = RTC_MODE2_CTRL_MODE(2) | RTC_CALENDAR_PRESCALER_DIV_1;//
	
	/* Set continuously mode. */
	RTC->MODE2.READREQ.reg |= RTC_READREQ_RCONT;
	
	RTC->MODE2.INTENSET.reg = RTC_MODE2_INTFLAG_ALARM0;
	
	/* Enable RTC module. */
	RTC->MODE2.CTRL.reg |= RTC_MODE2_CTRL_MODE(2) | RTC_CALENDAR_PRESCALER_DIV_1;
	RTC->MODE2.CTRL.reg |= RTC_MODE2_CTRL_ENABLE;
	
	NVIC_ClearPendingIRQ(RTC_IRQn); 
	NVIC_SetPriority(RTC_IRQn, 100);
	NVIC_EnableIRQ(RTC_IRQn);
}

void rtc_calendar_get_time(struct rtc_calendar_time *time1)
{
	//struct rtc_calendar_time mytime = time1;
	/* Change of read method based on value of continuously_update value in
	 * the configuration structure. */
	if (!(time1->continuously_update)) {
		/* Request read on CLOCK register. */
		RTC->MODE2.READREQ.reg = RTC_READREQ_RREQ;

		while (RTC->MODE2.STATUS.reg & RTC_STATUS_SYNCBUSY) {
			/* Wait for synchronization */
		}
	} else if (!(RTC->MODE2.READREQ.reg & RTC_READREQ_RCONT)){
		RTC->MODE2.READREQ.reg |= RTC_READREQ_RCONT | RTC_READREQ_RREQ;
		 /* wait that the first Read request finishes */
		while (RTC->MODE2.STATUS.reg & RTC_STATUS_SYNCBUSY) {
			/* Wait for synchronization */
		}
	}

	/* Read value. */
	uint32_t register_value = RTC->MODE2.CLOCK.reg;

	/* Convert value to time structure. */
	rtc_calendar_register_value_to_time(register_value, time1);
}

uint32_t rtc_calendar_time_to_register_value(struct rtc_calendar_time *time1)
{
	/* Initialize return value. */
	uint32_t register_value;

	/* Set year value into register_value minus initial year. */
	register_value = (time1->year - time1->year_init_value) <<
	RTC_MODE2_CLOCK_YEAR_Pos;

	/* Set month value into register_value. */
	register_value |= (time1->month << RTC_MODE2_CLOCK_MONTH_Pos);

	/* Set day value into register_value. */
	register_value |= (time1->day << RTC_MODE2_CLOCK_DAY_Pos);

	/* Set 24 hour value into register_value. */
	register_value |= (time1->hour << RTC_MODE2_CLOCK_HOUR_Pos);

	/* Set minute value into register_value. */
	register_value |= (time1->minute << RTC_MODE2_CLOCK_MINUTE_Pos);

	/* Set second value into register_value. */
	register_value |= (time1->second << RTC_MODE2_CLOCK_SECOND_Pos);
	

	return register_value;
}

void rtc_calendar_register_value_to_time(uint32_t register_value, struct rtc_calendar_time *time1)
{
	/* Set year plus value of initial year. */
	time1->year = ((register_value & RTC_MODE2_CLOCK_YEAR_Msk) >>
	RTC_MODE2_CLOCK_YEAR_Pos) + time1->year_init_value;

	/* Set month value into time struct. */
	time1->month = ((register_value & RTC_MODE2_CLOCK_MONTH_Msk) >>
	RTC_MODE2_CLOCK_MONTH_Pos);

	/* Set day value into time struct. */
	time1->day = ((register_value & RTC_MODE2_CLOCK_DAY_Msk) >>
	RTC_MODE2_CLOCK_DAY_Pos);

	time1->hour = ((register_value & RTC_MODE2_CLOCK_HOUR_Msk) >>
	RTC_MODE2_CLOCK_HOUR_Pos);

	/* Set minute value into time struct. */
	time1->minute = ((register_value & RTC_MODE2_CLOCK_MINUTE_Msk) >>
	RTC_MODE2_CLOCK_MINUTE_Pos);

	/* Set second value into time struct. */
	time1->second = ((register_value & RTC_MODE2_CLOCK_SECOND_Msk) >>
	RTC_MODE2_CLOCK_SECOND_Pos);	
}

void rtc_calendar_set_time (struct rtc_calendar_time *time1)
{
	uint32_t register_value = rtc_calendar_time_to_register_value(time1);

	while (RTC->MODE2.STATUS.reg & RTC_STATUS_SYNCBUSY) {
		/* Wait for synchronization */
	}

	/* Write value to register. */
	RTC->MODE2.CLOCK.reg = register_value;
}

void rtc_calendar_set_alarm(struct rtc_calendar_time *time1)
{
	while (RTC->MODE2.STATUS.reg & RTC_STATUS_SYNCBUSY) {
		/* Wait for synchronization */
	}

	/* Get register_value from time. */
	uint32_t register_value = rtc_calendar_time_to_register_value(time1);

	while (RTC->MODE2.STATUS.reg & RTC_STATUS_SYNCBUSY) {
		/* Wait for synchronization */
	}

	/* Set alarm value. */
	RTC->MODE2.Mode2Alarm[0].ALARM.reg = register_value;

	/* Set alarm mask */
	RTC->MODE2.Mode2Alarm[0].MASK.reg = RTC_MODE2_MASK_SEL_HHMMSS;

	return STATUS_OK;
}