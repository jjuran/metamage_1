/*	=============
 *	FSTree_Dev.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_Dev.hh"

// Standard C
#include "errno.h"

// Nucleus
#include "Nucleus/Convert.h"

// POSeven
#include "POSeven/Errno.hh"

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
	
	class FSTree_dev_modem : public FSTree
	{
		public:
			static std::string OnlyName()  { return "cu.modem"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetDevFSTree(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
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
	
	
	struct dev_con_Details
	{
		static std::string Name()  { return "con"; }
		
		FSTreePtr Parent() const  { return GetSingleton< FSTree_dev >(); }
		
		FSTreePtr Lookup( const std::string& name ) const;
		
		const ConsoleMap& ItemSequence() const  { return GetConsoleMap(); }
		
		static FSNode ConvertToFSNode( ConsoleMap::value_type console );
		
		FSNode operator()( ConsoleMap::value_type console ) const  { return ConvertToFSNode( console ); }
	};
	
	typedef FSTree_Special< dev_con_Details > FSTree_dev_con;
	
	
	struct dev_pts_Details
	{
		static std::string Name()  { return "pts"; }
		
		FSTreePtr Parent() const  { return GetSingleton< FSTree_dev >(); }
		
		FSTreePtr Lookup( const std::string& name ) const;
		
		const PseudoTTYMap& ItemSequence() const  { return GetPseudoTTYMap(); }
		
		static FSNode ConvertToFSNode( PseudoTTYMap::value_type tty );
		
		FSNode operator()( PseudoTTYMap::value_type tty ) const  { return ConvertToFSNode( tty ); }
	};
	
	typedef FSTree_Special< dev_pts_Details > FSTree_dev_pts;
	
	
	class FSTree_dev_con_N : public FSTree
	{
		private:
			unsigned itsIndex;
		
		public:
			FSTree_dev_con_N( unsigned index ) : itsIndex( index )  {}
			
			std::string Name() const;
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_dev_con >(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	class FSTree_dev_pts_N : public FSTree
	{
		private:
			unsigned itsIndex;
		
		public:
			FSTree_dev_pts_N( unsigned index ) : itsIndex( index )  {}
			
			std::string Name() const;
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_dev_pts >(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
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
	
	boost::shared_ptr< IOHandle > FSTree_dev_modem::Open( OpenFlags /*flags*/ ) const
	{
		return OpenSerialDevice();
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
	
	boost::shared_ptr< IOHandle > FSTree_dev_con_N::Open( OpenFlags flags ) const
	{
		return GetConsoleByID( itsIndex ).shared_from_this();
	}
	
	boost::shared_ptr< IOHandle > FSTree_dev_pts_N::Open( OpenFlags flags ) const
	{
		return GetPseudoTTYByID( itsIndex ).shared_from_this();
	}
	
	FSTree_dev::FSTree_dev()
	{
		Map( "null",    FSTreePtr( new FSTree_Device( "null"    ) ) );
		Map( "zero",    FSTreePtr( new FSTree_Device( "zero"    ) ) );
		Map( "console", FSTreePtr( new FSTree_Device( "console" ) ) );
		
		MapSingleton< FSTree_dev_tty   >();
		MapSingleton< FSTree_dev_modem >();
		
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
	
	
	FSTreePtr dev_con_Details::Lookup( const std::string& name ) const
	{
		unsigned index = std::atoi( name.c_str() );
		
		return FSTreePtr( new FSTree_dev_con_N( index ) );
	}
	
	FSTreePtr dev_pts_Details::Lookup( const std::string& name ) const
	{
		unsigned index = std::atoi( name.c_str() );
		
		return FSTreePtr( new FSTree_dev_pts_N( index ) );
	}
	
	FSNode dev_con_Details::ConvertToFSNode( ConsoleMap::value_type consoleMapping )
	{
		ConsoleID id = consoleMapping.first;
		
		ASSERT( !consoleMapping.second.expired() );
		
		boost::shared_ptr< IOHandle > io = consoleMapping.second.lock();
		
		TTYHandle& console = IOHandle_Cast< TTYHandle >( *io );
		
		const std::string& pathname = console.TTYName();
		
		std::string name = pathname.substr( pathname.find_last_of( "/" ) + 1, pathname.npos );
		
		FSTreePtr tree( new FSTree_dev_con_N( id ) );
		
		return FSNode( name, tree );
	}
	
	FSNode dev_pts_Details::ConvertToFSNode( PseudoTTYMap::value_type ttyMapping )
	{
		TerminalID id = ttyMapping.first;
		
		ASSERT( !ttyMapping.second.expired() );
		
		boost::shared_ptr< IOHandle > io = ttyMapping.second.lock();
		
		TTYHandle& tty = IOHandle_Cast< TTYHandle >( *io );
		
		const std::string& pathname = tty.TTYName();
		
		std::string name = pathname.substr( pathname.find_last_of( "/" ) + 1, pathname.npos );
		
		FSTreePtr tree( new FSTree_dev_pts_N( id ) );
		
		return FSNode( name, tree );
	}
	
	
	std::string FSTree_dev_con_N::Name() const
	{
		return NN::Convert< std::string >( itsIndex );
	}
	
	std::string FSTree_dev_pts_N::Name() const
	{
		return NN::Convert< std::string >( itsIndex );
	}
	
}

