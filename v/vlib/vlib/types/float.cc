/*
	float.cc
	--------
*/

#include "vlib/types/float.hh"

// Standard C
#include <math.h>
#include <stdio.h>
#include <string.h>

// iota
#include "iota/endian.hh"

// math
#include "math/integer.hh"

// bignum
#include "bignum/float.hh"

// vlib
#include "vlib/assign.hh"
#include "vlib/symbol.hh"
#include "vlib/targets.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/fraction.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/type.hh"


namespace vlib
{

using math::integer::cmp_t;

static inline
cmp_t compare( float_type a, float_type b )
{
	return a < b ? -1 :
	       b < a ?  1 :
	                0;
}

static inline
float_type to_float( const bignum::integer& i )
{
	return bignum::to_float< float_type >( i );
}

Float::Float( float_type f )
:
	Value( Value_other, &float_dispatch )
{
	pod_cast< float_type >() = f;
}

static
Value assign_to_float( const Value& v )
{
	if ( Float::test( v ) )
	{
		return v;
	}
	
	/*
		Allow integers and fractions to be assigned to
		`float` variables and parameters implicitly.
	*/
	
	if ( const Integer* i = v.is< Integer >() )
	{
		return Float( to_float( i->number() ) );
	}
	
	if ( const Fraction* f = v.is< Fraction >() )
	{
		const float_type numer = to_float( v.expr()->left .number() );
		const float_type denom = to_float( v.expr()->right.number() );
		
		return Float( numer / denom );
	}
	
	return Value_NIL;
}

Value Float::coerce( const Value& v )
{
	if ( Value assigned = assign_to_float( v ) )
	{
		return assigned;
	}
	
	switch ( v.type() )
	{
		default:
			THROW( "float conversion not defined for type" );
		
		case Value_empty_list:
			return Float( 0.0 );
	}
}

/*
	The maximum length of a double printed to six decimal places
	appears to be 316.  Add one for the trailing NUL and round up.
*/

const size_t float_output_length = 320;

static inline
void inscribe_float( char* buffer, size_t size, float_type x )
{
	snprintf( buffer, size, "%f", x );
}

static
size_t float_str_size( const Value& v )
{
	char buffer[ float_output_length ];
	
	memset( buffer, '\0', sizeof buffer );
	
	const float_type f = static_cast< const Float& >( v ).get();
	
	inscribe_float( buffer, sizeof buffer, f );
	
	return strlen( buffer );
}

static
char* float_str_copy( char* p, const Value& v )
{
	const float_type f = static_cast< const Float& >( v ).get();
	
	inscribe_float( p, float_output_length, f );
	
	return strchr( p, '\0' );
}

static const stringify float_str =
{
	NULL,
	&float_str_size,
	&float_str_copy,
};

static const stringifiers float_stringifiers =
{
	&float_str,
	// For rep, fall back to str
	// TODO:  float bin is unimplemented
};

static
bool float_verity( const Value& v )
{
	float_type x = static_cast< const Float& >( v );
	
	return x != 0.0;
}

static const veritization float_veritization =
{
	&float_verity,
};

static
cmp_t float_order( const Value& a, const Value& b )
{
	float_type one = static_cast< const Float& >( a );
	float_type two = static_cast< const Float& >( b );
	
	if  ( isnan( one )  ||  isnan( two ) )
	{
		THROW( "NaN can't be ordered" );
	}
	
	return compare( one, two );
}

static
bool float_equal( const Value& a, const Value& b )
{
	float_type one = static_cast< const Float& >( a );
	float_type two = static_cast< const Float& >( b );
	
	if  ( isnan( one )  &&  isnan( two ) )
	{
		return true;
	}
	
	return one == two;
}

static const comparison float_comparison =
{
	&float_order,
	&float_equal,
};

static
Value unary_op_handler( op_type op, const Value& v )
{
	switch ( op )
	{
		case Op_typeof:
			return Type( float_vtype );
		
		case Op_unary_plus:
			return v;
		
		case Op_unary_minus:
			return Float( -static_cast< const Float& >( v ).get() );
		
		default:
			break;
	}
	
	return Value();
}

static
void binary_math( float_type& a, op_type op, const Value& x )
{
	float_type b = x.to< Float >();
	
	switch ( op )
	{
		case Op_add:       a += b;  break;
		case Op_subtract:  a -= b;  break;
		case Op_multiply:  a *= b;  break;
		case Op_divide:    a /= b;  break;
		
		case Op_empower:   a = pow( a, b );  break;
		
		default:
			// not reached
			break;
	}
}

static
Value binary_op_handler( op_type op, const Value& a, const Value& b )
{
	switch ( op )
	{
		case Op_add:
		case Op_subtract:
		case Op_multiply:
		case Op_divide:
		case Op_empower:
			break;
		
		default:
			return Value();  // unimplemented
	}
	
	float_type one = static_cast< const Float& >( a ).get();
	
	binary_math( one, op, b );
	
	return Float( one );
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
	
	float_type& one = static_cast< Float& >( *target.addr ).get();
	
	binary_math( one, op, b );
	
	return *target.addr;
}

static const operators ops =
{
	&unary_op_handler,
	&binary_op_handler,
	NULL,
	&mutating_op_handler,
};

const dispatch float_dispatch =
{
	&float_stringifiers,
	&float_veritization,
	&float_comparison,
	&ops,
};

const type_info float_vtype =
{
	"float",
	&assign_to_float,
	&Float::coerce,
	0,
	0,
	0,
};

}
