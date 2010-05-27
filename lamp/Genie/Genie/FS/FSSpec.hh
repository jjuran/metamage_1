/*	=========
 *	FSSpec.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSSPEC_HH
#define GENIE_FILESYSTEM_FSSPEC_HH

// Nitrogen
#include "Mac/Files/Types/FSDirSpec.hh"
#include "Mac/Files/Types/FSVolumeRefNum.hh"

// Genie
#include "Genie/FS/FSTree.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	bool VolumeIsOnServer( Mac::FSVolumeRefNum vRefNum );
	
	inline bool FileIsOnServer( const FSSpec& file )
	{
		return VolumeIsOnServer( Mac::FSVolumeRefNum( file.vRefNum ) );
	}
	
	Mac::FSDirSpec Dir_From_CInfo( const CInfoPBRec& cInfo );
	
	Mac::FSDirSpec Dir_From_FSSpec( const FSSpec& dir );
	
	FSSpec GetFSSpecFromFSTree( const FSTreePtr& file );
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item, bool onServer );
	
	FSTreePtr FSTreeFromFSDirSpec( const Mac::FSDirSpec& dir, bool onServer );
	
}

#endif

