/*	=====================
 *	SystemCallRegistry.hh
 *	=====================
 */

#ifndef GENIE_SYSTEMCALLREGISTRY_HH
#define GENIE_SYSTEMCALLREGISTRY_HH

// Relix
#include "relix/syscalls.h"


namespace relix
{
	
#ifdef __MC68K__
	
	typedef unsigned short syscall_number_t;
	
#else
	
	typedef unsigned syscall_number_t;
	
#endif
	
	// Register system calls
	
	#define REGISTER_SYSTEM_CALL(call)  \
		::relix::SystemCallRegistration call##_syscall_( __NR_##call, #call, (void*) call )
	
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
	
}

namespace Genie
{
	
	using relix::SystemCall;
	using relix::gLastSystemCall;
	using relix::gSystemCallArray;
	
	const SystemCall* LookUpSystemCallByName( const char* name );
	
}

#endif
