//
//  application.cpp
//  
//
//  Created by Michiel Soer.
//
//

#include "hardware.h"
#include "radio.h"
#include "spark_v5.h"
#include "audio.h"
#include "flash.h"

// Lookup table for printing binary numbers
const char *bit_rep[16] = {
  [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
  [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
  [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
  [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
};

const char nibble_hex[16] = {
	[ 0] = '0', [ 1] = '1', [ 2] = '2', [ 3] = '3',
	[ 4] = '4', [ 5] = '5', [ 6] = '6', [ 7] = '7',
	[ 8] = '8', [ 9] = '9', [10] = 'a', [11] = 'b',
	[12] = 'c', [13] = 'd', [14] = 'e', [15] = 'f',
};


// Link setup
unsigned short linkPacketSize; // Bits
unsigned short linkPacketRate; // packets/s
unsigned char  linkSleepMode;
unsigned short linkSyncWord;
unsigned short linkPreambleSize;
unsigned short linkMinNoiLvl;
unsigned short linkMinPulAmp;
unsigned short linkMinSigStr;
bool linkBidirectional;
bool linkMaster;
bool linkRandom;
bool linkAudio;
uint8_t linkTXMask[12];

bool BISTcompleted;
bool runGUI = true;

#define FLASH_SIZE 64 // 64 bytes is one page, must write a whole page at a time
uint8_t flash_mem[FLASH_SIZE];
const uint32_t flash_addr = 0x00030600;

void flashWrite()
{
	uint8_t p = 0;
	for(uint8_t i=0;i<FLASH_SIZE;i++)
		flash_mem[i] = 0;
	flash_mem[p] = 0xcd; p++;
	
	// Put configuration
	flash_mem[p] = linkPacketSize >> 8; p++;
	flash_mem[p] = linkPacketSize & 0xFF; p++;
	flash_mem[p] = linkPacketRate >> 8; p++;
	flash_mem[p] = linkPacketRate & 0xFF; p++;
	flash_mem[p] = linkSleepMode; p++;
	flash_mem[p] = linkSyncWord >> 8; p++;
	flash_mem[p] = linkSyncWord & 0xFF; p++;
	flash_mem[p] = linkBidirectional; p++;
	flash_mem[p] = linkMaster; p++;
	flash_mem[p] = linkRandom; p++;
	flash_mem[p] = linkAudio; p++;
	for(uint8_t i=0;i<12;i++)
		flash_mem[p++] = linkTXMask[i];
	flash_mem[p] = audioDecimate; p++;	
	
	
	// Write
	flash_erase_row(flash_addr);
	flash_write_buffer(flash_addr, flash_mem, FLASH_SIZE);
}

void flashRead()
{
	uint8_t p = 0;
	
	// Read
	flash_read_buffer(flash_addr, flash_mem, FLASH_SIZE);
	/*
	for(uint8_t i=0;i<FLASH_SIZE;i++)
	usbPrintf("%02hx ", flash_mem[i]);
	usbPrintf("\n");*/
	
	// Get configuration
	if(flash_mem[p++]!= 0xcd) // header
		return;
	
	linkPacketSize = flash_mem[p++]; linkPacketSize <<= 8;
	linkPacketSize |= flash_mem[p++];
	linkPacketRate = flash_mem[p++]; linkPacketRate <<= 8;
	linkPacketRate |= flash_mem[p++];
	linkSleepMode = flash_mem[p++];
	linkSyncWord = flash_mem[p++]; linkSyncWord <<= 8;
	linkSyncWord |= flash_mem[p++];
	linkBidirectional = flash_mem[p++];
	linkMaster = flash_mem[p++];
	linkRandom = flash_mem[p++];
	linkAudio = flash_mem[p++];	
	for(uint8_t i=0;i<12;i++)
		linkTXMask[i] = flash_mem[p++];
	audioDecimate = flash_mem[p++];	
}

void defaultLinkSettings()
{
	flash_init();
	
	linkPacketSize = 240;
	linkPacketRate = 2000;
	linkSleepMode = SPARK_SLEEP_SHALLOW;
	linkSyncWord = 0xc8b5;
	linkPreambleSize = 0x0f;
	linkMinNoiLvl = 0x11;
	linkMinPulAmp = 0x2a;
	linkMinSigStr = 0x12;
	linkBidirectional = true;
	linkMaster =  false;
	linkRandom = false;
	linkAudio = false;
	
	linkTXMask[0] = 0x00;
	linkTXMask[1] = 0x00;
	linkTXMask[2] = 0xbd;
	linkTXMask[3] = 0xbb;
	linkTXMask[4] = 0xb9;
	linkTXMask[5] = 0xb7;
	linkTXMask[6] = 0xb5;
	linkTXMask[7] = 0xb3;
	linkTXMask[8] = 0x00;
	linkTXMask[9] = 0x00;
	linkTXMask[10] = 0x00;
	linkTXMask[11] = 0x00;
	
	audioDecimate = 6;
	
	// Try to read configuration
	flashRead();
}

void configureLink()
{
	linkPacketSize = linkPacketSize & 0xFFF0; // Multiple of 2 bytes
	
	// Packet setup
	sparkPacketSetup(linkMinNoiLvl, linkMinPulAmp, linkMinSigStr, linkPreambleSize, linkPacketSize/8, linkSyncWord, linkRandom); // preamble, packet size, sync word, gen random
	
	// Setup sleep and wakeup cycle
	sparkSleepSetup(linkSleepMode, linkPacketRate, 100); // sleep depth, packets/s, framelength in us
	
	// Link Mode
	sparkSetBidirectional(linkBidirectional, linkMaster);  
	
	// TX spectrum
	sparkSpectralSetup(linkTXMask);
}

// Statistics 
unsigned long statSendPackets;
unsigned long statReceivedPackets;
unsigned long statMissingPackets;
unsigned long statNumErrors;
unsigned long statBERdist[31];
unsigned char statLastRSSI;
unsigned char statLastNSSI;
unsigned int  statLinkLost;
unsigned int  statException;

void resetStatistics()
{
  statSendPackets = 0;
  statReceivedPackets = 0;
  statMissingPackets = 0;
  statNumErrors = 0;
  for(unsigned char i=0;i<31;i++)
    statBERdist[i] = 0;
  statLastRSSI = 0;
  statLastNSSI = 0;
  statLinkLost = 0;
  statException = 0;
  
  audioResetStatistics();
}

void printStatistics()
{   
  // Tell mode
  if(sparkIsBidirectional())
  {
	  if(sparkIsMaster())
		usbPrintf("Bidirectional Master\n");
	  else if(sparkIsScanning())
		usbPrintf("Bidirectional Slave: Scanning\n");
	  else
		usbPrintf("Bidirectional Slave: Locked\n");
  }
  else
  { 
  	  if(sparkGetMode() == SPARK_MODE_RX)
	  {
		if(sparkIsScanning())
		  usbPrintf("RX Mode: scanning\n");
		else
		  usbPrintf("RX Mode: locked\n");
	  }
	  else
		usbPrintf("TX Mode\n");
  }
    
  usbPrintf("statException = %d\n", statException);
  usbPrintf("sparkLastInt = %d\n", sparkLastInt);
  usbPrintf("statLinkLost = %d\n", statLinkLost);
  usbPrintf("statSendPackets = %d\n", statSendPackets);
  usbPrintf("statReceivedPackets = %d\n", statReceivedPackets);
  usbPrintf("statMissingPackets = %d\n", statMissingPackets);
  usbPrintf("statNumErrors = %d\n", statNumErrors);
  usbPrintf("=> PER = 1 in %d\n", (statReceivedPackets+statMissingPackets)/statMissingPackets);
  usbPrintf("=> BER = 1 in %d\n",(statReceivedPackets*linkPacketSize)/statNumErrors);
  
  usbPrintf("0 errors: %d packets (%d%%)\n", statBERdist[0], (statBERdist[0]*100)/statReceivedPackets);
  usbPrintf("1 errors: %d packets (%d%%)\n", statBERdist[1], (statBERdist[1]*100)/statReceivedPackets);
  usbPrintf("2 errors: %d packets (%d%%)\n", statBERdist[2], (statBERdist[2]*100)/statReceivedPackets);
  usbPrintf("3-4 errors: %d packets (%d%%)\n", statBERdist[3], (statBERdist[3]*100)/statReceivedPackets);
  usbPrintf("5-8 errors: %d packets (%d%%)\n", statBERdist[4], (statBERdist[4]*100)/statReceivedPackets);
  usbPrintf("9-15 errors: %d packets (%d%%)\n", statBERdist[5], (statBERdist[5]*100)/statReceivedPackets);
  usbPrintf("16-31 errors: %d packets (%d%%)\n", statBERdist[6], (statBERdist[6]*100)/statReceivedPackets);
  usbPrintf(">=32 errors: %d packets (%d%%)\n", statBERdist[7], (statBERdist[7]*100)/statReceivedPackets);
  
  uint16_t p;
  
  p=0;
  std_buf[p] = 'T'; p++;
  std_buf[p] = 'X'; p++;
  std_buf[p] = ' '; p++;
  for(unsigned char i=0;i<linkPacketSize/8;i++)
  {
	  std_buf[p] = nibble_hex[sparkTXbuffer[i] >> 4]; p++;
	  std_buf[p] = nibble_hex[sparkTXbuffer[i] & 0x0F]; p++;
	  std_buf[p] = ' '; p++;
	  
	  if(p > 55)
	  {
		 // std_buf[p] = '\n'; p++;
		  std_buf[p] = 0; p++;
		  usbPrintf(std_buf);
		  p = 0;
	  }
  } 
  usbPrintf("\n"); 
  
  p=0;
  std_buf[p] = 'R'; p++;
  std_buf[p] = 'X'; p++;
  std_buf[p] = ' '; p++;
  for(unsigned char i=0;i<linkPacketSize/8;i++)
  {
	  std_buf[p] = nibble_hex[sparkRXbuffer[i] >> 4]; p++;
	  std_buf[p] = nibble_hex[sparkRXbuffer[i] & 0x0F]; p++;
	  std_buf[p] = ' '; p++;
	  
	  if(p > 55)
	  {
		  //std_buf[p] = '\n'; p++;
		  std_buf[p] = 0; p++;
		  usbPrintf(std_buf);
		  p = 0;
	  }
  }
  usbPrintf("\n");
  
  /*
  for(unsigned char i=0;i<linkPacketSize/8;i++)
    usbPrintf("%02hx ", sparkTXbuffer[i]);
  usbPrintf("\n");
  for(unsigned char i=0;i<linkPacketSize/8;i++)
  usbPrintf("%02hx ", sparkRXbuffer[i]);
  usbPrintf("\n");*/
  
  usbPrintf("statLastRSSI: %d\n", statLastRSSI);
  usbPrintf("statLastNSSI: %d\n", statLastNSSI);
  usbPrintf("statLastRXWaited: %d\n", sparkPacketRXWaited);
  usbPrintf("lastLinkMargin: %d dB\n", (statLastNSSI-statLastRSSI)/2);
  
  if(linkAudio)
	audioPrintStatistics();
}
 
void publishStats(void)
{
	usbPrintf("%d %d %d %d %d %d %d\n", statSendPackets, statReceivedPackets, statMissingPackets, statNumErrors, statLastRSSI, statLastNSSI, sparkPacketRXWaited);
}

// Compare two bytes and return number of bit errors (0-8)
inline uint8_t byteErrors(uint8_t b1, uint8_t b2)
{
  uint8_t errors = 0;
  
  if( (b1 & 1) != (b2 & 1) )
    errors++;
  if( (b1 & 2) != (b2 & 2) )
    errors++;
  if( (b1 & 4) != (b2 & 4) )
    errors++;
  if( (b1 & 8) != (b2 & 8) )
    errors++;
  if( (b1 & 16) != (b2 & 16) )
    errors++;
  if( (b1 & 32) != (b2 & 32) )
    errors++;
  if( (b1 & 64) != (b2 & 64) )
    errors++;
  if( (b1 & 128) != (b2 & 128) )
    errors++;
  
  return errors;
}

// Callbacks activated by Spark driver
void callbackPacketSend(unsigned char numBytes)
{
  // Fill packet
  if(linkAudio)
  {
	audioFillPacket(numBytes);
  }
  else if(!linkRandom)
  {
    for(unsigned char i=0;i<numBytes;i++)
      sparkTXbuffer[i] = i; // Dummy data
	  
	sparkPushTX(0, numBytes);
  }
    
  statSendPackets++;
}

bool callbackPacketReceived(unsigned char numBytes) // Return true if packet was succesfully received
{
  unsigned char errors = 0;
  uint8_t b;
  bool reception = false; 
 
  
  if(linkAudio)
  {
	  reception = audioProcessPacket(numBytes);
	  errors = 0;
  }
  else
  {
	  sparkPullRX(0, numBytes);
	  
	  // Get packet?
	  for(unsigned char i=0;i<numBytes;i++)
	  {
		b = sparkRXbuffer[i];
		if(linkRandom)
		  errors = b;
		else
		  errors += byteErrors(b, i); // This takes a lot of time
    
		reception = true;
	  }
  }

  // Statistics
  if(!reception)
  {
    statMissingPackets++;
  }
  else
  {
    statReceivedPackets++;
    statNumErrors += errors;
    
	/*
    if(errors == 0)
      statBERdist[0]++;
    else if(errors <= 1)
      statBERdist[1]++;
    else if(errors <= 2)
      statBERdist[2]++;
    else if(errors <= 4)
      statBERdist[3]++;
    else if(errors <= 8)
      statBERdist[4]++;
    else if(errors <= 16)
      statBERdist[5]++;
    else if(errors <= 32)
      statBERdist[6]++;
    else
      statBERdist[7]++;
	  */
	if(errors >= 30)
		errors = 30;
	statBERdist[errors]++;
      
    statLastRSSI = sparkPacketRSSI;
    statLastNSSI = sparkPacketNSSI;
  }
   
  
  return reception;
}

void callBackPacketUpkeep(void)
{
	if(linkAudio)
		audioUpkeepPacket();
}

void callbackLinkLost(void)
{
  statLinkLost++;
}

void callbackException(void)
{
  statException++;
}

// Application layer
void appStart(void)
{
	if(linkAudio)
		audioStart();
	audioReset();
	sparkStartRadio();
	resetStatistics();
}

void appStop(void)
{	
	sparkStopRadio();
	if(linkAudio)
		audioStop();
}

void appInit(void)
{
  // Test Radio
  /*
  BISTcompleted = sparkBIST(false);
  if(!BISTcompleted)
    return;*/
  
  // Setup radio
  sparkRadioSetup();
  
  // Setup link parameters  
  defaultLinkSettings();
  configureLink();
  
  // Setup Audio
  if(linkAudio)
    audioConfigure();
  
  // Start
  appStart();
}

// GUI interface
uint8_t in_buf[128];
uint8_t out_buf[128];

void guiSendBER(void) // Send BER distribution
{
	uint8_t b;
	uint16_t p = 0; // Pointer of packet
	
	// Header
	out_buf[p] = 0x04; p++;
	
	// Byte #1 (MSB), byte #2, byte #3 (LSB), total number of errors
	out_buf[p] = (statNumErrors >> 16) & 0xFF; p++;
	out_buf[p] = (statNumErrors >> 8) & 0xFF; p++;
	out_buf[p] = (statNumErrors >> 0) & 0xFF; p++;
	
	// Byte #4 and byte #5 and byte #6
	out_buf[p++] = statLastRSSI;
	out_buf[p++] = statLastNSSI;
	out_buf[p++] =(statLastNSSI-statLastRSSI)/2;
	
	// Error distribution, 2 bytes per bin (MSB first), 0 errors first
	// ! increase number of bins?
	out_buf[p] = statSendPackets >> 8; p++;
	out_buf[p] = statSendPackets & 0xFF; p++;
	for(uint8_t i=0;i<31;i++)
	{
		out_buf[p] = statBERdist[i] >> 8; p++;
		out_buf[p] = statBERdist[i] & 0xFF; p++;
	}	
	out_buf[p] = statMissingPackets >> 8; p++;
	out_buf[p] = statMissingPackets & 0xFF; p++;
	
	// Send the packet
	usb_put(out_buf, p);
}

void guiSendAudio(void) // Send BER distribution
{
	uint8_t b;
	uint16_t p = 0; // Pointer of packet
	
	// Header
	out_buf[p] = 0x05; p++;
	
	// Send the packet
	usb_put(out_buf, p);
}

void guiSendConfig(void) // Sends the configuration package
{
	uint8_t b;
	uint16_t p = 0; // Pointer of packet
	
	// Header
	out_buf[p] = 0x03; p++;
	
	// Byte #0, bit 1-0 tells application: BER (0b00) or audio (0b01)
	b = 0;
	if(linkBidirectional)
	{
		if(linkMaster)
			b |= 0x03;
		else
			b |= 0x02;	
	}
	else
	{
		if(linkMaster)
			b |= 0x00;
		else
			b |= 0x01;
	}
	b <<= 2;
	//if(linkMaster)
	//	b |= 1 << 3;
	if(linkAudio)
		b |= 1 << 0;
	if(linkRandom)
		b |= 1 << 5;
	if(sparkIsRunning())
		b |= 1 << 7;
	out_buf[p] = b; p++;
	
	// Byte #1 (MSB), byte #2 (LSB), TX address, not implemented
	out_buf[p] = 0; p++;
	out_buf[p] = 0; p++;
	
	// Byte #3 (MSB), byte #4 (LSB), RX address, not implemented
	out_buf[p] = 0; p++;
	out_buf[p] = 0; p++;
	
	// Byte #5, packet payload size in bytes
	out_buf[p] = linkPacketSize/8; p++;
	
	// Byte #6 (MSB), byte #7 (LSB), packet period in multiples of 100us
	uint16_t period = 10000 / linkPacketRate; // linkPacketRate is packets/s
	out_buf[p] = period >> 8; p++;
	out_buf[p] = period & 0x00FF; p++;
	
	// Byte #8 (MSB), byte #9 (LSB), packet period in multiples of 100us
	//uint16_t period = 10000 / linkPacketRate; // linkPacketRate is packets/s
	out_buf[p] = period >> 8; p++;
	out_buf[p] = period & 0x00FF; p++;
	
	// Byte #10, parameter bitset, not implemented
	out_buf[p] = 0; p++;
	
	// Byte #11, not implemented
	out_buf[p] = 0; p++;
	
	// Send the packet
	usb_put(out_buf, p);
}

void guiReceiveConfig(void) // Changes configuration, reads from in_buf
{
	uint8_t b;
	uint16_t p = 1; // Pointer of packet, start from 1 because of header	
	
	//appStop();	
	
	// Byte #0, , bit 1-0 tells application: BER (0b00) or audio (0b01)
	b = in_buf[p]; p++;
	switch( (b >> 2) & 0x03)
	{
		case 0x00:
			linkBidirectional = false;
			linkMaster = true;
			break;
		case 0x01:
			linkBidirectional = false;
			linkMaster = false;
			break;
		case 0x02:
			linkBidirectional = true;
			linkMaster = false;
			break;
		case 0x03:
			linkBidirectional = true;
			linkMaster = true;
			break;
	}
	//linkMaster = b & (1<<3);
	linkAudio = b & (1<<0);
	linkRandom = b & (1<<5);
	
	// Byte #1 (MSB), byte #2 (LSB), TX address, not implemented
	b = in_buf[p]; p++;
	b = in_buf[p]; p++;
	
	// Byte #3 (MSB), byte #4 (LSB), RX address, not implemented
	b = in_buf[p]; p++;
	b = in_buf[p]; p++;
	
	// Byte #5, packet payload size in bytes
	b = in_buf[p]; p++;
	linkPacketSize = ((uint16_t)b) * 8l;
	
	// Byte #6 (MSB), byte #7 (LSB), packet period in multiples of 100us
	uint16_t period;
	b = in_buf[p]; p++;
	period = b; period << 8;
	b = in_buf[p]; p++;
	period |= b;
	linkPacketRate = 10000 / period;
	
	// Byte #8 (MSB), byte #9 (LSB), packet period in multiples of 100us
	b = in_buf[p]; p++;
	b = in_buf[p]; p++;
	
	// Byte #10, parameter bitset, not implemented
	b = in_buf[p]; p++;
	
	// Byte #11, not implemented
	b = in_buf[p]; p++;
	
	configureLink();
	//appStart();
}

void guiHandle(void)
{
	uint16_t size = usbBytesAvailable();	
	usb_get(in_buf, size);	
	
	// Read first byte for command
	switch(in_buf[0])
	{
		case 'i': // Request configuration
			guiSendConfig();
			break;
			
		case 'w': // Write configuration
			guiReceiveConfig();
			guiSendConfig();
			break;
			
		case 'r': 
			resetStatistics();
			break;
			
		case '+': // Start radio
			appStart();			
			break;
			
		case '-': // Stop radio
			appStop();
			break;
			
		case '~': // Request BER data
			if(linkAudio)
				guiSendAudio();
			else
				guiSendBER();
			break;
			
		case 0x05: // Request Audio data
			//guiSendAudio();
			break;
			
		case 0x06:
			break;
		
		case 0x07:
			break;
			 
		case 'G':
			runGUI = false;
			usbPrintf("Verbose mode\n");
			break;
			
		default:
			guiSendConfig();
			break;
	}	
}

void handleUSB(void)
{
  char command;
  uint8_t w;
  uint16_t b,r;
  
  
  // Loop through input
  while(usbIsAvailable()){
	
	if(runGUI)
	{
		guiHandle();
		continue;
	}
	
    command = usbGet(); // First command character
    
    
    switch(command){
		
	  case  'G': // Gabriel entry point for GUI, byte code 0x47
		  runGUI = true;
		  break;
      
      case 'i': // Identity            
          usbPrintf("==Eval Kit==\n");
          if(!BISTcompleted)
            usbPrintf("BIST failed\n");
          usbPrintf("linkPacketSize = %d\n", linkPacketSize);
          usbPrintf("linkPacketRate = %d\n", linkPacketRate);
          usbPrintf("linkSleepMode = p%d\n", linkSleepMode);
		  usbPrintf("linkSyncWord = 0x%04hx\n", linkSyncWord);
		  usbPrintf("linkPreambleSize = 0x%02hx\n", linkPreambleSize);
		  usbPrintf("linkMinNoiLvl = 0x%02hx\n", linkMinNoiLvl);
		  usbPrintf("linkMinPulAmp = 0x%02hx\n", linkMinPulAmp);
		  usbPrintf("linkMinSigStr = 0x%02hx\n", linkMinSigStr);
		  usbPrintf("linkBidirectional = %d\n", linkBidirectional);
		  usbPrintf("linkMaster = %d\n", linkMaster);
		  usbPrintf("linkRandom = %d\n", linkRandom);
		  usbPrintf("linkAudio = %d\n", linkAudio);
		  usbPrintf("linkMask = %02hx%02hx%02hx%02hx%02hx%02hx%02hx%02hx%02hx%02hx%02hx%02hx\n", linkTXMask[0],linkTXMask[1],linkTXMask[2],linkTXMask[3], linkTXMask[4],linkTXMask[5],linkTXMask[6],linkTXMask[7], linkTXMask[8],linkTXMask[9],linkTXMask[10],linkTXMask[11] );
		  usbPrintf("audioDecimate = %d\n", audioDecimate);
		  /*
          usbPrintf("sparkMode = "); 
          if(sparkGetMode() == SPARK_MODE_RX)
            usbPrintf("SPARK_MODE_RX\n");
          else
            usbPrintf("SPARK_MODE_TX\n");*/
          break;  
		  
		case 'L': // Toggle on-board LED
			toggleLED();
			break;

        case '?':
          usbPrintf("Running BIST..\n");
          //sparkStopRadio();
          //boardSetup();
          BISTcompleted = sparkBIST(true);
          usbPrintf("done\n");
          break;
		  
		case '~':
			publishStats();
			break;

        case 'l': // Packet rate
          linkPacketRate = usbParseInt();
          usbPrintf("linkPacketRate = %d\n", linkPacketRate);
          appStop();
          configureLink();
          appStart();
          break;
		  
		case 'J':
			disableClockGate();
			break;
		
		case 'D':
		  audioDecimate = usbParseInt();
		  break;

        case 'w': // Packet size
          linkPacketSize = (usbParseInt()/8)*8; 
          usbPrintf("linkPacketSize = %d\n", linkPacketSize);
          appStop();
          configureLink();
          appStart();
          break;

        case 'c': // calibrate DLL and VCRO
		 appStop();
          b = usbParseInt();
          w = sparkCalDLL(true);
          usbPrintf("sparkCalDLL(RX) = %02hx\n", w);
          w = sparkCalDLL(false);
          usbPrintf("sparkCalDLL(TX) = %02hx\n", w);
          w = sparkCalVCRO(b);
          usbPrintf("sparkCalVCRO(%d) = %02hx\n", b,w);
          break;
      
      case '+':
        // Start the cycle
        usbPrintf("Start radio \n");
        appStart();
        break; 

      case '-':
        // Stop the cycle
        usbPrintf("Stop radio \n");
        appStop();
        break;
		
		case 'a':
			appStop();
			while(!usbIsAvailable());
			std_buf[0] = usbGet();
			switch(std_buf[0])
			{
				case '0':
					linkAudio = false;
					break;
				case '1':
					linkAudio = true;
					break;
			}
			if(linkAudio)
				audioConfigure();
			appStart();
			break;
      
      case 'p':
        appStop();
        while(!usbIsAvailable());
        std_buf[0] = usbGet();
        switch(std_buf[0])
        {
          case '0':
          usbPrintf("SLEEP_NONE\n");
          linkSleepMode = SPARK_SLEEP_NONE;
          configureLink();
          break;
          case '1':
          usbPrintf("SLEEP_IDLE\n");
          linkSleepMode = SPARK_SLEEP_IDLE;
          configureLink();
          break;
          case '2':
          usbPrintf("SLEEP_SHALLOW\n");
          linkSleepMode = SPARK_SLEEP_SHALLOW;
          configureLink();
          break;
          case '3':
          usbPrintf("SLEEP_DEEP\n");
          linkSleepMode = SPARK_SLEEP_DEEP;
          configureLink();
          break;
        }
        appStart();
        break;

      case 'r':
        usbPrintf("Resetting statistics\n");
        resetStatistics();
        break;

      case 'x':
        appStop();
        linkRandom = !linkRandom;
        configureLink();
        usbPrintf("Random = %d\n", linkRandom);
        break;
		
	 case 'S': // Change syncword
		appStop();
		std_buf[0] = usbGet(); std_buf[1] = usbGet();
		sscanf((char*) &std_buf[0], "%2hx", &b);
		linkSyncWord = b << 8;
		std_buf[0] = usbGet(); std_buf[1] = usbGet();
		sscanf((char*) &std_buf[0], "%2hx", &b);
		linkSyncWord |= b;
		configureLink();
		appStart();
		break;
		
	 case 'P': // Change preamble size
	 appStop();
	 std_buf[0] = usbGet(); std_buf[1] = usbGet();
	 sscanf((char*) &std_buf[0], "%2hx", &b);
	 linkPreambleSize = b ;
	 configureLink();
	 appStart();
	 break;		
	 
	 case 'X': // Change min noise level
	 appStop();
	 std_buf[0] = usbGet(); std_buf[1] = usbGet();
	 sscanf((char*) &std_buf[0], "%2hx", &b);
	 linkMinNoiLvl = b ;
	 configureLink();
	 appStart();
	 break;

	 case 'Y': // Change min pulse zoom
	 appStop();
	 std_buf[0] = usbGet(); std_buf[1] = usbGet();
	 sscanf((char*) &std_buf[0], "%2hx", &b);
	 linkMinPulAmp = b ;
	 configureLink();
	 appStart();
	 break;
	 
	 case 'Z': // Change min signal threshold
	 appStop();
	 std_buf[0] = usbGet(); std_buf[1] = usbGet();
	 sscanf((char*) &std_buf[0], "%2hx", &b);
	 linkMinSigStr = b ;
	 configureLink();
	 appStart();
	 break;

      case 'B': // Change link to bidirectional
        usbPrintf("Bidirectional mode\n");
        appStop();
		linkBidirectional = true;
        std_buf[0] = usbGet();
        if(std_buf[0] == '0')
          linkMaster = false;
        else
          linkMaster = true;
		configureLink();
        appStart();
        break;

      case 's':
        usbPrintf("Statistics:\n");
        printStatistics();
        break;
      
      case 'm': // Spectral mask, 12 bytes
        appStop();
		usb_get(std_buf, 24);
		for(uint8_t i=0;i<12;i++)
		{
			sscanf((char*) &std_buf[i*2], "%2hx", &b);
			linkTXMask[i] = b;
		}
		//sscanf((char*)std_buf, "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx", &linkTXMask[0],&linkTXMask[1],&linkTXMask[2],&linkTXMask[3], &linkTXMask[4],&linkTXMask[5],&linkTXMask[6],&linkTXMask[7], &linkTXMask[8],&linkTXMask[9],&linkTXMask[10],&linkTXMask[11] );
		/*
		for(uint8_t i=0;i<12;i++)
		{
			w =  usbGet();	
				
		}*/
		configureLink();
		appStart();
        break;
		
	  case '>':
		usbPrintf("Writing Flash\n");
		flashWrite();
		break;
	
	  case '<':
		usbPrintf("Reading Flash\n");
		appStop();
		flashRead();
		configureLink();
		appStart();
		break;

      case 'v':
        usbPrintf("Voltage set: ");
        std_buf[0] = usbGet();
        startSPI();
        switch(std_buf[0])
        {
          case '0':
            sendSPI(SPI_WRITE | REG_RESISTUNE); sendSPI(0x08);
            usbPrintf("1.08V\n");
            break;
            
          case '1':
            sendSPI(SPI_WRITE | REG_RESISTUNE); sendSPI(0x09);
            usbPrintf("1.03V\n");
            break;
            
          case '2':
            sendSPI(SPI_WRITE | REG_RESISTUNE); sendSPI(0x0a);
            usbPrintf("1.04V\n");
            break;
            
          case '3':
            sendSPI(SPI_WRITE | REG_RESISTUNE); sendSPI(0x0b);
            usbPrintf("0.97V\n");
            break;
        }
        stopSPI();
        break;

      case 'R':
		appStop();
        usbPrintf("RX mode\n");
        linkBidirectional = false;
		linkMaster = false;
		configureLink();
        //sparkChangeMode(SPARK_MODE_RX);
        resetStatistics();
		appStart();
        break;

      case 'T':
        appStop();
        usbPrintf("TX mode\n");
        linkBidirectional = false;
        linkMaster = true;
		configureLink();
        //sparkChangeMode(SPARK_MODE_TX);
        resetStatistics();
        appStart();
        break;
      
      case '[': // SPI command mode
        std_buf[0] = std_buf[1] = std_buf[2];
        startSPI();
        while(std_buf[0] != ']' && std_buf[1] != ']')
        {
          while(!usbIsAvailable());
          std_buf[0] = usbGet();
          if(std_buf[0] != ']')
          {
            while(!usbIsAvailable());
            std_buf[1] = usbGet();
            sscanf((char *)std_buf, "%2hx", &b);
          
            r = sendSPI(b);
            usbPrintf("%s%s %02hx \t %s%s %02hx\n",bit_rep[b >> 4], bit_rep[b & 0x0F], b, bit_rep[r >> 4], bit_rep[r & 0x0F], r);
          }
        }
        stopSPI();
        break;

      case '\n':
          break;
        
      default:
        usbPrintf("??\n");
        break;
    }
    
  }
}
