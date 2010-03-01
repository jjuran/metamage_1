// Nitrogen/Timer.cc
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/Timer.hh"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( TimeManager )
	
	
	static void RegisterTimeManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class TimeManagerErrorsRegistration
	{
		public:
			TimeManagerErrorsRegistration()  { RegisterTimeManagerErrors(); }
	};
	
	static TimeManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	nucleus::owned< TMTaskPtr > InstallTimeTask ( TMTask& tmTask )
	{
		ThrowOSStatus( ::InstallTimeTask( reinterpret_cast< QElemPtr >( &tmTask ) ) );
		return nucleus::owned< TMTaskPtr >::seize( &tmTask );
	}
	
	nucleus::owned< TMTaskPtr > InstallXTimeTask( TMTask& tmTask )
	{
		ThrowOSStatus( ::InstallXTimeTask( reinterpret_cast< QElemPtr >( &tmTask ) ) );
		return nucleus::owned< TMTaskPtr >::seize( &tmTask );
	}
	
	void PrimeTimeTask( TMTask& tmTask, long count )
	{
		ThrowOSStatus( ::PrimeTimeTask( reinterpret_cast< QElemPtr >( &tmTask ), count ) );
	}
	
	void RemoveTimeTask( nucleus::owned< TMTaskPtr > tmTaskPtr )
	{
		ThrowOSStatus( ::RemoveTimeTask( reinterpret_cast< QElemPtr >( tmTaskPtr.release() ) ) );
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

