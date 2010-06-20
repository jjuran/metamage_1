/*
	new_focuser.hh
	--------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_FS_NEW_FOCUSER_HH
#define GENIE_FS_NEW_FOCUSER_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_focuser( const FSTreePtr&     parent,
	                       const plus::string&  name,
	                       const void*          args );
	
}

#endif

