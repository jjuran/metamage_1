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
#include "Genie/FS/DynamicGroups.hh"
#include "Genie/FS/FSTree_dev_gestalt.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/SymbolicLink.hh"
#include "Genie/IO/PseudoTTY.hh"
#include "Genie/IO/SerialDevice.hh"
#include "Genie/IO/SimpleDevice.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class FSTree_Device : public FSTree
	{
		public:
			FSTree_Device( const FSTreePtr&     parent,
			               const plus::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
	};
	
	class FSTree_SimpleDevice : public FSTree_Device
	{
		public:
			FSTree_SimpleDevice( const FSTreePtr&     parent,
			                     const plus::string&  name )
			:
				FSTree_Device( parent, name )
			{
			}
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	typedef boost::shared_ptr< IOHandle > (*OpenProc)( OpenFlags flags );
	
	class FSTree_BasicDevice : public FSTree
	{
		private:
			OpenProc itsOpener;
			mode_t   itsPermMode;
		
		public:
			FSTree_BasicDevice( const FSTreePtr&     parent,
			                    const plus::string&  name,
			                    OpenProc             opener,
			                    mode_t               perm )
			:
				FSTree( parent, name ),
				itsOpener( opener ),
				itsPermMode( perm )
			{
			}
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return itsPermMode; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				return itsOpener( flags );
			}
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
	struct dev_Serial
	{
		static const mode_t perm = S_IRUSR | S_IWUSR;
		
		typedef boost::shared_ptr< IOHandle > IORef;
		
		static IORef open( OpenFlags flags )
		{
			const bool nonblocking = flags & O_NONBLOCK;
			
			return OpenSerialDevice( Port::Name(), Mode::isPassive, nonblocking );
		}
	};
	
	typedef dev_Serial< CallOut_Traits, ModemPort_Traits   > dev_cumodem;
	typedef dev_Serial< CallOut_Traits, PrinterPort_Traits > dev_cuprinter;
	typedef dev_Serial< DialIn_Traits,  ModemPort_Traits   > dev_ttymodem;
	typedef dev_Serial< DialIn_Traits,  PrinterPort_Traits > dev_ttyprinter;
	
	
	class ConsoleTTYHandle;
	
	typedef FSTree_DynamicGroup< ConsoleTTYHandle > FSTree_dev_con;
	typedef FSTree_DynamicGroup< PseudoTTYHandle  > FSTree_dev_pts;
	
	
	boost::shared_ptr< IOHandle > FSTree_SimpleDevice::Open( OpenFlags /*flags*/ ) const
	{
		return GetSimpleDeviceHandle( Name() );
	}
	
	struct dev_tty
	{
		static const mode_t perm = S_IRUSR | S_IWUSR;
		
		typedef boost::shared_ptr< IOHandle > IORef;
		
		static IORef open( OpenFlags flags )
		{
			const IORef& tty = CurrentProcess().ControllingTerminal();
			
			if ( tty.get() == NULL )
			{
				p7::throw_errno( ENOENT );
			}
			
			return tty;
		}
	};
	
	
	static FSTreePtr dev_fd_Factory( const FSTreePtr&     parent,
	                                 const plus::string&  name )
	{
		return FSTreePtr( New_FSTree_SymbolicLink( parent, name, "/proc/self/fd" ) );
	}
	
	template < class Opener >
	static FSTreePtr BasicDevice_Factory( const FSTreePtr&     parent,
	                                      const plus::string&  name )
	{
		return seize_ptr( new FSTree_BasicDevice( parent,
		                                          name,
		                                          &Opener::open,
		                                          Opener::perm ) );
	}
	
	static FSTreePtr SimpleDevice_Factory( const FSTreePtr&     parent,
	                                       const plus::string&  name )
	{
		return seize_ptr( new FSTree_SimpleDevice( parent, name ) );
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

