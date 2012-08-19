/*
	dir_method_set.hh
	-----------------
*/

#ifndef VFS_METHODS_DIRMETHODSET_HH
#define VFS_METHODS_DIRMETHODSET_HH

// POSIX
#include <sys/types.h>

// plus
#include "plus/string.hh"

// vfs
#include "vfs/dir_contents_fwd.hh"
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	typedef node_ptr (*lookup_method)( const node*, const plus::string&, const node* );
	
	typedef void (*listdir_method)( const node* it, vfs::dir_contents& cache );
	
	typedef void (*mkdir_method)( const node* it, mode_t mode );
	
	typedef filehandle_ptr (*opendir_method)( const node* it );
	
	
	struct dir_method_set
	{
		lookup_method   lookup;
		listdir_method  listdir;
		mkdir_method    mkdir;
		opendir_method  opendir;
	};
	
}

#endif

