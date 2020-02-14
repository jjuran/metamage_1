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

short CurPageOption : 0x0936;


static timer_node VBL_timer_node;
static bool       timer_scheduled;

/*
	The SoundMusicSys module used in games like Prince of Persia expects to
	find its newly installed VBL task in the VBL queue, but it doesn't check
	the queue head.  In order to make it work properly, place our own "task"
	at the head of the queue so the check succeeds.
*/

static VBLTask dummy_VBL_task;

static
void dummy_VBL_proc( VBLTask* task : __A0 )
{
	// Don't do anything; just stick around in the queue
	
	task->vblCount = 0x7FFF;
}

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
asm void call_VBL_task( VBLTask* task : __A0, VBLProcPtr proc : __A1 )
{
	MOVEM.L  D3/A2-A3,-(SP)
	
	JSR      (A1)
	
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
			call_VBL_task( task, task->vblAddr );
		}
		
		if ( task->vblCount == 0 )
		{
			// Dequeue this task
			*slot = it->qLink;
			
			if ( (QElemPtr) task == VBLQueue.qTail )
			{
				VBLQueue.qTail = (QElemPtr) slot;
			}
			
			if ( VBLQueue.qHead == VBLQueue.qTail )
			{
				// This should be our dummy VBL
				
				VBLQueue.qHead = VBLQueue.qTail = NULL;
			}
			
			if ( VBLQueue.qHead == NULL  &&  CurPageOption >= 0 )
			{
				unlock_screen();
			}
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
	if ( CurPageOption >= 0 )
	{
		unlock_screen();
		lock_screen();
	}
	
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
	
	if ( VBLQueue.qHead == vbl )
	{
		// place our dummy VBL task at the head of the queue
		
		dummy_VBL_task.qLink    = vbl;
		dummy_VBL_task.vblAddr  = (VBLUPP) &dummy_VBL_proc;
		dummy_VBL_task.vblCount = 0x7FFF;
		
		VBLQueue.qHead = (QElem*) &dummy_VBL_task;
	}
	
	if ( ! timer_scheduled )
	{
		if ( CurPageOption >= 0 )
		{
			lock_screen();
		}
		
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
	
	if ( VBLQueue.qHead == VBLQueue.qTail )
	{
		// This should be our dummy VBL
		
		VBLQueue.qHead = VBLQueue.qTail = NULL;
	}
	
	if ( VBLQueue.qHead == NULL )
	{
		cancel( &VBL_timer_node );
		
		timer_scheduled = false;
		
		if ( CurPageOption >= 0 )
		{
			unlock_screen();
		}
	}
	
	reenable_interrupts( saved_SR );
	
	return noErr;
}
