/*
	type_info.hh
	------------
*/

#ifndef VLIB_TYPEINFO_HH
#define VLIB_TYPEINFO_HH

// plus
#include "plus/string_fwd.hh"


namespace vlib
{
	
	class Value;
	
	typedef Value (*assign_proc)( const Value& v );
	typedef Value (*coerce_proc)( const Value& v );
	
	typedef Value (*member_proc)( const Value& obj,
	                              const plus::string& member );
	
	struct type_info
	{
		char const* const  name;
		assign_proc const  assign;
		coerce_proc const  coerce;
		member_proc const  member;
		int         const  flags;
	};
	
}

#endif
