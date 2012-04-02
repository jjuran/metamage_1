/*
	string_common.cc
	----------------
*/

#include "plus/string_common.hh"

// Standard C++
#include <stdexcept>


#define LENGTH_ERROR_MESSAGE  "string size can't exceed 0x7fffffff"


namespace plus
{
	
	void string_check_size( long size )
	{
		// 2 GB limit on 32-bit platforms
		
		if ( size < 0 )
		{
			const bool _32bit = sizeof size == 4;
			
			const char* message = _32bit ? LENGTH_ERROR_MESSAGE
			                             : LENGTH_ERROR_MESSAGE "ffffffff";
			
			throw std::length_error( message );
		}
	}
	
}

