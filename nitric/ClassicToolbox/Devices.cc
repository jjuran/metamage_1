/*	==========
 *	Devices.cc
 *	==========
 */

#include "ClassicToolbox/Devices.hh"

#include "Nitrogen/Quickdraw.hh"

// nucleus
#include "nucleus/saved.hh"


namespace Nitrogen
{
	
	nucleus::owned< DriverRefNum > OpenDriver( ConstStr255Param name )
	{
		SInt16 result;
		ThrowOSStatus( ::OpenDriver( name, &result ) );
		
		return nucleus::owned< DriverRefNum >::seize( DriverRefNum( result ) );
	}
	
	void CloseDriver( nucleus::owned< DriverRefNum > driverRefNum )
	{
		ThrowOSStatus( ::CloseDriver( driverRefNum.release() ) );
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
		nucleus::saved< Port > savedPort;

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

