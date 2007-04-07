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
#include "Genie/Console.hh"
#include "Genie/Devices.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/SerialDevice.hh"
#include "Genie/IO/SimpleDevice.hh"
#include "Genie/IO/TTY.hh"
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	class FSTree_dev : public FSTree_Virtual
	{
		public:
			FSTree_dev();
			
			std::string Name() const  { return "dev"; }
			
			FSTreePtr Parent() const  { return FSRoot(); }
	};
	
	class FSTree_dev_fd : public FSTree
	{
		bool IsLink() const { return true; }
		
		std::string Name() const  { return "fd"; }
		
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
			std::string Name() const  { return "tty"; }
			
			FSTreePtr Parent() const  { return GetDevFSTree(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	class FSTree_dev_modem : public FSTree
	{
		public:
			std::string Name() const  { return "cu.modem"; }
			
			FSTreePtr Parent() const  { return GetDevFSTree(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	struct dev_term_Details;
	
	typedef FSTree_Special< dev_term_Details > FSTree_dev_term;
	
	struct dev_term_Details
	{
		static std::string Name()  { return "term"; }
		
		FSTreePtr Parent() const  { return FSTreePtr( GetSingleton< FSTree_dev >() ); }
		
		FSTreePtr Lookup( const std::string& name ) const;
		
		const ConsolesOwner::Map& ItemSequence() const  { return GetConsoleMap(); }
		
		static FSNode ConvertToFSNode( ConsolesOwner::Map::value_type console );
		
		FSNode operator()( ConsolesOwner::Map::value_type console ) const  { return ConvertToFSNode( console ); }
	};
	
	
	class FSTree_dev_term_N : public FSTree
	{
		private:
			unsigned itsIndex;
		
		public:
			FSTree_dev_term_N( unsigned index ) : itsIndex( index )  {}
			
			std::string Name() const;
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_dev_term >(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
	};
	
	
	FSTreePtr GetDevFSTree()
	{
		return GetSingleton< FSTree_dev >();
	}
	
	
	boost::shared_ptr< IOHandle > FSTree_Device::Open( OpenFlags flags ) const
	{
		return GetSimpleDeviceHandle( deviceName );
	}
	
	boost::shared_ptr< IOHandle > FSTree_dev_tty::Open( OpenFlags flags ) const
	{
		TTYHandle* tty = CurrentProcess().ControllingTerminal();
		
		if ( tty == NULL )
		{
			P7::ThrowErrno( ENOENT );
		}
		
		return tty->shared_from_this();
	}
	
	boost::shared_ptr< IOHandle > FSTree_dev_modem::Open( OpenFlags flags ) const
	{
		return boost::shared_ptr< IOHandle >( new SerialDeviceHandle() );
	}
	
	
	FSTree_dev::FSTree_dev()
	{
		Map( "null",    FSTreePtr( new FSTree_Device( "null"    ) ) );
		Map( "zero",    FSTreePtr( new FSTree_Device( "zero"    ) ) );
		Map( "console", FSTreePtr( new FSTree_Device( "console" ) ) );
		
		Map( "tty", FSTreePtr( GetSingleton< FSTree_dev_tty >() ) );
		
		Map( "cu.modem", FSTreePtr( GetSingleton< FSTree_dev_modem >() ) );
		
		Map( "term", FSTreePtr( GetSingleton< FSTree_dev_term >() ) );
		Map( "fd",   FSTreePtr( GetSingleton< FSTree_dev_fd   >() ) );
	}
	
	
	FSTreePtr dev_term_Details::Lookup( const std::string& name ) const
	{
		unsigned index = std::atoi( name.c_str() );
		
		return FSTreePtr( new FSTree_dev_term_N( index ) );
	}
	
	FSNode dev_term_Details::ConvertToFSNode( ConsolesOwner::Map::value_type consoleMapping )
	{
		Console* console = consoleMapping.first;
		
		const std::string& pathname = console->TTYName();
		
		const std::string& name = pathname.substr( sizeof "/dev/term/" - 1, pathname.npos );
		
		unsigned index = std::atoi( name.c_str() );
		
		FSTreePtr tree( new FSTree_dev_term_N( index ) );
		
		return FSNode( name, tree );
	}
	
	
	std::string FSTree_dev_term_N::Name() const
	{
		return NN::Convert< std::string >( itsIndex );
	}
	
}

