/*
 * flash.c
 *
 * Created: 2017-05-17 15:17:38
 *  Author: collerette_a
 */ 

#include <asf.h>
#include "flash.h"

void configure_nvm(void)
{
	struct nvm_config config_nvm;
	nvm_get_config_defaults(&config_nvm);
	config_nvm.manual_page_write = false;
	nvm_set_config(&config_nvm);
}
