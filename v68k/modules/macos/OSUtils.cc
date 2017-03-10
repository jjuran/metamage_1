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

pascal void Enqueue_patch( QElem* qEntry : __A0, QHdr* queue : __A1 )
{
	qEntry->qLink = NULL;
	
	// TODO:  Disable interrupts
	
	if ( queue->qHead == NULL )
	{
		queue->qHead = qEntry;
	}
	else
	{
		queue->qTail->qLink = qEntry;
	}
	
	queue->qTail = qEntry;
	
	// TODO:  Reenable interrupts
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
