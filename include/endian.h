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