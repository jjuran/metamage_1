/*	==========
 *	Devices.cp
 *	==========
 */

#ifndef __MACH__
#ifndef CLASSICTOOLBOX_DEVICES_H
#include "ClassicToolbox/Devices.h"
#endif
#endif


namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	Owned< DriverRefNum > OpenDriver( ConstStr255Param name )
	{
		OnlyOnce< RegisterDeviceManagerErrors >();
		
		SInt16 result;
		ThrowOSStatus( ::OpenDriver( name, &result ) );
		
		return Owned< DriverRefNum >::Seize( DriverRefNum( result ) );
	}
	
	Owned< DriverRefNum > OpenDriver( const std::string& name )
	{
		return OpenDriver( Str255( name ) );
	}
	
	void CloseDriver( Owned< DriverRefNum > driverRefNum )
	{
		OnlyOnce< RegisterDeviceManagerErrors >();
		
		ThrowOSStatus( ::CloseDriver( driverRefNum.Release() ) );
	}
	
	void KillIO( DriverRefNum driverRefNum )
	{
		OnlyOnce< RegisterDeviceManagerErrors >();
		
		ThrowOSStatus( ::KillIO( driverRefNum ) );
	}
	
	void Control( DriverRefNum driverRefNum, CSCode csCode, const void* csParamPtr )
	{
		OnlyOnce< RegisterDeviceManagerErrors >();
		
		ThrowOSStatus( ::Control( driverRefNum, csCode, csParamPtr ) );
	}
	
	void Status( DriverRefNum driverRefNum, CSCode csCode, void* csParamPtr )
	{
		OnlyOnce< RegisterDeviceManagerErrors >();
		
		ThrowOSStatus( ::Status( driverRefNum, csCode, csParamPtr ) );
	}
	
	int Read( DriverRefNum driverRefNum, char* data, std::size_t byteCount )
	{
		OnlyOnce< RegisterDeviceManagerErrors >();
		
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
		OnlyOnce< RegisterDeviceManagerErrors >();
		
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
	
#endif  // CALL_NOT_IN_CARBON
	
	void RegisterDeviceManagerErrors()
	{
		RegisterOSStatus< controlErr   >();
		RegisterOSStatus< statusErr    >();
		RegisterOSStatus< readErr      >();
		RegisterOSStatus< writErr      >();
		RegisterOSStatus< badUnitErr   >();
		RegisterOSStatus< unitEmptyErr >();
		RegisterOSStatus< openErr      >();
		RegisterOSStatus< closErr      >();
		RegisterOSStatus< dRemovErr    >();
		RegisterOSStatus< dInstErr     >();
		RegisterOSStatus< abortErr     >();
		RegisterOSStatus< notOpenErr   >();
		RegisterOSStatus< ioErr        >();
	}
	
}

