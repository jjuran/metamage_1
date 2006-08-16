/*	=======
 *	CDROM.h
 *	=======
 */

#ifndef CLASSICEXTRAS_CDROM_H
#define CLASSICEXTRAS_CDROM_H

// Nucleus
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif

// Nitrogen Extras / ClassicToolbox
#ifndef CLASSICTOOLBOX_DEVICES_H
#include "ClassicToolbox/Devices.h"
#endif


namespace NitrogenExtras
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	typedef NN::Selector< struct CDROMPowerLevel_Tag, unsigned char >::Type CDROMPowerLevel;
	
	namespace Constants
	{
		
		static const CDROMPowerLevel kCDROMPowerLevelActive  = CDROMPowerLevel( 0 );
		static const CDROMPowerLevel kCDROMPowerLevelStandby = CDROMPowerLevel( 1 );
		static const CDROMPowerLevel kCDROMPowerLevelIdle    = CDROMPowerLevel( 2 );
		static const CDROMPowerLevel kCDROMPowerLevelSleep   = CDROMPowerLevel( 3 );
		
	}
	
	using namespace Constants;
	
	
	struct CDROMDrive
	{
		N::DriverRefNum    dRefNum;
		N::FSVolumeRefNum  vRefNum;
		
		CDROMDrive()  {}
		
		CDROMDrive( N::DriverRefNum    d,
		            N::FSVolumeRefNum  v = 1 ) : dRefNum( d ),
		                                         vRefNum( v )  {}
	};
	
#if CALL_NOT_IN_CARBON
	
	#pragma mark -
	#pragma mark ¥ Open ¥
	
	N::DriverRefNum OpenCDROMDriver();
	
	#pragma mark -
	#pragma mark ¥ Special CD-ROM Control Calls ¥
	
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
	#pragma mark ¥ Supported Status Calls ¥
	
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

