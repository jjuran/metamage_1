/*
	named_ops.cc
	------------
*/

#include "vlib/named_ops.hh"


#define ARRAY_LEN( a ) (sizeof (a) / sizeof (a)[0])
#define ARRAY_END( a ) ((a) + ARRAY_LEN(a))


namespace vlib
{
	
	struct named_op_mapping
	{
		const char*  name;
		op_type      op;
	};
	
	static
	const named_op_mapping named_ops[] =
	{
		{ "div",  Op_divide },
		{ "do",   Op_do_2   },
		{ "else", Op_else   },
		{ "isa",  Op_isa    },
		{ "map",  Op_map    },
		{ "mod",  Op_modulo },
		{ "then", Op_then   },
	};
	
	static
	const named_op_mapping named_unary_ops[] =
	{
		{ "const", Op_const },
		{ "if",    Op_if    },
		{ "var",   Op_var   },
		{ "while", Op_while },
	};
	
	static
	const named_op_mapping* find( const named_op_mapping*  begin,
	                              const named_op_mapping*  end,
	                              const plus::string&      name )
	{
		const named_op_mapping* it = begin;
		
		while ( it < end )
		{
			if ( it->name == name )
			{
				return it;
			}
			
			++it;
		}
		
		return 0;  // NULL
	}
	
	#define FIND( array, key )  find( array, ARRAY_END( array ), key )
	
	op_type op_from_name( const plus::string& name )
	{
		if ( const named_op_mapping* it = FIND( named_ops, name ) )
		{
			return it->op;
		}
		
		return Op_none;
	}
	
	op_type unary_op_from_name( const plus::string& name )
	{
		if ( const named_op_mapping* it = FIND( named_unary_ops, name ) )
		{
			return it->op;
		}
		
		return Op_none;
	}
	
}
