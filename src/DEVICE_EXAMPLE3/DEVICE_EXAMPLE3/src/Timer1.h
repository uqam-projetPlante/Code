/*
 * Timer1.h
 *
 * Created: 2017-09-19 16:43:17
 *  Author: collerette_a
 */ 


#ifndef TIMER1_H_
#define TIMER1_H_

#undef ENABLE

static inline void timerInit1(void)
{
	PM->APBCMASK.reg |= PM_APBCMASK_TC3;

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(TC3_GCLK_ID) |
	GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(2); // Take from clock generator 2 (8 MHz)

	TC3->COUNT16.CTRLA.reg =
	TC_CTRLA_MODE(TC_CTRLA_MODE_COUNT16_Val) |
	TC_CTRLA_WAVEGEN(TC_CTRLA_WAVEGEN_MFRQ_Val) |
	TC_CTRLA_PRESCALER(TC_CTRLA_PRESCALER_DIV64_Val) |// Divide clock by 64
	TC_CTRLA_PRESCSYNC(TC_CTRLA_PRESCSYNC_PRESC_Val);
	TC3->COUNT16.COUNT.reg = 0;
	TC3->COUNT16.CC[0].reg = 2125u; // about 60 times per second
	TC3->COUNT16.INTENSET.bit.MC0 = 1;
	TC3->COUNT16.INTENSET.bit.OVF = 1;
	TC3->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
	
	NVIC_ClearPendingIRQ(TC3_IRQn); // Global Enable interrupt on Sercom1
	NVIC_SetPriority(TC3_IRQn, 100);
	NVIC_EnableIRQ(TC3_IRQn);
}




#endif /* TIMER1_H_ */