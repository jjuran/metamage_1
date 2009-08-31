/*	============
 *	Quickdraw.cp
 *	============
 */

#include "ClassicToolbox/Quickdraw.h"


namespace Nitrogen
{
	
	namespace Function
	{
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
		
		static pascal void ClosePort ( GrafPtr  port )  { ::ClosePort ( port ); }
		static pascal void CloseCPort( CGrafPtr port )  { ::CloseCPort( port ); }
		
	#else
		
		using ::ClosePort;
		using ::CloseCPort;
		
	#endif
	}
	
	Nucleus::Owned< GrafPtr > OpenPort( GrafPtr port )
	{
		::OpenPort( port );
		
		return Nucleus::Owned< GrafPtr >::Seize( port, &Function::ClosePort );
	}
	
	Nucleus::Owned< CGrafPtr > OpenCPort( CGrafPtr port )
	{
		::OpenCPort( port );
		
		return Nucleus::Owned< CGrafPtr >::Seize( port, &Function::CloseCPort );
	}
	
}

