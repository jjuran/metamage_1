/*
	Genie/FS/sys/port/ADDR.hh
	-------------------------
*/

#ifndef GENIE_FS_SYS_PORT_ADDR_HH
#define GENIE_FS_SYS_PORT_ADDR_HH

// Genie
#include "Genie/FS/FSTree_Directory.hh"


namespace Pedestal
{
	
	class View;
	
}

namespace Genie
{
	
	extern const FSTree_Premapped::Mapping sys_window_REF_Mappings[];
	
	void NotifyWindowOfViewLoss( const FSTree* window_key, const FSTree* view );
	
	void RemoveUserWindow( const FSTree* key );
	
	bool InvalidateWindow( const FSTree* key );
	
	void InstallViewInWindow    ( const boost::intrusive_ptr< Pedestal::View >& view, const FSTree* key );
	void UninstallViewFromWindow( const boost::intrusive_ptr< Pedestal::View >& view, const FSTree* key );
	
	const FSTree* GetWindowFocus( const FSTree* window );
	
	void SetWindowFocus( const FSTree* window, const FSTree* focus );
	
}

#endif

