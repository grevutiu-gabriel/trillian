#ifndef _TRILLIAN_WAV_FILE_H_
#define _TRILLIAN_WAV_FILE_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct tr_wavfile
{
	FILE*         filehandle;
	unsigned char mode;
	
	unsigned short int channels;
	unsigned int       totalsamples;
	unsigned int       samplerate;
	unsigned int       bytespersample;
	
	void (*frompcm_func) (void* pSourcePCM, void* pFloatDest, unsigned int pNumSamples);
	unsigned int datastartpos;
} tr_wavfile;


extern int  tr_wavopen(FILE* pFile, tr_wavfile* pWav, unsigned char pMode);
extern void tr_wavclose(tr_wavfile* pWav);

extern int  tr_wavread(tr_wavfile* pWav, float* pBuffer, unsigned int pNumSamples);
extern int  tr_wavwrite(tr_wavfile* pWav, float* pBuffer, unsigned int pNumSamples);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_TRILLIAN_WAV_FILE_H_