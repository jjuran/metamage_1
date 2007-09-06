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

namespace Nucleus
{
	
	struct NullDebuggingContext
	{
		void Show() const  {}
	};
	
	struct DefaultDebuggingContext
	{
		const char*  text;
		const char*  file;
		int          line;
		
		DefaultDebuggingContext() : text(), file(), line()  {}
		
		DefaultDebuggingContext( const char*  t,
		                         const char*  f,
		                         int          l ) : text( t ),
		                                            file( f ),
		                                            line( l )
		{
		}
		
		void Show() const;
	};
	
	
	
#ifndef NUCLEUS_DEBUGGING_CONTEXT
	
	#if TARGET_CONFIG_DEBUGGING
	
		#define NUCLEUS_DEBUGGING_CONTEXT Nucleus::DefaultDebuggingContext
		
		#ifdef __GNUC__
		
			#define NUCLEUS_CREATE_DEBUGGING_CONTEXT()  Nucleus::DebuggingContext( __PRETTY_FUNCTION__, __FILE__, __LINE__ )
		
		#else
		
			#define NUCLEUS_CREATE_DEBUGGING_CONTEXT()  Nucleus::DebuggingContext( "", __FILE__, __LINE__ )
		
		#endif
		
	#else
	
		#define NUCLEUS_DEBUGGING_CONTEXT Nucleus::NullDebuggingContext
	
	#endif
	
#endif
	
	typedef NUCLEUS_DEBUGGING_CONTEXT DebuggingContext;
	
#ifndef NUCLEUS_CREATE_DEBUGGING_CONTEXT
	
	#define NUCLEUS_CREATE_DEBUGGING_CONTEXT()  Nucleus::DebuggingContext()
	
#endif
	
}

#endif

