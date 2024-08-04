/*	===========
 *	demangle.hh
 *	===========
 */

#ifndef RECALL_DEMANGLE_HH
#define RECALL_DEMANGLE_HH

// recall
#include "recall/enough_string.hh"


namespace recall
{
	
	void demangle_MWC68K( enough_string& result, const char* name );
	void demangle_MWCPPC( enough_string& result, const char* name );
	
}

#endif
