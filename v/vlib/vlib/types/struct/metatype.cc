/*
	metatype.cc
	-----------
*/

#include "vlib/types/struct/metatype.hh"

// vlib
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/typing.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// vx
#include "vlib/types/struct/struct.hh"
#include "vlib/types/struct/type.hh"


namespace vlib
{

static
const char* structmetatype_str_data( const Value& v )
{
	return "struct";
}

static const stringify structmetatype_str =
{
	&structmetatype_str_data,
	NULL,
	NULL,
};

const stringifiers structmetatype_stringifiers =
{
	&structmetatype_str,
	// rep: ditto
	// bin: not defined
};

static
Value unary_op_handler( op_type op, const Value& v )
{
	switch ( op )
	{
		case Op_typeof:
			return Type( type_vtype );
		
		default:
			break;
	}
	
	return NIL;
}

static
Value binary_op_handler( op_type op, const Value& a, const Value& b )
{
	switch ( op )
	{
		case Op_function:
		case Op_named_unary:
			return Struct_Type( b, String() );
		
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

static
bool typecheck( const Value& type, const Value& v )
{
	return v.is< Struct >();
}

static const typing type =
{
	&typecheck,
	NULL,
	Type_pure,
};

const dispatch structmetatype_dispatch =
{
	&structmetatype_stringifiers,
	NULL,
	NULL,
	&ops,
	&type,
};

}
