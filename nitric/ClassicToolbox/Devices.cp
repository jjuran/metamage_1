/*	==========
 *	Devices.cp
 *	==========
 */

#ifndef __MACH__
#ifndef CLASSICTOOLBOX_DEVICES_H
#include "ClassicToolbox/Devices.h"
#endif
#endif

#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif

// Nucleus
#ifndef NUCLEUS_SAVED_H
#include "Nucleus/Saved.h"
#endif


namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	Nucleus::Owned< DriverRefNum > OpenDriver( ConstStr255Param name )
	{
		Nucleus::OnlyOnce< RegisterDeviceManagerErrors >();
		
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
		Nucleus::OnlyOnce< RegisterDeviceManagerErrors >();
		
		ThrowOSStatus( ::CloseDriver( driverRefNum.Release() ) );
	}
	
	void KillIO( DriverRefNum driverRefNum )
	{
		Nucleus::OnlyOnce< RegisterDeviceManagerErrors >();
		
		ThrowOSStatus( ::KillIO( driverRefNum ) );
	}
	
	void Control( DriverRefNum driverRefNum, CSCode csCode, const void* csParamPtr )
	{
		Nucleus::OnlyOnce< RegisterDeviceManagerErrors >();
		
		ThrowOSStatus( ::Control( driverRefNum, csCode, csParamPtr ) );
	}
	
	void Status( DriverRefNum driverRefNum, CSCode csCode, void* csParamPtr )
	{
		Nucleus::OnlyOnce< RegisterDeviceManagerErrors >();
		
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
		Nucleus::OnlyOnce< RegisterDeviceManagerErrors >();
		
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
		Nucleus::OnlyOnce< RegisterDeviceManagerErrors >();
		
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

