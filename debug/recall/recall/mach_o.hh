/*	=========
 *	mach_o.hh
 *	=========
 */

#ifndef RECALL_MACHO_HH
#define RECALL_MACHO_HH

// Standard C++
#include <string>

// Recall
#include "recall/return_address.hh"


namespace recall
{
	
	const char* find_symbol_name( return_address_mach_o addr );
	
	inline std::string get_symbol_string( const char* name )
	{
		return name;
	}
	
}

#endif

