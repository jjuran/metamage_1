/*
	registry.hh
	-----------
*/

#ifndef RELIX_SYSCALL_REGISTRY_HH
#define RELIX_SYSCALL_REGISTRY_HH

// relix
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
		::relix::system_call_registration call##_syscall_( __NR_##call, #call, (void*) call )
	
	void register_system_call( syscall_number_t index, const char* name, void* func );
	
	class system_call_registration
	{
		public:
			system_call_registration( syscall_number_t index, const char* name, void* func )
			{
				register_system_call( index, name, func );
			}
	};
	
	struct system_call
	{
		void*        function;
		const char*  name;
		
		system_call();
		
		system_call( void* f, const char* nom ) : function( f ), name( nom )  {}
	};
	
	extern system_call* the_syscall_array;
	
	extern syscall_number_t the_last_syscall;
	
}

#endif
