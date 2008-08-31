/*	==================
 *	CompilerOptions.cc
 *	==================
 */

#include "A-line/CompilerOptions.hh"

// A-line
#include "A-line/TargetNames.hh"


namespace ALine
{
	
	CompilerOptions::CompilerOptions( std::string name, const TargetInfo& target_info )
	: its_project_name                ( name        ),
	  its_target_info                 ( target_info ),
	  it_has_precompiled_header_source( false       )
	{
		
	}
	
	void CompilerOptions::AddDefinedMacro( const char* macro_definition )
	{
		its_macros.push_back( macro_definition );
	}
	
	void CompilerOptions::DefineMacro( const std::string& macro, const std::string& value )
	{
		its_macro_storage.push_back( "-D" + macro + "=" + value );
		
		its_macros.push_back( its_macro_storage.back().c_str() );
	}
	
	void CompilerOptions::DefineMacro( const std::string& macro, bool value )
	{
		DefineMacro( macro, value ? "1" : "0" );
	}
	
	void CompilerOptions::SetPrecompiledHeaderSource( const std::string& pch )
	{
		its_precompiled_header_source_pathname = pch;
		
		it_has_precompiled_header_source = true;
	}
	
	inline std::string Include( const std::string& dir )
	{
		// Turn a pathname into a -I option.
		// Works for gcc and CW Pro 6; doesn't work for CW Pro 2, MrC, or SC.
		
		return "-I" + dir;
	}
	
	void CompilerOptions::AppendIncludeDir( const std::string& dir )
	{
		its_include_search_options.push_back( Include( dir ) );
	}
	
	void CompilerOptions::PrependIncludeDir( const std::string& dir )
	{
		its_include_search_options.insert( its_include_search_options.begin(), Include( dir ) );
	}
	
}

