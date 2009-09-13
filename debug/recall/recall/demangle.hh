/*	===========
 *	Unmangle.hh
 *	===========
 */

#ifndef BACKTRACE_UNMANGLE_HH
#define BACKTRACE_UNMANGLE_HH

// Standard C++
#include <string>


namespace recall
{
	
	class Unmangle_Failed {};
	
	std::string UnmangleMWC68K( const std::string& name );
	std::string UnmangleMWCPPC( const std::string& name );
	
}

#endif

