/*
	fold.hh
	-------
*/

#ifndef VLIB_FOLD_HH
#define VLIB_FOLD_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class lexical_scope;
	
	Value fold( const Value& v, lexical_scope* scope );
	
}

#endif
