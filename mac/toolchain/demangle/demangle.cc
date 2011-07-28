/*
	demangle.cc
	-----------
	
	Copyright 2010, Joshua Juran
*/

// recall
#include "recall/demangle.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


namespace p7 = poseven;


namespace tool
{
	
	static void demangle( const char* name )
	{
		plus::var_string demangled;
		
		name[0] == '.' ? recall::demangle_MWCPPC( demangled, name )
		               : recall::demangle_MWC68K( demangled, name );
		
		demangled += "\n";
		
		p7::write( p7::stdout_fileno, demangled );
	}
	
	int Main( int argc, char** argv )
	{
		while ( *++argv != NULL )
		{
			demangle( *argv );
		}
		
		return 0;
	}

}

