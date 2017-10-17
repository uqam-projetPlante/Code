/*
 * usart.c
 *
 * Created: 2017-10-10 12:50:43
 *  Author: collerette_a
 */ 

#include <asf.h>
#undef ENABLE // Clash between ASF and plain SAM

/*- Definitions -------------------------------------------------------------*/
#define UART_SERCOM           SERCOM4
#define UART_SERCOM_GCLK_ID   SERCOM4_GCLK_ID_CORE
#define UART_SERCOM_CLK_GEN   4
#define UART_SERCOM_APBCMASK  PM_APBCMASK_SERCOM4
#define PINMUX_PAD0_TX        PINMUX_PB10D_SERCOM4_PAD2
#define PINMUX_PAD1_RX        PINMUX_PB11D_SERCOM4_PAD3

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


inline void usart_init(void)
{
	pin_set_peripheral_function(PINMUX_PAD0_TX);
	pin_set_peripheral_function(PINMUX_PAD1_RX);  

	PM->APBCMASK.reg |= UART_SERCOM_APBCMASK;

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(UART_SERCOM_GCLK_ID) |
	GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(UART_SERCOM_CLK_GEN);

	//I2C_SERCOM->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;
	//while (I2C_SERCOM->I2CM.SYNCBUSY.reg);
//
	//SERCOM2->I2CM.BAUD.reg = SERCOM_I2CM_BAUD_BAUD(34);
	//while (I2C_SERCOM->I2CM.SYNCBUSY.reg);
//
	//I2C_SERCOM->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_ENABLE |
	//SERCOM_I2CM_CTRLA_MODE_I2C_MASTER |
	//SERCOM_I2CM_CTRLA_SDAHOLD(3);
	//while (I2C_SERCOM->I2CM.SYNCBUSY.reg);
//
	//I2C_SERCOM->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(1);
	//while (I2C_SERCOM->I2CM.SYNCBUSY.reg);	
}