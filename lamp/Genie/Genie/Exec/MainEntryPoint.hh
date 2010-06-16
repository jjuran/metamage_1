/*	=================
 *	MainEntryPoint.hh
 *	=================
 */

#ifndef GENIE_MAINENTRYPOINT_HH
#define GENIE_MAINENTRYPOINT_HH

// plus
#include "plus/ref_count.hh"


namespace Genie
{
	
	typedef void (*Dispatcher)( ... );
	
	
	class MainEntryPoint : public plus::ref_count< MainEntryPoint >
	{
		public:
			virtual ~MainEntryPoint();
			
			virtual int Invoke( int argc, char** argv, char** envp, Dispatcher dispatcher ) = 0;
	};
	
}

#endif

