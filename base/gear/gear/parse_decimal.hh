/*
	gear/parse_decimal.hh
	---------------------
*/

#ifndef GEAR_PARSEDECIMAL_HH
#define GEAR_PARSEDECIMAL_HH


namespace gear
{
	
	unsigned parse_unsigned_decimal( const char **pp );
	
	int parse_decimal( const char **pp );
	
	inline unsigned parse_unsigned_decimal( const char *p )
	{
		return parse_unsigned_decimal( &p );
	}
	
	inline int parse_decimal( const char *p )
	{
		return parse_decimal( &p );
	}
	
}

#endif

