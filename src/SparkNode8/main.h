/*
 * main.h
 *
 * Created: 2017-05-19 16:02:44
 *  Author: etudiant
 */ 


#ifndef MAIN_H_
#define MAIN_H_

// Compare two bytes and return number of bit errors (0-8)
inline uint8_t byteErrors(uint8_t b1, uint8_t b2)
{
	uint8_t errors = 0;
	
	if( (b1 & 1) != (b2 & 1) )
		errors++;
	if( (b1 & 2) != (b2 & 2) )
		errors++;
	if( (b1 & 4) != (b2 & 4) )
		errors++;
	if( (b1 & 8) != (b2 & 8) )
		errors++;
	if( (b1 & 16) != (b2 & 16) )
		errors++;
	if( (b1 & 32) != (b2 & 32) )
		errors++;
	if( (b1 & 64) != (b2 & 64) )
		errors++;
	if( (b1 & 128) != (b2 & 128) )
		errors++;
	
	return errors;
}

#endif /* MAIN_H_ */