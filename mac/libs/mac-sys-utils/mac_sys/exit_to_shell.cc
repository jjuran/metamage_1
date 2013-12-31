/*
	exit_to_shell.cc
	----------------
*/

#include "mac_sys/exit_to_shell.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

#ifndef __PROCESSES__
#include <Processes.h>
#endif


namespace mac {
namespace sys {
	
	void exit_to_shell()
	{
		::ExitToShell();
	}
	
}
}

