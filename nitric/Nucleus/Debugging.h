// Nucleus/Debugging.h
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_DEBUGGING_H
#define NUCLEUS_DEBUGGING_H


#ifndef TARGET_CONFIG_DEBUGGING

	#if defined( NUCLEUS_DEBUG ) || defined( NITROGEN_DEBUG )
	
		#define TARGET_CONFIG_DEBUGGING 1
	
	#else
	
		#define TARGET_CONFIG_DEBUGGING 0
	
	#endif

#endif


#if TARGET_CONFIG_DEBUGGING && defined( NUCLEUS_USES_BACKTRACE )
	
	// Recall
	#include "recall/backtrace.hh"
	
	#define NUCLEUS_DEBUGGING_CONTEXT recall::DebuggingContext
	
#endif


namespace Nucleus
{
	
	struct NullDebuggingContext
	{
		void Show() const  {}
	};
	
#ifndef NUCLEUS_DEBUGGING_CONTEXT
	
	#define NUCLEUS_DEBUGGING_CONTEXT Nucleus::NullDebuggingContext
	
#endif
	
	typedef NUCLEUS_DEBUGGING_CONTEXT DebuggingContext;
	
}

#endif

