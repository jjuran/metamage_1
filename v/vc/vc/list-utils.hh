/*
	list-utils.hh
	-------------
*/

#ifndef VC_LISTUTILS_HH
#define VC_LISTUTILS_HH

// vc
#include "vc/value.hh"


namespace vc
{
	
	const Value& first( const Value& list );
	const Value& rest ( const Value& list );
	
	unsigned long count( const Value& list );
	
	Value make_pair( const Value& left, const Value& right );
	
}

#endif
