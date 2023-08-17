/*	==========
 *	Devices.cc
 *	==========
 */

#include "ClassicToolbox/Devices.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Nitrogen
{
	
	using Mac::ThrowOSStatus;
	
	
	nucleus::owned< Mac::DriverRefNum > OpenDriver( ConstStr255Param name )
	{
		SInt16 result;
		ThrowOSStatus( ::OpenDriver( name, &result ) );
		
		return nucleus::owned< Mac::DriverRefNum >::seize( Mac::DriverRefNum( result ) );
	}
	
	void CloseDriver( nucleus::owned< Mac::DriverRefNum > driverRefNum )
	{
		ThrowOSStatus( ::CloseDriver( driverRefNum.release() ) );
	}
	
	void KillIO( Mac::DriverRefNum driverRefNum )
	{
		ThrowOSStatus( ::KillIO( driverRefNum ) );
	}
	
	void Control( Mac::DriverRefNum driverRefNum, CSCode csCode, const void* csParamPtr )
	{
		ThrowOSStatus( ::Control( driverRefNum, csCode, csParamPtr ) );
	}
	
	void Status( Mac::DriverRefNum driverRefNum, CSCode csCode, void* csParamPtr )
	{
		ThrowOSStatus( ::Status( driverRefNum, csCode, csParamPtr ) );
	}
	
	void PBControlSync( ParamBlockRec& paramBlock )
	{
		ThrowOSStatus( ::PBControlSync( &paramBlock ) );
	}
	
	int Read( Mac::DriverRefNum driverRefNum, char* data, std::size_t byteCount )
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
	
	int Write( Mac::DriverRefNum driverRefNum, const char* data, std::size_t byteCount )
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
