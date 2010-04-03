/**
   Runtime endian operations
   See:  http://www.ibm.com/developerworks/aix/library/au-endianc/index.html?ca=drs-
		 http://www.gdargaud.net/Hack/SourceCode.html
		 http://www.gamedev.net/reference/articles/article2091.asp
		 
	@important  InitEndian needs to be called before you start calling the endian functions
				othewise strange things will start happening
*/

#include "endian.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

const int testEndian = 1;

unsigned short (*BigUShort)    ( unsigned short pVar );
unsigned short (*LittleUShort) ( unsigned short pVar );
unsigned long  (*BigULong)     ( unsigned long  pVar );
unsigned long  (*LittleULong)  ( unsigned long  pVar );
short (*BigShort)     ( short pVar );
short (*LittleShort)  ( short pVar );
long  (*BigLong)      ( long   pVar );
long  (*LittleLong)   ( long   pVar );
float (*BigFloat)     ( float pVar );
float (*LittleFloat)  ( float pVar );


/**
	Map function pointers to correct functions for the endian of the system running
*/
void InitEndian()
{
	if( IS_BIG_ENDIAN() )
	{
		BigUShort    = NO_FLIP_USHORT;
		LittleUShort = FLIP_USHORT;
		BigULong     = NO_FLIP_ULONG;
		LittleULong  = FLIP_ULONG;
		BigShort     = NO_FLIP_SHORT;
		LittleShort  = FLIP_SHORT;
		BigLong      = NO_FLIP_LONG;
		LittleLong   = FLIP_LONG;
		BigFloat     = NO_FLIP_FLOAT;
		LittleFloat  = FLIP_FLOAT;
	}
	else
	{
		BigUShort    = FLIP_USHORT;
		LittleUShort = NO_FLIP_USHORT;
		BigULong     = FLIP_ULONG;
		LittleULong  = NO_FLIP_ULONG;
		BigShort     = FLIP_SHORT;
		LittleShort  = NO_FLIP_SHORT;
		BigLong      = FLIP_LONG;
		LittleLong   = NO_FLIP_LONG;
		BigFloat     = FLIP_FLOAT;
		LittleFloat  = NO_FLIP_FLOAT;
	}
}

unsigned long  FLIP_ULONG(unsigned long pVar)
{
	return *(unsigned long*) FlipEndian((void*)&pVar, sizeof(unsigned long));
}
  
unsigned short FLIP_USHORT(unsigned short pVar)
{
	return *(unsigned short*) FlipEndian((void*)&pVar, sizeof(unsigned short));
}

long  FLIP_LONG(long pVar)
{
	return *(long*)  FlipEndian((void*)&pVar, sizeof(long));
}

short FLIP_SHORT(short pVar)
{
	return *(short*) FlipEndian((void*)&pVar, sizeof(short));
}

float FLIP_FLOAT(float pVar)
{
	return *(float*) FlipEndian((void*)&pVar, sizeof(float));
}


unsigned long  NO_FLIP_ULONG(unsigned long pVar)
{
	return pVar;
}

unsigned short NO_FLIP_USHORT(unsigned short pVar)
{
	return pVar;
}

long  NO_FLIP_LONG(long pVar)
{
	return pVar;
}

short NO_FLIP_SHORT(short pVar)
{
	return pVar;
}

float NO_FLIP_FLOAT(float pVar)
{
	return pVar;
}


/*
	Performs an endian swap up to 16bytes [2,3,4,8,16]
	TODO:  See if bit shift version is faster
*/
void* FlipEndian(void* pAddr, const int pNb) 
{
	static char Swapped[16];
	switch (pNb) {
		case 2:	Swapped[0]=*((char*)pAddr+1);
				Swapped[1]=*((char*)pAddr  );
				break;
		case 3:
				Swapped[0]=*((char*)pAddr+2);
				Swapped[1]=*((char*)pAddr+1);
				Swapped[2]=*((char*)pAddr  );
				break;
		case 4:	Swapped[0]=*((char*)pAddr+3);
				Swapped[1]=*((char*)pAddr+2);
				Swapped[2]=*((char*)pAddr+1);
				Swapped[3]=*((char*)pAddr  );
				break;
		case 8:	Swapped[0]=*((char*)pAddr+7);
				Swapped[1]=*((char*)pAddr+6);
				Swapped[2]=*((char*)pAddr+5);
				Swapped[3]=*((char*)pAddr+4);
				Swapped[4]=*((char*)pAddr+3);
				Swapped[5]=*((char*)pAddr+2);
				Swapped[6]=*((char*)pAddr+1);
				Swapped[7]=*((char*)pAddr  );
				break;
		case 16:Swapped[0] =*((char*)pAddr+15);
				Swapped[1] =*((char*)pAddr+14);
				Swapped[2] =*((char*)pAddr+13);
				Swapped[3] =*((char*)pAddr+12);
				Swapped[4] =*((char*)pAddr+11);
				Swapped[5] =*((char*)pAddr+10);
				Swapped[6] =*((char*)pAddr+9 );
				Swapped[7] =*((char*)pAddr+8 );
				Swapped[8] =*((char*)pAddr+7 );
				Swapped[9] =*((char*)pAddr+6 );
				Swapped[10]=*((char*)pAddr+5 );
				Swapped[11]=*((char*)pAddr+4 );
				Swapped[12]=*((char*)pAddr+3 );
				Swapped[13]=*((char*)pAddr+2 );
				Swapped[14]=*((char*)pAddr+1 );
				Swapped[15]=*((char*)pAddr   );
				break;
		default:
			printf("!!  FlipEndian(): Can't do a flip on %i bytes  !!\n", pNb);
			break;
	}
	return (void*)Swapped;
}

#ifdef __cplusplus
}
#endif //__cplusplus