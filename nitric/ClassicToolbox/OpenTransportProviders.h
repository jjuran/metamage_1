/*	========================
 *	OpenTransportProviders.h
 *	========================
 */

#ifndef CLASSICTOOLBOX_OPENTRANSPORTPROVIDERS_H
#define CLASSICTOOLBOX_OPENTRANSPORTPROVIDERS_H

// Nostalgia
#include "Nostalgia/OpenTransportProviders.hh"

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"


namespace Nitrogen
{
	
	Nucleus::Owned< InetSvcRef >
	//
	OTOpenInternetServices( Nucleus::Owned< OTConfigurationRef > config );
	
	Nucleus::Owned< InetSvcRef >
	//
	OTOpenInternetServices( DefaultInternetServicesPath /**/ );
	
}

#endif

