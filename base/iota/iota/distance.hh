/*
	iota/distance.hh
	----------------
	
	Joshua Juran
*/

#ifndef IOTA_DISTANCE_HH
#define IOTA_DISTANCE_HH


namespace iota
{
	
	inline unsigned long distance( const void* begin, const void* end )
	{
		return static_cast< const char* >( end ) - static_cast< const char* >( begin );
	}
	
}

#endif

