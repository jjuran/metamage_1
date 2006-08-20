/*	===================
 *	StringPredicates.hh
 *	===================
 */

#ifndef BITSANDBYTES_STRINGPREDICATES_HH
#define BITSANDBYTES_STRINGPREDICATES_HH

// Standard C++
#include <string>


namespace BitsAndBytes
{
	
	// End of string
	inline bool eos( std::size_t pos )
	{
		return pos == std::string::npos;
	}
	
}

#endif

