/*
	print.hh
	--------
*/

#ifndef LOGOFWAR_PRINT_HH
#define LOGOFWAR_PRINT_HH

namespace logofwar
{
	
	void print( const char* s, unsigned n );
	
	void print( const char* s );
	
	void print_dec( int x );
	void print_hex( int x );
	
	inline void print( int x )  { print_dec( x ); }
	
}

#endif
