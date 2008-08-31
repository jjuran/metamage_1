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
			std::string                 its_project_name;
			const TargetInfo&           its_target_info;
			Macros                      its_macros;
			std::list< std::string >    its_macro_storage;
			std::vector< std::string >  its_include_search_options;
			std::string                 its_precompiled_header_source_pathname;
			bool                        it_has_precompiled_header_source;
		
		public:
			CompilerOptions( std::string name, const TargetInfo& its_target_info );
			
			const std::string& Name() const  { return its_project_name; }
			const TargetInfo& Target() const  { return its_target_info; }
			
			void AddDefinedMacro( const char* macro_definition );
			
			void DefineMacro( const std::string& macro, const std::string& value );
			void DefineMacro( const std::string& macro, bool value = true );
			
			void SetPrecompiledHeaderSource( const std::string& pch );
			void SetOutput( const std::string& output );
			
			const Macros& GetMacros() const  { return its_macros; }
			
			bool HasPrecompiledHeaderSource() const  { return it_has_precompiled_header_source; }
			
			const std::string& PrecompiledHeaderSource() const  { return its_precompiled_header_source_pathname; }
			
			std::vector< std::string > const& IncludeDirOptions() const  { return its_include_search_options; }
			
			void AppendIncludeDir ( const std::string& dir );
			void PrependIncludeDir( const std::string& dir );
			
	};
	
}

