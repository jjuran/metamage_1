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
	
	static Owned< StringHandle > NewHandleSys( ConstStr255Param str )
	{
		std::size_t size = 1 + str[0];
		Owned< StringHandle > block = Handle_Cast< unsigned char >( NewHandleSys( size ) );
		::BlockMoveData( str, *block.Get(), size );
		return block;
	}
	
	Owned< CRMSerialPtr > New_CRMSerialRecord( ConstStr255Param inputDriverName,
	                                           ConstStr255Param outputDriverName,
	                                           ConstStr255Param portName )
	{
		Owned< CRMSerialPtr > result = Owned< CRMSerialPtr >::Seize
		(
			Ptr_Cast< CRMSerialRecord >( NewPtrSysClear( sizeof (CRMSerialRecord) ).Release() )
		);
		
		CRMSerialPtr crmSerial = result;
		
		crmSerial->inputDriverName  = NewHandleSys( inputDriverName  ).Release();
		crmSerial->outputDriverName = NewHandleSys( outputDriverName ).Release();
		crmSerial->name             = NewHandleSys( portName         ).Release();
		crmSerial->deviceIcon = NULL;
		crmSerial->ratedSpeed = 230400;
		crmSerial->maxSpeed = 230400;
		
		return result;
	}
	
#endif  // #if CALL_NOT_IN_CARBON
	
}

