/*
	OSUtils.cc
	----------
*/

#include "OSUtils.hh"

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

// POSIX
#include <time.h>
#include <unistd.h>


uint32_t Ticks : 0x016A;


pascal long TickCount_patch()
{
	return Ticks;
}

pascal long Delay_patch( long numTicks : __A0 ) : __D0
{
	const long nanoseconds_per_tick = 1000 * 1000 * 1000 / 60;
	
	const long seconds     = numTicks / 60;
	const long nanoseconds = numTicks % 60 * nanoseconds_per_tick;
	
	timespec delay = { seconds, nanoseconds };
	
	timespec remaining;
	
	while ( nanosleep( &delay, &remaining ) != 0 )
	{
		delay = remaining;
	}
	
	return Ticks;
}

asm
pascal void Enqueue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 )
{
	CLR.L    (A0)     // qEntry->qLink = NULL;
	
	// TODO:  Disable interrupts
	
	MOVE.L   A0,D2    // Move qEntry to D2 (freeing up A0 for next pointer)
	ADDQ.L   #2,A1    // Point to qHead field of queue
	
	MOVEA.L  A1,A0    // next = &queue->qHead;
	TST.L    (A1)+    // if (queue->qHead)  // advances A1 to &queue->qTail
	BEQ.S    set_link
	
	MOVE.L   (A1),A0  // next = &queue->qTail->qLink;
	
set_link:
	
	MOVE.L   D2,(A0)  // *next        = qEntry;
	MOVE.L   D2,(A1)  // queue->qTail = qEntry;
	
	// TODO:  Reenable interrupts
	
	RTS
}

asm
pascal short Dequeue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 ) : __D0
{
	MOVEQ.L  #0,D0       // noErr
	
	ADDQ.L   #2,A1       // Point to qHead field of queue
	MOVE.L   A1,D1       // Copy &queue->qHead to D1
	
	// TODO:  Disable interrupts, save and restore A1
	
	CMPA.L   (A1)+,A0    // if ( qEntry == queue->qHead )
	BNE.S    not_first
	
	CMP.L    (A1),A0     // if ( qEntry == queue->qTail )
	BNE.S    first_but_not_last
	
	CLR.L    (A1)        // queue->qTail = NULL;
	
first_but_not_last:
	
	MOVE.L   (A0),-(A1)  // queue->qHead = qEntry->qLink;
	
	BRA.S    cleanup
	
not_found:
	
	MOVEQ.L  #-1,D0      // qErr: "Entry not in specified queue"
	BRA.S    cleanup
	
not_first:
	
	MOVE.L   A0,D2       // Move qEntry to D2 (freeing up A0 for next pointer)
	
loop:
	
	MOVEA.L  D1,A0       // prev = next;         // or &queue->qHead, once
	MOVE.L   (A0),D1     // next = next->qLink;  // or queue->qHead, once
	
	BEQ.S    not_found   // while ( next != NULL )
	
	CMP.L    D2,D1       // if ( next == qEntry )
	BNE.S    loop
	
	CMP.L    (A1),D2     // if ( qEntry == queue->qTail )
	BNE.S    not_last
	
	MOVE.L   A0,(A1)     // queue->qTail = prev;
	
not_last:
	
	MOVEA.L  D2,A1
	MOVE.L   (A1),(A0)   // prev->qLink = qEntry->qLink;
	
cleanup:
	
	// TODO:  Reenable interrupts
	
	RTS
}

pascal void SysBeep_patch( short duration )
{
	char c = 0x07;
	
	write( STDOUT_FILENO, &c, sizeof c );
}
