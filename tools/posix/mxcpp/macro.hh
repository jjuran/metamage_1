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
	
	struct macro_t
	{
		token_list  its_pattern;
		token_list  its_replacement;
		
		macro_t()
		{
		}
		
		macro_t( const token_list& p, const token_list& r )
		:
			its_pattern( p ),
			its_replacement( r )
		{
		}
		
		token_list& pattern()      { return its_pattern;     }
		token_list& replacement()  { return its_replacement; }
		
		token_list const& pattern()     const  { return its_pattern;     }
		token_list const& replacement() const  { return its_replacement; }
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
