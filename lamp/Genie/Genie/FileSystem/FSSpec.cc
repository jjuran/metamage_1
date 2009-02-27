/*	=========
 *	FSSpec.cc
 *	=========
 */

#include "Genie/FileSystem/FSSpec.hh"

// Genie
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
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
		CInfoPBRec cInfo;
		
		FSpGetCatInfo< FNF_Throws >( cInfo, dir );
		
		return Dir_From_CInfo( cInfo );
	}
	
}

