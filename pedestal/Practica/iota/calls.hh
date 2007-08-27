/*	=============
 *	iota/calls.hh
 *	=============
 */

#ifndef IOTA_CALLS_HH
#define IOTA_CALLS_HH

// Standard C/C++
#include <cstdio>
#include <cstdlib>


namespace iota
{
	
	inline int Call( int result, const char* debug_message )
	{
		if ( result == -1 )
		{
			std::perror( debug_message );
			
			std::abort();
		}
		
		return result;
	}
	
}

#define CALL( call )  iota::Call( call, __FILE__ ": " #call )

#endif

