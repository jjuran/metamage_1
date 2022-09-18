/*
	endianness.cc
	-------------
*/

#include "vlib/types/endianness.hh"

// vlib
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/endian.hh"


namespace vlib
{

static const char* endianness_names[] =
{
	"big-endian",
	"native-endian",
	"little-endian",
};

static
const char* endianness_str_data( const Value& v )
{
	endianness e = v.pod_cast< endianness >();
	
	return endianness_names[ e + 1 ];
}

static const stringify endianness_str =
{
	&endianness_str_data,
	NULL,
	NULL,
};

const stringifiers endianness_stringifiers =
{
	&endianness_str,
	// rep: ditto
	// bin: not defined
};

static
Value unary_op_handler( op_type op, const Value& v )
{
	switch ( op )
	{
		case Op_typeof:
			return v;
		
		default:
			break;
	}
	
	return NIL;
}

static
Value binary_op_handler( op_type op, const Value& a, const Value& b )
{
	const Endianness& e = static_cast< const Endianness& >( a );
	
	switch ( op )
	{
		case Op_function:
		case Op_named_unary:
			return endianized( e, b );
		
		default:
			break;
	}
	
	return NIL;
}

static const operators ops =
{
	&unary_op_handler,
	&binary_op_handler,
};

const dispatch endianness_dispatch =
{
	&endianness_stringifiers,
	0,  // NULL
	0,  // NULL
	&ops,
};

Endianness::Endianness( endianness e )
:
	Value( Value_other, &endianness_dispatch )
{
	pod_cast< endianness >() = e;
}

}
