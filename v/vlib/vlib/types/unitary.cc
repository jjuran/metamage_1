/*
	unitary.cc
	----------
*/

#include "vlib/types/unitary.hh"

// vlib
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/types/bareword.hh"


#ifndef NULL
#define NULL  0
#endif


namespace vlib
{
	
	static
	bool unitary_equal( const Value& a, const Value& b )
	{
		return a.string() == b.string();
	}
	
	const comparison unitary_comparison =
	{
		NULL,
		&unitary_equal,
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return v;
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
	};
	
	const dispatch unitary_dispatch =
	{
		&bareword_stringifiers,
		NULL,
		&unitary_comparison,
		&ops,
	};
	
	Unitary::Unitary( const plus::string& name )
	:
		Value( (const vu_string&) name, V_str, &unitary_dispatch )
	{
		set_evaluated();
	}
	
}
