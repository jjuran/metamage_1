/*	====================
 *	FSTree_crm_serial.cc
 *	====================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FileSystem/FSTree_crm_serial.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_sys_mac_crm.hh"


namespace Genie
{
	
	static N::CRMRecPtr GetCRMRecPtrFromID( N::CRMDeviceID id )
	{
		N::CRMResource_Container crmResources = N::CRMResources( N::crmSerialDevice );
		
		typedef N::CRMResource_Container::const_iterator Iter;
		
		for ( Iter it = crmResources.begin();  it != crmResources.end();  ++it )
		{
			N::CRMRecPtr rec = *it;
			
			if ( rec->crmDeviceID == id )
			{
				return rec;
			}
		}
		
		return NULL;
	}
	
	std::string CRMDeviceID_KeyName_Traits::NameFromKey( const Key& key )
	{
		return NN::Convert< std::string >( key );
	}
	
	CRMDeviceID_KeyName_Traits::Key CRMDeviceID_KeyName_Traits::KeyFromName( const std::string& name )
	{
		return Key( std::atoi( name.c_str() ) );
	}
	
	
	bool sys_mac_crm_serial_Details::KeyIsValid( const Key& key )
	{
		// We can't just call N::CRMSearch< N::crmSerialDevice >( key ),
		// because that returns the *next* record.  So, linear search it is.
		
		return GetCRMRecPtrFromID( key ) != NULL;
	}
	
	FSTreePtr sys_mac_crm_serial_Details::Parent() const
	{
		return GetSingleton< FSTree_sys_mac_crm >();
	}
	
	
	class sys_mac_crm_serial_N_name_Query
	{
		private:
			typedef N::CRMDeviceID            Key;
			typedef StringHandle              CRMSerialRecord::*Selector;
			
			Key itsKey;
			
			Selector itsSelector;
		
		public:
			sys_mac_crm_serial_N_name_Query( const Key&  key,
			                                 Selector    selector ) : itsKey     ( key      ),
			                                                          itsSelector( selector )
			{
			}
			
			std::string operator()() const
			{
				N::CRMRecPtr crmRec = GetCRMRecPtrFromID( itsKey );
				
				N::CRMSerialPtr serialPtr = NN::Convert< N::CRMSerialPtr >( crmRec );
				
				StringHandle h = serialPtr->*itsSelector;
				
				std::string name = NN::Convert< std::string >( N::Str255( *h ) );
				
				std::string output = name + "\n";
				
				return output;
			}
	};
	
	
	FSTree_sys_mac_crm_serial_N::FSTree_sys_mac_crm_serial_N( const Key& key ) : itsKey( key )
	{
		typedef FSTree_QueryFile< sys_mac_crm_serial_N_name_Query > QueryFile;
		
		typedef sys_mac_crm_serial_N_name_Query Query;
		
		std::string pathname = Pathname();
		
		Map( FSTreePtr( new QueryFile( pathname, "name",   Query( key, &CRMSerialRecord::name             ) ) ) );
		Map( FSTreePtr( new QueryFile( pathname, "input",  Query( key, &CRMSerialRecord::inputDriverName  ) ) ) );
		Map( FSTreePtr( new QueryFile( pathname, "output", Query( key, &CRMSerialRecord::outputDriverName ) ) ) );
	}
	
}

#endif

