/*	=============
 *	FSTree_sys.cc
 *	=============
 */

#include "Genie/FS/FSTree_sys.hh"

// iota
#include "iota/hexidecimal.hh"

// Nitrogen
#include "Nitrogen/Sound.h"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/Exec/GetMainEntry.hh"
#include "Genie/FS/FSTree_sys_app.hh"
#include "Genie/FS/FSTree_sys_cpu.hh"
#include "Genie/FS/FSTree_sys_mac.hh"
#include "Genie/FS/FSTree_sys_window.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	typedef const SystemCall* SystemCallPtr;
	
	struct SystemCall_KeyName_Traits
	{
		typedef SystemCallPtr Key;
		
		static std::string NameFromKey( const Key& key )
		{
			if ( key->name == NULL )
			{
				std::string name;
				
				name.resize( sizeof '.' + sizeof (unsigned) * 2 );  // 9
				
				name[0] = '.';
				
				iota::encode_32_bit_hex( (unsigned) key, &name[1] );
				
				return name;
			}
			
			return key->name;
		}
		
		static Key CheckKey( Key key )
		{
			if ( key == NULL )
			{
				p7::throw_errno( ENOENT );
			}
			
			return key;
		}
		
		static Key KeyFromName( const std::string& name )
		{
			return CheckKey( LookUpSystemCallByName( name.c_str() ) );
		}
	};
	
	
	class FSTree_sys_kernel_bin_EXE : public FSTree
	{
		private:
			MainEntry itsMainEntry;
		
		public:
			FSTree_sys_kernel_bin_EXE( const FSTreePtr&    parent,
			                           const std::string&  name,
			                           Trivial_Entry       main ) : FSTree( parent, name ),
			                                                        itsMainEntry( GetMainEntryFromAddress( main ) )
			{
			}
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IXUSR; }
			
			MainEntry GetMainEntry() const  { return itsMainEntry; }
	};
	
	
	struct sys_kernel_syscall_Details : public SystemCall_KeyName_Traits
	{
		typedef SystemCallRegistry Sequence;
		
		static const Sequence& ItemSequence()  { return GetSystemCallRegistry(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return &value; }
		
		static bool KeyIsValid( const Key& key )
		{
			return key != NULL;
		}
		
		FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                        const std::string&  name,
		                        const Key&          key ) const;
	};
	
	typedef FSTree_Sequence< sys_kernel_syscall_Details > FSTree_sys_kernel_syscall;
	
	
	template < int (*main)() >
	static void main_and_exit()
	{
		CurrentProcess().Exit( main() );
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
		return FSTreePtr( new FSTree_sys_kernel_bin_EXE( parent, name, main_and_exit< main > ) );
	}
	
	extern const FSTree_Premapped::Mapping sys_kernel_bin_Mappings[];
	
	const FSTree_Premapped::Mapping sys_kernel_bin_Mappings[] =
	{
		{ "true",  &Executable_Factory< main_true  > },
		{ "false", &Executable_Factory< main_false > },
		
		{ "beep", &Executable_Factory< main_beep > },
		
		{ NULL, NULL }
	};
	
	
	extern const FSTree_Premapped::Mapping sys_kernel_Mappings[];
	
	const FSTree_Premapped::Mapping sys_kernel_Mappings[] =
	{
		{ "bin",     &Premapped_Factory< sys_kernel_bin_Mappings > },
		
		{ "syscall", &Basic_Factory< FSTree_sys_kernel_syscall > },
		
		{ NULL, NULL }
	};
	
	const FSTree_Premapped::Mapping sys_Mappings[] =
	{
		{ "app",    &Premapped_Factory< sys_app_Mappings    > },
		{ "cpu",    &Premapped_Factory< sys_cpu_Mappings    > },
		{ "kernel", &Premapped_Factory< sys_kernel_Mappings > },
		{ "mac",    &Premapped_Factory< sys_mac_Mappings    > },
		
		{ "window", &Basic_Factory< FSTree_sys_window > },
		
		{ NULL, NULL }
	};
	
	
	FSTreePtr sys_kernel_syscall_Details::GetChildNode( const FSTreePtr&    parent,
	                                                    const std::string&  name,
	                                                    const Key&          key ) const
	{
		return FSTreePtr( new FSTree_Premapped( parent, name ) );
	}
	
}

