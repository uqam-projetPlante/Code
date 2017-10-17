/*
 * DMAC.h
 *
 * Created: 2017-06-27 10:32:42
 *  Author: collerette_a
 */ 


#ifndef DMA_DRIVER_H_
#define DMA_DRIVER_H_

#include "status_codes.h"

#undef ENABLE // Clash between ASF and plain SAM

#define NEW_CHANNEL 0

#define CONF_MAX_USED_CHANNEL_NUM     1

DmacDescriptor descriptor_section[CONF_MAX_USED_CHANNEL_NUM];

/** Initial write back memory section. */
static DmacDescriptor _write_back_section[CONF_MAX_USED_CHANNEL_NUM];

/** Internal DMA resource pool. */
static struct dma_resource* _dma_active_resource[CONF_MAX_USED_CHANNEL_NUM];
	
enum dma_callback_type {
	/** Callback for any of transfer errors. A transfer error is flagged
     *	if a bus error is detected during an AHB access or when the DMAC
	 *  fetches an invalid descriptor. */
	DMA_CALLBACK_TRANSFER_ERROR,
	/** Callback for transfer complete. */
	DMA_CALLBACK_TRANSFER_DONE,
	/** Callback for channel suspend. */
	DMA_CALLBACK_CHANNEL_SUSPEND,
	/** Number of available callbacks. */
	DMA_CALLBACK_N,
};

/** Forward definition of the DMA resource. */
struct dma_resource;
/** Type definition for a DMA resource callback function. */
typedef void (*dma_callback_t)(struct dma_resource *const resource);
/** Structure for DMA transfer resource. */
struct dma_resource {
	/** Allocated DMA channel ID */
	uint8_t channel_id;
	/** Array of callback functions for DMA transfer job */
	dma_callback_t callback[DMA_CALLBACK_N];
	/** Bit mask for enabled callbacks */
	uint8_t callback_enable;
	/** Transferred data size */
	uint32_t transfered_size;
	/** DMA transfer descriptor */
	DmacDescriptor* descriptor;
};

/* Global function prototypes */
void DMA_init(void);
void DMA_start_transfer(uint8_t*, int len);
void DMA_wait_complete(void);

#endif /* DMAC_H_ */