/*
	Commands.cc
	-----------
	
	Joshua Juran
*/

#include "A-line/Commands.hh"

// Standard C
#include <stdlib.h>


namespace tool
{
	
	Command& AugmentCommand( Command& command, const Command& more )
	{
		command.insert( command.end(), more.begin(), more.end() );
		
		return command;
	}
	
	Command& AugmentCommand( Command& command, const StringVector& more )
	{
		for ( size_t i = 0;  i < more.size();  ++i )
		{
			command.push_back( more[ i ].c_str() );
		}
		
		return command;
	}
	
	
	CommandGenerator::CommandGenerator( const TargetInfo& target )
	:
		target( target ),
		m68k  ( target.platform & arch68K               ),
		ppc   ( target.platform & archPPC               ),
		x86   ( target.platform & archX86               ),
		pc_rel( target.platform & model_near  &&  m68k  ),
		a4    ( target.platform & runtimeA4CodeResource ),
		a5    ( target.platform & runtimeA5CodeSegments ),
		cfm   ( target.platform & runtimeCodeFragments  ),
		machO ( target.platform & runtimeMachO          ),
		blue  ( target.platform & apiMacBlue            ),
		carbon( target.platform & apiMacCarbon          ),
		sym   ( target.build     == buildSymbolics ),
		debug ( target.build     != buildRelease   ),
		gnu   ( target.toolchain == toolchainUnix  )
	{
	}
	
	const char* CommandGenerator::UnixCompilerName() const
	{
		if ( const char* cc = getenv( "CC" ) )
		{
			return cc;
		}
		
		return gnu ? "cc" : "mwcc";
	}
	
	const char* CommandGenerator::LinkerName() const
	{
		if ( const char* ld = getenv( "LD" ) )
		{
			return ld;
		}
		
		return gnu ? "c++" : "ld";
	}
	
}
