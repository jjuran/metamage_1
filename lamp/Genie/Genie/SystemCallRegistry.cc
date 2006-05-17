/*	=====================
 *	SystemCallRegistry.cc
 *	=====================
 */

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
#if defined(__MWERKS__) && TARGET_CPU_68K
	
	extern SystemCall setenv_syscall_;
	extern SystemCall SpawnVFork_syscall_;
	
	static bool LinkMyInitCodeYouSonOfABitch()
	{
		if ( &setenv_syscall_ == NULL )
		{
			return false;
		}
		
		if ( &SpawnVFork_syscall_ == NULL )
		{
			return false;
		}
		
		return false;
	}
	
#else
	
	inline bool LinkMyInitCodeYouSonOfABitch()  { return true; }
	
#endif
	
	static SystemCallRegistry& TheSystemCallRegistry()
	{
		static SystemCallRegistry theSystemCallRegistry;
		
		(void)LinkMyInitCodeYouSonOfABitch();
		
		return theSystemCallRegistry;
	}
	
	void RegisterSystemCall( const char* name, void* func )
	{
		TheSystemCallRegistry()[ name ] = func;
	}
	
	SystemCallRegistry::const_iterator SystemCallsBegin()
	{
		return TheSystemCallRegistry().begin();
	}
	
	SystemCallRegistry::const_iterator SystemCallsEnd()
	{
		return TheSystemCallRegistry().end();
	}
	
}
