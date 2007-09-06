// Nucleus/Debugging.cp
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_DEBUGGING_H
#include "Nucleus/Debugging.h"
#endif

#include <cstdio>


namespace Nucleus
{
	
	void DefaultDebuggingContext::Show() const
	{
		if ( file == NULL  ||  text == NULL )
		{
			return;
		}
		
		std::fprintf( stderr, "%s:%d: %s\n", file, line, text );
	}
	
}

