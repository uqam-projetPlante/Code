/*
 * spi_sercom1.h
 *
 * Created: 2017-05-18 10:56:54
 *  Author: etudiant
 */ 


#ifndef SPI_SERCOM0_H_
#define SPI_SERCOM0_H_

//#include <samd21j18a.h>


#undef ENABLE // Clash between ASF and plain SAM

inline void spiInit0(void)
{
	PM->APBCMASK.bit.SERCOM0_ = 1;
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID_SERCOM0_CORE;
	while(GCLK->STATUS.bit.SYNCBUSY);
	const SERCOM_SPI_CTRLA_Type ctrla = {
		.bit.DORD = 0, // MSB first
		.bit.CPHA = 0, // Mode 0
		.bit.CPOL = 0,
		.bit.FORM = 0, // SPI frame
		.bit.DIPO = 1, // MISO on PAD[3]
		.bit.DOPO = 3, // MOSI on PAD[0], SCK on PAD[3], SS_ on PAD[1]
		.bit.MODE = 3  // Master
	};
	SERCOM0->SPI.CTRLA.reg = ctrla.reg;
	const SERCOM_SPI_CTRLB_Type ctrlb = {
		.bit.RXEN = 1,   // RX enabled
		.bit.MSSEN = 0,  // HW SS
		.bit.CHSIZE = 0 // 8-bit
	};
	SERCOM0->SPI.CTRLB.reg = ctrlb.reg;

	//SERCOM0->SPI.BAUD.reg = 0; // Rate is clock / 2
	SERCOM0->SPI.BAUD.reg = 1; // Rate = 48 MHz / 2(BAUD+1), maximum speed

	// Mux for SERCOM1 PA16,PA17,PA18,PA19
	const PORT_WRCONFIG_Type wrconfig = {
		.bit.WRPINCFG = 1,
		.bit.WRPMUX = 1,
		.bit.PMUX = MUX_PA08C_SERCOM0_PAD0,
		.bit.PMUXEN = 1,
		.bit.HWSEL = 0,
		.bit.PINMASK = (uint16_t)((PORT_PA08 | PORT_PA09 | PORT_PA11))
	};
	PORT->Group[0].WRCONFIG.reg = wrconfig.reg;

	SERCOM0->SPI.CTRLA.bit.ENABLE = 1;
	while(SERCOM0->SPI.SYNCBUSY.bit.ENABLE);
	
	// Pull S_CS high
	REG_PORT_DIRSET0 = PORT_PA10;
	REG_PORT_OUTSET0 = PORT_PA10;
}

inline void spiStart0()
{
	REG_PORT_OUTCLR0 = PORT_PA10;
}

inline uint8_t spiSend0(uint8_t data)
{
	while(SERCOM0->SPI.INTFLAG.bit.DRE == 0);
	SERCOM0->SPI.DATA.reg = data;
	while(SERCOM0->SPI.INTFLAG.bit.RXC == 0);
	return SERCOM0->SPI.DATA.reg;
}

inline void spiPush0(uint8_t data)
{
	SERCOM0->SPI.DATA.reg;
	while(SERCOM0->SPI.INTFLAG.bit.DRE == 0);
	SERCOM0->SPI.DATA.reg = data;
	//while(SERCOM0->SPI.INTFLAG.bit.RXC == 0);
	
}

inline void spiPushMulti0(uint8_t *data, uint8_t numdata)
{
	while(SERCOM0->SPI.INTFLAG.bit.DRE == 0);
	SERCOM0->SPI.DATA.reg = *data; data++;
	for(uint8_t i=1;i<numdata;i++)
	{		
		while(SERCOM0->SPI.INTFLAG.bit.RXC == 0);
		SERCOM0->SPI.DATA.reg;
		while(SERCOM0->SPI.INTFLAG.bit.DRE == 0);		
		SERCOM0->SPI.DATA.reg = *data; data++;			
	}
	while(SERCOM0->SPI.INTFLAG.bit.RXC == 0);
	SERCOM0->SPI.DATA.reg;
	/*
	while(SERCOM0->SPI.INTFLAG.bit.DRE == 0);
	SERCOM0->SPI.DATA.reg = *data; data++;
	for(uint8_t i=0;i<numdata;i++)
	{		
		while(SERCOM0->SPI.INTFLAG.bit.TXC == 0);
		SERCOM0->SPI.DATA.reg;
		SERCOM0->SPI.DATA.reg = *data; data++;
	}
	while(SERCOM0->SPI.INTFLAG.bit.RXC == 0);
	SERCOM0->SPI.DATA.reg;	*/
}

inline uint8_t spiPull0(void)
{
	SERCOM0->SPI.DATA.reg = 0x00;
	while(SERCOM0->SPI.INTFLAG.bit.RXC == 0);
	return SERCOM0->SPI.DATA.reg;
}

inline void spiPullMulti0(uint8_t *data, uint8_t numdata)
{
	while(SERCOM0->SPI.INTFLAG.bit.DRE == 0);
	SERCOM0->SPI.DATA.reg = 0x00;
	for(uint8_t i=0;i<numdata-1;i++)
	{		
		while(SERCOM0->SPI.INTFLAG.bit.RXC == 0);
		SERCOM0->SPI.DATA.reg = 0x00;
		*data = SERCOM0->SPI.DATA.reg; data++;
	}
	while(SERCOM0->SPI.INTFLAG.bit.RXC == 0);
	*data = SERCOM0->SPI.DATA.reg;
}

inline void spiStop0()
{
	REG_PORT_OUTSET0 = PORT_PA10;
}


#endif /* SPI_SERCOM0_H_ */