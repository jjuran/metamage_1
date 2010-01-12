/*
	
	Install.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_INSTALL_HH
#define SILVER_INSTALL_HH

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


namespace Silver
{
	
	typedef OSErr (*InstallerProcPtr)();
	
	OSErr Install( InstallerProcPtr installer );
	
}

#endif

