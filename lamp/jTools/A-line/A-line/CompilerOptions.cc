/*	==================
 *	CompilerOptions.cc
 *	==================
 */

#include "A-line/CompilerOptions.hh"

// Standard C++
#include <algorithm>

// plus
#include "plus/string/concat.hh"

// vxo
#include "vxo/strpool.hh"

// A-line
#include "A-line/TargetNames.hh"


namespace tool
{
	
	CompilerOptions::CompilerOptions( const plus::string&  name,
	                                  const TargetInfo&    target_info )
	: its_project_name                ( name        ),
	  its_target_info                 ( target_info ),
	  it_has_precompiled_header_source( false       )
	{
		
	}
	
	void CompilerOptions::AddDefinedMacro( const char* macro_definition )
	{
		its_macros.push_back( macro_definition );
	}
	
	void CompilerOptions::DefineMacro( const plus::string& macro, const plus::string& value )
	{
		static vxo::StrPool macro_storage;
		
		its_macros.push_back( macro_storage[ "-D" + macro + "=" + value ] );
	}
	
	void CompilerOptions::DefineMacro( const plus::string& macro, bool value )
	{
		DefineMacro( macro, value ? "1" : "0" );
	}
	
	void CompilerOptions::SetPrecompiledHeaderSource( const plus::string& pch )
	{
		its_precompiled_header_source_pathname = pch;
		
		it_has_precompiled_header_source = true;
	}
	
	static inline plus::string Include( const plus::string& dir )
	{
		/*
			Input an include path and output the command-line option.
			This works for gcc and CW Pro 6.
			
			It doesn't work for CW Pro 3, MrC, or SC, but we can preprocess
			separately in these cases.
		*/
		
		return "-I" + dir;
	}
	
	void CompilerOptions::AppendIncludeDir( const plus::string& dir )
	{
		const plus::string include = Include( dir );
		
		StringVector& dirs = its_include_search_options;
		
		// This is O(n^3), where
		// n[1] is the number of calls to AppendIncludeDir(),
		// n[2] is the number of unique paths, and
		// n[3] is the average length of a path.
		// None of these are expected to be large enough to require optimization,
		// and saving even a single disk access will be a performance win.
		
		if ( std::find( dirs.begin(), dirs.end(), include ) == dirs.end() )
		{
			its_include_search_options.push_back( include );
		}
	}
	
	void CompilerOptions::PrependIncludeDir( const plus::string& dir )
	{
		its_include_search_options.insert( its_include_search_options.begin(), Include( dir ) );
	}
	
}
