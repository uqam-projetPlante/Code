/*
 * usart.c
 *
 * Created: 2017-10-10 12:50:43
 *  Author: collerette_a
 */ 

#include <asf.h>
#include "usart.h"
#undef ENABLE // Clash between ASF and plain SAM

/*- Definitions -------------------------------------------------------------*/
#define UART_SERCOM           SERCOM4
#define UART_SERCOM_GCLK_ID   SERCOM4_GCLK_ID_CORE
#define UART_SERCOM_CLK_GEN   2
#define UART_SERCOM_APBCMASK  PM_APBCMASK_SERCOM4
#define PINMUX_PAD2_TX        PINMUX_PB08D_SERCOM4_PAD0
#define PINMUX_PAD3_RX        PINMUX_PB09D_SERCOM4_PAD1

void pin_set_peripheral_function(uint32_t pinmux)
{
    /* the variable pinmux consist of two components:
        31:16 is a pad, wich includes:
            31:21 : port information 0->PORTA, 1->PORTB
            20:16 : pin 0-31
        15:00 pin multiplex information
        there are defines for pinmux like: PINMUX_PA09D_SERCOM2_PAD1 
    */
    uint16_t pad = pinmux >> 16;    // get pad (port+pin)
    uint8_t port = pad >> 5;        // get port
    uint8_t pin  = pad & 0x1F;      // get number of pin - no port information anymore
    
    PORT->Group[port].PINCFG[pin].bit.PMUXEN = 1;
    
    /* each pinmux register is for two pins! with pin/2 you can get the index of the needed pinmux register
       the p mux resiter is 8Bit   (7:4 odd pin; 3:0 evan bit)  */
    // reset pinmux values.                             VV shift if pin is odd (if evan:  (4*(pin & 1))==0  )
    PORT->Group[port].PMUX[pin/2].reg &= ~( 0xF << (4*(pin & 1)) );
                    //          
    // set new values
PORT->Group[port].PMUX[pin/2].reg |= ( (uint8_t)( (pinmux&0xFFFF) <<(4*(pin&1)) ) ); 
}


void uart_init(void)
{
	pin_set_peripheral_function(PINMUX_PAD2_TX);
	pin_set_peripheral_function(PINMUX_PAD3_RX);  

	PM->APBCMASK.reg |= UART_SERCOM_APBCMASK;

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(UART_SERCOM_GCLK_ID) |
	GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(UART_SERCOM_CLK_GEN);

	UART_SERCOM->USART.CTRLB.reg = SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_RXEN;
	while (UART_SERCOM->USART.SYNCBUSY.bit.CTRLB);
	
	UART_SERCOM->USART.BAUD.reg = SERCOM_USART_BAUD_BAUD(64278);//60503 = 38400 baud rate & 64278 = 9600 baud rate
	
	UART_SERCOM->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE(1) | SERCOM_USART_CTRLA_DORD |
	SERCOM_USART_CTRLA_RXPO(1) | SERCOM_USART_CTRLA_TXPO(0) | SERCOM_USART_CTRLA_SAMPR(0) |
	SERCOM_USART_CTRLA_ENABLE;
	while (UART_SERCOM->USART.SYNCBUSY.bit.ENABLE);

}

uart_send(uint8_t tx_data)
{
	/* Wait until synchronization is complete */
	//while (UART_SERCOM->USART.SYNCBUSY.reg);

	/* Write data to USART module */
	UART_SERCOM->USART.DATA.reg = tx_data;

	while (!(UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_TXC)) {
		/* Wait until data is sent */
	}

	/* Wait until Transmit is complete or timeout */
	for (uint32_t i = 0; i <= USART_TIMEOUT; i++) {
		if (UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_TXC) {
			break;
			}
			 else if (i == USART_TIMEOUT) {
			return STATUS_ERR_TIMEOUT;
		}
	}
}

uint8_t uart_read()
{
	/* Check if USART has new data */
	if(!(UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXC))
	{
		return 0;
	}
	else
	{
		/* Wait until synchronization is complete */
		while (UART_SERCOM->USART.SYNCBUSY.reg);
		/* Read data from USART module */
		return UART_SERCOM->USART.DATA.reg;
	}
}

void uart_sendBuffer(const uint8_t *tx_data, uint16_t length)
{
	
	/* Wait until synchronization is complete */
	while (UART_SERCOM->USART.SYNCBUSY.reg);

	uint16_t tx_pos = 0;

	/* Blocks while buffer is being transferred */
	while (length--) {
		/* Wait for the USART to be ready for new data and abort
		* operation if it doesn't get ready within the timeout*/
		for (uint32_t i = 0; i <= USART_TIMEOUT; i++) {
			if (UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE) {
				break;
			} else if (i == USART_TIMEOUT) {
				return STATUS_ERR_TIMEOUT;
			}
		}

		/* Data to send is at least 8 bits long */
		uint16_t data_to_send = tx_data[tx_pos++];

		/* Check if the character size exceeds 8 bit */
		//if (module->character_size == USART_CHARACTER_SIZE_9BIT) {
			//data_to_send |= (tx_data[tx_pos++] << 8);
		//}

		/* Send the data through the USART module */
			/* Check if USART is ready for new data */
			//if (!(usart_hw->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE)) {
				///* Return error code */
				//return STATUS_BUSY;
			//}
			//#endif

		/* Wait until synchronization is complete */
		while (UART_SERCOM->USART.SYNCBUSY.reg);

			/* Write data to USART module */
			UART_SERCOM->USART.DATA.reg = data_to_send;

			while (!(UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_TXC)) {
				/* Wait until data is sent */
			}
	}

	/* Wait until Transmit is complete or timeout */
	for (uint32_t i = 0; i <= USART_TIMEOUT; i++) {
		if (UART_SERCOM->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_TXC) {
			break;
		} else if (i == USART_TIMEOUT) {
			return STATUS_ERR_TIMEOUT;
		}
	}

	return STATUS_OK;
}
