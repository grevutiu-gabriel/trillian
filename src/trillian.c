/* trillian */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#define TRILLIAN_MAJ_VER 0x00
#define TRILLIAN_MIN_VER 0x0000
#define TRILLIAN_INC_VER 0x000001

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

static int quiet = 0; /*quiet mode flag*/

static void tr_version(void);
static void tr_help(void);
static void tr_parseoptions(int argc, char** argv);

/*Valid long options*/
struct option tr_long_options[] = {
	{"help", 0, 0, 'h'},
	{"version", 0, 0, 'v'},
	{"quiet", 0, 0, 's'},
	{"silent", 0, 0, 's'},
	{NULL, 0, 0, 0}
};

static void tr_version(void)
{
	fprintf(stdout, "Trillian %i.%i.%i\n", TRILLIAN_MAJ_VER, TRILLIAN_MIN_VER, TRILLIAN_INC_VER);
	fprintf(stdout, "Copyright (C) 2010 Michael Jones\n");
}

static void tr_help(void)
{
	tr_version();
	fprintf(stdout, "Usage: trillian [options] input.wav response.wav output.wav \n");
	fprintf(stdout, "Options: \n");
	fprintf(stdout, "  -h, --help            Show this message and exit. \n");
	fprintf(stdout, "  -v, --version         Print out version number and exit. \n");
	fprintf(stdout, "  -s, --silent --quiet  Quiet mode, no output to console. \n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Report bugs to    : trillianbugs@jonesaudio.co.uk \n");
	fprintf(stdout, "Trillian homepage : http://www.jonesaudio.co.uk/trillian \n");
}

static void tr_parseoptions(int argc, char** argv)
{
	int option_index = 1;
	int opt;
	
	while((opt = getopt_long(argc, argv, "vhs", tr_long_options, &option_index)) != -1)
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
			default:
				fprintf(stderr, "Unknown argument\n");
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
	
	if(quiet == 0) {
		tr_version();
		fprintf(stdout, "Hello World!\n");
	}
	
	return 0;
}

#ifdef __cplusplus
}
#endif //__cplusplus