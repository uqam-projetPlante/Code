//
//  audio.c
//
//
//  Created by Michiel Soer.
//
//

#include "hardware.h"
#include "radio.h"
#include "spark_v5.h"
#include "fifo_byte.h"
#include "WM8731.h" // Audio Codec
#include "fec_crc.h"

#define DO_UPKEEP

// Audio 
uint8_t audioDecimate;

#define FIFO_SIZE 4*1024
fifo_t *TXbuf, *RXbuf;
bool toggle;
bool mySEQ, myEXP, myEMPTY;
bool hisSEQ, hisEXP, hisEMPTY;
char lastMySEQ[32];
char lastMyEXP[32];
char lastHisSEQ[32];
char lastHisEXP[32];

unsigned long audioRXmissing;
unsigned long audioRXreceived;
unsigned long audioRXduplicate;
unsigned long audioRXrejected;
unsigned long audioTXsent;
unsigned long audioTXisempty;
unsigned long audioTXresent;

unsigned char upkeepBytes;

void updateSeqStats()
{
	char b = 'X';
	
	for(unsigned char i=0;i<30;i++)
	{
		lastMySEQ[i] = lastMySEQ[i+1];
		lastMyEXP[i] = lastMyEXP[i+1];
		lastHisSEQ[i] = lastHisSEQ[i+1];
		lastHisEXP[i] = lastHisEXP[i+1];
	}
	if(mySEQ)
		b = '1';
	else
		b = '0';
	lastMySEQ[30] =b;
	
	if(myEXP)
		b = '1';
	else
		b = '0';
	lastMyEXP[30] = b;
	
	if(hisSEQ)
		b = '1';
	else
		b = '0';
	lastHisSEQ[30] = b;
	
	if(hisEXP)
		b = '1';
	else
		b = '0';
	lastHisEXP[30] = b;
}

void audioResetStatistics(void)
{
	audioRXmissing = 0;
	audioRXreceived = 0; 
	audioRXduplicate = 0;
	audioRXrejected = 0;
	audioTXsent = 0;
	audioTXresent = 0;
	audioTXisempty = 0;
	
	lastMySEQ[ 31] = 0;
	lastMyEXP[31] = 0;
	lastHisSEQ[31] = 0;
	lastHisEXP[31] = 0;
}

void audioPrintStatistics(void)
{
	usbPrintf("Audio TX sent / resent / isempty  = %d / %d / %d \n", audioTXsent, audioTXresent, audioTXisempty);
	usbPrintf("Audio RX  received / duplicate / missing / rejected = %d / %d / %d / %d\n", audioRXreceived, audioRXduplicate, audioRXmissing, audioRXrejected);
	/*
	usbPrintf("Audio mySEQ:  %s\n", lastMySEQ);
	usbPrintf("Audio myEXP:  %s\n", lastMyEXP);
	usbPrintf("Audio HisSEQ: %s\n", lastHisSEQ);
	usbPrintf("Audio HisEXP: %s\n", lastHisEXP);*/
	usbPrintf("RX buf %d, TX buf %d\n", elements(RXbuf), elements(TXbuf));
}

void audioStart(void)
{
	WM8731_Activate();
	upkeepBytes = 0;
}

void audioStop(void)
{
	WM8731_Deactivate();
}

bool fill;
bool newPacket;

void audioReset(void)
{
	// Empty FIFOs?
	
	mySEQ = true; // Push the first packet in
	myEXP = hisSEQ = hisEXP = false;
	
	upkeepBytes = 0;
	fill = false;
	newPacket = false;
}

void audioConfigure(void)
{
	WM8731_Init();
	
	TXbuf = createFifo(sizeof(uint16_t)*FIFO_SIZE);
	RXbuf = createFifo(sizeof(uint16_t)*FIFO_SIZE);
	toggle = false;
	
	audioReset();
}



bool audioProcessPacket(unsigned char numBytes)
{
	uint16_t w;
	uint8_t MAC;
	newPacket = false;	
	 
	
	
	//updateSeqStats();
	
	// Is there a packet?
	if(numBytes < 1)
	{
		audioRXmissing++;
		upkeepBytes = 0;	
		newPacket = false;
		return false;
	}
	
#ifdef DO_UPKEEP		
	sparkPullRX(0, 4);
	upkeepBytes = numBytes;
#else
	sparkPullRX(0, numBytes);
#endif
	
	//sparkPullRX(0, 4); // Get header
	//upkeepBytes = numBytes; // Get the rest later
	
	// Header
	MAC = sparkRXbuffer[1];	
	
	/*
	uint16_t CRC = 0xFFFF;
	CRC = calcCRC(MAC, CRC);
	CRC = calcCRC(sparkRXbuffer[2], CRC);
	CRC = calcCRC(sparkRXbuffer[3], CRC);*/
			
	//if(CRC != 0x0000)
	if(crc8_table[MAC] != sparkRXbuffer[2]) // Check CRC of header 
	{
		audioRXrejected++;	
		newPacket = false;	
		return true;
	}
	
	/*
	if(sparkRXbuffer[2] != 0xd5) // Identifier
	{
		audioRXrejected++;
		return true;
	}
	MAC = sparkRXbuffer[3];	*/
	
	// Process header
	hisSEQ = (MAC & 0x01) != 0x00;
	hisEXP = (MAC & 0x02) != 0x00;
	hisEMPTY = MAC & 0x10;
	
	if(!hisEMPTY)
	{
		if(sparkCRCfail)
		{
			audioRXrejected++;	
			newPacket = false;		
			return true;
		}
	}
	
	if( (myEXP && hisSEQ) || (!myEXP && !hisSEQ))
	{
		newPacket = true;
		myEXP = !myEXP;
	}
	
	
	// New Data
	if(!RXbuf || !newPacket)
	{
		audioRXduplicate++;	
		newPacket = false;		
		return true;
	}
	
#ifndef DO_UPKEEP	
	if(!hisEMPTY)
	{
		
		REG_PORT_OUTCLR1 = PORT_PB09;
		if(elements(RXbuf)>(FIFO_SIZE-200))
			fill = false;
		if(elements(RXbuf)<(FIFO_SIZE/2))
			fill = true;		
		
		if(fill)
		{
			
			// Size of uint16_t transfer is (numBytes-4)/2
			uint16_t remaining = (FIFO_SIZE-RXbuf->head);
			uint16_t transfer = (numBytes-6) >> 1; 
			
			// Memcpy
			if(transfer <= remaining)
			{
				memcpy(&RXbuf->buffer[RXbuf->head], &sparkRXbuffer[4], transfer << 1);
				
				//RXbuf->head = RXbuf->head+transfer;
			}
			else
			{
				// To possible end of FIFO
				memcpy(&RXbuf->buffer[RXbuf->head], &sparkRXbuffer[4], remaining << 1);
				// From possible start of FIFO
				memcpy(&RXbuf->buffer[0], &sparkRXbuffer[4+(remaining << 1)], (transfer - remaining) << 1);
				
				//RXbuf->head = RXbuf->head+transfer-RXbuf->size;
			}
			RXbuf->head = (RXbuf->head+transfer)%RXbuf->size;
			
			// Update pointer
			/*
			RXbuf->head = RXbuf->head+transfer;
			if(RXbuf->head > (RXbuf->size-1))
				RXbuf->head -= RXbuf->size;*/
		}
		
		
		//for(unsigned char i=4;i<numBytes-2;i+=2)
		//{
			//w = (((uint16_t)sparkRXbuffer[i+1])<<8) | ((uint16_t)sparkRXbuffer[i]);
			////w = (((uint16_t)sparkRXbuffer[i])<<8) | ((uint16_t)sparkRXbuffer[i+1]);
			//if(fill)
				//push(RXbuf, w);
		//}
		REG_PORT_OUTSET1 = PORT_PB09;
	}
#endif
	
	audioRXreceived++;	
	
	
	return true;
}

void audioUpkeepPacket(void)
{
#ifdef DO_UPKEEP	
	unsigned char numBytes = upkeepBytes;
	// Fish the remaining bytes out
	
	if(numBytes == 0)
		return;
		
	sparkPullRX(4, numBytes);
	
	if(!hisEMPTY && newPacket)
	{
		
		REG_PORT_OUTCLR1 = PORT_PB09;
		if(elements(RXbuf)>(FIFO_SIZE-200))
		fill = false;
		if(elements(RXbuf)<(FIFO_SIZE/2))
		fill = true;
		
		if(fill)
		{
			
			// Size of uint16_t transfer is (numBytes-4)/2
			uint16_t remaining = (FIFO_SIZE-RXbuf->head);
			uint16_t transfer = (numBytes-6) >> 1;
			
			// Memcpy
			if(transfer <= remaining)
			{
				memcpy(&RXbuf->buffer[RXbuf->head], &sparkRXbuffer[4], transfer << 1);
				//RXbuf->head = RXbuf->head+transfer;
			}
			else
			{
				// To possible end of FIFO
				memcpy(&RXbuf->buffer[RXbuf->head], &sparkRXbuffer[4], remaining << 1);
				// From possible start of FIFO
				memcpy(&RXbuf->buffer[0], &sparkRXbuffer[4+(remaining << 1)], (transfer - remaining) << 1);
				
				//RXbuf->head = RXbuf->head+transfer-RXbuf->size;
			}
			RXbuf->head = (RXbuf->head+transfer)%RXbuf->size;
			
			// Update pointer
			/*
			RXbuf->head = RXbuf->head+transfer;
			if(RXbuf->head > (RXbuf->size-1))
			RXbuf->head -= RXbuf->size;*/
		}
		
		
		//for(unsigned char i=4;i<numBytes-2;i+=2)
		//{
		//w = (((uint16_t)sparkRXbuffer[i+1])<<8) | ((uint16_t)sparkRXbuffer[i]);
		////w = (((uint16_t)sparkRXbuffer[i])<<8) | ((uint16_t)sparkRXbuffer[i+1]);
		//if(fill)
		//push(RXbuf, w);
		//}
		REG_PORT_OUTSET1 = PORT_PB09;
	}
#endif	
}

void audioFillPacket(unsigned char numBytes)
{
	uint16_t w;
	uint8_t MAC = 0x00;
	bool newPacket = false;
	
	//updateSeqStats();
	
	// Can we fill?
	/*
	if(elements(TXbuf)<numBytes)
		myEMPTY = true;
	else
		myEMPTY = false;*/
		
	// Process header
	if( (hisEXP && !mySEQ) || (!hisEXP && mySEQ))
	{
		//if(!myEMPTY)
		mySEQ = !mySEQ; // Next packet
		newPacket = true;
	}			
	if(mySEQ)
		MAC |= 0x01;
	if(myEXP)
		MAC |= 0x02;	
	if(newPacket)// && myEMPTY)
	{
		// Can we fill?
		if(elements(TXbuf)<numBytes)
			myEMPTY = true;
		else
			myEMPTY = false;				
	}
	if(myEMPTY)
		MAC |= 0x10;
	
	
	
	// Header
	sparkTXbuffer[0] = 0xFF; // Spacer
	sparkTXbuffer[1] = MAC; // MAC	
	
	/*
	uint16_t CRC = 0xFFFF;
	CRC = calcCRC(MAC, CRC);
	
	sparkTXbuffer[2] = CRC >> 8; // MSB part of CRC
	sparkTXbuffer[3] = CRC & 0x00FF; // LSB part of CRC*/
	sparkTXbuffer[2] = crc8_table[MAC]; // Header CRC
	sparkTXbuffer[3] = 0x00; // Unused for now
	
	// New Data
	if(!TXbuf || !newPacket)
	{
		audioTXresent++;
		
		sparkPushTX(0, numBytes);
		return;
	}	
	audioTXsent++;
	
	if(myEMPTY)
	{	
		
		for(unsigned char i=4;i<numBytes;i++)
			sparkTXbuffer[i] = 0x7F;		
			
		audioTXisempty++;
		
		sparkPushTX(0, numBytes);
		return;
	}
	else
	{	 
		REG_PORT_OUTCLR1 = PORT_PB09;
		
		
		// Check for size!	
		
		// Size of uint16_t transfer is (numBytes-4)/2
		uint16_t remaining = (FIFO_SIZE-TXbuf->tail);
		uint16_t transfer = (numBytes-6) >> 1;
		
		// Memcpy
		if(transfer <= remaining)
		{
			memcpy(&sparkTXbuffer[4], &TXbuf->buffer[TXbuf->tail], transfer << 1);
			//TXbuf->tail = (TXbuf->tail+transfer)%TXbuf->size;
		}
		else
		{
			// To possible end of FIFO
			memcpy(&sparkTXbuffer[4], &TXbuf->buffer[TXbuf->tail], remaining << 1);
			// From possible start of FIFO
			memcpy(&sparkTXbuffer[4+(remaining << 1)], &TXbuf->buffer[0], (transfer - remaining) << 1);
			//TXbuf->tail = TXbuf->tail+transfer-TXbuf->size;
		}
		TXbuf->tail = (TXbuf->tail+transfer)%TXbuf->size;
		
		// Update pointer
		/*
		TXbuf->tail = TXbuf->tail+transfer;
		if(TXbuf->tail > (TXbuf->size-1))
			TXbuf->tail -= TXbuf->size;	*/
		
		/*
		for(unsigned char i=4;i<numBytes-2;i+=2)
		{		
			pop(TXbuf, &w);
			sparkTXbuffer[i+1] = (uint8_t)(w >> 8);
			sparkTXbuffer[i] = (uint8_t)(w & 0x00ff);
			//sparkTXbuffer[i] = (uint8_t)(w >> 8);
			//sparkTXbuffer[i+1] = (uint8_t)(w & 0x00ff);
			
			//sparkTXbuffer[i+1] = 0xF0;
			//sparkTXbuffer[i] = 0x00;
		}*/
		
		REG_PORT_OUTSET1 = PORT_PB09;
		
	}
	
	sparkPushTX(0, numBytes);
}

uint8_t count;
uint16_t filter[8];
uint16_t last_sample;
bool playback;

//#define DECIMATE 6

uint32_t callbackAudioSample(uint32_t in_sample)
{
	uint32_t out_sample;// = last_sample; // Loop through
	uint16_t b;
	
	count = (count+1) % audioDecimate;
	
	filter[count] = (in_sample & 0x0000FFFF);
	
	if(TXbuf && count == 0)
	{
		push(TXbuf, filter[0]); // Add average filtering to remove antialias
		//push(TXbuf, (((uint32_t)filter[0])+((uint32_t)filter[1])+((uint32_t)filter[2]))/3);
		//push(TXbuf, in_sample & 0x0000FFFF); // Take left channel
	}
	
	if(RXbuf && count == 0)
	{
		if(isEmpty(RXbuf))
			playback = false;
		else if(elements(RXbuf)>(FIFO_SIZE/2))
			playback = true;		
			
		if(!playback)
		{
			last_sample = 0x7F;
			
		}
		else
		{
			pop(RXbuf, &b);
			//if(b == 0x00) // Forbidden value
				//b = 0x7F;
			//last_sample = 0x0F;
			last_sample = b;
				
		}		
	}	
	
	//last_sample = filter[0];
	out_sample = ((uint32_t) last_sample) << 16 | last_sample; // Duplicate channel		

	return out_sample;
}



