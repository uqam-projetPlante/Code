/*
 * _12c_driver.h
 *
 * Created: 2017-05-19 17:14:53
 *  Author: collerette_a
 */ 


#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_


#undef ENABLE // Clash between ASF and plain SAM

/*- Definitions -------------------------------------------------------------*/
#define I2C_SERCOM            SERCOM2
#define I2C_SERCOM_GCLK_ID    SERCOM2_GCLK_ID_CORE
#define I2C_SERCOM_CLK_GEN    2
#define I2C_SERCOM_APBCMASK   PM_APBCMASK_SERCOM2
#define PINMUX_PAD0           PINMUX_PA08D_SERCOM2_PAD0
#define PINMUX_PAD1           PINMUX_PA09D_SERCOM2_PAD1

enum
{
	I2C_TRANSFER_WRITE = 0,
	I2C_TRANSFER_READ  = 1,
};

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
inline void pin_set_peripheral_function(uint32_t pinmux)
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


inline void i2c_init(void)
{
	pin_set_peripheral_function(PINMUX_PAD0);
	pin_set_peripheral_function(PINMUX_PAD1);  

	PM->APBCMASK.reg |= I2C_SERCOM_APBCMASK;

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(I2C_SERCOM_GCLK_ID) |
	GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(I2C_SERCOM_CLK_GEN);

	I2C_SERCOM->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;
	while (I2C_SERCOM->I2CM.SYNCBUSY.reg);

	SERCOM2->I2CM.BAUD.reg = SERCOM_I2CM_BAUD_BAUD(34);
	while (I2C_SERCOM->I2CM.SYNCBUSY.reg);

	I2C_SERCOM->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_ENABLE |
	SERCOM_I2CM_CTRLA_MODE_I2C_MASTER |
	SERCOM_I2CM_CTRLA_SDAHOLD(3);
	while (I2C_SERCOM->I2CM.SYNCBUSY.reg);

	I2C_SERCOM->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(1);
	while (I2C_SERCOM->I2CM.SYNCBUSY.reg);	
}


//-----------------------------------------------------------------------------
inline bool i2c_write(uint8_t *data, int size, uint8_t addr)
{
	I2C_SERCOM->I2CM.ADDR.reg = (addr<<1) | I2C_TRANSFER_WRITE;

	while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));

	if (I2C_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
	{
		I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		//dbg_log("I2C: RXNACK during write (address)\r\n");
		udi_cdc_putc('A');
		return false;
	}

	for (int i = 0; i < size; i++)
	{
		I2C_SERCOM->I2CM.DATA.reg = data[i];

		while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));

		if (I2C_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
		{
			I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
			//dbg_log("I2C: RXNACK during write (data)\r\n");
			udi_cdc_putc('B');
			return false;
		}
	}
	//udi_cdc_putc('B');

	I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);

	return true;
}

//-----------------------------------------------------------------------------
inline bool i2c_write_start(uint8_t addr)
{
	I2C_SERCOM->I2CM.ADDR.reg = (addr<<1) | I2C_TRANSFER_WRITE;

	while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));

	if (I2C_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
	{
		I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
inline bool i2c_write_byte(uint8_t byte)
{
	I2C_SERCOM->I2CM.DATA.reg = byte;

	while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));

	if (I2C_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
	{
		I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
inline void i2c_write_stop(void)
{
	I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
}

//-----------------------------------------------------------------------------
inline bool i2c_read(uint8_t *data, int size, uint8_t addr)
{
	I2C_SERCOM->I2CM.ADDR.reg = (addr<<1)  | I2C_TRANSFER_READ;

	while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB));

	if (I2C_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
	{
		I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		//dbg_log("I2C: RXNACK during read (address)\r\n");
		return false;
	}

	I2C_SERCOM->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;

	for (int i = 0; i < size-1; i++)
	{
		data[i] = I2C_SERCOM->I2CM.DATA.reg;
		while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB));
	}

	if (size)
	{
		I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
		I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		data[size-1] = I2C_SERCOM->I2CM.DATA.reg;
	}
	I2C_SERCOM->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(1);
	while (I2C_SERCOM->I2CM.SYNCBUSY.reg);
	return true;
}

//-----------------------------------------------------------------------------
inline bool i2c_busy(uint8_t addr)
{
	bool busy;

	I2C_SERCOM->I2CM.ADDR.reg = (addr<<1) | I2C_TRANSFER_WRITE;

	while (0 == (I2C_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));

	busy = (0 != (I2C_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK));

	I2C_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);

	return busy;
}


#endif /* 12C_DRIVER_H_ */