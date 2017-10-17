/*
 * spi_sercom1.h
 *
 * Created: 2017-05-18 10:56:54
 *  Author: etudiant
 */ 


#ifndef SPI_SERCOM1_H_
#define SPI_SERCOM1_H_

//#include <samd21j18a.h>
#include "DMA_driver.h"

#define USE_DMA

#undef ENABLE // Clash between ASF and plain SAM

inline void spiInit1(void)
{
	PM->APBCMASK.bit.SERCOM1_ = 1;
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID_SERCOM1_CORE;
	//GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID_SERCOM1_CORE | GCLK_CLKCTRL_GEN(2); // Clock from source 8 (96 MHz)
	while(GCLK->STATUS.bit.SYNCBUSY);
	
	const SERCOM_SPI_CTRLA_Type ctrla = {
		.bit.DORD = 0, // MSB first
		.bit.CPHA = 0, // Mode 0
		.bit.CPOL = 0,
		.bit.FORM = 0, // SPI frame
		.bit.DIPO = 1, // MISO on PAD[1]
		.bit.DOPO = 3, // MOSI on PAD[0], SCK on PAD[3], SS_ on PAD[1]
		.bit.MODE = 3  // Master
	};
	SERCOM1->SPI.CTRLA.reg = ctrla.reg;
	const SERCOM_SPI_CTRLB_Type ctrlb = {
		.bit.RXEN = 1,   // RX enabled
		.bit.MSSEN = 0,  // HW SS
		.bit.CHSIZE = 0 // 8-bit
	};
	SERCOM1->SPI.CTRLB.reg = ctrlb.reg;

	//SERCOM1->SPI.BAUD.reg =  0; // Rate is clock / 2
	SERCOM1->SPI.BAUD.reg = 1; // Rate = 48 MHz / 2(BAUD+1), maximum speed
	//SERCOM1->SPI.BAUD.reg = 3; // Rate = 96 MHz / 2(BAUD+1), maximum speed

	// Mux for SERCOM1 PA16,PA17,PA18,PA19
	const PORT_WRCONFIG_Type wrconfig = {
		.bit.WRPINCFG = 1,
		.bit.WRPMUX = 1,
		.bit.PMUX = MUX_PA16C_SERCOM1_PAD0,
		.bit.PMUXEN = 1,
		.bit.HWSEL = 1,
		.bit.PINMASK = (uint16_t)((PORT_PA16 | PORT_PA17 | PORT_PA19) >> 16)
	};
	PORT->Group[0].WRCONFIG.reg = wrconfig.reg;

	SERCOM1->SPI.CTRLA.bit.ENABLE = 1;
	while(SERCOM1->SPI.SYNCBUSY.bit.ENABLE);
	
	// Configure DMA?
#ifdef USE_DMA
	DMA_init();
#endif
	
	// Pull S_CS high
	REG_PORT_DIRSET1 = PORT_PB08;
	REG_PORT_OUTSET1 = PORT_PB08;
}

inline void spiStart1()
{
	REG_PORT_OUTCLR1 = PORT_PB08;
}

inline uint8_t spiSend1(uint8_t data)
{
	while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);
	SERCOM1->SPI.DATA.reg = data;
	while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
	return SERCOM1->SPI.DATA.reg;
}

inline void spiPush1(uint8_t data)
{
	SERCOM1->SPI.DATA.reg;
	while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);
	SERCOM1->SPI.DATA.reg = data;
	//while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
	
}

inline void spiPushMulti1(uint8_t *data, uint8_t numdata)
{
#ifdef USE_DMA
	while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);	
	DMA_start_transfer(data, numdata );
	DMA_wait_complete();	
		
	while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);	
	while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
	SERCOM1->SPI.DATA.reg;		
	SERCOM1->SPI.STATUS.bit.BUFOVF = 1; // Reset buffer overflow	
	SERCOM1->SPI.DATA.reg;
	SERCOM1->SPI.DATA.reg;
	
	
	//while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
	//SERCOM1->SPI.DATA.reg;	
	
#else
	while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);
	SERCOM1->SPI.DATA.reg = *data; data++;
	for(uint8_t i=1;i<numdata;i++)
	{		
		while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
		SERCOM1->SPI.DATA.reg;
		while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);		
		SERCOM1->SPI.DATA.reg = *data; data++;			
	}
	while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
	SERCOM1->SPI.DATA.reg;
#endif
	/*
	while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);
	SERCOM1->SPI.DATA.reg = *data; data++;
	for(uint8_t i=0;i<numdata;i++)
	{		
		while(SERCOM1->SPI.INTFLAG.bit.TXC == 0);
		SERCOM1->SPI.DATA.reg;
		SERCOM1->SPI.DATA.reg = *data; data++;
	}
	while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
	SERCOM1->SPI.DATA.reg;	*/
}

inline uint8_t spiPull1(void)
{
	SERCOM1->SPI.DATA.reg = 0x00;
	while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
	return SERCOM1->SPI.DATA.reg;
}

inline void spiPullMulti1(uint8_t *data, uint8_t numdata)
{
	while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);
	SERCOM1->SPI.DATA.reg = 0x00;
	for(uint8_t i=0;i<numdata-1;i++)
	{		
		while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
		SERCOM1->SPI.DATA.reg = 0x00;
		*data = SERCOM1->SPI.DATA.reg; data++;
	}
	while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
	*data = SERCOM1->SPI.DATA.reg;
}

inline void spiStop1()
{
	REG_PORT_OUTSET1 = PORT_PB08;
}


#endif /* SPI_SERCOM1_H_ */