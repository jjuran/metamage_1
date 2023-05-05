/*
	string.hh
	---------
*/

#ifndef VXOSTRING_STRING_HH
#define VXOSTRING_STRING_HH

// plus
#include "plus/string.hh"

// vxo
#include "vxo/string.hh"


namespace vxo
{
	
	class DynamicString : public BasicString
	{
		private:
			typedef plus::string string;
		
		public:
			DynamicString( const string& s );
			
			string&       get()        { return *(string*)       this; }
			string const& get() const  { return *(string const*) this; }
			
			bool empty() const  { return get().empty(); }
			
			const char* c_str() const  { return get().c_str(); }
			
			operator string const&() const  { return get(); }
			
			DynamicString& operator=( const string& s );
	};
	
	typedef DynamicString String;
	
}

#endif
