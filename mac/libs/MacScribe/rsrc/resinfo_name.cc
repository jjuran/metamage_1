/*
	resinfo_name.cc
	---------------
*/

#include "rsrc/resinfo_name.hh"

// more-libc
#include "more/string.h"

// gear
#include "gear/hexidecimal.hh"

// MacScribe
#include "quad/quad_name.hh"

// Nitrogen
#include "Nitrogen/Resources.hh"


namespace MacScribe
{
	
	namespace N = Nitrogen;
	
	
	plus::string get_name_from_ResInfo( const N::GetResInfo_Result& info )
	{
		plus::string type_quad = make_quad_name( info.type );
		
		const std::size_t type_size = type_quad.size();
		
		const bool has_name = info.name[0];
		
		const std::size_t size = 4 + has_name * (1 + info.name[0]) + 1 + type_size;
		
		plus::string result;
		
		char* p = result.reset( size );
		
		gear::encode_16_bit_hex( info.id, p );
		
		p += 4;
		
		if ( has_name )
		{
			*p++ = '-';
			
			p = (char*) mempcpy( p, (char*) info.name + 1, info.name[0] );
		}
		
		*p++ = '.';
		
		p = (char*) mempcpy( p, type_quad.data(), type_size );
		
		return result;
	}
	
}

