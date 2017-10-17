/*
 * flash.c
 *
 * Created: 2017-05-17 15:17:38
 *  Author: collerette_a
 */ 

//#include <asf.h>#include <samd21j18a.h>
#include "flash.h"

struct flash_module {
	/** Number of bytes contained per page. */
	uint16_t page_size;
	/** Total number of pages in the NVM memory. */
	uint16_t number_of_pages;
	/** If \c false, a page write command will be issued automatically when the
	 *  page buffer is full. */
	bool manual_page_write;
};

static struct flash_module flash_dev;

void flash_init(void)
{
	PM->APBBMASK.reg |= PM_APBBMASK_NVMCTRL;
	
	/* Clear error flags */
	NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;
	
	/* wait until the module is ready */
	while(NVMCTRL->INTFLAG.bit.READY == 0);
	
	/* Writing configuration to the CTRLB register */
	NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_RWS(NVMCTRL->CTRLB.bit.RWS) ;
	
	/* Initialize the internal device struct */
	flash_dev.page_size         = (8 << NVMCTRL->PARAM.bit.PSZ);
	flash_dev.number_of_pages   = NVMCTRL->PARAM.bit.NVMP;
	flash_dev.manual_page_write = true;	
}

void flash_read_buffer(
const uint32_t source_address,
uint8_t *const buffer,
uint16_t length)
{
	/* wait until the module is ready */
	while(NVMCTRL->INTFLAG.bit.READY == 0);

	/* Clear error flags */
	NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

	uint32_t page_address = source_address / 2;

	/* NVM _must_ be accessed as a series of 16-bit words, perform manual copy
	 * to ensure alignment */
	for (uint16_t i = 0; i < length; i += 2) {
		/* Fetch next 16-bit chunk from the NVM memory space */
		uint16_t data = NVM_MEMORY[page_address++];

		/* Copy first byte of the 16-bit chunk to the destination buffer */
		buffer[i] = (data & 0xFF);

		/* If we are not at the end of a read request with an odd byte count,
		 * store the next byte of data as well */
		if (i < (length - 1)) {
			buffer[i + 1] = (data >> 8);
		}
	}	
}

void flash_erase_row(
const uint32_t row_address)
{
	/* wait until the module is ready */
	while(NVMCTRL->INTFLAG.bit.READY == 0);

	/* Clear error flags */
	NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

	/* Set address and command */
	NVMCTRL->ADDR.reg  = (uintptr_t)&NVM_MEMORY[row_address / 4];
	
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_ER | NVMCTRL_CTRLA_CMDEX_KEY;
}

void flash_write_buffer(
const uint32_t destination_address,
const uint8_t *buffer,
uint16_t length)
{
	/* wait until the module is ready */
	while(NVMCTRL->INTFLAG.bit.READY == 0);
		
	/* Erase the page buffer before buffering new data */
	NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY;

	/* wait until the module is ready */
	while(NVMCTRL->INTFLAG.bit.READY == 0);

	/* Clear error flags */
	NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

	uint32_t nvm_address = destination_address / 2;

	/* NVM _must_ be accessed as a series of 16-bit words, perform manual copy
	 * to ensure alignment */
	for (uint16_t i = 0; i < length; i += 2) {
		uint16_t data;

		/* Copy first byte of the 16-bit chunk to the temporary buffer */
		data = buffer[i];

		/* If we are not at the end of a write request with an odd byte count,
		 * store the next byte of data as well */
		if (i < (length - 1)) {
			data |= (buffer[i + 1] << 8);
		}

		/* Store next 16-bit chunk to the NVM memory space */
		NVM_MEMORY[nvm_address++] = data;
	}
}

