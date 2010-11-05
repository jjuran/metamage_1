/*
	quad_name.cc
	------------
*/

#include "Genie/FS/quad_name.hh"

// Standard C
#include <ctype.h>

// iota
#include "iota/hexidecimal.hh"
#include "iota/quad.hh"

// plus
#include "plus/hexidecimal.hh"
#include "plus/var_string.hh"


namespace Genie
{
	
	quad_t parse_quad_name( const char* name, unsigned size )
	{
		if ( size == 8 )
		{
			return iota::decode_32_bit_hex( name );
		}
		
		if ( size == 4 )
		{
			return iota::decode_quad( name );
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
		
		return iota::decode_quad( buffer );
	}
	
	quad_t parse_quad_name( const plus::string& name )
	{
		return parse_quad_name( name.data(), name.size() );
	}
	
	static inline bool char_is_safe( char c )
	{
		return isalnum( c )  ||  c == '_';
	}
	
	void inscribe_quad_name( plus::var_string& out, quad_t q )
	{
		const char a = q >> 24;
		const char b = q >> 16 & 0xFF;
		const char c = q >>  8 & 0xFF;
		const char d = q >>  0 & 0xFF;
		
		int n_spaces = d != ' ' ? 0
		             : c != ' ' ? 1
		             : b != ' ' ? 2
		             : a != ' ' ? 3
		             :            4;
		
		bool safe = true;
		
		switch ( n_spaces )
		{
			case 0:
				safe = char_is_safe( d )  ||  d == '#';  // special exemption
				
				if ( !safe )  break;
			
			case 1:
				safe = char_is_safe( c );
				
				if ( !safe )  break;
			
			case 2:
				safe = char_is_safe( b );
				
				if ( !safe )  break;
			
			case 3:
				safe = char_is_safe( a );
				break;
			
			case 4:
				safe = false;
		}
		
		if ( safe )
		{
			out += a;
			out += b;
			out += c;
			out += d;
		}
		else
		{
			plus::encode_32_bit_hex( out, q );
		}
	}
	
}

