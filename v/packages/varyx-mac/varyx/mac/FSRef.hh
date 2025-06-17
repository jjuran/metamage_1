/*
	FSRef.hh
	--------
*/

#ifndef VARYX_MAC_FSREF_HH
#define VARYX_MAC_FSREF_HH

// vlib
#include "vlib/value.hh"


struct FSRef;


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
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &FSRef_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			FSRef();
			FSRef( const char* unix_path );
			
			const ::FSRef& get() const
			{
				return *(const ::FSRef*) pointer();
			}
			
			::FSRef* pointer() const
			{
				return (::FSRef*) Value::pointer();
			}
	};
	
	extern const type_info FSRef_vtype;
	
}
}

#endif
