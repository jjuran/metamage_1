// Nitrogen/Timer.h
// ----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
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
	namespace Private
	{
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
		
		inline void InvokeTimerUPP( TMTaskPtr tmTaskPtr, ::TimerUPP userUPP )
		{
			::InvokeTimerUPP( tmTaskPtr, userUPP );
		}
		
	#else
		
		using ::InvokeTimerUPP;
		
	#endif
	}
	
	struct TimerUPP_Details : Basic_UPP_Details< ::TimerUPP,
	                                             ::TimerProcPtr,
	                                             ::NewTimerUPP,
	                                             ::DisposeTimerUPP,
	                                             Private::InvokeTimerUPP >
	{};
	
	typedef UPP< TimerUPP_Details > TimerUPP;
	
	Nucleus::Owned< TMTaskPtr > InstallTimeTask ( TMTask& tmTask );
	Nucleus::Owned< TMTaskPtr > InstallXTimeTask( TMTask& tmTask );
	
	void PrimeTimeTask( TMTask& tmTask, long count );
	
	void RemoveTimeTask( Nucleus::Owned< TMTaskPtr > tmTask );
	
	// ...
	
	UnsignedWide Microseconds();
	
	inline Nucleus::Owned< TimerUPP > NewTimerUPP( ::TimerProcPtr p )
	{
		return NewUPP< TimerUPP >( p );
	}
	
	inline void DisposeTimerUPP( Nucleus::Owned< TimerUPP > )  {}
	
#if !TARGET_CPU_68K || TARGET_RT_MAC_CFM
	
	inline void InvokeTimerUPP( TMTaskPtr tmTaskPtr,
	                            TimerUPP  userUPP )
	{
		userUPP( tmTaskPtr );
	}
	
#endif
	
}

#endif

