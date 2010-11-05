/*
	resources.hh
	------------
*/

#ifndef GENIE_FS_RESOURCES_HH
#define GENIE_FS_RESOURCES_HH

// Genie
#include "Genie/FS/FSTree.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	class FSTreeCache;
	
	void iterate_resources( const FSSpec& file, FSTreeCache& cache );
	
	FSTreePtr Get_RsrcFile_FSTree( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               const FSSpec&        file );
	
}

#endif

