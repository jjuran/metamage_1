// Sound.cp

#ifndef NITROGEN_SOUND_H
#include "Nitrogen/Sound.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen {
	
	void RegisterSoundManagerErrors()
	{
		RegisterOSStatus< siBadSoundInDevice >();
	}
	
	SPBGetIndexedDevice_Result SPBGetIndexedDevice( std::size_t count )
	{
		OnlyOnce< RegisterSoundManagerErrors >();
		
		SPBGetIndexedDevice_Result result;
		::Handle deviceIconHandle;
		
		ThrowOSStatus(
			::SPBGetIndexedDevice( count, result.deviceName, &deviceIconHandle )
		);
		result.deviceIconHandle = Owned< Handle >::Seize( deviceIconHandle );
		
		return result;
	}
	
	Owned< SoundInputRefNum > SPBOpenDevice( ConstStr255Param deviceName, SoundInputPermissions permission )
	{
		OnlyOnce< RegisterSoundManagerErrors >();
		
		long refNum;
		ThrowOSStatus( ::SPBOpenDevice( deviceName, permission, &refNum ) );
		return Owned< SoundInputRefNum >::Seize( SoundInputRefNum( refNum ) );
	}
	
	void SPBCloseDevice( Owned< SoundInputRefNum > refNum )
	{
		OnlyOnce< RegisterSoundManagerErrors >();
		
		ThrowOSStatus( ::SPBCloseDevice( refNum.Release() ) );
	}
	
	void SPBGetDeviceInfo( SoundInputRefNum refNum, SoundInputDeviceInfoType infoType, void* infoData )
	{
		OnlyOnce< RegisterSoundManagerErrors >();
		
		ThrowOSStatus( ::SPBGetDeviceInfo( refNum, infoType, infoData ) );
	}
	
	void SPBSetDeviceInfo( SoundInputRefNum refNum, SoundInputDeviceInfoType infoType, void* infoData )
	{
		OnlyOnce< RegisterSoundManagerErrors >();
		
		ThrowOSStatus( ::SPBGetDeviceInfo( refNum, infoType, infoData ) );
	}
	
}

