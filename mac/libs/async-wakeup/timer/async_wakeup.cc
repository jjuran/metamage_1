/*
	async_wakeup.cc
	---------------
*/

#include "mac_sys/async_wakeup.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef MAC_OS_X_VERSION_10_8
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif
#endif
#ifndef __PROCESSES__
#include <Processes.h>
#endif
#ifndef __TIMER__
#include <Timer.h>
#endif

// mac-sys-utils
#include "mac_sys/current_process.hh"
#include "mac_sys/delay.hh"
#include "mac_sys/gestalt.hh"


#if TARGET_CPU_68K
	Byte CPUFlag : 0x12F;
	
	#define IN( reg ) : __##reg
#else
	#define IN( reg )  /**/
#endif

namespace mac {
namespace sys {

using mac::sys::gestalt;

static volatile bool wakeup_requested;

const bool is_osx = TARGET_API_MAC_CARBON  &&  gestalt( 'sysv' ) >= 0x1000;

static inline
void prime_timer( TMTaskPtr task )
{
	/*
		Expect the timer task as an argument instead of using a global,
		so on 68K we don't rely on the value of A5.
	*/
	
	::PrimeTime( (QElemPtr) task, 10 );  // 10ms
}

struct wakeup_TMTask
{
	TMTask                      tm;
	const ProcessSerialNumber*  psn;
};

static pascal
void async_wakeup_proc( TMTaskPtr task IN( a1 ) )
{
	if ( wakeup_requested )
	{
		prime_timer( task );
		
		::WakeUpProcess( ((const wakeup_TMTask*) task)->psn );
	}
}

static wakeup_TMTask the_wakeup_timer =
{
	{
		0,
		0,
		::NewTimerUPP( (TimerProcPtr) &async_wakeup_proc ),
		0,
		0,
		0
	},
	&current_process()
};

static pascal
void deferred_wakeup( long param IN( a1 ) )
{
	do
	{
		::WakeUpProcess( the_wakeup_timer.psn );
		
		mac::sys::delay( 1 );  // one tick == 16ms
	}
	while ( wakeup_requested );
}

static DeferredTask the_wakeup_dt =
{
	0,
	0,
	0,
	is_osx ? ::NewDeferredTaskUPP( (DeferredTaskProcPtr) &deferred_wakeup )
	       : 0,
	0,
	0,
};

static inline
void schedule_wakeup( TMTaskPtr task )
{
	if ( is_osx )
	{
		OSErr err = DTInstall( &the_wakeup_dt );
	}
	else
	{
		prime_timer( task );
	}
}

/*
	Determine whether the ProcessSerialNumber for this process (as
	determined by mac-sys-utils) is real or not.  In the absence of
	System 7 or MultiFinder, it's zero in both fields (which is the
	same as {0, kNoProcess}, which GetCurrentProcess() will never
	return.)
	
	In a single-process system, there's no need to request wakeups,
	since this process is always in front and WaitNextEvent() will
	always return nullEvents instead of sleeping.
*/

static inline
bool is_multiproc_PSN( const ProcessSerialNumber& psn )
{
	return ! TARGET_CPU_68K  ||  psn.lowLongOfPSN != 0;
}

const bool multiproc_available = is_multiproc_PSN( *the_wakeup_timer.psn );

static
Boolean lock_timer()
{
	register void** mutex = (void**) &the_wakeup_timer.tm.qLink;
	
#if TARGET_CPU_68K
	
	const Byte cpu68000 = 0;
	
	if ( CPUFlag == cpu68000 )
	{
		// 68000 only
		
		register Boolean acquired = false;  // Initialize to squelch warning
		
		asm
		{
			TAS    (mutex)
			SEQ    acquired
		}
		
		return acquired;
	}
	
#endif
	
#ifndef MAC_OS_X_VERSION_10_8
	
	return ::OTCompareAndSwapPtr( NULL, (void*) 0xFFFFFFFF, mutex );
	
#endif
	
	return false;
}

void request_async_wakeup()
{
	if ( ! multiproc_available )
	{
		return;
	}
	
	wakeup_requested = true;
	
	if ( lock_timer() )
	{
		the_wakeup_timer.tm.tmWakeUp   = 0;
		the_wakeup_timer.tm.tmReserved = 0;
		
		::InsTime( (QElemPtr) &the_wakeup_timer );
	}
	
	schedule_wakeup( &the_wakeup_timer.tm );
	
	::WakeUpProcess( the_wakeup_timer.psn );
}

void clear_async_wakeup()
{
	wakeup_requested = false;
}

}
}
