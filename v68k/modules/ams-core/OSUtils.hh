/*
	OSUtils.hh
	----------
*/

#ifndef OSUTILS_HH
#define OSUTILS_HH

struct DateTimeRec;
struct QElem;
struct QHdr;

short ReadDateTime_patch( long* secs : __A0 );

DateTimeRec* Secs2Date_patch( unsigned long  secs : __D0,
                              DateTimeRec*   date : __A0 );

void Enqueue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 );

short Dequeue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 );

long Delay_patch( long numTicks : __A0 );

pascal void SysBeep_patch( short duration );

#endif
