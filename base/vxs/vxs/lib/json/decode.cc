/*
	json/decode.cc
	--------------
*/

#include "vxs/lib/json/decode.hh"

// Standard C
#include <string.h>

// iota
#include "iota/char_types.hh"

// debug
#include "debug/assert.hh"

// vxo
#include "vxo/boolean.hh"
#include "vxo/container.hh"
#include "vxo/error.hh"
#include "vxo/ptrvec.hh"
#include "vxo/symbol.hh"

// vxs
#include "vxs/lib/json/decode_number.hh"
#include "vxs/lib/json/decode_string.hh"


#define STRLEN( s )  (sizeof s - 1)
#define STR_LEN( s )  "" s, (sizeof s - 1)

#define OutOfMemory Error( out_of_memory )


namespace vxo
{
namespace json
{

static
char skip_whitespace( const char*& p )
{
	char c;
	
	do
	{
		c = *p++;
	}
	while ( c == ' '  ||  c == '\t'  ||  c == '\n'  ||  c == '\r' );
	
	return c;
}

#define MATCH( p, s )  (strncmp( p, STR_LEN( s ) ) == 0  &&  (p += STRLEN( s )))

static
Box decode_symbol( const char*& p )
{
	if ( MATCH( p, "null" ) )  return Symbol( null );
	
	if ( MATCH( p, "true"  ) )  return Bool( true  );
	if ( MATCH( p, "false" ) )  return Bool( false );
	
	return Error( "undefined symbol" );
}

typedef Box (*decoder_proc)( const char*& p );

Box json_decode( const char* json )
{
	Box compound_result;
	
	Container* current = NULL;
	
	UPtrVec< Container > stack;
	
	int key_required = 0;
	
	const char* p = json;
	
	char c;
	
value_expected:
	
	c = skip_whitespace( p );
	
	decoder_proc decoder = NULL;
	
	if ( c == '"' )
	{
		decoder = &decode_string;
		
		goto decoding_atom;
	}
	
	if ( key_required < 0  &&  c != '}' )
	{
		return Error( "expected string or '}'" );
	}
	
	if ( iota::is_digit( c )  ||  c == '-' )
	{
		--p;
		
		decoder = &decode_number;
		
		goto decoding_atom;
	}
	
	if ( iota::is_alpha( c ) )
	{
		--p;
		
		decoder = &decode_symbol;
		
		goto decoding_atom;
	}
	
	if ( c == '['  ||  c == '{' )
	{
		box_type type = c == '[' ? Box_array : Box_map;
		
		char c0 = c;
		
		c = skip_whitespace( p );
		
		if ( c == ']'  ||  c == '}' )
		{
			if ( c - c0 != 2 )
			{
				return Error( c == ']' ? "expected '}', not ']'"
				                       : "expected ']', not '}'" );
			}
			
			Empty empty( type );
			
			if ( ! current )
			{
				return empty;
			}
			
			if ( ! current->push_back_nothrow( empty ) )
			{
				return OutOfMemory;
			}
			
			goto terminator_expected;
		}
		
		key_required = 0;
		
		if ( type == Box_map )
		{
			key_required = -1;
		}
		
		if ( ! stack.push_back_nothrow( current ) )
		{
			return OutOfMemory;
		}
		
		current = current ? (Container*) current->expand_by_nothrow( 1 )
		                  : (Container*) &compound_result;
		
		if ( ! current )
		{
			return OutOfMemory;
		}
		
		*current = Container( type, 2 );
		
		--p;
		goto value_expected;
	}
	
	return Error( "expected a value" );
	
decoding_atom:
	
	{
		Box atom = decoder( p );
		
		c = skip_whitespace( p );
		
		if ( ! atom  ||  (! current  &&  c == '\0') )
		{
			return atom;
		}
		
		if ( ! current )
		{
			return Error( "excess input after top-level atom" );
		}
		
		if ( ! current->push_back_nothrow( atom ) )
		{
			return OutOfMemory;
		}
	}
	
	--p;
	
terminator_expected:
	
	c = skip_whitespace( p );
	
	if ( c == '\0' )
	{
		return Error( "unclosed container" );
	}
	
	if ( (key_required < 0)  !=  (c == ':') )
	{
		const char* msg = key_required < 0 ? "expected ':' after key"
		                : key_required > 0 ? "expected ',' or '}', not ':'"
		                :                    "expected ',' or ']', not ':'";
		
		return Error( msg );
	}
	
	switch ( c )
	{
		case ']':
		case '}':
			ASSERT( ! stack.empty() );
			
			if ( c != (current->subtype_byte() == Box_map ? '}' : ']') )
			{
				return Error( c == ']' ? "expected '}', not ']'"
				                       : "expected ']', not '}'" );
			}
			
			current = stack.back();
			
			stack.pop_back();
			
			if ( stack.empty() )
			{
				if ( skip_whitespace( p ) )
				{
					return Error( "excess input after complete value" );
				}
				
				return compound_result;
			}
			
			key_required = current->subtype_byte() == Box_map;
			
			goto terminator_expected;
		
		case ',':
		case ':':
			key_required = -key_required;
			
			goto value_expected;
		
		default:
			break;
	}
	
	return Error( "expected a terminator" );
}

}
}
