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

pascal short Dequeue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 ) : __D0
{
	QElemPtr prev = NULL;
	
	// TODO:  Disable interrupts, save and restore A1
	
	QElemPtr next = queue->qHead;
	
	while ( next != NULL )
	{
		if ( next == qEntry )
		{
			QElemPtr* link = prev ? &prev->qLink : &queue->qHead;
			
			*link = qEntry->qLink;
			
			if ( qEntry == queue->qTail )
			{
				queue->qTail = prev;
				
				if ( prev == NULL )
				{
					queue->qHead = NULL;
				}
			}
			
			// TODO:  Reenable interrupts
			
			return noErr;
		}
		
		prev = next;
		next = next->qLink;
	}
	
	// TODO:  Reenable interrupts
	
	return qErr;  // "Entry not in specified queue"
}

pascal void SysBeep_patch( short duration )
{
	char c = 0x07;
	
	write( STDOUT_FILENO, &c, sizeof c );
}
