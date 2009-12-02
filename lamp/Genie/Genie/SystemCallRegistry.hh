/*	=====================
 *	SystemCallRegistry.hh
 *	=====================
 */

#ifndef GENIE_SYSTEMCALLREGISTRY_HH
#define GENIE_SYSTEMCALLREGISTRY_HH

// Standard C++
#include <vector>

// Lamp
#include "lamp/syscalls.h"


namespace Genie
{
	
	// Register system calls
	
	#define REGISTER_SYSTEM_CALL(call)  \
		SystemCallRegistration call##_syscall_( __NR_##call, #call, (void*) call )
	
	void RegisterSystemCall( unsigned index, const char* name, void* func );
	
	class SystemCallRegistration
	{
		public:
			SystemCallRegistration( unsigned index, const char* name, void* func )
			{
				RegisterSystemCall( index, name, func );
			}
	};
	
	struct SystemCall
	{
		void*        function;
		const char*  name;
		
		SystemCall();
		
		SystemCall( void* f, const char* nom ) : function( f ), name( nom )  {}
	};
	
	extern SystemCall* gSystemCallArray;
	
	extern std::size_t gLastSystemCall;
	
	typedef std::vector< SystemCall > SystemCallRegistry;
	
	const SystemCallRegistry& GetSystemCallRegistry();
	
	const SystemCall* GetSystemCall( unsigned index );
	
	const SystemCall* LookUpSystemCallByName( const char* name );
	
}

#endif

