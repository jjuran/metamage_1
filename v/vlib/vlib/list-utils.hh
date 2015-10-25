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
	
	const Value& first( const Value& list );
	const Value& rest ( const Value& list );
	
	unsigned long count( const Value& list );
	
	Value make_pair( const Value& left, const Value& right );
	
}

#endif
