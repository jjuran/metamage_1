/*	============
 *	ASRegistry.h
 *	============
 */

#ifndef NITROGEN_ASREGISTRY_H
#define NITROGEN_ASREGISTRY_H

// Mac OS
#ifndef __ASREGISTRY__
#include <ASRegistry.h>
#endif

// Nitrogen
#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif


namespace Nitrogen
{
	
	// Do we ever need the missing value outside an object accessor?
	inline Owned< AEDescList, AETokenDisposer > MissingValue()
	{
		return AECreateToken< typeType >( cMissingValue );
	}
	
}

#endif

