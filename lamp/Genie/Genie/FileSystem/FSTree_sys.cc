/*	=============
 *	FSTree_sys.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_sys.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/MacWindows.h"
#include "Nitrogen/Processes.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/IO/Base.hh"
#include "Genie/IO/Device.hh"
#include "Genie/IO/MemoryFile.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	typedef const SystemCall* SystemCallPtr;
	
	struct SystemCall_KeyName_Traits
	{
		typedef SystemCallPtr Key;
		
		static std::string NameFromKey( const Key& key )
		{
			return key->name ? key->name
			                 : "." + Pointer_KeyName_Traits< SystemCallPtr >::NameFromKey( key );
		}
		
		static Key CheckKey( Key key )
		{
			if ( key == NULL )  p7::throw_errno( ENOENT );
			
			return key;
		}
		
		static Key KeyFromName( const std::string& name )
		{
			return CheckKey( LookUpSystemCallByName( name.c_str() ) );
		}
	};
	
	struct WindowRef_KeyName_Traits : public Pointer_KeyName_Traits< N::WindowRef >  {};
	
	struct VRefNum_KeyName_Traits
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
			
			return vRefNum;
		}
		
		static bool KeyIsValid( const Key& key )
		{
			try
			{
				(void) N::FSMakeFSSpec( key, N::fsRtDirID, NULL );
			}
			catch ( const N::NSVErr& err )
			{
			#ifdef __MWERKS__
				
				if ( err.Get() != nsvErr )
				{
					throw;
				}
				
			#endif
				
				return false;
			}
			
			return true;
		}
	};
	
	struct ProcessSerialNumber_KeyName_Traits
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
	
	
	class FSTree_sys_kernel_syscall_PTR;
	
	
	struct sys_kernel_syscall_Details : public SystemCall_KeyName_Traits
	{
		typedef SystemCallRegistry Sequence;
		
		typedef FSTree_sys_kernel_syscall_PTR ChildNode;
		
		static std::string Name()  { return "syscall"; }
		
		static FSTreePtr Parent()  { return GetSingleton< FSTree_sys_kernel >(); }
		
		static const Sequence& ItemSequence()  { return GetSystemCallRegistry(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return &value; }
		
		static bool KeyIsValid( const Key& key )
		{
			return key != NULL;
		}
	};
	
	typedef FSTree_Sequence< sys_kernel_syscall_Details > FSTree_sys_kernel_syscall;
	
	class FSTree_sys_kernel_syscall_PTR : public FSTree_Virtual,
	                                      public SystemCall_KeyName_Traits
	{
		private:
			Key itsKey;
		
		public:
			FSTree_sys_kernel_syscall_PTR( const Key& key ) : itsKey( key )  {}
			
			std::string Name() const  { return NameFromKey( itsKey ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_kernel_syscall >(); }
	};
	
	
	class FSTree_sys_window_REF;
	
	
	struct sys_window_Details : public WindowRef_KeyName_Traits
	{
		typedef N::WindowList_Container Sequence;
		
		typedef FSTree_sys_window_REF ChildNode;
		
		static std::string Name()  { return "window"; }
		
		static FSTreePtr Parent()  { return GetSingleton< FSTree_sys >(); }
		
		static const Sequence& ItemSequence()  { return N::WindowList(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key )
		{
			const Sequence& sequence = ItemSequence();
			
			return std::find( sequence.begin(), sequence.end(), key ) != sequence.end();
		}
	};
	
	typedef FSTree_Sequence< sys_window_Details > FSTree_sys_window;
	
	class FSTree_sys_window_REF : public FSTree_Virtual,
	                              public WindowRef_KeyName_Traits
	{
		private:
			Key itsKey;
		
		public:
			FSTree_sys_window_REF( const Key& key ) : itsKey( key )  {}
			
			std::string Name() const  { return NameFromKey( itsKey ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_window >(); }
	};
	
	
	class FSTree_sys_mac : public FSTree_Virtual
	{
		public:
			FSTree_sys_mac();
			
			static std::string OnlyName()  { return "mac"; }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys >(); }
	};
	
	
	class FSTree_sys_mac_vol_N;
	
	
	struct sys_mac_vol_Details : public VRefNum_KeyName_Traits
	{
		typedef N::Volume_Container Sequence;
		
		typedef FSTree_sys_mac_vol_N ChildNode;
		
		static std::string Name()  { return "vol"; }
		
		static FSTreePtr Parent()  { return GetSingleton< FSTree_sys_mac >(); }
		
		static const Sequence& ItemSequence()  { return N::Volumes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
	};
	
	typedef FSTree_Sequence< sys_mac_vol_Details > FSTree_sys_mac_vol;
	
	class FSTree_sys_mac_vol_N : public FSTree_Virtual,
	                             public VRefNum_KeyName_Traits
	{
		private:
			Key itsKey;
		
		public:
			FSTree_sys_mac_vol_N( const Key& key );
			
			std::string Name() const  { return NameFromKey( itsKey ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_mac_vol >(); }
	};
	
	
	class FSTree_sys_mac_proc_PSN;
	
	struct sys_mac_proc_Details : public ProcessSerialNumber_KeyName_Traits
	{
		typedef N::Process_Container Sequence;
		
		typedef FSTree_sys_mac_proc_PSN ChildNode;
		
		static std::string Name()  { return "proc"; }
		
		static FSTreePtr Parent()  { return GetSingleton< FSTree_sys_mac >(); }
		
		static const Sequence& ItemSequence()  { return N::Processes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key )
		{
			try
			{
				(void) N::GetNextProcess( key );
			}
			catch ( const N::ProcNotFound& err )
			{
			#ifdef __MWERKS__
				
				if ( err.Get() != procNotFound )
				{
					throw;
				}
				
			#endif
				
				return false;
			}
			
			return true;
		}
	};
	
	typedef FSTree_Sequence< sys_mac_proc_Details > FSTree_sys_mac_proc;
	
	class FSTree_sys_mac_proc_PSN : public FSTree_Virtual,
	                                public ProcessSerialNumber_KeyName_Traits
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
	
	
	static UInt32 GetROMSize()
	{
		static UInt32 romSize = N::Gestalt( N::GestaltSelector( gestaltROMSize ) );
		
		return romSize;
	}
	
	class FSTree_sys_mac_rom : public FSTree
	{
		public:
			FSTree_sys_mac_rom()  {}
			
			std::string Name() const  { return "rom"; }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_mac >(); }
			
			mode_t FileTypeMode() const  { return S_IFREG; }
			
			mode_t FilePermMode() const  { return TARGET_API_MAC_CARBON ? 0 : S_IRUSR; }
			
			off_t GetEOF() const  { return GetROMSize(); }
			
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
		MapSingleton< FSTree_sys_kernel_bin     >();
		MapSingleton< FSTree_sys_kernel_syscall >();
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
		
		try
		{
			GetROMSize();
			
			MapSingleton< FSTree_sys_mac_rom >();
		}
		catch ( ... )
		{
		}
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
	
	
	boost::shared_ptr< IOHandle > FSTree_sys_mac_rom::Open( OpenFlags flags ) const
	{
	#if TARGET_API_MAC_CARBON
		
		p7::throw_errno( EPERM );
		
		// Not reached
		return boost::shared_ptr< IOHandle >();
		
	#else
		
		return boost::shared_ptr< IOHandle >( new MemoryFileHandle( shared_from_this(),
		                                                            LMGetROMBase(),
		                                                            GetEOF() ) );
		
	#endif
	}
	
	
	FSTreePtr Get_sys_mac_vol_N( Nitrogen::FSVolumeRefNum vRefNum )
	{
		return FSTreePtr( new FSTree_sys_mac_vol_N( vRefNum ) );
	}
	
}

