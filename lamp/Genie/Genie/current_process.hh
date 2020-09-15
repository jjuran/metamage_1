/*
	current_process.hh
	------------------
*/

#ifndef GENIE_CURRENTPROCESS_HH
#define GENIE_CURRENTPROCESS_HH

// relix-kernel
#include "relix/api/errno.hh"


namespace Genie
{
	
	class Process;
	
}

namespace relix
{
	
	extern Genie::Process* gCurrentProcess;
	
}

namespace Genie
{
	
	inline Process& current_process()  { return *relix::gCurrentProcess; }
	
	using relix::set_errno;
	using relix::set_errno_from_exception;
	
}

#endif
