// Nucleus/NAssert.h
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2005-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_NASSERT_H
#define NUCLEUS_NASSERT_H

#ifndef NUCLEUS_DEBUGGING_H
#include "Nucleus/Debugging.h"
#endif


#if TARGET_CONFIG_DEBUGGING
	
	#define ASSERT( cond )  if ( !(cond) ) Nucleus::ReportAssertionFailureAndAbort( #cond,  __FILE__, __LINE__ ); else 0
	
#else
	
	#define ASSERT( cond )  if ( 0 ) (void) !(cond); else 0
	
#endif


namespace Nucleus
{
	
	void ReportAssertionFailureAndAbort( const char* text, const char* file, int line );
	
}

#endif

