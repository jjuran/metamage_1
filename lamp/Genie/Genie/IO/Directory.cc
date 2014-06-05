/*	============
 *	Directory.cc
 *	============
 */

#include "Genie/IO/Directory.hh"

// Standard C/C++
#include <cstring>

// POSIX
#include <dirent.h>
#include "fcntl.h"
#include "sys/stat.h"

// Extended API Set, part 2
#include "extended-api-set/part-2.h"

// debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/primitives/inode.hh"
#include "vfs/primitives/listdir.hh"
#include "vfs/primitives/parent_inode.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	static ssize_t dir_read( filehandle* that, char* buffer, size_t n )
	{
		ASSERT( n >= sizeof (dirent) );
		
		dirent& entry = *(dirent*) buffer;
		
		int result = static_cast< dir_handle& >( *that ).readdir( entry );
		
		return result;
	}
	
	const stream_method_set dir_stream_methods =
	{
		NULL,
		&dir_read,
	};
	
	static const filehandle_method_set dir_methods =
	{
		NULL,
		NULL,
		&dir_stream_methods,
	};
	
	
	static dir_contents_box get_contents( const node& dir )
	{
		dir_contents_box result;
		
		dir_contents& contents = *result;
		
		contents.push_back( dir_entry( inode       ( dir ), "."  ) );
		contents.push_back( dir_entry( parent_inode( dir ), ".." ) );
		
		listdir( dir, contents );
		
		return result;
	}
	
	dir_handle::dir_handle( const node* dir, filehandle_destructor dtor  )
	:
		filehandle( dir,
		            O_RDONLY | O_DIRECTORY,
		            &dir_methods,
		            0,
		            dtor )
	{
	}
	
	dir_handle::dir_handle( const filehandle_method_set& methods )
	:
		filehandle( NULL,
		            O_RDONLY | O_DIRECTORY,
		            &methods )
	{
	}
	
	dir_handle::~dir_handle()
	{
	}
	
	static void set_dir_entry( dirent& dir, ino_t inode, const plus::string& name )
	{
		dir.d_ino = inode;
		
		if ( name.size() >= sizeof dir.d_name )
		{
			p7::throw_errno( EOVERFLOW );
		}
		
		std::strcpy( dir.d_name, name.c_str() );
	}
	
	int dir_handle::readdir( dirent& entry )
	{
		if ( !its_contents.get() )
		{
			its_contents = get_contents( *get_file( *this ) );
		}
		
		dir_contents& contents = *its_contents;
		
		const int i = get_mark();
		
		if ( i >= contents.size() )
		{
			return 0;
		}
		
		dir_entry node = contents.at( i );
		
		advance_mark( 1 );
		
		set_dir_entry( entry, node.inode, node.name );
		
		return sizeof (dirent);
	}
	
}
