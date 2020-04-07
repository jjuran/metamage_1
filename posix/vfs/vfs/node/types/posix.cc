/*
	posix.cc
	--------
*/

#include "vfs/node/types/posix.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// posix-utils
#include "posix/open_or_connect.hh"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/opendir.hh"
#include "poseven/functions/stat.hh"
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/types/posix.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	static node_ptr new_posix_file( plus::string         path,
	                                const plus::string&  name,
	                                const node*          parent,
	                                uid_t                user );
	
	struct posix_extra
	{
		plus::datum_storage  path;
		struct stat          status;
	};
	
	static void fix_loose_filemode( struct stat& st )
	{
		if ( st.st_mode & 0002  &&  st.st_mode & 0111 )
		{
			// world-writable AND executable
			
			if ( S_ISREG( st.st_mode ) )
			{
				/*
					This is a problem:  A regular file which is not only
					writable by anybody (which is bad enough), but is also
					executable. This may be the result of brokenly creating
					files too permissively on a Unix filesystem, or mounting
					a brain-damaged filesystem that doesn't store permissions
					and simply reports 0777 for all files.
					
					Note that a file with 0666 may still be a problem, but
					the particular failure mode we're addressing here leaves
					files with 0777, which can't ever be desirable.  At least
					with 0666 it's presumably *intentional*, and therefore
					your own fault rather than someone else's if a file gets
					clobbered.
					
					To mitigate, set the file permissions to zero.  This does
					two things:  It prevents all access except by privileged
					users, and it prevents users from inadvertantly executing
					the file.
				*/
				
				st.st_mode = S_IFREG | 0000;
			}
			else
			{
				/*
					This is perfectly normal for symlinks and reasonable for
					certain directories, like /tmp.  Files of other types
					probably shouldn't be executable at all (what does x mean
					for a device, socket, or FIFO?), but the brain damage in
					question is specific to regular files (since the missing
					knowledge of POSIX file modes includes type as well as
					permissions).
				*/
			}
		}
	}
	
	static void posix_stat( const node*     that,
	                        struct stat&  sb )
	{
		posix_extra& extra = *(posix_extra*) that->extra();
		
		sb = extra.status;
		
		fix_loose_filemode( sb );
	}
	
	static n::owned< p7::fd_t > open_or_connect( const char* path, int flags, mode_t mode )
	{
		const int fd = posix::open_or_connect( path, flags, mode );
		
		p7::throw_posix_result( fd );
		
		return n::owned< p7::fd_t >::seize( p7::fd_t( fd ) );
	}
	
	static filehandle_ptr posix_open( const node* that, int flags, mode_t mode )
	{
		posix_extra& extra = *(posix_extra*) that->extra();
		
		const plus::string& path = reinterpret_cast< const plus::string& >( extra.path );
		
		n::owned< p7::fd_t > fd = open_or_connect( path.c_str(), flags, mode );
		
		filehandle_ptr result = new_posix_fd( flags, fd.get() );
		
		fd.release();
		
		return result;
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
		
		if ( strlen( name.c_str() ) < name_size )
		{
			/*
				This name has an embedded NUL.  It couldn't have come from
				any POSIX subsystem (e.g. MacRelix system calls), but it's
				possible over Freemount.  Presumably it's a path resolution
				attack.  Such names can't exist in POSIX, so report ENOENT.
			*/
			
			p7::throw_errno( ENOENT );
		}
		
		plus::string path;
		
		char* p = path.reset( path_size + 1 + name_size );
		
		p = (char*) mempcpy( p, begin( extra.path ), path_size );
		
		*p++ = '/';
		
		mempcpy( p, name.data(), name_size );
		
		return new_posix_file( path.move(), name, parent, that->user() );
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
	
	static const item_method_set posix_item_methods =
	{
		&posix_stat,
	};
	
	static const data_method_set posix_data_methods =
	{
		&posix_open,
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
		&posix_item_methods,
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
	                                const node*          parent,
	                                uid_t                user )
	{
		struct stat sb;
		
		p7::stat( path, sb );
		
		fix_loose_filemode( sb );
		
		node* result = new node( parent,
		                         name,
		                         sb.st_mode,
		                         user,
		                         &posix_methods,
		                         sizeof (posix_extra),
		                         &dispose_posix_file );
		
		posix_extra& extra = *(posix_extra*) result->extra();
		
		plus::construct_from_move( extra.path, path.move() );
		
		extra.status = sb;
		
		return result;
	}
	
	node_ptr new_posix_root( const plus::string& path, uid_t user )
	{
		return new_posix_file( path, plus::string::null, NULL, user );
	}
	
}
