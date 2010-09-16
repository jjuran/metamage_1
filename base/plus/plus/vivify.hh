/*
	vivify.hh
	---------
*/

#ifndef PLUS_VIVIFY_HH
#define PLUS_VIVIFY_HH

// iota
#include "iota/decimal.hh"


namespace plus
{
	
	struct vivify_bool
	{
		typedef bool result_type;
		
		static bool apply( const char* begin, const char* end )
		{
			return iota::parse_unsigned_decimal( begin ) != 0;
		}
	};
	
	template < class Int >
	struct vivify_int
	{
		typedef Int result_type;
		
		static Int apply( const char* begin, const char* end )
		{
			return Int( iota::parse_decimal( begin ) );
		}
	};
	
	template < class Int >
	struct vivify_unsigned
	{
		typedef Int result_type;
		
		static Int apply( const char* begin, const char* end )
		{
			return Int( iota::parse_unsigned_decimal( begin ) );
		}
	};
	
}

#endif

