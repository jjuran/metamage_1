/*	===========
 *	demangle.hh
 *	===========
 */

#ifndef RECALL_DEMANGLE_HH
#define RECALL_DEMANGLE_HH


namespace plus
{
	
	class string;
	class var_string;
	
}

namespace recall
{
	
	class demangle_failed {};
	
	void demangle_MWC68K( plus::var_string& result, const plus::string& name );
	void demangle_MWCPPC( plus::var_string& result, const plus::string& name );
	
}

#endif

