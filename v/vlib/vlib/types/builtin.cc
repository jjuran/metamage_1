/*
	builtin.cc
	----------
*/

#include "vlib/types/builtin.hh"

// vlib
#include "vlib/value.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/stringify.hh"


namespace vlib
{
	
	/*
		proc_info and type_info can be thought of as extending builtin_info,
		although they don't actually derive from it.
	*/
	
	struct builtin_info
	{
		char const* const  name;
	};
	
	static
	const char* builtin_str_data( const Value& v )
	{
		return v.dereference< builtin_info >().name;
	}
	
	static const stringify builtin_str =
	{
		&builtin_str_data,
		NULL,
		NULL,
	};
	
	const stringifiers builtin_stringifiers =
	{
		&builtin_str,
		// rep: ditto
		// bin: not defined
	};
	
	
	static inline
	const builtin_info* addr( const Value& v )
	{
		return &v.dereference< builtin_info >();
	}
	
	static
	bool builtin_equal( const Value& a, const Value& b )
	{
		return addr( a ) == addr( b );
	}
	
	const comparison builtin_comparison =
	{
		NULL,  // built-in procs and types have no ordering
		&builtin_equal,
	};
	
}
