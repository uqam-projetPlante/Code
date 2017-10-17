/*
 * _12c_driver.h
 *
 * Created: 2017-05-19 17:14:53
 *  Author: collerette_a
 */ 


#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_
#include "mux.h"

#undef ENABLE // Clash between ASF and plain SAM

/*- Definitions -------------------------------------------------------------*/
#define I2C_SERCOM            SERCOM2
#define I2C_SERCOM_GCLK_ID    SERCOM2_GCLK_ID_CORE
#define I2C_SERCOM_CLK_GEN    2
#define I2C_SERCOM_APBCMASK   PM_APBCMASK_SERCOM2
#define PINMUX_PAD0           PINMUX_PA12C_SERCOM2_PAD0
#define PINMUX_PAD1           PINMUX_PA13C_SERCOM2_PAD1

enum
{
	I2C_TRANSFER_WRITE = 0,
	I2C_TRANSFER_READ  = 1,
};

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------


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
		//udi_cdc_putc('A');
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
			//udi_cdc_putc('B');
			return false;
		}
	}

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