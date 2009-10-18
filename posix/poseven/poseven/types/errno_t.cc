// errno_t.cc
// ----------

// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "poseven/types/errno_t.hh"

// Nucleus
#include "nucleus/error_code.hh"


namespace poseven
{
	
	void throw_errno_internal( errno_t number )
	{
		nucleus::throw_error_code< errno_t >( number );
	}
	
}

