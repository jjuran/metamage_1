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
#include "relix/fs/con_group.hh"
#include "relix/fs/pts_group.hh"
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
	
	
	static
	vfs::filehandle_ptr serial_open( const vfs::node* that, int flags, mode_t mode )
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
	
	
	const data_method_set serial_data_methods =
	{
		&serial_open,
	};
	
	const node_method_set serial_methods =
	{
		NULL,
		&serial_data_methods,
	};
	
	
	const data_method_set gestalt_data_methods =
	{
		&vfs::open_gestalt,
	};
	
	const node_method_set gestalt_methods =
	{
		NULL,
		&gestalt_data_methods,
	};
	
	
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
	
	
	static
	vfs::filehandle_ptr tty_open( const vfs::node* that, int flags, mode_t mode )
	{
		vfs::filehandle* tty = relix::current_process().get_process_group().get_session().get_ctty().get();
		
		if ( tty == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return tty;
	}
	
	const data_method_set tty_data_methods =
	{
		&tty_open,
	};
	
	const node_method_set tty_methods =
	{
		NULL,
		&tty_data_methods,
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
		tty_methods,
	};
	
	static basicdevice_params gestalt_params =
	{
		S_IFCHR | S_IRUSR,
		gestalt_methods,
	};
	
	static basicdevice_params serial_params =
	{
		S_IFCHR | S_IRUSR | S_IWUSR,
		serial_methods,
	};
	
	static vfs::node_ptr SimpleDevice_Factory( const vfs::node*     parent,
	                                           const plus::string&  name,
	                                           const void*          args )
	{
		return new vfs::node( parent, name, S_IFCHR | 0600, &simple_device_methods );
	}
	
	using vfs::dynamic_group_factory;
	
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
		
		{ "con", &dynamic_group_factory, &relix::get_con_group() },
		
	#if CONFIG_PTS
		
		{ "pts", &dynamic_group_factory, &relix::get_pts_group() },
		
	#endif
		
		{ "fd", &vfs::new_static_symlink, "/proc/self/fd" },
		
		{ NULL, NULL }
	};
	
}
