/*
	iota/quad.hh
	------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef IOTA_QUAD_HH
#define IOTA_QUAD_HH


namespace iota
{
	
	unsigned long decode_quad( const unsigned char* s );
	
	void encode_quad( unsigned long q, unsigned char* s );
	
	
	inline unsigned long decode_quad( const char* s )
	{
		return decode_quad( (const unsigned char*) s );
	}
	
	inline void encode_quad( unsigned long q, char* s )
	{
		encode_quad( q, (unsigned char*) s );
	}
	
}

#endif

