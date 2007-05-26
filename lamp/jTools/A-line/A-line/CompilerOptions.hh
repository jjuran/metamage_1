/*	==================
 *	CompilerOptions.hh
 *	==================
 */

#pragma once

// C++
#include <map>
#include <string>
#include <vector>

// Nitrogen
#include "Nitrogen/Files.h"

// A-line
#include "A-line/TargetTypes.hh"


namespace ALine
{
	
	namespace N = Nitrogen;
	
	
	typedef std::map< std::string, std::string > Map;
	
	struct TargetInfo;
	
	class CompilerOptions
	{
		private:
			const TargetInfo& targetInfo;
			Map myMacros;
			FSSpec myPrecompiledHeaderSource;
			FSSpec myPrecompiledHeaderImage;
			N::FSDirSpec myOutputDir;
			std::vector< N::FSDirSpec > myUserOnlyIncludeDirs;
			bool hasPrecompiledHeaderSource;
			bool hasPrecompiledHeaderImage;
			bool needsCwdSource;
		
		public:
			CompilerOptions( const TargetInfo& targetInfo );
			
			const TargetInfo& Target() const  { return targetInfo; }
			
			void DefineMacro( const std::string& macro, const std::string& value );
			void DefineMacro( const std::string& macro, const char* value );
			void DefineMacro( const std::string& macro, bool value = true );
			
			void SetPrecompiledHeaderSource( const FSSpec& pch );
			void SetPrecompiledHeaderImage ( const FSSpec& phi );
			void SetOutput( const N::FSDirSpec& output );
			
			const Map&    Macros() const  { return myMacros; }
			
			bool HasPrecompiledHeaderSource() const  { return hasPrecompiledHeaderSource; }
			bool HasPrecompiledHeaderImage () const  { return hasPrecompiledHeaderImage;  }
			
			const FSSpec& PrecompiledHeaderSource() const  { return myPrecompiledHeaderSource; }
			const FSSpec& PrecompiledHeaderImage () const  { return myPrecompiledHeaderImage;  }
			
			const N::FSDirSpec& Output() const  { return myOutputDir; }
			
			std::vector< N::FSDirSpec > const& UserOnlyIncludeDirs() const       { return myUserOnlyIncludeDirs; }
			std::vector< N::FSDirSpec >      & UserOnlyIncludeDirs()             { return myUserOnlyIncludeDirs; }
			
			void SetCwdSource( bool needs = true )  { needsCwdSource = needs; }
			bool NeedsCwdSource() const  { return needsCwdSource; }
			
	};
	
}

