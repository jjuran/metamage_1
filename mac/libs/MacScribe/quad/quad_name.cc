/*
	quad_name.cc
	------------
*/

#include "quad/quad_name.hh"

// gear
#include "gear/hexadecimal.hh"
#include "gear/quad.hh"

// MacScribe
#include "quad/safe.hh"


namespace MacScribe
{
	
	quad_t parse_quad_name( const char* name, unsigned size )
	{
		if ( size == 8 )
		{
			return gear::decode_32_bit_hex( name );
		}
		
		if ( size == 4 )
		{
			return gear::decode_quad( name );
		}
		
		char buffer[ 4 ] = { ' ', ' ', ' ', ' ' };
		
		switch ( size )
		{
			case 3:
				buffer[2] = name[2];
			
			case 2:
				buffer[1] = name[1];
			
			case 1:
				buffer[0] = name[0];
				break;
			
			default:
				throw invalid_quad_name();
		}
		
		return gear::decode_quad( buffer );
	}
	
	quad_t parse_quad_name( const plus::string& name )
	{
		return parse_quad_name( name.data(), name.size() );
	}
	
	plus::string make_quad_name( quad_t q )
	{
		const char a = q >> 24;
		const char b = q >> 16;
		const char c = q >>  8;
		const char d = q >>  0;
		
		bool safe = is_safe_quad( q );
		
		plus::string result;
		
		char* p = result.reset( safe ? 4 : 8 );
		
		if ( safe )
		{
			*p++ = a;
			*p++ = b;
			*p++ = c;
			*p++ = d;
		}
		else
		{
			gear::encode_32_bit_hex( q, p );
		}
		
		return result;
	}
	
}
