/*	====================
 *	FSTree_crm_serial.cc
 *	====================
 */

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
			typedef N::CRMDeviceID Key;
			
			Key itsKey;
		
		public:
			sys_mac_crm_serial_N_name_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string operator()() const
			{
				N::CRMRecPtr crmRec = GetCRMRecPtrFromID( itsKey );
				
				N::CRMSerialPtr serialPtr = NN::Convert< N::CRMSerialPtr >( crmRec );
				
				std::string name = NN::Convert< std::string >( N::Str255( *serialPtr->name ) );
				
				std::string output = name + "\n";
				
				return output;
			}
	};
	
	
	FSTree_sys_mac_crm_serial_N::FSTree_sys_mac_crm_serial_N( const Key& key ) : itsKey( key )
	{
		Map( FSTreePtr( new FSTree_QueryFile< sys_mac_crm_serial_N_name_Query >( Pathname(),
		                                                                        "name",
		                                                                        sys_mac_crm_serial_N_name_Query( key ) ) ) );
	}
	
}

