/*
	endian.hh
	---------
*/

#ifndef VLIB_TYPES_ENDIAN_HH
#define VLIB_TYPES_ENDIAN_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class Endianness;
	
	struct dispatch;
	
	extern const dispatch endian_dispatch;
	
	class Endian : public Value
	{
		public:
			typedef Endianness E;
			
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &endian_dispatch;
			}
			
			Endian( const Endianness& e, const Value& type );
			
			const Endianness& endianness() const  { return (E&) expr()->left;  }
			const Value& underlying_type() const  { return      expr()->right; }
	};
	
	Value endianized( const Endianness& e, const Value& type );
	
}

#endif
