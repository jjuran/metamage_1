/*
	Volumes.cc
	----------
*/

#include "Genie/FS/Volumes.hh"

// POSIX
#include <sys/stat.h>

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// plus
#include "plus/mac_utf8.hh"
#include "plus/replaced_string.hh"
#include "plus/var_string.hh"

// Nitrogen
#include "Mac/Files/Types/FSDirSpec.hh"
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

#include "Nitrogen/Str.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/misc_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace n = nucleus;
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
		N::Str255 name_copy = name;
		
		HParamBlockRec pb;
		
		pb.volumeParam.ioVRefNum  = 0;
		pb.volumeParam.ioNamePtr  = name_copy;
		pb.volumeParam.ioVolIndex = -1;  // use use ioNamePtr/ioVRefNum combination
		
		Mac::ThrowOSStatus( ::PBHGetVInfoSync( &pb ) );
		
		return Mac::FSVolumeRefNum( pb.volumeParam.ioVRefNum );
	}
	
	
	static FSTreePtr volumes_link_resolve( const FSTree* node )
	{
		// Convert UTF-8 to MacRoman, ':' to '/'
		plus::var_string mac_name = slashes_from_colons( plus::mac_from_utf8( node->name() ) );
		
		mac_name += ":";
		
		const Mac::FSVolumeRefNum vRefNum = GetVRefNum( mac_name );
		
		const Mac::FSDirSpec dir = n::make< Mac::FSDirSpec >( vRefNum, Mac::fsRtDirID );
		
		return FSTreeFromFSDirSpec( dir );
	}
	
	static const link_method_set volumes_link_link_methods =
	{
		NULL,
		&volumes_link_resolve
	};
	
	static const node_method_set volumes_link_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&volumes_link_link_methods
	};
	
	
	static FSTreePtr volumes_lookup( const FSTree*        node,
	                                 const plus::string&  name,
	                                 const FSTree*        parent )
	{
		return new FSTree( parent,
		                   name,
		                   S_IFLNK | 0777,
		                   &volumes_link_methods );
	}
	
	static void volumes_listdir( const FSTree*  node,
	                             FSTreeCache&   cache )
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
			
			cache.push_back( FSNode( inode, name ) );
		}
	}
	
	
	static ino_t volumes_inode( const FSTree* node )
	{
		return fsRtParID;
	}
	
	static const dir_method_set volumes_dir_methods =
	{
		&volumes_lookup,
		&volumes_listdir
	};
	
	static const misc_method_set volumes_misc_methods =
	{
		NULL,
		NULL,
		&volumes_inode
	};
	
	static const node_method_set volumes_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&volumes_dir_methods,
		NULL,
		&volumes_misc_methods
	};
	
	
	FSTreePtr New_FSTree_Volumes( const FSTreePtr&     parent,
	                              const plus::string&  name,
	                              const void*          args )
	{
		return new FSTree( parent,
		                   name,
		                   S_IFDIR | 0700,
		                   &volumes_methods );
	}
	
}

