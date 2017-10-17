/*
 * audio.h
 *
 * Created: 2017-07-06 13:51:57
 *  Author: etudiant
 */ 


#ifndef AUDIO_H_
#define AUDIO_H_

// Vars
extern uint8_t audioDecimate;

// Audio application layer
void audioResetStatistics(void);
void audioPrintStatistics(void);
void audioStart(void);
void audioStop(void);
void audioReset(void);
void audioConfigure(void);
bool audioProcessPacket(unsigned char numBytes);
void audioFillPacket(unsigned char numBytes);
void audioUpkeepPacket(void);

#endif /* AUDIO_H_ */