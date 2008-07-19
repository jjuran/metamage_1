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
			
			FSTreePtr ResolveLink() const  { return ResolvePathname( ReadLink(), FSRoot() ); }
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
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_tty : public FSTree
	{
		public:
			FSTree_dev_tty( const FSTreePtr&    parent,
			                const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_Serial : public FSTree
	{
		private:
			const char*  itsPortName;
			bool         itIsPassive;
		
		public:
			FSTree_dev_Serial( const FSTreePtr&    parent,
			                   const std::string&  name,
			                   const char*         port,
			                   bool                passive ) : FSTree     ( parent, name ),
			                                                   itsPortName( port    ),
			                                                   itIsPassive( passive )
			{
			}
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_cumodem : public FSTree_dev_Serial
	{
		public:
			FSTree_dev_cumodem( const FSTreePtr&    parent,
			                    const std::string&  name ) : FSTree_dev_Serial( parent, name, "A", false )
			{
			}
	};
	
	class FSTree_dev_cuprinter : public FSTree_dev_Serial
	{
		public:
			FSTree_dev_cuprinter( const FSTreePtr&    parent,
			                      const std::string&  name ) : FSTree_dev_Serial( parent, name, "B", false )
			{
			}
	};
	
	class FSTree_dev_ttymodem : public FSTree_dev_Serial
	{
		public:
			FSTree_dev_ttymodem( const FSTreePtr&    parent,
			                     const std::string&  name ) : FSTree_dev_Serial( parent, name, "A", true )
			{
			}
	};
	
	class FSTree_dev_ttyprinter : public FSTree_dev_Serial
	{
		public:
			FSTree_dev_ttyprinter( const FSTreePtr&    parent,
			                       const std::string&  name ) : FSTree_dev_Serial( parent, name, "B", true )
			{
			}
	};
	
	class FSTree_dev_new : public FSTree_Functional_Singleton
	{
		public:
			FSTree_dev_new( const FSTreePtr&    parent,
			                const std::string&  name ) : FSTree_Functional_Singleton( parent, name )
			{
			}
			
			void Init();
	};
	
	class FSTree_dev_new_buffer : public FSTree
	{
		public:
			FSTree_dev_new_buffer( const FSTreePtr&    parent,
			                       const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_new_console : public FSTree
	{
		public:
			FSTree_dev_new_console( const FSTreePtr&    parent,
			                        const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_new_port : public FSTree
	{
		public:
			FSTree_dev_new_port( const FSTreePtr&    parent,
			                     const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	typedef FSTree_Sequence< DynamicGroup_Details< ConsoleTTYHandle > > FSTree_dev_con;
	typedef FSTree_Sequence< DynamicGroup_Details< PseudoTTYHandle  > > FSTree_dev_pts;
	
	
	FSTreePtr GetDevFSTree()
	{
		return GetSingleton< FSTree_dev >( FSRoot(), "dev" );
	}
	
	
	boost::shared_ptr< IOHandle > FSTree_Device::Open( OpenFlags /*flags*/ ) const
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

