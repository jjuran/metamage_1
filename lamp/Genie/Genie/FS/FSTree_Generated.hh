/*	===================
 *	FSTree_Generated.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_GENERATED_HH
#define GENIE_FILESYSTEM_FSTREE_GENERATED_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	typedef std::string (*Generated_ReadHook)( const FSTree* that );
	
	FSTreePtr New_FSTree_Generated( const FSTreePtr&    parent,
	                                const std::string&  name,
	                                Generated_ReadHook  readHook );
	
}

#endif

