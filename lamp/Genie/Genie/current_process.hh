/*
	current_process.hh
	------------------
*/

#ifndef GENIE_CURRENTPROCESS_HH
#define GENIE_CURRENTPROCESS_HH


namespace Genie
{
	
	class Process;
	
	extern Process* gCurrentProcess;
	
	
	inline Process& current_process()  { return *gCurrentProcess; }
	
	int set_errno( int errnum );
	
	int set_errno_from_exception();
	
}

#endif

