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
			
			virtual int Invoke( int argc, iota::argv_t argv ) = 0;
	};
	
}

#endif

