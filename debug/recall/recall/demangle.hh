/*	===========
 *	demangle.hh
 *	===========
 */

#ifndef RECALL_DEMANGLE_HH
#define RECALL_DEMANGLE_HH

// Standard C++
#include <string>


namespace recall
{
	
	class Unmangle_Failed {};
	
	std::string UnmangleMWC68K( const std::string& name );
	std::string UnmangleMWCPPC( const std::string& name );
	
}

#endif

