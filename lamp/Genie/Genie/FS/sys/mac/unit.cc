/*
	Genie/FS/sys/mac/unit.cc
	------------------------
*/

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON

#include "Genie/FS/sys/mac/unit.hh"

// Standard C
#include <ctype.h>

// iota
#include "iota/decimal.hh"

// plus
#include "plus/contains.hh"
#include "plus/serialize.hh"

// Debug
#include "debug/assert.hh"

// nucleus
#include "nucleus/indexed_sequence.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/serialize_Str255.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Nitrogen
{
	
	struct UnitTable_Container_Specifics
	{
		typedef UInt16             size_type;
		typedef SInt16             difference_type;
		typedef AuxDCEHandle       value_type;
		typedef const value_type&  const_reference;
		typedef const value_type*  const_pointer;
		
		class Nothing {};
		
		typedef Nothing context_type;
		
		
		static size_type Size( context_type )
		{
			return LMGetUnitTableEntryCount();
		}
		
		static const_reference get_reference( context_type state, size_type position )
		{
			AuxDCEHandle* base = (AuxDCEHandle*) LMGetUTableBase();
			
			return base[ position ];
		}
		
		static const_pointer get_pointer( context_type state, size_type position )
		{
			return &get_reference( state, position );
		}
	};
	
	class UnitTable_Container: public nucleus::const_indexed_sequence< UnitTable_Container_Specifics >
	{
		friend UnitTable_Container UnitTable();
		
		private:
			UnitTable_Container()
			: nucleus::const_indexed_sequence< UnitTable_Container_Specifics >( UnitTable_Container_Specifics::Nothing() )
			{}
	};
	
	inline UnitTable_Container UnitTable()
	{
		return UnitTable_Container();
	}
	
	
	UnitTableDrivers_Container_Specifics::key_type
	//
	UnitTableDrivers_Container_Specifics::get_next_key( key_type key )
	{
		const key_type end = end_key();
		
		while ( ++key < end  &&  *key == NULL )
		{
			continue;
		}
		
		return key;
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	struct decode_unit_number
	{
		static unsigned apply( const plus::string& name )
		{
			return iota::parse_unsigned_decimal( name.c_str() );
		}
	};
	
	static UnitNumber GetKey( const FSTree* that )
	{
		return UnitNumber( decode_unit_number::apply( that->ParentRef()->Name() ) );
	}
	
	
	static inline AuxDCEHandle* GetUTableBase()
	{
		return (AuxDCEHandle*) LMGetUTableBase();
	}
	
	
	static bool is_valid_unit_number( UInt32 i )
	{
		AuxDCEHandle* base = GetUTableBase();
		
		const UInt16 count = LMGetUnitTableEntryCount();
		
		return i < count  &&  base[ i ] != NULL;
	}
	
	
	struct GetDriverFlags : plus::serialize_hex< short >
	{
		static short Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			return dceHandle[0]->dCtlFlags;
		}
	};
	
	const unsigned char* GetDriverName_WithinHandle( AuxDCEHandle dceHandle )
	{
		ASSERT( dceHandle != NULL );
		
		if ( dceHandle[0]->dCtlDriver != NULL )
		{
			const bool ramBased = dceHandle[0]->dCtlFlags & dRAMBasedMask;
			
			const Ptr drvr = dceHandle[0]->dCtlDriver;
			
			// Dereferences a handle if ramBased
			const DRVRHeaderPtr header = ramBased ? *(DRVRHeader **) drvr
			                                      :  (DRVRHeader * ) drvr;
			
			if ( header != NULL )
			{
				return header->drvrName;
			}
		}
		
		return "\p";
	}
	
	static N::Str255 GetDriverName( AuxDCEHandle dceHandle )
	{
		const unsigned char* name = GetDriverName_WithinHandle( dceHandle );
		
		// Safely copy Pascal string onto stack
		return N::Str255( name );
	}
	
	struct DriverName : serialize_Str255_contents
	{
		static N::Str255 Get( AuxDCEHandle dceHandle )
		{
			return GetDriverName( dceHandle );
		}
	};
	
	struct GetDriverSlot : plus::serialize_int< char >
	{
		// dCtlSlot is defined as 'char', but int is more debugger-friendly
		typedef int result_type;
		
		static int Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlSlot == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlSlot;
		}
	};
	
	struct GetDriverSlotId : plus::serialize_int< char >
	{
		// dCtlSlotId is defined as 'char', but int is more debugger-friendly
		typedef int result_type;
		
		static int Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlSlotId == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlSlotId;
		}
	};
	
	struct GetDriverBase : plus::serialize_hex< UInt32 >
	{
		static UInt32 Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlDevBase == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlDevBase;
		}
	};
	
	struct GetDriverOwner : plus::serialize_pointer
	{
		static Ptr Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlOwner == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlOwner;
		}
	};
	
	struct GetDriverExternalDeviceID : plus::serialize_int< char >
	{
		// dCtlExtDev is defined as 'char', but int is more debugger-friendly
		typedef int result_type;
		
		static int Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlExtDev == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlExtDev;
		}
	};
	
	template < class Accessor >
	struct sys_mac_unit_N_Property : readonly_property
	{
		static const size_t fixed_size = Accessor::fixed_size;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			UnitNumber key = GetKey( that );
			
			if ( !is_valid_unit_number( key ) )
			{
				throw FSTree_Property::Undefined();
			}
			
			AuxDCEHandle dceHandle = GetUTableBase()[ key ];
			
			const typename Accessor::result_type data = Accessor::Get( dceHandle );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
	};
	
	
	struct valid_name_of_unit_number
	{
		typedef canonical_positive_integer well_formed_name;
		
		static bool applies( const plus::string& name )
		{
			return well_formed_name::applies( name )  &&  is_valid_unit_number( decode_unit_number::apply( name ) );
		}
	};
	
	extern const FSTree_Premapped::Mapping sys_mac_unit_N_Mappings[];
	
	static FSTreePtr unit_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		if ( !valid_name_of_unit_number::applies( name ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return Premapped_Factory( parent, name, sys_mac_unit_N_Mappings );
	}
	
	class unit_IteratorConverter
	{
		public:
			FSNode operator()( N::UnitTableDrivers_Container::const_reference ref ) const
			{
				const int i = &ref - GetUTableBase();
				
				const ino_t inode = i;
				
				plus::string name = iota::inscribe_decimal( i );
				
				return FSNode( inode, name );
			}
	};
	
	static void unit_iterate( const FSTreePtr& parent, FSTreeCache& cache )
	{
		unit_IteratorConverter converter;
		
		N::UnitTableDrivers_Container sequence = N::UnitTableDrivers();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< sys_mac_unit_N_Property< prop > >::value
	
	const FSTree_Premapped::Mapping sys_mac_unit_N_Mappings[] =
	{
		{ "flags",  PROPERTY( GetDriverFlags            ) },
		{ "name",   PROPERTY( DriverName                ) },
		{ "slot",   PROPERTY( GetDriverSlot             ) },
		{ "id",     PROPERTY( GetDriverSlotId           ) },
		{ "base",   PROPERTY( GetDriverBase             ) },
		{ "owner",  PROPERTY( GetDriverOwner            ) },
		{ "extdev", PROPERTY( GetDriverExternalDeviceID ) },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_unit( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return new_basic_directory( parent, name, unit_lookup, unit_iterate );
	}
	
}

#endif

