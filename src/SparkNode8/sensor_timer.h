/*
 * sensor_timer.h
 *
 * Created: 2017-08-16 15:12:47
 *  Author: etudiant
 */ 


#ifndef SENSOR_TIMER_H_
#define SENSOR_TIMER_H_

#undef ENABLE

inline void sensor_timer_Init(void)
{
	PM->APBCMASK.reg |= PM_APBCMASK_TC3;

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(TC3_GCLK_ID) |
	GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(6); // Take from clock generator 6 (20.48 MHz)

	TC3->COUNT16.CTRLA.reg =
	TC_CTRLA_MODE(TC_CTRLA_MODE_COUNT16_Val) |
	TC_CTRLA_WAVEGEN(TC_CTRLA_WAVEGEN_MFRQ_Val) |
	TC_CTRLA_PRESCALER(TC_CTRLA_PRESCALER_DIV64_Val) |
	TC_CTRLA_PRESCSYNC(TC_CTRLA_PRESCSYNC_PRESC_Val); // Divide clock by 64
	TC3->COUNT16.COUNT.reg = 0;
	TC3->COUNT16.CC[0].reg = 32u; // Count to 8 to get 100us
	//TC3->COUNT16.INTENSET.bit.MC0 = 1;
	TC3->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;

	//NVIC_ClearPendingIRQ(TC3_IRQn); // Global Enable interrupt on Sercom1
	//NVIC_SetPriority(TC3_IRQn, 100);
	//NVIC_EnableIRQ(TC3_IRQn);
}

inline void sensor_timer_Delay(uint16_t ms)
{
	uint16_t cnt;
	
	//NVIC_DisableIRQ(USB_IRQn);
	
	TC3->COUNT16.COUNT.reg = 0; cnt = 0;
	
	while(cnt!=ms) // Count the ms till target
	{
		TC3->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0; // Clear interrupt flag
		while (0 == TC3->COUNT16.INTFLAG.bit.MC0); // Wait for interrupt
		cnt++;
	}
	
	//NVIC_EnableIRQ(USB_IRQn);
}



#endif /* SENSOR_TIMER_H_ */