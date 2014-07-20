/*	=====================
 *	SystemCallRegistry.cc
 *	=====================
 */

#include "Genie/SystemCallRegistry.hh"

// Standard C
#include <string.h>

// Standard C++
#include <vector>

// relix-kernel
#include "relix/syscall/unimplemented.hh"


namespace relix
{
	
	typedef std::vector< system_call > SystemCallRegistry;
	
	system_call* the_syscall_array = NULL;
	
	syscall_number_t the_last_syscall = 0;
	
	
	system_call::system_call() : function( (void*) unimplemented ), name()
	{
		
	}
	
	static SystemCallRegistry& TheSystemCallRegistry()
	{
		static SystemCallRegistry theSystemCallRegistry( 1 );
		
		return theSystemCallRegistry;
	}
	
	void RegisterSystemCall( syscall_number_t index, const char* name, void* func )
	{
		SystemCallRegistry& registry = TheSystemCallRegistry();
		
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

namespace Genie
{
	
	using namespace relix;
	
	
	const SystemCall* LookUpSystemCallByName( const char* name )
	{
		SystemCall* it  = the_syscall_array;
		SystemCall* end = it + the_last_syscall;
		
		while ( it < end )
		{
			if ( strcmp( name, it->name ) == 0 )
			{
				return it;
			}
		}
		
		return NULL;
	}
	
}
