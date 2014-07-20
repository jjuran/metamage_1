/*	=========
 *	reexec.cc
 *	=========
 */

// Debug
#include "debug/assert.hh"

// relix
#include "relix/syscall/registry.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	static int reexec( Reexec_Function f, void* _1,
	                                      void* _2,
	                                      void* _3,
	                                      void* _4,
	                                      void* _5,
	                                      void* _6,
	                                      void* _7 )
	{
		try
		{
			Process& current = current_process();
			
			// Start a new thread with the child's process context
			current.Reexec( f, _1, _2, _3, _4, _5, _6, _7 );
			
			// Not reached
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( reexec );
	
	#pragma force_active reset
	
}
