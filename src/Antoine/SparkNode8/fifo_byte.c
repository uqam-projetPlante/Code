#include "fifo_byte.h"

//fifo_t* createFifo(size_t size)
//{
	//uint8_t* buffer = (uint8_t*)malloc(size);
//
	//if (buffer == NULL)
	//return NULL;
//
	//fifo_t* fifo = (fifo_t*)malloc(sizeof(fifo_t));
//
	//if (fifo == NULL)
	//{
		//free(buffer);
		//return NULL;
	//}
//
	//fifo->buffer = buffer;
	//fifo->head = 0;
	//fifo->tail = 0;
	//fifo->size = size;
//
	//return fifo;
//}
//
//#define CHECK_FIFO_NULL(fifo) if (fifo == NULL) return 0;
//
//size_t push(fifo_t* fifo, uint8_t byte)
//{
	//CHECK_FIFO_NULL(fifo);
//
	//if (isFull(fifo) == true)
	//return 0;
//
	//fifo->buffer[fifo->head] = byte;
//
	//fifo->head++;
	//if (fifo->head == fifo->size)
	//fifo->head = 0;
//
	//return 1;
//}
//
//size_t pop(fifo_t* fifo, uint8_t *byte)
//{
	//CHECK_FIFO_NULL(fifo);
//
	//if (isEmpty(fifo) == true)
	//return 0;
//
	//*byte = fifo->buffer[fifo->tail];
//
	//fifo->tail++;
	//if (fifo->tail == fifo->size)
	//fifo->tail = 0;
//
	//return 1;
//}
//
//bool isFull(fifo_t* fifo)
//{
	//if ((fifo->head == (fifo->size - 1) && fifo->tail == 0) || (fifo->head == (fifo->tail - 1)))
	//return true;
	//else
	//return false;
//}
//
//bool isEmpty(fifo_t* fifo)
//{
	//if (fifo->head == fifo->tail)
	//return true;
	//else
	//return false;
//}
//
//size_t elements(fifo_t* fifo)
//{
	//if (fifo->head == fifo->tail)
	//return 0;
	//else if ((fifo->head == (fifo->size - 1) && fifo->tail == 0) || (fifo->head == (fifo->tail - 1)))
	//return fifo->size;
	//else if (fifo->head < fifo->tail)
	//return (fifo->head) + (fifo->size - fifo->tail);
	//else
	//return fifo->head - fifo->tail;
//}