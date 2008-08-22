/*	==================
 *	CompilerOptions.cc
 *	==================
 */

#include "A-line/CompilerOptions.hh"

// A-line
#include "A-line/TargetNames.hh"


namespace ALine
{
	
	CompilerOptions::CompilerOptions( std::string name, const TargetInfo& targetInfo )
	:
		projectName( name ),
		targetInfo( targetInfo ),
		hasPrecompiledHeaderSource( false )
	{
		
	}
	
	void CompilerOptions::AddDefinedMacro( const char* macro_definition )
	{
		itsMacros.push_back( macro_definition );
	}
	
	void CompilerOptions::DefineMacro( const std::string& macro, const std::string& value )
	{
		itsMacroStorage.push_back( "-D" + macro + "=" + value );
		
		itsMacros.push_back( itsMacroStorage.back().c_str() );
	}
	
	void CompilerOptions::DefineMacro( const std::string& macro, bool value )
	{
		DefineMacro( macro, value ? "1" : "0" );
	}
	
	void CompilerOptions::SetPrecompiledHeaderSource( const std::string& pch )
	{
		myPrecompiledHeaderSource = pch;
		hasPrecompiledHeaderSource = true;
	}
	
	inline std::string Include( const std::string& dir )
	{
		// Turn a pathname into a -I option.
		// Works for gcc and CW Pro 6; doesn't work for CW Pro 2, MrC, or SC.
		
		return "-I" + dir;
	}
	
	void CompilerOptions::AppendIncludeDir( const std::string& dir )
	{
		itsIncludeDirOptions.push_back( Include( dir ) );
	}
	
	void CompilerOptions::PrependIncludeDir( const std::string& dir )
	{
		itsIncludeDirOptions.insert( itsIncludeDirOptions.begin(), Include( dir ) );
	}
	
}

