/*
 * WM8731_Test.h
 *
 * Created: 2017-05-18 14:53:25
 *  Author: collerette_a
 */ 


#ifndef WM8731_TEST_H_
#define WM8731_TEST_H_

void PMPWaitBusy(void);
void Set_Index(unsigned short index);
void Write_Command( unsigned short cmd );
void Write_Data(unsigned int _data);

void main1(void);




#endif /* WM8731_TEST_H_ */