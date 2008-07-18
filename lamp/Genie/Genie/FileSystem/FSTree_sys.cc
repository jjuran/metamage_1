/*	=============
 *	FSTree_sys.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_sys.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/Processes.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_sys_mac.hh"
#include "Genie/FileSystem/FSTree_sys_set.hh"
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
	
	
	class FSTree_sys_kernel : public FSTree_Functional< Singleton_Functional_Details >
	{
		public:
			void Init();
			
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
		
		FSTreePtr GetChildNode( const FSTreePtr& parent, const Key& key ) const;
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
	
	
	void FSTree_sys::Init()
	{
		MapSingleton< FSTree_sys_kernel >();
		MapSingleton< FSTree_sys_mac    >();
		MapSingleton< FSTree_sys_set    >();
	}
	
	void FSTree_sys_kernel::Init()
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
	
	FSTreePtr sys_kernel_syscall_Details::GetChildNode( const FSTreePtr& parent, const Key& key ) const
	{
		return MakeFSTree( new FSTree_sys_kernel_syscall_PTR( key ) );
	}
	
}

