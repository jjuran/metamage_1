/*	=====================
 *	ShareOpenTransport.cc
 *	=====================
 */

#include "Genie/Utilities/ShareOpenTransport.hh"

// ClassicToolbox
#include "ClassicToolbox/OpenTransport.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static unsigned OpenTransport_refcount = 0;
	
	
	void InitOpenTransport_Shared()
	{
		if ( OpenTransport_refcount == 0 )
		{
			N::InitOpenTransport();
		}
		
		++OpenTransport_refcount;
	}
	
	void CloseOpenTransport_Shared()
	{
		if ( --OpenTransport_refcount == 0 )
		{
			N::CloseOpenTransport();
		}
	}
	
}

