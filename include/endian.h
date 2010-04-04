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

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define BIG_ENDIAN     1
#define LITTLE_ENDIAN  0

extern const int testEndian;
#define IS_BIG_ENDIAN()      ( (*(char*)&testEndian) == 0 )
#define IS_LITTLE_ENDIAN()   ( (*(char*)&testEndian) != 0 )

void InitEndian();

extern unsigned short (*BigUShort)    ( unsigned short pVar );  //unsigned
extern unsigned short (*LittleUShort) ( unsigned short pVar );
extern unsigned long  (*BigULong)     ( unsigned long  pVar );
extern unsigned long  (*LittleULong)  ( unsigned long  pVar );
extern short  (*BigShort)     ( short pVar  );  //signed
extern short  (*LittleShort)  ( short pVar  );
extern long   (*BigLong)      ( long   pVar );
extern long   (*LittleLong)   ( long   pVar );
extern float  (*BigFloat)     ( float pVar  );
extern float  (*LittleFloat)  ( float pVar  );

unsigned long  FLIP_ULONG(unsigned long pVar); //unsigned
unsigned short FLIP_USHORT(unsigned short pVar);
long           FLIP_LONG(long pVar);  //signed
short          FLIP_SHORT(short pVar);
float          FLIP_FLOAT(float pVar);

unsigned long  NO_FLIP_ULONG(unsigned long pVar);
unsigned short NO_FLIP_USHORT(unsigned short pVar);
long           NO_FLIP_LONG(long pVar);
short          NO_FLIP_SHORT(short pVar);
float          NO_FLIP_FLOAT(float pVar);

void* FlipEndian(void* pAddr, const int pNb); 


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_ENDIAN_H_