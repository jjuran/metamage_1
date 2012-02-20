/*	=============
 *	FSTree_Dev.cc
 *	=============
 */

#include "Genie/FS/FSTree_Dev.hh"

// Standard C
#include "errno.h"

// POSIX
#include "fcntl.h"

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/DynamicGroups.hh"
#include "Genie/FS/FSTree_dev_gestalt.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/SymbolicLink.hh"
#include "Genie/IO/PseudoTTY.hh"
#include "Genie/IO/SerialDevice.hh"
#include "Genie/IO/SimpleDevice.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	struct CallOut_Traits
	{
		static const bool isPassive = false;
	};
	
	struct DialIn_Traits
	{
		static const bool isPassive = true;
	};
	
	struct ModemPort_Traits
	{
		static const char* Name()  { return "A"; }
	};
	
	struct PrinterPort_Traits
	{
		static const char* Name()  { return "B"; }
	};
	
	template < class Mode, class Port >
	struct dev_Serial
	{
		static const mode_t perm = S_IRUSR | S_IWUSR;
		
		static IOPtr open( const FSTree* node, int flags, mode_t mode );
	};
	
	template < class Mode, class Port >
	IOPtr dev_Serial< Mode, Port >::open( const FSTree* node, int flags, mode_t mode )
	{
		const bool nonblocking = flags & O_NONBLOCK;
		
		return OpenSerialDevice( Port::Name(), Mode::isPassive, nonblocking );
	}
	
	typedef dev_Serial< CallOut_Traits, ModemPort_Traits   > dev_cumodem;
	typedef dev_Serial< CallOut_Traits, PrinterPort_Traits > dev_cuprinter;
	typedef dev_Serial< DialIn_Traits,  ModemPort_Traits   > dev_ttymodem;
	typedef dev_Serial< DialIn_Traits,  PrinterPort_Traits > dev_ttyprinter;
	
	
	class ConsoleTTYHandle;
	
	typedef FSTree_DynamicGroup< ConsoleTTYHandle > FSTree_dev_con;
	typedef FSTree_DynamicGroup< PseudoTTYHandle  > FSTree_dev_pts;
	
	
	static IOPtr simple_device_open( const FSTree* node, int flags, mode_t mode )
	{
		return GetSimpleDeviceHandle( node->name() );
	}
	
	static const data_method_set simple_device_data_methods =
	{
		&simple_device_open
	};
	
	static const node_method_set simple_device_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&simple_device_data_methods
	};
	
	struct dev_tty
	{
		static const mode_t perm = S_IRUSR | S_IWUSR;
		
		static IOPtr open( const FSTree* node, int flags, mode_t mode );
	};
	
	IOPtr dev_tty::open( const FSTree* node, int flags, mode_t mode )
	{
		const IOPtr& tty = CurrentProcess().ControllingTerminal();
		
		if ( tty.get() == NULL )
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
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&data_methods
	};
	
	
	static FSTreePtr dev_fd_Factory( const FSTreePtr&     parent,
	                                 const plus::string&  name,
	                                 const void*          args )
	{
		return FSTreePtr( New_FSTree_SymbolicLink( parent, name, "/proc/self/fd" ) );
	}
	
	template < class Opener >
	static FSTreePtr BasicDevice_Factory( const FSTreePtr&     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		return new FSTree( parent,
		                   name,
		                   S_IFCHR | Opener::perm,
		                   &basic_device< Opener >::node_methods );
	}
	
	static FSTreePtr SimpleDevice_Factory( const FSTreePtr&     parent,
	                                       const plus::string&  name,
	                                       const void*          args )
	{
		return new FSTree( parent, name, S_IFCHR | 0600, &simple_device_methods );
	}
	
	const FSTree_Premapped::Mapping dev_Mappings[] =
	{
		{ "null",    &SimpleDevice_Factory },
		{ "zero",    &SimpleDevice_Factory },
		{ "console", &SimpleDevice_Factory },
		
		{ "tty", &BasicDevice_Factory< dev_tty > },
		
		{ "cu.modem",    &BasicDevice_Factory< dev_cumodem    > },
		{ "cu.printer",  &BasicDevice_Factory< dev_cuprinter  > },
		{ "tty.modem",   &BasicDevice_Factory< dev_ttymodem   > },
		{ "tty.printer", &BasicDevice_Factory< dev_ttyprinter > },
		
		{ "gestalt", &BasicDevice_Factory< dev_gestalt > },
		
		{ "con", &Basic_Factory< FSTree_dev_con > },
		{ "pts", &Basic_Factory< FSTree_dev_pts > },
		
		{ "fd", &dev_fd_Factory },
		
		{ NULL, NULL }
	};
	
}

