// Nucleus/NAssert.cp
// ------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2005-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_NASSERT_H
#include "Nucleus/NAssert.h"
#endif

// Standard C/C++
#include <cstdio>
#include <cstdlib>

namespace Nucleus
{
	
	void ReportAssertionFailureAndAbort( const char* text, const char* file, int line )
	{
		std::fprintf( stderr,
		              "Assertion '%s' failed in %s, line %d\n",
		              text,
		              file,
		              line );
		
		std::abort();
	}
	
}

