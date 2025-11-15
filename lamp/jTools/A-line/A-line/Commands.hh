/*	===========
 *	Commands.hh
 *	===========
 */

#ifndef ALINE_COMMANDS_HH
#define ALINE_COMMANDS_HH

// A-line
#include "A-line/StrConVector.hh"
#include "A-line/StringVector.hh"
#include "A-line/TargetInfo.hh"


namespace tool
{
	
	typedef StrConVector Command;
	
	
	inline Command MakeCommand( const char* a, const char* b )
	{
		Command result;
		
		result.push_back( a );
		result.push_back( b );
		
		return result;
	}
	
	Command& AugmentCommand( Command& command, const Command&      more );
	Command& AugmentCommand( Command& command, const StringVector& more );
	
	inline Command OutputOption( const char* pathname )
	{
		return MakeCommand( "-o", pathname );
	}
	
	struct CommandGenerator
	{
		TargetInfo target;
		
		bool m68k, ppc, x86;
		bool pc_rel;  // model near
		bool a4, a5, cfm, machO;
		bool blue, carbon;
		bool sym, debug;
		bool gnu;
		
		CommandGenerator( const TargetInfo& target );
		
		const char* UnixCompilerName() const;
		
		Command CompilerName() const
		{
			return MakeCommand( UnixCompilerName(), "-c" );
		}
		
		const char* LinkerName() const;
		
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
				
				if ( ppc )
				{
					result.push_back( "-fasm-blocks" );
				}
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
			return   sym   ? "-O0"
			       : debug ? "-O2"
			       : gnu   ? "-O2"
			       :         "-O4";
		}
		
		Command CodeGenOptions() const
		{
			Command result;
			
			result.push_back( Optimization() );
			
			if ( sym )
			{
				result.push_back( "-g" );
			}
			
			if ( pc_rel )
			{
				result.push_back( "-mpcrel" );
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
		
		// CodeWarrior only
		const char* MWTargetSharedLibrary() const
		{
			return "-dynamic";
		}
		
		const char* LinkerOptions() const
		{
			return + sym || gnu ? ""
			       : debug      ? "-S"
			       :              "-s";
		}
		
	};
	
}

#endif
