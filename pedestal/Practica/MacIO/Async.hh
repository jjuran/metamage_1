/*	========
 *	Async.hh
 *	========
 */

#ifndef MACIO_ASYNC_HH
#define MACIO_ASYNC_HH

// MacIO
#include "MacIO/Routines.hh"


namespace MacIO
{
	
	template < class Call_Traits, class Policy, class Callback >
	typename Policy::Result
	//
	PBAsync( typename Call_Traits::PB&  pb,
	         Callback                   callback,
	         ::IOCompletionUPP          completion )
	{
		Call_Traits::IOCompletion( pb ) = completion;
		
		Policy::HandleOSStatus( Call_Traits::Async( pb ) );
			
		while ( Call_Traits::IOResult( pb ) > 0 )
		{
			callback();
		}
		
		return Policy::HandleOSStatus( Call_Traits::IOResult( pb ) );
	}
	
}

#endif

