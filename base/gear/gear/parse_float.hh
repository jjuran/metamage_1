/*
	gear/parse_float.hh
	-------------------
*/

#ifndef GEAR_PARSEFLOAT_HH
#define GEAR_PARSEFLOAT_HH


namespace gear
{
	
	float parse_float( const char** pp );
	
	inline float parse_float( const char* begin )
	{
		return parse_float( &begin );
	}
	
}

#endif

