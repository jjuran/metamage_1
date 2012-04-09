/*
	Union.hh
	--------
	
	Copyright 2010, Joshua Juran
*/

#ifndef GENIE_FS_UNION_HH
#define GENIE_FS_UNION_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_Union( const FSTreePtr&     parent,
	                            const plus::string&  name,
	                            const FSTreePtr&     top,
	                            const FSTreePtr&     bottom );
	
}

#endif

