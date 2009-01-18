/*	==========
 *	Devices.cp
 *	==========
 */

#include "ClassicToolbox/Devices.h"

#include "Nitrogen/QuickDraw.h"

// Nucleus
#include "Nucleus/Saved.h"


namespace Nitrogen
{
	
	Nucleus::Owned< DriverRefNum > OpenDriver( ConstStr255Param name )
	{
		SInt16 result;
		ThrowOSStatus( ::OpenDriver( name, &result ) );
		
		return Nucleus::Owned< DriverRefNum >::Seize( DriverRefNum( result ) );
	}
	
	Nucleus::Owned< DriverRefNum > OpenDriver( const std::string& name )
	{
		return OpenDriver( Str255( name ) );
	}
	
	void CloseDriver( Nucleus::Owned< DriverRefNum > driverRefNum )
	{
		ThrowOSStatus( ::CloseDriver( driverRefNum.Release() ) );
	}
	
	void KillIO( DriverRefNum driverRefNum )
	{
		ThrowOSStatus( ::KillIO( driverRefNum ) );
	}
	
	void Control( DriverRefNum driverRefNum, CSCode csCode, const void* csParamPtr )
	{
		ThrowOSStatus( ::Control( driverRefNum, csCode, csParamPtr ) );
	}
	
	void Status( DriverRefNum driverRefNum, CSCode csCode, void* csParamPtr )
	{
		ThrowOSStatus( ::Status( driverRefNum, csCode, csParamPtr ) );
	}
	
	void PBControlSync( ParamBlockRec& paramBlock )
	{
		ThrowOSStatus( ::PBControlSync( &paramBlock ) );
	}
	
	void OpenDeskAcc( ConstStr255Param deskAccName )
	{
		Nucleus::Saved< Port_Value > savedPort;

		::OpenDeskAcc( deskAccName );
	}
	
	int Read( DriverRefNum driverRefNum, char* data, std::size_t byteCount )
	{
		ParamBlockRec pb;
		IOParam& io = pb.ioParam;
		
		io.ioRefNum = driverRefNum;
		io.ioBuffer = data;
		io.ioReqCount = byteCount;
		io.ioCompletion = NULL;
		io.ioVRefNum = 0;
		io.ioPosMode = 0;
		
		ThrowOSStatus( ::PBReadSync( &pb ) );
		
		return io.ioActCount;
	}
	
	int Write( DriverRefNum driverRefNum, const char* data, std::size_t byteCount )
	{
		ParamBlockRec pb;
		IOParam& io = pb.ioParam;
		
		io.ioRefNum = driverRefNum;
		io.ioBuffer = const_cast< char* >( data );
		io.ioReqCount = byteCount;
		io.ioCompletion = NULL;
		io.ioVRefNum = 0;
		io.ioPosMode = 0;
		
		ThrowOSStatus( ::PBWriteSync( &pb ) );
		
		return io.ioActCount;
	}
	
}

