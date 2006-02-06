// Assert.h

#ifndef NUCLEUS_NASSERT_H
#define NUCLEUS_NASSERT_H


#ifdef NUCLEUS_DEBUG

#define ASSERT( cond )  Nucleus::Assert( cond, #cond,  __FILE__, __LINE__ )

#else

#define ASSERT( cond )  // nothing

#endif

namespace Nucleus
{
	
	void Assert( bool condition, const char* text, const char* file, int line );
	
}

#endif

