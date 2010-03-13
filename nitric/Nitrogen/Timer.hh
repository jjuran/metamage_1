// Nitrogen/Timer.hh
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_TIMER_HH
#define NITROGEN_TIMER_HH

// Mac OS
#ifndef __TIMER__
#include <Timer.h>
#endif

// Nucleus
#include "Nucleus/ErrorsRegistered.h"

// Nitrogen
#include "Nitrogen/MacTypes.hh"
#include "Nitrogen/OSStatus.hh"
#include "Nitrogen/UPP.hh"


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( TimeManager );
	
	using ::TMTask;
	using ::TMTaskPtr;
	
}

namespace nucleus
{
	
	template <>
	struct disposer< TMTaskPtr > : public std::unary_function< TMTaskPtr, void >
	{
		void operator()( TMTaskPtr tmTaskPtr ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::TimeManager );
			
			const QElemPtr qElem = (QElemPtr) tmTaskPtr;
			
			::Nitrogen::HandleDestructionOSStatus( ::RemoveTimeTask( qElem ) );
		}
	};
	
}

namespace Nitrogen
{
	
	typedef pascal void (*TimerProcPtr)( TMTaskPtr tmTaskPtr );
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	struct TimerUPP_Details
	{
		typedef ::TimerUPP UPPType;
		
		// This is the stack-based function signature
		typedef TimerProcPtr ProcPtr;
		
		template < ProcPtr procPtr >
		static pascal void Glue()
		{
			Call_With_A0_Glue< ProcPtr, procPtr >();
		}
	};
	
	typedef GlueUPP< TimerUPP_Details > TimerUPP;
	
#else
	
	struct TimerUPP_Details : Basic_UPP_Details< ::TimerUPP,
	                                             ::TimerProcPtr,
	                                             ::NewTimerUPP,
	                                             ::DisposeTimerUPP,
	                                             ::InvokeTimerUPP >
	{
	};
	
	typedef UPP< TimerUPP_Details > TimerUPP;
	
#endif
	
	nucleus::owned< TMTaskPtr > InstallTimeTask ( TMTask& tmTask );
	nucleus::owned< TMTaskPtr > InstallXTimeTask( TMTask& tmTask );
	
	void PrimeTimeTask( TMTask& tmTask, long count );
	
	void RemoveTimeTask( nucleus::owned< TMTaskPtr > tmTask );
	
	// ...
	
	UnsignedWide Microseconds();
	
	inline nucleus::owned< TimerUPP > NewTimerUPP( TimerProcPtr p )
	{
		return NewUPP< TimerUPP >( p );
	}
	
	inline void DisposeTimerUPP( nucleus::owned< TimerUPP > )
	{
	}
	
	inline void InvokeTimerUPP( TMTaskPtr  tmTaskPtr,
	                            TimerUPP   userUPP )
	{
		::InvokeTimerUPP( tmTaskPtr, userUPP.Get() );
	}
	
}

#endif

