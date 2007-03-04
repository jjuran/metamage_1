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
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_stdlib )
	DEFINE_MODULE_INIT( Kernel_stdlib )
	
	
	static char* getenv( const char* name )
	{
		return CurrentProcess().GetEnv( name );
	}
	
	REGISTER_SYSTEM_CALL( getenv );
	
	static int setenv( const char* name, const char* value, int overwrite )
	{
		CurrentProcess().SetEnv( name, value, overwrite );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( setenv );
	
	static int putenv( const char* string )
	{
		CurrentProcess().PutEnv( string );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( putenv );
	
	static void unsetenv( const char* name )
	{
		CurrentProcess().UnsetEnv( name );
	}
	
	REGISTER_SYSTEM_CALL( unsetenv );
	
	static int clearenv()
	{
		CurrentProcess().ClearEnv();
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( clearenv );
	
}

