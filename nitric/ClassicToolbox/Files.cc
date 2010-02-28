/*	========
 *	Files.cc
 *	========
 */

#include "ClassicToolbox/Files.hh"


namespace Nitrogen
{
	
	void Eject( ConstStr63Param volName )
	{
		ThrowOSStatus( ::Eject( volName, 0 ) );
	}
	
	void Eject( FSVolumeRefNum vRefNum )
	{
		ThrowOSStatus( ::Eject( NULL, vRefNum ) );
	}
	
	void MountVol( FSVolumeRefNum driveNumber )
	{
		ParamBlockRec pb = { 0 };
		
		pb.volumeParam.ioVRefNum = driveNumber;
		
		ThrowOSStatus( ::PBMountVol( &pb ) );
	}
	
}

