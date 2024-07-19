/*
	record.cc
	---------
*/

#include "vlib/types/record.hh"

// vlib
#include "vlib/table-utils.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Type( record_vtype );
			
			case Op_unary_deref:
				return v.expr()->right;
			
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
			case Op_member:
				return keyed_subscript( Value( Op_array, a.expr()->right ), b );
			
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
	
	const dispatch record_dispatch =
	{
		0,  // NULL
		0,  // NULL
		0,  // NULL
		&ops,
	};
	
	Value Record::coerce( const Value& fields )
	{
		list_iterator args( fields );
		
		if ( ! args )
		{
			THROW( "empty records not allowed" );
		}
		
		while ( args )
		{
			const Value& v = args.use();
			
			if ( Expr* expr = v.expr() )
			{
				if ( expr->op == Op_mapping  &&  expr->left.type() == V_str )
				{
					continue;
				}
			}
			
			THROW( "invalid record field" );
		}
		
		return Record( fields );
	}
	
	Record::Record( const Value& fields )
	:
		Value( dummy_operand, Op_record, fields, &record_dispatch )
	{
	}
	
	const type_info record_vtype =
	{
		"record",
		&assign_to< Record >,
		&Record::coerce,
		0,
		0,  // member lookup handled in binary_op_handler()
		Type_pure,
	};
	
}
