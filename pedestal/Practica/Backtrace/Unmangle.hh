/*	===========
 *	Unmangle.hh
 *	===========
 */

#ifndef BACKTRACE_UNMANGLE_HH
#define BACKTRACE_UNMANGLE_HH

// Standard C++
#include <string>


namespace Backtrace
{
	
	class Unmangle_Failed {};
	
	std::string UnmangleName68K( const char* name, const char* end );
	std::string UnmangleNamePPC( const char* name, const char* end );
	
}

#endif

