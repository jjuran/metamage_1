// Timer.h

#ifndef NITROGEN_TIMER_H
#define NITROGEN_TIMER_H

#ifndef __TIMER__
#include <Timer.h>
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif


namespace Nitrogen
{
	
	void RegisterTimeManagerErrors();
	
	using ::TMTask;
	using ::TMTaskPtr;
	
	template <>
	struct Disposer< TMTaskPtr > : public  std::unary_function< TMTaskPtr, void >,
	                               private DefaultDestructionOSStatusPolicy
	{
		void operator()( TMTaskPtr tmTaskPtr ) const
		{
			OnlyOnce< RegisterTimeManagerErrors >();
			
			HandleDestructionOSStatus( ::RemoveTimeTask( reinterpret_cast< QElemPtr >( tmTaskPtr ) ) );
		}
	};
	
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
	
	Owned< TMTaskPtr > InstallTimeTask ( TMTask& tmTask );
	Owned< TMTaskPtr > InstallXTimeTask( TMTask& tmTask );
	
	void PrimeTimeTask( TMTask& tmTask, long count );
	
	void RemoveTimeTask( Owned< TMTaskPtr > tmTask );
	
	// ...
	
	UnsignedWide Microseconds();
	
	inline Owned< TimerUPP > NewTimerUPP( ::TimerProcPtr p )
	{
		return NewUPP< TimerUPP >( p );
	}
	
	inline void DisposeTimerUPP( Owned< TimerUPP > )  {}
	
#if !TARGET_CPU_68K || TARGET_RT_MAC_CFM
	
	inline void InvokeTimerUPP( TMTaskPtr tmTaskPtr,
	                            TimerUPP  userUPP )
	{
		userUPP( tmTaskPtr );
	}
	
#endif
	
}

#endif

