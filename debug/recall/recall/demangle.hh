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
	
	void demangle_MWC68K( std::string& result, const std::string& name );
	void demangle_MWCPPC( std::string& result, const std::string& name );
	
}

#endif

