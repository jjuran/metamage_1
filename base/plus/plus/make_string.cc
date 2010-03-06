/*
	plus/make_string.cc
	-------------------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/make_string.hh"


namespace plus
{
	
	std::string make_string( const unsigned char* s )
	{
		const char* data = (const char*) &s[1];
		
		const std::size_t length = s[0];
		
		return std::string( data, length );
	}
	
}

