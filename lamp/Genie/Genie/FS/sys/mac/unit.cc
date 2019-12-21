/*
	Genie/FS/sys/mac/unit.cc
	------------------------
*/

#include "Genie/FS/sys/mac/unit.hh"

// mac-types
#include "mac_types/AuxDCE.hh"

// mac-sys-utils
#include "mac_sys/unit_table.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/contains.hh"
#include "plus/serialize.hh"
#include "plus/string.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/basic_directory.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/serialize_Str255.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	using mac::types::AuxDCEHandle;
	
	typedef char*           Ptr;
	typedef unsigned long   UInt32;
	typedef unsigned short  UnitNumber;
	
	
	struct decode_unit_number
	{
		static unsigned apply( const plus::string& name )
		{
			return gear::parse_unsigned_decimal( name.c_str() );
		}
	};
	
	static UnitNumber GetKey( const vfs::node* that )
	{
		return UnitNumber( decode_unit_number::apply( that->name() ) );
	}
	
	
	static bool is_valid_unit_number( UInt32 i )
	{
		AuxDCEHandle* base = mac::sys::get_unit_table_base();
		
		const short count = mac::sys::get_unit_table_entry_count();
		
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
	
	static N::Str255 GetDriverName( AuxDCEHandle dceHandle )
	{
		const unsigned char* name = mac::sys::get_driver_name( dceHandle );
		
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
	
	static
	AuxDCEHandle get_node_dce( const vfs::node* that )
	{
		UnitNumber key = GetKey( that );
		
		if ( !is_valid_unit_number( key ) )
		{
			throw vfs::undefined_property();
		}
		
		return mac::sys::get_unit_table_base()[ key ];
	}
	
	#define DEFINE_GETTER( p )  \
	static void p##_get( plus::var_string& result, const vfs::node* that, bool binary )  \
	{  \
		typedef p Accessor;                                             \
		AuxDCEHandle dceHandle = get_node_dce( that );                  \
		const Accessor::result_type data = Accessor::Get( dceHandle );  \
		Accessor::deconstruct::apply( result, data, binary );           \
	}
	
	DEFINE_GETTER( GetDriverFlags            );
	DEFINE_GETTER( DriverName                );
	DEFINE_GETTER( GetDriverSlot             );
	DEFINE_GETTER( GetDriverSlotId           );
	DEFINE_GETTER( GetDriverBase             );
	DEFINE_GETTER( GetDriverOwner            );
	DEFINE_GETTER( GetDriverExternalDeviceID );
	
	#define DEFINE_PARAMS( p )  \
	static const vfs::property_params p##_params = {p::fixed_size, &p##_get}
	
	DEFINE_PARAMS( GetDriverFlags            );
	DEFINE_PARAMS( DriverName                );
	DEFINE_PARAMS( GetDriverSlot             );
	DEFINE_PARAMS( GetDriverSlotId           );
	DEFINE_PARAMS( GetDriverBase             );
	DEFINE_PARAMS( GetDriverOwner            );
	DEFINE_PARAMS( GetDriverExternalDeviceID );
	
	struct valid_name_of_unit_number
	{
		typedef canonical_positive_integer well_formed_name;
		
		static bool applies( const plus::string& name )
		{
			return well_formed_name::applies( name )  &&  is_valid_unit_number( decode_unit_number::apply( name ) );
		}
	};
	
	extern const vfs::fixed_mapping sys_mac_unit_N_Mappings[];
	
	static vfs::node_ptr unit_lookup( const vfs::node* parent, const plus::string& name )
	{
		if ( !valid_name_of_unit_number::applies( name ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return fixed_dir( parent, name, sys_mac_unit_N_Mappings );
	}
	
	static void unit_iterate( const vfs::node* parent, vfs::dir_contents& cache )
	{
		const short n = mac::sys::get_unit_table_entry_count();
		
		AuxDCEHandle* it = mac::sys::get_unit_table_base();
		
		for ( int i = 0;  i < n;  ++i, ++it )
		{
			if ( *it )
			{
				const ino_t inode = i;
				
				plus::string name = gear::inscribe_decimal( i );
				
				cache.push_back( vfs::dir_entry( inode, name ) );
			}
		}
	}
	
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_unit_N_Mappings[] =
	{
		{ "flags",  PROPERTY( GetDriverFlags            ) },
		{ "name",   PROPERTY( DriverName                ) },
		{ "slot",   PROPERTY( GetDriverSlot             ) },
		{ "id",     PROPERTY( GetDriverSlotId           ) },
		{ "base",   PROPERTY( GetDriverBase             ) },
		{ "owner",  PROPERTY( GetDriverOwner            ) },
		{ "extdev", PROPERTY( GetDriverExternalDeviceID ) },
		
		{ ".~flags",  PROPERTY( GetDriverFlags            ) },
		{ ".~name",   PROPERTY( DriverName                ) },
		{ ".~slot",   PROPERTY( GetDriverSlot             ) },
		{ ".~id",     PROPERTY( GetDriverSlotId           ) },
		{ ".~base",   PROPERTY( GetDriverBase             ) },
		{ ".~owner",  PROPERTY( GetDriverOwner            ) },
		{ ".~extdev", PROPERTY( GetDriverExternalDeviceID ) },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_sys_mac_unit( const vfs::node*     parent,
	                                       const plus::string&  name,
	                                       const void*          args )
	{
		return vfs::new_basic_directory( parent, name, unit_lookup, unit_iterate );
	}
	
}
