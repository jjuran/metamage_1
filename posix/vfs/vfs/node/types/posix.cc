/*
	posix.cc
	--------
*/

#include "vfs/node/types/posix.hh"

// more-libc
#include "more/string.h"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/functions/opendir.hh"
#include "poseven/functions/stat.hh"
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	static node_ptr new_posix_file( plus::string         path,
	                                const plus::string&  name,
	                                const node*          parent );
	
	struct posix_extra
	{
		plus::datum_storage  path;
		struct stat          status;
	};
	
	static void posix_stat( const node*     that,
	                        struct ::stat&  sb )
	{
		posix_extra& extra = *(posix_extra*) that->extra();
		
		sb = extra.status;
	}
	
	static plus::string posix_slurp( const node* that )
	{
		posix_extra& extra = *(posix_extra*) that->extra();
		
		const plus::string& path = reinterpret_cast< const plus::string& >( extra.path );
		
		return p7::slurp( path.c_str() );
	}
	
	static node_ptr posix_lookup( const node*          that,
	                              const plus::string&  name,
	                              const node*          parent )
	{
		posix_extra& extra = *(posix_extra*) that->extra();
		
		const size_t path_size = size( extra.path );
		const size_t name_size = name.size();
		
		plus::string path;
		
		char* p = path.reset( path_size + 1 + name_size );
		
		p = (char*) mempcpy( p, begin( extra.path ), path_size );
		
		*p++ = '/';
		
		mempcpy( p, name.data(), name_size );
		
		return new_posix_file( path.move(), name, parent );
	}
	
	static void posix_listdir( const node*         that,
	                           vfs::dir_contents&  cache )
	{
		posix_extra& extra = *(posix_extra*) that->extra();
		
		const plus::string& path = reinterpret_cast< const plus::string& >( extra.path );
		
		n::owned< p7::dir_t > dir = p7::opendir( path );
		
		while ( const dirent* entry = ::readdir( dir ) )
		{
			const char* name = entry->d_name;
			
			if ( name[0] == '.'  &&  name[ 1 + (name[1] == '.') ] == '\0' )
			{
				continue;
			}
			
			const vfs::dir_entry node( entry->d_ino, name );
			
			cache.push_back( node );
		}
	}
	
	static const data_method_set posix_data_methods =
	{
		NULL,
		NULL,
		NULL,
		&posix_slurp,
	};
	
	static const dir_method_set posix_dir_methods =
	{
		&posix_lookup,
		&posix_listdir,
		NULL,
		NULL,
	};
	
	static const node_method_set posix_methods =
	{
		&posix_stat,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&posix_data_methods,
		NULL,
		&posix_dir_methods,
	};
	
	static void dispose_posix_file( const node* that )
	{
		posix_extra& extra = *(posix_extra*) that->extra();
		
		destroy( extra.path );
	}
	
	static node_ptr new_posix_file( plus::string         path,
	                                const plus::string&  name,
	                                const node*          parent )
	{
		struct stat sb = p7::stat( path );
		
		node* result = new node( parent,
		                         name,
		                         sb.st_mode,
		                         &posix_methods,
		                         sizeof (posix_extra),
		                         &dispose_posix_file );
		
		posix_extra& extra = *(posix_extra*) result->extra();
		
		plus::construct_from_move( extra.path, path.move() );
		
		extra.status = sb;
		
		return result;
	}
	
	node_ptr new_posix_root( const plus::string& path )
	{
		return new_posix_file( path, plus::string::null, NULL );
	}
	
}

