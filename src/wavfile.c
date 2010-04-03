#include "wavfile.h"
#include "endian.h"

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static const unsigned long  WAV_RIFF        = 0x52494646; /* "riff" */
static const unsigned long  WAV_FMT_WAVE    = 0x57415645; /* "wave" */
static const unsigned long  WAV_FMT         = 0x666d7420; /* "fmt " */
static const unsigned long  WAV_DATA        = 0x64617461; /* "data" */
static const unsigned long  WAV_PCMCNK_SIZE = 16;
static const unsigned short WAV_PCM         = 1;


static int  tr_iswav(tr_wavfile* pWav);
static int  tr_findwavchunk(unsigned int pCnkID, FILE* pFile, long int* pCnkPos);
static int  tr_wavreadfmt(tr_wavfile* pWav);
static int  tr_wavreaddata(tr_wavfile* pWav);
static void tr_wavwriteheaders(tr_wavfile* pWav);

static void tr_convert_pcm16_float(signed short int* pPCMData, float* pOutput, unsigned int pNumSamples);
static void tr_convert_float_pcm16(float* pFloatData, signed short int* pOutput, unsigned int pNumSamples);

typedef struct
{
	unsigned long riffID;
	unsigned long filesize;
	unsigned long fmt;
} tr_wavfile_riff;

typedef struct
{
	unsigned long      fmtID;
	unsigned long      chunksize;
	unsigned short int format;
	unsigned short int numchannels;
	unsigned long      samplerate;
	unsigned long      byterate;
	unsigned short int blockalign;
	unsigned short int bitspersample;
} tr_wavfile_fmt;

typedef struct
{
	unsigned long cnkID;
	unsigned long cnksize;
} tr_wavfile_cnkheader;


int tr_wavopen(FILE* pFile, tr_wavfile* pWav, unsigned char pMode)
{
	pWav->filehandle = pFile;
	fseek(pWav->filehandle, 0, SEEK_SET);
	pWav->mode = pMode;
	
	switch(pMode)
	{
	case 'r':
		if( tr_iswav(pWav) )
		{
			if( tr_wavreadfmt(pWav) )
			{
				if( tr_wavreaddata(pWav) )
				{
					return 1;
				}
			}
		}
		break;
	case 'w':
		pWav->samplerate     = 44100;
		pWav->bytespersample = 2;
		pWav->channels       = 1;
		pWav->datastartpos   = sizeof(tr_wavfile_riff) + sizeof(tr_wavfile_fmt) + sizeof(tr_wavfile_cnkheader);
		break;
	default:
		break;
	}
	
	return 0;
}

void tr_wavclose(tr_wavfile* pWav)
{
	switch(pWav->mode)
	{
	case 'w':
		tr_wavwriteheaders(pWav);
		break;
	default:
		break;
	}
}

int tr_wavread(tr_wavfile* pWav, float* pBuffer, unsigned int pNumSamples)
{
	signed short int* readbuffer = malloc(pNumSamples * pWav->bytespersample);
	fseek(pWav->filehandle, pWav->datastartpos, SEEK_SET);
	size_t readCount = fread(readbuffer, pWav->bytespersample, pNumSamples, pWav->filehandle);
	
	pWav->frompcm_func(readbuffer, pBuffer, readCount);
	free(readbuffer);
	
	if( readCount != pNumSamples)
	{
		return 0;
	}
	return 1;
}

int tr_wavwrite(tr_wavfile* pWav, float* pBuffer, unsigned int pNumSamples)
{
	signed short* temp = malloc(pNumSamples * sizeof(signed short));
	tr_convert_float_pcm16(pBuffer, temp, pNumSamples);
	fseek(pWav->filehandle, pWav->datastartpos + pWav->totalsamples * sizeof(signed short), SEEK_SET);
	size_t writeCount = fwrite(temp, sizeof(signed short), pNumSamples, pWav->filehandle);
	free(temp);
	
	pWav->totalsamples += writeCount;
	
	if(writeCount != pNumSamples)
	{
		return 0;
	}
	return 1;
}

int tr_iswav(tr_wavfile* pWav)
{
	fseek(pWav->filehandle, 0, SEEK_SET);
	
	tr_wavfile_riff riff;
	fread(&riff, sizeof(tr_wavfile_riff), 1, pWav->filehandle);
	riff.riffID   = BigULong(riff.riffID);
	riff.filesize = LittleULong(riff.filesize);
	riff.fmt      = BigULong(riff.fmt);
	
	if( (riff.riffID == WAV_RIFF) && (riff.fmt == WAV_FMT_WAVE) )
	{
		return 1;
	}
	
	return 0;
}

int tr_findwavchunk(unsigned int pCnkID, FILE* pFile, long int* pCnkPos)
{
	long int originalPos = 0;
	size_t result        = 0;
	int found            = 0;
	tr_wavfile_cnkheader cnkStart;
	
	originalPos = ftell(pFile);
	rewind(pFile);
	*pCnkPos = ftell(pFile);
	
	do
	{
		memset(&cnkStart, 0, sizeof(tr_wavfile_cnkheader));
		fseek(pFile, *pCnkPos, SEEK_SET);
		
		result = fread(&cnkStart, 1, sizeof(tr_wavfile_cnkheader), pFile);
		cnkStart.cnkID    = BigULong(cnkStart.cnkID);
		cnkStart.cnksize  = LittleULong(cnkStart.cnksize);
		
		if(result != sizeof(tr_wavfile_cnkheader) ) /* End of file? */
		{
			*pCnkPos = 0;
			rewind(pFile);
			return 0;
		}
		
		if(cnkStart.cnkID == pCnkID)
		{
			found = 1;
		}
		else
		{
			if(cnkStart.cnkID != WAV_RIFF)
			{
				*pCnkPos += cnkStart.cnksize + sizeof(tr_wavfile_cnkheader);
			}
			else
			{	/* It's the riff cnk */
				*pCnkPos += sizeof(tr_wavfile_riff);
			}
		}
		
	} while(found == 0);
	
	return 1;
}

int tr_wavreadfmt(tr_wavfile* pWav)
{
	long int chunkpos = 0;
	if ( tr_findwavchunk(WAV_FMT, pWav->filehandle, &chunkpos) )
	{
		fseek(pWav->filehandle, chunkpos, SEEK_SET);
		tr_wavfile_fmt fmt;
		fread(&fmt, sizeof(tr_wavfile_fmt), 1, pWav->filehandle);
		
		pWav->channels       = LittleUShort(fmt.numchannels);
		pWav->samplerate     = LittleULong(fmt.samplerate);
		pWav->bytespersample = LittleUShort(fmt.bitspersample) /8;
		pWav->datastartpos   = ftell(pWav->filehandle);
		
		switch(pWav->bytespersample)
		{
		case 2:
			pWav->frompcm_func = (void(*)(void* pSourcePCM, void* pFloatDest, unsigned int pNumSamples))tr_convert_pcm16_float;
			break;
		default:
			return 0;
		}
		return 1;
	}
	return 0;
}

int tr_wavreaddata(tr_wavfile* pWav)
{
	long int chunkpos = 0;
	if ( tr_findwavchunk(WAV_DATA, pWav->filehandle, &chunkpos) )
	{
		fseek(pWav->filehandle, chunkpos, SEEK_SET);
		tr_wavfile_cnkheader data;
		fread(&data, sizeof(tr_wavfile_cnkheader), 1, pWav->filehandle);
		
		pWav->totalsamples = data.cnksize / pWav->bytespersample;
		return 1;
	}
	return 0;
}

void tr_wavwriteheaders(tr_wavfile* pWav)
{
	fseek(pWav->filehandle, 0, SEEK_END);
	unsigned long fileSize = ftell(pWav->filehandle);
	if(fileSize % 2) fileSize += 1;
	
	fseek(pWav->filehandle, 0, SEEK_SET);

	tr_wavfile_riff riff;
	riff.riffID = BigULong(WAV_RIFF);
	riff.filesize = LittleULong(fileSize);
	riff.fmt = BigULong(WAV_FMT_WAVE);
	fwrite(&riff, 1, sizeof(tr_wavfile_riff), pWav->filehandle);

	tr_wavfile_fmt fmt;
	fmt.fmtID         = BigULong(WAV_FMT);
	fmt.chunksize     = LittleULong(sizeof(tr_wavfile_fmt)- sizeof(tr_wavfile_cnkheader));
	fmt.format        = LittleULong(WAV_PCM);
	fmt.numchannels   = LittleULong(pWav->channels);
	fmt.samplerate    = LittleULong(pWav->samplerate);
	fmt.byterate      = LittleULong(pWav->samplerate * pWav->channels * WAV_PCMCNK_SIZE/8);
	fmt.blockalign    = LittleULong(pWav->channels * WAV_PCMCNK_SIZE/8);
	fmt.bitspersample = LittleULong(WAV_PCMCNK_SIZE);
	fwrite(&fmt, 1, sizeof(tr_wavfile_fmt), pWav->filehandle);

	tr_wavfile_cnkheader data;
	data.cnkID    = BigULong(WAV_DATA);
	data.cnksize =  LittleULong(fileSize - pWav->datastartpos);
	fwrite(&data, 1, sizeof(tr_wavfile_cnkheader), pWav->filehandle);
}


void tr_convert_pcm16_float(signed short int* pPCMData, float* pOutput, unsigned int pNumSamples)
{
	const double gain = 1.0/32768.0;
	
	unsigned int remainder = pNumSamples % 8;
	pNumSamples -= remainder;
	
	while(pNumSamples > 0)
	{
		*pOutput++  = (*pPCMData++) * gain;
		*pOutput++  = (*pPCMData++) * gain;
		*pOutput++  = (*pPCMData++) * gain;
		*pOutput++  = (*pPCMData++) * gain;
		*pOutput++  = (*pPCMData++) * gain;
		*pOutput++  = (*pPCMData++) * gain;
		*pOutput++  = (*pPCMData++) * gain;
		*pOutput++  = (*pPCMData++) * gain;

		pNumSamples -= 8;
	}
	
	switch(remainder)
	{
		case 7:  *pOutput++  = (*pPCMData++) * gain;
		case 6:  *pOutput++  = (*pPCMData++) * gain;
		case 5:  *pOutput++  = (*pPCMData++) * gain;
		case 4:  *pOutput++  = (*pPCMData++) * gain;
		case 3:  *pOutput++  = (*pPCMData++) * gain;
		case 2:  *pOutput++  = (*pPCMData++) * gain;
		case 1:  *pOutput++  = (*pPCMData++) * gain;
	}
}

void tr_convert_float_pcm16(float* pFloatData, signed short int* pOutput, unsigned int pNumSamples)
{
	const double gain      = 32767.0;
	unsigned int remainder = pNumSamples % 8;
	pNumSamples -= remainder;
	
	while(pNumSamples > 0)
	{
		*pOutput++ = (*pFloatData++) * gain;
		*pOutput++ = (*pFloatData++) * gain;
		*pOutput++ = (*pFloatData++) * gain;
		*pOutput++ = (*pFloatData++) * gain;
		*pOutput++ = (*pFloatData++) * gain;
		*pOutput++ = (*pFloatData++) * gain;
		*pOutput++ = (*pFloatData++) * gain;
		*pOutput++ = (*pFloatData++) * gain;

		pNumSamples -= 8;
	}
	
	switch(remainder)
	{
		case 7:  *pOutput++ = (*pFloatData++) * gain;
		case 6:  *pOutput++ = (*pFloatData++) * gain;
		case 5:  *pOutput++ = (*pFloatData++) * gain;
		case 4:  *pOutput++ = (*pFloatData++) * gain;
		case 3:  *pOutput++ = (*pFloatData++) * gain;
		case 2:  *pOutput++ = (*pFloatData++) * gain;
		case 1:  *pOutput++ = (*pFloatData++) * gain;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */