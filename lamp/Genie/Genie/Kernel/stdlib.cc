/*	=========
 *	stdlib.cc
 *	=========
 */

// Standard C
#include <errno.h>

// POSIX
#include "stdlib.h"

// POSeven
//#include "POSeven/Errno.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	//namespace P7 = POSeven;
	
	
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

#pragma export on
	
	char* getenv( const char* name )
	{
		return Genie::getenv( name );
	}
	
	int setenv( const char* name, const char* value, int overwrite )
	{
		return Genie::setenv( name, value, overwrite );
	}
	
	int putenv( const char* string )
	{
		return Genie::putenv( string );
	}
	
	void unsetenv( const char* name )
	{
		return Genie::unsetenv( name );
	}
	
	int clearenv()
	{
		return Genie::clearenv();
	}
	
#pragma export reset

