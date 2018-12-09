/*
	term.cc
	-------
*/

#include "vlib/types/term.hh"

// Standard C++
#include <new>

// vlib
#include "vlib/generic.hh"
#include "vlib/symbol.hh"  // codependent
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/refs.hh"
#include "vlib/dispatch/stringify.hh"


namespace vlib
{
	
	static
	const char* term_str_data( const Value& v )
	{
		return v.sym()->name().c_str();
	}
	
	static
	size_t term_str_size( const Value& v )
	{
		return v.sym()->name().size();
	}
	
	static const stringify term_str =
	{
		&term_str_data,
		&term_str_size,
		NULL,
	};
	
	static const stringifiers term_stringifiers =
	{
		&term_str,
		// rep: reuse str
		// bin: undefined
	};
	
	static
	void getrefs( const Value& v, put_ref put, void* param )
	{
		const Term& term = static_cast< const Term& >( v );
		
		const Symbol& sym = v.dereference< Symbol >();
		
		put( sym.get  (), param );
		put( sym.vtype(), param );
	}
	
	static const refs ref =
	{
		&getrefs,
	};
	
	static const dispatch term_dispatch =
	{
		&term_stringifiers,
		NULL,
		NULL,
		NULL,
		NULL,
		&ref,
	};
	
	Term::Term( symbol_type symtype, const plus::string& name )
	:
		Value( sizeof (Symbol), &generic_destructor< Symbol >, Value_symbol, &term_dispatch )
	{
		new ((void*) pointer()) Symbol( symtype, name );
	}
	
}
