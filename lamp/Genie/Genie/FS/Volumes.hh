/*
	Volumes.hh
	----------
	
	Copyright 2010, Joshua Juran
*/

#ifndef GENIE_FS_VOLUMES_HH
#define GENIE_FS_VOLUMES_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_Volumes( const FSTreePtr&     parent,
	                              const plus::string&  name );
	
}

#endif

