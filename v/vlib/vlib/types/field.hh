/*
	field.hh
	--------
*/

#ifndef VLIB_TYPES_FIELD_HH
#define VLIB_TYPES_FIELD_HH

// vlib
#include "vlib/type_info.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct proc_info;
	
	extern const dispatch field_dispatch;
	
	class Field : public Value
	{
		public:
			Field( unsigned          size,
			       const type_info&  type,
			       const proc_info&  encode,
			       const proc_info&  decode );
			
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &field_dispatch;
			}
			
			const type_info& base_type() const
			{
				return this->expr()->left.expr()->left.typeinfo();
			}
			
			const char* name() const
			{
				return base_type().name;
			}
			
			const Value& size() const
			{
				return this->expr()->left.expr()->right;
			}
			
			const Value& encoder() const
			{
				return this->expr()->right.expr()->left;
			}
			
			const Value& decoder() const
			{
				return this->expr()->right.expr()->right;
			}
	};
	
}

#endif
