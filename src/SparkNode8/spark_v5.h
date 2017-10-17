//
//  spark_v5.h
//
//
//  Created by Michiel Soer.
//
//

#ifndef spark_h
#define spark_h

// Tell what version we are
#define SPARK_V5

// SPI flags
#define SPI_ADDR_MASK   0x3F // 5 bit address
#define SPI_WRITE       0x40
#define SPI_BURST       0x80

// Register addresses
#define REG_INT1          0x00
#define REG_INT2          0x01
#define REG_TXBUF         0x02
#define REG_RXBUF         0x03
#define REG_SLEEPCFG      0x04
#define REG_TIMERCFG      0x05
#define REG_XTMRCNT8      0x06
#define REG_XTMRCNT0      0x07
#define REG_RXPWRUP       0x08
#define REG_TXDELAY       0x09
#define REG_DLLPWRUP      0x0A
#define REG_PLLPWRUP      0x0B
#define REG_PLLRATIO      0x0C
#define REG_RESISTUNE     0x0D
#define REG_DLLTUNE       0x0E
#define REG_RANDPHASE     0x0F
#define REG_TXPATTERN0    0x10
#define REG_TXPATTERN1    0x11
#define REG_TXPATTERN2    0x12
#define REG_TXPATTERN3    0x13
#define REG_TXPATTERN4    0x14
#define REG_TXPATTERN5    0x15
#define REG_TXPATTERN6    0x16
#define REG_TXPATTERN7    0x17
#define REG_TXPATTERN8    0x18
#define REG_TXPATTERN9    0x19
#define REG_TXPATTERN10   0x1A
#define REG_TXPATTERN11   0x1B
#define REG_RXFILTERS     0x1C
#define REG_RXGAINSET     0x1D
#define REG_CALIBRSET     0x1E
#define REG_COMMANDS      0x1F
#define REG_RXWAITED8     0x20
#define REG_RXWAITED0     0x21
#define REG_RSSI          0x22
#define REG_NEWPKTSIZ     0x23
#define REG_NEWPKTADR8    0x24
#define REG_NEWPKTADR0    0x25
#define REG_NEWPKTCRC8    0x26
#define REG_NEWPKTCRC0    0x27
#define REG_MAXNOILVL     0x28
#define REG_MINNOILVL     0x29
#define REG_MINPULAMP     0x2A
#define REG_MINSIGSTR     0x2B
#define REG_RXPERIOD8     0x2C
#define REG_RXPERIOD0     0x2D
#define REG_CHCLRCHK      0x2E
#define REG_NOISEWAIT     0x2F
#define REG_MAINCTRL      0x30
#define REG_MANUCTRL      0x31
#define REG_PREFRAME      0x32
#define REG_PREAMBLEN     0x33
#define REG_SYNCWORD8     0x34
#define REG_SYNCWORD0     0x35
#define REG_CRCPOLYNO8    0x36
#define REG_CRCPOLYNO0    0x37
#define REG_TXADDRESS8    0x38
#define REG_TXADDRESS0    0x39
#define REG_RXADDRESS8    0x3A
#define REG_RXADDRESS0    0x3B
#define REG_TXPKTSIZE     0x3C
#define REG_RXPKTSIZE     0x3D
#define REG_PKTCONFIG     0x3E
#define REG_FIFO          0x3F

// REG_INT1 0x00
#define BIT_WAKEUP        0x40
#define BIT_NOISYTX       0x20
#define BIT_TXEND         0x10
#define BIT_NEWPKT        0x08
#define BIT_ADDRMAT       0x04
#define BIT_BRDCAST       0x02
#define BIT_CRCERRO       0x01

// REG_INT2 0x01
#define BIT_CALIBRT       0x80
#define BIT_XOTIMER       0x40
#define BIT_RXTOUT        0x20
#define BIT_TXUDRFL       0x10
#define BIT_RXOVRFL       0x08
#define BIT_TXOVRFL       0x04
#define BIT_BUFLOAD       0x02
#define BIT_BUFSTOP       0x01 

// REG_SLEEPCFG 0x04
#define BIT_SLPDEPTH_DEEP     0xC0
#define BIT_SLPDEPTH_SHALLOW  0x40
#define BIT_SLPDEPTH_IDLE     0x00
#define BIT_AUSRXTO       0x20
#define BIT_AUSTXEND      0x10
#define BIT_AUSRXEND      0x08
#define BIT_AUSMATCH      0x04
#define BIT_AUSBRDCA      0x02
#define BIT_AUSNOISY      0x01

// REG_COMMANDS 0x1F
#define BIT_FLUSHTX       0x01
#define BIT_FLUSHRX       0x02
#define BIT_GOTOSLP       0x04
#define BIT_SYNTIMER      0x08
#define BIT_STARTTX       0x10
#define BIT_RXMODE        0x20
#define BIT_SKIPWAKE      0x40
#define BIT_CALIBRAT      0x80

#define BIT_REFPWRD       0x01
#define BIT_PLLPWRD       0x02
#define BIT_DCDCPWRD      0x04
#define BIT_DLLPWRD       0x08
#define BIT_TXPWRD        0x10
#define BIT_RXPWRD        0x20
#define BIT_RXIDLE        0x40
#define BIT_LEAD_LAG      0x80


#endif /*spark_h */
