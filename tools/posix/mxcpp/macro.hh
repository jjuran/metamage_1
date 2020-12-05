/*
	macro.hh
	--------
*/

#ifndef MXCPP_MACRO_HH
#define MXCPP_MACRO_HH

// plus
#include "plus/string_fwd.hh"

// mxcpp
#include "tokenize.hh"


namespace tool
{
	
	struct macro_t : token_lists
	{
		macro_t() : token_lists( 2 )
		{
			push_back( token_list() );
			push_back( token_list() );
		}
		
		macro_t( const token_list& p, const token_list& r )
		:
			token_lists( 2 )
		{
			push_back( p );
			push_back( r );
		}
		
		token_list& pattern()      { return (*this)[ 0 ]; }
		token_list& replacement()  { return (*this)[ 1 ]; }
		
		token_list const& pattern()     const  { return (*this)[ 0 ]; }
		token_list const& replacement() const  { return (*this)[ 1 ]; }
	};
	
	inline bool operator==( const macro_t& a, const macro_t& b )
	{
		return a.pattern() == b.pattern() && a.replacement() == b.replacement();
	}
	
	inline bool operator!=( const macro_t& a, const macro_t& b )
	{
		return !( a == b );
	}
	
	
	class macro_redefined {};
	
	void define_macro( const plus::string& pattern, const plus::string& replacement );
	
	void undef_macro( const plus::string& name );
	
	bool is_defined( const plus::string& name );
	
	const macro_t* find_macro( const plus::string& name );
	
}

#endif
