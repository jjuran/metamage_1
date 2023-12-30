/*
	namespace.cc
	------------
*/

#include "vlib/types/namespace.hh"

// vlib
#include "vlib/namespace_info.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/types/builtin.hh"
#include "vlib/types/string.hh"


namespace vlib
{
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		const Namespace& space = static_cast< const Namespace& >( a );
		
		switch ( op )
		{
			case Op_member:
				if ( b.type() != V_str )
				{
					THROW( "non-string namespace member name" );
				}
				
				if ( b.string() == "name" )
				{
					return String( space->name );
				}
				
				return space->getter( b.string() );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		0,  // NULL
		&binary_op_handler,
	};
	
	const dispatch namespace_dispatch =
	{
		&builtin_stringifiers,
		0,  // NULL
		&builtin_comparison,
		&ops,
	};
	
}
