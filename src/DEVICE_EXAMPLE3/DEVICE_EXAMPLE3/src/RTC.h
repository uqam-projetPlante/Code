/*
 * RTC.h
 *
 * Created: 2017-09-15 14:38:50
 *  Author: collerette_a
 */ 


#ifndef RTC_H_
#define RTC_H_

#undef ENABLE // Clash between ASF and plain SAM

struct rtc_calendar_time {
	/** Second value */
	uint8_t  second;
	/** Minute value */
	uint8_t  minute;
	/** Hour value */
	uint8_t  hour;
	/** PM/AM value, \c true for PM, or \c false for AM */
	bool     pm;
	/** Day value, where day 1 is the first day of the month */
	uint8_t  day;
	/** Month value, where month 1 is January */
	uint8_t  month;
	/** Year value */
	uint16_t year;
	/** Year init value */
	uint16_t year_init_value;
	bool clock_24h;
	bool continuously_update;
};

enum rtc_calendar_prescaler {
	/** RTC input clock frequency is prescaled by a factor of 1 */
	RTC_CALENDAR_PRESCALER_DIV_1    = RTC_MODE2_CTRL_PRESCALER_DIV1,
	/** RTC input clock frequency is prescaled by a factor of 2 */
	RTC_CALENDAR_PRESCALER_DIV_2    = RTC_MODE2_CTRL_PRESCALER_DIV2,
	/** RTC input clock frequency is prescaled by a factor of 4 */
	RTC_CALENDAR_PRESCALER_DIV_4    = RTC_MODE2_CTRL_PRESCALER_DIV4,
	/** RTC input clock frequency is prescaled by a factor of 8 */
	RTC_CALENDAR_PRESCALER_DIV_8    = RTC_MODE2_CTRL_PRESCALER_DIV8,
	/** RTC input clock frequency is prescaled by a factor of 16 */
	RTC_CALENDAR_PRESCALER_DIV_16   = RTC_MODE2_CTRL_PRESCALER_DIV16,
	/** RTC input clock frequency is prescaled by a factor of 32 */
	RTC_CALENDAR_PRESCALER_DIV_32   = RTC_MODE2_CTRL_PRESCALER_DIV32,
	/** RTC input clock frequency is prescaled by a factor of 64 */
	RTC_CALENDAR_PRESCALER_DIV_64   = RTC_MODE2_CTRL_PRESCALER_DIV64,
	/** RTC input clock frequency is prescaled by a factor of 128 */
	RTC_CALENDAR_PRESCALER_DIV_128  = RTC_MODE2_CTRL_PRESCALER_DIV128,
	/** RTC input clock frequency is prescaled by a factor of 256 */
	RTC_CALENDAR_PRESCALER_DIV_256  = RTC_MODE2_CTRL_PRESCALER_DIV256,
	/** RTC input clock frequency is prescaled by a factor of 512 */
	RTC_CALENDAR_PRESCALER_DIV_512  = RTC_MODE2_CTRL_PRESCALER_DIV512,
	/** RTC input clock frequency is prescaled by a factor of 1024 */
	RTC_CALENDAR_PRESCALER_DIV_1024 = RTC_MODE2_CTRL_PRESCALER_DIV1024,
};


void RTC_init(void);
uint32_t rtc_calendar_time_to_register_value(struct rtc_calendar_time *time1);
void rtc_calendar_get_time(struct rtc_calendar_time *time1);
void rtc_calendar_register_value_to_time(uint32_t reg_val, struct rtc_calendar_time *time1);
void test666(struct rtc_calendar_time *time1);
void rtc_calendar_set_alarm(struct rtc_calendar_time *time1);
void rtc_calendar_set_time (struct rtc_calendar_time *time1);




#endif /* RTC_H_ */