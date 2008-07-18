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
			FSTree_dev( const FSTreePtr& parent ) : FSTree_Functional_Singleton( parent )
			{
			}
			
			void Init();
			
			std::string Name() const  { return "dev"; }
			
			FSTreePtr Parent() const  { return FSRoot(); }
	};
	
	class FSTree_dev_fd : public FSTree
	{
		public:
			FSTree_dev_fd( const FSTreePtr& parent ) : FSTree( parent )
			{
			}
			
			bool IsLink() const { return true; }
			
			static const char* OnlyName()  { return "fd"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetDevFSTree(); }
			
			std::string ReadLink() const  { return "/proc/self/fd"; }
			
			FSTreePtr ResolveLink() const  { return ResolvePathname( ReadLink(), FSRoot() ); }
	};
	
	class FSTree_Device : public FSTree
	{
		private:
			std::string deviceName;
		
		public:
			FSTree_Device( const FSTreePtr&    parent,
			               const std::string&  name ) : FSTree( parent ),
			                                            deviceName( name )
			{
			}
			
			std::string Name() const  { return deviceName; }
			
			FSTreePtr Parent() const  { return GetDevFSTree(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_tty : public FSTree
	{
		public:
			FSTree_dev_tty( const FSTreePtr& parent ) : FSTree( parent )
			{
			}
			
			static std::string OnlyName()  { return "tty"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetDevFSTree(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_Serial : public FSTree
	{
		private:
			const char*  itsDeviceName;
			const char*  itsPortName;
			bool         itIsPassive;
		
		public:
			FSTree_dev_Serial( const FSTreePtr&  parent,
			                   const char*       device,
			                   const char*       port,
			                   bool              passive ) : FSTree       ( parent  ),
			                                                 itsDeviceName( device  ),
			                                                 itsPortName  ( port    ),
			                                                 itIsPassive  ( passive )
			{
			}
			
			std::string Name() const  { return itsDeviceName; }
			
			FSTreePtr Parent() const  { return GetDevFSTree(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_cumodem : public FSTree_dev_Serial
	{
		public:
			FSTree_dev_cumodem( const FSTreePtr& parent ) : FSTree_dev_Serial( parent, OnlyName(), "A", false )
			{
			}
			
			static const char* OnlyName()  { return "cu.modem"; }
	};
	
	class FSTree_dev_cuprinter : public FSTree_dev_Serial
	{
		public:
			FSTree_dev_cuprinter( const FSTreePtr& parent ) : FSTree_dev_Serial( parent, OnlyName(), "B", false )
			{
			}
			
			static const char* OnlyName()  { return "cu.printer"; }
	};
	
	class FSTree_dev_ttymodem : public FSTree_dev_Serial
	{
		public:
			FSTree_dev_ttymodem( const FSTreePtr& parent ) : FSTree_dev_Serial( parent, OnlyName(), "A", true )
			{
			}
			
			static const char* OnlyName()  { return "tty.modem"; }
	};
	
	class FSTree_dev_ttyprinter : public FSTree_dev_Serial
	{
		public:
			FSTree_dev_ttyprinter( const FSTreePtr& parent ) : FSTree_dev_Serial( parent, OnlyName(), "B", true )
			{
			}
			
			static const char* OnlyName()  { return "tty.printer"; }
	};
	
	class FSTree_dev_new : public FSTree_Functional_Singleton
	{
		public:
			FSTree_dev_new( const FSTreePtr& parent ) : FSTree_Functional_Singleton( parent )
			{
			}
			
			void Init();
			
			static std::string OnlyName()  { return "new"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetDevFSTree(); }
	};
	
	class FSTree_dev_new_buffer : public FSTree
	{
		public:
			FSTree_dev_new_buffer( const FSTreePtr& parent ) : FSTree( parent )
			{
			}
			
			static std::string OnlyName()  { return "buffer"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_dev_new  >(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_new_console : public FSTree
	{
		public:
			FSTree_dev_new_console( const FSTreePtr& parent ) : FSTree( parent )
			{
			}
			
			static std::string OnlyName()  { return "console"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_dev_new  >(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_new_port : public FSTree
	{
		public:
			FSTree_dev_new_port( const FSTreePtr& parent ) : FSTree( parent )
			{
			}
			
			static std::string OnlyName()  { return "port"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_dev_new  >(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	template <>
	struct IOHandle_Name_Traits< ConsoleTTYHandle >
	{
		static std::string Name()  { return "con"; }
	};
	
	template <>
	struct IOHandle_Name_Traits< PseudoTTYHandle >
	{
		static std::string Name()  { return "pts"; }
	};
	
	
	template <>
	struct IOHandle_Parent_Traits< ConsoleTTYHandle >
	{
		typedef FSTree_dev Tree;
	};
	
	template <>
	struct IOHandle_Parent_Traits< PseudoTTYHandle >
	{
		typedef FSTree_dev Tree;
	};
	
	
	typedef FSTree_Sequence< DynamicGroup_Details< ConsoleTTYHandle > > FSTree_dev_con;
	typedef FSTree_Sequence< DynamicGroup_Details< PseudoTTYHandle  > > FSTree_dev_pts;
	
	
	template <>
	struct IOHandle_FSTree_Traits< ConsoleTTYHandle >
	{
		typedef FSTree_dev_con Tree;
	};
	
	template <>
	struct IOHandle_FSTree_Traits< PseudoTTYHandle >
	{
		typedef FSTree_dev_pts Tree;
	};
	
	
	FSTreePtr GetDevFSTree()
	{
		return GetSingleton< FSTree_dev >();
	}
	
	
	boost::shared_ptr< IOHandle > FSTree_Device::Open( OpenFlags /*flags*/ ) const
	{
		return GetSimpleDeviceHandle( deviceName );
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
	
	boost::shared_ptr< IOHandle > FSTree_dev_Serial::Open( OpenFlags flags ) const
	{
		bool nonblocking = flags & O_NONBLOCK;
		
		return OpenSerialDevice( itsPortName, itIsPassive, nonblocking );
	}
	
	boost::shared_ptr< IOHandle > FSTree_dev_new_buffer::Open( OpenFlags flags ) const
	{
		return NewDynamicElement< BufferFileHandle >();
	}
	
	boost::shared_ptr< IOHandle > FSTree_dev_new_console::Open( OpenFlags flags ) const
	{
		return NewDynamicElement< ConsoleTTYHandle >();
	}
	
	boost::shared_ptr< IOHandle > FSTree_dev_new_port::Open( OpenFlags flags ) const
	{
		return NewDynamicElement< GraphicsWindow >();
	}
	
	
	static FSTreePtr Device_Factory( const FSTreePtr&    parent,
	                                 const std::string&  name )
	{
		return MakeFSTree( new FSTree_Device( parent, name ) );
	}
	
	void FSTree_dev::Init()
	{
		Map( "null",    &Device_Factory );
		Map( "zero",    &Device_Factory );
		Map( "console", &Device_Factory );
		
		Map( "tty", &Singleton_Factory< FSTree_dev_tty > );
		
		Map( "cu.modem",    &Singleton_Factory< FSTree_dev_cumodem    > );
		Map( "cu.printer",  &Singleton_Factory< FSTree_dev_cuprinter  > );
		Map( "tty.modem",   &Singleton_Factory< FSTree_dev_ttymodem   > );
		Map( "tty.printer", &Singleton_Factory< FSTree_dev_ttyprinter > );
		
		Map( "new", &Singleton_Factory< FSTree_dev_new > );
		Map( "con", &Singleton_Factory< FSTree_dev_con > );
		Map( "pts", &Singleton_Factory< FSTree_dev_pts > );
		Map( "fd",  &Singleton_Factory< FSTree_dev_fd  > );
	}
	
	
	void FSTree_dev_new::Init()
	{
		Map( "buffer",  &Singleton_Factory< FSTree_dev_new_buffer  > );
		Map( "console", &Singleton_Factory< FSTree_dev_new_console > );
		Map( "port",    &Singleton_Factory< FSTree_dev_new_port    > );
	}
	
}

