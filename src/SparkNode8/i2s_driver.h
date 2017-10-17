/*
* _i2s_driver.h
*
* Created: 2017-05-19 17:14:53
*  Author: collerette_a
*/


#ifndef I2S_DRIVER_H_
#define I2S_DRIVER_H_

//#include <samd21j18a.h>
#include "mux.h"

#undef ENABLE // Clash between ASF and plain SAM

/*- Definitions -------------------------------------------------------------*/
#define I2S_GCLK_ID				GCLK_CLKCTRL_ID_I2S_0_Val
#define I2S_CLK_GEN				4	//24.576 Mhz
#define I2S_APBCMASK			PM_APBCMASK_I2S

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------


static inline void i2s_init(void)
{
	pin_set_peripheral_function(PINMUX_PA07G_I2S_SD0);
	pin_set_peripheral_function(PINMUX_PA08G_I2S_SD1);
	//pin_set_peripheral_function(PINMUX_PA09G_I2S_MCK0);
	pin_set_peripheral_function(PINMUX_PA10G_I2S_SCK0);
	pin_set_peripheral_function(PINMUX_PA11G_I2S_FS0);
	
	PM->APBCMASK.reg |= I2S_APBCMASK;
	/*
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(I2S_GCLK_ID) |
	GCLK_CLKCTRL_CLKEN |
	GCLK_CLKCTRL_GEN(I2S_CLK_GEN);*/

/*
	I2S->CLKCTRL[0].reg = I2S_CLKCTRL_SLOTSIZE(0x03) | // 32 bits slot size 
	I2S_CLKCTRL_NBSLOTS(0x01) |  // 2 slots (1 for each channel)
	I2S_CLKCTRL_BITDELAY |		 // 1 bit delay (i2s mode)
	I2S_CLKCTRL_FSWIDTH(0x01) |  // HALF frame
	I2S_CLKCTRL_MCKDIV(0x07);  //Sclk = Fs*nbslot*nbbits = 48khz*2*32768hz = 3.072 MHz = 24.576Mhz/8 = 24.576Mhz/(MCKDIC+1)
	*/
	I2S->CLKCTRL[0].reg = I2S_CLKCTRL_SLOTSIZE(0x03) | // 32 bits slot size 
	I2S_CLKCTRL_NBSLOTS(0x01) | // 2 slots (1 for each channel)
	//I2S->CLKCTRL[0].reg = I2S_CLKCTRL_SLOTSIZE(0x01) | // 16 bits slot size 
	//I2S_CLKCTRL_NBSLOTS(0x05) | // 6 slots (1 for each channel)
	I2S_CLKCTRL_BITDELAY |		 // 1 bit delay (i2s mode)
	I2S_CLKCTRL_FSSEL_FSPIN | 
	I2S_CLKCTRL_FSWIDTH(0x01) |  // HALF frame
	I2S_CLKCTRL_SCKSEL_SCKPIN;
	
	//I2S->SERCTRL[0].reg = I2S_SERCTRL_DATASIZE(0x00) | // data size configured at 32-bit (using 16 MSBs only)
	I2S->SERCTRL[0].reg = I2S_SERCTRL_DATASIZE(0x05) | // data size configured at 16-bit
	//I2S_SERCTRL_SLOTDIS1 | I2S_SERCTRL_SLOTDIS2 | I2S_SERCTRL_SLOTDIS4 | I2S_SERCTRL_SLOTDIS5 |
	I2S_SERCTRL_SLOTADJ_LEFT | // Left adjusted
	I2S_SERCTRL_WORDADJ_RIGHT | // Left adjusted
	I2S_SERCTRL_SERMODE(0x01);	 // transmit mode
	
	//I2S->SERCTRL[1].reg = I2S_SERCTRL_DATASIZE(0x00) | // data size configured at 32-bit (using 16 MSBs only)
	I2S->SERCTRL[1].reg = I2S_SERCTRL_DATASIZE(0x05) | // data size configured at 16-bit
	//I2S_SERCTRL_SLOTDIS1 | I2S_SERCTRL_SLOTDIS2 | I2S_SERCTRL_SLOTDIS4 | I2S_SERCTRL_SLOTDIS5 |
	I2S_SERCTRL_SLOTADJ_LEFT | // Left adjusted
	I2S_SERCTRL_WORDADJ_RIGHT | // Left adjusted
	I2S_SERCTRL_SERMODE(0x00);	 // receive mode	
	
	I2S->INTENSET.bit.TXRDY0 = 1; //enable tx ready interrupt
	//I2S->INTENSET.bit.RXRDY1 = 1; //enable rx ready interrupt

	NVIC_DisableIRQ(I2S_IRQn);
	
	I2S->CTRLA.reg = I2S_CTRLA_SEREN0 |
	I2S_CTRLA_SEREN1 |
	I2S_CTRLA_CKEN0 |
	I2S_CTRLA_ENABLE;
	
	//while (I2S->SYNCBUSY.reg);
	
	// Start I2S
	NVIC_ClearPendingIRQ(I2S_IRQn); // Global Enable interrupt on Sercom1
	NVIC_EnableIRQ(I2S_IRQn);
}

inline void i2s_write(uint32_t sample)
{
	
	while(I2S->INTFLAG.bit.TXRDY0 == 0);
	I2S->DATA[0].reg = sample;
	//while (I2S->SYNCBUSY.bit.DATA0);
}

inline uint32_t i2s_read(void)
{
	uint32_t sample;
	while(I2S->INTFLAG.bit.RXRDY1 == 0);
	sample = I2S->DATA[1].reg;
	//while (I2S->SYNCBUSY.bit.DATA1);
	return sample;
}

#endif /* 12S_DRIVER_H_ */