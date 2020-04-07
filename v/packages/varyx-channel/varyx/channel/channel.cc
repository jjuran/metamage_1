/*
	channel.cc
	----------
*/

#include "varyx/channel/channel.hh"

// Standard C++
#include <new>

// vlib
#include "vlib/generic.hh"
#include "vlib/list-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/proc.hh"

// varyx-channel
#include "varyx/channel/metatype.hh"
#include "varyx/channel/state.hh"


namespace varyx
{
namespace channel
{
	
	using namespace vlib;
	
	
	static
	const char* channel_str_data( const Value& v )
	{
		return "<channel>";
	}
	
	static const stringify channel_str =
	{
		&channel_str_data,
		NULL,
		NULL,
	};
	
	const stringifiers channel_stringifiers =
	{
		&channel_str,
		// rep: ditto
		// bin: not defined
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		const Channel& channel = static_cast< const Channel& >( v );
		
		switch ( op )
		{
			case Op_typeof:
				return Channel_Metatype();
			
			case Op_recv:
				return channel.get()->recv();
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value v_close( const Value& v )
	{
		const Channel& channel = static_cast< const Channel& >( v );
		
		channel.get()->close();
		
		return nothing;
	}
	
	static
	Value v_recv( const Value& v )
	{
		const Channel& channel = static_cast< const Channel& >( v );
		
		return channel.get()->recv();
	}
	
	static const proc_info proc_close = { "close", &v_close, NULL };
	static const proc_info proc_recv  = { "recv",  &v_recv,  NULL };
	
	static
	Value channel_member( const Value& obj, const plus::string& name )
	{
		if ( name == "recv" )
		{
			return bind_args( Proc( proc_recv ), obj );
		}
		
		if ( name == "close" )
		{
			return bind_args( Proc( proc_close ), obj );
		}
		
		THROW( "nonexistent channel member" );
		
		return Value();
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		const Channel& channel = static_cast< const Channel& >( a );
		
		switch ( op )
		{
			case Op_send:
				if ( ! channel.get()->send( b ) )
				{
					THROW( "send to closed channel" );
				}
				
				return a;
			
			case Op_member:
				if ( b.type() != V_str )
				{
					THROW( "non-string channel member name" );
				}
				
				return channel_member( a, b.string() );
			
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
	
	const dispatch channel_dispatch =
	{
		&channel_stringifiers,
		NULL,
		NULL,
		&ops,
	};
	
	Channel::Channel()
	:
		Value( sizeof (channel_state),
		       &generic_destructor< channel_state >,
		       Value_other,
		       &channel_dispatch )
	{
		new ((void*) pointer()) channel_state();
	}
	
}
}
