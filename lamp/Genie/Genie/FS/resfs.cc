/*
	resfs.cc
	--------
*/

#include "Genie/FS/resfs.hh"

// POSIX
#include <sys/stat.h>

// Standard C
#include <errno.h>

// Standard C++
#include <map>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/file_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node.hh"

// Nitrogen
#include "Nitrogen/Resources.hh"

// Genie
#include "Genie/code/prepare_executable.hh"
#include "Genie/IO/Handle.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	namespace N = Nitrogen;
	
	
	typedef std::map< plus::string, vfs::node_ptr > map_of_nodes_by_name;
	
	typedef std::map< const vfs::node*, map_of_nodes_by_name > map_of_nodes_by_parent_and_name;
	
	static map_of_nodes_by_parent_and_name resfs_node_map;
	
	
	static vfs::node_ptr resfs_dir_lookup( const vfs::node*     that,
	                                       const plus::string&  name,
	                                       const vfs::node*     parent )
	{
		typedef map_of_nodes_by_parent_and_name::const_iterator Iter;
		
		Iter it = resfs_node_map.find( that );
		
		if ( it != resfs_node_map.end() )
		{
			const map_of_nodes_by_name& map = it->second;
			
			typedef map_of_nodes_by_name::const_iterator Iter;
			
			Iter it = map.find( name );
			
			if ( it != map.end() )
			{
				return it->second;
			}
		}
		
		p7::throw_errno( ENOENT );
		
		// Not reached
		return vfs::node_ptr();
	}
	
	static void resfs_dir_listdir( const vfs::node*    that,
	                               vfs::dir_contents&  cache )
	{
		typedef map_of_nodes_by_parent_and_name::const_iterator Iter;
		
		Iter it = resfs_node_map.find( that );
		
		if ( it != resfs_node_map.end() )
		{
			const map_of_nodes_by_name& map = it->second;
			
			typedef map_of_nodes_by_name::const_iterator Iter;
			
			const Iter end = map.end();
			
			for ( Iter it = map.begin();  it != end;  ++it )
			{
				const vfs::dir_entry node( 0, it->first );
				
				cache.push_back( node );
			}
		}
	}
	
	static const vfs::dir_method_set resfs_dir_dirmethods =
	{
		&resfs_dir_lookup,
		&resfs_dir_listdir
	};
	
	static const vfs::node_method_set resfs_dir_methods =
	{
		NULL,
		NULL,
		NULL,
		&resfs_dir_dirmethods
	};
	
	
	struct resfs_file_extra
	{
		ResType  res_type;
		short    res_id;
	};
	
	static vfs::filehandle_ptr resfs_file_open( const vfs::node* that, int flags, mode_t mode )
	{
		resfs_file_extra& extra = *(resfs_file_extra*) that->extra();
		
		const Mac::ResType resType = Mac::ResType( extra.res_type );
		const Mac::ResID   resID   = Mac::ResID  ( extra.res_id   );
		
		Mac::Handle r = N::Get1Resource( resType, resID );
		
		return new_Handle_handle( *that, flags, N::DetachResource( r ) );
	}
	
	static off_t resfs_file_geteof( const vfs::node* that )
	{
		resfs_file_extra& extra = *(resfs_file_extra*) that->extra();
		
		::SetResLoad( false );
		
		::Handle r = ::Get1Resource( extra.res_type, extra.res_id );
		
		::SetResLoad( true );
		
		if ( r != NULL )
		{
			const Size size = ::GetHandleSize( r );
			
			::ReleaseResource( r );
			
			return size;
		}
		
		return 0;
	}
	
	static
	void resfs_file_stat( const vfs::node*  that,
	                      struct stat&      st )
	{
		resfs_file_extra& extra = *(resfs_file_extra*) that->extra();
		
		memset( &st, '\0', sizeof (struct stat) );
		
		st.st_mode = extra.res_type == 'Exec' ? S_IFREG | 0555 : S_IFREG | 0444;
		st.st_size = resfs_file_geteof( that );
	}
	
	static vfs::program_ptr resfs_file_loadexec( const vfs::node* that )
	{
		resfs_file_extra& extra = *(resfs_file_extra*) that->extra();
		
		const Mac::ResType resType = Mac::ResType( extra.res_type );
		const Mac::ResID   resID   = Mac::ResID  ( extra.res_id   );
		
		execution_unit unit = N::DetachResource( N::Get1Resource( resType, resID ) );
		
		N::HLockHi( unit.get() );
		
		return prepare_executable( unit );
	}
	
	static const vfs::item_method_set resfs_file_itemmethods =
	{
		&resfs_file_stat,
	};
	
	static const vfs::data_method_set resfs_file_datamethods =
	{
		&resfs_file_open,
		&resfs_file_geteof
	};
	
	static const vfs::file_method_set resfs_file_filemethods =
	{
		NULL,
		NULL,
		NULL,
		&resfs_file_loadexec
	};
	
	static const vfs::node_method_set resfs_file_methods =
	{
		&resfs_file_itemmethods,
		&resfs_file_datamethods,
		NULL,
		NULL,
		&resfs_file_filemethods
	};
	
	static vfs::node_ptr new_resfs_dir( const vfs::node*     parent,
	                                    const plus::string&  name )
	{
		return new vfs::node( parent,
		                      name,
		                      S_IFDIR | 0500,
		                      &resfs_dir_methods );
	}
	
	static vfs::node_ptr new_resfs_file( const vfs::node*     parent,
	                                     const plus::string&  name,
	                                     ResType              type,
	                                     short                id )
	{
		vfs::node* result = new vfs::node( parent,
		                                   name,
		                                   S_IFREG | 0500,
		                                   &resfs_file_methods,
		                                   sizeof (resfs_file_extra) );
		
		resfs_file_extra& extra = *(resfs_file_extra*) result->extra();
		
		extra.res_type = type;
		extra.res_id   = id;
		
		return result;
	}
	
	
	static vfs::node_ptr lookup_resfs_dir( const vfs::node*     parent,
	                                       const plus::string&  name )
	{
		vfs::node_ptr& node = resfs_node_map[ parent ][ name ];
		
		if ( node.get() == NULL )
		{
			node = new_resfs_dir( parent, name );
		}
		
		return node;
	}
	
	static void map_resfs_file( const vfs::node*  parent,
	                            const char*       path,
	                            ResType           type,
	                            short             id )
	{
		// Skip leading slash(es)
		
		while ( *path == '/' )
		{
			++path;
		}
		
		while ( *path != '\0' )
		{
			const char* slash = strchr( path, '/' );
			
			if ( slash != NULL )
			{
				plus::string dir_name( path, slash );
				
				vfs::node_ptr dir = lookup_resfs_dir( parent, dir_name );
				
				parent = dir.get();
				
				path = slash;
				
				while ( *++path == '/' )
				{
					continue;
				}
			}
			else
			{
				resfs_node_map[ parent ][ path ] = new_resfs_file( parent, path, type, id );
				
				return;
			}
		}
		
		// FIXME:  Path contains no non-dir filename, report an error
	}
	
	static void map_resfs_files_of_type( const vfs::node*  root,
	                                     ResType           type )
	{
		const short n_execs = ::Count1Resources( type );
		
		for ( short i = 1;  i <= n_execs;  ++i )
		{
			Handle r = ::Get1IndResource( type, i );
			
			if ( r == NULL )
			{
				// FIXME:  Report error
				
				continue;
			}
			
			short    id;
			ResType  type;
			Str255   name;
			
			::GetResInfo( r, &id, &type, name );
			
			map_resfs_file( root,
			                plus::string( name ).c_str(),
			                type,
			                id );
			
			::ReleaseResource( r );
		}
	}
	
	vfs::node_ptr new_resfs_root()
	{
		static vfs::node_ptr resfs_root_node = new_resfs_dir( NULL, plus::string::null );
		
		::SetResLoad( false );
		
		map_resfs_files_of_type( resfs_root_node.get(), 'Data' );
		map_resfs_files_of_type( resfs_root_node.get(), 'Exec' );
		
		::SetResLoad( true );
		
		return resfs_root_node;
	}
	
}
