/*
	string.hh
	---------
*/

#ifndef VXO_STRING_HH
#define VXO_STRING_HH

// vxo
#include "vxo/box.hh"


namespace vxo
{
	
	class BasicString : public Box
	{
		public:
			static bool test( const Box& box )
			{
				return (uint8_t) box.datatype_byte() <= Box_max_inline_string;
			}
	};
	
	class StaticString : public BasicString
	{
		public:
			explicit StaticString( const char* s );
	};
	
}

#endif
