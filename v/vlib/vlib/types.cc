/*
	types.cc
	--------
*/

#include "vlib/types.hh"

// vlib
#include "vlib/type_info.hh"


namespace vlib
{
	
	#define DEFINE_TYPE_INFO( type )  \
	const type_info type##_vtype = { #type }
	
	DEFINE_TYPE_INFO( boolean  );
	DEFINE_TYPE_INFO( function );
	DEFINE_TYPE_INFO( integer  );
	DEFINE_TYPE_INFO( string   );
	DEFINE_TYPE_INFO( type     );
	
}
