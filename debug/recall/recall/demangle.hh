/*	===========
 *	demangle.hh
 *	===========
 */

#ifndef RECALL_DEMANGLE_HH
#define RECALL_DEMANGLE_HH

// plus
#include "plus/string_fwd.hh"
#include "plus/var_string_fwd.hh"


namespace recall
{
	
	void demangle_MWC68K( plus::var_string& result, const plus::string& name );
	void demangle_MWCPPC( plus::var_string& result, const plus::string& name );
	
}

#endif
