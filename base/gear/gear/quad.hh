/*
	gear/quad.hh
	------------
*/

#ifndef GEAR_QUAD_HH
#define GEAR_QUAD_HH


namespace gear
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

