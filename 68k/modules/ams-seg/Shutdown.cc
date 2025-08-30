/*
	Shutdown.cc
	-----------
*/

#include "Shutdown.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif
#ifndef __SCRAP__
#include <Scrap.h>
#endif
#ifndef __SHUTDOWN__
#include <ShutDown.h>
#endif

// POSIX
#include <unistd.h>

// log-of-war
#include "logofwar/report.hh"

// ams-seg
#include "options.hh"


#pragma exceptions off


#define COUNT(array)  (sizeof (array) / sizeof *(array))


struct shutdown_task
{
	QElemPtr    qLink;
	short       qType;
	short       flags;
	ShutDwnUPP  proc;
};

static QHdr ShutdownQueue;

static
void call_procs( short selector : __D0, short phase )
{
	volatile QElemPtr* addr = &ShutdownQueue.qHead;
	
	while ( shutdown_task* next = (shutdown_task*) *addr )
	{
		const short flags = next->flags;
		const short which = flags & sdRestartOrPower;  // 1 or 2, or 0
		
		if ( which == 0  ||  which == selector )
		{
			if ( flags - which == phase )
			{
				next->proc();
				
				*addr = next->qLink;
				
				DisposePtr( (Ptr) next );
				
				continue;
			}
		}
		
		addr = &next->qLink;
	}
}

static
void unimplemented_call( short selector : __D0 )
{
	FATAL = "unimplemented ShutDown call ", selector;
	
	asm { ILLEGAL }
}

static
void ShutDown_call( short selector : __D0 )
{
	/*
		TODO:  Perform various housekeeping tasks, like
		telling drivers goodbye and unmounting volumes.
	*/
	
	call_procs( selector, sdOnDrivers );
	
	UnloadScrap();
	
	call_procs( selector, sdOnUnmount );
	
	call_procs( selector, 0 );  // either sdOnPowerOff or sdOnRestart
	
	if ( linger_on_exit )
	{
		pause();
	}
	
	_exit( 0 );
}

static
pascal
void ShutDwnInstall_call( ShutDwnUPP proc, short flags )
{
	typedef shutdown_task Task;
	
	Task* task = (Task*) NewPtrSysClear( sizeof (Task) );
	
	task->flags = flags;
	task->proc  = proc;
	
	Enqueue( (QElemPtr) task, &ShutdownQueue );
}

static void* handlers[] =
{
	&unimplemented_call,
	&ShutDown_call,        // ShutDwnPower
	&ShutDown_call,        // ShutDwnStart
	&ShutDwnInstall_call,
};

enum
{
	n_handlers = COUNT( handlers ),
};

asm void ShutDown_patch( short selector )
{
	MOVEA.L  (SP)+,A0        // pop return address
	MOVE.W   (SP)+,D0        // pop selector
	MOVE.L   A0,-(SP)        // push return address
	
	LEA      handlers,A0
	
	CMPI.W   #n_handlers,D0  // selector <=> n_handlers
	BPL.S    just_go         // if selector >= n_handlers, use 0 offset
	
	MOVE.W   D0,D1           // copy selector as array index
	
	LSL.W    #2,D1           // scale index to pointer width
	ADDA.W   D1,A0           // apply byte offset
	
just_go:
	
	MOVEA.L  (A0),A0         // load the handler address
	
	JMP      (A0)
}
