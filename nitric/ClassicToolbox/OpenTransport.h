/*	===============
 *	OpenTransport.h
 *	===============
 */

#ifndef CLASSICTOOLBOX_OPENTRANSPORT_H
#define CLASSICTOOLBOX_OPENTRANSPORT_H

// Nostalgia
#include "Nostalgia/OpenTransport.hh"

// Nitrogen
#include "Nitrogen/OpenTransport.h"


namespace Nitrogen
{
	
	inline void InitOpenTransport()
	{
		ThrowOSStatus( ::InitOpenTransport() );
	}
	
	using ::CloseOpenTransport;
	
	Nucleus::Owned< EndpointRef >
	//
	OTOpenEndpoint( Nucleus::Owned< OTConfigurationRef >  config,
	                TEndpointInfo*                        info = NULL );
	
}

#endif

