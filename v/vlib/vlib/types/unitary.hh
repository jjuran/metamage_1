/*
	unitary.hh
	----------
*/

#ifndef VLIB_TYPES_UNITARY_HH
#define VLIB_TYPES_UNITARY_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	
	extern const dispatch unitary_dispatch;
	
	class Unitary : public Value
	{
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &unitary_dispatch;
			}
			
			Unitary( const plus::string& name );
	};
	
}

#endif
