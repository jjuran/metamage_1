/*	===================
 *	CRMSerialDevices.cc
 *	===================
 */

#include "ClassicToolbox/CRMSerialDevices.hh"

// Nitrogen
#include "Nitrogen/MacMemory.hh"


namespace Nitrogen
{
	
	static nucleus::owned< StringHandle > NewHandleSys( ConstStr255Param str )
	{
		std::size_t size = 1 + str[0];
		nucleus::owned< StringHandle > block = Handle_Cast< unsigned char >( NewHandleSys( size ) );
		::BlockMoveData( str, *block.get(), size );
		return block;
	}
	
	nucleus::owned< CRMSerialPtr > New_CRMSerialRecord( ConstStr255Param inputDriverName,
	                                                    ConstStr255Param outputDriverName,
	                                                    ConstStr255Param portName )
	{
		nucleus::owned< CRMSerialPtr > result = nucleus::owned< CRMSerialPtr >::seize
		(
			Ptr_Cast< CRMSerialRecord >( NewPtrSysClear( sizeof (CRMSerialRecord) ).release() )
		);
		
		CRMSerialPtr crmSerial = result;
		
		crmSerial->inputDriverName  = NewHandleSys( inputDriverName  ).release();
		crmSerial->outputDriverName = NewHandleSys( outputDriverName ).release();
		crmSerial->name             = NewHandleSys( portName         ).release();
		crmSerial->deviceIcon = NULL;
		crmSerial->ratedSpeed = 230400;
		crmSerial->maxSpeed = 230400;
		
		return result;
	}
	
}

