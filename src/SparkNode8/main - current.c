#include "common/hw.h"
#include "usb.h"

#define GCLK_SYSTEM 0
#define GCLK_32K    2

int main(void)
{
/*    if (PM->RCAUSE.reg & (PM_RCAUSE_POR | PM_RCAUSE_BOD12 | PM_RCAUSE_BOD33 | PM_RCAUSE_SYST)) {
        // On powerup, force a clean reset of the MT7620
        pin_low(PIN_SOC_RST);
        pin_out(PIN_SOC_RST);

        // turn off 3.3V to SoC
        pin_low(PIN_SOC_PWR);
        pin_out(PIN_SOC_PWR);

        // pull 1.8V low
        pin_low(PIN_18_V);
        pin_out(PIN_18_V);

        clock_init_crystal(GCLK_SYSTEM, GCLK_32K);
        timer_clock_enable(TC_BOOT);

        // hold everything low
        boot_delay_ms(50); // power off for 50ms

        pin_high(PIN_SOC_PWR);

        boot_delay_ms(2); // 2ms until 1.8 rail comes on

        pin_high(PIN_18_V);

        boot_delay_ms(50); // 50ms before soc rst comes on
    } else {*/
        clock_init_crystal(GCLK_SYSTEM, GCLK_32K);
//    }

	PORT->Group[0].PINCFG[24].bit.PMUXEN = 1;
	PORT->Group[0].PINCFG[25].bit.PMUXEN = 1;
	PORT->Group[0].PMUX[12].reg = 0x66;

	usb_init();
	usb_attach();

	while (true)
		usb_cb_handle_ep_in(1);
}
