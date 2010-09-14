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
#include "Genie/FS/stringify.hh"
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
	
	
	struct GetDriverFlags
	{
		static const bool alwaysStringified = false;
		
		typedef short Result;
		
		typedef stringify_16_bit_hex stringify;
		
		static Result Get( AuxDCEHandle dceHandle )
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
	
	struct DriverName
	{
		static const bool alwaysStringified = true;
		
		typedef N::Str255 Result;
		
		typedef stringify_pascal_string stringify;
		
		static Result Get( AuxDCEHandle dceHandle )
		{
			return GetDriverName( dceHandle );
		}
	};
	
	struct GetDriverSlot
	{
		static const bool alwaysStringified = false;
		
		// dCtlSlot is defined as 'char', but we want integer conversion
		typedef UInt16 Result;
		
		typedef stringify_short stringify;
		
		static Result Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlSlot == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlSlot;
		}
	};
	
	struct GetDriverBase
	{
		static const bool alwaysStringified = false;
		
		typedef long Result;
		
		typedef stringify_32_bit_hex stringify;
		
		static Result Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlDevBase == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlDevBase;
		}
	};
	
	template < class Accessor >
	struct sys_mac_unit_N_Property
	{
		static void Read( plus::var_string& result, const FSTree* that, bool binary )
		{
			UnitNumber key = GetKey( that );
			
			if ( !is_valid_unit_number( key ) )
			{
				throw FSTree_Property::Undefined();
			}
			
			AuxDCEHandle dceHandle = GetUTableBase()[ key ];
			
			const typename Accessor::Result data = Accessor::Get( dceHandle );
			
			result = Accessor::stringify::apply( data, binary );
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
		
		return Premapped_Factory< sys_mac_unit_N_Mappings >( parent, name, NULL );
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
	
	static void unit_iterate( FSTreeCache& cache )
	{
		unit_IteratorConverter converter;
		
		N::UnitTableDrivers_Container sequence = N::UnitTableDrivers();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		typedef sys_mac_unit_N_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_unit_N_Mappings[] =
	{
		{ "flags", &Property_Factory< GetDriverFlags > },
		{ "name",  &Property_Factory< DriverName  > },
		{ "slot",  &Property_Factory< GetDriverSlot  > },
		{ "base",  &Property_Factory< GetDriverBase  > },
		
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

