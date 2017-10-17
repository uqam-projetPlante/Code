/*
 * keypad.c
 *
 * Created: 2017-09-22 16:46:17
 *  Author: collerette_a
 */ 
//-----------------------------
// code for 4x4 keypad scanning
//---------------------------
#include <asf.h>
#include "delay.h"
#include "keypad.h"

char keyPadMatrix[] =
{
	'1','2','3','A',
	'4','5','6','B',
	'7','8','9','C',
	'*','0','#','D',
	0xFF
};

	void keypadInit()
	{	
		//set rows as outputs
		REG_PORT_DIRSET1 = PORT_ROW1;
		REG_PORT_OUTCLR1 = PORT_ROW1;
		
		REG_PORT_DIRSET1 = PORT_ROW2;
		REG_PORT_OUTCLR1 = PORT_ROW2;
		
		REG_PORT_DIRSET1 = PORT_ROW3;
		REG_PORT_OUTCLR1 = PORT_ROW3;
		
		REG_PORT_DIRSET1 = PORT_ROW4;
		REG_PORT_OUTCLR1 = PORT_ROW4;
		
		//set cols as inputs
		REG_PORT_DIRCLR1 = PORT_COL1;
		PORT->Group[GROUP_COL1].PINCFG[PIN_COL1].bit.INEN = 1;
		PORT->Group[GROUP_COL1].PINCFG[PIN_COL1].bit.PULLEN = 1;
		REG_PORT_CTRL1 = PORT_COL1;
		
		REG_PORT_DIRCLR1 = PORT_COL2;
		PORT->Group[GROUP_COL2].PINCFG[PIN_COL2].bit.INEN = 1;
		PORT->Group[GROUP_COL2].PINCFG[PIN_COL2].bit.PULLEN = 1;
		REG_PORT_CTRL1 = PORT_COL2;
		
		REG_PORT_DIRCLR1 = PORT_COL3;
		PORT->Group[GROUP_COL3].PINCFG[PIN_COL3].bit.INEN = 1;
		PORT->Group[GROUP_COL3].PINCFG[PIN_COL3].bit.PULLEN = 1;
		REG_PORT_CTRL1 = PORT_COL3;
		
		REG_PORT_DIRCLR1 = PORT_COL4;
		PORT->Group[GROUP_COL4].PINCFG[PIN_COL4].bit.INEN = 1;
		PORT->Group[GROUP_COL4].PINCFG[PIN_COL4].bit.PULLEN = 1;
		REG_PORT_CTRL1 = PORT_COL4;
	}

	char keypadScan()
	{
		 //This routine returns the first key found to be pressed during the scan.
		char key = 16;
		
		REG_PORT_OUTSET1 = PORT_ROW1;
		
		if(REG_PORT_IN1&PORT_COL1)
		key = 0; 
		if(REG_PORT_IN1&PORT_COL2)
		key = 1;
		if(REG_PORT_IN1&PORT_COL3)
		key = 2;
		if(REG_PORT_IN1&PORT_COL4)
		key = 3;

		REG_PORT_OUTCLR1 = PORT_ROW1;
		
		REG_PORT_OUTSET1 = PORT_ROW2;
		
		if(REG_PORT_IN1&PORT_COL1)
		key = 4;
		if(REG_PORT_IN1&PORT_COL2)
		key = 5;
		if(REG_PORT_IN1&PORT_COL3)
		key = 6;
		if(REG_PORT_IN1&PORT_COL4)
		key = 7;

		REG_PORT_OUTCLR1 = PORT_ROW2;

		REG_PORT_OUTSET1 = PORT_ROW3;
		
		if(REG_PORT_IN1&PORT_COL1)
		key = 8;
		if(REG_PORT_IN1&PORT_COL2)
		key = 9;
		if(REG_PORT_IN1&PORT_COL3)
		key = 10;
		if(REG_PORT_IN1&PORT_COL4)
		key = 11;

		REG_PORT_OUTCLR1 = PORT_ROW3;
		
		REG_PORT_OUTSET1 = PORT_ROW4;
		
		if(REG_PORT_IN1&PORT_COL1)
		key = 12;
		if(REG_PORT_IN1&PORT_COL2)
		key = 13;
		if(REG_PORT_IN1&PORT_COL3)
		key = 14;
		if(REG_PORT_IN1&PORT_COL4)
		key = 15;

		REG_PORT_OUTCLR1 = PORT_ROW4;			
		
		return keyPadMatrix[key];
	}
//
	//------------ debouncing -----------------
	//int checkKey( void)
	//{
		//int i=0, r=0, j=0;
		//int c;
//
		 //wait for a key pressed
		//do{
			//timerDelay( 10);
			//if ((c = readKEY()))
			//{
				//if ( c>r)       // if more than one button pressed
				//r = c;
				//i++;
			//}
			//else
			//i=0;
		//} while ( i<10);
//
		 //wait for key release
		//i =0;
		//do {
			//timerDelay( 10);
			//if ( (c = readKEY()))
			//{
				//if (c>r)        // if more then one button pressed
				//r = c;
				//i=0;
				//j++;            // detect long button hold
			//}
			//else
			//i++;
		//} while ( i<10);
//
		//if ( j>80)                  // longer than 500ms
		//r+=0x80;
//
		//return r;
	//}