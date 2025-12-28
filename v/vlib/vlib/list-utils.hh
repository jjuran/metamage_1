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
	
	struct mutable_list_overrun {};
	
	bool is_single( const Value& v );
	
	const Value& rest ( const Value& list );
	
	Value& first_mutable( Value& list );
	Value& rest_mutable ( Value& list );
	
	const Value& get_nth( const Value& list, unsigned i );
	
	Value& get_nth_mutable( Value& list, unsigned i );
	
	inline
	const Value& first( const Value& list )
	{
		return get_nth( list, 0 );
	}
	
	unsigned long count( const Value& list );
	
	void splice_lists( Value& left, const Value& right );
	
	Value make_list( const Value& left, const Value& right );
	
	Value reverse_list( const Value& list );
	
	Value bind_args( const Value& f, const Value& arguments );
	
}

#endif
