/*
	string-utils.cc
	---------------
*/

#include "vlib/string-utils.hh"

// more-libc
#include "more/string.h"

// plus
#include "plus/decimal.hh"

// vlib
#include "vlib/error.hh"


namespace vlib
{
	
	using ::mempcpy;
	
	static inline
	char* mempcpy( char* p, const plus::string& s )
	{
		return (char*) mempcpy( p, s.data(), s.size() );
	}
	
	
	static
	plus::string::size_type composite_length( const Value& value )
	{
		switch ( value.type )
		{
			case Value_empty_list:  // ""
			case Value_string:
			case Value_function:
				return value.string.size();
			
			case Value_boolean:
				return 4 + value.number.is_zero();  // "true" or "false"
			
			case Value_number:
				return decimal_length( value.number );
			
			case Value_pair:
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in composite_length()" );
				break;
		}
		
		Expr* expr = value.expr.get();
		
		plus::string::size_type total = composite_length( expr->left );
		
		while ( Expr* next = expr->right.expr.get() )
		{
			total += composite_length( next->left );
			
			expr = next;
		}
		
		return total + composite_length( expr->right );
	}
	
	static
	char* make_string( char* p, const Value& value )
	{
		switch ( value.type )
		{
			case Value_empty_list:  // ""
			case Value_string:
			case Value_function:
				return mempcpy( p, value.string );
			
			case Value_boolean:
				if ( value.number.is_zero() )
				{
					return (char*) mempcpy( p, STR_LEN( "false" ) );
				}
				else
				{
					return (char*) mempcpy( p, STR_LEN( "true" ) );
				}
			
			case Value_number:
				return encode_decimal( p, value.number );
			
			case Value_pair:
				if ( is_function( value ) )
				{
					TYPE_ERROR( "stringification of compound function" );
				}
				break;
			
			default:
				INTERNAL_ERROR( "unsupported type in make_string()" );
				break;
		}
		
		Expr* expr = value.expr.get();
		
		p = make_string( p, expr->left );
		
		while ( Expr* next = expr->right.expr.get() )
		{
			p = make_string( p, next->left );
			
			expr = next;
		}
		
		return make_string( p, expr->right );
	}
	
	plus::string make_string( const Value& value )
	{
		const plus::string::size_type size = composite_length( value );
		
		plus::string result;
		
		char* p = result.reset( size );
		
		make_string( p, value );
		
		return result;
	}
	
	plus::string repeat( const plus::string& s, plus::string::size_type n )
	{
		typedef plus::string::size_type size_t;
		
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
			return make_string( v );
		}
		
		plus::integer size = glue.size();
		
		size *= n - 1;
		
		size += composite_length( v );
		
		if ( size > size_t( -1 ) )
		{
			DOMAIN_ERROR( "excessively large arguments to join()" );
		}
		
		plus::string result;
		
		char* p = result.reset( size.clipped() );
		
		const Value* next = &v;
		
		while ( Expr* expr = next->expr.get() )
		{
			p = make_string( p, expr->left );
			
			p = mempcpy( p, glue );
			
			next = &expr->right;
		}
		
		make_string( p, *next );
		
		return result;
	}
	
}
