// Nitrogen/Sound.cp
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/Sound.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"


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
		SPBGetIndexedDevice_Result  result;
		::Handle                    deviceIconHandle;
		
		ThrowOSStatus( ::SPBGetIndexedDevice( count,
		                                      result.deviceName,
		                                      &deviceIconHandle ) );
		
		result.deviceIconHandle = Nucleus::Owned< Handle >::Seize( deviceIconHandle );
		
		return result;
	}
	
	Nucleus::Owned< SoundInputRefNum > SPBOpenDevice( ConstStr255Param       deviceName,
	                                                  SoundInputPermissions  permission )
	{
		long refNum;
		ThrowOSStatus( ::SPBOpenDevice( deviceName, permission, &refNum ) );
		
		return Nucleus::Owned< SoundInputRefNum >::Seize( SoundInputRefNum( refNum ) );
	}
	
	void SPBCloseDevice( Nucleus::Owned< SoundInputRefNum > refNum )
	{
		ThrowOSStatus( ::SPBCloseDevice( refNum.Release() ) );
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
	
	namespace CompileTests
	{
		
		SoundInputDevice_Container::const_iterator Bar();
		
		static void Foo()
		{
			typedef SoundInputDevice_Container::const_iterator const_iterator;
			
			SPBGetIndexedDevice_Result result = SPBGetIndexedDevice( 1 );
			
			result = SPBGetIndexedDevice( 2 );
			
			const_iterator one;
			
			const_iterator two = one;
			
			const_iterator three( const_iterator() );
			
			SoundInputDevices().begin();
			
			const_iterator::Transfer( Bar() );
			
			one = const_iterator::Transfer( SoundInputDevices().begin() );
			one = SoundInputDevices().begin();
			
			const_iterator four = SoundInputDevices().begin();
		}
		
	}
	
}

