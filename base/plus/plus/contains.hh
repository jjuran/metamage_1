/*
	plus/contains.hh
	----------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef PLUS_CONTAINS_HH
#define PLUS_CONTAINS_HH

// Standard C++
#include <algorithm>


namespace plus
{
	
	template < class Sequence, class Member >
	inline bool contains( const Sequence& sequence, const Member& member )
	{
		return std::find( sequence.begin(),
		                  sequence.end(),
		                  member ) != sequence.end();
	}
	
	template < class Sequence, class Predicate >
	inline bool contains_if( const Sequence& sequence, const Predicate& predicate )
	{
		return std::find_if( sequence.begin(),
		                     sequence.end(),
		                     predicate ) != sequence.end();
	}
	
}

#endif

