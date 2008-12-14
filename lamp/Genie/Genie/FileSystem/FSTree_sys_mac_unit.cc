/*	======================
 *	FSTree_sys_mac_unit.cc
 *	======================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FileSystem/FSTree_sys_mac_unit.hh"

// BitsAndBytes
#include "HexStrings.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	AuxDCEHandle* GetUTableBase()
	{
		return (AuxDCEHandle*) LMGetUTableBase();
	}
	
	
	bool sys_mac_unit_Details::KeyIsValid( const Key& key )
	{
		AuxDCEHandle* base = GetUTableBase();
		
		const UInt16 count = LMGetUnitTableEntryCount();
		
		return key < count  &&  base[ key ] != NULL;
	}
	
	
	struct GetDriverFlags
	{
		typedef std::string Result;
		
		std::string Get( AuxDCEHandle dceHandle ) const
		{
			ASSERT( dceHandle != NULL );
			
			short flags = dceHandle[0]->dCtlFlags;
			
			return BitsAndBytes::EncodeAsHex( &flags, sizeof flags );
		}
	};
	
	struct GetDriverName
	{
		typedef std::string Result;
		
		std::string Get( AuxDCEHandle dceHandle ) const
		{
			ASSERT( dceHandle != NULL );
			
			std::string name;
			
			if ( dceHandle[0]->dCtlDriver != NULL )
			{
				const bool ramBased = dceHandle[0]->dCtlFlags & dRAMBasedMask;
				
				// Dereferences a handle if ramBased
				DRVRHeaderPtr header = ramBased ? *reinterpret_cast< DRVRHeader** >( dceHandle[0]->dCtlDriver )
				                                :  reinterpret_cast< DRVRHeader*  >( dceHandle[0]->dCtlDriver );
				
				// Copy Pascal string onto stack before we allocate memory
				name = NN::Convert< std::string >( N::Str255( header->drvrName ) );
			}
			
			return name;
		}
	};
	
	struct GetDriverSlot
	{
		// dCtlSlot is defined as 'char', but we want integer conversion
		typedef UInt16 Result;
		
		Result Get( AuxDCEHandle dceHandle ) const
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlSlot == 0 )
			{
				N::ThrowOSStatus( fnfErr );
			}
			
			return dceHandle[0]->dCtlSlot;
		}
	};
	
	struct GetDriverBase
	{
		typedef std::string Result;
		
		Result Get( AuxDCEHandle dceHandle ) const
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlDevBase == 0 )
			{
				N::ThrowOSStatus( fnfErr );
			}
			
			long base = dceHandle[0]->dCtlDevBase;
			
			return BitsAndBytes::EncodeAsHex( &base, sizeof base );
		}
	};
	
	template < class Accessor >
	class sys_mac_unit_N_Query
	{
		private:
			typedef UnitNumber Key;
			
			Key itsKey;
			
			std::string itsValue;
		
		public:
			sys_mac_unit_N_Query( const Key& key ) : itsKey( key ), itsValue( Make() )
			{
			}
			
			const std::string& Get() const
			{
				return itsValue;
			}
			
			std::string Make() const
			{
				std::string output;
				
				if ( sys_mac_unit_Details::KeyIsValid( itsKey ) )
				{
					AuxDCEHandle dceHandle = GetUTableBase()[ itsKey ];
					
					output = NN::Convert< std::string >( Accessor().Get( dceHandle ) );
					
					output += "\n";
				}
				
				return output;
			}
	};
	
	
	extern const Functional_Traits< UnitNumber_KeyName_Traits::Key >::Mapping sys_mac_unit_N_Mappings[];
	
	FSTreePtr sys_mac_unit_Details::GetChildNode( const FSTreePtr&    parent,
		                                          const std::string&  name,
		                                          const Key&          key )
	{
		return Premapped_Factory< Key, sys_mac_unit_N_Mappings >( parent, name, key );
	}
	
	
	template < class Accessor >
	static FSTreePtr Query_Factory( const FSTreePtr&                parent,
	                                const std::string&              name,
	                                UnitNumber_KeyName_Traits::Key  key )
	{
		typedef sys_mac_unit_N_Query< Accessor > Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return FSTreePtr( new QueryFile( parent, name, Query( key ) ) );
	}
	
	const Functional_Traits< UnitNumber_KeyName_Traits::Key >::Mapping sys_mac_unit_N_Mappings[] =
	{
		{ "flags", &Query_Factory< GetDriverFlags > },
		{ "name",  &Query_Factory< GetDriverName  > },
		{ "slot",  &Query_Factory< GetDriverSlot  >, true },
		{ "base",  &Query_Factory< GetDriverBase  >, true },
		
		{ NULL, NULL }
	};
	
}

#endif

