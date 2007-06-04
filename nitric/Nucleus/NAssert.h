// Nucleus/NAssert.h
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2005 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


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

