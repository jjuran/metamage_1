/*	=================
 *	MainEntryPoint.hh
 *	=================
 */

#ifndef GENIE_MAINENTRYPOINT_HH
#define GENIE_MAINENTRYPOINT_HH

// Iota
#include "iota/argv.hh"
#include "iota/environ.hh"


namespace Genie
{
	
	class MainEntryPoint
	{
		public:
			virtual ~MainEntryPoint();
			
			virtual void Invoke( int argc, iota::argv_t argv, iota::environ_t envp ) = 0;
	};
	
}

#endif

