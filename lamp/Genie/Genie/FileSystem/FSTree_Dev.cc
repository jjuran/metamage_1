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
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/BufferFile.hh"
#include "Genie/IO/ConsoleTTY.hh"
#include "Genie/IO/GraphicsPort.hh"
#include "Genie/IO/PseudoTTY.hh"
#include "Genie/IO/SerialDevice.hh"
#include "Genie/IO/SimpleDevice.hh"
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	class FSTree_dev : public FSTree_Virtual
	{
		public:
			FSTree_dev();
			
			std::string Name() const  { return "dev"; }
			
			FSTreePtr Parent() const  { return FSRoot(); }
	};
	
	class FSTree_dev_fd : public FSTree
	{
		public:
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
			FSTree_Device( const std::string& name ) : deviceName( name )  {}
			
			std::string Name() const  { return deviceName; }
			
			FSTreePtr Parent() const  { return GetDevFSTree(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_tty : public FSTree
	{
		public:
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
			FSTree_dev_Serial( const char*  device,
			                   const char*  port,
			                   bool         passive ) : itsDeviceName( device  ),
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
			FSTree_dev_cumodem() : FSTree_dev_Serial( OnlyName(), "A", false )  {}
			
			static const char* OnlyName()  { return "cu.modem"; }
	};
	
	class FSTree_dev_cuprinter : public FSTree_dev_Serial
	{
		public:
			FSTree_dev_cuprinter() : FSTree_dev_Serial( OnlyName(), "B", false )  {}
			
			static const char* OnlyName()  { return "cu.printer"; }
	};
	
	class FSTree_dev_ttymodem : public FSTree_dev_Serial
	{
		public:
			FSTree_dev_ttymodem() : FSTree_dev_Serial( OnlyName(), "A", true )  {}
			
			static const char* OnlyName()  { return "tty.modem"; }
	};
	
	class FSTree_dev_ttyprinter : public FSTree_dev_Serial
	{
		public:
			FSTree_dev_ttyprinter() : FSTree_dev_Serial( OnlyName(), "B", true )  {}
			
			static const char* OnlyName()  { return "tty.printer"; }
	};
	
	class FSTree_dev_new : public FSTree_Virtual
	{
		public:
			FSTree_dev_new();
			
			static std::string OnlyName()  { return "new"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetDevFSTree(); }
	};
	
	class FSTree_dev_new_buffer : public FSTree
	{
		public:
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
			static std::string OnlyName()  { return "port"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_dev_new  >(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	template < class Metadetails >
	struct dev_TTY_Details : public Metadetails, public Integer_KeyName_Traits< unsigned >
	{
		typedef typename Metadetails::Sequence Sequence;
		
		typedef typename Metadetails::FSTree_N ChildNode;
		
		FSTreePtr Parent() const  { return GetSingleton< FSTree_dev >(); }
		
		FSTreePtr Lookup( const std::string& name ) const;
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value.first; }
		
		static bool KeyIsValid( const Key& key )
		{
			const Sequence& sequence = ItemSequence();
			
			return sequence.find( key ) != sequence.end();
		}
		
		static std::string GetChildName( const Sequence::value_type& child )
		{
			ASSERT( !child.second.expired() );
			
			boost::shared_ptr< IOHandle > io = child.second.lock();
			
			TerminalHandle& terminal = IOHandle_Cast< TerminalHandle >( *io );
			
			return io::get_filename( terminal.TTYName() );
		}
		
		FSTreePtr GetChildNode( const Key& key ) const
		{
			return FSTreePtr( new ChildNode( key ) );
		}
	};
	
	class FSTree_dev_con_N;
	class FSTree_dev_pts_N;
	
	struct dev_con_Metadetails
	{
		typedef ConsoleMap        Sequence;
		typedef FSTree_dev_con_N  FSTree_N;
		
		static std::string Name()  { return "con"; }
		
		const Sequence& ItemSequence() const  { return GetConsoleMap(); }
	};
	
	struct dev_pts_Metadetails
	{
		typedef PseudoTTYMap      Sequence;
		typedef FSTree_dev_pts_N  FSTree_N;
		
		static std::string Name()  { return "pts"; }
		
		const Sequence& ItemSequence() const  { return GetPseudoTTYMap(); }
	};
	
	typedef FSTree_Special< dev_TTY_Details< dev_con_Metadetails > > FSTree_dev_con;
	typedef FSTree_Special< dev_TTY_Details< dev_pts_Metadetails > > FSTree_dev_pts;
	
	
	class FSTree_dev_TTY_N_Base : public FSTree
	{
		private:
			unsigned itsID;
		
		protected:
			unsigned ID() const  { return itsID; }
		
		public:
			FSTree_dev_TTY_N_Base( unsigned id ) : itsID( id )
			{
			}
			
			std::string Name() const  { return NN::Convert< std::string >( itsID ); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
	};
	
	class FSTree_dev_con_N : public FSTree_dev_TTY_N_Base
	{
		public:
			FSTree_dev_con_N( unsigned id ) : FSTree_dev_TTY_N_Base( id )
			{
			}
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_dev_con >(); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				return GetConsoleByID( ID() ).shared_from_this();
			}
	};
	
	
	class FSTree_dev_pts_N : public FSTree_dev_TTY_N_Base
	{
		public:
			FSTree_dev_pts_N( unsigned id ) : FSTree_dev_TTY_N_Base( id )
			{
			}
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_dev_pts >(); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				return GetPseudoTTYByID( ID() );
			}
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
		return NewBufferFile();
	}
	
	boost::shared_ptr< IOHandle > FSTree_dev_new_console::Open( OpenFlags flags ) const
	{
		return NewConsoleDevice();
	}
	
	boost::shared_ptr< IOHandle > FSTree_dev_new_port::Open( OpenFlags flags ) const
	{
		return NewGraphicsPort();
	}
	
	
	FSTree_dev::FSTree_dev()
	{
		Map( FSTreePtr( new FSTree_Device( "null"    ) ) );
		Map( FSTreePtr( new FSTree_Device( "zero"    ) ) );
		Map( FSTreePtr( new FSTree_Device( "console" ) ) );
		
		MapSingleton< FSTree_dev_tty >();
		
		MapSingleton< FSTree_dev_cumodem    >();
		MapSingleton< FSTree_dev_cuprinter  >();
		MapSingleton< FSTree_dev_ttymodem   >();
		MapSingleton< FSTree_dev_ttyprinter >();
		
		MapSingleton< FSTree_dev_new >();
		MapSingleton< FSTree_dev_con >();
		MapSingleton< FSTree_dev_pts >();
		MapSingleton< FSTree_dev_fd  >();
	}
	
	
	FSTree_dev_new::FSTree_dev_new()
	{
		MapSingleton< FSTree_dev_new_buffer  >();
		MapSingleton< FSTree_dev_new_console >();
		MapSingleton< FSTree_dev_new_port    >();
	}
	
	
	template < class Metadetails >
	FSTreePtr dev_TTY_Details< Metadetails >::Lookup( const std::string& name ) const
	{
		Key key = KeyFromName( name );
		
		return FSTreePtr( new ChildNode( key ) );
	}
	
}

