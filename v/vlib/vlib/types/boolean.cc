/*
	boolean.cc
	----------
*/

#include "vlib/types/boolean.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/type_info.hh"
#include "vlib/types/null.hh"


namespace vlib
{
	
	Value Boolean::coerce( const Value& v )
	{
		switch ( v.type() )
		{
			case Value_boolean:
				return v;
			
			default:
				INTERNAL_ERROR( "invalid type in coerce_to_boolean()" );
			
			case Value_empty_list:
				return Boolean();
			
			case Value_byte:
			case Value_number:
				return ! v.number().is_zero();
			
			case Value_data:
			case Value_string:
				return ! v.string().empty();
			
			case Value_base_type:
				return &v.typeinfo() != &null_vtype;
			
			case Value_function:
			case Value_pair:
				if ( Expr* expr = v.expr() )
				{
					if ( expr->op == Op_array )
					{
						// Empty array is false.
						return ! is_empty( expr->right );
					}
				}
				
				return true;
		}
	}
	
	const type_info boolean_vtype =
	{
		"boolean",
		&assign_to< Boolean >,
		&Boolean::coerce,
		0,
	};
	
}
