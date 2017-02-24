/*
	integer.cc
	----------
*/

#include "vlib/types/integer.hh"

// iota
#include "iota/char_types.hh"

// plus
#include "plus/decimal.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"


namespace vlib
{
	
	static
	bool is_decimal( const char* p, plus::string::size_type n )
	{
		if ( n == 0 )
		{
			return false;
		}
		
		if ( *p == '-'  ||  *p == '+' )
		{
			++p;
			
			if ( --n == 0 )
			{
				return false;
			}
		}
		
		do
		{
			if ( ! iota::is_digit( *p++ ) )
			{
				return false;
			}
		}
		while ( --n );
		
		return true;
	}
	
	static
	plus::integer decode_int( const plus::string& s )
	{
		if ( ! is_decimal( s.data(), s.size() ) )
		{
			THROW( "not a decimal integer" );
		}
		
		return plus::decode_decimal( s );
	}
	
	Value Integer::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_number:
				return v;
			
			default:
				THROW( "integer conversion not defined for type" );
			
			case Value_empty_list:
				return Integer();
			
			case Value_boolean:
				return Integer( v.boolean() + 0 );
			
			case Value_byte:
				return Integer( v.number() );
			
			case Value_string:
				return Integer( decode_int( v.string() ) );
		}
	}
	
	static
	size_t integer_str_size( const Value& v )
	{
		return decimal_length( v.number() );
	}
	
	static
	char* integer_str_copy( char* p, const Value& v )
	{
		return encode_decimal( p, v.number() );
	}
	
	static const stringify integer_str =
	{
		NULL,
		&integer_str_size,
		&integer_str_copy,
	};
	
	static const stringifiers integer_stringifiers =
	{
		&integer_str,
		// For rep, fall back to str
		// Integer bin is unimplemented
	};
	
	static
	bool integer_verity( const Value& v )
	{
		return ! v.number().is_zero();
	}
	
	static const veritization integer_veritization =
	{
		&integer_verity,
	};
	
	static
	cmp_t integer_order( const Value& a, const Value& b )
	{
		return compare( a.number(), b.number() );
	}
	
	static const comparison integer_comparison =
	{
		&integer_order,
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_unary_plus:
				return v;
			
			case Op_unary_minus:
				return Integer( -v.number() );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
	};
	
	const dispatch integer_dispatch =
	{
		&integer_stringifiers,
		&integer_veritization,
		&integer_comparison,
		&ops,
	};
	
	const type_info integer_vtype =
	{
		"integer",
		&assign_to< Integer >,
		&Integer::coerce,
		0,
	};
	
}
