#include "common/hw.h"
//#include "samd/usart_samd.h"
#include "usb.h"
#include "hardware.h"
#include "application.h"

#define GCLK_SYSTEM 0
#define GCLK_32K    5

uint8_t std_buffer[1024];

/*
void handleUSB(void)
{
	uint16_t size = usb_bytes_available();
	if(size > 0)
	{
		usb_get(std_buffer, size);
		usb_put(std_buffer, size);
	}	
}*/

int main(void)
{
	// Setup clocks
    clock_init_crystal(GCLK_SYSTEM, GCLK_32K);
	
	// Setup board
	boardSetup();	
	
	// Enable USB pins
	PORT->Group[0].PINCFG[24].bit.PMUXEN = 1;
	PORT->Group[0].PINCFG[25].bit.PMUXEN = 1;
	PORT->Group[0].PMUX[12].reg = 0x66;

	usb_init();
	usb_setup_fifos();
	usb_attach();	
	
	// Startup radio
	appInit();	

	while (true)
		handleUSB();
}
