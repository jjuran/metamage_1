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
		{ "div", Op_divide },
		{ "mod", Op_modulo },
	};
	
	static
	const named_op_mapping* find( const plus::string& name )
	{
		const named_op_mapping* begin = named_ops;
		const named_op_mapping* end   = ARRAY_END( named_ops );
		
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
	
	op_type op_from_name( const plus::string& name )
	{
		if ( const named_op_mapping* it = find( name ) )
		{
			return it->op;
		}
		
		return Op_none;
	}
	
}
