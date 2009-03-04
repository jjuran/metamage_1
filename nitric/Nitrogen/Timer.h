// Nitrogen/Timer.h
// ----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_TIMER_H
#define NITROGEN_TIMER_H

#ifndef __TIMER__
#include <Timer.h>
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif
#include "Nucleus/ErrorsRegistered.h"


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( TimeManager );
	
	using ::TMTask;
	using ::TMTaskPtr;
  }

namespace Nucleus
  {
	template <>
	struct Disposer< Nitrogen::TMTaskPtr > : public  std::unary_function< Nitrogen::TMTaskPtr, void >,
                                             private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::TMTaskPtr tmTaskPtr ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::TimeManager );
			
			HandleDestructionOSStatus( ::RemoveTimeTask( reinterpret_cast< ::QElemPtr >( tmTaskPtr ) ) );
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
	
	Nucleus::Owned< TMTaskPtr > InstallTimeTask ( TMTask& tmTask );
	Nucleus::Owned< TMTaskPtr > InstallXTimeTask( TMTask& tmTask );
	
	void PrimeTimeTask( TMTask& tmTask, long count );
	
	void RemoveTimeTask( Nucleus::Owned< TMTaskPtr > tmTask );
	
	// ...
	
	UnsignedWide Microseconds();
	
	inline Nucleus::Owned< TimerUPP > NewTimerUPP( TimerProcPtr p )
	{
		return NewUPP< TimerUPP >( p );
	}
	
	inline void DisposeTimerUPP( Nucleus::Owned< TimerUPP > )  {}
	
	inline void InvokeTimerUPP( TMTaskPtr tmTaskPtr,
	                            TimerUPP  userUPP )
	{
		::InvokeTimerUPP( tmTaskPtr, userUPP.Get() );
	}
	
}

#endif

