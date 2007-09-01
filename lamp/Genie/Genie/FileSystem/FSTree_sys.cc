/*	=============
 *	FSTree_sys.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_sys.hh"

// Nucleus
#include "Nucleus/Convert.h"

// Nitrogen
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/Processes.h"

// POSeven
#include "POSeven/Errno.hh"

// BitsAndBytes
#include "HexStrings.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/IO/Base.hh"
#include "Genie/IO/Device.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	
	class FSTree_sys_kernel : public FSTree_Virtual
	{
		public:
			FSTree_sys_kernel();
			
			static std::string OnlyName()  { return "kernel"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys >(); }
	};
	
	class FSTree_sys_mac : public FSTree_Virtual
	{
		public:
			FSTree_sys_mac();
			
			static std::string OnlyName()  { return "mac"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys >(); }
	};
	
	struct sys_mac_proc_Details
	{
		static std::string Name()  { return "proc"; }
		
		static FSTreePtr Parent()  { return GetSingleton< FSTree_sys_mac >(); }
		
		static FSTreePtr Lookup( const std::string& name );
		
		static const N::Process_Container& ItemSequence()  { return N::Processes(); }
		
		static FSNode ConvertToFSNode( const N::ProcessSerialNumber& proc );
		
		FSNode operator()( const N::ProcessSerialNumber& proc ) const  { return ConvertToFSNode( proc ); }
	};
	
	typedef FSTree_Special< sys_mac_proc_Details > FSTree_sys_mac_proc;
	
	class FSTree_sys_mac_proc_PSN : public FSTree_Virtual
	{
		private:
			ProcessSerialNumber itsPSN;
		
		public:
			FSTree_sys_mac_proc_PSN( const ProcessSerialNumber& psn );
			
			std::string Name() const;
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_mac_proc >(); }
	};
	
	
	class FSTree_sys_mac_gestalt : public FSTree
	{
		public:
			FSTree_sys_mac_gestalt()  {}
			
			static std::string OnlyName()  { return "gestalt"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_mac >(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	FSTree_sys::FSTree_sys()
	{
		MapSingleton< FSTree_sys_kernel >();
		MapSingleton< FSTree_sys_mac    >();
	}
	
	FSTree_sys_kernel::FSTree_sys_kernel()
	{
		
	}
	
	FSTree_sys_mac::FSTree_sys_mac()
	{
		MapSingleton< FSTree_sys_mac_proc    >();
		MapSingleton< FSTree_sys_mac_gestalt >();
	}
	
	
	static UInt32 DecodeHex32( const char* begin, const char* end )
	{
		using BitsAndBytes::DecodeHex;
		
		std::string decoded = DecodeHex( std::string( begin, end ) );
		
		if ( decoded.size() != sizeof (UInt32) )
		{
			P7::ThrowErrno( ENOENT );
		}
		
		return *(UInt32*) decoded.data();
	}
	
	FSTreePtr sys_mac_proc_Details::Lookup( const std::string& name_string )
	{
		ProcessSerialNumber psn = { 0, 0 };
		
		const char* name = name_string.c_str();
		
		if ( const char* hyphen = std::strchr( name, '-' ) )
		{
			psn.highLongOfPSN = DecodeHex32( name, hyphen );
			
			name = hyphen + 1;
		}
		
		psn.lowLongOfPSN = DecodeHex32( name, &*name_string.end() );
		
		return FSTreePtr( new FSTree_sys_mac_proc_PSN( psn ) );
	}
	
	static std::string NameFromPSN( const ProcessSerialNumber& psn )
	{
		using BitsAndBytes::EncodeAsHex;
		
		std::string name = EncodeAsHex( psn.lowLongOfPSN );
		
		if ( psn.highLongOfPSN != 0 )
		{
			name = EncodeAsHex( psn.highLongOfPSN ) + "-" + name;
		}
		
		return name;
	}
	
	FSNode sys_mac_proc_Details::ConvertToFSNode( const N::ProcessSerialNumber& psn )
	{
		FSTreePtr tree( new FSTree_sys_mac_proc_PSN( psn ) );
		
		return FSNode( NameFromPSN( psn ), tree );
	}
	
	
	FSTree_sys_mac_proc_PSN::FSTree_sys_mac_proc_PSN( const ProcessSerialNumber& psn ) : itsPSN( psn )
	{
		
	}
	
	std::string FSTree_sys_mac_proc_PSN::Name() const
	{
		return NameFromPSN( itsPSN );
	}
	
	
	class GestaltDeviceHandle : public DeviceHandle
	{
		public:
			FSTreePtr GetFile() const  { return GetSingleton< FSTree_sys_mac_gestalt >(); }
			
			unsigned int SysPoll() const  { return 0; }
			
			int SysRead( char* data, std::size_t byteCount )  { return 0; }
			
			int SysWrite( const char* data, std::size_t byteCount )  { return byteCount; }
			
			void IOCtl( unsigned long request, int* argp );
	};
	
	
	void GestaltDeviceHandle::IOCtl( unsigned long request, int* argp )
	{
		N::GestaltSelector selector = N::GestaltSelector( request );
		
		long value = N::Gestalt( selector );
		
		if ( argp != NULL )
		{
			*argp = value;
		}
	}
	
	boost::shared_ptr< IOHandle > FSTree_sys_mac_gestalt::Open( OpenFlags flags ) const
	{
		return boost::shared_ptr< IOHandle >( new GestaltDeviceHandle() );
	}
	
}

