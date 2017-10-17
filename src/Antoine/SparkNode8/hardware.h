/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HARDWARE_H_
#define HARDWARE_H_

//#include <asf.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "sam.h"
#include "spi_sercom0.h"
#include "spi_sercom1.h"
#include "delay.h"
#include "clockgen.h"

// Setup
void resetRadio(void);
void boardSetup(void);
void enableExtInt(void);
void disableExtInt(void);
bool getMCUint(void);
void toggleLED(void);

// Clock routines
void enableClockGate(void);
void disableClockGate(void);

// Timer routines
inline void delay(unsigned int us)
{
	timerDelay(us);
}

// SPI routines
void startSPI(void);
unsigned char sendSPI(unsigned char cmd);
void pushSPI(unsigned char *data, unsigned char numdata);
void pullSPI(unsigned char *data, unsigned char numdata);
void stopSPI(void);

// USB routines
extern char std_buf[128];

bool usbIsAvailable(void);
uint16_t usbBytesAvailable(void);
uint8_t usbGet(void);
void usbPut(uint8_t c);
void usbPrintf(const char *fmt, ...);
uint32_t usbParseInt();

// Watchdog, 32KHz timer
void wdSetup(unsigned short ticks);
void wdReset();
void wdEnable();
void wdDisable();

#endif /* HARDWARE_H_ */
