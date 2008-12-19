/*	======================
 *	FSTree_sys_mac_unit.cc
 *	======================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FileSystem/FSTree_sys_mac_unit.hh"

// BitsAndBytes
#include "HexStrings.hh"

// Genie
#include "Genie/FileSystem/FSTree_Property.hh"


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
		
		static std::string Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			short flags = dceHandle[0]->dCtlFlags;
			
			return BitsAndBytes::EncodeAsHex( &flags, sizeof flags );
		}
	};
	
	struct GetDriverName
	{
		typedef std::string Result;
		
		static std::string Get( AuxDCEHandle dceHandle )
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
		
		static Result Get( AuxDCEHandle dceHandle )
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
		
		static Result Get( AuxDCEHandle dceHandle )
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
	struct sys_mac_unit_N_Property
	{
		private:
			typedef UnitNumber Key;
		
		public:
			static std::string Read( Key key )
			{
				if ( !sys_mac_unit_Details::KeyIsValid( key ) )
				{
					throw FSTree_Property::Undefined();
				}
				
				AuxDCEHandle dceHandle = GetUTableBase()[ key ];
				
				return NN::Convert< std::string >( Accessor::Get( dceHandle ) );
			}
	};
	
	
	extern const Functional_Traits< void >::Mapping sys_mac_unit_N_Mappings[];
	
	FSTreePtr sys_mac_unit_Details::GetChildNode( const FSTreePtr&    parent,
		                                          const std::string&  name,
		                                          const Key&          key )
	{
		return Premapped_Factory< sys_mac_unit_N_Mappings >( parent, name );
	}
	
	
	static UnitNumber GetKey( const FSTree* that )
	{
		return UnitNumber_KeyName_Traits::KeyFromName( that->ParentRef()->Name() );
	}
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_mac_unit_N_Property< Accessor > Property;
		
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &GetKey,
		                                       &Property::Read ) );
	}
	
	const Functional_Traits< void >::Mapping sys_mac_unit_N_Mappings[] =
	{
		{ "flags", &Property_Factory< GetDriverFlags > },
		{ "name",  &Property_Factory< GetDriverName  > },
		{ "slot",  &Property_Factory< GetDriverSlot  >, true },
		{ "base",  &Property_Factory< GetDriverBase  >, true },
		
		{ NULL, NULL }
	};
	
}

#endif

