#pragma once

#include "sam.h"
#include "../usb.h"

#define USB_EP_size_to_gc(x) ((x <=   8) ? 0:\
							  (x <=  16) ? 1:\
							  (x <=  32) ? 2:\
							  (x <=  64) ? 3:\
							  (x <= 128) ? 4:\
							  (x <= 256) ? 5:\
							  (x <= 512) ? 6:\
										   7)

#define USB_EPTYPE_DISABLED 0
#define USB_EPTYPE_CONTROL 1
#define USB_EPTYPE_ISOCHRONOUS 2
#define USB_EPTYPE_BULK 3
#define USB_EPTYPE_INTERRUPT 4
#define USB_EPTYPE_DUAL_BANK 5

extern UsbDeviceDescriptor usb_endpoints[];
extern const uint8_t usb_num_endpoints;

#define USB_ALIGN __attribute__((__aligned__(4)))

#define USB_ENDPOINTS(NUM_EP) \
	const uint8_t usb_num_endpoints = (NUM_EP); \
	UsbDeviceDescriptor usb_endpoints[(NUM_EP)+1];

void* samd_serial_number_string_descriptor();
