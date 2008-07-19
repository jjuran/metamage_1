/*	=============
 *	FSTree_sys.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_sys.hh"

// Nucleus
#include "Nucleus/ArrayContainerFunctions.h"

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
	
	
	class FSTree_sys_kernel : public FSTree_Functional_Singleton
	{
		public:
			FSTree_sys_kernel( const FSTreePtr&    parent,
			                   const std::string&  name ) : FSTree_Functional_Singleton( parent, name )
			{
			}
			
			void Init();
	};
	
	class FSTree_sys_kernel_bin : public FSTree_Functional_Singleton
	{
		public:
			FSTree_sys_kernel_bin( const FSTreePtr&    parent,
			                       const std::string&  name ) : FSTree_Functional_Singleton( parent, name )
			{
			}
			
			void Init();
	};
	
	class FSTree_sys_kernel_bin_EXE : public FSTree
	{
		private:
			typedef int (*Main0)();
			typedef int (*Main2)( int argc, char const *const *argv );
			typedef int (*Main3)( int argc, char const *const *argv, char const *const *envp );
			
			MainEntry itsMainEntry;
		
		public:
			FSTree_sys_kernel_bin_EXE( const FSTreePtr&    parent,
			                           const std::string&  name,
			                           Main3               main ) : FSTree( parent, name ),
			                                                        itsMainEntry( GetMainEntryFromAddress( main ) )
			{
			}
			
			FSTree_sys_kernel_bin_EXE( const FSTreePtr&    parent,
			                           const std::string&  name,
			                           Main2               main ) : FSTree( parent, name ),
			                                                        itsMainEntry( GetMainEntryFromAddress( main ) )
			{
			}
			
			FSTree_sys_kernel_bin_EXE( const FSTreePtr&    parent,
			                           const std::string&  name,
			                           Main0               main ) : FSTree( parent, name ),
			                                                        itsMainEntry( GetMainEntryFromAddress( main ) )
			{
			}
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IXUSR; }
			
			MainEntry GetMainEntry() const  { return itsMainEntry; }
	};
	
	
	class FSTree_sys_kernel_syscall_PTR;
	
	
	struct sys_kernel_syscall_Details : public SystemCall_KeyName_Traits
	{
		typedef SystemCallRegistry Sequence;
		
		typedef FSTree_sys_kernel_syscall_PTR ChildNode;
		
		static const Sequence& ItemSequence()  { return GetSystemCallRegistry(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return &value; }
		
		static bool KeyIsValid( const Key& key )
		{
			return key != NULL;
		}
		
		FSTreePtr GetChildNode( const FSTreePtr& parent, const Key& key ) const;
	};
	
	typedef FSTree_Sequence< sys_kernel_syscall_Details > FSTree_sys_kernel_syscall;
	
	class FSTree_sys_kernel_syscall_PTR : public FSTree_Functional_Singleton,
	                                      public SystemCall_KeyName_Traits
	{
		private:
			Key itsKey;
		
		public:
			FSTree_sys_kernel_syscall_PTR( const FSTreePtr&  parent,
			                               const Key&        key ) : FSTree_Functional_Singleton( parent ),
			                                                         itsKey( key )
			{
			}
			
			std::string Name() const  { return NameFromKey( itsKey ); }
	};
	
	
	static FSTree_Functional_Singleton::Mapping sys_Mappings[] =
	{
		{ "kernel", &Singleton_Factory< FSTree_sys_kernel > },
		{ "mac",    &Singleton_Factory< FSTree_sys_mac    > },
		{ "set",    &Singleton_Factory< FSTree_sys_set    > }
	};
	
	void FSTree_sys::Init()
	{
		AddMappings( sys_Mappings, NN::ArrayEnd( sys_Mappings ) );
	}
	
	static FSTree_Functional_Singleton::Mapping sys_kernel_Mappings[] =
	{
		{ "bin",     &Singleton_Factory< FSTree_sys_kernel_bin     > },
		{ "syscall", &Singleton_Factory< FSTree_sys_kernel_syscall > }
	};
	
	void FSTree_sys_kernel::Init()
	{
		AddMappings( sys_kernel_Mappings, NN::ArrayEnd( sys_kernel_Mappings ) );
	}
	
	namespace
	{
		
		int main_true()
		{
			return 0;
		}
		
		int main_false()
		{
			return 1;
		}
		
		int main_beep()
		{
			N::SysBeep();
			
			return 0;
		}
		
	}
	
	template < int (*main)() >
	static FSTreePtr Executable_Factory( const FSTreePtr&    parent,
	                                     const std::string&  name )
	{
		return MakeFSTree( new FSTree_sys_kernel_bin_EXE( parent, name, main ) );
	}
	
	static FSTree_Functional_Singleton::Mapping sys_kernel_bin_Mappings[] =
	{
		{ "true",  &Executable_Factory< main_true  > },
		{ "false", &Executable_Factory< main_false > },
		
		{ "beep", &Executable_Factory< main_beep > }
	};
	
	void FSTree_sys_kernel_bin::Init()
	{
		AddMappings( sys_kernel_bin_Mappings, NN::ArrayEnd( sys_kernel_bin_Mappings ) );
	}
	
	FSTreePtr sys_kernel_syscall_Details::GetChildNode( const FSTreePtr& parent, const Key& key ) const
	{
		return MakeFSTree( new FSTree_sys_kernel_syscall_PTR( parent, key ) );
	}
	
}

