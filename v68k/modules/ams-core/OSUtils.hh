/*
	OSUtils.hh
	----------
*/

#ifndef OSUTILS_HH
#define OSUTILS_HH

struct QElem;
struct QHdr;

pascal short ReadDateTime_patch( long* secs : __A0 ) : __D0;

pascal void Enqueue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 );

pascal short Dequeue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 ) : __D0;

pascal long Delay_patch( long numTicks : __A0 ) : __D0;

pascal void SysBeep_patch( short duration );

#endif
