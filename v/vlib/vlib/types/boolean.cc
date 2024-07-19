/*
	boolean.cc
	----------
*/

#include "vlib/types/boolean.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	const Boolean False( false );
	const Boolean True ( true  );
	
	
	static
	truth_test get_truth_test( const Value& v )
	{
		if ( const dispatch* methods = v.dispatch_methods() )
		{
			if ( const veritization* t = methods->to_boolean )
			{
				return t->verity;
			}
		}
		
		return NULL;
	}
	
	Value Boolean::coerce( const Value& v )
	{
		if ( const truth_test is_true = get_truth_test( v ) )
		{
			return Boolean( is_true( v ) );
		}
		
		switch ( v.type() )
		{
			case Value_boolean:
				return v;
			
			default:
				INTERNAL_ERROR( "invalid type in coerce_to_boolean()" );
			
			case Value_empty_list:
			case Value_empty_array:
				return Boolean();
			
			case Value_packed:
			case Value_string:
				return Boolean( ! v.string().empty() );
			
			case Value_pair:
			case Value_other:
				return True;
		}
	}
	
	static
	const char* boolean_str_data( const Value& v )
	{
		return (const Boolean&) v ? "true" : "false";
	}
	
	static const stringify boolean_str =
	{
		&boolean_str_data,
		NULL,
		NULL,
	};
	
	const stringifiers boolean_stringifiers =
	{
		&boolean_str,
		// rep: ditto
		// bin: not defined
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		const Boolean& b = (const Boolean&) v;
		
		switch ( op )
		{
			case Op_typeof:  return Type( boolean_vtype );
			
			case Op_unary_plus:   return Integer( +!!b );
			case Op_unary_minus:  return Integer( -!!b );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
	};
	
	const dispatch boolean_dispatch =
	{
		&boolean_stringifiers,
		NULL,
		NULL,
		&ops,
	};
	
	const type_info boolean_vtype =
	{
		"boolean",
		&assign_to< Boolean >,
		&Boolean::coerce,
		0,
		0,
		Type_pure,
	};
	
}
