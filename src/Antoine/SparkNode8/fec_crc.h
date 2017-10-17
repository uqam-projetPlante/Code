//
//  fec_crc.h
//  
//
//  Created by Michiel Soer on 18/11/16.
//
//

#ifndef fec_crc_h
#define fec_crc_h

#include <string.h> // For memset

#ifdef ARDUINO
  #pragma GCC optimize ("-O3") // Crazily optimize for speed
  #include <Arduino.h>
#endif

/************************************************************************************************************** 
 * FUNCTION PROTOTYPES
 */
//unsigned short fecEncode(unsigned char *pOutData, unsigned char *pInData, unsigned short nRemBytes);
//unsigned short fecDecode(unsigned char *pDecData, unsigned char* pInData, unsigned short RemBytes);
//static unsigned char hammWeight(unsigned char a);
//static unsigned char my_min(unsigned char a, unsigned char b);
//static unsigned short calcCRC(unsigned char crcData, unsigned short crcReg);
//int test_fec(void);

/************************************************************************************************************** 
 * GLOBAL VARIABLES
 */

// Look-up source state index when:
// Destination state --\ /-- Each of two possible source states
const unsigned char aTrellisSourceStateLut[8][2] =
{
    {0, 4}, {0, 4}, {1, 5}, {1, 5}, {2, 6}, {2, 6}, {3, 7}, {3, 7},
};
// State {0,4} -> State 0 // State {0,4} -> State 1 // State {1,5} -> State 2 // State {1,5} -> State 3 // State {2,6} -> State 4 // State {2,6} -> State 5 // State {3,7} -> State 6 // State {3,7} -> State 7
// Look-up expected output when:
// Destination state --\

const unsigned char aTrellisTransitionOutput[8][2] =
{
{0, 3}, {3, 0}, {1, 2}, {2, 1}, {3, 0}, {0, 3}, {2, 1}, {1, 2},
};

// Look-up input bit at encoder when:
// Destination state --\

const unsigned char aTrellisTransitionInput[8] =
{
0, 1, 0, 1, 0, 1, 0, 1,
};


inline unsigned char hammWeight(unsigned char a)
{
    a = ((a & 0xAA) >> 1) + (a & 0x55);
    a = ((a & 0xCC) >> 2) + (a & 0x33);
    a = ((a & 0xF0) >> 4) + (a & 0x0F);
    return a;
}

inline unsigned char my_min(unsigned char a, unsigned char b)
{
    return (a <= b ? a : b);
}

// Start with a uint16_t filled with 0xFFFF, then apply calcCRC
inline unsigned short calcCRC(unsigned char crcData, unsigned short crcReg)
{
	/*
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
            crcReg = (crcReg << 1) ^ 0x8005;
        else
            crcReg = (crcReg << 1); crcData <<= 1;
    }*/
	if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
		crcReg = (crcReg << 1) ^ 0x8005;
	else
		crcReg = (crcReg << 1); crcData <<= 1;
	if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
		crcReg = (crcReg << 1) ^ 0x8005;
	else
		crcReg = (crcReg << 1); crcData <<= 1;
	if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
		crcReg = (crcReg << 1) ^ 0x8005;
	else
		crcReg = (crcReg << 1); crcData <<= 1;
	if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
		crcReg = (crcReg << 1) ^ 0x8005;
	else
		crcReg = (crcReg << 1); crcData <<= 1;
	if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
		crcReg = (crcReg << 1) ^ 0x8005;
	else
		crcReg = (crcReg << 1); crcData <<= 1;
	if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
		crcReg = (crcReg << 1) ^ 0x8005;
	else
		crcReg = (crcReg << 1); crcData <<= 1;
	if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
		crcReg = (crcReg << 1) ^ 0x8005;
	else
		crcReg = (crcReg << 1); crcData <<= 1;
	if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
		crcReg = (crcReg << 1) ^ 0x8005;
	else
		crcReg = (crcReg << 1); crcData <<= 1;
		
    return crcReg;
}

// Encodes two bytes at a time into four bytes
// Returns number of encoded bytes
inline unsigned short fecEncode(unsigned char *pOutData, unsigned char *pInData, unsigned short nRemBytes)
{
    unsigned short fecEncodeTable[] = {0, 3, 1, 2, 3, 0, 2, 1, 3, 0, 2, 1, 0, 3, 1, 2};
    static unsigned short fecReg;
    unsigned short fecOutput;
    unsigned char preInterleave[4];
    unsigned char i = 0;
    unsigned char j = 0;
    
    if (pInData==NULL){    
        fecReg = 0; //Initialize CRC
        return(0);
    }
    
    // FEC
    for(i=0;i<2;i++)
    {
        fecOutput = 0;
        fecReg = (fecReg & 0x700) | (pInData[i] & 0xFF);
        for (j = 0; j < 8; j++) {
            fecOutput = (fecOutput << 2) | fecEncodeTable[fecReg >> 7];
            fecReg = (fecReg << 1) & 0x7FF;
        }
        preInterleave[i << 1] = fecOutput >> 8;
        preInterleave[(i << 1)+1] = fecOutput & 0xFF;
    }
    
    // Interleave and write to output
    /*
    {
        //unsigned char aDeintData[4];
        signed char iOut;
        signed char iIn;
        
        // De-interleave received data (and change pInData to point to de-interleaved data)
        for (iOut = 0; iOut < 4; iOut++)
        {
            unsigned char dataByte = 0;
            for (iIn = 3; iIn >= 0; iIn--)
                dataByte = (dataByte << 2) | ((preInterleave[iIn] >>( iOut << 1)) & 0x03);
            pOutData[iOut] = dataByte;
        }
        
    }*/
    /*
    {
      pOutData[0] =  preInterleave[0] & 0x03;

      pOutData[1] =  preInterleave[0] & 0x0c;
      pOutData[2] =  preInterleave[0] & 0x30;
      pOutData[3] =  preInterleave[0] & 0xc0;
      
      pOutData[1] = pOutData[1] >> 1; pOutData[1] = pOutData[1] >> 1;
      
      pOutData[2] = pOutData[2] >> 1; pOutData[2] = pOutData[2] >> 1;
      
      pOutData[3] = pOutData[3] >> 1; pOutData[3] = pOutData[3] >> 1;  
      
      pOutData[3] |= preInterleave[1] & 0xc0;
      
      pOutData[2] |= preInterleave[1] & 0x30;
      
      pOutData[1] |= preInterleave[1] & 0x0c;
      
      preInterleave[1] = preInterleave[1] << 1; preInterleave[1] = preInterleave[1] << 1;
      
      pOutData[0] |= preInterleave[1] & 0x0c;

      pOutData[2] = pOutData[2] >> 1; pOutData[2] = pOutData[2] >> 1;
      pOutData[3] = pOutData[3] >> 1; pOutData[3] = pOutData[3] >> 1;

      pOutData[3] |= preInterleave[2] & 0xc0;
      pOutData[2] |= preInterleave[2] & 0x30;
      preInterleave[2] = preInterleave[2] << 1; preInterleave[2] = preInterleave[2] << 1;
      pOutData[1] |= preInterleave[2] & 0xc0;
      preInterleave[2] = preInterleave[2] << 1; preInterleave[2] = preInterleave[2] << 1;
      pOutData[0] |= preInterleave[2] & 0x30;

      pOutData[3] = pOutData[3] >> 1; pOutData[3] = pOutData[3] >> 1;
      
      pOutData[3] |= preInterleave[3] & 0xc0;
      preInterleave[3] = preInterleave[3] << 1; preInterleave[3] = preInterleave[3] << 1;
      pOutData[2] |= preInterleave[3] & 0xc0;
      preInterleave[3] = preInterleave[3] << 1; preInterleave[3] = preInterleave[3] << 1;
      pOutData[1] |= preInterleave[3] & 0xc0;
      preInterleave[3] = preInterleave[3] << 1; preInterleave[3] = preInterleave[3] << 1;
      pOutData[0] |= preInterleave[3] & 0xc0;
     
    }*/

    pOutData[0] = preInterleave[0];
    pOutData[1] = preInterleave[1];
    pOutData[2] = preInterleave[2];
    pOutData[3] = preInterleave[3];
    
    return 2;
}

// Decodes four bytes at a time into two bytes
// Return two bytes
inline unsigned short fecDecode(unsigned char *pDecData, unsigned char* pInData, unsigned short nRemBytes)
{
    // Two sets of buffers (last, current) for each destination state for holding:
    static unsigned char nCost[2][8]; // Accumulated path cost
    static unsigned long aPath[2][8]; // Encoder input data (32b window)
    
    // Indices of (last, current) buffer for each iteration
    static unsigned char iLastBuf;
    static unsigned char iCurrBuf;
    
    // Number of bits in each path buffer
    static unsigned char nPathBits;
    
    // Variables used to hold # Viterbi iterations to run, # bytes output, // minimum cost for any destination state, bit index of input symbol
    unsigned char nIterations;
    unsigned short nOutputBytes = 0;
    unsigned char nMinCost;
    signed char iBit = 8 - 2;
    
    // Initialize variables at start of packet (and return without doing any more)
    if (pDecData == NULL) {
        unsigned char n;
        memset(nCost, 0, sizeof(nCost));
        for (n = 1; n < 8; n++)
        {
            nCost[0][n] = 100;
            nCost[1][n] = 0; // Added
            aPath[0][n] = 0; // Added
            aPath[1][n] = 0; // Added
        }
        iLastBuf = 0;
        iCurrBuf = 1;
        nPathBits = 0;
        return 0;
    }
    
    
    /*
        unsigned char aDeintData[4];
        signed char iOut;
        signed char iIn;
        
        // De-interleave received data (and change pInData to point to de-interleaved data)
        for (iOut = 0; iOut < 4; iOut++)
        {
            unsigned char dataByte = 0;
            for (iIn = 3; iIn >= 0; iIn--)
                dataByte = (dataByte << 2) | ((pInData[iIn] >>( 2 * iOut)) & 0x03);
            aDeintData[iOut] = dataByte;
        }
        
        pInData = aDeintData;
    */
    
    // Process up to 4 bytes of de-interleaved input data, processing one encoder symbol (2b) at a time
    for (nIterations = 16; nIterations > 0; nIterations--)
    {
        unsigned char iDestState;
        unsigned char symbol = ((*pInData) >> iBit) & 0x03;
        
        // Find minimum cost so that we can normalize costs (only last iteration used)
        nMinCost = 0xFF;
        
        // Get 2b input symbol (MSB first) and do one iteration of Viterbi decoding
        if ((iBit -= 2) < 0) {
            iBit = 6;
            pInData++; // Update pointer to the next byte of received data
        }
        
        // For each destination state in the trellis, calculate hamming costs for both possible paths into state and
        // select the one with lowest cost.
        for (iDestState = 0; iDestState < 8; iDestState++) {
            unsigned char nCost0;
            unsigned char nCost1;
            unsigned char iSrcState0;
            unsigned char iSrcState1;
            unsigned char nInputBit;
            
            nInputBit = aTrellisTransitionInput[iDestState];
            
            // Calculate cost of transition from each of the two source states (cost is Hamming difference between
            // received 2b symbol and expected symbol for transition)
            iSrcState0 = aTrellisSourceStateLut[iDestState][0];
            nCost0 = nCost[iLastBuf][iSrcState0];
            nCost0 += hammWeight(symbol ^ aTrellisTransitionOutput[iDestState][0]);
            
            iSrcState1 = aTrellisSourceStateLut[iDestState][1];
            nCost1 = nCost[iLastBuf][iSrcState1];
            nCost1 += hammWeight(symbol ^ aTrellisTransitionOutput[iDestState][1]);
            
            // Select transition that gives lowest cost in destination state, copy that source state's path and add // new decoded bit
            if (nCost0 <= nCost1) {
                nCost[iCurrBuf][iDestState] = nCost0;
                nMinCost = my_min(nMinCost, nCost0);
                aPath[iCurrBuf][iDestState] = (aPath[iLastBuf][iSrcState0] << 1) | nInputBit;
            } else {
                nCost[iCurrBuf][iDestState] = nCost1;
                nMinCost = my_min(nMinCost, nCost1);
                aPath[iCurrBuf][iDestState] = (aPath[iLastBuf][iSrcState1] << 1) | nInputBit;
            }
        }
        nPathBits++;
        
        // If trellis history is sufficiently long, output a byte of decoded data
        if (nPathBits == 32) {
            *pDecData++ = (aPath[iCurrBuf][0] >> 24) & 0xFF;
            nOutputBytes++;
            nPathBits -= 8;
            nRemBytes--;
        }
        
        // After having processed 3-symbol trellis terminator, flush out remaining data
        if ((nRemBytes <= 3) && (nPathBits == ((8 * nRemBytes) + 3))) {
            while (nPathBits >= 8) {
                *pDecData++ = (aPath[iCurrBuf][0] >> (nPathBits - 8)) & 0xFF; nOutputBytes++;
                nPathBits -= 8;
            }
            return nOutputBytes;
        }
        
        // Swap current and last buffers for next iteration
        iLastBuf = (iLastBuf + 1) % 2;
        iCurrBuf = (iCurrBuf + 1) % 2;
    }
    
    // Normalize costs so that minimum cost becomes 0
    {
        unsigned char iState;
        for (iState = 0; iState < 8; iState++)
            nCost[iLastBuf][iState] -= nMinCost;
    }
    
    return nOutputBytes;
}

// From Stack Overflow
// https://stackoverflow.com/questions/20554869/implementing-crc8-on-arduino-to-write-to-mlx90614

static const uint8_t crc8_table[] = {
	0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31,
	0x24, 0x23, 0x2a, 0x2d, 0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
	0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 0xe0, 0xe7, 0xee, 0xe9,
	0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
	0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1,
	0xb4, 0xb3, 0xba, 0xbd, 0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
	0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 0xb7, 0xb0, 0xb9, 0xbe,
	0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
	0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16,
	0x03, 0x04, 0x0d, 0x0a, 0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
	0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 0x89, 0x8e, 0x87, 0x80,
	0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
	0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8,
	0xdd, 0xda, 0xd3, 0xd4, 0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
	0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 0x19, 0x1e, 0x17, 0x10,
	0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
	0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f,
	0x6a, 0x6d, 0x64, 0x63, 0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
	0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 0xae, 0xa9, 0xa0, 0xa7,
	0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
	0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef,
	0xfa, 0xfd, 0xf4, 0xf3
};

uint8_t
crc8(uint8_t *p, uint8_t len)
{
	uint16_t i;
	uint16_t crc = 0x0;

	while (len--) {
		i = (crc ^ *p++) & 0xFF;
		crc = (crc8_table[i] ^ (crc << 8)) & 0xFF;
	}

	return crc & 0xFF;
}

#endif /* fec_crc_h */
