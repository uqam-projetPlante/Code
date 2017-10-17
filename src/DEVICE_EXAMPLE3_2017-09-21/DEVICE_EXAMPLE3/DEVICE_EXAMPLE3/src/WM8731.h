/*
 * WM8731.h
 *
 * Created: 2017-05-18 13:17:47
 *  Author: collerette_a
 */ 


#ifndef WM8731_H_
#define WM8731_H_

#define WM8731_DATA_LENGTH 2
#define WM8731_SLAVE_ADDR 0x1a

void WM8731_Init(void);
void WM8731_CMD(char address, unsigned int cmd);
void WM8731_SetVolume(char LeftCh, char RightCh);
void WM8731_Activate(void);
void WM8731_Deactivate(void);



#endif /* WM8731_H_ */