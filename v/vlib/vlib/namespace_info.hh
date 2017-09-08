/*
	namespace_info.hh
	-----------------
*/

#ifndef VLIB_NAMESPACEINFO_HH
#define VLIB_NAMESPACEINFO_HH

// plus
#include "plus/string_fwd.hh"


namespace vlib
{
	
	class Value;
	
	typedef Value (*getter_type)( const plus::string& name );
	
	struct namespace_info
	{
		const char*  name;
		getter_type  getter;
	};
	
}

#endif
