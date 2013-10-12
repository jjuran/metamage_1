/*
	current_process.cc
	------------------
*/

#include "Genie/current_process.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// relix-kernel
#include "relix/api/current_thread.hh"
#include "relix/api/errno.hh"
#include "relix/signal/caught_signal.hh"

// Genie
#include "Genie/Faults.hh"
#include "Genie/Process.hh"


namespace relix
{
	
	thread& current_thread()
	{
		return Genie::current_process();
	}
	
	int set_errno( int errnum )
	{
		return Genie::gCurrentProcess->SetErrno( errnum );
	}
	
}

namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static p7::errno_t GetErrnoFromException()
	{
		try
		{
			throw;
		}
		catch ( const p7::errno_t& errnum )
		{
			return errnum;
		}
		catch ( const relix::caught_signal& signal )
		{
			relix::the_caught_signal = signal;
			
			return EINTR;
		}
		catch ( const Mac::OSStatus& err )
		{
			return OSErrno::ErrnoFromOSStatus( err );
		}
		catch ( ... )
		{
		}
		
		return EINVAL;
	}
	
	
	int set_errno( int errnum )
	{
		return gCurrentProcess->SetErrno( errnum );
	}
	
	int set_errno_from_exception()
	{
		return gCurrentProcess->SetErrno( GetErrnoFromException() );
	}
	
}

