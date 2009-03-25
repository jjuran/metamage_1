/*
	
	Install.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_INSTALL_HH
#define SILVER_INSTALL_HH

// Universal Interfaces
#include <MacTypes.h>


namespace Silver
{
	
	typedef OSErr (*InstallerProcPtr)();
	
	OSErr Install( InstallerProcPtr installer );
	
}

#endif

