/*
	OSUtils.hh
	----------
*/

#ifndef OSUTILS_HH
#define OSUTILS_HH

struct DateTimeRec;
struct QElem;
struct QHdr;
struct SysEnvRec;

unsigned long CmpString_patch( const unsigned char* a : __A0,
                               const unsigned char* b : __A1,
                               unsigned long        m : __D0,
                               unsigned short       A : __D1 );

long RelString_patch( const unsigned char* a : __A0,
                      const unsigned char* b : __A1,
                      unsigned long        m : __D0,
                      unsigned short       A : __D1 );

unsigned char* UprString_patch( unsigned char* s : __A0,
                                short len        : __D0,
                                short trap_word  : __D1 );

short ReadDateTime_patch( long* secs : __A0 );

DateTimeRec* Secs2Date_patch( unsigned long  secs : __D0,
                              DateTimeRec*   date : __A0 );

short InitUtil_patch();

void Enqueue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 );

short Dequeue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 );

long Delay_patch( long numTicks : __A0, short trap_word : __D1 );

pascal void SysBeep_patch( short duration );

short SysEnvirons_patch( short v : __D0, SysEnvRec* env : __A0 );

#endif
