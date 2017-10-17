/*
 * clockgen.h
 *
 * Created: 2017-05-18 14:42:33
 *  Author: etudiant
 */ 


#ifndef CLOCKGEN_H_
#define CLOCKGEN_H_

//#include <samd21g18a.h>

#undef ENABLE // Clash between ASF and plain SAM

inline void clockInit(void)
{
	PM->APBAMASK.bit.SYSCTRL_ = 1;
	
	// Start XOSC32K
	const SYSCTRL_XOSC32K_Type xosc32k = {
		.bit.ENABLE = 1,
		.bit.XTALEN = 1,
		.bit.EN32K = 1,
		.bit.AAMPEN = 0,
		.bit.RUNSTDBY = 1,
		.bit.ONDEMAND = 0,
		.bit.STARTUP = 0,
		.bit.WRTLOCK = 0
	};
	SYSCTRL->XOSC32K = xosc32k;
	
	// Start XOSC with 20.48 MHz crystal
	//const SYSCTRL_XOSC_Type xosc = {
		//.bit.AMPGC = 0,
		//.bit.ENABLE = 1,
		//.bit.GAIN = 0x4,
		//.bit.ONDEMAND = 0,
		//.bit.RUNSTDBY = 1,
		//.bit.STARTUP = 0xf,
		//.bit.XTALEN = 1
	//};
	//SYSCTRL->XOSC = xosc;
	
	// Setup FDPLL96M for 20.48 MHz, integer factor of 625
	const SYSCTRL_DPLLCTRLA_Type dpllctrla = {
		.bit.ENABLE = 1,
		.bit.ONDEMAND = 0,
		.bit.RUNSTDBY = 1
	};
	SYSCTRL->DPLLCTRLA = dpllctrla;
	
	const SYSCTRL_DPLLCTRLB_Type dpllctrlb = {
		.bit.FILTER = 0,
		.bit.LPEN = 0,
		.bit.WUF = 0,
		.bit.REFCLK = 0,
		.bit.LTIME = 0,
		.bit.LBYPASS = 1,
		.bit.DIV = 0
	};
	SYSCTRL->DPLLCTRLB = dpllctrlb;
	
	const SYSCTRL_DPLLRATIO_Type dpllratio = {
		.bit.LDR = 1249u,
		//.bit.LDR = 1500u,
		.bit.LDRFRAC = 0
	};
	SYSCTRL->DPLLRATIO = dpllratio;
	
	// Set divider for clock controller 6
	const GCLK_GENDIV_Type gendiv = {
		.bit.ID = GCLK_CLKCTRL_GEN_GCLK6_Val,
		.bit.DIV = 2
	};
	GCLK->GENDIV = gendiv;
	
	// Feed FDPLL to clock controller 6
	const GCLK_GENCTRL_Type gentrcl = {
		.bit.ID =  GCLK_CLKCTRL_GEN_GCLK6_Val,
		//.bit.SRC = GCLK_GENCTRL_SRC_XOSC_Val,
		.bit.SRC = GCLK_GENCTRL_SRC_FDPLL_Val,
		.bit.GENEN = 1,
		.bit.IDC = 0,
		.bit.OOV = 0,
		.bit.OE = 1,
		.bit.DIVSEL = 0,
		.bit.RUNSTDBY = 1
	};
	GCLK->GENCTRL = gentrcl;
	
	// Set divider for clock controller 2
	const GCLK_GENDIV_Type gendiv2 = {
		.bit.ID = GCLK_CLKCTRL_GEN_GCLK2_Val,
		.bit.DIV = 0
	};
	GCLK->GENDIV = gendiv2;
	
	// Feed FDPLL to clock controller 2
	const GCLK_GENCTRL_Type gentrcl2 = {
		.bit.ID =  GCLK_CLKCTRL_GEN_GCLK2_Val,
		//.bit.SRC = GCLK_GENCTRL_SRC_XOSC_Val,
		.bit.SRC = GCLK_GENCTRL_SRC_FDPLL_Val,
		.bit.GENEN = 1,
		.bit.IDC = 0,
		.bit.OOV = 0,
		.bit.OE = 1,
		.bit.DIVSEL = 0,
		.bit.RUNSTDBY = 1
	};
	GCLK->GENCTRL = gentrcl2;
	
	// Multiplex pin PA22 to GCLK_IO[6]
	const PORT_WRCONFIG_Type wrconfig = {
		.bit.WRPINCFG = 1,
		.bit.WRPMUX = 1,
		.bit.PMUX =  MUX_PA22H_GCLK_IO6,
		.bit.PMUXEN = 1,
		.bit.HWSEL = 1,
		.bit.PINMASK = (uint16_t)((PORT_PA22) >> 16)
	};
	PORT->Group[0].WRCONFIG.reg = wrconfig.reg;
}

inline void clockEnable(void)
{
	const GCLK_GENCTRL_Type gentrcl = {
		.bit.ID =  GCLK_CLKCTRL_GEN_GCLK6_Val,
		.bit.SRC = GCLK_GENCTRL_SRC_FDPLL_Val,
		.bit.GENEN = 1,
		.bit.IDC = 0,
		.bit.OOV = 0,
		.bit.OE = 1,
		//.bit.OE = 0,
		.bit.DIVSEL = 0,
		.bit.RUNSTDBY = 1
	};
	GCLK->GENCTRL = gentrcl;
}

inline void clockDisable(void)
{
	const GCLK_GENCTRL_Type gentrcl = {
		.bit.ID =  GCLK_CLKCTRL_GEN_GCLK6_Val,
		.bit.SRC = GCLK_GENCTRL_SRC_FDPLL_Val,
		.bit.GENEN = 1,
		.bit.IDC = 0,
		.bit.OOV = 0,
		.bit.OE = 0,
		.bit.DIVSEL = 0,
		.bit.RUNSTDBY = 1
	};
	GCLK->GENCTRL = gentrcl;
}



#endif /* CLOCKGEN_H_ */