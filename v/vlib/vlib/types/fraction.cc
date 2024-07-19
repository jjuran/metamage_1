/*
	fraction.cc
	-----------
*/

#include "vlib/types/fraction.hh"

// iota
#include "iota/char_types.hh"

// bignum
#include "bignum/decimal.hh"
#include "bignum/fraction.hh"

// vlib
#include "vlib/assign.hh"
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static
	bignum::fraction make_fraction( const Value& v )
	{
		Expr* expr = v.expr();
		
		const bignum::integer& numer = expr->left.number();
		const bignum::integer& denom = expr->right.number();
		
		return bignum::fraction( numer, denom );
	}
	
	static
	const bignum::fraction& nonzero( const bignum::fraction& x )
	{
		if ( x.is_zero() )
		{
			THROW( "division by zero" );
		}
		
		return x;
	}
	
	Fraction::Fraction( const bignum::fraction& f )
	:
		Value( Integer( f.numerator() ), Op_divide, Integer( f.denominator() ), &fraction_dispatch )
	{
	}
	
	Value Fraction::coerce( const Value& v )
	{
		if ( const Integer* numer = v.is< Integer >() )
		{
			return Fraction( bignum::fraction( numer->number() ) );
		}
		
		switch ( v.type() )
		{
			default:
				THROW( "fraction conversion not defined for type" );
			
			case Value_empty_list:
				return Fraction( bignum::fraction() );
		}
	}
	
	static
	size_t fraction_str_size( const Value& v )
	{
		const Fraction& f = static_cast< const Fraction& >( v );
		
		return 1 + decimal_length( f.numerator  ().get() )
		         + decimal_length( f.denominator().get() );
	}
	
	static
	char* fraction_str_copy( char* p, const Value& v )
	{
		const Fraction& f = static_cast< const Fraction& >( v );
		
		p = encode_decimal( p, f.numerator().get() );
		
		*p++ = '/';
		
		p = encode_decimal( p, f.denominator().get() );
		
		return p;
	}
	
	static const stringify fraction_str =
	{
		NULL,
		&fraction_str_size,
		&fraction_str_copy,
	};
	
	static
	size_t fraction_rep_size( const Value& v )
	{
		return 2 + fraction_str_size( v );
	}
	
	static
	char* fraction_rep_copy( char* p, const Value& v )
	{
		*p++ = '(';
		
		p = fraction_str_copy( p, v );
		
		*p++ = ')';
		
		return p;
	}
	
	static const stringify fraction_rep =
	{
		NULL,
		&fraction_rep_size,
		&fraction_rep_copy,
	};
	
	static const stringifiers fraction_stringifiers =
	{
		&fraction_str,
		&fraction_rep,
		// Fraction bin is unimplemented
	};
	
	static
	bool fraction_verity( const Value& v )
	{
		return ! v.expr()->left.number().is_zero();
	}
	
	static const veritization fraction_veritization =
	{
		&fraction_verity,
	};
	
	static
	cmp_t fraction_order( const Value& a, const Value& b )
	{
		bignum::fraction one = make_fraction( a );
		bignum::fraction two = make_fraction( b );
		
		return compare( one, two );
	}
	
	static const comparison fraction_comparison =
	{
		&fraction_order,
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( fraction_vtype );
			
			case Op_unary_plus:
				return v;
			
			case Op_unary_minus:
				{
					const Expr* expr = v.expr();
					
					const bignum::integer& numer = expr->left.number();
					const bignum::integer& denom = expr->right.number();
					
					return Fraction( bignum::fraction( -numer, denom ) );
				}
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const bignum::integer integer_one( 1 );
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		using bignum::fraction;
		using bignum::integer;
		
		const integer& a_numer = a.expr()->left.number();
		const integer& a_denom = a.expr()->right.number();
		
		const integer* numer;
		const integer* denom;
		
		if ( b.is< Integer >() )
		{
			numer = &b.number();
			denom = &integer_one;
		}
		else if ( b.is< Fraction >() )
		{
			numer = &b.expr()->left.number();
			denom = &b.expr()->right.number();
		}
		else
		{
			return Value();
		}
		
		const integer& b_numer = *numer;
		const integer& b_denom = *denom;
		
		fraction one( a_numer, a_denom );
		fraction two( b_numer, b_denom );
		
		fraction result;
		
		switch ( op )
		{
			case Op_add:       result = one + two;  break;
			case Op_subtract:  result = one - two;  break;
			case Op_multiply:  result = one * two;  break;
			
			case Op_divide:  result = one / nonzero( two );  break;
			
			case Op_empower:
				if ( b_denom != 1 )
				{
					goto non_arithmetic;
				}
				
				result = raise_to_power( one, b_numer );
				break;
			
			default:
				goto non_arithmetic;
		}
		
		if ( result.denominator() == 1 )
		{
			return Integer( result.numerator() );
		}
		
		return Fraction( result );
		
	non_arithmetic:
		
		return Value();
	}
	
	static
	Value mutating_op_handler( op_type        op,
	                           const Target&  target,
	                           const Value&   x,
	                           const Value&   b )
	{
		switch ( op )
		{
			case Op_increase_by:  op = Op_add;       break;
			case Op_decrease_by:  op = Op_subtract;  break;
			case Op_multiply_by:  op = Op_multiply;  break;
			case Op_divide_by:    op = Op_divide;    break;
			
			default:
				return Value();  // unimplemented
		}
		
		const Value result = binary_op_handler( op, *target.addr, b );
		
		if ( result )
		{
			assign( target, result );
		}
		
		return result;
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
		NULL,
		&mutating_op_handler,
	};
	
	const dispatch fraction_dispatch =
	{
		&fraction_stringifiers,
		&fraction_veritization,
		&fraction_comparison,
		&ops,
	};
	
	const type_info fraction_vtype =
	{
		"fraction",
		&assign_to< Fraction >,
		&Fraction::coerce,
		0,
		0,
		Type_pure,
	};
	
}
