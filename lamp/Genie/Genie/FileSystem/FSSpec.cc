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
	
	
	N::FSDirSpec Dir_From_FSSpec( const FSSpec& dir )
	{
		CInfoPBRec cInfo;
		
		FSpGetCatInfo< FNF_Throws >( cInfo, dir );
		
		const bool is_dir = cInfo.hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		if ( !is_dir )
		{
			// I wanted a dir but you gave me a file.  You creep.
			throw N::ErrFSNotAFolder();
		}
		
		const N::FSDirID dirID = N::FSDirID( cInfo.dirInfo.ioDrDirID );
		
		return NN::Make< N::FSDirSpec >( N::FSVolumeRefNum( dir.vRefNum ), dirID );
	}
	
	FSTreePtr FSTreeFromFSDirSpec( const N::FSDirSpec& dir )
	{
		return FSTreeFromFSSpec( FSMakeFSSpec< FNF_Throws >( dir, NULL ) );
	}
	
}

