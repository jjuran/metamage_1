/*	=======================
 *	Divergence/Utilities.hh
 *	=======================
 */

#ifndef DIVERGENCE_UTILITIES_HH
#define DIVERGENCE_UTILITIES_HH

#if !defined( __MACOS__ ) && !defined( __APPLE__ )

#error Divergence is Mac-only

#endif

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif


namespace Divergence
{
	
	FSSpec ResolvePathToFSSpec( const char* path );
	
}

#endif
