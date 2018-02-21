/*
	bareword.hh
	-----------
*/

#ifndef VLIB_TYPES_BAREWORD_HH
#define VLIB_TYPES_BAREWORD_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	struct dispatch;
	struct stringifiers;
	
	extern const stringifiers  bareword_stringifiers;
	extern const dispatch      bareword_dispatch;
	
	class Bareword : public Value
	{
		protected:
			Bareword( const plus::string& s )
			:
				Value( (const vu_string&) s, V_str, &bareword_dispatch )
			{
			}
			
		public:
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &bareword_dispatch;
			}
			
			const plus::string& get() const  { return string(); }
	};
	
	class Member : public Bareword
	{
		public:
			Member( const plus::string& s ) : Bareword( s )
			{
			}
	};
	
}

#endif
