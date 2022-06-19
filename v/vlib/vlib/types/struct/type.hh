/*
	type.hh
	-------
*/

#ifndef VLIB_TYPES_STRUCT_TYPE_HH
#define VLIB_TYPES_STRUCT_TYPE_HH

// more-libc
#include "more/size.h"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class Member;
	struct dispatch;
	
	extern const dispatch structtype_dispatch;
	
	class Struct_Type : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &structtype_dispatch;
			}
			
			Struct_Type( const Value& fields, const Value& name );
			
			const Value& name() const;
			const Value& fields() const;
			
			size_t byte_size() const;
			
			Value get( const Value& data, const Member& name ) const;
			
			void set( Value& data, const Member& name, const Value& v ) const;
			
			Value pack_initializer_list( const Value& init_list ) const;
			
			Value unpack_fields( const Value& data ) const;
	};
	
}

#endif
