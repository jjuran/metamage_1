/*	==================
 *	CompilerOptions.hh
 *	==================
 */

#pragma once

// C++
#include <list>
#include <string>
#include <vector>

// A-line
#include "A-line/TargetTypes.hh"


namespace ALine
{
	
	typedef std::vector< const char* > Macros;
	
	struct TargetInfo;
	
	
	class CompilerOptions
	{
		private:
			std::string projectName;
			const TargetInfo& targetInfo;
			Macros itsMacros;
			std::list< std::string > itsMacroStorage;
			std::string myPrecompiledHeaderSource;
			std::vector< std::string > itsIncludeDirOptions;
			bool hasPrecompiledHeaderSource;
		
		public:
			CompilerOptions( std::string name, const TargetInfo& targetInfo );
			
			const std::string& Name() const  { return projectName; }
			const TargetInfo& Target() const  { return targetInfo; }
			
			void AddDefinedMacro( const char* macro_definition );
			
			void DefineMacro( const std::string& macro, const std::string& value );
			void DefineMacro( const std::string& macro, bool value = true );
			
			void SetPrecompiledHeaderSource( const std::string& pch );
			void SetOutput( const std::string& output );
			
			const Macros& GetMacros() const  { return itsMacros; }
			
			bool HasPrecompiledHeaderSource() const  { return hasPrecompiledHeaderSource; }
			
			const std::string& PrecompiledHeaderSource() const  { return myPrecompiledHeaderSource; }
			
			std::vector< std::string > const& IncludeDirOptions() const  { return itsIncludeDirOptions; }
			
			void AppendIncludeDir ( const std::string& dir );
			void PrependIncludeDir( const std::string& dir );
			
	};
	
}

