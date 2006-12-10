// Timer.cp

#ifndef NITROGEN_TIMER_H
#include "Nitrogen/Timer.h"
#endif

namespace Nitrogen
{
	
	TimeManagerErrorsRegistrationDependency::TimeManagerErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterTimeManagerErrors();
	
	
	class TimeManagerErrorsRegistration
	{
		public:
			TimeManagerErrorsRegistration()  { RegisterTimeManagerErrors(); }
	};
	
	static TimeManagerErrorsRegistration theRegistration;
	
	
	Nucleus::Owned< TMTaskPtr > InstallTimeTask ( TMTask& tmTask )
	{
		ThrowOSStatus( ::InstallTimeTask( reinterpret_cast< QElemPtr >( &tmTask ) ) );
		return Nucleus::Owned< TMTaskPtr >::Seize( &tmTask );
	}
	
	Nucleus::Owned< TMTaskPtr > InstallXTimeTask( TMTask& tmTask )
	{
		ThrowOSStatus( ::InstallXTimeTask( reinterpret_cast< QElemPtr >( &tmTask ) ) );
		return Nucleus::Owned< TMTaskPtr >::Seize( &tmTask );
	}
	
	void PrimeTimeTask( TMTask& tmTask, long count )
	{
		ThrowOSStatus( ::PrimeTimeTask( reinterpret_cast< QElemPtr >( &tmTask ), count ) );
	}
	
	void RemoveTimeTask( Nucleus::Owned< TMTaskPtr > tmTaskPtr )
	{
		ThrowOSStatus( ::RemoveTimeTask( reinterpret_cast< QElemPtr >( tmTaskPtr.Release() ) ) );
	}
	
	UnsignedWide Microseconds()
	{
		::UnsignedWide result;
		::Microseconds( &result );
		
		return *reinterpret_cast< UnsignedWide* >( &result );
	}
	
	void RegisterTimeManagerErrors()
	{
	//	RegisterOSStatus< /* no errors defined */ >();
	}
	
}

