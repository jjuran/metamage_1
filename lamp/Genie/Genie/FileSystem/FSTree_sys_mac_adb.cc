/*	=====================
 *	FSTree_sys_mac_adb.cc
 *	=====================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FileSystem/FSTree_sys_mac_adb.hh"

// BitsAndBytes
#include "HexStrings.hh"

// Arcana
#include "ADBProtocol.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	bool sys_mac_adb_Details::KeyIsValid( const Key& key )
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
	
	
	extern const Functional_Traits< ADBAddress_KeyName_Traits::Key >::Mapping sys_mac_adb_N_Mappings[];
	
	FSTreePtr sys_mac_adb_Details::GetChildNode( const FSTreePtr&    parent,
		                                         const std::string&  name,
		                                         const Key&          key )
	{
		return Premapped_Factory< Key, sys_mac_adb_N_Mappings >( parent, name, key );
	}
	
	
	class sys_mac_adb_N_type_Query
	{
		private:
			typedef N::ADBAddress Key;
			
			Key itsKey;
		
		public:
			sys_mac_adb_N_type_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string Get() const
			{
				using BitsAndBytes::ByteAsHex;
				
				ADBDataBlock data = N::GetADBInfo( itsKey );
				
				std::string value = ByteAsHex( data.devType );
				
				std::string output = value + "\n";
				
				return output;
			}
	};
	
	class sys_mac_adb_N_origin_Query
	{
		private:
			typedef N::ADBAddress Key;
			
			Key itsKey;
		
		public:
			sys_mac_adb_N_origin_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string Get() const
			{
				using BitsAndBytes::ByteAsHex;
				
				ADBDataBlock data = N::GetADBInfo( itsKey );
				
				std::string output = "0\n";
				
				output[0] += data.origADBAddr;
				
				return output;
			}
	};
	
	static void WriteADBRegister( N::ADBAddress address, int i, std::string& result )
	{
		using BitsAndBytes::ByteAsHex;
		
		ADBRegister reg = GetADBRegister( address, i );
		
		for ( unsigned i = 1;  i <= reg.buffer[0];  ++i )
		{
			result += ByteAsHex( reg.buffer[i] );
			
			result += ' ';
		}
		
		if ( reg.buffer[0] != 0 )
		{
			result.resize( result.size() - 1 );  // kill extra space
		}
		
		result += '\n';
	}
	
	class sys_mac_adb_N_registers_Query
	{
		private:
			typedef N::ADBAddress Key;
			
			Key itsKey;
		
		public:
			sys_mac_adb_N_registers_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string Get() const
			{
				using BitsAndBytes::ByteAsHex;
				
				std::string output;
				
				WriteADBRegister( itsKey, 0, output );
				WriteADBRegister( itsKey, 1, output );
				WriteADBRegister( itsKey, 2, output );
				WriteADBRegister( itsKey, 3, output );
				
				return output;
			}
	};
	
	
	static FSTreePtr Type_Factory( const FSTreePtr&                parent,
	                               const std::string&              name,
	                               ADBAddress_KeyName_Traits::Key  key )
	{
		typedef sys_mac_adb_N_type_Query Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return FSTreePtr( new QueryFile( parent, name, Query( key ) ) );
	}
	
	static FSTreePtr Origin_Factory( const FSTreePtr&                parent,
	                                 const std::string&              name,
	                                 ADBAddress_KeyName_Traits::Key  key )
	{
		typedef sys_mac_adb_N_origin_Query Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return FSTreePtr( new QueryFile( parent, name, Query( key ) ) );
	}
	
	static FSTreePtr Registers_Factory( const FSTreePtr&                parent,
	                                    const std::string&              name,
	                                    ADBAddress_KeyName_Traits::Key  key )
	{
		typedef sys_mac_adb_N_registers_Query Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return FSTreePtr( new QueryFile( parent, name, Query( key ) ) );
	}
	
	const Functional_Traits< ADBAddress_KeyName_Traits::Key >::Mapping sys_mac_adb_N_Mappings[] =
	{
		{ "type",      &Type_Factory      },
		{ "origin",    &Origin_Factory    },
		{ "registers", &Registers_Factory },
		
		{ NULL, NULL }
	};
	
}

#endif

