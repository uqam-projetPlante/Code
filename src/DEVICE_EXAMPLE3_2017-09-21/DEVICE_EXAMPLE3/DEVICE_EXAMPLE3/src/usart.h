/*
 * usart.h
 *
 * Created: 2017-10-10 12:50:29
 *  Author: collerette_a
 */ 


#ifndef USART_H_
#define USART_H_

#define USART_TIMEOUT 0xFFFF

void uart_init(void);
void uart_sendBuffer(const uint8_t *tx_data, uint16_t length);
uart_send(uint8_t tx_data);
uint8_t uart_read(void);



#endif /* USART_H_ */