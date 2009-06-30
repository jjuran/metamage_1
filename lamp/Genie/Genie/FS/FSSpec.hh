/*	=========
 *	FSSpec.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSSPEC_HH
#define GENIE_FILESYSTEM_FSSPEC_HH

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FS/FSTree.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	bool VolumeIsOnServer( Nitrogen::FSVolumeRefNum vRefNum );
	
	inline bool FileIsOnServer( const FSSpec& file )
	{
		return VolumeIsOnServer( Nitrogen::FSVolumeRefNum( file.vRefNum ) );
	}
	
	Nitrogen::FSDirSpec Dir_From_CInfo( const CInfoPBRec& cInfo );
	
	Nitrogen::FSDirSpec Dir_From_FSSpec( const FSSpec& dir );
	
	FSSpec GetFSSpecFromFSTree( const FSTreePtr& file );
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item, bool onServer );
	
	FSTreePtr FSTreeFromFSDirSpec( const Nitrogen::FSDirSpec& dir, bool onServer );
	
}

#endif

