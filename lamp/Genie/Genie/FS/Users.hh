/*
	Users.hh
	--------
	
	Copyright 2010, Joshua Juran
*/

#ifndef GENIE_FS_USERS_HH
#define GENIE_FS_USERS_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_Users( const FSTree*        parent,
	                            const plus::string&  name,
	                            const void*          args );
	
}

#endif

