/*
	iterator.cc
	-----------
*/

#include "vlib/types/iterator.hh"

// vlib
#include "vlib/list-utils.hh"
#include "vlib/targets.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static
	const char* iterator_str_data( const Value& v )
	{
		return "<iterator>";
	}
	
	static const stringify iterator_str =
	{
		&iterator_str_data,
		NULL,
		NULL,
	};
	
	static const stringifiers iterator_stringifiers =
	{
		&iterator_str,
	};
	
	static
	bool iterator_truth( const Value& v )
	{
		// Check if the iterator is empty.
		return ! is_empty_list( v.expr()->right );
	}
	
	static const veritization iterator_veritization =
	{
		&iterator_truth,
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( iterator_vtype );
			
			case Op_unary_deref:
				return first( v.expr()->right );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	const Value& increment( Value& v )
	{
		Value& list = v.unshare().expr()->right;
		
		list = rest( list );
		
		return v;
	}
	
	static
	Value advance_op_handler( op_type op, const Target& target )
	{
		Value& v = *target.addr;
		
		switch ( op )
		{
			case Op_preinc:
				return increment( v );
			
			case Op_postinc:
				{
					Value result = v;
					
					increment( v );
					
					return result;
				}
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		NULL,
		&advance_op_handler,
	};
	
	const dispatch iterator_dispatch =
	{
		&iterator_stringifiers,
		&iterator_veritization,
		NULL,
		&ops,
	};
	
	const type_info iterator_vtype =
	{
		"iterator",
		&assign_to< Iterator >,
		NULL,
		NULL,
		NULL,
		Type_pure,
	};
	
}
