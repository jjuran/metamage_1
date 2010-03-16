// Nitrogen/Sound.cc
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/Sound.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( SoundManager )
	
	
	static void RegisterSoundManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class SoundManagerErrorsRegistration
	{
		public:
			SoundManagerErrorsRegistration()  { RegisterSoundManagerErrors(); }
	};
	
	static SoundManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	SPBGetIndexedDevice_Result SPBGetIndexedDevice( std::size_t count )
	{
		SPB_IndexedDevice  device;
		::Handle           deviceIconHandle;
		
		ThrowOSStatus( ::SPBGetIndexedDevice( count,
		                                      device.deviceName,
		                                      &deviceIconHandle ) );
		
		device.deviceIconHandle = Handle( deviceIconHandle );
		
		return SPBGetIndexedDevice_Result::seize( device );
	}
	
	Str255 SPBGetIndexedDevice_Name( std::size_t index )
	{
		Str255    result;
		::Handle  deviceIconHandle;
		
		ThrowOSStatus( ::SPBGetIndexedDevice( index,
		                                      result,
		                                      &deviceIconHandle ) );
		
		::DisposeHandle( deviceIconHandle );
		
		return result;
	}
	
	nucleus::owned< SoundInputRefNum > SPBOpenDevice( ConstStr255Param       deviceName,
	                                                  SoundInputPermissions  permission )
	{
		long refNum;
		ThrowOSStatus( ::SPBOpenDevice( deviceName, permission, &refNum ) );
		
		return nucleus::owned< SoundInputRefNum >::seize( SoundInputRefNum( refNum ) );
	}
	
	void SPBCloseDevice( nucleus::owned< SoundInputRefNum > refNum )
	{
		ThrowOSStatus( ::SPBCloseDevice( refNum.release() ) );
	}
	
	void SPBGetDeviceInfo( SoundInputRefNum          refNum,
	                       SoundInputDeviceInfoType  infoType,
	                       void*                     infoData )
	{
		ThrowOSStatus( ::SPBGetDeviceInfo( refNum, infoType, infoData ) );
	}
	
	void SPBSetDeviceInfo( SoundInputRefNum          refNum,
	                       SoundInputDeviceInfoType  infoType,
	                       void*                     infoData )
	{
		ThrowOSStatus( ::SPBSetDeviceInfo( refNum, infoType, infoData ) );
	}
	
	void RegisterSoundManagerErrors()
	{
		RegisterOSStatus< noHardware                   >();
		RegisterOSStatus< notEnoughHardware            >();
		RegisterOSStatus< queueFull                    >();
		RegisterOSStatus< resProblem                   >();
		RegisterOSStatus< badChannel                   >();
		RegisterOSStatus< badFormat                    >();
		RegisterOSStatus< notEnoughBufferSpace         >();
		RegisterOSStatus< badFileFormat                >();
		RegisterOSStatus< channelBusy                  >();
		RegisterOSStatus< buffersTooSmall              >();
		RegisterOSStatus< channelNotBusy               >();
		RegisterOSStatus< noMoreRealTime               >();
		RegisterOSStatus< siVBRCompressionNotSupported >();
		RegisterOSStatus< siNoSoundInHardware          >();
		RegisterOSStatus< siBadSoundInDevice           >();
		RegisterOSStatus< siNoBufferSpecified          >();
		RegisterOSStatus< siInvalidCompression         >();
		RegisterOSStatus< siHardDriveTooSlow           >();
		RegisterOSStatus< siInvalidSampleRate          >();
		RegisterOSStatus< siInvalidSampleSize          >();
		RegisterOSStatus< siDeviceBusyErr              >();
		RegisterOSStatus< siBadDeviceName              >();
		RegisterOSStatus< siBadRefNum                  >();
		RegisterOSStatus< siInputDeviceErr             >();
		RegisterOSStatus< siUnknownInfoType            >();
		RegisterOSStatus< siUnknownQuality             >();
	}
	
}

