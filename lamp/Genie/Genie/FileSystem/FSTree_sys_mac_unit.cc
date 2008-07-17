/*	======================
 *	FSTree_sys_mac_unit.cc
 *	======================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FileSystem/FSTree_sys_mac_unit.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_sys_mac.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	AuxDCEHandle* GetUTableBase()
	{
		return (AuxDCEHandle*) LMGetUTableBase();
	}
	
	std::string UnitNumber_KeyName_Traits::NameFromKey( const Key& key )
	{
		return NN::Convert< std::string >( key );
	}
	
	UnitNumber_KeyName_Traits::Key UnitNumber_KeyName_Traits::KeyFromName( const std::string& name )
	{
		return std::atoi( name.c_str() );
	}
	
	
	bool sys_mac_unit_Details::KeyIsValid( const Key& key )
	{
		AuxDCEHandle* base = GetUTableBase();
		
		const UInt16 count = LMGetUnitTableEntryCount();
		
		//return key < count  &&  base[ key ] != NULL;
		return key < count;
	}
	
	class sys_mac_unit_N_name_Query
	{
		private:
			typedef UnitNumber Key;
			
			Key itsKey;
		
		public:
			sys_mac_unit_N_name_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string operator()() const
			{
				std::string name;
				
				if ( sys_mac_unit_Details::KeyIsValid( itsKey ) )
				{
					AuxDCEHandle dceHandle = GetUTableBase()[ itsKey ];
					
					if ( dceHandle != NULL  &&  dceHandle[0]->dCtlDriver != NULL )
					{
						const bool ramBased = dceHandle[0]->dCtlFlags & dRAMBasedMask;
						
						// Dereferences a handle if ramBased
						DRVRHeaderPtr header = ramBased ? *reinterpret_cast< DRVRHeader** >( dceHandle[0]->dCtlDriver )
						                                :  reinterpret_cast< DRVRHeader*  >( dceHandle[0]->dCtlDriver );
						
						// Copy Pascal string onto stack before we allocate memory
						name = NN::Convert< std::string >( N::Str255( header->drvrName ) );
					}
				}
				
				std::string output = name + "\n";
				
				return output;
			}
	};
	
	
	FSTreePtr sys_mac_unit_Details::Parent() const
	{
		return GetSingleton< FSTree_sys_mac >();
	}
	
	
	static FSTreePtr Name_Factory( const FSTreePtr&                 parent,
	                               const std::string&               name,
	                               UnitNumber_KeyName_Traits::Key   key )
	{
		typedef sys_mac_unit_N_name_Query Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name, Query( key ) ) );
	}
	
	void FSTree_sys_mac_unit_N::Init()
	{
		Map( "name", &Name_Factory );
	}
	
}

#endif

