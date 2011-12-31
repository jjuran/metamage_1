/*
	Genie/FS/app.hh
	---------------
*/

#ifndef GENIE_FS_APP_HH
#define GENIE_FS_APP_HH

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace plus
{
	
	class string;
	
}

namespace Genie
{
	
	FSTreePtr New_FSTree_app( const FSTreePtr&     parent,
	                          const plus::string&  name,
	                          const void*          args );
	
}

#endif

