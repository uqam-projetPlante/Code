/*
 * flash.h
 *
 * Created: 2017-05-17 15:19:03
 *  Author: collerette_a
 */ 


#ifndef FLASH_H_
#define FLASH_H_


void configure_nvm(void);

//#include <samd21j18a.h>
#include <stdbool.h>

#undef ENABLE // Clash between ASF and plain SAM

/*- Definitions -------------------------------------------------------------*/
#define NVM_MEMORY        ((volatile uint16_t *)FLASH_ADDR)
#define FLASH_ROW_PAGES 4

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------

void flash_init(void);
void flash_read_buffer(
const uint32_t source_address,
uint8_t *const buffer,
uint16_t length);
void flash_write_buffer(
const uint32_t destination_address,
const uint8_t *buffer,
uint16_t length);
void flash_erase_row(const uint32_t row_address);

#endif /* FLASH_H_ */