/*
	metatype.cc
	-----------
*/

#include "varyx/channel/metatype.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/types.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/typing.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// varyx-channel
#include "varyx/channel/channel.hh"


namespace varyx
{
namespace channel
{
	
	using namespace vlib;
	
	
	static
	const char* channelmetatype_str_data( const Value& v )
	{
		return "channel";
	}
	
	static const stringify channelmetatype_str =
	{
		&channelmetatype_str_data,
		NULL,
		NULL,
	};
	
	const stringifiers channelmetatype_stringifiers =
	{
		&channelmetatype_str,
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
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				if ( is_empty_list( b ) )
				{
					return Channel();
				}
				THROW( "invalid channel argument" );
			
			case Op_subscript:
				THROW( "channel subtypes are TBD" );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
	};
	
	static
	bool typecheck( const Value& type, const Value& v )
	{
		return Channel::test( v );
	}
	
	static const typing type =
	{
		&typecheck,
	};
	
	const dispatch channelmetatype_dispatch =
	{
		&channelmetatype_stringifiers,
		NULL,
		NULL,
		&ops,
		&type,
	};
	
}
}
