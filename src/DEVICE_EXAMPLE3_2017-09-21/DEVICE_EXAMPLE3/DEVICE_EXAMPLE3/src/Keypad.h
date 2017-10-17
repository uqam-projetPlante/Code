/*
 * Keypad.h
 *
 * Created: 2017-09-22 16:45:58
 *  Author: collerette_a
 */ 

#undef ENABLE // Clash between ASF and plain SAM

#ifndef KEYPAD_H_
#define KEYPAD_H_

#define	PORT_ROW1	PORT_PB05 
#define GROUP_ROW1	1
#define PIN_ROW1	5
#define	PORT_ROW2	PORT_PB03
#define GROUP_ROW2	1
#define PIN_ROW2	3
#define	PORT_ROW3	PORT_PB07
#define GROUP_ROW3	1
#define PIN_ROW3	7
#define	PORT_ROW4	PORT_PB01
#define GROUP_ROW4	1
#define PIN_ROW4	1

#define PORT_COL1	PORT_PB04
#define GROUP_COL1	1
#define PIN_COL1	4
#define PORT_COL2	PORT_PB02
#define GROUP_COL2	1
#define PIN_COL2	2
#define	PORT_COL3	PORT_PB06
#define GROUP_COL3	1
#define PIN_COL3	6
#define	PORT_COL4	PORT_PB00
#define GROUP_COL4	1
#define PIN_COL4	0

int checkKey();
int readKey();
char keypadScan(void);
void keypadInit(void);



#endif /* KEYPAD_H_ */