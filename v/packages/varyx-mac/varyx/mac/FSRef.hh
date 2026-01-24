/*
	FSRef.hh
	--------
*/

#ifndef VARYX_MAC_FSREF_HH
#define VARYX_MAC_FSREF_HH

// mac-types
#include "mac_types/FSRef.hh"

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
	
	extern const dispatch FSRef_dispatch;
	
	class FSRef : public Value
	{
		typedef ::mac::types::FSRef POD_type;
		
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &FSRef_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			FSRef();
			FSRef( const char* unix_path );
			
			const POD_type& get() const
			{
				return *(const POD_type*) pointer();
			}
			
			POD_type* pointer() const
			{
				return (POD_type*) Value::pointer();
			}
	};
	
	extern const type_info FSRef_vtype;
	
}
}

#endif
