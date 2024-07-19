/*
	integer.cc
	----------
*/

#include "vlib/types/integer.hh"

// iota
#include "iota/char_types.hh"

// bignum
#include "bignum/decimal.hh"

// vlib
#include "vlib/assign.hh"
#include "vlib/string-utils.hh"
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/fraction.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static
	const bignum::integer& nonzero( const bignum::integer& x )
	{
		if ( x.is_zero() )
		{
			THROW( "division by zero" );
		}
		
		return x;
	}
	
	static
	Value division( const bignum::integer& numer, const bignum::integer& denom )
	{
		bignum::fraction f( numer, nonzero( denom ) );
		
		if ( numer.is_zero() )
		{
			return Integer();
		}
		
		if ( f.denominator() == 1 )
		{
			return Integer( f.numerator() );
		}
		
		return Fraction( f );
	}
	
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
	bignum::integer decode_int( const plus::string& s )
	{
		if ( ! is_decimal( s.data(), s.size() ) )
		{
			THROW( "not a decimal integer" );
		}
		
		return bignum::decode_decimal( s );
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
				return Integer( !!(const Boolean&) v + 0 );
			
			case Value_byte:
				return Integer( static_cast< const Byte& >( v ).get() );
			
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
			case Op_typeof:
				return Type( integer_vtype );
			
			case Op_unary_plus:
				return v;
			
			case Op_unary_minus:
				return Integer( -v.number() );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		if ( op == Op_function  ||  op == Op_named_unary )
		{
			if ( b.is< String >() )
			{
				return String( str( Value( a, b ) ) );
			}
		}
		
		if ( b.is< Integer >() )
		{
			const bignum::integer& one = a.number();
			const bignum::integer& two = b.number();
			
			bignum::integer result;
			
			switch ( op )
			{
				case Op_add:       result = one + two;  break;
				case Op_subtract:  result = one - two;  break;
				case Op_multiply:  result = one * two;  break;
				
				case Op_divide:
					return division( one, two );
				
				case Op_DIV:     result = one / nonzero( two );  break;
				case Op_remain:  result = one % nonzero( two );  break;
				
				case Op_modulo:  result = modulo( one, nonzero( two ) );  break;
				
				case Op_empower:  result = raise_to_power( one, two );  break;
				
				default:
					goto non_arithmetic;
			}
			
			return Integer( result );
		}
		
		if ( b.is< Fraction >() )
		{
			Fraction a_ = bignum::fraction( a.number() );
			
			return fraction_dispatch.ops->binary( op, a_, b );
		}
		
	non_arithmetic:
		
		return Value();
	}
	
	static
	Value advance_op_handler( op_type op, const Target& target )
	{
		int step;
		
		switch ( op )
		{
			default:  // shouldn't happen
			
			case Op_preinc:  case Op_postinc:  step =  1;  break;
			case Op_predec:  case Op_postdec:  step = -1;  break;
		}
		
		const bignum::integer& i = target.addr->number();
		
		Integer result = i + step;
		
		switch ( op )
		{
			case Op_preinc:
			case Op_predec:
				assign( target, result );
				
				return result;
			
			case Op_postinc:
			case Op_postdec:
				{
					Value returned = *target.addr;
					
					assign( target, result );
					
					return returned;
				}
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value mutating_op_handler( op_type        op,
	                           const Target&  target,
	                           const Value&   x,
	                           const Value&   b )
	{
		bool dividing = true;
		
		switch ( op )
		{
			case Op_increase_by:
			case Op_decrease_by:
			case Op_multiply_by:
				dividing = false;
				// fall through
			case Op_divide_by:
			case Op_div_int_by:
			case Op_remain_by:
				break;
			
			default:
				return Value();  // unimplemented
		}
		
		if ( b.type() != V_int )
		{
			THROW( "numeric update requires numeric operands" );
		}
		
		const bignum::integer& k = b.number();
		
		if ( dividing  &&  k.is_zero() )
		{
			THROW( "division by zero in update" );
		}
		
		const bignum::integer& i = target.addr->number();
		
		bignum::integer j = i;
		
		if ( op == Op_divide_by )
		{
			Value result = division( j, k );
			
			assign( target, result );
			
			return result;
		}
		
		switch ( op )
		{
			case Op_increase_by:  j += k;  break;
			case Op_decrease_by:  j -= k;  break;
			case Op_multiply_by:  j *= k;  break;
			case Op_div_int_by:   j /= k;  break;
			case Op_remain_by:    j %= k;  break;
			
			default:
				break;
		}
		
		Integer result = j;
		
		assign( target, result );
		
		return result;
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
		&advance_op_handler,
		&mutating_op_handler,
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
		0,
		Type_pure,
	};
	
	void bad_cast_thrower::operator()() const
	{
		THROW( "bad integer cast" );
	}
	
}
