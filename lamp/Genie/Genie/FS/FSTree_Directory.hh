/*	===================
 *	FSTree_Directory.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_DIRECTORY_HH
#define GENIE_FILESYSTEM_FSTREE_DIRECTORY_HH

// vfs
#include "vfs/nodes/fixed_dir.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	namespace premapped
	{
		
		typedef vfs::node_destructor  destructor;
		typedef vfs::node_factory     function;
		typedef vfs::fixed_mapping    mapping;
		
		using vfs::empty_mappings;
		
	}
	
	
	FSTreePtr Premapped_Factory( const FSTree*               parent,
	                             const plus::string&         name,
	                             const premapped::mapping    mappings[] = premapped::empty_mappings,
	                             void                      (*dtor)(const FSTree*) = NULL );
	
}

#endif

