/*
	resources.hh
	------------
*/

#ifndef GENIE_FS_RESOURCES_HH
#define GENIE_FS_RESOURCES_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"

// vfs
#include "vfs/node_ptr.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	void iterate_resources( const FSSpec& file, vfs::dir_contents& cache );
	
	vfs::node_ptr Get_RsrcFile_FSTree( const vfs::node*     parent,
	                                   const plus::string&  name,
	                                   const FSSpec&        file );
	
}

#endif
