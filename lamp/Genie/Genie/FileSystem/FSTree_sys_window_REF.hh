/*	========================
 *	FSTree_sys_window_REF.hh
 *	========================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSWINDOWREF_HH
#define GENIE_FILESYSTEM_FSTREESYSWINDOWREF_HH

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	extern const FSTree_Premapped::Mapping sys_window_REF_Mappings[];
	
	void RemoveUserWindow( const FSTree* key );
	
	bool InvalidateWindow( const FSTree* key );
	
}

#endif

