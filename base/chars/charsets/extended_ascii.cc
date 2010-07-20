/*
	extended_ascii.cc
	-----------------
*/

#include "charsets/extended_ascii.hh"

// Standard C++
#include <algorithm>


namespace chars
{
	
	static inline bool operator<( const unicode_mapping& mapping, unichar_t uc )
	{
		return mapping.unicode < uc;
	}
	
	char extended_ascii_from_unicode( unichar_t              uc,
	                                  const unicode_mapping  map[] )
	{
		const unicode_mapping* map_end = map + n_extended_ASCII_mappings;
		
		const unicode_mapping* it = std::lower_bound( map, map_end, uc );
		
		if ( it != map_end  &&  it->unicode == uc )
		{
			return it->code;
		}
		
		return 0;
	}
	
	unsigned put_code_point_into_extended_ascii( unichar_t              uc,
	                                             char*                  p,
	                                             const unicode_mapping  map[] )
	{
		char c = uc;
		
		if ( uc >= 0x80 )
		{
			c = extended_ascii_from_unicode( uc, map );
			
			if ( c == 0 )
			{
				return 0;
			}
		}
		
		*p = c;
		
		return 1;
	}
	
}

