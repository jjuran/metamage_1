// Assert.cp

#ifndef NUCLEUS_NASSERT_H
#include "Nucleus/NAssert.h"
#endif

// Standard C/C++
#include <cstdio>
#include <cstdlib>

namespace Nucleus
{
	
	void Assert( bool condition, const char* text, const char* file, int line )
	{
		if ( !condition )
		{
			std::fprintf( stderr,
			              "Assertion '%s' failed in %s, line %d\n",
			              text,
			              file,
			              line );
			
			std::exit( EXIT_FAILURE );
		}
	}
	
}

