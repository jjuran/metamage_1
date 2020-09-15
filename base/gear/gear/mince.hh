/*
	gear/mince.hh
	-------------
*/

#ifndef GEAR_MINCE_HH
#define GEAR_MINCE_HH


namespace gear
{
	
	inline
	unsigned minced_length( const char* src, unsigned len, unsigned stride )
	{
		return (len + stride - 1) / stride;
	}
	
	char* mince( char* dst, const char* src, const char* end, unsigned stride );
	
}

#endif
