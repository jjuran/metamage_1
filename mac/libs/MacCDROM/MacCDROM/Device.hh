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
		short vRefNum;
		short dRefNum;
		
		CDROMDrive()  {}
		
		CDROMDrive( short d, short v = 1 )  // volume index starts at 1
		:
			vRefNum( v ),
			dRefNum( d )
		{
		}
	};
	
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
	
}

#endif
