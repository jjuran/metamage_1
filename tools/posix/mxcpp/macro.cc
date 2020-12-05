/*
	macro.cc
	--------
*/

#include "macro.hh"

// vxo
#include "vxo/strmap.hh"

// mxcpp
#include "exception.hh"
#include "predefined.hh"


namespace tool
{
	
	typedef vxo::StrMap_to< macro_t > macro_map;
	
	static macro_map global_macros;
	
	
	void define_macro( const plus::string& pattern, const plus::string& replacement )
	{
		macro_t new_macro;
		
		tokenize( pattern,     new_macro.pattern()     );
		tokenize( replacement, new_macro.replacement() );
		
		const plus::string& name = new_macro.pattern()[ 0 ];
		
		macro_map::const_iterator it = global_macros.find( name );
		
		if ( it == global_macros.end() )
		{
			global_macros[ name ] = new_macro;
		}
		else if ( it->second != new_macro )
		{
			throw exception( pattern );
		}
	}
	
	void undef_macro( const plus::string& name )
	{
		global_macros.erase( name );
	}
	
	bool is_defined( const plus::string& name )
	{
		plus::string predef_value = eval_predefined_macro( name );
		
		return   predef_value.empty() ? false
		       : predef_value != name ? true
		       :                        global_macros.find( name ) != global_macros.end();
	}
	
	const macro_t* find_macro( const plus::string& name )
	{
		macro_map::const_iterator it = global_macros.find( name );
		
		return it != global_macros.end() ? &it->second : NULL;
	}
	
}
