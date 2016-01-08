/*
	symdesc.hh
	----------
*/

#ifndef VLIB_SYMDESC_HH
#define VLIB_SYMDESC_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	Value make_metasymbol( const plus::string& name, int depth, int index );
	
	const Value& resolve_symbol( const Value& v, const Value& stack );
	
}

#endif
