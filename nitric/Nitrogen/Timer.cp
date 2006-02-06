// Timer.cp

#ifndef NITROGEN_TIMER_H
#include "Nitrogen/Timer.h"
#endif

namespace Nitrogen
{
	
	Owned< TMTaskPtr > InstallTimeTask ( TMTask& tmTask )
	{
		OnlyOnce< RegisterTimeManagerErrors >();
		
		ThrowOSStatus( ::InstallTimeTask( reinterpret_cast< QElemPtr >( &tmTask ) ) );
		return Owned< TMTaskPtr >::Seize( &tmTask );
	}
	
	Owned< TMTaskPtr > InstallXTimeTask( TMTask& tmTask )
	{
		OnlyOnce< RegisterTimeManagerErrors >();
		
		ThrowOSStatus( ::InstallXTimeTask( reinterpret_cast< QElemPtr >( &tmTask ) ) );
		return Owned< TMTaskPtr >::Seize( &tmTask );
	}
	
	void PrimeTimeTask( TMTask& tmTask, long count )
	{
		OnlyOnce< RegisterTimeManagerErrors >();
		
		ThrowOSStatus( ::PrimeTimeTask( reinterpret_cast< QElemPtr >( &tmTask ), count ) );
	}
	
	void RemoveTimeTask( Owned< TMTaskPtr > tmTaskPtr )
	{
		OnlyOnce< RegisterTimeManagerErrors >();
		
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

