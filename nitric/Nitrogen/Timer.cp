// Timer.cp

#ifndef NITROGEN_TIMER_H
#include "Nitrogen/Timer.h"
#endif

namespace Nitrogen
{
	
	Nucleus::Owned< TMTaskPtr > InstallTimeTask ( TMTask& tmTask )
	{
		Nucleus::OnlyOnce< RegisterTimeManagerErrors >();
		
		ThrowOSStatus( ::InstallTimeTask( reinterpret_cast< QElemPtr >( &tmTask ) ) );
		return Nucleus::Owned< TMTaskPtr >::Seize( &tmTask );
	}
	
	Nucleus::Owned< TMTaskPtr > InstallXTimeTask( TMTask& tmTask )
	{
		Nucleus::OnlyOnce< RegisterTimeManagerErrors >();
		
		ThrowOSStatus( ::InstallXTimeTask( reinterpret_cast< QElemPtr >( &tmTask ) ) );
		return Nucleus::Owned< TMTaskPtr >::Seize( &tmTask );
	}
	
	void PrimeTimeTask( TMTask& tmTask, long count )
	{
		Nucleus::OnlyOnce< RegisterTimeManagerErrors >();
		
		ThrowOSStatus( ::PrimeTimeTask( reinterpret_cast< QElemPtr >( &tmTask ), count ) );
	}
	
	void RemoveTimeTask( Nucleus::Owned< TMTaskPtr > tmTaskPtr )
	{
		Nucleus::OnlyOnce< RegisterTimeManagerErrors >();
		
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

