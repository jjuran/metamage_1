/*
	c_string.hh
	-----------
*/

#ifndef CSTRING_HH
#define CSTRING_HH

// ams-common
#include "callouts.hh"


#define CSTR( s )  (*c_string(s))


class c_string
{
	private:
		char buffer[ 256 ];
	
	public:
		c_string( const unsigned char* s );
		
		const char* operator*() const  { return buffer; }
};

inline
c_string::c_string( const unsigned char* s )
{
	const unsigned long n = s[ 0 ];
	
	fast_memcpy( buffer, s + 1, n );
	
	buffer[ n ] = '\0';
}

#endif
