/*	=================
 *	MainEntryPoint.hh
 *	=================
 */

#ifndef GENIE_MAINENTRYPOINT_HH
#define GENIE_MAINENTRYPOINT_HH

// Iota
#include "iota/argv.hh"
#include "iota/environ.hh"

// plus
#include "plus/ref_count.hh"


namespace Genie
{
	
	typedef void (*Dispatcher)( ... );
	
	
	class MainEntryPoint : public plus::ref_count< MainEntryPoint >
	{
		public:
			virtual ~MainEntryPoint();
			
			virtual int Invoke( int argc, iota::argv_t argv, iota::envp_t envp, Dispatcher dispatcher ) = 0;
	};
	
}

#endif

