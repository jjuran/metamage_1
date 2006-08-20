/*	===================
 *	StringPredicates.hh
 *	===================
 *	
 *	No implementation necessary.
 */

#pragma once

// Standard C++
#include <string>


namespace BitsAndBytes {
	
	using std::string;
	
	// End of string
	inline bool eos(string::size_type pos)
	{
		return pos == string::npos;
	}
	
}

