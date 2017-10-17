#include "samd/usb_samd.h"
#include "fifo.h"

typedef struct {
	USB_ConfigurationDescriptor conf;
	USB_InterfaceDescriptor interf;
	USB_EndpointDescriptor endpOUT;
	USB_EndpointDescriptor endpIN;
} __attribute__ ((packed)) USB_ConfCollectionDesc;

USB_ALIGN const USB_DeviceDescriptor devDesc = {
.bLength			= sizeof(USB_DeviceDescriptor),
.bDescriptorType	= USB_DTYPE_Device,
.bcdUSB				= 0x0200,
.bDeviceClass		= USB_CSCP_NoDeviceClass,
.bDeviceSubClass	= USB_CSCP_NoDeviceSubclass,
.bDeviceProtocol	= USB_CSCP_NoDeviceProtocol,
.bMaxPacketSize0	= USB_EP0_SIZE,
.idVendor			= USB_VID_ATMEL,
.idProduct			= USB_PID_ATMEL_TEST_VENDOR_CLASS,
.bcdDevice			= (USB_DEVICE_MAJOR_VERSION << 8) | USB_DEVICE_MINOR_VERSION,
.iManufacturer		= 0x01,
.iProduct			= 0x02,
.iSerialNumber		= 0x03,
.bNumConfigurations = 0x01 };

USB_ALIGN const USB_ConfCollectionDesc confDesc = {
.conf.bLength			   = sizeof(USB_ConfigurationDescriptor),
.conf.bDescriptorType	   = USB_DTYPE_Configuration,
.conf.wTotalLength		   = sizeof(USB_ConfCollectionDesc),
.conf.bNumInterfaces	   = 0x01,
.conf.bConfigurationValue  = 0x01,
.conf.iConfiguration	   = 0x04,
.conf.bmAttributes		   = USB_CONFIG_ATTR_BUSPOWERED,
.conf.bMaxPower			   = USB_CONFIG_POWER_MA(100),
.interf.bLength			   = sizeof(USB_InterfaceDescriptor),
.interf.bDescriptorType	   = USB_DTYPE_Interface,
.interf.bInterfaceNumber   = 0x00,
.interf.bAlternateSetting  = 0x00,
.interf.bNumEndpoints	   = 0x02,
.interf.bInterfaceClass	   = USB_CSCP_NoDeviceClass,
.interf.bInterfaceSubClass = USB_CSCP_NoDeviceSubclass,
.interf.bInterfaceProtocol = USB_CSCP_NoDeviceProtocol,
.interf.iInterface		   = 0x05,
.endpOUT.bLength		   = sizeof(USB_EndpointDescriptor),
.endpOUT.bDescriptorType   = USB_DTYPE_Endpoint,
.endpOUT.bEndpointAddress  = USB_OUT | 0x01,
.endpOUT.bmAttributes	   = USB_EP_TYPE_INTERRUPT,
.endpOUT.wMaxPacketSize	   = USB_EP1_SIZE,
.endpOUT.bInterval		   = 20, //200 michiel
.endpIN.bLength			   = sizeof(USB_EndpointDescriptor),
.endpIN.bDescriptorType	   = USB_DTYPE_Endpoint,
.endpIN.bEndpointAddress   = USB_IN | 0x01,
.endpIN.bmAttributes	   = USB_EP_TYPE_INTERRUPT,
.endpIN.wMaxPacketSize	   = USB_EP1_SIZE,
.endpIN.bInterval		   = 20 }; // 200 michiel

USB_ALIGN const USB_StringDescriptor langDesc = {
.bLength = 4,
.bDescriptorType = USB_DTYPE_String,
.bString = { USB_LANGUAGE_EN_US }};

USB_ALIGN const USB_StringDescriptor msft_os = {
.bLength = 18,
.bDescriptorType = USB_DTYPE_String,
.bString = { u'M', u'S', u'F', u'T', u'1', u'0', u'0', USB_REQ_GetMSDescriptor }};

USB_ALIGN const USB_MicrosoftCompatibleDescriptor msft_compatible = {
.dwLength = sizeof(USB_MicrosoftCompatibleDescriptor) + (1 * sizeof(USB_MicrosoftCompatibleDescriptor_Interface)),
.bcdVersion = 0x0100,
.wIndex = 0x0004,
.bCount = 0x01,
.reserved = {0, 0, 0, 0, 0, 0, 0},
.interfaces = {{
	.bFirstInterfaceNumber = 0x00,
	.reserved1 = 0x01,
//	.compatibleID = "LIBUSBK\0",
//	.compatibleID = "LIBUSB0\0",
	.compatibleID = "WinUSB\0\0",
	.subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
	.reserved2 = {0, 0, 0, 0, 0, 0}}}};

typedef struct {
	uint32_t dwLength;
	uint16_t bcdVersion;
	uint16_t wIndex;
	uint16_t wCount;
	uint32_t dwPropLength1;
	uint32_t dwType1;
	uint16_t wNameLength1;
	uint16_t name1[20];
	uint32_t dwDataLength1;
	uint16_t data1[39];
} __attribute__((packed)) USB_MicrosoftExtendedPropertiesDescriptor;

USB_ALIGN const USB_MicrosoftExtendedPropertiesDescriptor msft_extended = {
.dwLength = 142,
.bcdVersion = 0x0100,
.wIndex = 0x05,
.wCount = 0x01,
.dwPropLength1 = 132,
.dwType1 = 1,
.wNameLength1 = 40,
.name1 = u"DeviceInterfaceGUID\0",
.dwDataLength1 = 78,
.data1 = u"{3c33bbfd-71f9-4815-8b8f-7cd1ef928b3d}\0" };

USB_ALIGN UsbDeviceDescriptor usb_endpoints[8];
USB_ALIGN const uint8_t usb_num_endpoints = 8;

USB_ALIGN uint8_t ep1_buf_in[USB_EP1_SIZE];
USB_ALIGN uint8_t ep1_buf_out[USB_EP1_SIZE];

uint8_t toUARTbuffer[256];
uint8_t byteQueueToUSB = 0, toUSBbyteCount = 0, byteQueueToUART = 0, pktSize = 0;//, USBpktSize = 0;
uint8_t nextByte = 0;

//bool echoing = true;

void usb_cb_reset(void) {
	// nothing so far
}

void usb_cb_control_setup(void) {
	uint16_t len = usb_setup.wLength;
	if (usb_setup.bRequest == USB_REQ_GetMSDescriptor) {
		switch (usb_setup.wIndex) {
		  case 0x0004:
			if (len > msft_compatible.dwLength)
				len = msft_compatible.dwLength;
			memcpy(ep0_buf_in, &msft_compatible, len);
			break;
		  case 0x0005:
			if (len > msft_extended.dwLength)
				len = msft_extended.dwLength;
			memcpy(ep0_buf_in, &msft_extended, len);
			break;
		  default:
			return usb_ep0_stall();
		}
		usb_ep0_in(len);
		usb_ep0_out();
	}
	else
		usb_ep0_stall();
}

void usb_cb_control_in_completion(void) {

}

void usb_cb_control_out_completion(void) {

}

void usb_cb_completion(void) {
	// no int flag remaining
}

bool usb_cb_set_configuration(uint8_t config) {
	switch (config) {
	  case 0:
		usb_disable_ep(confDesc.endpOUT.bEndpointAddress);
		usb_disable_ep(confDesc.endpIN.bEndpointAddress);
		return true;
	  case 1:
		usb_enable_ep(confDesc.endpOUT.bEndpointAddress, confDesc.endpOUT.bmAttributes, confDesc.endpOUT.wMaxPacketSize);
		usb_enable_ep(confDesc.endpIN.bEndpointAddress, confDesc.endpIN.bmAttributes, confDesc.endpIN.wMaxPacketSize);
		usb_ep_start_out(confDesc.endpOUT.bEndpointAddress, ep1_buf_out, confDesc.endpOUT.wMaxPacketSize);
		return true;
	  default:
		return false;
	}
	return true;
}

bool usb_cb_set_interface(uint16_t interface, uint16_t altsetting) {
	return !(interface || altsetting);
}

uint16_t usb_cb_get_descriptor(uint8_t type, uint8_t index, const uint8_t** descriptor_ptr) {
	switch (type) {
	  case USB_DTYPE_Device:
		*descriptor_ptr = (uint8_t *)&devDesc;
		return ( index ? 0 : devDesc.bLength );
	  case USB_DTYPE_Configuration:
		*descriptor_ptr = (uint8_t *)&confDesc;
		return ( index ? 0 : confDesc.conf.wTotalLength );
	  case USB_DTYPE_String:
		switch (index) {
		  case 0:
			*descriptor_ptr = (uint8_t *)&langDesc;
			break;
		  case 1:
			*descriptor_ptr = usb_string_to_descriptor("ATMEL [Spark firmware]");
			break;
		  case 2:
			*descriptor_ptr = usb_string_to_descriptor("WinUSB test device");
			break;
		  case 3:
			*descriptor_ptr = samd_serial_number_string_descriptor();
			break;
		  case 4:
			*descriptor_ptr = usb_string_to_descriptor("Configuration string descriptor");
			break;
		  case 5:
			*descriptor_ptr = usb_string_to_descriptor("Interface string descriptor");
			break;
		  case MSFT_ID:
			*descriptor_ptr = (uint8_t *)&msft_os;
			break;
		  default:
			return 0;
		}
		return ((USB_StringDescriptor *)*descriptor_ptr)->bLength;
	  case USB_DTYPE_DeviceCapabilities:
	  default: // There is an unrecognized descriptor type request !
		return 0;
	}
}

//================================
fifo_t *buffer; // For storing incoming packets
volatile bool sending_data;

void usb_setup_fifos(void)
{
	buffer = createFifo(1024);
}

uint16_t usb_bytes_available(void)
{
	return elements(buffer);
}

void usb_get(char *point, uint16_t num_char)
{
	for(uint16_t i=0;i<num_char;i++)
		pop(buffer, &point[i]);
}

void usb_put(char *point, uint16_t num_char)
{
	sending_data = true;
	usb_ep_start_in(confDesc.endpIN.bEndpointAddress, point, num_char, true);
	//usb_ep_wait_for_ready(confDesc.endpIN.bEndpointAddress);
	while(sending_data == true);
}

// Handle incoming data
void usb_cb_handle_ep_out(uint8_t ep) {
	uint32_t size = usb_ep_out_length(ep);
	
	switch (ep) {		
		case 0x01:
			usb_ep_start_out(confDesc.endpOUT.bEndpointAddress, ep1_buf_out, confDesc.endpOUT.wMaxPacketSize);
			for(uint8_t i=0;i<size;i++)
				push(buffer, ep1_buf_out[i]);
				
			//usb_ep_start_in(confDesc.endpIN.bEndpointAddress, ep1_buf_out, size, true);
			return;
		
		default:
			return;
	}
}

// Typically don't do anything extra when we send data out
void usb_cb_handle_ep_in(uint8_t ep) {
	switch (ep) {
		case 0x01:
			byteQueueToUSB = 0;
			sending_data = false;
			return;
		
		default:
			return;
	}
}