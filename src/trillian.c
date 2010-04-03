/* 
	Trillian 
	Audio convolution utility
	Trillian homepage : http://code.google.com/p/trillian
	
	Copyright (C) 2010  Mike Jones

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "wavfile.h"
#include "endian.h"

#define TRILLIAN_MAJ_VER 0x00
#define TRILLIAN_MIN_VER 0x0000
#define TRILLIAN_INC_VER 0x000003

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int quiet = 0; /*quiet mode flag*/
static char* outfilename = NULL;

static void tr_version(void);
static void tr_help(void);
static void tr_parseoptions(int argc, char** argv);

/* Valid long options */
struct option tr_long_options[] = {
	{"help", 0, 0, 'h'},
	{"version", 0, 0, 'v'},
	{"quiet", 0, 0, 's'},
	{"silent", 0, 0, 's'},
	{"output", 1, 0, 'o'},
	{NULL, 0, 0, 0}
};

static void tr_version(void)
{
	fprintf(stdout, "Trillian %i.%i.%i\n", TRILLIAN_MAJ_VER, TRILLIAN_MIN_VER, TRILLIAN_INC_VER);
	fprintf(stdout, "Copyright (C) 2010 Michael Jones\n");
	fprintf(stdout, "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n");
	fprintf(stdout, "This is free software: you are free to change and redistribute it.\n");
	fprintf(stdout, "There is NO WARRANTY, to the extent permitted by law.\n");
}

static void tr_help(void)
{
	tr_version();
	fprintf(stdout, "\n");
	fprintf(stdout, "Usage: trillian [options] input.wav response.wav \n");
	fprintf(stdout, "Options: \n");
	fprintf(stdout, "  -h, --help             Show this message and exit. \n");
	fprintf(stdout, "  -v, --version          Display version number and exit. \n");
	fprintf(stdout, "  -s, --silent, --quiet  Quiet mode; no output to console (stdout). \n");
	fprintf(stdout, "  -o, --output           Use given filename for output wav file. \n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Report bugs to     : trillian-discuss@googlegroups.com \n");
	fprintf(stdout, "Trillian home page : http://code.google.com/p/trillian \n");
}

static void tr_parseoptions(int argc, char** argv)
{
	int option_index = 1;
	int opt;
	
	while((opt = getopt_long(argc, argv, "vhso:", tr_long_options, &option_index)) != -1)
	{
		switch(opt)
		{
			case 'h':
				tr_help();
				exit(0);
				break;
			case 'v':
				tr_version();
				exit(0);
				break;
			case 's':
				quiet = 1;
				break;
			case 'o':
				outfilename = strdup(optarg);
				break;
			default:
				fprintf(stderr, "ERROR: Invalid argument. Use -h for help \n");
				exit(1); /* We probably could survive, better to just bail for now; at least that way we can guarentee nothing bad will happen */
				break;
		}
	}
}


int main(int argc, char** argv)
{
	if(argc == 1)
	{
		tr_help();
		return 1;
	}
	
	tr_parseoptions(argc, argv);
	
	if(!quiet)
	{
		tr_version();
	}
	
	if(argc - optind < 2)
	{
		fprintf(stderr, "ERROR: Requires input and response file to be specified. Use -h for help \n");
		return 1;
	}
	
	
	const char* infilename       = argv[optind];
	const char* responsefilename = argv[optind+1];
	
	/* Make a filename from the input and response names */
	if(!outfilename)
	{
		char* innameend = strrchr(infilename, '.');
		int innamelen = innameend ? innameend-infilename : strlen(infilename);
		
		char* responsenameend = strrchr(responsefilename, '.');
		int responsenamelen = responsenameend ? responsenameend-responsefilename : strlen(responsefilename);
		
		outfilename = malloc(innamelen+responsenamelen + 5); /* + '.wav '*/
		strncpy(outfilename, infilename, innamelen);
		strncat(outfilename, responsefilename, responsenamelen);
		strcat(outfilename, ".wav");
	}
	
	
	InitEndian();
	
	/* Setup the input file */
	FILE* infile = fopen(infilename, "rb");
	if(infile == 0)
	{
		fprintf(stderr, "ERROR: Failed opening input file %s\n", infilename);
		return 1;
	}
	
	tr_wavfile inputwav;
	if( !tr_wavopen(infile, &inputwav, 'r') )
	{
		fprintf(stderr, "ERROR: Invalid file type, %s is not a wav file\n", infilename);
		return 1;
	}
	
	if(!quiet)
	{
		fprintf(stdout, "\n");
		fprintf(stdout, "Input file         : %s\n", infilename);
		fprintf(stdout, "  Samples          : %u\n", inputwav.totalsamples);
		fprintf(stdout, "  Channels         : %i\n", inputwav.channels);
		fprintf(stdout, "  Sample rate      : %u\n", inputwav.samplerate);
		fprintf(stdout, "  Bytes per sample : %u\n", inputwav.bytespersample);
		fprintf(stdout, "  Duration seconds : %.2f\n", (float)(inputwav.totalsamples / inputwav.samplerate));
	}
	
	/* Now setup the response file */
	FILE* responsefile = fopen(responsefilename, "rb");
	if(responsefile == 0)
	{
		fprintf(stderr, "ERROR: Failed opening response file %s\n", responsefilename);
		return 1;
	}
	
	tr_wavfile responsewav;
	if(!tr_wavopen(responsefile, &responsewav, 'r'))
	{
		fprintf(stderr, "ERROR: Invalid file type, %s is not a wav file\n", responsefilename);
		return 1;
	}
	
	if(!quiet)
	{
		fprintf(stdout, "\n");
		fprintf(stdout, "Response file      : %s\n", responsefilename);
		fprintf(stdout, "  Samples          : %u\n", responsewav.totalsamples);
		fprintf(stdout, "  Channels         : %i\n", responsewav.channels);
		fprintf(stdout, "  Sample rate      : %u\n", responsewav.samplerate);
		fprintf(stdout, "  Bytes per sample : %u\n", responsewav.bytespersample);
		fprintf(stdout, "  Duration seconds : %.2f\n", (float)(responsewav.totalsamples / responsewav.samplerate));
		
		fprintf(stdout, "\n");
		fprintf(stdout, "Reading %s and %s into memory\n", infilename, responsefilename);
	}
	
	/* Both are open, now read them into memory */
	float* inputbuffer = malloc(inputwav.totalsamples * sizeof(float));
	if( !tr_wavread(&inputwav, inputbuffer, inputwav.totalsamples) )
	{
		fprintf(stderr, "ERROR: Failed reading input file\n");
		return 1;
	}
	
	float* responsebuffer = malloc(responsewav.totalsamples * sizeof(float));
	if(!tr_wavread(&responsewav, responsebuffer, responsewav.totalsamples))
	{
		fprintf(stderr, "ERROR: Failed reading input file\n");
		return 1;
	}
	
	/* Prepare a buffer to accept the data from our processing */
	unsigned int samplesinput    = inputwav.totalsamples;
	unsigned int samplesresponse = responsewav.totalsamples;
	unsigned int samplestotal    = samplesinput + samplesresponse + 1;
	float* outputbuffer = malloc(samplestotal*sizeof(float));
	
	float* conv = outputbuffer;
	unsigned int inoffset;
	unsigned int reoffset;
	unsigned int n_hi;
	unsigned int n_lo;
	
	if(!quiet)
	{
		fprintf(stdout, "Processing audio\n");
	}
	
	/* Do the processing (time domain convolution) */
	unsigned int i;
	for(i = 0; i < samplestotal; i++)
	{
		n_lo = i < samplesresponse ? 0 : i - samplesresponse;
		n_hi = samplesinput < i ? samplesinput : i;
		
		inoffset = n_lo;
		reoffset = i - n_lo;
		
		*conv = 0.0f;
		unsigned int n;
		for(n = n_lo; n < n_hi; n++)
		{
			*conv += *(inputbuffer+inoffset) * *(responsebuffer+reoffset);
			++inoffset;
			--reoffset;
		}
		conv++;
	}
	
	if(!quiet)
	{
		fprintf(stdout, "Normalising audio\n");
	}
	
	/* Normalise the data */
	conv = outputbuffer;
	float maxsample = 0.0f;
	
	for(i = 0; i < samplestotal; i++)
	{
		if(*conv > maxsample)
		{
			maxsample = *conv;
		}
		++conv;
	}
	
	conv = outputbuffer;
	float normalise = 1.0f / maxsample;
	
	for(i = 0; i < samplestotal; i++)
	{
		*conv++ *= normalise;
	}
	
	/* Setup the output file */
	FILE* outfile = fopen(outfilename, "wb");
	if(outfile == 0)
	{
		fprintf(stderr, "ERROR: Failed opening output file %s\n", outfilename);
		return 1;
	}
	
	tr_wavfile outwav;
	tr_wavopen(outfile, &outwav, 'w');
	
	if(!quiet)
	{
		fprintf(stdout, "\n");
		fprintf(stdout, "Output file        : %s\n", outfilename);
		fprintf(stdout, "  Samples          : %u\n", samplestotal);
		fprintf(stdout, "  Channels         : %i\n", outwav.channels);
		fprintf(stdout, "  Sample rate      : %u\n", outwav.samplerate);
		fprintf(stdout, "  Bytes per sample : %u\n", outwav.bytespersample);
		fprintf(stdout, "  Duration seconds : %.2f\n", (float)(samplestotal / outwav.samplerate));
		
		fprintf(stdout, "\n");
		fprintf(stdout, "Writing data out to %s\n", outfilename);
	}
	
	/* Write out data from the processing to file */
	if(!tr_wavwrite(&outwav, outputbuffer, samplestotal))
	{
		fprintf(stderr, "ERROR: Failed during write of %s.  File may be malformed\n", outfilename);
	}

	/* Clean up */
	free(inputbuffer);
	free(responsebuffer);
	free(outputbuffer);
	
	tr_wavclose(&inputwav);
	tr_wavclose(&responsewav);
	tr_wavclose(&outwav);
	
	fclose(infile);
	fclose(responsefile);
	fclose(outfile);
	
	return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */