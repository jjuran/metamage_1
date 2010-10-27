/*
	Genie/FS/sys/mac/adb.cc
	-----------------------
*/

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON

#include "Genie/FS/sys/mac/adb.hh"

// Standard C
#include <ctype.h>

// iota
#include "iota/hexidecimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// ClassicToolbox
#include "ClassicToolbox/DeskBus.hh"

// MacFeatures
#include "MacFeatures/ADB.hh"

// Arcana
#include "ADB/Protocol.hh"

// Genie
#include "Genie/FS/append_hex_encoded_byte.hh"
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Generated.hh"
#include "Genie/FS/FSTree_Property.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static inline N::ADBAddress GetKeyFromParent( const FSTreePtr& parent )
	{
		return N::ADBAddress( iota::decoded_hex_digit( parent->Name()[0] ) );
	}
	
	static N::ADBAddress GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
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
			return name.size() == 1  &&  name[0] & 0x20  && isxdigit( name[0] );
		}
	};
	
	static bool name_is_valid_ADBAddress( const plus::string& name )
	{
		if ( !well_formed_ADBAddress_name::applies( name ) )
		{
			return false;
		}
		
		const unsigned char hex_digit = iota::decoded_hex_digit( name[0] );
		
		return ADBAddress_is_valid( N::ADBAddress( hex_digit ) );
	}
	
	extern const FSTree_Premapped::Mapping sys_mac_adb_N_Mappings[];
	
	static FSTreePtr adb_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		if ( !name_is_valid_ADBAddress( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return Premapped_Factory( parent, name, sys_mac_adb_N_Mappings );
	}
	
	class adb_IteratorConverter
	{
		public:
			FSNode operator()( N::ADBAddress addr ) const
			{
				const ino_t inode = addr;
				
				plus::string name( 1, iota::encoded_hex_char( addr ) );
				
				return FSNode( inode, name );
			}
	};
	
	static void adb_iterate( FSTreeCache& cache )
	{
		adb_IteratorConverter converter;
		
		N::ADBDevice_Container sequence;
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	class sys_mac_adb_N_type
	{
		private:
			typedef N::ADBAddress Key;
		
		public:
			static void Read( plus::var_string& result, const FSTree* that, bool binary )
			{
				Key key = GetKey( that );
				
				ADBDataBlock data = N::GetADBInfo( key );
				
				append_hex_encoded_byte( result, data.devType );
			}
	};
	
	class sys_mac_adb_N_origin
	{
		private:
			typedef N::ADBAddress Key;
		
		public:
			static void Read( plus::var_string& result, const FSTree* that, bool binary )
			{
				Key key = GetKey( that );
				
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
	
	class sys_mac_adb_N_registers
	{
		private:
			typedef N::ADBAddress Key;
		
		public:
			static plus::string Read( const FSTree* that )
			{
				Key key = GetKey( that );
				
				plus::var_string output;
				
				WriteADBRegister( key, 0, output );
				WriteADBRegister( key, 1, output );
				WriteADBRegister( key, 2, output );
				WriteADBRegister( key, 3, output );
				
				return output;
			}
	};
	
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	static FSTreePtr Registers_Factory( const FSTreePtr&     parent,
	                                    const plus::string&  name,
	                                    const void*          args )
	{
		return New_FSTree_Generated( parent,
		                             name,
		                             &sys_mac_adb_N_registers::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_adb_N_Mappings[] =
	{
		{ "type",      &Property_Factory< sys_mac_adb_N_type   > },
		{ "origin",    &Property_Factory< sys_mac_adb_N_origin > },
		{ "registers", &Registers_Factory                        },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_adb( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		if ( !MacFeatures::Has_ADB() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return new_basic_directory( parent, name, adb_lookup, adb_iterate );
	}
	
}

#endif

