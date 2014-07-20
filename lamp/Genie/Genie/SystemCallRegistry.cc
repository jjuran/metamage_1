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
	
	typedef std::vector< SystemCall > SystemCallRegistry;
	
	SystemCall* gSystemCallArray = NULL;
	
	syscall_number_t gLastSystemCall = 0;
	
	
	SystemCall::SystemCall() : function( (void*) unimplemented ), name()
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
		
		if ( index >= gLastSystemCall )
		{
			gLastSystemCall = index + 1;
			
			const unsigned required_size = gLastSystemCall + 1;
			
			registry.resize( required_size );
			
			gSystemCallArray = &registry[0];
		}
		
		registry[ index ] = SystemCall( func, name );
	}
	
}

namespace Genie
{
	
	using namespace relix;
	
	
	const SystemCall* LookUpSystemCallByName( const char* name )
	{
		SystemCall* it  = gSystemCallArray;
		SystemCall* end = it + gLastSystemCall;
		
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
