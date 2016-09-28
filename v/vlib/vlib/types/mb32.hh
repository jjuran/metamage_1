/*
	mb32.hh
	-------
*/

#ifndef VLIB_TYPES_MB32_HH
#define VLIB_TYPES_MB32_HH

// plus
#include "plus/string_fwd.hh"

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
	extern const dispatch mb32_dispatch;
	
	class MB32 : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.type() == Value_mb32;
			}
			
			static Value coerce( const Value& v );
			
			MB32( uint32_t mb = 0 );
		
		private:
			MB32( const plus::string& s );
	};
	
	extern const type_info mb32_vtype;
	
}

#endif
