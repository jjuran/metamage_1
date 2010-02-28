/*	===================
 *	CRMSerialDevices.cc
 *	===================
 */

#include "ClassicToolbox/CRMSerialDevices.hh"

// Nitrogen
#include "Nitrogen/MacMemory.hh"


namespace Nitrogen
{
	
	static Nucleus::Owned< StringHandle > NewHandleSys( ConstStr255Param str )
	{
		std::size_t size = 1 + str[0];
		Nucleus::Owned< StringHandle > block = Handle_Cast< unsigned char >( NewHandleSys( size ) );
		::BlockMoveData( str, *block.Get(), size );
		return block;
	}
	
	Nucleus::Owned< CRMSerialPtr > New_CRMSerialRecord( ConstStr255Param inputDriverName,
	                                                    ConstStr255Param outputDriverName,
	                                                    ConstStr255Param portName )
	{
		Nucleus::Owned< CRMSerialPtr > result = Nucleus::Owned< CRMSerialPtr >::Seize
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
	
}

