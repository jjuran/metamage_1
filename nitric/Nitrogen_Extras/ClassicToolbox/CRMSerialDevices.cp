/*	===================
 *	CRMSerialDevices.cp
 *	===================
 */

#ifndef CLASSICTOOLBOX_CRMSERIALDEVICES_HH
#include "ClassicToolbox/CRMSerialDevices.h"
#endif

// Nitrogen
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif


namespace Nitrogen
{
	
	#if CALL_NOT_IN_CARBON
	
	Owned< CRMSerialPtr > New_CRMSerialRecord( Owned< StringHandle > inputDriverName,
	                                           Owned< StringHandle > outputDriverName,
	                                           Owned< StringHandle > portName )
	{
		Owned< CRMSerialPtr > result = Owned< CRMSerialPtr >::Seize
		(
			reinterpret_cast< CRMSerialPtr >
			(
				NewPtrSysClear( sizeof (CRMSerialRecord) ).Release().Get()
			)
		);
		
		CRMSerialPtr crmSerial = result;
		
		crmSerial->inputDriverName  = inputDriverName.Release();
		crmSerial->outputDriverName = outputDriverName.Release();
		crmSerial->name             = portName.Release();
		crmSerial->deviceIcon = NULL;
		crmSerial->ratedSpeed = 230400;
		crmSerial->maxSpeed = 230400;
		
		return result;
	}
	
}

#endif  // #if CALL_NOT_IN_CARBON

