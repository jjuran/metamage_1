/*	================
 *	TextEdit_text.hh
 *	================
 */

#ifndef GENIE_FILESYSTEM_TEXTEDIT_TEXT_HH
#define GENIE_FILESYSTEM_TEXTEDIT_TEXT_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_TextEdit_text( const vfs::node*     parent,
	                                        const plus::string&  name,
	                                        const void*          args );
	
}

#endif
