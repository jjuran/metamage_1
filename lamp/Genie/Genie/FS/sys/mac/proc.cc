/*
	Genie/FS/sys/mac/proc.cc
	------------------------
*/

#include "Genie/FS/sys/mac/proc.hh"

// POSIX
#include <sys/stat.h>

// mac-sys-utils
#include "mac_sys/has/ProcessManager.hh"

// gear
#include "gear/hexadecimal.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/Processes.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/basic_directory.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/FSSpec.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static plus::string encoded_ProcessSerialNumber( const ProcessSerialNumber& psn )
	{
		const bool extended = psn.highLongOfPSN != 0;
		
		plus::string name;
		
		const size_t eight = sizeof psn.highLongOfPSN * 2;
		
		const size_t more = extended ? eight + sizeof '-' : 0;
		
		char* p = name.reset( more + eight );
		
		if ( extended )
		{
			gear::encode_32_bit_hex( psn.highLongOfPSN, p );
			
			p[ eight ] = '-';
		}
		
		gear::encode_32_bit_hex( psn.lowLongOfPSN, &p[ more ] );
		
		return name;
	}
	
	
	static ProcessSerialNumber decoded_ProcessSerialNumber( const plus::string& name )
	{
		ProcessSerialNumber psn = { 0, 0 };
		
		const char* begin = name.c_str();
		const char* end   = name.c_str() + name.size();
		
		UInt32 high = 0;
		
		if ( const char* hyphen = std::strchr( begin, '-' ) )
		{
			if ( hyphen - begin != sizeof (UInt32) * 2 )
			{
				return psn;
			}
			
			high = gear::decode_32_bit_hex( begin );
			
			if ( high == 0 )
			{
				return psn;
			}
			
			begin = hyphen + 1;
		}
		
		if ( end - begin != sizeof (UInt32) * 2 )
		{
			return psn;
		}
		
		psn.highLongOfPSN = high;
		
		psn.lowLongOfPSN = gear::decode_32_bit_hex( begin );
		
		return psn;
	}
	
	
	static ProcessSerialNumber GetKeyFromParent( const vfs::node* parent )
	{
		return decoded_ProcessSerialNumber( parent->name() );
	}
	
	
	static bool is_valid_ProcessSerialNumber( const ProcessSerialNumber& key )
	{
		if ( key.lowLongOfPSN == 0  &&  key.highLongOfPSN == 0 )
		{
			return false;
		}
		
		try
		{
			(void) N::GetProcessInformation( key );
		}
		catch ( const Mac::OSStatus& err )
		{
			if ( err != procNotFound )
			{
				throw;
			}
			
			return false;
		}
		
		return true;
	}
	
	
	extern const vfs::fixed_mapping sys_mac_proc_PSN_Mappings[];
	
	static vfs::node_ptr psn_lookup( const vfs::node* parent, const plus::string& name )
	{
		const ProcessSerialNumber psn = decoded_ProcessSerialNumber( name.c_str() );
		
		const bool canonical = encoded_ProcessSerialNumber( psn ) == name;
		
		if ( !canonical  ||  !is_valid_ProcessSerialNumber( psn ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return fixed_dir( parent, name, sys_mac_proc_PSN_Mappings );
	}
	
	static void psn_iterate( const vfs::node* parent, vfs::dir_contents& cache )
	{
		ProcessSerialNumber psn = { 0, kNoProcess };
		
		while ( true )
		{
			OSErr err = GetNextProcess( &psn );
			
			if ( err == procNotFound )
			{
				break;
			}
			
			Mac::ThrowOSStatus( err );
			
			const ino_t inode = 0;
			
			plus::string name = encoded_ProcessSerialNumber( psn );
			
			cache.push_back( vfs::dir_entry( inode, name ) );
		}
	}
	
	
	static
	void name_get( plus::var_string& result, const vfs::node* that, bool binary, const plus::string& name )
	{
		ProcessSerialNumber key = GetKeyFromParent( that );
		
		Str255 procName;
		
		ProcessInfoRec processInfo;
		
		nucleus::initialize< ProcessInfoRec >( processInfo );
		
		processInfo.processName = procName;
		
		N::GetProcessInformation( key, processInfo );
		
		result = procName;
		
		if ( name[ 0 ] != '.' )
		{
			result = plus::utf8_from_mac( result );
		}
	}
	
	static const vfs::property_params sys_mac_proc_PSN_name_params =
	{
		vfs::no_fixed_size,
		(vfs::property_get_hook) &name_get,
	};
	
	static vfs::node_ptr mac_proc_exe_resolve( const vfs::node* that )
	{
		ProcessSerialNumber psn = GetKeyFromParent( that->owner() );
		
		const FSSpec file = N::GetProcessAppSpec( psn );
		
		return FSTreeFromFSSpec( file );
	}
	
	static const vfs::link_method_set mac_proc_exe_link_methods =
	{
		NULL,
		&mac_proc_exe_resolve
	};
	
	static const vfs::node_method_set mac_proc_exe_methods =
	{
		NULL,
		NULL,
		&mac_proc_exe_link_methods
	};
	
	
	static vfs::node_ptr Executable_Factory( const vfs::node*     parent,
	                                         const plus::string&  name,
	                                         const void*          args )
	{
		return new vfs::node( parent, name, S_IFLNK | 0777, &mac_proc_exe_methods );
	}
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_proc_PSN_Mappings[] =
	{
		{ ".mac-name", PROPERTY( sys_mac_proc_PSN_name ) },
		{ "name",      PROPERTY( sys_mac_proc_PSN_name ) },
		
		{ "exe", &Executable_Factory },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_sys_mac_proc( const vfs::node*     parent,
	                                       const plus::string&  name,
	                                       const void*          args )
	{
		if ( ! mac::sys::has_ProcessManager() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return vfs::new_basic_directory( parent, name, psn_lookup, psn_iterate );
	}
	
}
