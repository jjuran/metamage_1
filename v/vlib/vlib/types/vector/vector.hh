/*
	vector.hh
	---------
*/

#ifndef VLIB_TYPES_VECTOR_VECTOR_HH
#define VLIB_TYPES_VECTOR_VECTOR_HH

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
			typedef unsigned long size_type;
			
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &vector_dispatch;
			}
			
			Vector( const Value& endec, size_type n = 0 );
			
			Vector( const Value& endec, const plus::string& buffer );
			
			const Value& get_endec() const  { return expr()->left; }
			
			size_type size() const;
			
			Value at( size_type i ) const;
			
			Value assign_at( const Value& v, size_type i, bool coercive );
	};
	
	extern const type_info vector_vtype;
	
}

#endif
