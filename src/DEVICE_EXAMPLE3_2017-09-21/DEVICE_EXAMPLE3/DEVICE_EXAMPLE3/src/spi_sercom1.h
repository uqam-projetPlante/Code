/*
 * spi_sercom1.h
 *
 * Created: 2017-05-18 10:56:54
 *  Author: etudiant
 */ 


#ifndef SPI_SERCOM1_H_
#define SPI_SERCOM1_H_

#include <samd21j18a.h>
#include <asf.h>//Actually we only need core_cm0plus.h 


#undef ENABLE // Clash between ASF and plain SAM

static inline void spiInit1(void)
{
	PM->APBCMASK.bit.SERCOM1_ = 1;
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID_SERCOM1_CORE;
	while(GCLK->STATUS.bit.SYNCBUSY);
	const SERCOM_SPI_CTRLA_Type ctrla = {
		.bit.DORD = 0, // MSB first
		.bit.CPHA = 0, // Mode 0
		.bit.CPOL = 0, // SCK idle low
		.bit.FORM = 0, // SPI frame
		.bit.DIPO = 2, // MOSI on PAD[2]
		.bit.DOPO = 3, // MISO on PAD[0], SCK on PAD[3], SS_ on PAD[1]
		.bit.MODE = 2  // Slave
	};
	SERCOM1->SPI.CTRLA.reg = ctrla.reg;
	
	const SERCOM_SPI_CTRLB_Type ctrlb = {
		.bit.MSSEN = 1,  // HW SS
		.bit.CHSIZE = 0, // 8-bit
		.bit.SSDE = 1 // Enable ss low detect
	};
	SERCOM1->SPI.CTRLB.reg = ctrlb.reg;

	//SERCOM1->SPI.BAUD.reg = 0; // Rate is clock / 2
	SERCOM1->SPI.BAUD.reg = 3; // Rate = 48 MHz / 2(BAUD+1)

	// Mux for SERCOM1 PA16,PA17,PA18,PA19
	const PORT_WRCONFIG_Type wrconfig = {
		.bit.WRPINCFG = 1,
		.bit.WRPMUX = 1,
		.bit.PMUX = MUX_PA16C_SERCOM1_PAD0,
		.bit.PMUXEN = 1,
		.bit.HWSEL = 1,
		.bit.PINMASK = (uint16_t)((PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19) >> 16)
	};
	PORT->Group[0].WRCONFIG.reg = wrconfig.reg;
	
	SERCOM1->SPI.CTRLB.bit.RXEN = 1;// RX enabled
	while(SERCOM1->SPI.SYNCBUSY.bit.ENABLE);

	SERCOM1->SPI.CTRLA.bit.ENABLE = 1;
	while(SERCOM1->SPI.SYNCBUSY.bit.ENABLE);
	
	//SERCOM1->SPI.INTENSET.bit.SSL= 1;// Enable SS low detect interrupt
	
	//system_interrupt_enable(SERCOM1_IRQn);//enable sercom1 interrupt
	
	//irq_initialize_vectors();
	NVIC_ClearPendingIRQ(SERCOM1_IRQn); // Global Enable interrupt on Sercom1
	NVIC_SetPriority(SERCOM1_IRQn, 100);               
	NVIC_EnableIRQ(SERCOM1_IRQn);		
}

inline uint8_t spiSend1(uint8_t data)
{
	while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);
	SERCOM1->SPI.DATA.reg = data;
	while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
	return SERCOM1->SPI.DATA.reg;
	return 0;
}


#endif /* SPI_SERCOM1_H_ */