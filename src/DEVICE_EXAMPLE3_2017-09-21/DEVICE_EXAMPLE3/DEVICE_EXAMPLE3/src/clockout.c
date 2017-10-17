/*
 * clock.c
 *
 * Created: 2017-05-08 17:16:49
 *  Author: collerette_a
 */ 

#include "clockout.h"
#include <asf.h>

void clockout_config()
{
	/* MUX configuration*/
	struct system_pinmux_config pinmux_conf;
	system_pinmux_get_config_defaults(&pinmux_conf);
	
	/* 32 khz Crystal output*/
	pinmux_conf.mux_position = PINMUX_PB15H_GCLK_IO1;
	pinmux_conf.direction    = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	system_pinmux_pin_set_config(EXT2_PIN_10, &pinmux_conf);
	
	/* Main clock output*/
	pinmux_conf.mux_position = PINMUX_PB12H_GCLK_IO6;
	pinmux_conf.direction    = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	system_pinmux_pin_set_config(EXT2_PIN_7, &pinmux_conf);
}