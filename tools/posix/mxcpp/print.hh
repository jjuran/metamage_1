/*
	print.hh
	--------
*/

#ifndef MXCPP_PRINT_HH
#define MXCPP_PRINT_HH

// Standard C
#include <stddef.h>


namespace plus
{
	
	class string;
	
}

namespace tool
{
	
	void print( const char* line, size_t length, bool flush = false );
	
	void print( const plus::string& line, bool flush = false );
	
}

#endif
