/*	=============
 *	FSTree_sys.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_sys.hh"

// Nucleus
#include "Nucleus/Convert.h"
#include "Nucleus/LinkedListContainer.h"

// Nitrogen
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/MacWindows.h"
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
	namespace p7 = poseven;
	
	
	static UInt32 DecodeHex32( const char* begin, const char* end )
	{
		using BitsAndBytes::DecodeHex;
		
		std::string decoded = DecodeHex( std::string( begin, end ) );
		
		if ( decoded.size() != sizeof (UInt32) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return *(UInt32*) decoded.data();
	}
	
	class FSTree_sys_kernel : public FSTree_Virtual
	{
		public:
			FSTree_sys_kernel();
			
			static std::string OnlyName()  { return "kernel"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys >(); }
	};
	
	struct sys_window_Details
	{
		static std::string Name()  { return "window"; }
		
		static FSTreePtr Parent()  { return GetSingleton< FSTree_sys >(); }
		
		static FSTreePtr Lookup( const std::string& name );
		
		static const N::WindowList_Container& ItemSequence()  { return N::WindowList(); }
		
		static FSNode ConvertToFSNode( const N::WindowRef& window );
		
		FSNode operator()( const N::WindowRef& window ) const  { return ConvertToFSNode( window ); }
	};
	
	typedef FSTree_Special< sys_window_Details > FSTree_sys_window;
	
	class FSTree_sys_window_REF : public FSTree_Virtual
	{
		private:
			N::WindowRef itsWindow;
		
		public:
			FSTree_sys_window_REF( const N::WindowRef& window ) : itsWindow( window )  {}
			
			std::string Name() const;
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_window >(); }
	};
	
	static std::string NameFromWindow( const N::WindowRef& window )
	{
		using BitsAndBytes::EncodeAsHex;
		
		std::string name = EncodeAsHex( window );
		
		return name;
	}
	
	FSTreePtr sys_window_Details::Lookup( const std::string& name )
	{
		N::WindowRef window = reinterpret_cast< N::WindowRef >( DecodeHex32( &*name.begin(), &*name.end() ) );
		
		return FSTreePtr( new FSTree_sys_window_REF( window ) );
	}
	
	FSNode sys_window_Details::ConvertToFSNode( const N::WindowRef& window )
	{
		FSTreePtr tree( new FSTree_sys_window_REF( window ) );
		
		return FSNode( NameFromWindow( window ), tree );
	}
	
	std::string FSTree_sys_window_REF::Name() const
	{
		return NameFromWindow( itsWindow );
	}
	
	
	class FSTree_sys_mac : public FSTree_Virtual
	{
		public:
			FSTree_sys_mac();
			
			static std::string OnlyName()  { return "mac"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys >(); }
	};
	
	struct sys_mac_vol_Details
	{
		static std::string Name()  { return "vol"; }
		
		static FSTreePtr Parent()  { return GetSingleton< FSTree_sys_mac >(); }
		
		static FSTreePtr Lookup( const std::string& name );
		
		static const N::Volume_Container& ItemSequence()  { return N::Volumes(); }
		
		static FSNode ConvertToFSNode( N::FSVolumeRefNum vRefNum );
		
		FSNode operator()( N::FSVolumeRefNum vRefNum ) const  { return ConvertToFSNode( vRefNum ); }
	};
	
	typedef FSTree_Special< sys_mac_vol_Details > FSTree_sys_mac_vol;
	
	class FSTree_sys_mac_vol_N : public FSTree_Virtual
	{
		private:
			N::FSVolumeRefNum itsVRefNum;
		
		public:
			FSTree_sys_mac_vol_N( N::FSVolumeRefNum vRefNum );
			
			std::string Name() const  { return NN::Convert< std::string >( -itsVRefNum ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_mac_vol >(); }
	};
	
	class FSTree_sys_mac_vol_N_root : public FSTree
	{
		private:
			N::FSVolumeRefNum itsVRefNum;
		
		public:
			FSTree_sys_mac_vol_N_root( N::FSVolumeRefNum vRefNum ) : itsVRefNum( vRefNum )  {}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return "root"; }
			
			FSTreePtr Parent() const  { return FSTreePtr( new FSTree_sys_mac_vol_N( itsVRefNum ) ); }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const  { return FSTreeFromFSSpec( N::FSMakeFSSpec( itsVRefNum, N::fsRtDirID, NULL ) ); }
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
	
	class FSTree_sys_mac_proc_PSN_exe : public FSTree
	{
		private:
			ProcessSerialNumber itsPSN;
		
		public:
			FSTree_sys_mac_proc_PSN_exe( const ProcessSerialNumber& psn ) : itsPSN( psn )  {}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return "exe"; }
			
			FSTreePtr Parent() const  { return FSTreePtr( new FSTree_sys_mac_proc_PSN( itsPSN ) ); }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const  { return FSTreeFromFSSpec( N::GetProcessAppSpec( itsPSN ) ); }
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
		MapSingleton< FSTree_sys_window >();
		MapSingleton< FSTree_sys_mac    >();
	}
	
	FSTree_sys_kernel::FSTree_sys_kernel()
	{
		
	}
	
	FSTree_sys_mac::FSTree_sys_mac()
	{
		MapSingleton< FSTree_sys_mac_vol     >();
		MapSingleton< FSTree_sys_mac_proc    >();
		MapSingleton< FSTree_sys_mac_gestalt >();
	}
	
	
	FSTreePtr sys_mac_vol_Details::Lookup( const std::string& name_string )
	{
		int n = std::atoi( name_string.c_str() );
		
		if ( n <= 0  ||  SInt16( n ) != n )
		{
			p7::throw_errno( ENOENT );
		}
		
		N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( -n );
		
		N::FSMakeFSSpec( vRefNum, N::fsRtDirID, NULL );
		
		return FSTreePtr( new FSTree_sys_mac_vol_N( vRefNum ) );
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
	
	FSNode sys_mac_vol_Details::ConvertToFSNode( N::FSVolumeRefNum vRefNum )
	{
		FSTreePtr tree( new FSTree_sys_mac_vol_N( vRefNum ) );
		
		return FSNode( NN::Convert< std::string >( -vRefNum ), tree );
	}
	
	FSNode sys_mac_proc_Details::ConvertToFSNode( const N::ProcessSerialNumber& psn )
	{
		FSTreePtr tree( new FSTree_sys_mac_proc_PSN( psn ) );
		
		return FSNode( NameFromPSN( psn ), tree );
	}
	
	
	FSTree_sys_mac_vol_N::FSTree_sys_mac_vol_N( N::FSVolumeRefNum vRefNum ) : itsVRefNum( vRefNum )
	{
		Map( "root", FSTreePtr( new FSTree_sys_mac_vol_N_root( vRefNum ) ) );
	}
	
	FSTree_sys_mac_proc_PSN::FSTree_sys_mac_proc_PSN( const ProcessSerialNumber& psn ) : itsPSN( psn )
	{
		Map( "exe", FSTreePtr( new FSTree_sys_mac_proc_PSN_exe( psn ) ) );
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

