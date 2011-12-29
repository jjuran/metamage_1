/*
	quad_name.cc
	------------
*/

#include "quad/quad_name.hh"

// Standard C/C++
#include <cctype>

// gear
#include "gear/hexidecimal.hh"
#include "gear/quad.hh"


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
	
	static bool char_is_initial_safe( char c )
	{
		return std::isalnum( c )  ||  c == '_'  ||  c & 0x80;
	}
	
	static bool char_is_safe( char c )
	{
		return char_is_initial_safe( c )  ||  c == '!'  ||  c == '#'  ||  c == '.';
	}
	
	plus::string make_quad_name( quad_t q )
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
				safe = char_is_safe( d );
				
				if ( !safe )  break;
			
			case 1:
				safe = char_is_safe( c );
				
				if ( !safe )  break;
			
			case 2:
				safe = char_is_safe( b );
				
				if ( !safe )  break;
			
			case 3:
				safe = char_is_initial_safe( a );
				break;
			
			case 4:
				safe = false;
		}
		
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

