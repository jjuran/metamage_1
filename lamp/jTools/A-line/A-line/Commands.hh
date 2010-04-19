/*	===========
 *	Commands.hh
 *	===========
 */

#ifndef ALINE_COMMANDS_HH
#define ALINE_COMMANDS_HH

// Standard C++
#include <algorithm>
#include <string>
#include <vector>

// A-line
#include "A-line/TargetInfo.hh"

// CompileDriver
#include "CompileDriver/Platform.hh"


namespace tool
{
	
	typedef std::vector< const char* > Command;
	
	
	void RunCommand( const Command&      command,
			         const char*         diagnostics_pathname,
			         const std::string&  caption );
	
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
		bool a4, a5, cfm, machO;
		bool blue, carbon;
		bool debug;
		bool gnu;
		
		CommandGenerator( const TargetInfo& target )
		:
			target( target ),
			m68k  ( target.platform & arch68K               ),
			ppc   ( target.platform & archPPC               ),
			x86   ( target.platform & archX86               ),
			a4    ( target.platform & runtimeA4CodeResource ),
			a5    ( target.platform & runtimeA5CodeSegments ),
			cfm   ( target.platform & runtimeCodeFragments  ),
			machO ( target.platform & runtimeMachO          ),
			blue  ( target.platform & apiMacBlue            ),
			carbon( target.platform & apiMacCarbon          ),
			debug ( target.build     == buildDebug   ),
			gnu   ( target.toolchain == toolchainGNU )
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
			if ( !ALINE_CROSS_DEVELOPMENT )
			{
				return Command();
			}
			
			const char* arch = ppc  ? "ppc"
			                 : m68k ? "m68k"
			                 :        "i386";
			
			return MakeCommand( "-arch", arch );
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
		
		Command ResourceTypeAndID( const char* type_and_id ) const
		{
			return MakeCommand( "-rt", type_and_id );
		}
		
		Command ResourceName( const char* name ) const
		{
			return MakeCommand( "-rsrcname", name );
		}
		
		const char* CustomDriverHeader() const  { return "-custom"; }
		
		// CodeWarrior only
		const char* MWTargetSharedLibrary() const
		{
			return "-dynamic";
		}
		
		const char* LinkerOptions() const
		{
			return debug || gnu ? "" : "-s";
		}
		
	};
	
}

#endif

