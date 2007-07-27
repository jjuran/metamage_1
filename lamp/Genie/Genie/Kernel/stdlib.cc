/*	=========
 *	stdlib.cc
 *	=========
 */

// Standard C
#include <errno.h>
#include <stdlib.h>

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_stdlib )
	DEFINE_MODULE_INIT( Kernel_stdlib )
	
	
	static char* getenv( const char* name )
	{
		SystemCallFrame frame( "getenv" );
		
		return CurrentProcess().GetEnv( name );
	}
	
	REGISTER_SYSTEM_CALL( getenv );
	
	static int setenv( const char* name, const char* value, int overwrite )
	{
		SystemCallFrame frame( "setenv" );
		
		CurrentProcess().SetEnv( name, value, overwrite );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( setenv );
	
	static int putenv( const char* string )
	{
		SystemCallFrame frame( "putenv" );
		
		CurrentProcess().PutEnv( string );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( putenv );
	
	static void unsetenv( const char* name )
	{
		SystemCallFrame frame( "unsetenv" );
		
		CurrentProcess().UnsetEnv( name );
	}
	
	REGISTER_SYSTEM_CALL( unsetenv );
	
	static int clearenv()
	{
		SystemCallFrame frame( "clearenv" );
		
		CurrentProcess().ClearEnv();
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( clearenv );
	
}

