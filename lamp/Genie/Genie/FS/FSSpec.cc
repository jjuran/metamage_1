/*	=========
 *	FSSpec.cc
 *	=========
 */

#include "Genie/FS/FSSpec.hh"

// Nitrogen
#include "Mac/Files/Types/FSDirID.hh"

#include "Nitrogen/OSStatus.hh"

// MacIO
#include "MacIO/GetCatInfo_Sync.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	bool VolumeIsOnServer( Mac::FSVolumeRefNum vRefNum )
	{
		GetVolParmsInfoBuffer parmsInfo = { 0 };
		
		HParamBlockRec pb = { 0 };
		
		HIOParam& io = pb.ioParam;
		
		io.ioVRefNum  = vRefNum;
		io.ioBuffer   = (char *) &parmsInfo;
		io.ioReqCount = sizeof parmsInfo;
		
		N::ThrowOSStatus( ::PBHGetVolParmsSync( &pb ) );
		
		return parmsInfo.vMServerAdr != 0;
	}
	
	Mac::FSDirSpec Dir_From_CInfo( const CInfoPBRec& cInfo )
	{
		const bool is_dir = cInfo.hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		if ( !is_dir )
		{
			// I wanted a dir but you gave me a file.  You creep.
			N::ThrowOSStatus( errFSNotAFolder );
		}
		
		const Mac::FSVolumeRefNum vRefNum = Mac::FSVolumeRefNum( cInfo.dirInfo.ioVRefNum );
		const Mac::FSDirID        dirID   = Mac::FSDirID       ( cInfo.dirInfo.ioDrDirID );
		
		return n::make< Mac::FSDirSpec >( vRefNum, dirID );
	}
	
	Mac::FSDirSpec Dir_From_FSSpec( const FSSpec& dir )
	{
		CInfoPBRec cInfo = { 0 };
		
		MacIO::GetCatInfo< MacIO::Throw_All >( cInfo, dir );
		
		return Dir_From_CInfo( cInfo );
	}
	
}

