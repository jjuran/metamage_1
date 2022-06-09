/*
	struct.hh
	---------
*/

#ifndef VLIB_TYPES_STRUCT_STRUCT_HH
#define VLIB_TYPES_STRUCT_STRUCT_HH

// more-libc
#include "more/size.h"

// vlib
#include "vlib/iterators/list_iterator.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
	class Struct_Type;
	
	extern const dispatch struct_dispatch;
	
	class Struct : public Value
	{
		public:
			typedef Struct_Type Type;
			
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &struct_dispatch;
			}
			
			Struct( const Type& type, const plus::string& buffer );
			
			explicit
			Struct( const Type& type, const Value& init_list = empty_list );
			
			Type  const& get_type() const  { return *(Type*) &expr()->left;  }
			Value const& get_data() const  { return           expr()->right; }
			Value&       get_data()        { return           expr()->right; }
			
			Value& mutable_data()  { return unshare().expr()->right; }
	};
	
}

#endif
