/*
	freeaddrinfo.hh
	---------------
*/

#ifndef POSEVEN_FUNCTIONS_FREEADDRINFO_HH
#define POSEVEN_FUNCTIONS_FREEADDRINFO_HH

// POSIX
#include <netdb.h>

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif


namespace nucleus
{
	
	template <>
	struct disposer< ::addrinfo* >
	{
		typedef ::addrinfo*  argument_type;
		typedef void         result_type;
		
		void operator()( ::addrinfo* ai ) const
		{
			::freeaddrinfo( ai );
		}
	};
	
}

namespace poseven
{
	
	void freeaddrinfo( nucleus::owned< ::addrinfo* > ai );
	
}

#endif
