/*
	gui/new/caption.hh
	------------------
*/

#ifndef GENIE_FS_GUI_NEW_CAPTION_HH
#define GENIE_FS_GUI_NEW_CAPTION_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_new_caption( const FSTree*        parent,
	                                  const plus::string&  name,
	                                  const void*          args );
	
}

#endif

