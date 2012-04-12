/*
	dir_method_set.hh
	-----------------
*/

#ifndef GENIE_FS_DIRMETHODSET_HH
#define GENIE_FS_DIRMETHODSET_HH

// POSIX
#include <sys/types.h>

// plus
#include "plus/string.hh"

// vfs
#include "vfs/dir_contents_fwd.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"
#include "Genie/IO/IOPtr.hh"


namespace Genie
{
	
	typedef FSTreePtr (*lookup_method)( const FSTree*, const plus::string&, const FSTree* );
	
	typedef void (*listdir_method)( const FSTree* node, vfs::dir_contents& cache );
	
	typedef void (*mkdir_method)( const FSTree* node, mode_t mode );
	
	typedef IOPtr (*opendir_method)( const FSTree* node );
	
	typedef IOPtr (*chdir_method)( const FSTree* node );
	
	
	struct dir_method_set
	{
		lookup_method   lookup;
		listdir_method  listdir;
		mkdir_method    mkdir;
		opendir_method  opendir;
		chdir_method    chdir;
	};
	
}

#endif

