/*
	FSSpec.hh
	---------
*/

#ifndef VARYX_MAC_FSSPEC_HH
#define VARYX_MAC_FSSPEC_HH

// mac-types
#include "mac_types/FSSpec.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
}

namespace varyx
{
namespace mac
{
	
	using namespace vlib;
	
	extern const dispatch FSSpec_dispatch;
	
	class FSSpec : public Value
	{
		typedef ::mac::types::FSSpec POD_type;
		
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &FSSpec_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			FSSpec();
			FSSpec( const char* unix_path );
			
			const POD_type& get() const
			{
				return *(const POD_type*) pointer();
			}
			
			POD_type* pointer() const
			{
				return (POD_type*) Value::pointer();
			}
	};
	
	extern const type_info FSSpec_vtype;
	
}
}

#endif
