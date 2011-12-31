/*	================
 *	TextEdit_text.hh
 *	================
 */

#ifndef GENIE_FILESYSTEM_TEXTEDIT_TEXT_HH
#define GENIE_FILESYSTEM_TEXTEDIT_TEXT_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_TextEdit_text( const FSTreePtr&     parent,
	                                    const plus::string&  name,
	                                    const void*          args );
	
}

#endif

