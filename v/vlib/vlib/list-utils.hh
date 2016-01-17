/*
	list-utils.hh
	-------------
*/

#ifndef VLIB_LISTUTILS_HH
#define VLIB_LISTUTILS_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	const Value& get_nth( const Value& list, unsigned i );
	
	unsigned long count( const Value& list );
	
	Value make_pair( const Value& left, const Value& right );
	
}

#endif
