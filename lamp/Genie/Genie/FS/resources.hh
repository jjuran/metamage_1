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

// Genie
#include "Genie/FS/FSTreePtr.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	void iterate_resources( const FSSpec& file, vfs::dir_contents& cache );
	
	FSTreePtr Get_RsrcFile_FSTree( const FSTree*        parent,
	                               const plus::string&  name,
	                               const FSSpec&        file );
	
}

#endif

