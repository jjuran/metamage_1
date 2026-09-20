/*
	terminate.cc
	------------
*/

#include "terminate.hh"

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#ifndef __SHUTDOWN__
#include <ShutDown.h>
#endif

// mac-glue-utils
#include "mac_glue/Gestalt.hh"


static inline
bool is_loader_present()
{
	StringPtr app_name;
	
	return TARGET_CPU_68K  &&
	       (app_name = (StringPtr) mac::glue::gestalt( 'Load' ))  &&
	       *app_name != '\0';
}

void terminate( bool quitting )
{
	ShowCursor();
	
	if ( TARGET_CPU_68K  &&  quitting  &&  is_loader_present() )
	{
		ShutDwnPower();
	}
	
	ExitToShell();
}
