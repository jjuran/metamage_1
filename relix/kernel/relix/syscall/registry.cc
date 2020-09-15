/*
	registry.cc
	-----------
*/

#include "relix/syscall/registry.hh"

// Standard C++
#include <vector>

// relix-kernel
#include "relix/syscall/unimplemented.hh"


namespace relix
{
	
	typedef std::vector< system_call > system_call_registry;
	
	system_call* the_syscall_array = 0;  // NULL
	
	syscall_number_t the_last_syscall = 0;
	
	
	system_call::system_call() : function( (void*) relix::unimplemented ), name()
	{
		
	}
	
	static system_call_registry& get_system_call_registry()
	{
		static system_call_registry the_system_call_registry( 1 );
		
		return the_system_call_registry;
	}
	
	void register_system_call( syscall_number_t index, const char* name, void* func )
	{
		system_call_registry& registry = get_system_call_registry();
		
		if ( index >= the_last_syscall )
		{
			the_last_syscall = index + 1;
			
			const unsigned required_size = the_last_syscall + 1;
			
			registry.resize( required_size );
			
			the_syscall_array = &registry[0];
		}
		
		registry[ index ] = system_call( func, name );
	}
	
}
