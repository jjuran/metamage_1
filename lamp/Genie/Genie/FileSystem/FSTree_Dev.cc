/*	=============
 *	FSTree_Dev.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_Dev.hh"

// Standard C
#include "errno.h"

// POSIX
#include "fcntl.h"

// Nucleus
#include "Nucleus/ArrayContainerFunctions.h"
#include "Nucleus/Convert.h"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/Pathnames.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/FileSystem/DynamicGroups.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/BufferFile.hh"
#include "Genie/IO/ConsoleTTY.hh"
#include "Genie/IO/GraphicsPort.hh"
#include "Genie/IO/PseudoTTY.hh"
#include "Genie/IO/SerialDevice.hh"
#include "Genie/IO/SimpleDevice.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	class FSTree_dev : public FSTree_Functional_Singleton
	{
		public:
			FSTree_dev( const FSTreePtr&    parent,
			            const std::string&  name ) : FSTree_Functional_Singleton( parent, name )
			{
			}
			
			void Init();
	};
	
	class FSTree_dev_fd : public FSTree
	{
		public:
			FSTree_dev_fd( const FSTreePtr&    parent,
			               const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			bool IsLink() const { return true; }
			
			std::string ReadLink() const  { return "/proc/self/fd"; }
			
			FSTreePtr ResolveLink() const  { return ResolvePathname( ReadLink() ); }
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
		static const char* Name()  { return "A"; }
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
	
	
	class FSTree_dev_new : public FSTree_Functional_Singleton
	{
		public:
			FSTree_dev_new( const FSTreePtr&    parent,
			                const std::string&  name ) : FSTree_Functional_Singleton( parent, name )
			{
			}
			
			void Init();
	};
	
	template < class Handle >
	class FSTree_dev_new_Device : public FSTree_Device
	{
		public:
			FSTree_dev_new_Device( const FSTreePtr&    parent,
			                       const std::string&  name ) : FSTree_Device( parent, name )
			{
			}
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				return NewDynamicElement< Handle >();
			}
	};
	
	
	typedef FSTree_Sequence< DynamicGroup_Details< ConsoleTTYHandle > > FSTree_dev_con;
	typedef FSTree_Sequence< DynamicGroup_Details< PseudoTTYHandle  > > FSTree_dev_pts;
	
	
	FSTreePtr GetDevFSTree()
	{
		return GetSingleton< FSTree_dev >( FSRoot(), "dev" );
	}
	
	
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
		return MakeFSTree( new FSTree_SimpleDevice( parent, name ) );
	}
	
	static FSTree_dev::Mapping dev_Mappings[] =
	{
		{ "null",    &SimpleDevice_Factory },
		{ "zero",    &SimpleDevice_Factory },
		{ "console", &SimpleDevice_Factory },
		
		{ "tty", &Singleton_Factory< FSTree_dev_tty > },
		
		{ "cu.modem",    &Singleton_Factory< FSTree_dev_cumodem    > },
		{ "cu.printer",  &Singleton_Factory< FSTree_dev_cuprinter  > },
		{ "tty.modem",   &Singleton_Factory< FSTree_dev_ttymodem   > },
		{ "tty.printer", &Singleton_Factory< FSTree_dev_ttyprinter > },
		
		{ "new", &Singleton_Factory< FSTree_dev_new > },
		{ "con", &Singleton_Factory< FSTree_dev_con > },
		{ "pts", &Singleton_Factory< FSTree_dev_pts > },
		{ "fd",  &Singleton_Factory< FSTree_dev_fd  > }
	};
	
	void FSTree_dev::Init()
	{
		AddMappings( dev_Mappings, NN::ArrayEnd( dev_Mappings ) );
	}
	
	
	static FSTree_dev_new::Mapping dev_new_Mappings[] =
	{
		{ "buffer",  &Singleton_Factory< FSTree_dev_new_Device< BufferFileHandle > > },
		{ "console", &Singleton_Factory< FSTree_dev_new_Device< ConsoleTTYHandle > > },
		{ "port",    &Singleton_Factory< FSTree_dev_new_Device< GraphicsWindow   > > }
	};
	
	void FSTree_dev_new::Init()
	{
		AddMappings( dev_new_Mappings, NN::ArrayEnd( dev_new_Mappings ) );
	}
	
}

