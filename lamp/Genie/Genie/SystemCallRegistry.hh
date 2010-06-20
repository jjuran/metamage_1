/*	=====================
 *	SystemCallRegistry.hh
 *	=====================
 */

#ifndef GENIE_SYSTEMCALLREGISTRY_HH
#define GENIE_SYSTEMCALLREGISTRY_HH

// Lamp
#include "lamp/syscalls.h"


namespace Genie
{
	
#ifdef __MC68K__
	
	typedef unsigned short syscall_number_t;
	
#else
	
	typedef unsigned syscall_number_t;
	
#endif
	
	// Register system calls
	
	#define REGISTER_SYSTEM_CALL(call)  \
		SystemCallRegistration call##_syscall_( __NR_##call, #call, (void*) call )
	
	void RegisterSystemCall( syscall_number_t index, const char* name, void* func );
	
	class SystemCallRegistration
	{
		public:
			SystemCallRegistration( syscall_number_t index, const char* name, void* func )
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
	
	extern syscall_number_t gLastSystemCall;
	
	const SystemCall* GetSystemCall( syscall_number_t index );
	
	const SystemCall* LookUpSystemCallByName( const char* name );
	
}

#endif

