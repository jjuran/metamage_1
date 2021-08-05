/*
	FSSpec.hh
	---------
*/

#ifndef VARYX_MAC_FSSPEC_HH
#define VARYX_MAC_FSSPEC_HH

// vlib
#include "vlib/value.hh"


struct FSSpec;


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
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &FSSpec_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			FSSpec();
			FSSpec( const char* unix_path );
			
			const ::FSSpec& get() const
			{
				return *(const ::FSSpec*) pointer();
			}
			
			::FSSpec* pointer() const
			{
				return (::FSSpec*) Value::pointer();
			}
	};
	
	extern const type_info FSSpec_vtype;
	
}
}

#endif
