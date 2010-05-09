/*
	plus/make_string.cc
	-------------------
	
	Copyright 2010, Joshua Juran
*/

#include "plus/make_string.hh"


namespace plus
{
	
	string make_string( const unsigned char* s )
	{
		const char* data = (const char*) &s[1];
		
		const unsigned long length = s[0];
		
		return string( data, length );
	}
	
}

