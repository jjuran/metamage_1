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
#include "poseven/Errno.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/FS/DynamicGroups.hh"
#include "Genie/FS/FSTree_dev_gestalt.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/IO/PseudoTTY.hh"
#include "Genie/IO/SerialDevice.hh"
#include "Genie/IO/SimpleDevice.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class FSTree_dev_fd : public FSTree
	{
		public:
			FSTree_dev_fd( const FSTreePtr&    parent,
			               const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			bool IsLink() const { return true; }
			
			std::string ReadLink() const  { return "/proc/self/fd"; }
			
			FSTreePtr ResolveLink() const  { return ResolveAbsolutePath( STR_LEN( "/proc/self/fd" ) ); }
	};
	
	class FSTree_Device : public FSTree
	{
		public:
			FSTree_Device( const FSTreePtr&    parent,
			               const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
	};
	
	class FSTree_SimpleDevice : public FSTree_Device
	{
		public:
			FSTree_SimpleDevice( const FSTreePtr&    parent,
			                     const std::string&  name ) : FSTree_Device( parent, name )
			{
			}
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_tty : public FSTree_Device
	{
		public:
			FSTree_dev_tty( const FSTreePtr&    parent,
			                const std::string&  name ) : FSTree_Device( parent, name )
			{
			}
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
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
	class FSTree_dev_Serial : public FSTree_Device
	{
		public:
			FSTree_dev_Serial( const FSTreePtr&    parent,
			                   const std::string&  name ) : FSTree_Device( parent, name )
			{
			}
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				bool nonblocking = flags & O_NONBLOCK;
				
				return OpenSerialDevice( Port::Name(), Mode::isPassive, nonblocking );
			}
	};
	
	typedef FSTree_dev_Serial< CallOut_Traits, ModemPort_Traits   > FSTree_dev_cumodem;
	typedef FSTree_dev_Serial< CallOut_Traits, PrinterPort_Traits > FSTree_dev_cuprinter;
	typedef FSTree_dev_Serial< DialIn_Traits,  ModemPort_Traits   > FSTree_dev_ttymodem;
	typedef FSTree_dev_Serial< DialIn_Traits,  PrinterPort_Traits > FSTree_dev_ttyprinter;
	
	
	class ConsoleTTYHandle;
	
	typedef FSTree_Sequence< DynamicGroup_Details< ConsoleTTYHandle > > FSTree_dev_con;
	typedef FSTree_Sequence< DynamicGroup_Details< PseudoTTYHandle  > > FSTree_dev_pts;
	
	
	boost::shared_ptr< IOHandle > FSTree_SimpleDevice::Open( OpenFlags /*flags*/ ) const
	{
		return GetSimpleDeviceHandle( Name() );
	}
	
	boost::shared_ptr< IOHandle > FSTree_dev_tty::Open( OpenFlags /*flags*/ ) const
	{
		const boost::shared_ptr< IOHandle >& tty = CurrentProcess().ControllingTerminal();
		
		if ( tty.get() == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return tty;
	}
	
	
	static FSTreePtr SimpleDevice_Factory( const FSTreePtr&    parent,
	                                       const std::string&  name )
	{
		return FSTreePtr( new FSTree_SimpleDevice( parent, name ) );
	}
	
	const FSTree_Premapped::Mapping dev_Mappings[] =
	{
		{ "null",    &SimpleDevice_Factory },
		{ "zero",    &SimpleDevice_Factory },
		{ "console", &SimpleDevice_Factory },
		
		{ "tty", &Basic_Factory< FSTree_dev_tty > },
		
		{ "cu.modem",    &Basic_Factory< FSTree_dev_cumodem    > },
		{ "cu.printer",  &Basic_Factory< FSTree_dev_cuprinter  > },
		{ "tty.modem",   &Basic_Factory< FSTree_dev_ttymodem   > },
		{ "tty.printer", &Basic_Factory< FSTree_dev_ttyprinter > },
		
		{ "gestalt", &Basic_Factory< FSTree_dev_gestalt > },
		
		{ "con", &Basic_Factory< FSTree_dev_con > },
		{ "pts", &Basic_Factory< FSTree_dev_pts > },
		{ "fd",  &Basic_Factory< FSTree_dev_fd  > },
		
		{ NULL, NULL }
	};
	
}

