/*	=========
 *	FSSpec.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSSPEC_HH
#define GENIE_FILESYSTEM_FSSPEC_HH

// mac-types
#include "mac_types/VRefNum_DirID.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node_ptr.hh"

// MacTypes.h
typedef unsigned char Byte;

// Files.h
union CInfoPBRec;
struct FSSpec;


namespace Genie
{
	
	mac::types::VRefNum_DirID Dir_From_CInfo( const CInfoPBRec& cInfo );
	
	vfs::node_ptr FSTreeFromFSSpec( const FSSpec& item );
	
	vfs::node_ptr node_from_HFS( short vRefNum, long dirID, const Byte* name );
	
	vfs::node_ptr node_from_dirID( short vRefNum, long dirID );
	
	inline
	vfs::node_ptr node_from_dir( const mac::types::VRefNum_DirID& dir )
	{
		return node_from_dirID( dir.vRefNum, dir.dirID );
	}
	
}

#endif
