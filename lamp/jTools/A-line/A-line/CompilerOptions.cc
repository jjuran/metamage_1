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
	
	void CompilerOptions::DefineMacro( const std::string& macro, const std::string& value )
	{
		myMacros[ macro ] = value;
	}
	
	void CompilerOptions::DefineMacro( const std::string& macro, const char* value )
	{
		DefineMacro( macro, std::string( value ) );
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
	
	void CompilerOptions::SetPrecompiledHeaderImage( const std::string& phi )
	{
		myPrecompiledHeaderImage = phi;
	}
	
	void CompilerOptions::SetOutput( const std::string& output )
	{
		myOutputDir = output;
	}
	
}

