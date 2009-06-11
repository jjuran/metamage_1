/*	=========
 *	FSSpec.cc
 *	=========
 */

#include "Genie/FS/FSSpec.hh"

// Genie
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	bool VolumeIsOnServer( N::FSVolumeRefNum vRefNum )
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
	
	N::FSDirSpec Dir_From_CInfo( const CInfoPBRec& cInfo )
	{
		const bool is_dir = cInfo.hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		if ( !is_dir )
		{
			// I wanted a dir but you gave me a file.  You creep.
			throw N::ErrFSNotAFolder();
		}
		
		const N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( cInfo.dirInfo.ioVRefNum );
		const N::FSDirID        dirID   = N::FSDirID       ( cInfo.dirInfo.ioDrDirID );
		
		return NN::Make< N::FSDirSpec >( vRefNum, dirID );
	}
	
	N::FSDirSpec Dir_From_FSSpec( const FSSpec& dir )
	{
		CInfoPBRec cInfo = { 0 };
		
		FSpGetCatInfo< FNF_Throws >( cInfo, dir );
		
		return Dir_From_CInfo( cInfo );
	}
	
}

