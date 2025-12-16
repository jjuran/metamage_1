/*
	Region.hh
	---------
*/

#ifndef VARYX_QUICKDRAW_REGION_HH
#define VARYX_QUICKDRAW_REGION_HH

// iota
#include "iota/class.hh"

// vlib
#include "vlib/value.hh"


typedef char** Handle;

namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
}

namespace varyx
{
namespace quickdraw
{
	
	using namespace vlib;
	
	extern const dispatch Region_dispatch;
	
	class Region : public Value
	{
		NO_NEW_DELETE
		
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &Region_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			Region();
			
			::Handle get_handle() const
			{
				return *(::Handle*) pointer();
			}
			
			unsigned long data_size(         ) const;
			char*         copy_data( char* p ) const;
	};
	
	extern const type_info Region_vtype;
	
}
}

#endif
