//
//  radio.cpp
//  
//
//  Created by Michiel Soer.
//
//

#include "radio.h"
#include "spark_v5.h"

#define XTAL_CYCLES 32768
#define PLL_CYCLES 20480000
//#define PLL_CYCLES 20000000
#define PLL_STARTUP_CYCLES 0x20

// Callback for frame processing, define these in the user modules
void callbackPacketSend(unsigned char numBytes);
bool callbackPacketReceived(unsigned char numBytes);
void callBackPacketUpkeep(void);
void callbackLinkLost(void);
void callbackException(void);

// External vars
unsigned char *sparkRXbuffer;
unsigned char *sparkTXbuffer;
unsigned char sparkPacketRSSI;
unsigned char sparkPacketNSSI;
unsigned short sparkPacketRXWaited;
unsigned char sparkLastInt;
bool sparkCRCfail;

// Internal vars
unsigned char mode;
bool bidirectional;
bool master;
bool running;

unsigned char packetNumBytes;
bool packetRandom;
unsigned char lostPackets;
unsigned char rxData[SPARK_BUFFER_SIZE];
unsigned char txData[SPARK_BUFFER_SIZE];

unsigned char sleepCfg;

void applySleepOffset(short offset);

bool sparkBIST(bool verbose)
{
  unsigned char b;
  bool fail;

  disableExtInt();
  resetRadio();
  
  // Test SPI
  if(verbose)
	usbPrintf("SPI..");
  startSPI();
  sendSPI(SPI_WRITE | REG_INT1); sendSPI(0x00);
  sendSPI(SPI_WRITE | REG_INT2); sendSPI(0x00);
  sendSPI(REG_PREAMBLEN); b = sendSPI(0x00);
  stopSPI();
  if(b != 0x09)
  {
	if(verbose)
		usbPrintf("FAIL\n");
    return false;
  }
  else
  {
	  if(verbose)
		usbPrintf("OK\n");
  }

  // Test MCU_int
  fail = false;
  if(verbose)
	usbPrintf("MCU_INT..");
  startSPI();
  sendSPI(SPI_WRITE | REG_INT1); sendSPI(0x80);
  sendSPI(SPI_WRITE | REG_INT2); sendSPI(0x00);
  delay(10);
  fail = fail |  (getMCUint()); // should be low
  sendSPI(SPI_WRITE | REG_INT1); sendSPI(0x00);
  delay(10);
  fail = fail |  !(getMCUint()); // should be high
  sendSPI(SPI_WRITE | REG_INT1); sendSPI(0x00);
  stopSPI();
  if(fail)
  {
	  if(verbose)
		usbPrintf("FAIL\n");
    return false;
  }
  else
  {
	  if(verbose)
		usbPrintf("OK\n");
  }

  // Test Power cycling
  startSPI();
  sendSPI(SPI_WRITE | REG_SLEEPCFG); sendSPI(0x00);
  sendSPI(SPI_WRITE | REG_TIMERCFG); sendSPI(0x00);
  sendSPI(SPI_WRITE | REG_COMMANDS); sendSPI(0x00); // Wakeup all voltage domains
  delay(10);
  sendSPI(REG_COMMANDS); b = sendSPI(0x00);
  stopSPI();
  if(verbose)
	usbPrintf("REF_PWRD..");
  if(!(b & 0x01))
  {
	  if(verbose)
		usbPrintf("FAIL\n");
    return false;
  }
  else
  {
	  if(verbose)
		usbPrintf("OK\n");
  }
  if(verbose)
	usbPrintf("PLL_PWRD..");
  if(!(b & 0x02))
  {
	  if(verbose)
		usbPrintf("FAIL\n");
    return false;
  }
  else
  {
	  if(verbose)
		usbPrintf("OK\n");
  }
  if(verbose)
	usbPrintf("DCDC_PWRD..");
  if(!(b & 0x04))
  {
	  if(verbose)
		usbPrintf("FAIL\n");
    return false;
  }
  else
  {
    if(verbose)
		usbPrintf("OK\n");
  }
  if(verbose)
	usbPrintf("DLL_PWRD..");
  if(!(b & 0x08))
  {
	  if(verbose)
		usbPrintf("FAIL\n");
    return false;
  }
  else
  {
	  if(verbose)
		usbPrintf("OK\n");
  }

  return true;
}

unsigned char sparkCalDLL(bool RX)
{
  unsigned short w;
  unsigned char dllTune;
  unsigned char MSB;

  // Only works when we are in TX mode
  startSPI();
  if(RX)
  {
    sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(0x20); // Switch to RX mode
  }
  else
  {
    sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(0x00); // Switch to TX mode
  }
  
  sendSPI(REG_DLLTUNE); MSB = sendSPI(0x00) & 0xf0; // Get MSB
  
  while(1) // Wait until DLL is powered up
  {
    sendSPI(REG_COMMANDS); w = sendSPI(0x00);
    if(w & BIT_DLLPWRD)
      break;
  }
  
  for(unsigned char i=0;i<16;i++)
  {
     dllTune = MSB | ((unsigned char) i);
     sendSPI(SPI_WRITE | REG_DLLTUNE); sendSPI(dllTune); // Adjust DLL-TUNE
     
     delay(10);// Wait a bit to stabilize

     sendSPI(REG_COMMANDS); w = sendSPI(0x00);
     if(w & (1 << 7) )
      break;
  }
  sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(BIT_GOTOSLP); // Go to sleep
  stopSPI();

  return dllTune & 0x0f;
}


static unsigned char rev_lookup[16] = {
0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

uint8_t inline reverse(uint8_t n) {
   // Reverse the top and bottom nibble then swap them.
   return (rev_lookup[n&0b1111] << 4) | rev_lookup[n>>4];
}

unsigned char sparkCalVCRO(unsigned short MHz)
{
  unsigned short w;
  unsigned char calParam;
  unsigned char ticks;

  ticks = ( ((unsigned long)MHz) * 1000) / (PLL_CYCLES /1000); // Number of VCRO cycles in one PLL cycle

  calParam = reverse(~ticks); // Code into cal code

  //usbPrintf("%02hx %02hx %02hx\n", ticks, ~ticks, calParam);

  startSPI();
  
  sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(0x00); // Switch to TX mode

  while(1) // Wait until DLL is powered up
  {
    sendSPI(REG_COMMANDS); w = sendSPI(0x00);
    if(w & BIT_DLLPWRD)
      break;
  }
  
  sendSPI(SPI_WRITE | REG_CALIBRSET); sendSPI(calParam); // Set calParam
  sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(BIT_CALIBRAT); // Trigger calibration

  while(1) // Wait until calibration is done
  {
    sendSPI(REG_INT2); w = sendSPI(0x00);
    if(w & BIT_CALIBRT)
      break;
  }

  sendSPI(REG_CALIBRSET); w = sendSPI(0x00); // Get calibration result
  sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(BIT_GOTOSLP); // Go to sleep
  stopSPI();

  return w;
}

void sparkRadioSetup() // Setup the UWB
{
  // Setup the UWB
  startSPI();

  // Setup interrupts
  sendSPI(SPI_WRITE | REG_INT1); sendSPI(0x00);
  sendSPI(SPI_WRITE | REG_INT2); sendSPI(0x00);
  
  // Setup PLL
  sendSPI(SPI_WRITE | REG_PLLRATIO); sendSPI(0x00);
  sendSPI(SPI_WRITE | REG_RESISTUNE); sendSPI(0x08);

  // TX Spectrum
  sendSPI(SPI_WRITE | REG_TXPATTERN0); sendSPI(0x00);
  sendSPI(SPI_WRITE | REG_TXPATTERN1); sendSPI(0x00);
  sendSPI(SPI_WRITE | REG_TXPATTERN2); sendSPI(0xbd);
  sendSPI(SPI_WRITE | REG_TXPATTERN3); sendSPI(0xbd);
  sendSPI(SPI_WRITE | REG_TXPATTERN4); sendSPI(0xbc);
  sendSPI(SPI_WRITE | REG_TXPATTERN5); sendSPI(0xbc);
  sendSPI(SPI_WRITE | REG_TXPATTERN6); sendSPI(0xbb);
  sendSPI(SPI_WRITE | REG_TXPATTERN7); sendSPI(0xbb);
  sendSPI(SPI_WRITE | REG_TXPATTERN8); sendSPI(0xba);
  sendSPI(SPI_WRITE | REG_TXPATTERN9); sendSPI(0xba);
  sendSPI(SPI_WRITE | REG_TXPATTERN10); sendSPI(0x00);
  sendSPI(SPI_WRITE | REG_TXPATTERN11); sendSPI(0x00);

  // Temporary radio setup for optimized link
   sendSPI(0x4e); sendSPI(0xf6);
  //sendSPI(0x4e); sendSPI(0x0F);
  sendSPI(0x5c); sendSPI(0x1b);
  sendSPI(0x71); sendSPI(0x02);
  
  // Enable CRC
  sendSPI(SPI_WRITE | REG_CRCPOLYNO8); sendSPI(0x80);
  sendSPI(SPI_WRITE | REG_CRCPOLYNO0); sendSPI(0x05);

  stopSPI();

  //calDLL(); // Take care not to screw up register 0E first nibble

  // Switch off
  //sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(0x04);
  
  // Variables and modes
  bidirectional = false;
  master = false;
  running = false;
  mode = SPARK_MODE_TX;
  sparkRXbuffer = rxData;
  sparkTXbuffer = txData;
  sparkCRCfail = false;
}

// Spark Interrupt
bool cycling;
bool adjusting;

void callbackWatchdog() {
  // Restart radio
  sparkStopRadio();
  sparkStartRadio();

  // Raise exception
  callbackException();
}

inline void callbackFrameStart() {
  /*
  switch(mode)
    {
      case MODE_RX:
        startSPI();
        sendSPI(SPI_WRITE | REG_COMMANDS); sendSPI(BIT_RXMODE);
        stopSPI();
        break;

      case MODE_TX:
        startSPI();
        sendSPI(SPI_WRITE | REG_COMMANDS); sendSPI(BIT_STARTTX);
        stopSPI();
        break;
    }*/
}

inline void callbackFrameEnd() {
  bool reception;
  unsigned short rxWaited;
  unsigned char numBytes;
  
  switch(mode)
    {
      case SPARK_MODE_RX:
        if(cycling)
          applySleepOffset(-1); // Cycle next frame
        else
          applySleepOffset(0); // Cycle next frame

        // Get packet
        startSPI();
        sendSPI(REG_RXBUF);  numBytes = sendSPI(0x00); 
		//sendSPI(SPI_BURST | REG_FIFO);
		//pullSPI(rxData, numBytes);
		stopSPI();
		
		//sparkPullRX(0, numBytes);
		
		/*
        sendSPI(SPI_BURST | REG_FIFO); 
		pullSPI(rxData, numBytes);*/
		/*
        for(unsigned char i=0;i<numBytes;i++)
        {
			//sendSPI(REG_FIFO); 
			rxData[i] = sendSPI(0x00);
        }*/      
        reception = callbackPacketReceived(numBytes); // See if packet is correct		

        // Check lost packets
        if(cycling)
            lostPackets = 0;
        else if(reception)
        {
          lostPackets = 0; 
        }
        else
          lostPackets++;

        if(lostPackets > 100 && !(bidirectional && master)) // Link is lost, don't care if master and bidirectional
        {
          cycling = true;
          lostPackets = 0;
          callbackLinkLost();
        }

        // Get number of PLL cycles in IDLE before preamble is detected
        startSPI();
        sendSPI(REG_RXWAITED8); rxWaited = sendSPI(0x00);
        rxWaited = rxWaited << 8;
        sendSPI(REG_RXWAITED0); rxWaited |= sendSPI(0x00);
		rxWaited = rxWaited >> 1; // Offset to remove bit 0 (which is not used)
		if(reception)
		{
			 sendSPI(REG_RSSI); sparkPacketRSSI = sendSPI(0x00);
			 sendSPI(REG_MAXNOILVL); sparkPacketNSSI = sendSPI(0x00);
			 sparkPacketRXWaited = rxWaited;
		}
        stopSPI();
        
        if(reception && !(bidirectional && master))
        {
          if(cycling)
          {
            cycling = false; // We have locked on*/
            if(!bidirectional)
            {
              adjusting = true;
              applySleepOffset(1); // Step back to good frame
            }
          }
          else if(!adjusting)
          {
            // Adjust clocking, 625 is a full XTAL cycle
            if(rxWaited > (725))
            {
              applySleepOffset(1);
              //Serial.println(rxWaited);
              adjusting = true;
            }
            else if(rxWaited < (100))
            {
              applySleepOffset(-1);
              //Serial.println(rxWaited);
              adjusting = true;
            }  
            else
              applySleepOffset(0);          
          }
          else
          {
            adjusting = false;
            applySleepOffset(0);
          }
        }
        else if(!cycling)
        {
          adjusting = false;
          applySleepOffset(0);  
        }
        
		// Prepare next packet in bidirectional
		
		if(bidirectional)
		{
			
			startSPI();
			sendSPI(REG_COMMANDS); sendSPI(BIT_FLUSHTX);
			stopSPI();
			
			callbackPacketSend(packetNumBytes);		
			/*
			startSPI();
			sendSPI(REG_COMMANDS); sendSPI(BIT_FLUSHTX);
			//sendSPI(SPI_BURST | SPI_WRITE | REG_FIFO);
			//pushSPI(txData, packetNumBytes);
			stopSPI();
			sparkPushTX(0, packetNumBytes);			*/
			
			
			//sendSPI(SPI_BURST | SPI_WRITE | REG_FIFO);
			//pushSPI(txData, packetNumBytes);			
					
		}
			
        break;

      case SPARK_MODE_TX:
        //adjusting = false;
        applySleepOffset(0); // Cycle next frame
      
		if(!bidirectional)
		{
			
			startSPI();
			sendSPI(REG_COMMANDS); sendSPI(BIT_FLUSHTX);
			stopSPI();
			
			callbackPacketSend(packetNumBytes);
		
		/*
			// Set packet
			if(!packetRandom)
			{
			  startSPI();
			  sendSPI(REG_COMMANDS); sendSPI(BIT_FLUSHTX);
			  //sendSPI(SPI_BURST | SPI_WRITE | REG_FIFO);
			  //pushSPI(txData, packetNumBytes);
			  stopSPI();
			  sparkPushTX(0, packetNumBytes);
			  
			  //sendSPI(SPI_BURST | SPI_WRITE | REG_FIFO);
			  //pushSPI(txData, packetNumBytes);		 
			  
			}*/
		}
		else
			callBackPacketUpkeep(); // Push remaining RX data 
		break;
		
    }
}

bool clockGated;
bool wakeupWasLast;
unsigned short sleep_cycles;
unsigned short frame_cycles;
unsigned char sleepingMode;

void callbackExtInterrupt()
{
  unsigned char int1, int2;
  
  startSPI();
  int1 = sendSPI(REG_INT2); int2 = sendSPI(0x00); // Clear interrupt
  //sendSPI(REG_INT2); int2 = sendSPI(0x00); // Clear interrupt
  stopSPI();
  
  if(int1 & 0x01)
	sparkCRCfail = true;
  else
	sparkCRCfail = false;
   
  if(int1 & BIT_WAKEUP)
  {
	sparkLastInt = 1;
    /*
    if(wakeupWasLast) // We missed an interrupt, so fix it
    {
       // Flip mode if bidirect?
      if(bidirectional)
      {
        if(mode == SPARK_MODE_RX)
          sparkChangeMode(SPARK_MODE_TX);
        else
          sparkChangeMode(SPARK_MODE_RX);
      }
    }*/
    wakeupWasLast = !wakeupWasLast;
    
    if(!clockGated) // We missed a XOTIMER event
    {
	  enableClockGate();
    
      startSPI();
      //sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(0x00);
      switch(mode)
      {
        case SPARK_MODE_RX:
          sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(BIT_GOTOSLP | BIT_RXMODE);
          break;
          
        case SPARK_MODE_TX:
          sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(BIT_GOTOSLP | BIT_STARTTX);
          break;
      }
      stopSPI();
    }

    //delayMicroseconds(50+90);
    
  }
  else if( (int1 & BIT_TXEND) || (int1 & BIT_NEWPKT) || (int2 & BIT_RXTOUT) )
  { 
	  sparkLastInt = 2;
	  
    if(clockGated)
      callbackFrameEnd();

    // Flip mode if bidirect?
    //if(bidirectional && wakeupWasLast)
	if(bidirectional)
    {
      if(mode == SPARK_MODE_RX)
        sparkChangeMode(SPARK_MODE_TX);
      else
        sparkChangeMode(SPARK_MODE_RX);
    }
    wakeupWasLast = false;

    startSPI();
    //sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(0x00);
    switch(mode)
    {
      case SPARK_MODE_RX:
        sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(BIT_GOTOSLP | BIT_RXMODE);
        break;
        
      case SPARK_MODE_TX:
        sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(BIT_GOTOSLP | BIT_STARTTX);
        break;
    }
    
    if(sleepingMode == SPARK_SLEEP_DEEP)
    {
      //sendSPI(SPI_WRITE | REG_INT1); sendSPI(BIT_WAKEUP);
      //sendSPI(SPI_WRITE | REG_INT2); sendSPI(BIT_XOTIMER);
      sendSPI(SPI_WRITE | REG_INT1); sendSPI(BIT_WAKEUP | BIT_TXEND | BIT_NEWPKT); 
      sendSPI(SPI_WRITE | REG_INT2); sendSPI(BIT_XOTIMER | BIT_RXTOUT);
    }
    else if(sleepingMode != SPARK_SLEEP_NONE)
    {
      //sendSPI(SPI_WRITE | REG_INT1); sendSPI(BIT_WAKEUP);
      //sendSPI(SPI_WRITE | REG_INT2); sendSPI(0x00);
	  /*
	  if(sleepingMode == SPARK_SLEEP_SHALLOW)
	  {
		sendSPI(SPI_WRITE | REG_INT1); sendSPI(BIT_TXEND | BIT_NEWPKT); 
	  }
	  else
	  {*/
		  sendSPI(SPI_WRITE | REG_INT1); sendSPI(BIT_WAKEUP | BIT_TXEND | BIT_NEWPKT); 
	  //}
      sendSPI(SPI_WRITE | REG_INT2); sendSPI(0x00 | BIT_RXTOUT);
    }
    stopSPI();

    //delayMicroseconds(50); // keep clock stable until sleep is in effect
    
    if(sleepingMode == SPARK_SLEEP_DEEP)
    {
      //SET(MCU_GATE, LOW); // Shutdown PLL to save power
	  disableClockGate();
      clockGated = false;
    }
  }
  else if( ((int2 & BIT_XOTIMER) && sleepingMode == SPARK_SLEEP_DEEP) || ((int2 & BIT_XOTIMER) && sleepingMode == SPARK_SLEEP_NONE)) // This interrupt is sometimes not generated!
  {
	  sparkLastInt = 3;
	  
    if(sleepingMode == SPARK_SLEEP_NONE)
    {
      if(mode == SPARK_MODE_TX)
      {
        startSPI();
        sendSPI(SPI_WRITE | REG_COMMANDS); sendSPI(BIT_STARTTX);
        stopSPI();
      }
    }
    else
    {
      //SET(MCU_GATE, HIGH); // Bring PLL back up
	  enableClockGate();
      clockGated = true;
  
      startSPI();
      sendSPI(SPI_WRITE | REG_INT1); sendSPI(BIT_WAKEUP | BIT_TXEND | BIT_NEWPKT); 
      sendSPI(SPI_WRITE | REG_INT2); sendSPI(0x00 | BIT_RXTOUT); 
      //sendSPI(SPI_WRITE | REG_INT1); sendSPI(BIT_WAKEUP); 
      //sendSPI(SPI_WRITE | REG_INT2); sendSPI(0x00); 
      stopSPI();
    }
  }
  
  startSPI();
  int1 = sendSPI(REG_INT2); int2 = sendSPI(0x00); // Clear interrupt
  //sendSPI(REG_INT2); int2 = sendSPI(0x00); // Clear interrupt
  stopSPI();

  // Reset watchdog
  wdReset();
}

void sparkSleepSetup(unsigned char sleepMode, unsigned short packetsPerSecond, unsigned short frameLengthMicro)
{
  unsigned char sleepBits;

  // Decode sleeping mode
  sleepingMode = sleepMode;
  switch(sleepMode)
  {
    case SPARK_SLEEP_NONE:
      sleepBits = BIT_SLPDEPTH_IDLE; // BUT DON'T GO TO SLEEP 
      break;

    case SPARK_SLEEP_IDLE:
      sleepBits = BIT_SLPDEPTH_IDLE;
      break;

    case SPARK_SLEEP_SHALLOW:
      sleepBits = BIT_SLPDEPTH_SHALLOW;
      break;

    case SPARK_SLEEP_DEEP:
      sleepBits = BIT_SLPDEPTH_DEEP;
      break;
  }
   
  // Number of XTAL clock cycles to sleep, excluding PLL startup in deep sleep
  if(sleepingMode == SPARK_SLEEP_DEEP)
    sleep_cycles = XTAL_CYCLES / packetsPerSecond - PLL_STARTUP_CYCLES;
  else if(sleepingMode == SPARK_SLEEP_SHALLOW)
    sleep_cycles = XTAL_CYCLES / packetsPerSecond;
  else
    sleep_cycles = PLL_CYCLES / packetsPerSecond;

  // Number of PLL clock cycle for the frame, equal to RX timeout, not meaningful for TX (unless we want to center TX in the frame)
  frame_cycles = ( ((unsigned long)PLL_CYCLES) * ((unsigned long)frameLengthMicro)) / 1024 / 1024 / 8; // REG_RXPERIOD is in PLL cycles X8
  
  // Power Cycling Setup
  startSPI();
  if(sleepingMode == SPARK_SLEEP_NONE) 
  {
    sleepCfg = sleepBits;
  }
  else
  {
    sleepCfg = sleepBits | BIT_AUSRXTO | BIT_AUSTXEND | BIT_AUSRXEND; // auto sleep on RX reception and timeout
  }
  sendSPI(SPI_WRITE | REG_SLEEPCFG); sendSPI(sleepCfg); 
  sendSPI(SPI_WRITE | REG_XTMRCNT0); sendSPI(sleep_cycles % 256); sendSPI(SPI_WRITE | REG_XTMRCNT8); sendSPI(sleep_cycles / 256); // Set sleep timer
  sendSPI(SPI_WRITE | REG_PLLPWRUP); sendSPI(PLL_STARTUP_CYCLES); // 3ms PLL powerup
  sendSPI(SPI_WRITE | REG_RXPERIOD0); sendSPI(frame_cycles % 256); sendSPI(SPI_WRITE | REG_RXPERIOD8); sendSPI(frame_cycles / 256); // Set timer to approx 30 Hz
  sendSPI(SPI_WRITE | REG_TXDELAY); sendSPI(0xff); // Move TX frame as far back as we can
  sendSPI(SPI_WRITE | REG_INT1); sendSPI(0x00); 
  sendSPI(SPI_WRITE | REG_INT2); sendSPI(0x00); 
  stopSPI();

  // Watchdog
  unsigned short wdTicks = 2*PLL_CYCLES / packetsPerSecond;
  if(wdTicks < 1)
    wdTicks = 1;
  wdSetup(wdTicks);
}

void applySleepOffset(short offset)
{
  short new_cycles;
  
  if(sleepingMode == SPARK_SLEEP_IDLE)
    new_cycles = ((short) sleep_cycles) + offset*625;
  else
    new_cycles = ((short) sleep_cycles) + offset;
    
  startSPI();
  sendSPI(SPI_WRITE | REG_XTMRCNT0); sendSPI((new_cycles) % 256); sendSPI(SPI_WRITE | REG_XTMRCNT8); sendSPI((new_cycles) / 256);
  //sendSPI(SPI_WRITE | REG_XTMRCNT0); sendSPI(sleep_cycles % 256); sendSPI(SPI_WRITE | REG_XTMRCNT8); sendSPI(sleep_cycles / 256); // Set timer to approx 30 Hz
  stopSPI();
}

void sparkPacketSetup(unsigned char minNoiLvl,unsigned char minPulAmp, unsigned char minSigStr, unsigned char preambleSize, unsigned char packetSize, unsigned short syncWord, bool randomData)
{
  packetNumBytes = packetSize;
  packetRandom = randomData;
  
  startSPI();
  sendSPI(SPI_WRITE | REG_PREAMBLEN); sendSPI(preambleSize); // PREAMBLEN, maximum 64
  sendSPI(SPI_WRITE | REG_TXPKTSIZE ); sendSPI(packetSize); // TXPKTSIZE
  sendSPI(SPI_WRITE | REG_RXPKTSIZE ); sendSPI(packetSize); // RXPKTSIZE
  sendSPI(SPI_WRITE | REG_SYNCWORD8); sendSPI(syncWord / 256); // SYNBYTE
  sendSPI(SPI_WRITE | REG_SYNCWORD0); sendSPI(syncWord % 256); // SYNBYTE
  sendSPI(SPI_WRITE | REG_MINNOILVL); sendSPI(minNoiLvl); // 
  sendSPI(SPI_WRITE | REG_MINPULAMP); sendSPI(minPulAmp); // 
  sendSPI(SPI_WRITE | REG_MINSIGSTR); sendSPI(minSigStr); // 
  //sendSPI(SPI_WRITE | REG_MAINCTRL); sendSPI(0x10 | 0x08); // Data src to random LFSR
  if(packetRandom)
  {
    sendSPI(SPI_WRITE | REG_MAINCTRL); sendSPI(0x10); // Data src to random LFSR
  }
  else
  {
    sendSPI(SPI_WRITE | REG_MAINCTRL); sendSPI(0x00);
  }
  sendSPI(SPI_WRITE | REG_PREFRAME); sendSPI(0x1a); // Preamble check and relaxed syncword check
  //sendSPI(SPI_WRITE | REG_MAXNOILVL); sendSPI(0x1b); // Put noise down a little bit

  stopSPI();
}

void sparkSpectralSetup(unsigned char *mask)
{
	startSPI();
	sendSPI(SPI_WRITE | SPI_BURST | REG_TXPATTERN0);
	for(unsigned char i=0;i<12;i++)
		sendSPI(mask[i]);	
	stopSPI();
}

void sparkPushTX(unsigned char start, unsigned char stop)
{
	startSPI();	
	sendSPI(SPI_BURST | SPI_WRITE | REG_FIFO);
	pushSPI(&txData[start], stop-start);
	stopSPI();
}

void sparkPullRX(unsigned char start, unsigned char stop)
{
	startSPI();
    sendSPI(SPI_BURST | REG_FIFO); 
	pullSPI(&rxData[start], stop-start);		
    stopSPI();	
}

void sparkStartRadio()
{
  clockGated = true;
  wakeupWasLast = false;
  enableClockGate();
  
  // Start the cycle
  startSPI();
  sendSPI(SPI_WRITE | REG_SLEEPCFG); sendSPI(sleepCfg);
  stopSPI();
  
  if(sleepingMode == SPARK_SLEEP_NONE)
  {
    switch(mode)
    {
        case SPARK_MODE_RX:
          startSPI();
          sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(BIT_RXMODE);
          stopSPI();
          break;
          
        case SPARK_MODE_TX:
          startSPI();
          sendSPI(SPI_WRITE | REG_INT1); sendSPI(0x00); 
          sendSPI(SPI_WRITE | REG_INT2); sendSPI(BIT_XOTIMER);
          stopSPI();
          break;
    }
  }
  else
  {
    startSPI();
    sendSPI(SPI_WRITE | REG_TIMERCFG); sendSPI(0x01); // Autowakeup
	/*
	if(sleepingMode == SPARK_SLEEP_SHALLOW)
	{
		sendSPI(SPI_WRITE | REG_INT1); sendSPI(BIT_TXEND | BIT_NEWPKT); 
	}
	else
	{*/
		sendSPI(SPI_WRITE | REG_INT1); sendSPI(BIT_WAKEUP | BIT_TXEND | BIT_NEWPKT); 
	//}
    sendSPI(SPI_WRITE | REG_INT2); sendSPI(0x00 | BIT_RXTOUT);
    //sendSPI(SPI_WRITE | REG_INT1); sendSPI(BIT_WAKEUP); 
    //sendSPI(SPI_WRITE | REG_INT2); sendSPI(0x00); 
    switch(mode)
    {
        case SPARK_MODE_RX:
          sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(BIT_GOTOSLP | BIT_RXMODE);
          break;
          
        case SPARK_MODE_TX:
          sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(BIT_GOTOSLP | BIT_STARTTX);
          break;
    }
    stopSPI();
    if(!bidirectional)
      cycling = true;
    else
      cycling = false;
  }

  // Flush TX if required
  startSPI();
  //sendSPI(SPI_WRITE | REG_COMMANDS); sendSPI(BIT_FLUSHTX | BIT_FLUSHRX);
  stopSPI();
  if(mode == SPARK_MODE_TX)
  {
	// Setup a packet
	
    //callbackFrameEnd(); 
	callbackPacketSend(packetNumBytes);
	startSPI();
	sendSPI(REG_COMMANDS); sendSPI(BIT_FLUSHTX);
	sendSPI(SPI_BURST | SPI_WRITE | REG_FIFO);
	pushSPI(txData, packetNumBytes);
	stopSPI();	
  }
	
  enableExtInt();

  // Enable watchdog
  if(sleepingMode != SPARK_SLEEP_NONE)
	  wdEnable();
	  
  running = true;
}

void sparkStopRadio()
{
	running = false;
  wdDisable();
  
  disableExtInt();
  
  // Stop the cycle
  startSPI();
  sendSPI(SPI_WRITE | REG_TIMERCFG); sendSPI(0x00); // Don't wakeup anymore
  sendSPI(SPI_WRITE | REG_SLEEPCFG); sendSPI(0x00); // Don't auto sleep anymore
  sendSPI(SPI_WRITE | REG_INT1); sendSPI(0x00); 
  sendSPI(SPI_WRITE | REG_INT2); sendSPI(0x00);
  sendSPI(SPI_WRITE | REG_COMMANDS);  sendSPI(BIT_GOTOSLP);
  sendSPI(SPI_WRITE | REG_COMMANDS); sendSPI(BIT_FLUSHTX | BIT_FLUSHRX);
  stopSPI();
  
  if(bidirectional)
	sparkChangeMode(SPARK_MODE_RX); // Must start in RX
}

unsigned char sparkGetMode()
{
  return mode;
}

void sparkChangeMode(unsigned char newMode)
{
  unsigned char b;
  
  mode = newMode;
  startSPI();
  sendSPI(REG_MAINCTRL); b = sendSPI(0x00);
  if(mode == SPARK_MODE_RX)
  {
    //cycling = true;
    sendSPI(SPI_WRITE | REG_MAINCTRL); sendSPI(b | 0x08); // Data src to random LFSR
  }
  else
  {
    sendSPI(SPI_WRITE | REG_MAINCTRL); sendSPI(b & (~0x08));
  }
  stopSPI();
}

bool sparkIsScanning()
{
  return cycling;
}

void sparkSetBidirectional(bool newBidirect, bool newMaster)
{
  bidirectional = newBidirect;
  master = newMaster; 
  cycling = false;
  
  if(bidirectional)
	sparkChangeMode(SPARK_MODE_RX); // Must start in RX
  else if(master)
	sparkChangeMode(SPARK_MODE_TX); // Unidirectional masters are TX
  else
	sparkChangeMode(SPARK_MODE_RX); // Must start in RX
}

bool sparkIsBidirectional()
{
  return bidirectional;
}

bool sparkIsMaster()
{
  return master;
}

bool sparkIsRunning()
{
	return running;
}

