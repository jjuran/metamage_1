/*	===========
 *	demangle.hh
 *	===========
 */

#ifndef RECALL_DEMANGLE_HH
#define RECALL_DEMANGLE_HH

// plus
#include "plus/var_string_fwd.hh"


namespace recall
{
	
	void demangle_MWC68K( plus::var_string& result, const char* name );
	void demangle_MWCPPC( plus::var_string& result, const char* name );
	
}

#endif
