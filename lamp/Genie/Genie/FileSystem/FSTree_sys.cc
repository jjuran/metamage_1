/*	=============
 *	FSTree_sys.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_sys.hh"

// Nucleus
#include "Nucleus/Convert.h"

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
	
	
	template < class Key > struct KeyName_Traits;
	
	template <> struct KeyName_Traits< N::WindowRef >
	{
		typedef N::WindowRef Key;
		
		static std::string NameFromKey( const Key& key )
		{
			return BitsAndBytes::EncodeAsHex( key );
		}
		
		static Key KeyFromName( const std::string& name )
		{
			return reinterpret_cast< Key >( DecodeHex32( &*name.begin(), &*name.end() ) );
		}
	};
	
	template <> struct KeyName_Traits< N::FSVolumeRefNum >
	{
		typedef N::FSVolumeRefNum Key;
		
		static std::string NameFromKey( const Key& key )
		{
			return NN::Convert< std::string >( -key );
		}
		
		static Key KeyFromName( const std::string& name )
		{
			int n = std::atoi( name.c_str() );
			
			if ( n <= 0  ||  SInt16( n ) != n )
			{
				p7::throw_errno( ENOENT );
			}
			
			N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( -n );
			
			// Verify it exists
			(void) N::FSMakeFSSpec( vRefNum, N::fsRtDirID, NULL );
			
			return vRefNum;
		}
	};
	
	template <> struct KeyName_Traits< N::ProcessSerialNumber >
	{
		typedef N::ProcessSerialNumber Key;
		
		static std::string NameFromKey( const Key& psn )
		{
			using BitsAndBytes::EncodeAsHex;
			
			std::string name = EncodeAsHex( psn.lowLongOfPSN );
			
			if ( psn.highLongOfPSN != 0 )
			{
				name = EncodeAsHex( psn.highLongOfPSN ) + "-" + name;
			}
			
			return name;
		}
		
		static Key KeyFromName( const std::string& name )
		{
			ProcessSerialNumber psn = { 0, 0 };
			
			const char* begin = name.c_str();
			const char* end   = name.c_str() + name.size();
			
			if ( const char* hyphen = std::strchr( begin, '-' ) )
			{
				psn.highLongOfPSN = DecodeHex32( begin, hyphen );
				
				begin = hyphen + 1;
			}
			
			psn.lowLongOfPSN = DecodeHex32( begin, end );
			
			return psn;
		}
	};
	
	
	class FSTree_sys_kernel : public FSTree_Virtual
	{
		public:
			FSTree_sys_kernel();
			
			std::string Name() const  { return "kernel"; }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys >(); }
	};
	
	class FSTree_sys_kernel_bin : public FSTree_Virtual
	{
		public:
			FSTree_sys_kernel_bin();
			
			std::string Name() const  { return "bin"; }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_kernel >(); }
	};
	
	class FSTree_sys_kernel_bin_EXE : public FSTree
	{
		private:
			typedef int (*Main0)();
			typedef int (*Main2)( int argc, char const *const *argv );
			typedef int (*Main3)( int argc, char const *const *argv, char const *const *envp );
			
			std::string itsName;
			
			MainEntry itsMainEntry;
		
		public:
			FSTree_sys_kernel_bin_EXE( const std::string&  name,
			                           Main3               main ) : itsName( name ),
			                                                        itsMainEntry( GetMainEntryFromAddress( main ) )
			{
			}
			
			FSTree_sys_kernel_bin_EXE( const std::string&  name,
			                           Main2               main ) : itsName( name ),
			                                                        itsMainEntry( GetMainEntryFromAddress( main ) )
			{
			}
			
			FSTree_sys_kernel_bin_EXE( const std::string&  name,
			                           Main0               main ) : itsName( name ),
			                                                        itsMainEntry( GetMainEntryFromAddress( main ) )
			{
			}
			
			std::string Name() const  { return itsName; }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_kernel_bin >(); }
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IXUSR; }
			
			MainEntry GetMainEntry() const  { return itsMainEntry; }
	};
	
	
	struct sys_window_Details : public KeyName_Traits< N::WindowRef >
	{
		typedef N::WindowList_Container Sequence;
		
		static std::string Name()  { return "window"; }
		
		static FSTreePtr Parent()  { return GetSingleton< FSTree_sys >(); }
		
		static FSTreePtr Lookup( const std::string& name );
		
		static const Sequence& ItemSequence()  { return N::WindowList(); }
		
		static std::string ChildName( const Sequence::value_type& child )
		{
			return NameFromKey( child );
		}
		
		static FSTreePtr ChildNode( const Sequence::value_type& child );
	};
	
	typedef FSTree_Special< sys_window_Details > FSTree_sys_window;
	
	class FSTree_sys_window_REF : public FSTree_Virtual,
	                              public KeyName_Traits< N::WindowRef >
	{
		private:
			Key itsKey;
		
		public:
			FSTree_sys_window_REF( const Key& key ) : itsKey( key )  {}
			
			std::string Name() const  { return NameFromKey( itsKey ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_window >(); }
	};
	
	FSTreePtr sys_window_Details::Lookup( const std::string& name )
	{
		N::WindowRef window = KeyFromName( name );
		
		return FSTreePtr( new FSTree_sys_window_REF( window ) );
	}
	
	FSTreePtr sys_window_Details::ChildNode( const Sequence::value_type& child )
	{
		return FSTreePtr( new FSTree_sys_window_REF( child ) );
	}
	
	
	class FSTree_sys_mac : public FSTree_Virtual
	{
		public:
			FSTree_sys_mac();
			
			static std::string OnlyName()  { return "mac"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys >(); }
	};
	
	struct sys_mac_vol_Details : public KeyName_Traits< N::FSVolumeRefNum >
	{
		typedef N::Volume_Container Sequence;
		
		static std::string Name()  { return "vol"; }
		
		static FSTreePtr Parent()  { return GetSingleton< FSTree_sys_mac >(); }
		
		static FSTreePtr Lookup( const std::string& name );
		
		static const Sequence& ItemSequence()  { return N::Volumes(); }
		
		static std::string ChildName( const Sequence::value_type& child )
		{
			return NameFromKey( child );
		}
		
		static FSTreePtr ChildNode( const Sequence::value_type& child );
	};
	
	typedef FSTree_Special< sys_mac_vol_Details > FSTree_sys_mac_vol;
	
	class FSTree_sys_mac_vol_N : public FSTree_Virtual,
	                             public KeyName_Traits< N::FSVolumeRefNum >
	{
		private:
			Key itsKey;
		
		public:
			FSTree_sys_mac_vol_N( const Key& key );
			
			std::string Name() const  { return NameFromKey( itsKey ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_mac_vol >(); }
	};
	
	
	struct sys_mac_proc_Details : public KeyName_Traits< N::ProcessSerialNumber >
	{
		typedef N::Process_Container Sequence;
		
		static std::string Name()  { return "proc"; }
		
		static FSTreePtr Parent()  { return GetSingleton< FSTree_sys_mac >(); }
		
		static FSTreePtr Lookup( const std::string& name );
		
		static const Sequence& ItemSequence()  { return N::Processes(); }
		
		static std::string ChildName( const Sequence::value_type& child )
		{
			return NameFromKey( child );
		}
		
		static FSTreePtr ChildNode( const Sequence::value_type& child );
	};
	
	typedef FSTree_Special< sys_mac_proc_Details > FSTree_sys_mac_proc;
	
	class FSTree_sys_mac_proc_PSN : public FSTree_Virtual,
	                                public KeyName_Traits< N::ProcessSerialNumber >
	{
		private:
			Key itsKey;
		
		public:
			FSTree_sys_mac_proc_PSN( const Key& key );
			
			std::string Name() const  { return NameFromKey( itsKey ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_mac_proc >(); }
	};
	
	class FSTree_sys_mac_proc_PSN_exe : public FSTree
	{
		private:
			typedef ProcessSerialNumber Key;
			
			Key itsKey;
		
		public:
			FSTree_sys_mac_proc_PSN_exe( const Key& key ) : itsKey( key )  {}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return "exe"; }
			
			FSTreePtr Parent() const  { return FSTreePtr( new FSTree_sys_mac_proc_PSN( itsKey ) ); }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const  { return FSTreeFromFSSpec( N::GetProcessAppSpec( itsKey ) ); }
	};
	
	
	class FSTree_sys_mac_gestalt : public FSTree
	{
		public:
			FSTree_sys_mac_gestalt()  {}
			
			std::string Name() const  { return "gestalt"; }
			
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
		MapSingleton< FSTree_sys_kernel_bin >();
	}
	
	static int main_true()
	{
		return 0;
	}
	
	static int main_false()
	{
		return 1;
	}
	
	static int main_beep()
	{
		N::SysBeep();
		
		return 0;
	}
	
	FSTree_sys_kernel_bin::FSTree_sys_kernel_bin()
	{
		Map( FSTreePtr( new FSTree_sys_kernel_bin_EXE( "true",  main_true  ) ) );
		Map( FSTreePtr( new FSTree_sys_kernel_bin_EXE( "false", main_false ) ) );
		
		Map( FSTreePtr( new FSTree_sys_kernel_bin_EXE( "beep", main_beep ) ) );
	}
	
	FSTree_sys_mac::FSTree_sys_mac()
	{
		MapSingleton< FSTree_sys_mac_vol     >();
		MapSingleton< FSTree_sys_mac_proc    >();
		MapSingleton< FSTree_sys_mac_gestalt >();
	}
	
	
	FSTreePtr sys_mac_vol_Details::Lookup( const std::string& name )
	{
		Key vRefNum = KeyFromName( name );
		
		return FSTreePtr( new FSTree_sys_mac_vol_N( vRefNum ) );
	}
	
	FSTreePtr sys_mac_proc_Details::Lookup( const std::string& name )
	{
		Key psn = KeyFromName( name );
		
		return FSTreePtr( new FSTree_sys_mac_proc_PSN( psn ) );
	}
	
	FSTreePtr sys_mac_vol_Details::ChildNode( const Sequence::value_type& child )
	{
		return FSTreePtr( new FSTree_sys_mac_vol_N( child ) );
	}
	
	FSTreePtr sys_mac_proc_Details::ChildNode( const Sequence::value_type& child )
	{
		return FSTreePtr( new FSTree_sys_mac_proc_PSN( child ) );
	}
	
	
	FSTree_sys_mac_vol_N::FSTree_sys_mac_vol_N( const Key& key ) : itsKey( key )
	{
		FSSpec volume = N::FSMakeFSSpec( key, N::fsRtDirID, "\p" );
		
		Map( FSTreeFromFSSpec( volume ) );  // volume roots are named "mnt", not the volume name
	}
	
	FSTree_sys_mac_proc_PSN::FSTree_sys_mac_proc_PSN( const Key& key ) : itsKey( key )
	{
		Map( FSTreePtr( new FSTree_sys_mac_proc_PSN_exe( key ) ) );
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
	
	FSTreePtr Get_sys_mac_vol_N( Nitrogen::FSVolumeRefNum vRefNum )
	{
		return FSTreePtr( new FSTree_sys_mac_vol_N( vRefNum ) );
	}
	
}

