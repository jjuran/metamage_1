/*
	vector.hh
	---------
*/

#ifndef VLIB_TYPES_VECTOR_VECTOR_HH
#define VLIB_TYPES_VECTOR_VECTOR_HH

// more-libc
#include "more/size.h"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch vector_dispatch;
	
	class Vector : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &vector_dispatch;
			}
			
			Vector( const Value& type, size_t n );
			
			Vector( const Value& type, const plus::string& buffer );
			Vector( const Value& type, const plus::string& buffer, size_t n );
			
			const Value& get_type() const  { return expr()->left; }
			
			const Value& get_endec() const;
			
			size_t size() const;
			
			Value at( size_t i ) const;
			
			Value assign_at( const Value& v, size_t i, bool coercive );
	};
	
	size_t sizeof_vector( const Value& type, size_t n );
	
	extern const type_info vector_vtype;
	
}

#endif
