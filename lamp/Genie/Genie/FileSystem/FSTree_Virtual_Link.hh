/*	======================
 *	FSTree_Virtual_Link.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREEVIRTUALLINK_HH
#define GENIE_FILESYSTEM_FSTREEVIRTUALLINK_HH

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_Virtual_Link( const FSTreePtr&    parent,
	                                   const std::string&  name,
	                                   const std::string&  target );
	
}

#endif

