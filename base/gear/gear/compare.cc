/*
	gear/compare.cc
	---------------
*/

#include "gear/compare.hh"


namespace gear
{
	
	cmp_t compare_bytes( byte_t const*  a,
	                     size_t         a_size,
	                     byte_t const*  b,
	                     size_t         b_size )
	{
		size_t size = b_size < a_size ? b_size : a_size;
		
		while ( size-- )
		{
			if ( *a < *b )
			{
				return -1;
			}
			
			if ( *a++ > *b++ )
			{
				return 1;
			}
		}
		
		return + a_size < b_size ? -1
		       : a_size > b_size ?  1
		       :                    0;
	}
	
}
