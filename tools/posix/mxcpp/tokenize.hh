/*
	tokenize.hh
	-----------
*/

#ifndef MXCPP_TOKENIZE_HH
#define MXCPP_TOKENIZE_HH

// plus
#include "plus/string.hh"

// vxo
#include "vxo/strvec.hh"


namespace tool
{
	
	class token_list : public vxo::StrVec
	{
		public:
			vxo::StrVec      & get()        { return *this; }
			vxo::StrVec const& get() const  { return *this; }
	};
	
	inline bool operator==( const token_list& a, const token_list& b )
	{
		return a.get() == b.get();
	}
	
	inline bool operator!=( const token_list& a, const token_list& b )
	{
		return !( a == b );
	}
	
	void tokenize( const plus::string& input, token_list& output );
	
	token_list tokenize( const plus::string& input );
	
}

#endif
