/*
 * hardware.c
 *
 * Created: 2017-05-24 10:41:18
 *  Author: etudiant
 */ 

#include <stdio.h>
#include <stdarg.h>
#include "hardware.h"
#include "usb.h"

#include "i2c_driver.h"
#include "i2s_driver.h"
#include "sensor_timer.h"

//#define WD_ENABLE
#define PCB_V2

void callbackExtInterrupt(); // Define this callback in radio.c
uint32_t callbackAudioSample(uint32_t); // Define this callback in application.c
 
bool extIntEnable; 

void enableExtInt(void) {
	extIntEnable = true;
	NVIC_ClearPendingIRQ(EIC_IRQn);
	NVIC_EnableIRQ(EIC_IRQn);
	//EIC->CTRL.bit.ENABLE = 1; 
}

void disableExtInt(void){	
	extIntEnable = false;
	//EIC->CTRL.bit.ENABLE = 0; 
	NVIC_ClearPendingIRQ(EIC_IRQn);
	NVIC_DisableIRQ(EIC_IRQn);
	//EIC->INTFLAG.bit.EXTINT5 = 1;
}

void enableClockGate(void)
{
	clockEnable();
}

bool getMCUint(void) 
{
	bool value;
	
#ifdef PCB_V2	
	// On PA03
	value = REG_PORT_IN0 & (0x0001 << 3);
#else	
	// On PA05
	value = REG_PORT_IN0 & (0x0001 << 5);
#endif

	return value;
}

void toggleLED(void)
{
#ifdef PCB_V2
	REG_PORT_OUTTGL1 = PORT_PB03;
#endif
}


void disableClockGate(void)
{
	clockDisable();
}

uint32_t interrupts;

void startSPI(void) {
	//__disable_irq();
	//interrupts = NVIC->ISER[0];
	//NVIC->ICER[0] = 0xffffffff; // Clear all interrupts
	
	NVIC_DisableIRQ(EIC_IRQn);
#ifdef PCB_V2	
	spiStart1();
#else
	spiStart0();
#endif
}

unsigned char sendSPI(unsigned char cmd) {
	//uint32_t check = NVIC->ISER[0];
	
#ifdef PCB_V2
	return spiSend1(cmd);
#else
	return spiSend0(cmd);
#endif
}

void pushSPI(unsigned char *data, unsigned char numdata){
#ifdef PCB_V2
	spiPushMulti1(data, numdata);
#else
	spiPushMulti0(data, numdata);
#endif
	
}

void pullSPI(unsigned char *data, unsigned char numdata){
	
#ifdef PCB_V2
	spiPullMulti1(data, numdata);
#else
	spiPullMulti0(data, numdata);
#endif
}


void stopSPI(void) {
#ifdef PCB_V2
	spiStop1();
#else
	spiStop0();
#endif

	//__enable_irq();
	if(extIntEnable)
		NVIC_EnableIRQ(EIC_IRQn);
	//NVIC->ISER[0] = interrupts;
}

void resetRadio(void)
{
	// Pull SPARK_RST low
#ifdef PCB_V2
	REG_PORT_DIRSET1 = PORT_PB02;
	REG_PORT_OUTCLR1 = PORT_PB02;
	timerDelay(10);
	REG_PORT_OUTSET1 = PORT_PB02;
#else
	REG_PORT_DIRSET0 = PORT_PA21;
	REG_PORT_OUTCLR0 = PORT_PA21;
	timerDelay(10);
	REG_PORT_OUTSET0 = PORT_PA21;
#endif
}

void boardSetup(void)
{
	__disable_irq();
	
	// Start system
	//system_init();
	
	// Config SPI
#ifdef PCB_V2
	spiInit1();
#else
	spiInit0();
#endif
	
	// Config output clocks
	clockInit();
	
	// Init timer for delay
	timerInit();
	
	// Pull IO_EN high
#ifdef PCB_V2
	REG_PORT_DIRSET0 = PORT_PA20;
	REG_PORT_OUTSET0 = PORT_PA20;
	//REG_PORT_OUTCLR0 = PORT_PA20;
#else
	REG_PORT_DIRSET0 = PORT_PA06;
	REG_PORT_OUTSET0 = PORT_PA06;
	//REG_PORT_OUTCLR0 = PORT_PA06;
#endif
	
	// Pull PLL_SEL high
#ifdef PCB_V2
	REG_PORT_DIRSET0 = PORT_PA27;
	REG_PORT_OUTSET0 = PORT_PA27;
	//REG_PORT_OUTCLR0 = PORT_PA27;
#else	
	REG_PORT_DIRSET0 = PORT_PA03;
	REG_PORT_OUTSET0 = PORT_PA03;
	//REG_PORT_OUTCLR0 = PORT_PA03;
#endif

	// Pull XTAL_SEL low
#ifdef PCB_V2
	REG_PORT_DIRSET1 = PORT_PB23;
	//REG_PORT_OUTSET1 = PORT_PB23;
	REG_PORT_OUTCLR1 = PORT_PB23;
#else
	REG_PORT_DIRSET0 = PORT_PA27;
	//REG_PORT_OUTSET0 = PORT_PA27;
	REG_PORT_OUTCLR0 = PORT_PA27;
#endif
	
	// Pull SPARK_RST low
#ifdef PCB_V2
	REG_PORT_DIRSET1 = PORT_PB02;
	REG_PORT_OUTCLR1 = PORT_PB02;
	timerDelay(1500);
	REG_PORT_OUTSET1 = PORT_PB02;
#else
	REG_PORT_DIRSET0 = PORT_PA21;
	REG_PORT_OUTCLR0 = PORT_PA21;
	timerDelay(1500);
	REG_PORT_OUTSET0 = PORT_PA21;
#endif

#ifdef PCB_V2
	// Enable debug port
	REG_PORT_DIRSET1 = PORT_PB09;
	REG_PORT_OUTCLR1 = PORT_PB09;
	REG_PORT_DIRSET1 = PORT_PB11;
	REG_PORT_OUTCLR1 = PORT_PB11;
	
	// Enable LED port
	REG_PORT_DIRSET1 = PORT_PB03;
	REG_PORT_OUTCLR1 = PORT_PB03;
#endif
	
	// External interrupt 5 for MCU_INT
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_EIC;
	PM->APBAMASK.bit.EIC_ = 1;
	
#ifdef PCB_V2
	EIC->EVCTRL.bit.EXTINTEO3 = 1;
	EIC->INTENSET.bit.EXTINT3 = 1;
	EIC->CONFIG->bit.SENSE3 = 0x5; // Low level detection
	EIC->CTRL.bit.ENABLE = 1; // Enable external interrupts
	
	const PORT_WRCONFIG_Type wrconfig = {
		.bit.WRPINCFG = 1,
		.bit.WRPMUX = 1,
		.bit.PMUX = MUX_PA03A_EIC_EXTINT3,
		.bit.PMUXEN = 1,
		.bit.HWSEL = 0,
		.bit.PINMASK = (uint16_t)((PORT_PA03))
	};
	PORT->Group[0].WRCONFIG.reg = wrconfig.reg;
#else	
	EIC->EVCTRL.bit.EXTINTEO5 = 1;
	EIC->INTENSET.bit.EXTINT5 = 1;
	EIC->CONFIG->bit.SENSE5 = 0x5; // Low level detection
	EIC->CTRL.bit.ENABLE = 1; // Enable external interrupts
	
	const PORT_WRCONFIG_Type wrconfig = {
		.bit.WRPINCFG = 1,
		.bit.WRPMUX = 1,
		.bit.PMUX = MUX_PA05A_EIC_EXTINT5,
		.bit.PMUXEN = 1,
		.bit.HWSEL = 0,
		.bit.PINMASK = (uint16_t)((PORT_PA05))
	};
	PORT->Group[0].WRCONFIG.reg = wrconfig.reg;
#endif
	
	//NVIC_EnableIRQ(EIC_IRQn);
	
	disableExtInt();
	
	NVIC_SetPriority(I2S_IRQn, 1);
	NVIC_SetPriority(USB_IRQn, 3);
	NVIC_SetPriority(TC5_IRQn, 3);
	NVIC_SetPriority(EIC_IRQn, 2);
	
	__enable_irq();
	
	// Setup audio
	i2c_init();			
	i2s_init();	

	//setup sensors
	OPT3001_init();	
	Si7021_init();
	MMA8491Q_init();
	sensor_timer_Init();
	

	REG_PORT_DIRSET0 = PORT_PA05;
	REG_PORT_OUTCLR0 = PORT_PA05;
	//REG_PORT_OUTSET0 = PORT_PA05;
	REG_PORT_DIRSET0 = PORT_PA04;
	REG_PORT_OUTCLR0 = PORT_PA04;
	//REG_PORT_OUTSET0 = PORT_PA04;
	REG_PORT_DIRSET1 = PORT_PB09;
	REG_PORT_OUTCLR1 = PORT_PB09;
	//REG_PORT_OUTSET1 = PORT_PB09;

	// Start USB stack to authorize VBus monitoring
	//udc_start();		
}

void EIC_Handler(void)
{
	// Check if right interrupt?
#ifdef PCB_V2
	REG_PORT_OUTSET1 = PORT_PB09;
#endif
	
	// Call radio callback
	callbackExtInterrupt();
	
		
#ifdef PCB_V2
	REG_PORT_OUTCLR1 = PORT_PB09;

#endif
	
	// Handle interrupt
	EIC->INTFLAG.bit.EXTINT5 = 1;
}

// Audio
uint32_t i2s_value;

//i2s handler: triggers every 1/(48000*2) seconds. When ready to tx or rx 1 channel
void I2S_Handler(void){
	
#ifdef PCB_V2
	REG_PORT_OUTSET1 = PORT_PB11;
#endif
	
	//NVIC_DisableIRQ(USB_IRQn);
	
	//if(I2S->INTFLAG.bit.RXRDY1 == 1 || I2S->INTFLAG.bit.TXRDY0 == 1)
	{
			
		i2s_value = i2s_read();//read value from CODEC
		
		/*
		read_buf = i2s_read();//read value fom CODEC
		hitx = (read_buf>>24);//get sample's high byte from i2s buffer [31:24 = hitx]
		lotx = (read_buf>>16) & 0xff;//get sample's low byte from i2s buffer [23:16 = lotx] [15:0 = 0]
		push(TX_fifo, hitx);//put sample's high byte in TX FIFO
		push(TX_fifo, lotx);//put sample's low byte in TX FIFO*/
	//}
	
	//if(I2S->INTFLAG.bit.TXRDY0 == 1){
	//{
			
		i2s_write(callbackAudioSample(i2s_value)); //write sample to Codec
		//i2s_write(i2s_value); //write sample to Codec
		
		/*
		pop(RX_fifo, &hirx);//get sample's high byte from RX FIFO
		pop(RX_fifo, &lorx);//get sample's high byte from RX FIFO
		write_buf = (((hirx<<24)) | lorx<<16);//create i2s buffer [31:24 = hirx] [23:16 = lorx] [15:0 = 0]
		i2s_write(write_buf);//write sample to Codec*/
	}
	
	//NVIC_EnableIRQ(USB_IRQn);
	I2S->INTFLAG.bit.RXRDY1;
	I2S->INTFLAG.bit.TXRDY0;
	
void TC3_Handler(void){

		REG_PORT_OUTSET0 = PORT_PA05;
		TC3->COUNT16.INTFLAG.reg = TC_INTFLAG_MC1;
		TC3->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
	}
#ifdef PCB_V2
	REG_PORT_OUTCLR1 = PORT_PB11;
#endif
}

// USB stdio
char std_buf[128];

bool usbIsAvailable(void)
{
	//return udi_cdc_is_rx_ready();
	return usb_bytes_available() > 0;
	//return 0;
}

uint16_t usbBytesAvailable(void)
{
	//return udi_cdc_is_rx_ready();
	return usb_bytes_available();
	//return 0;
}

uint8_t usbGet(void)
{
	uint8_t c;
	//return udi_cdc_getc();
	usb_get(&c, 1);
	return c;
	//return 0;
}

void usbPut(uint8_t c)
{
	//udi_cdc_putc(c);
	usb_put(&c, 1);
}

void usbPrintf(const char *fmt, ...)
{
	char *i = std_buf;
	
	va_list va;
	va_start(va, fmt);
	
	vsprintf(std_buf, fmt, va);
	
	usb_put(std_buf, strlen(std_buf));
	
	/*
	while(*i)
	{
		//udi_cdc_putc(*i);
		usb_put(i, 1);
		i++;
	}*/
	
	va_end(va);
}

uint32_t usbParseInt()
{ 
	unsigned char c=0;
	unsigned char *p;
	int number;
	
	p = std_buf;
	//c = udi_cdc_getc();
	usb_get(&c, 1);
	while(c>47 && c<58) // Only accept numbers
	{
		*p = c; p++;
		//c = udi_cdc_getc();
		usb_get(&c, 1);
	}
	*p = 0;
	sscanf(std_buf, "%d", &number);
	
	return number;
}

// Watchdog, 32KHz timer
void callbackWatchdog(); // Callback has to be defined in radio

#ifdef WD_ENABLE
void TC5_Handler(void) {
	
	if(TC5->COUNT16.INTFLAG.bit.MC0)
	{
		callbackWatchdog();
		TC5->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
	}	 	
}
#endif

void wdSetup(unsigned short ticks)
{
#ifdef WD_ENABLE	
	PM->APBCMASK.reg |= PM_APBCMASK_TC5;
	
	// Set divider for clock controller 3
	const GCLK_GENDIV_Type gendiv = {
		.bit.ID = GCLK_CLKCTRL_GEN_GCLK3_Val,
		.bit.DIV = 0
	};
	GCLK->GENDIV = gendiv;
	
	// Feed XOSC32K to clock controller 3
	const GCLK_GENCTRL_Type gentrcl = {
		.bit.ID =  GCLK_CLKCTRL_GEN_GCLK3_Val,
		.bit.SRC = GCLK_GENCTRL_SRC_XOSC32K_Val,
		.bit.GENEN = 1,
		.bit.IDC = 0,
		.bit.OOV = 0,
		.bit.OE = 1,
		.bit.DIVSEL = 0,
		.bit.RUNSTDBY = 1
	};
	GCLK->GENCTRL = gentrcl;

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(TC3_GCLK_ID) |
	GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(3); // Take from clock generator 5 (32 KHz)

	TC5->COUNT16.CTRLA.reg =
	TC_CTRLA_MODE(TC_CTRLA_MODE_COUNT16_Val) |
	TC_CTRLA_WAVEGEN(TC_CTRLA_WAVEGEN_MFRQ_Val) |
	TC_CTRLA_PRESCALER(TC_CTRLA_PRESCALER_DIV1_Val) |
	TC_CTRLA_PRESCSYNC(TC_CTRLA_PRESCSYNC_PRESC_Val); 
	TC5->COUNT16.COUNT.reg = 0;
	TC5->COUNT16.CC[0].reg = ticks;
	
	TC5->COUNT16.INTENSET.reg = TC_INTENSET_MC0; // Enable interrupt
	NVIC_EnableIRQ(TC5_IRQn); 
#endif	
}

void wdReset()
{
#ifdef WD_ENABLE	
	TC5->COUNT16.COUNT.reg = 0;
#endif
}

void wdEnable()
{
#ifdef WD_ENABLE
	TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
#endif
}

void wdDisable()
{
#ifdef WD_ENABLE	
	TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
#endif
}


