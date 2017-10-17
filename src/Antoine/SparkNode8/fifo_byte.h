/* Generic FIFO buffer Implementation
 * fifo_byte.h
 *
 * Created: 5/23/2017 11:12:05 PM
 *  Author: obrini
 */ 


#ifndef FIFO_BYTE_H_
#define FIFO_BYTE_H_

/*-------------------------------------------------------------*/
/*		Includes and dependencies			*/
/*-------------------------------------------------------------*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*-------------------------------------------------------------*/
/*		Typedefs enums & structs			*/
/*-------------------------------------------------------------*/

/**
 * This structure holds the data that defines a circular buffer
 */

typedef struct
{
	uint16_t* buffer;
	uint32_t head;
	uint32_t tail;
	size_t size;
}fifo_t; 

/*-------------------------------------------------------------*/
/*		Functions				*/
/*-------------------------------------------------------------*/

/**
 * @brief Checks if the FIFO is full.
 */
inline bool isFull(fifo_t* fifo)
{
	if ((fifo->head == (fifo->size - 1) && fifo->tail == 0) || (fifo->head == (fifo->tail - 1)))
	return true;
	else
	return false;
}

/**
 * @brief Checks if the FIFO is empty
 */
//bool isEmpty(fifo_t* fifo);
inline bool isEmpty(fifo_t* fifo)
{
	if (fifo->head == fifo->tail)
	return true;
	else
	return false;
}

/**
 * @brief Creates a FIFO using dynamic memory
 *
 * This function is used to create a buffer, it allocates memory for a buffer of
 * the requested size
 */
inline fifo_t* createFifo(size_t size)
{
	uint16_t* buffer = (uint16_t*)malloc(size);

	if (buffer == NULL)
	return NULL;

	fifo_t* fifo = (fifo_t*)malloc(sizeof(fifo_t));

	if (fifo == NULL)
	{
		free(buffer);
		return NULL;
	}

	fifo->buffer = buffer;
	fifo->head = 0;
	fifo->tail = 0;
	fifo->size = size / 2;

	return fifo;
}

#define CHECK_FIFO_NULL(fifo) if (fifo == NULL) return 0;

/**
 * @brief Adds one byte to the FIFO buffer
 */
inline size_t push(fifo_t* fifo, uint16_t byte)
{
	/*
	CHECK_FIFO_NULL(fifo);
*/
	if (isFull(fifo) == true)
	return 0;

	fifo->buffer[fifo->head] = byte;

	fifo->head++;
	if (fifo->head == fifo->size)
	fifo->head = 0;

	return 1;
}

/**
 * @brief Obtains one byte from the FIFO buffer
 */
inline size_t pop(fifo_t* fifo, uint16_t *byte)
{
	/*
	CHECK_FIFO_NULL(fifo);
*/
	if (isEmpty(fifo) == true)
	{
		//*byte = 0x00;
		*byte = 0x7F; // We do hear plops in UPKEEP mode from here, why is isEmpty not triggered?
		return 0;
	}
	

	*byte = fifo->buffer[fifo->tail];

	fifo->tail++;
	if (fifo->tail == fifo->size)
	fifo->tail = 0;

	return 1;
}

/**
 * @brief Returns the number of elements in the FIFO
 */
inline size_t elements(fifo_t* fifo)
{
	if (fifo->head == fifo->tail)
	return 0;
	else if ((fifo->head == (fifo->size - 1) && fifo->tail == 0) || (fifo->head == (fifo->tail - 1)))
	return fifo->size;
	else if (fifo->head < fifo->tail)
	return (fifo->head) + (fifo->size - fifo->tail);
	else
	return fifo->head - fifo->tail;
}



#endif /* FIFO_BYTE_H_ */