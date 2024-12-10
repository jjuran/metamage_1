/*
	map-reduce.hh
	-------------
*/

#ifndef VLIB_MAPREDUCE_HH
#define VLIB_MAPREDUCE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	Value map   ( const Value& container, const Value& f );
	Value delta ( const Value& container, const Value& f );
	Value seqpop( const Value& container, const Value& f );
	Value anyall( const Value& container, const Value& f, bool end_case );
	Value filter( const Value& container, const Value& f );
	Value reduce( const Value& container, const Value& f );
	
	inline
	Value if_any( const Value& container, const Value& f )
	{
		return anyall( container, f, true );
	}
	
	inline
	Value if_all( const Value& container, const Value& f )
	{
		return anyall( container, f, false );
	}
	
}

#endif
