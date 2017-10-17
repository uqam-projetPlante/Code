#pragma once

#include <stdbool.h>
#include "sam.h"

typedef struct Pin {
	uint8_t mux;
	uint8_t group;
	uint8_t pin;
	uint8_t chan;
	uint8_t tcc_id;
	uint8_t cc_chan;
	uint8_t alt_mux;
} Pin;

const extern char *git_version;

inline static void pin_mux(Pin p) {
  if (p.pin & 1) {
    PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXO = p.mux;
  } else {
    PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXE = p.mux;
  }

  PORT->Group[p.group].PINCFG[p.pin].bit.PMUXEN = 1;
}

// TODO: Somehow integrate with pin_mux
inline static void pin_alt_mux(Pin p) {
  if (p.pin & 1) {
    PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXO = p.alt_mux;
  } else {
    PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXE = p.alt_mux;
  }

  PORT->Group[p.group].PINCFG[p.pin].bit.PMUXEN = 1;
}

inline static void pin_analog(Pin p) {
  if (p.pin & 1) {
    PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXO = 0x1;
  } else {
    PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXE = 0x1;
  }

  PORT->Group[p.group].PINCFG[p.pin].bit.PMUXEN = 1;
}

inline static void pin_gpio(Pin p) {
  PORT->Group[p.group].PINCFG[p.pin].bit.PMUXEN = 0;
}

inline static void pin_out(Pin p) {
  pin_gpio(p);
  PORT->Group[p.group].DIRSET.reg = (1<<p.pin);
}

inline static void pin_dir(Pin p, bool out) {
  if (out) {
    PORT->Group[p.group].DIRSET.reg = (1<<p.pin);
  } else {
    PORT->Group[p.group].DIRCLR.reg = (1<<p.pin);
  }
}

inline static void pin_high(Pin p) {
  PORT->Group[p.group].OUTSET.reg = (1<<p.pin);
}

inline static void pin_low(Pin p) {
  PORT->Group[p.group].OUTCLR.reg = (1<<p.pin);
}

inline static void pin_toggle(Pin p) {
  PORT->Group[p.group].OUTTGL.reg = (1<<p.pin);
}

inline static void pin_set(Pin p, bool high) {
  if (high) {
    PORT->Group[p.group].OUTSET.reg = (1<<p.pin);
  } else {
    PORT->Group[p.group].OUTCLR.reg = (1<<p.pin);
  }
}

inline static void pin_in(Pin p) {
  pin_gpio(p);
  PORT->Group[p.group].PINCFG[p.pin].bit.INEN = 1;
  PORT->Group[p.group].DIRCLR.reg = (1<<p.pin);
}

inline static void pin_pull_up(Pin p) {
  pin_in(p);
  PORT->Group[p.group].PINCFG[p.pin].bit.PULLEN = 1;
  pin_high(p);
}

inline static void pin_pull_down(Pin p) {
  pin_in(p);
  PORT->Group[p.group].PINCFG[p.pin].bit.PULLEN = 1;
  pin_low(p);
}

inline static void pin_float(Pin p) {
  pin_in(p);
  PORT->Group[p.group].PINCFG[p.pin].bit.PULLEN = 0;
}

inline static bool pin_read(Pin p) {
  return (PORT->Group[p.group].IN.reg & (1<<p.pin)) != 0;
}

inline static void pin_mux_eic(Pin p) {
    if (p.pin & 1) {
      PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXO = 0;
    } else {
      PORT->Group[p.group].PMUX[p.pin/2].bit.PMUXE = 0;
    }

    PORT->Group[p.group].PINCFG[p.pin].bit.PMUXEN = 1;
}

inline static void eic_init() {
    PM->APBAMASK.reg |= PM_APBAMASK_EIC;

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0) |
        GCLK_CLKCTRL_ID(EIC_GCLK_ID);

    EIC->CTRL.reg = EIC_CTRL_ENABLE;
}

inline static uint8_t pin_extint(Pin p) {
  return p.pin % 16;
}

#define   EIC_CONFIG_SENSE_NONE      0x0u   /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE_RISE      0x1u   /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE_FALL      0x2u   /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE_BOTH      0x3u   /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE_LEVEL     0x4u   /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE_HIGH      0x4u   /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE_LOW       0x5u   /**< \brief (EIC_CONFIG) Low level detection */

inline static void eic_config(Pin p, uint8_t config) {
  uint8_t i = pin_extint(p);
  uint8_t pos = (i % 8) * 4;
  EIC->CONFIG[i/8].reg = (EIC->CONFIG[i/8].reg & ~(0xf << pos)) | (config << pos);
}

inline static uint8_t eic_read_config(Pin p) {
  uint8_t i = pin_extint(p);
  uint8_t pos = (i % 8) * 4;
  return (EIC->CONFIG[i/8].reg >> pos) & 0xf;
}

inline static void evsys_init() {
    PM->APBCMASK.reg |= PM_APBCMASK_EVSYS;
}

#define EVSYS_USER_NONE -1

inline static void evsys_config(uint8_t channel, uint8_t source, uint8_t user) {
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0) |
        GCLK_CLKCTRL_ID(EVSYS_GCLK_ID_0 + channel);

    EVSYS->CHANNEL.reg = EVSYS_CHANNEL_CHANNEL(channel)
                       | EVSYS_CHANNEL_EVGEN(source)
                       | EVSYS_CHANNEL_PATH_SYNCHRONOUS | EVSYS_CHANNEL_EDGSEL_RISING_EDGE;

    if (user != EVSYS_USER_NONE) {
      EVSYS->USER.reg = EVSYS_USER_CHANNEL(channel + 1) | EVSYS_USER_USER(user);
    }
}

#define EVSYS_EVD(N) ((N)<=7 ? (1<<((N) + 8)) : (1 << (24 + (N) - 8)))

// clock.c
void gclk_enable(uint32_t id, uint32_t src, uint32_t div);
void clock_init_usb(uint8_t clk_system);
void clock_init_crystal(uint8_t clk_system, uint8_t clk_32k);
