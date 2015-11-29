/*
	string-utils.cc
	---------------
*/

#include "vlib/string-utils.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// plus
#include "plus/decimal.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/proc_info.hh"
#include "vlib/quote.hh"
#include "vlib/type_info.hh"


namespace vlib
{
	
	using ::mempcpy;
	
	static inline
	char* mempcpy( char* p, const plus::string& s )
	{
		return (char*) mempcpy( p, s.data(), s.size() );
	}
	
	static inline
	char* mempcpy( char* p, const char* s )
	{
		return (char*) mempcpy( p, s, strlen( s ) );
	}
	
	
	static inline
	const char* reproduce_op( op_type op )
	{
		// This is the only one, for now.
		return " % ";
	}
	
	static inline
	bool use_quotes( stringification mode )
	{
		return mode != Stringified_to_print;
	}
	
	static inline
	bool use_parens( stringification mode )
	{
		return mode != Stringified_to_print;
	}
	
	static inline
	bool use_commas( stringification mode )
	{
		return mode != Stringified_to_print;
	}
	
	
	static
	size_t composite_length( const Value& value, stringification mode )
	{
		switch ( get_type( value ) )
		{
			case Value_empty_list:  // "()", ""
				return 2 * use_parens( mode );
			
			case Value_byte:
				if ( use_quotes( mode ) )
				{
					return quote_byte( value.number().clipped() ).size();
				}
				
				return 1;
			
			case Value_string:
				if ( use_quotes( mode ) )
				{
					return quote_string( get_str( value ) ).size();
				}
				
				return get_str( value ).size();
			
			case Value_function:
				return strlen( get_proc( value ).name );
			
			case Value_base_type:
				return strlen( value.typeinfo().name );
			
			case Value_boolean:
				return 4 + ! get_bool( value );  // "true" or "false"
			
			case Value_number:
				return decimal_length( get_int( value ) );
			
			case Value_pair:
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in composite_length()" );
				break;
		}
		
		Expr* expr = get_expr( value );
		
		size_t total = composite_length( expr->left, mode );
		
		total += 2 * use_parens( mode );  // 2 for parentheses
		
		if ( use_parens( mode )  &&  expr->op != Op_list )
		{
			total += strlen( reproduce_op( expr->op ) );
			
			total += composite_length( expr->right, Stringified_to_reproduce );
			
			return total;
		}
		
		total += 2 * use_commas( mode );  // 2 for first comma
		
		while ( Expr* next = get_expr( expr->right ) )
		{
			if ( next->op != Op_list )
			{
				break;
			}
			
			total += composite_length( next->left, mode );
			
			total += 2 * use_commas( mode );
			
			expr = next;
		}
		
		return total + composite_length( expr->right, mode );
	}
	
	static
	char* make_string( char* p, const Value& value, stringification mode )
	{
		switch ( get_type( value ) )
		{
			case Value_empty_list:  // ""
				if ( use_parens( mode ) )
				{
					p = (char*) mempcpy( p, STR_LEN( "()" ) );
				}
				
				return p;
			
			case Value_byte:
				if ( use_quotes( mode ) )
				{
					return mempcpy( p, quote_byte( value.number().clipped() ) );
				}
				
				*p++ = value.number().clipped();
				
				return p;
			
			case Value_string:
				if ( use_quotes( mode ) )
				{
					return mempcpy( p, quote_string( get_str( value ) ) );
				}
				
				return mempcpy( p, get_str( value ) );
			
			case Value_function:
				return mempcpy( p, get_proc( value ).name );
				
			case Value_base_type:
				return mempcpy( p, value.typeinfo().name );
			
			case Value_boolean:
				if ( ! get_bool( value ) )
				{
					return (char*) mempcpy( p, STR_LEN( "false" ) );
				}
				else
				{
					return (char*) mempcpy( p, STR_LEN( "true" ) );
				}
			
			case Value_number:
				return encode_decimal( p, get_int( value ) );
			
			case Value_pair:
				if ( ! use_parens( mode )  &&  is_function( value ) )
				{
					TYPE_ERROR( "stringification of compound function" );
				}
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in make_string()" );
				break;
		}
		
		Expr* expr = get_expr( value );
		
		if ( use_parens( mode ) )
		{
			*p++ = '(';
		}
		
		p = make_string( p, expr->left, mode );
		
		if ( use_parens( mode ) )
		{
			if ( expr->op != Op_list )
			{
				p = mempcpy( p, reproduce_op( expr->op ) );
				
				p = make_string( p, expr->right, Stringified_to_reproduce );
				
				*p++ = ')';
				
				return p;
			}
			
			p = (char*) mempcpy( p, STR_LEN( ", " ) );
		}
		
		while ( Expr* next = get_expr( expr->right ) )
		{
			if ( next->op != Op_list )
			{
				break;
			}
			
			p = make_string( p, next->left, mode );
			
			if ( use_commas( mode ) )
			{
				p = (char*) mempcpy( p, STR_LEN( ", " ) );
			}
			
			expr = next;
		}
		
		p = make_string( p, expr->right, mode );
		
		if ( use_parens( mode ) )
		{
			*p++ = ')';
		}
		
		return p;
	}
	
	plus::string make_string( const Value& value, stringification mode )
	{
		const size_t size = composite_length( value, mode );
		
		plus::string result;
		
		char* p = result.reset( size );
		
		make_string( p, value, mode );
		
		return result;
	}
	
	plus::string repeat( const plus::string& s, plus::string::size_type n )
	{
		if ( n > 65535 )
		{
			DOMAIN_ERROR( "excessively large string multiplier" );
		}
		
		const char*  data = s.data();
		const size_t size = s.size();
		
		if ( size > 65535 )
		{
			DOMAIN_ERROR( "excessively large string multiplicand" );
		}
		
		const size_t n_bytes = n * size;
		
		plus::string result;
		
		char* p = result.reset( n_bytes );
		
		while ( n-- > 0 )
		{
			p = (char*) mempcpy( p, data, size );
		}
		
		return result;
	}
	
	plus::string join( const plus::string& glue, const Value& v, unsigned n )
	{
		if ( n <= 1  ||  glue.empty() )
		{
			return make_string( v, Stringified_to_print );
		}
		
		plus::integer size = glue.size();
		
		size *= n - 1;
		
		size += composite_length( v, Stringified_to_print );
		
		if ( size > size_t( -1 ) )
		{
			DOMAIN_ERROR( "excessively large arguments to join()" );
		}
		
		plus::string result;
		
		char* p = result.reset( size.clipped() );
		
		const Value* next = &v;
		
		while ( Expr* expr = get_expr( *next ) )
		{
			p = make_string( p, expr->left, Stringified_to_print );
			
			p = mempcpy( p, glue );
			
			next = &expr->right;
		}
		
		make_string( p, *next, Stringified_to_print );
		
		return result;
	}
	
}
