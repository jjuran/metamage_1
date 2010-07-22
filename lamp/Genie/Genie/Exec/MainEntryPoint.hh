/*	=================
 *	MainEntryPoint.hh
 *	=================
 */

#ifndef GENIE_MAINENTRYPOINT_HH
#define GENIE_MAINENTRYPOINT_HH

// plus
#include "plus/ref_count.hh"


struct _lamp_system_parameter_block;


namespace Genie
{
	
	class MainEntryPoint : public plus::ref_count< MainEntryPoint >
	{
		public:
			virtual ~MainEntryPoint();
			
			virtual int Invoke( int                            argc,
			                    char**                         argv,
			                    char**                         envp,
			                    _lamp_system_parameter_block*  pb ) = 0;
	};
	
}

#endif

