/*
	print.hh
	--------
*/

#ifndef LOGOFWAR_PRINT_HH
#define LOGOFWAR_PRINT_HH

namespace logofwar
{
	
	enum hex32_t
	{
		hex32_min = 0,
		hex32_max = 0xffffffff,
	};
	
	void print( const char* s, unsigned n );
	
	void print( const char* s );
	
	void print_dec( int x );
	void print_hex( int x );
	
	inline void print( int x )  { print_dec( x ); }
	
	inline
	void print( hex32_t x )
	{
		logofwar::print( "$" );
		
		logofwar::print_hex( x );
	}
	
}

#endif
