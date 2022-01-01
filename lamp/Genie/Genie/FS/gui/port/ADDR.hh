/*
	Genie/FS/gui/port/ADDR.hh
	-------------------------
*/

#ifndef GENIE_FS_GUI_PORT_ADDR_HH
#define GENIE_FS_GUI_PORT_ADDR_HH

// vfs
#include "vfs/node/types/fixed_dir.hh"


namespace Pedestal
{
	
	class View;
	
}

namespace Genie
{
	
	extern const vfs::fixed_mapping gui_port_ADDR_Mappings[];
	
	void notify_port_of_view_loss( const vfs::node* port_key, const vfs::node* view );
	
	void remove_window_and_views_from_port( const vfs::node* key );
	
	bool invalidate_port_WindowRef( const vfs::node* key );
	
	void install_view_in_port    ( Pedestal::View& view, const vfs::node* key );
	void uninstall_view_from_port( Pedestal::View& view, const vfs::node* key );
	
	const vfs::node* get_port_focus( const vfs::node* port );
	
	void set_port_focus( const vfs::node* port, const vfs::node* focus );
	
}

#endif
