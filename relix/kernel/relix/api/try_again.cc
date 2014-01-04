/*
	try_again.cc
	------------
*/

#include "relix/api/try_again.hh"

// Standard C
#include <errno.h>

// poseven
#include "poseven/types/errno_t.hh"

// relix-kernel
#include "relix/api/yield.hh"


namespace relix
{
	
	namespace p7 = poseven;
	
	
	void try_again( bool nonblocking )
	{
		if ( nonblocking )
		{
			p7::throw_errno( EAGAIN );
		}
		
		yield( true );  // throw caught signals
	}
	
}

