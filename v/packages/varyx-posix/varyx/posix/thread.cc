/*
	thread.cc
	---------
*/

#include "varyx/posix/thread.hh"

// Standard C++
#include <new>

// vlib
#include "vlib/generic.hh"
#include "vlib/is_function.hh"
#include "vlib/list-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/refs.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/type.hh"

// varyx-posix
#include "varyx/posix/thread_state.hh"


namespace varyx
{
namespace posix
{
	
	using namespace vlib;
	
	
	Value Thread::coerce( const Value& v )
	{
		if ( ! is_function( v ) )
		{
			THROW( "thread argument must be a function" );
		}
		
		return Thread( v );
	}
	
	static
	const char* thread_str_data( const Value& v )
	{
		return "<thread>";
	}
	
	static const stringify thread_str =
	{
		&thread_str_data,
		NULL,
		NULL,
	};
	
	const stringifiers thread_stringifiers =
	{
		&thread_str,
		// rep: ditto
		// bin: not defined
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( thread_vtype );
			
			case Op_unary_deref:
				return **static_cast< const Thread& >( v ).get();
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
	};
	
	static
	void getref( const Value& v, put_ref put, void* param )
	{
		const Thread& thread = static_cast< const Thread& >( v );
		
		const thread_state& state = *thread.get();
		
		put( state.function(), param );
		put( state.result(),   param );
	}
	
	static const refs ref =
	{
		&getref,
	};
	
	const dispatch thread_dispatch =
	{
		&thread_stringifiers,
		NULL,
		NULL,
		&ops,
		NULL,
		&ref,
	};
	
	Thread::Thread( const Value& entry_point )
	:
		Value( sizeof (thread_state),
		       &generic_destructor< thread_state >,
		       Value_other,
		       &thread_dispatch )
	{
		new ((void*) pointer()) thread_state( entry_point );
	}
	
	static
	Value v_join( const Value& v )
	{
		const Thread& thread = static_cast< const Thread& >( v );
		
		thread.get()->join();
		
		return Value_nothing;
	}
	
	static
	Value v_cancel( const Value& v )
	{
		const Thread& thread = static_cast< const Thread& >( v );
		
		thread.get()->cancel();
		
		return Value_nothing;
	}
	
	static const Type thread = thread_vtype;
	
	static const proc_info proc_thread_join = { "join", &v_join, &thread };
	static const proc_info proc_thread_cancel = { "cancel", &v_cancel, &thread };
	
	static
	Value thread_member( const Value&         obj,
	                     const plus::string&  member )
	{
		if ( member == "join" )
		{
			return bind_args( Proc( proc_thread_join ), obj );
		}
		
		if ( member == "cancel" )
		{
			return bind_args( Proc( proc_thread_cancel ), obj );
		}
		
		THROW( "nonexistent thread member" );
		
		return Value_nothing;
	}
	
	const type_info thread_vtype =
	{
		"thread",
		&assign_to< Thread >,
		&Thread::coerce,
		&thread_member,
	};
	
}
}
