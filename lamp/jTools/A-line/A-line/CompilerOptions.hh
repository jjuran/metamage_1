/*	==================
 *	CompilerOptions.hh
 *	==================
 */

#pragma once

// C++
#include <map>
#include <string>
#include <vector>

// A-line
#include "A-line/TargetTypes.hh"


namespace ALine
{
	
	typedef std::map< std::string, std::string > Map;
	
	struct TargetInfo;
	
	
	class CompilerOptions
	{
		private:
			const TargetInfo& targetInfo;
			Map myMacros;
			std::string myPrecompiledHeaderSource;
			std::string myPrecompiledHeaderImage;
			std::string myOutputDir;
			std::vector< std::string > myUserOnlyIncludeDirs;
			bool hasPrecompiledHeaderSource;
			bool needsCwdSource;
		
		public:
			CompilerOptions( const TargetInfo& targetInfo );
			
			const TargetInfo& Target() const  { return targetInfo; }
			
			void DefineMacro( const std::string& macro, const std::string& value );
			void DefineMacro( const std::string& macro, const char* value );
			void DefineMacro( const std::string& macro, bool value = true );
			
			void SetPrecompiledHeaderSource( const std::string& pch );
			void SetPrecompiledHeaderImage ( const std::string& phi );
			void SetOutput( const std::string& output );
			
			const Map&    Macros() const  { return myMacros; }
			
			bool HasPrecompiledHeaderSource() const  { return hasPrecompiledHeaderSource; }
			
			const std::string& PrecompiledHeaderSource() const  { return myPrecompiledHeaderSource; }
			const std::string& PrecompiledHeaderImage () const  { return myPrecompiledHeaderImage;  }
			
			const std::string& Output() const  { return myOutputDir; }
			
			std::vector< std::string > const& UserOnlyIncludeDirs() const       { return myUserOnlyIncludeDirs; }
			std::vector< std::string >      & UserOnlyIncludeDirs()             { return myUserOnlyIncludeDirs; }
			
			void SetCwdSource( bool needs = true )  { needsCwdSource = needs; }
			bool NeedsCwdSource() const  { return needsCwdSource; }
			
	};
	
}

