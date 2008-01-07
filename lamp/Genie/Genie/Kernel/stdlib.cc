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


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_stdlib )
	DEFINE_MODULE_INIT( Kernel_stdlib )
	
	
	static char* getenv( const char* name )
	{
		SystemCallFrame frame( "getenv" );
		
		return frame.Caller().GetEnv( name );
	}
	
	REGISTER_SYSTEM_CALL( getenv );
	
	static int setenv( const char* name, const char* value, int overwrite )
	{
		SystemCallFrame frame( "setenv" );
		
		frame.Caller().SetEnv( name, value, overwrite );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( setenv );
	
	static int putenv( const char* string )
	{
		SystemCallFrame frame( "putenv" );
		
		frame.Caller().PutEnv( string );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( putenv );
	
	static void unsetenv( const char* name )
	{
		SystemCallFrame frame( "unsetenv" );
		
		frame.Caller().UnsetEnv( name );
	}
	
	REGISTER_SYSTEM_CALL( unsetenv );
	
	static int clearenv()
	{
		SystemCallFrame frame( "clearenv" );
		
		frame.Caller().ClearEnv();
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( clearenv );
	
}

