/*
	Volumes.cc
	----------
*/

#include "Genie/FS/Volumes.hh"

// POSIX
#include <sys/stat.h>

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// plus
#include "plus/mac_utf8.hh"
#include "plus/replaced_string.hh"
#include "plus/var_string.hh"

// Nitrogen
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

#include "Nitrogen/Str.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/misc_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/FSSpec.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static inline plus::string slashes_from_colons( const plus::string& unix_name )
	{
		return plus::replaced_string( unix_name, ':', '/' );
	}
	
	static inline plus::string colons_from_slashes( const plus::string& mac_name )
	{
		return plus::replaced_string( mac_name, '/', ':' );
	}
	
	static inline plus::string colons_from_slashes( const unsigned char* mac_name )
	{
		return colons_from_slashes( plus::string( mac_name ) );
	}
	
	
	static Mac::FSVolumeRefNum GetVRefNum( const plus::string& name )
	{
		OSErr err = bdNamErr;
		
		HParamBlockRec pb;
		
		N::Str255 name_copy = name;
		
		uint8_t len = name_copy[ 0 ];
		
		if ( len <= 27 )
		{
			pb.volumeParam.ioVRefNum  = 0;
			pb.volumeParam.ioNamePtr  = name_copy;
			pb.volumeParam.ioVolIndex = -1;  // use ioNamePtr/ioVRefNum combo
			
			err = ::PBHGetVInfoSync( &pb );
		}
		
		Mac::ThrowOSStatus( err );
		
		return Mac::FSVolumeRefNum( pb.volumeParam.ioVRefNum );
	}
	
	
	static vfs::node_ptr volumes_link_resolve( const vfs::node* that )
	{
		// Convert UTF-8 to MacRoman, ':' to '/'
		plus::var_string mac_name = slashes_from_colons( plus::mac_from_utf8( that->name() ) );
		
		mac_name += ":";
		
		const Mac::FSVolumeRefNum vRefNum = GetVRefNum( mac_name );
		
		return node_from_dirID( vRefNum, fsRtDirID );
	}
	
	static const vfs::link_method_set volumes_link_link_methods =
	{
		NULL,
		&volumes_link_resolve
	};
	
	static const vfs::node_method_set volumes_link_methods =
	{
		NULL,
		NULL,
		&volumes_link_link_methods
	};
	
	
	static vfs::node_ptr volumes_lookup( const vfs::node*     that,
	                                     const plus::string&  name,
	                                     const vfs::node*     parent )
	{
		return new vfs::node( parent,
		                      name,
		                      S_IFLNK | 0777,
		                      &volumes_link_methods );
	}
	
	static void volumes_listdir( const vfs::node*    that,
	                             vfs::dir_contents&  cache )
	{
		for ( int i = 1;  true;  ++i )
		{
			Str27 mac_name = "\p";
			
			HParamBlockRec pb;
			
			pb.volumeParam.ioNamePtr  = mac_name;
			pb.volumeParam.ioVRefNum  = 0;
			pb.volumeParam.ioVolIndex = i;
			
			const OSErr err = ::PBHGetVInfoSync( &pb );
			
			if ( err == nsvErr )
			{
				break;
			}
			
			Mac::ThrowOSStatus( err );
			
			const ino_t inode = -pb.volumeParam.ioVRefNum;
			
			const plus::string name = plus::utf8_from_mac( colons_from_slashes( mac_name ) );
			
			cache.push_back( vfs::dir_entry( inode, name ) );
		}
	}
	
	
	static ino_t volumes_inode( const vfs::node* that )
	{
		return fsRtParID;
	}
	
	static const vfs::dir_method_set volumes_dir_methods =
	{
		&volumes_lookup,
		&volumes_listdir
	};
	
	static const vfs::misc_method_set volumes_misc_methods =
	{
		NULL,
		NULL,
		&volumes_inode
	};
	
	static const vfs::node_method_set volumes_methods =
	{
		NULL,
		NULL,
		NULL,
		&volumes_dir_methods,
		NULL,
		&volumes_misc_methods
	};
	
	
	vfs::node_ptr New_FSTree_Volumes( const vfs::node*     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return new vfs::node( parent,
		                      name,
		                      S_IFDIR | 0700,
		                      &volumes_methods );
	}
	
}
