/*
	directory.cc
	------------
*/

#include "vfs/filehandle/types/directory.hh"

// POSIX
#include <dirent.h>
#include "fcntl.h"
#include "sys/stat.h"

// Standard C
#include <string.h>

// Extended API Set, part 2
#include "extended-api-set/part-2.h"

// debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_contents_box.hh"
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
	
	
	void destroy_dir_handle( filehandle* that )
	{
		dir_handle_extra& extra = *(dir_handle_extra*) that->extra();
		
		if ( extra.contents )
		{
			intrusive_ptr_release( extra.contents );
		}
		
		if ( extra.chained_destructor )
		{
			extra.chained_destructor( that );
		}
	};
	
	static
	void set_dir_entry( dirent& dir, ino_t inode, const plus::string& name )
	{
		dir.d_ino = inode;
		
		if ( name.size() >= sizeof dir.d_name )
		{
			p7::throw_errno( EOVERFLOW );
		}
		
		strcpy( dir.d_name, name.c_str() );
	}
	
	static
	ssize_t dir_readdir( filehandle* that, dirent& entry )
	{
		dir_handle_extra& extra = *(dir_handle_extra*) that->extra();
		
		if ( ! extra.contents )
		{
			node_ptr dir_node = get_file( *that );
			
			const node& dir = *dir_node;
			
			dir_contents_box box;
			
			dir_contents& contents = *box;
			
			contents.push_back( dir_entry( inode       ( dir ), "."  ) );
			contents.push_back( dir_entry( parent_inode( dir ), ".." ) );
			
			listdir( dir, contents );
			
			intrusive_ptr_add_ref( extra.contents = box.get() );
		}
		
		dir_contents& contents = *extra.contents;
		
		const off_t i = that->get_mark();
		
		if ( i >= contents.size() )
		{
			return 0;
		}
		
		dir_entry node = contents.at( i );
		
		that->advance_mark( 1 );
		
		set_dir_entry( entry, node.inode, node.name );
		
		return sizeof (dirent);
	}
	
	static ssize_t dir_read( filehandle* that, char* buffer, size_t n )
	{
		ASSERT( n >= sizeof (dirent) );
		
		dirent& entry = *(dirent*) buffer;
		
		return dir_readdir( that, entry );
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
	
	filehandle_ptr new_dir_handle( const node* dir, filehandle_destructor dtor )
	{
		using vfs::filehandle;
		
		filehandle* result = new filehandle( dir,
		                                     O_RDONLY | O_DIRECTORY,
		                                     &dir_methods,
		                                     sizeof (dir_handle_extra),
		                                     &destroy_dir_handle );
		
		dir_handle_extra& extra = *(dir_handle_extra*) result->extra();
		
		extra.chained_destructor = dtor;
		extra.contents           = NULL;
		
		return result;
	}
	
}
