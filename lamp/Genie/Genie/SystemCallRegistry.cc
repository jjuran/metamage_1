/*	=====================
 *	SystemCallRegistry.cc
 *	=====================
 */

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
#if TARGET_CPU_68K && defined(__MWERKS__)
	
	DECLARE_MODULE_INIT( Kernel_dirent )
	DECLARE_MODULE_INIT( Kernel_fcntl  )
	DECLARE_MODULE_INIT( Kernel_Spawn  )
	DECLARE_MODULE_INIT( Kernel_stat   )
	DECLARE_MODULE_INIT( Kernel_stdlib )
	DECLARE_MODULE_INIT( Kernel_wait   )
	
	void InitKernelModules()
	{
		CALL_MODULE_INIT( Kernel_dirent );
		CALL_MODULE_INIT( Kernel_fcntl  );
		CALL_MODULE_INIT( Kernel_Spawn  );
		CALL_MODULE_INIT( Kernel_stat   );
		CALL_MODULE_INIT( Kernel_stdlib );
		CALL_MODULE_INIT( Kernel_wait   );
	}
	
#endif
	
	static SystemCallRegistry& TheSystemCallRegistry()
	{
		static SystemCallRegistry theSystemCallRegistry;
		
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
