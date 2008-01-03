/*	=====================
 *	SystemCallRegistry.cc
 *	=====================
 */

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
#if TARGET_CPU_68K && defined(__MWERKS__)
	
	DECLARE_MODULE_INIT( Kernel_copyfile )
	DECLARE_MODULE_INIT( Kernel_dirent   )
	DECLARE_MODULE_INIT( Kernel_fcntl    )
	DECLARE_MODULE_INIT( Kernel_ioctl    )
	DECLARE_MODULE_INIT( Kernel_mkdir    )
	DECLARE_MODULE_INIT( Kernel_netdb    )
	DECLARE_MODULE_INIT( Kernel_realpath )
	DECLARE_MODULE_INIT( Kernel_rename   )
	DECLARE_MODULE_INIT( Kernel_rmdir    )
	DECLARE_MODULE_INIT( Kernel_select   )
	DECLARE_MODULE_INIT( Kernel_signal   )
	DECLARE_MODULE_INIT( Kernel_socket   )
	DECLARE_MODULE_INIT( Kernel_Spawn    )
	DECLARE_MODULE_INIT( Kernel_stat     )
	DECLARE_MODULE_INIT( Kernel_stdlib   )
	DECLARE_MODULE_INIT( Kernel_symlink  )
	DECLARE_MODULE_INIT( Kernel_time     )
	DECLARE_MODULE_INIT( Kernel_times    )
	DECLARE_MODULE_INIT( Kernel_uname    )
	DECLARE_MODULE_INIT( Kernel_unlink   )
	DECLARE_MODULE_INIT( Kernel_wait     )
	
	void InitKernelModules()
	{
		CALL_MODULE_INIT( Kernel_copyfile );
		CALL_MODULE_INIT( Kernel_dirent   );
		CALL_MODULE_INIT( Kernel_fcntl    );
		CALL_MODULE_INIT( Kernel_ioctl    );
		CALL_MODULE_INIT( Kernel_mkdir    );
		CALL_MODULE_INIT( Kernel_netdb    );
		CALL_MODULE_INIT( Kernel_realpath );
		CALL_MODULE_INIT( Kernel_rename   );
		CALL_MODULE_INIT( Kernel_rmdir    );
		CALL_MODULE_INIT( Kernel_select   );
		CALL_MODULE_INIT( Kernel_signal   );
		CALL_MODULE_INIT( Kernel_socket   );
		CALL_MODULE_INIT( Kernel_Spawn    );
		CALL_MODULE_INIT( Kernel_stat     );
		CALL_MODULE_INIT( Kernel_stdlib   );
		CALL_MODULE_INIT( Kernel_symlink  );
		CALL_MODULE_INIT( Kernel_time     );
		CALL_MODULE_INIT( Kernel_times    );
		CALL_MODULE_INIT( Kernel_uname    );
		CALL_MODULE_INIT( Kernel_unlink   );
		CALL_MODULE_INIT( Kernel_wait     );
	}
	
#endif
	
	static bool operator<( const SystemCall& a, const SystemCall& b )
	{
		return std::strcmp( a.name, b.name ) < 0;
	}
	
	static bool operator==( const SystemCall& systemCall, const char* name )
	{
		return std::strcmp( systemCall.name, name ) == 0;
	}
	
	static SystemCallRegistry& TheSystemCallRegistry()
	{
		static SystemCallRegistry theSystemCallRegistry( 1, SystemCall( "", NULL ) );
		
		return theSystemCallRegistry;
	}
	
	void RegisterSystemCall( unsigned index, const char* name, void* func )
	{
		const unsigned required_size = index + 1;
		
		SystemCallRegistry& registry = TheSystemCallRegistry();
		
		if ( registry.size() < required_size )
		{
			registry.resize( required_size );
		}
		
		registry[ index ] = SystemCall( name, func );
	}
	
	const SystemCall* GetSystemCall( unsigned index )
	{
		const unsigned required_size = index + 1;
		
		SystemCallRegistry& registry = TheSystemCallRegistry();
		
		if ( registry.size() < required_size )
		{
			return NULL;
		}
		
		return &registry[ index ];
	}
	
	SystemCallRegistry::const_iterator SystemCallsBegin()
	{
		return TheSystemCallRegistry().begin();
	}
	
	SystemCallRegistry::const_iterator SystemCallsEnd()
	{
		return TheSystemCallRegistry().end();
	}
	
	unsigned LookUpSystemCallIndex( const char* name )
	{
		static SystemCallRegistry::iterator begin = TheSystemCallRegistry().begin();
		static SystemCallRegistry::iterator end   = TheSystemCallRegistry().end  ();
		
		SystemCallRegistry::const_iterator it = std::find( begin, end, name );
		
		if ( it == end )
		{
			return 0;
		}
		
		return it - begin;
	}
	
}
