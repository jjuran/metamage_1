/*
	Genie/FS/sys/app.cc
	-------------------
*/

#include "Genie/FS/sys/app.hh"

// mac-sys-utils
#include "mac_sys/free_mem.hh"
#include "mac_sys/heap_size.hh"

// plus
#include "plus/serialize.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/sys/app/cmd.hh"
#include "Genie/FS/sys/app/dir.hh"
#include "Genie/FS/sys/app/exe.hh"
#include "Genie/FS/sys/app/menu.hh"
#include "Genie/FS/sys/app/window.hh"


#ifndef TARGET_API_MAC_CARBON
#define TARGET_API_MAC_CARBON  0
#endif


namespace Genie
{
	
	static
	void get( plus::var_string& result, const vfs::node* that, bool binary, const plus::string& name )
	{
		typedef plus::serialize_unsigned< long > Accessor;
		
		Accessor::result_type data;
		
		if ( ! TARGET_API_MAC_CARBON  &&  name[ 0 ] == 'h' )
		{
			data = mac::sys::heap_size();
		}
		else
		{
			data = mac::sys::free_mem();
		}
		
		Accessor::deconstruct::apply( result, data, binary );
	}
	
	static const vfs::property_params sys_app_mux_params =
	{
		plus::serialize_unsigned< long >::fixed_size,
		(vfs::property_get_hook) &get,
	};
	
	
	#define PREMAPPED( map )  &vfs::fixed_dir_factory, (const void*) map
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	#define PROPERTY_ACCESS( access )  PROPERTY( sys_app_mux )
	
	const vfs::fixed_mapping sys_app_Mappings[] =
	{
		{ "cmd",   &New_FSTree_sys_app_cmd },
		
		{ "dir",   &New_FSTree_sys_app_dir },
		{ "exe",   &New_FSTree_sys_app_exe },
		
		{ "freemem", PROPERTY_ACCESS( GetFreeMem ) },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "heapsize", PROPERTY_ACCESS( GetHeapSize ) },
		
	#endif
		
		{ "menu",   PREMAPPED( sys_app_menu_Mappings   ) },
		{ "window", PREMAPPED( sys_app_window_Mappings ) },
		
		{ NULL, NULL }
		
	};
	
}
