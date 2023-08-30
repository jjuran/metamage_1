/*
	MacCDROM/Device.hh
	------------------
	
	Joshua Juran
*/

#ifndef MACCDROM_DEVICE_HH
#define MACCDROM_DEVICE_HH

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif

// Nitrogen
#ifndef MAC_DEVICES_TYPES_DRIVERREFNUM_HH
#include "Mac/Devices/Types/DriverRefNum.hh"
#endif
#ifndef MAC_FILES_TYPES_FSVOLUMEREFNUM_HH
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#endif


namespace MacCDROM
{
	
	enum CDROMPowerLevel
	{
		kCDROMPowerLevelActive  = 0,
		kCDROMPowerLevelStandby = 1,
		kCDROMPowerLevelIdle    = 2,
		kCDROMPowerLevelSleep   = 3,
		
		kCDROMPowerLevel_Max = nucleus::enumeration_traits< unsigned char >::max
	};
	
	struct CDROMDrive
	{
		Mac::DriverRefNum    dRefNum;
		Mac::FSVolumeRefNum  vRefNum;
		
		CDROMDrive()  {}
		
		CDROMDrive( Mac::DriverRefNum    d,
		            Mac::FSVolumeRefNum  v = Mac::FSVolumeRefNum( 1 ) )  // index
		:
			dRefNum( d ),
			vRefNum( v )
		{
		}
	};
	
#if CALL_NOT_IN_CARBON
	
	#pragma mark -
	#pragma mark ** Open **
	
	Mac::DriverRefNum OpenCDROMDriver();
	
	#pragma mark -
	#pragma mark ** Special CD-ROM Control Calls **
	
	// csCode 70
	void SetPowerMode( const CDROMDrive& drive, CDROMPowerLevel powerLevel );
	
	// csCode 76
	void ModifyPostEvent( const CDROMDrive& drive, bool shouldPostEvent );
	
	// csCode 79
	// ChangeBlockSize
	
	// csCode 80
	void SetUserEject( const CDROMDrive& drive, bool enableManualEject );
	
	// csCode 81
	void SetPollFreq( const CDROMDrive& drive, unsigned short ticks );
	
	// csCode 1079
	void Quiescence( const CDROMDrive& drive, bool enableQuiescence );
	
	#pragma mark -
	#pragma mark ** Supported Status Calls **
	
	// csCode 8
	// DriveStatus
	
	// csCode 43
	// DriverGestalt
	
	// csCode 70
	CDROMPowerLevel GetPowerMode( const CDROMDrive& drive );
	
	// csCode 95
	// Get2KOffset
	
	// csCode 96
	// GetDriveType
	
	// csCode 98
	// GetBlockSize
	
	// csCode 120
	// ReturnDeviceIdent
	
	// csCode 121
	// GetCDFeatures
	
	
#endif
	
}

#endif
