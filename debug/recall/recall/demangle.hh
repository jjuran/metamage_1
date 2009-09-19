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
	
	class demangle_failed {};
	
	std::string demangle_MWC68K( const std::string& name );
	std::string demangle_MWCPPC( const std::string& name );
	
}

#endif

