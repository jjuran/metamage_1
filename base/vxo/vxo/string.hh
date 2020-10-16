/*
	string.hh
	---------
*/

#ifndef VXO_STRING_HH
#define VXO_STRING_HH

// plus
#include "plus/string_fwd.hh"

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
	
	class DynamicString : public BasicString
	{
		private:
			typedef plus::string string;
		
		public:
			DynamicString( const string& s );
			
			string&       get()        { return *(string*)       this; }
			string const& get() const  { return *(string const*) this; }
			
			operator string const&() const  { return get(); }
	};
	
	typedef DynamicString String;
	
}

#endif
