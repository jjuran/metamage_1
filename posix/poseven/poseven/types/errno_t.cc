// errno_t.cc
// ----------

// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "poseven/types/errno_t.hh"

// Nucleus
#include "Nucleus/ErrorCode.h"


namespace poseven
{
	
	void throw_errno_internal( errno_t number )
	{
		Nucleus::ThrowErrorCode< errno_t >( number );
	}
	
}

