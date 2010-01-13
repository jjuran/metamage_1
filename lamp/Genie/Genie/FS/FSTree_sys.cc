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
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTree_sys_app.hh"
#include "Genie/FS/FSTree_sys_cpu.hh"
#include "Genie/FS/FSTree_sys_mac.hh"
#include "Genie/FS/FSTree_sys_window.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	typedef const SystemCall* SystemCallPtr;
	
	
	static std::string name_of_syscall( SystemCallPtr key )
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
	
	
	static FSTreePtr syscall_lookup( const FSTreePtr& parent, const std::string& name )
	{
		if ( LookUpSystemCallByName( name.c_str() ) == NULL )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return seize_ptr( new FSTree_Premapped( parent, name ) );
	}
	
	class syscall_IteratorConverter
	{
		public:
			FSNode operator()( const SystemCall& value ) const
			{
				const ino_t inode = &value - GetSystemCall( 0 );
				
				std::string name = name_of_syscall( &value );
				
				return FSNode( inode, name );
			}
	};
	
	static void syscall_iterate( FSTreeCache& cache )
	{
		syscall_IteratorConverter converter;
		
		const SystemCallRegistry& sequence = GetSystemCallRegistry();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	static FSTreePtr New_sys_kernel_syscall( const FSTreePtr& parent, const std::string& name )
	{
		return new_basic_directory( parent, name, syscall_lookup, syscall_iterate );
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
		return seize_ptr( new FSTree_sys_kernel_bin_EXE( parent, name, main ) );
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
		
		{ "syscall", &New_sys_kernel_syscall },
		
		{ NULL, NULL }
	};
	
	const FSTree_Premapped::Mapping sys_Mappings[] =
	{
		{ "app",    &Premapped_Factory< sys_app_Mappings    > },
		{ "cpu",    &Premapped_Factory< sys_cpu_Mappings    > },
		{ "kernel", &Premapped_Factory< sys_kernel_Mappings > },
		{ "mac",    &Premapped_Factory< sys_mac_Mappings    > },
		
		{ "window", &New_FSTree_sys_window },
		
		{ NULL, NULL }
	};
	
}

