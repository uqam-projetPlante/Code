#include "sam.h"
#include "usart_samd.h"
#include "../usb.h"

#define USART_BAUD_RATE 115200
#define USART_SAMPLE_NUM 16
#define SHIFT 32

void SERCOM3_Handler() {
	if (SERCOM3->USART.INTFLAG.bit.RXC)
		usart_input_handler();
	if (SERCOM3->USART.INTFLAG.bit.DRE)
		usart_output_handler();
}

static inline void pin_set_peripheral_function(uint32_t pinmux) {
	uint8_t port = (uint8_t)((pinmux >> 16)/32);
	PORT->Group[port].PINCFG[((pinmux >> 16) - (port*32))].bit.PMUXEN = 1;
	PORT->Group[port].PMUX[((pinmux >> 16) - (port*32))/2].reg &= ~(0xF << (4 * ((pinmux >> 16) & 0x01u)));
	PORT->Group[port].PMUX[((pinmux >> 16) - (port*32))/2].reg |= (uint8_t)((pinmux & 0x0000FFFF) << (4 * ((pinmux >> 16) & 0x01u)));
}

static uint64_t long_division(uint64_t n, uint64_t d) {
	uint64_t q = 0, r = 0, bit_shift;
	for (int32_t i = 63; i >= 0; i--) {
		bit_shift = (uint64_t)1 << i;
		r = r << 1;
		if (n & bit_shift)
			r |= 0x01;
		if (r >= d) {
			r = r - d;
			q |= bit_shift;
		}
	}
	return q;
}

uint16_t calculate_baud_value(const uint32_t baudrate, const uint32_t peripheral_clock, uint8_t sample_num) {
	uint64_t ratio = 0, scale = 0, baud_calculated = 0;
	uint64_t temp1 = ((sample_num * (uint64_t)baudrate) << SHIFT);
	ratio = long_division(temp1, peripheral_clock);
	scale = ((uint64_t)1 << SHIFT) - ratio;
	baud_calculated = (65536 * scale) >> SHIFT;
	return baud_calculated;
}

void system_gclk_chan_enable(const uint8_t channel) {
	/* Select the requested generator channel */
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;
	/* Enable the generic clock */
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;
}

void system_gclk_chan_set_config(const uint8_t channel, uint32_t src) {
	/* Cache the new config to reduce sync requirements */
	uint32_t new_clkctrl_config = (channel << GCLK_CLKCTRL_ID_Pos);
	/* Select the desired generic clock generator */
	new_clkctrl_config |= src << GCLK_CLKCTRL_GEN_Pos;
	/* Disable generic clock channel */
	/* Select the requested generator channel */
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;
	/* Switch to known-working source so that the channel can be disabled */
	uint32_t prev_gen_id = GCLK->CLKCTRL.bit.GEN;
	GCLK->CLKCTRL.bit.GEN = 0;
	/* Disable the generic clock */
	GCLK->CLKCTRL.reg &= ~GCLK_CLKCTRL_CLKEN;
	while (GCLK->CLKCTRL.reg & GCLK_CLKCTRL_CLKEN);
	/* Restore previous configured clock generator */
	GCLK->CLKCTRL.bit.GEN = prev_gen_id;
	/* Write the new configuration */
	GCLK->CLKCTRL.reg = new_clkctrl_config;
}

void edbg_usart_clock_init(void) {
	uint32_t src = 0, gclk_index = SERCOM3_GCLK_ID_CORE;
	/* Turn on module in PM */
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM3;
	/* Turn on Generic clock for USART */
//	system_gclk_chan_get_config_defaults(src);
	/* Default is generator 0. Other wise need to configure like below */
	/* gclk_chan_conf.source_generator = GCLK_GENERATOR_1; */
	system_gclk_chan_set_config(gclk_index, src);
	system_gclk_chan_enable(gclk_index);
}

void edbg_usart_pin_init(void) {
	/* PA22 and PA23 set into peripheral function C */
	pin_set_peripheral_function(PINMUX_PA22C_SERCOM3_PAD0);
	pin_set_peripheral_function(PINMUX_PA23C_SERCOM3_PAD1);
}

void edbg_usart_init(void) {
	uint16_t baud_value = calculate_baud_value(USART_BAUD_RATE, 48000000, USART_SAMPLE_NUM);
	/* By setting the DORD bit LSB is transmitted first and setting the RXPO bit as 1 corresponding
	SERCOM PAD[1] will be used for data reception, PAD[0] will be used as TxD pin by
	setting TXPO bit as 0,16x over-sampling is selected by setting the SAMPR bit as 0,
	Generic clock is enabled in all sleep modes by setting RUNSTDBY bit as 1,
	USART clock mode is selected as USART with internal clock by setting MODE bit into 1.
	*/
	SERCOM3->USART.CTRLA.reg =	SERCOM_USART_CTRLA_DORD |
								SERCOM_USART_CTRLA_RXPO(0x1) |
								SERCOM_USART_CTRLA_TXPO(0x0) |
								SERCOM_USART_CTRLA_SAMPR(0x0)|
								SERCOM_USART_CTRLA_RUNSTDBY |
								SERCOM_USART_CTRLA_MODE_USART_INT_CLK;

	/* baud register value corresponds to the device communication baud rate */
	SERCOM3->USART.BAUD.reg = baud_value;
	/* 8-bits size is selected as character size by setting the bit CHSIZE as 0,
	TXEN bit and RXEN bits are set to enable the Transmitter and receiver */
	SERCOM3->USART.CTRLB.reg =	SERCOM_USART_CTRLB_CHSIZE(0x0) |
								SERCOM_USART_CTRLB_TXEN |
								SERCOM_USART_CTRLB_RXEN ;
	/* synchronization busy */
	while(SERCOM3->USART.SYNCBUSY.bit.CTRLB);
	/* SERCOM3 handler enabled */
	NVIC_EnableIRQ(SERCOM3_IRQn);
	/* receive complete interrupt set */
	SERCOM3->USART.INTENSET.reg = SERCOM_USART_INTFLAG_RXC;
	/* SERCOM3 peripheral enabled */
	SERCOM3->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
	/* synchronization busy */
	while(SERCOM3->USART.SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_ENABLE);
}

void usart_init(void) {
	edbg_usart_clock_init();
	edbg_usart_pin_init();
	edbg_usart_init();
}
