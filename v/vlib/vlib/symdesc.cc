/*
	symdesc.cc
	----------
*/

#include "vlib/symdesc.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/list-utils.hh"
#include "vlib/stack.hh"
#include "vlib/symbol.hh"


namespace vlib
{
	
	Value make_metasymbol( const plus::string& name, int depth, int index )
	{
		ASSERT( depth >= 0 );
		ASSERT( index >= 0 );
		
		const Value symdesc = Value::symdesc( (depth << 16) | index );
		
		Value metasymbol( Symbol_const, name );
		
		metasymbol.sym()->assign( symdesc );
		
		return metasymbol;
	}
	
	static
	const Value& resolve_symdesc( const Value& symdesc, const Value& stack )
	{
		ASSERT( symdesc.type() == V_desc );
		
		const int desc = symdesc.desc();
		
		const int index = desc & 0xFFFF;
		const int delta = desc >> 16;
		
		const Value& frame = nth_frame( stack, delta );
		
		Expr* expr = frame.expr();
		
		ASSERT( expr );
		ASSERT( expr->op == Op_frame );
		
		const Value& symbols = expr->right;
		
		ASSERT( index < count( symbols ) );
		
		return get_nth( symbols, index );
	}
	
	const Value& resolve_symbol( const Value& v, const Value& stack )
	{
		if ( const Symbol* sym = v.sym() )
		{
			const Value& value = sym->get();
			
			if ( value.type() == V_desc )
			{
				return resolve_symdesc( value, stack );
			}
		}
		
		return v;
	}
	
}
