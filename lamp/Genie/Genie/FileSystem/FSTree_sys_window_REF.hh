/*	========================
 *	FSTree_sys_window_REF.hh
 *	========================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSWINDOWREF_HH
#define GENIE_FILESYSTEM_FSTREESYSWINDOWREF_HH

// Nitrogen
#include "Nitrogen/MacWindows.h"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	extern const Functional_Traits< void >::Mapping sys_window_REF_Mappings[];
	
	void RemoveUserWindow( const FSTree* key );
	
	Nitrogen::WindowRef GetWindowRef( const FSTree* key );
	
}

#endif

