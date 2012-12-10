/*
	basic_directory.hh
	------------------
*/

#ifndef GENIE_FS_BASICDIRECTORY_HH
#define GENIE_FS_BASICDIRECTORY_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/dir_contents_fwd.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	typedef FSTreePtr (*Lookup_Proc)( const FSTree* parent, const plus::string& name );
	
	typedef void (*Iterate_Proc)( const FSTree* parent, vfs::dir_contents& cache );
	
	
	FSTreePtr new_basic_directory( const FSTree*        parent,
	                               const plus::string&  name,
	                               Lookup_Proc          lookup,
	                               Iterate_Proc         iterate );
	
}

#endif

