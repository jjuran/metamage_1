/*
	VBL.cc
	------
*/

#include "VBL.hh"

// Mac OS
#ifndef __RETRACE__
#include <Retrace.h>
#endif

// ams-common
#include "callouts.hh"
#include "interrupts.hh"
#include "reactor.hh"
#include "time.hh"

// ams-core
#include "reactor-core.hh"


QHdr VBLQueue : 0x0160;


static timer_node VBL_timer_node;
static bool       timer_scheduled;

static
void timeval_add( timeval& tv, uint32_t more_usecs )
{
	if ( (tv.tv_usec += more_usecs) >= 1000000 )
	{
		tv.tv_usec -= 1000000;
		
		++tv.tv_sec;
	}
}

static
asm void call_VBL_task( VBLProcPtr proc : __A0 )
{
	MOVEM.L  D3/A2-A3,-(SP)
	
	JSR      (A0)
	
	MOVEM.L  (SP)+,D3/A2-A3
	RTS
}

static
void schedule_VBL_timer()
{
	timeval now;
	time( &now );
	
	timeval_add( VBL_timer_node.wakeup, tick_microseconds );
	
	if ( now > VBL_timer_node.wakeup )
	{
		// A discontinuity has occurred.  Happens on first schedule.
		
		VBL_timer_node.wakeup = now;
		
		timeval_add( VBL_timer_node.wakeup, tick_microseconds );
	}
	
	timer_scheduled = true;
	
	schedule( &VBL_timer_node );
}

void do_VBL()
{
	QElemPtr volatile* slot = &VBLQueue.qHead;
	
	QElemPtr it = *slot;
	
	while ( it != NULL )
	{
		VBLTask* task = (VBLTask*) it;
		
		if ( --task->vblCount == 0 )
		{
			call_VBL_task( task->vblAddr );
		}
		
		if ( task->vblCount == 0 )
		{
			// Dequeue this task
			*slot = it->qLink;
		}
		else
		{
			slot = &it->qLink;
		}
		
		it = *slot;
	}
}

static
void VBL_ready( timer_node* )
{
	unlock_screen();
	lock_screen();
	
	timer_scheduled = false;
	
	do_VBL();
	
	if ( VBLQueue.qHead != NULL )
	{
		schedule_VBL_timer();
	}
}

short VInstall_patch( QElem* vbl : __A0 )
{
	VBLTask* task = (VBLTask*) vbl;
	
	short saved_SR = disable_interrupts();
	
	Enqueue( vbl, &VBLQueue );
	
	if ( ! timer_scheduled )
	{
		lock_screen();
		
		VBL_timer_node.ready = &VBL_ready;
		
		schedule_VBL_timer();
	}
	
	reenable_interrupts( saved_SR );
	
	return noErr;
}

short VRemove_patch( QElem* vbl : __A0 )
{
	short saved_SR = disable_interrupts();
	
	if ( OSErr err = Dequeue( vbl, &VBLQueue ) )
	{
		return err;
	}
	
	if ( VBLQueue.qHead == NULL )
	{
		cancel( &VBL_timer_node );
		
		timer_scheduled = false;
		
		unlock_screen();
	}
	
	reenable_interrupts( saved_SR );
	
	return noErr;
}
