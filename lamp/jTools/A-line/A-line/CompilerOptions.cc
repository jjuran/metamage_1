/*	==================
 *	CompilerOptions.cc
 *	==================
 */

#include "A-line/CompilerOptions.hh"

// A-line
#include "A-line/TargetNames.hh"


namespace ALine {
	
	// Static variables are zero-initialized
	static FSSpec sNullFSSpec;
	
	CompilerOptions::CompilerOptions( const TargetInfo& targetInfo )
	:
		targetInfo( targetInfo ),
		hasPrecompiledHeaderSource( false ),
		hasPrecompiledHeaderImage ( false ),
		needsCwdSource( false )
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
	
	void CompilerOptions::SetPrecompiledHeaderSource( const FSSpec& pch )
	{
		myPrecompiledHeaderSource = pch;
		hasPrecompiledHeaderSource = true;
	}
	
	void CompilerOptions::SetPrecompiledHeaderImage( const FSSpec& phi )
	{
		myPrecompiledHeaderImage = phi;
		hasPrecompiledHeaderImage = true;
	}
	
	void CompilerOptions::SetOutput( const N::FSDirSpec& output )
	{
		myOutputDir = output;
	}
	
}

