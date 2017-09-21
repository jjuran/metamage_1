/*
	OSUtils.cc
	----------
*/

#include "OSUtils.hh"

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

// POSIX
#include <time.h>
#include <unistd.h>


uint32_t Ticks : 0x016A;
uint32_t Time  : 0x020C;


#pragma mark -
#pragma mark Date and Time Operations
#pragma mark -

pascal short ReadDateTime_patch( long* secs : __A0 ) : __D0
{
	*secs = Time;
	
	return noErr;
}

#pragma mark -
#pragma mark Queue Manipulation
#pragma mark -

asm
pascal void Enqueue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 )
{
	CLR.L    (A0)     // qEntry->qLink = NULL;
	
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	ORI      #0x0700,SR  // mask all interrupts (except NMI)
	ANDI     #0xDFFF,SR  // exit supervisor mode
	
	// old SR (with old interrupt mask) is in D0
	
	MOVE.L   A0,D2    // Move qEntry to D2 (freeing up A0 for next pointer)
	ADDQ.L   #2,A1    // Point to qHead field of queue
	
	MOVEA.L  A1,A0    // next = &queue->qHead;
	TST.L    (A1)+    // if (queue->qHead)  // advances A1 to &queue->qTail
	BEQ.S    set_link
	
	MOVE.L   (A1),A0  // next = &queue->qTail->qLink;
	
set_link:
	
	MOVE.L   D2,(A0)  // *next        = qEntry;
	MOVE.L   D2,(A1)  // queue->qTail = qEntry;
	
	MOVE.L   D0,D1       // move old SR to D1
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	MOVE     D1,SR       // restore old SR (with old interrupt mask)
	
	RTS
}

asm
pascal short Dequeue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 ) : __D0
{
	CLR.W    -(SP)       // noErr
	
	ADDQ.L   #2,A1       // Point to qHead field of queue
	MOVE.L   A1,D1       // Copy &queue->qHead to D1
	
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	ORI      #0x0700,SR  // mask all interrupts (except NMI)
	ANDI     #0xDFFF,SR  // exit supervisor mode
	
	// old SR (with old interrupt mask) is in D0
	
	CMPA.L   (A1)+,A0    // if ( qEntry == queue->qHead )
	BNE.S    not_first
	
	CMP.L    (A1),A0     // if ( qEntry == queue->qTail )
	BNE.S    first_but_not_last
	
	CLR.L    (A1)        // queue->qTail = NULL;
	
first_but_not_last:
	
	MOVE.L   (A0),-(A1)  // queue->qHead = qEntry->qLink;
	
	BRA.S    cleanup
	
not_found:
	
	SUBQ.W   #1,(SP)     // qErr: "Entry not in specified queue"
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
	
	MOVE.L   D0,D1       // move old SR to D1
	JSR      0xFFFFFFFA  // enter_supervisor_mode()
	MOVE     D1,SR       // restore old SR (with old interrupt mask)
	
	MOVE.W   (SP)+,D0
	
	RTS
}

#pragma mark -
#pragma mark Miscellaneous Utilities
#pragma mark -

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

pascal void SysBeep_patch( short duration )
{
	char c = 0x07;
	
	write( STDOUT_FILENO, &c, sizeof c );
}
