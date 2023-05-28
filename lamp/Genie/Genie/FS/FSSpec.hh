/*	=========
 *	FSSpec.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSSPEC_HH
#define GENIE_FILESYSTEM_FSSPEC_HH

// mac-types
#include "mac_types/VRefNum_DirID.hh"

// vfs
#include "vfs/node_ptr.hh"

// Files.h
union CInfoPBRec;
struct FSSpec;


namespace Genie
{
	
	mac::types::VRefNum_DirID Dir_From_CInfo( const CInfoPBRec& cInfo );
	
	vfs::node_ptr FSTreeFromFSSpec( const FSSpec& item );
	
	vfs::node_ptr node_from_dirID( short vRefNum, long dirID );
	
	template < class Dir >
	inline vfs::node_ptr FSTreeFromFSDirSpec( const Dir& dir )
	{
		return node_from_dirID( dir.vRefNum, dir.dirID );
	}
	
}

#endif
