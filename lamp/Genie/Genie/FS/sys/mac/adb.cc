/*
	Genie/FS/sys/mac/adb.cc
	-----------------------
*/

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON

#include "Genie/FS/sys/mac/adb.hh"

// Standard C++
#include <algorithm>

// mac-config
#include "mac_config/adb.hh"

// iota
#include "iota/char_types.hh"

// mac-sys-utils
#include "mac_sys/has/ADB.hh"

// gear
#include "gear/hexadecimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// ClassicToolbox
#include "ClassicToolbox/DeskBus.hh"

// Arcana
#include "ADB/Protocol.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/basic_directory.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/generated_file.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/append_hex_encoded_byte.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static inline N::ADBAddress GetKeyFromParent( const vfs::node* parent )
	{
		return N::ADBAddress( gear::decoded_hex_digit( parent->name()[0] ) );
	}
	
	
	static bool ADBAddress_is_valid( N::ADBAddress key )
	{
		try
		{
			return N::GetADBInfo( key ).devType != 0;
		}
		catch ( ... )
		{
			return false;
		}
	}
	
	struct well_formed_ADBAddress_name
	{
		static bool applies( const plus::string& name )
		{
			return name.size() == 1  &&  name[0] & 0x20  && iota::is_xdigit( name[0] );
		}
	};
	
	static bool name_is_valid_ADBAddress( const plus::string& name )
	{
		if ( !well_formed_ADBAddress_name::applies( name ) )
		{
			return false;
		}
		
		const unsigned char hex_digit = gear::decoded_hex_digit( name[0] );
		
		return ADBAddress_is_valid( N::ADBAddress( hex_digit ) );
	}
	
	extern const vfs::fixed_mapping sys_mac_adb_N_Mappings[];
	
	static vfs::node_ptr adb_lookup( const vfs::node* parent, const plus::string& name )
	{
		if ( !name_is_valid_ADBAddress( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return fixed_dir( parent, name, sys_mac_adb_N_Mappings );
	}
	
	class adb_IteratorConverter
	{
		public:
			vfs::dir_entry operator()( N::ADBAddress addr ) const
			{
				const ino_t inode = addr;
				
				plus::string name( 1, gear::encoded_hex_char( addr ) );
				
				return vfs::dir_entry( inode, name );
			}
	};
	
	static void adb_iterate( const vfs::node* parent, vfs::dir_contents& cache )
	{
		adb_IteratorConverter converter;
		
		N::ADBDevice_Container sequence;
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	class sys_mac_adb_N_type : public vfs::readonly_property
	{
		private:
			typedef N::ADBAddress Key;
		
		public:
			static const int fixed_size = 2;
			
			static void get( plus::var_string& result, const vfs::node* that, bool binary )
			{
				Key key = GetKeyFromParent( that );
				
				ADBDataBlock data = N::GetADBInfo( key );
				
				append_hex_encoded_byte( result, data.devType );
			}
	};
	
	class sys_mac_adb_N_origin : public vfs::readonly_property
	{
		private:
			typedef N::ADBAddress Key;
		
		public:
			static const int fixed_size = 1;
			
			static void get( plus::var_string& result, const vfs::node* that, bool binary )
			{
				Key key = GetKeyFromParent( that );
				
				ADBDataBlock data = N::GetADBInfo( key );
				
				char c = '0' + data.origADBAddr;
				
				result = c;
			}
	};
	
	static void WriteADBRegister( N::ADBAddress address, int i, plus::var_string& result )
	{
		ADBRegister reg = GetADBRegister( address, i );
		
		for ( unsigned i = 1;  i <= reg.buffer[0];  ++i )
		{
			append_hex_encoded_byte( result, reg.buffer[i] );
			
			result += ' ';
		}
		
		if ( reg.buffer[0] != 0 )
		{
			result.resize( result.size() - 1 );  // kill extra space
		}
		
		result += '\n';
	}
	
	struct sys_mac_adb_N_registers
	{
		static plus::string Read( const vfs::node* parent, const plus::string& name )
		{
			N::ADBAddress key = GetKeyFromParent( parent );
			
			plus::var_string output;
			
			WriteADBRegister( key, 0, output );
			WriteADBRegister( key, 1, output );
			WriteADBRegister( key, 2, output );
			WriteADBRegister( key, 3, output );
			
			return output;
		}
	};
	
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< prop >::value
	
	const vfs::fixed_mapping sys_mac_adb_N_Mappings[] =
	{
		{ "type",      PROPERTY( sys_mac_adb_N_type   ) },
		{ "origin",    PROPERTY( sys_mac_adb_N_origin ) },
		
		{ "registers", &vfs::new_generated, (void*) &sys_mac_adb_N_registers::Read },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_sys_mac_adb( const vfs::node*     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		if ( ! (CONFIG_ADB  &&  (CONFIG_ADB_GRANTED  ||  mac::sys::has_ADB())) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return vfs::new_basic_directory( parent, name, adb_lookup, adb_iterate );
	}
	
}

#endif
