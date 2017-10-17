/*
* DMAC.c
*
* Created: 2017-06-27 10:36:34
*  Author: collerette_a
*/
/**
* \file
*
* \brief SAM Sercom SPI Driver with DMA Quick Start
*
* Copyright (C) 2014-2016 Atmel Corporation. All rights reserved.
*
* \asf_license_start
*
* \page License
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel microcontroller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
*/
#include <stdbool.h>
#include <string.h>
#include <samd21j18a.h>
#include "DMA_driver.h"

//#define DMA_USE_INTERRUPT

///* SPI buffer length*/
//#define BUF_LENGTH 20
//
///* SPI buffer */
//uint8_t buffer_tx[BUF_LENGTH] = {
	//0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
	//0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
//};

/* DMA resource */
struct dma_resource DMA_resource;

/* DMA transfer descriptor */
DmacDescriptor DMA_descriptor;

/* Local function prototypes */
void configure_dma_resource(struct dma_resource *resource);
void setup_transfer_descriptor(DmacDescriptor *descriptor);
void add_descriptor(struct dma_resource *resource, DmacDescriptor* descriptor);
void start_transfer_job(struct dma_resource *resource, uint8_t* buffer_tx,int len);

void configure_dma_resource(struct dma_resource *resource)
{
	uint8_t new_channel;
	
	/* Initialize clocks for DMA */
	PM->APBCMASK.reg |= PM_AHBMASK_DMAC;
	PM->APBBMASK.reg |= PM_APBBMASK_DMAC;

	/* Perform a software reset before enable DMA controller */
	DMAC->CTRL.reg &= ~DMAC_CTRL_DMAENABLE;
	DMAC->CTRL.reg = DMAC_CTRL_SWRST;

	/* Setup descriptor base address and write back section base
	* address */
	DMAC->BASEADDR.reg = (uint32_t)descriptor_section;
	DMAC->WRBADDR.reg = (uint32_t)_write_back_section;

	/* Enable all priority level at the same time */
	DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);
	
	new_channel = NEW_CHANNEL;
	
	/* Set the channel */
	resource->channel_id = new_channel;
	
	/** Perform a reset for the allocated channel */
	DMAC->CHID.reg = DMAC_CHID_ID(resource->channel_id);
	DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
	DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
	
	uint32_t temp_CHCTRLB_reg;

	/** Select the DMA channel and clear software trigger */
	DMAC->CHID.reg = DMAC_CHID_ID(resource->channel_id);
	DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 <<resource->channel_id));

	/** Select priority level, trigger source and trigger action */
	temp_CHCTRLB_reg = DMAC_CHCTRLB_LVL(0) |
	DMAC_CHCTRLB_TRIGSRC(SERCOM1_DMAC_ID_TX) | //ready to transmit
	//DMAC_CHCTRLB_TRIGACT(DMAC_CHCTRLB_TRIGACT_TRANSACTION_Val);
	DMAC_CHCTRLB_TRIGACT(DMAC_CHCTRLB_TRIGACT_BEAT_Val);

	/* Write config to CTRLB register */
	DMAC->CHCTRLB.reg = temp_CHCTRLB_reg;
	
	/* Enable DMA interrupt */
#ifdef DMA_USE_INTERRUPT
	NVIC_ClearPendingIRQ(DMAC_IRQn);
	NVIC_SetPriority(DMAC_IRQn, 2);
	NVIC_EnableIRQ(DMAC_IRQn);
#endif

	/* Set the interrupt flag */
	DMAC->CHID.reg = DMAC_CHID_ID(resource->channel_id);
	DMAC->CHINTENSET.reg = (DMAC_CHINTENSET_MASK & (1UL << DMA_CALLBACK_TRANSFER_DONE));
	
	resource->descriptor = NULL;
	
	/* Log the DMA resource into the internal DMA resource pool */
	_dma_active_resource[resource->channel_id] = resource;

}

void setup_transfer_descriptor(DmacDescriptor *descriptor)
{
	/* Set descriptor as valid */
	descriptor->BTCTRL.bit.VALID = true;
	/* Disable event output */
	descriptor->BTCTRL.bit.EVOSEL = 0;
	/* No block action */
	descriptor->BTCTRL.bit.BLOCKACT = 0;
	/* Set beat size to byte */
	descriptor->BTCTRL.bit.BEATSIZE = 0;
	/* Enable source increment */
	descriptor->BTCTRL.bit.SRCINC = true;
	/* Disable destination increment */
	descriptor->BTCTRL.bit.DSTINC = false;
	/* Step size is applied to the destination address */
	descriptor->BTCTRL.bit.STEPSEL = 0;
	/* Address increment is beat size multiplied by 1*/
	descriptor->BTCTRL.bit.STEPSIZE = 0;

	/* Set transfer size */
	//descriptor->BTCNT.reg = len/sizeof(uint8_t);
	/* Source address*/
	//descriptor->SRCADDR.reg = (uint32_t)buffer_tx + len;
	/* Destination address*/
	descriptor->DSTADDR.reg = (uint32_t)(&SERCOM1->SPI.DATA.reg);

	/* Set next transfer descriptor address */
	descriptor->DESCADDR.reg = 0;
}

void add_descriptor(struct dma_resource *resource, DmacDescriptor* descriptor)
{
	DmacDescriptor* desc = resource->descriptor;

	/* Look up for an empty space for the descriptor */
	if (desc == NULL) {
		resource->descriptor = descriptor;
		} else {
		/* Looking for end of descriptor link */
		while(desc->DESCADDR.reg != 0) {
			desc = (DmacDescriptor*)(desc->DESCADDR.reg);
		}
		/* Set to the end of descriptor list */
		desc->DESCADDR.reg = (uint32_t)descriptor;
	}

}

void start_transfer_job(struct dma_resource *resource, uint8_t* buffer_tx,int len)
{		
	/* Set transfer size */
	resource->descriptor->BTCNT.reg = len;
	/* Source address*/
	resource->descriptor->SRCADDR.reg = (uint32_t)buffer_tx + len;
	
	/* Set channel x descriptor 0 to the descriptor base address */
	memcpy(&descriptor_section[resource->channel_id], resource->descriptor,
	sizeof(DmacDescriptor));

	/* Enable the transfer channel */
	DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
}

void DMA_init(void)
{
	/* Setup DMA resource */
	configure_dma_resource(&DMA_resource);
	
	/* Setup transfer description */
	setup_transfer_descriptor(&DMA_descriptor);
	
	/*Add DMA resource descriptor*/
	add_descriptor(&DMA_resource, &DMA_descriptor);
}

void DMA_start_transfer(uint8_t* buffer_tx, int len){
	
	start_transfer_job(&DMA_resource, buffer_tx, len);
	
}

void DMA_wait_complete(void){
	while(DMAC->INTSTATUS.bit.CHINT0 == 0);
	//while(DMAC->CHINTFLAG.bit.TCMPL == 0);
	
	DMAC->CHINTFLAG.bit.TCMPL = 1; // Reset interrupt flags
	
	DMAC->CHCTRLA.reg = 0; // Disable channel
}
