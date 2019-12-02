/*	=============
 *	FSTree_Dev.cc
 *	=============
 */

#include "Genie/FS/FSTree_Dev.hh"

// Standard C
#include "errno.h"

// POSIX
#include "fcntl.h"
#include <sys/stat.h>

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/functions/new_static_symlink.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/dynamic_group.hh"

// MacVFS
#include "MacVFS/file/gestalt.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/config/mini.hh"
#include "relix/config/pts.hh"
#include "relix/fs/con_tag.hh"
#include "relix/fs/pts_tag.hh"
#include "relix/fs/simple_device.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"
#include "relix/task/session.hh"

// Genie
#include "Genie/IO/SerialDevice.hh"


#ifndef CONFIG_DEV_SERIAL
#define CONFIG_DEV_SERIAL  (!CONFIG_MINI)
#endif

namespace Genie
{
	
	namespace p7 = poseven;
	
	using vfs::data_method_set;
	using vfs::node_method_set;
	
	
	struct dev_serial
	{
		static vfs::filehandle_ptr open( const vfs::node* that, int flags, mode_t mode );
	};
	
	vfs::filehandle_ptr dev_serial::open( const vfs::node* that, int flags, mode_t mode )
	{
		/*
			cu.modem
			tty.modem
			cu.printer
			tty.printer
		*/
		
		const size_t len = that->name().size();
		
		const char* port_name  = len & 2 ? "B" : "A";
		const bool passive     = len & 1;
		const bool nonblocking = flags & O_NONBLOCK;
		
		return OpenSerialDevice( port_name, passive, nonblocking );
	}
	
	
	struct dev_gestalt
	{
		static vfs::filehandle_ptr open( const vfs::node* that, int flags, mode_t mode );
	};
	
	vfs::filehandle_ptr dev_gestalt::open( const vfs::node* that, int flags, mode_t mode )
	{
		return open_gestalt( that, flags, mode );
	}
	
	
	static vfs::filehandle_ptr simple_device_open( const vfs::node* that, int flags, mode_t mode )
	{
		return relix::open_simple_device( *that );
	}
	
	static const data_method_set simple_device_data_methods =
	{
		&simple_device_open
	};
	
	static const node_method_set simple_device_methods =
	{
		NULL,
		&simple_device_data_methods
	};
	
	struct dev_tty
	{
		static vfs::filehandle_ptr open( const vfs::node* that, int flags, mode_t mode );
	};
	
	vfs::filehandle_ptr dev_tty::open( const vfs::node* that, int flags, mode_t mode )
	{
		vfs::filehandle* tty = relix::current_process().get_process_group().get_session().get_ctty().get();
		
		if ( tty == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return tty;
	}
	
	
	template < class Opener >
	struct basic_device
	{
		static const data_method_set data_methods;
		static const node_method_set node_methods;
	};
	
	template < class Opener >
	const data_method_set basic_device< Opener >::data_methods =
	{
		&Opener::open
	};
	
	template < class Opener >
	const node_method_set basic_device< Opener >::node_methods =
	{
		NULL,
		&data_methods
	};
	
	
	struct basicdevice_params
	{
		mode_t perm;
		
		const node_method_set& node_methods;
	};
	
	static vfs::node_ptr basicdevice_factory( const vfs::node*     parent,
	                                          const plus::string&  name,
	                                          const void*          args )
	{
		const basicdevice_params& params = *(const basicdevice_params*) args;
		
		return new vfs::node( parent,
		                      name,
		                      params.perm,
		                      &params.node_methods );
	}
	
	static basicdevice_params tty_params =
	{
		S_IFCHR | S_IRUSR | S_IWUSR,
		basic_device< dev_tty >::node_methods,
	};
	
	static basicdevice_params gestalt_params =
	{
		S_IFCHR | S_IRUSR,
		basic_device< dev_gestalt >::node_methods,
	};
	
	static basicdevice_params serial_params =
	{
		S_IFCHR | S_IRUSR | S_IWUSR,
		basic_device< dev_serial >::node_methods,
	};
	
	static vfs::node_ptr SimpleDevice_Factory( const vfs::node*     parent,
	                                           const plus::string&  name,
	                                           const void*          args )
	{
		return new vfs::node( parent, name, S_IFCHR | 0600, &simple_device_methods );
	}
	
	using vfs::dynamic_group_factory;
	using vfs::dynamic_group_element;
	
	const vfs::fixed_mapping dev_Mappings[] =
	{
		{ "null",    &SimpleDevice_Factory },
		{ "zero",    &SimpleDevice_Factory },
		{ "console", &SimpleDevice_Factory },
		
		{ "tty", &basicdevice_factory, &tty_params },
		
	#if CONFIG_DEV_SERIAL
		
		{ "cu.modem",    &basicdevice_factory, &serial_params },
		{ "cu.printer",  &basicdevice_factory, &serial_params },
		{ "tty.modem",   &basicdevice_factory, &serial_params },
		{ "tty.printer", &basicdevice_factory, &serial_params },
		
	#endif
		
		{ "gestalt", &basicdevice_factory, &gestalt_params },
		
		{ "con", &dynamic_group_factory, &dynamic_group_element< relix::con_tag >::extra },
		
	#if CONFIG_PTS
		
		{ "pts", &dynamic_group_factory, &dynamic_group_element< relix::pts_tag  >::extra },
		
	#endif
		
		{ "fd", &vfs::new_static_symlink, "/proc/self/fd" },
		
		{ NULL, NULL }
	};
	
}
