/*
    This file is part of Trillian.
    Audio convolution utility.
    Trillian homepage : http://code.google.com/p/trillian.

    Copyright (C) 2010  Mike Jones

    Trillian is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Trillian is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Trillian.  If not, see <http://www.gnu.org/licenses/>.
*/

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