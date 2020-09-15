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
#include "vlib/types/symdesc.hh"
#include "vlib/types/term.hh"


namespace vlib
{
	
	Value make_metasymbol( const plus::string& name, int depth, int index )
	{
		ASSERT( depth >= 0 );
		ASSERT( index >= 0 );
		
		const SymDesc symdesc( depth, index );
		
		Constant metasymbol( name );
		
		metasymbol.sym()->assign( symdesc );
		
		return metasymbol;
	}
	
	static
	const Value& resolve_symdesc( const SymDesc& symdesc, const Value& stack )
	{
		const int index = symdesc.index();
		const int delta = symdesc.depth();
		
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
				return resolve_symdesc( (const SymDesc&) value, stack );
			}
		}
		
		return v;
	}
	
}
