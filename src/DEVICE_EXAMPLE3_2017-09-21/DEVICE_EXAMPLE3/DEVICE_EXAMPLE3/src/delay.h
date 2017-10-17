/*
 * delay.h
 *
 * Created: 2017-05-18 14:56:55
 *  Author: etudiant
 */ 


#ifndef DELAY_H_
#define DELAY_H_

#undef ENABLE

inline void timerInit(void)
{
	PM->APBCMASK.reg |= PM_APBCMASK_TC4;

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(TC4_GCLK_ID) |
	GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(2); // Take from clock generator 2 (8 MHz)

	TC4->COUNT16.CTRLA.reg =
	TC_CTRLA_MODE(TC_CTRLA_MODE_COUNT16_Val) |
	TC_CTRLA_WAVEGEN(TC_CTRLA_WAVEGEN_MFRQ_Val) |
	TC_CTRLA_PRESCALER(TC_CTRLA_PRESCALER_DIV64_Val) |
	TC_CTRLA_PRESCSYNC(TC_CTRLA_PRESCSYNC_PRESC_Val); // Divide clock by 64
	TC4->COUNT16.COUNT.reg = 0;
	TC4->COUNT16.CC[0].reg = 800u; // Count to 800 to get 0.1ms
	TC4->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
}

inline void timerDelay(uint16_t ms)
{
	uint16_t cnt;
	
	TC4->COUNT16.COUNT.reg = 0; cnt = 0;
	
	while(cnt!=ms) // Count the ms till target
	{
		TC4->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0; // Clear interrupt flag
		while (0 == TC4->COUNT16.INTFLAG.bit.MC0); // Wait for interrupt
		cnt++;
	}	
}


#endif /* DELAY_H_ */