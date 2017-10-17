//
//  radio.h
//  
//
//  Created by Michiel Soer.
//
//

#ifndef radio_h
#define radio_h

#include "hardware.h"

// Transmission mode
#define SPARK_MODE_RX       0
#define SPARK_MODE_TX       1

// Sleep mode
#define SPARK_SLEEP_NONE    0
#define SPARK_SLEEP_IDLE    1
#define SPARK_SLEEP_SHALLOW 2
#define SPARK_SLEEP_DEEP    3

// Buffers, 128 bytes each
#define SPARK_BUFFER_SIZE 128
extern unsigned char *sparkRXbuffer;
extern unsigned char *sparkTXbuffer;

// Packet information
extern unsigned char sparkPacketRSSI;
extern unsigned char sparkPacketNSSI;
extern unsigned short sparkPacketRXWaited;
extern unsigned char sparkLastInt;
extern bool sparkCRCfail;

// API
bool sparkBIST(bool verbose);
unsigned char sparkCalDLL(bool RX);
unsigned char sparkCalVCRO(unsigned short MHz);
void sparkRadioSetup();
void sparkSleepSetup(unsigned char sleepMode, unsigned short packetsPerSecond, unsigned short frameLengthMicro);
void sparkPacketSetup(unsigned char minNoiLvl, unsigned char minPulAmp, unsigned char minSigStr, unsigned char preambleSize, unsigned char packetSize, unsigned short syncWord, bool randomData);
void sparkSpectralSetup(unsigned char *mask);
void sparkPushTX(unsigned char start, unsigned char stop);
void sparkPullRX(unsigned char start, unsigned char stop);
void sparkStartRadio();
void sparkStopRadio();
unsigned char sparkGetMode();
void sparkChangeMode(unsigned char newMode);
bool sparkIsScanning();
void sparkSetBidirectional(bool newBidirect, bool newMaster);
bool sparkIsBidirectional();
bool sparkIsMaster();
bool sparkIsRunning();

#endif /* radio_h */
