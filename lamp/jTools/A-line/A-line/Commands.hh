/*	===========
 *	Commands.hh
 *	===========
 */

#pragma once

// Standard C++
#include <string>

// MoreFunctional
#include "FunctionalExtensions.hh"

// CompileDriver
#include "CompileDriver/Platform.hh"


namespace ALine
{
	
	namespace CD = CompileDriver;
	
	
	static const std::string space = " ";
	
	typedef std::vector< const char* > Command;
	
	inline Command MakeCommand( const char* a, const char* b )
	{
		Command result;
		
		result.push_back( a );
		result.push_back( b );
		
		return result;
	}
	
	inline Command& AugmentCommand( Command& command, const Command& more )
	{
		command.insert( command.end(), more.begin(), more.end() );
		
		return command;
	}
	
	static const char* c_str( const std::string& s )
	{
		return s.c_str();
	}
	
	inline Command& AugmentCommand( Command& command, const std::vector< std::string >& more )
	{
		command.reserve( command.size() + more.size() );
		
		std::transform( more.begin(),
		                more.end(),
		                std::back_inserter( command ),
		                std::ptr_fun( c_str ));
		
		return command;
	}
	
	inline Command OutputOption( const char* pathname )
	{
		return MakeCommand( "-o", pathname );
	}
	
	struct CommandGenerator
	{
		TargetInfo target;
		
		bool m68k, ppc, x86;
		bool a4, a5, cfm, machO, elf;
		bool blue, carbon;
		bool debug;
		bool gnu;
		
		CommandGenerator( const TargetInfo& target )
		:
			target( target ),
			m68k  ( target.platform & CD::arch68K               ),
			ppc   ( target.platform & CD::archPPC               ),
			x86   ( target.platform & CD::archX86               ),
			a4    ( target.platform & CD::runtimeA4CodeResource ),
			a5    ( target.platform & CD::runtimeA5CodeSegments ),
			cfm   ( target.platform & CD::runtimeCodeFragments  ),
			machO ( target.platform & CD::runtimeMachO          ),
			elf   ( target.platform & CD::runtimeELF            ),
			blue  ( target.platform & CD::apiMacToolbox         ),
			carbon( target.platform & CD::apiMacCarbon          ),
			debug ( target.build   == buildDebug ),
			gnu   ( target.toolkit == toolkitGNU )
		{}
		
		const char* UnixCompilerName() const  { return gnu ? "gcc" : "mwcc"; }
		
		Command CompilerName() const
		{
			return MakeCommand( UnixCompilerName(), "-c" );
		}
		
		const char* LinkerName() const
		{
			return gnu ? "g++"
			           : "ld";
		}
		
		Command TargetArchitecture() const
		{
			if ( elf )
			{
				return Command();
			}
			
			const char* arch = ppc  ? "ppc"
			                 : m68k ? "m68k"
			                 :        "i386";
			
			return MakeCommand( "-arch", arch );
		}
		
		Command LibraryMakerName() const
		{
			Command result;
			
			if ( gnu )
			{
				result.push_back( "ar" );
				result.push_back( "rcs" );
			}
			else
			{
				result.push_back( "ld" );
				result.push_back( "-static" );
				
				AugmentCommand( result, TargetArchitecture() );
			}
			
			return result;
		}
		
		// This means that we pass the precompiled output, not the header source.
		// Required for CodeWarrior (i.e. otherwise you lose precompile benefit).
		// Not supported by gcc.
		//bool PrecompiledHeaderIsImage() const  { return !gnu; }
		
		// This means that the prefix header is given by name only
		// and the parent directory of the precompiled header image
		// is in the search path.
		// Required for gcc unless the precompiled header image is in the same
		// directory as the precompiled header source.
		// Not supported by CodeWarrior.
		//bool PrecompiledHeaderIsSearched() const  { return gnu; }
		
		Command Prefix( const char* pathname ) const
		{
			return MakeCommand( "-include", pathname );
		}
		
		Command LanguageOptions() const
		{
			Command result;
			
			if ( machO )
			{
				// Pascal strings are Mac-only, but implied until OS X
				result.push_back( "-fpascal-strings" );
			}
			
			if ( !gnu )
			{
				// gcc won't compile shared_ptr without RTTI;
				// otherwise we can turn it off since we don't use it
				result.push_back( "-fno-rtti" );
			}
			
			return result;
		}
		
		
		const char* Optimization() const
		{
			return   debug ? "-O0"
			       : gnu   ? "-O2"
			       :         "-O4";
		}
		
		Command CodeGenOptions() const
		{
			Command result;
			
			result.push_back( Optimization() );
			
			if ( debug )
			{
				result.push_back( "-g" );
			}
			
			if ( m68k && cfm )
			{
				result.push_back( "-mCFM" );
			}
			
			if ( a4 )
			{
				result.push_back( "-mA4-globals" );
			}
			
			return result;
		}
		
		Command AllCompilerOptions() const
		{
			Command result;
			
			AugmentCommand( result, TargetArchitecture() );
			AugmentCommand( result, LanguageOptions   () );
			AugmentCommand( result, CodeGenOptions    () );
			
			return result;
		}
		
		Command OutputType( const char* type ) const
		{
			return MakeCommand( "-t", type );
		}
		
		Command OutputCreator( const char* creator ) const
		{
			return MakeCommand( "-c", creator );
		}
		
		Command ResourceTypeAndID( const char* type_and_id ) const
		{
			return MakeCommand( "-rt", type_and_id );
		}
		
		Command ResourceName( const char* name ) const
		{
			return MakeCommand( "-rsrcname", name );
		}
		
		const char* CustomDriverHeader() const  { return "-custom"; }
		
		const char* TargetApplication() const
		{
			return gnu ? "" : "-bundle";
		}
		
		// CodeWarrior only
		const char* MWTargetCodeResource() const
		{
			return "-object";
		}
		
		// CodeWarrior only
		const char* MWTargetSharedLibrary() const
		{
			return "-dynamic";
		}
		
		const char* TargetCommandLineTool() const
		{
			return gnu ? ""
			           : "-execute";
		}
		
		const char* LinkerOptions() const
		{
			return debug ? "" : "-s";
		}
		
	};
	
	
	template < class F, class Iter >
	std::string join( Iter begin, Iter end, const std::string& glue = "", F f = F() )
	{
		if ( begin == end )
		{
			return "";
		}
		
		std::string result = f( *begin++ );
		
		while ( begin != end )
		{
			result += glue;
			result += f( *begin++ );
		}
		
		return result;
	}
	
	template < class Iter >
	std::string join( Iter begin, Iter end, const std::string& glue = "" )
	{
		return join( begin, end, glue, more::identity< std::string >() );
	}
	
}

