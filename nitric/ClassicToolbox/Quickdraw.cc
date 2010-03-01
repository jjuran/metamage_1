/*	============
 *	Quickdraw.cc
 *	============
 */

#include "ClassicToolbox/Quickdraw.hh"


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
	
	nucleus::owned< GrafPtr > OpenPort( GrafPtr port )
	{
		::OpenPort( port );
		
		return nucleus::owned< GrafPtr >::seize( port, &Function::ClosePort );
	}
	
	nucleus::owned< CGrafPtr > OpenCPort( CGrafPtr port )
	{
		::OpenCPort( port );
		
		return nucleus::owned< CGrafPtr >::seize( port, &Function::CloseCPort );
	}
	
}

