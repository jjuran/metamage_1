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
#include "vlib/dispatch/verity.hh"
#include "vlib/types/builtin.hh"


namespace vlib
{
	
	static
	bool always_true( const Value& v )
	{
		return true;
	}
	
	static const veritization namespace_veritization =
	{
		&always_true,
	};
	
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
		&namespace_veritization,
		&builtin_comparison,
		&ops,
	};
	
}
