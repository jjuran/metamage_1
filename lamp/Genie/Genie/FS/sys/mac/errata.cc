/*
	Genie/FS/sys/mac/errata.cc
	--------------------------
*/

#include "Genie/FS/sys/mac/errata.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// Standard C++
#include <algorithm>

// mac-sys-utils
#include "mac_sys/get_machine_name.hh"
#include "mac_sys/unit_table.hh"

// plus
#include "plus/deconstruct_bool.hh"
#include "plus/var_string.hh"

// MacFeatures
#include "MacFeatures/BlueBoxed.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/property.hh"


namespace Genie
{
	
	struct RunningInClassic
	{
		static bool Test()  { return MacFeatures::Is_BlueBoxed(); }
	};
	
	
	struct RunningInRosetta
	{
		static bool Test()
		{
			if ( !TARGET_API_MAC_CARBON )
			{
				// 	Only Carbon apps can run in Rosetta
				return false;
			}
			
			const unsigned char* name = mac::sys::get_machine_name();
			
			const bool powerpc = name != NULL  &&  name[1] == 'P';
			
			return !powerpc;
		}
	};
	
	struct RunningInSheepShaver
	{
		static bool Test();
	};
	
	bool RunningInSheepShaver::Test()
	{
		using mac::types::AuxDCE;
		
		if ( TARGET_RT_MAC_MACHO )
		{
			return false;
		}
		
		const short n = mac::sys::get_unit_table_entry_count();
		
		AuxDCE*** const begin = mac::sys::get_unit_table_base();
		AuxDCE*** const end   = begin + n;
		
		for ( AuxDCE*** it = begin;  it < end;  ++it )
		{
			const unsigned char* name = mac::sys::get_driver_name( *it );
			
			if ( name[ 0 ] == 0 )
			{
				continue;
			}
			
			const bool undotted = name[ 1 ] != '.';
			
			const int cmp = memcmp( &".Display_Video_Apple_Sheep"[ undotted ],
			                        name + 1,
			                        name[ 0 ] );
			
			if ( cmp == 0 )
			{
				return true;
			}
			
			++it;
		}
		
		return false;
	}
	
	struct RunningInWeakEmulator
	{
		static bool Test()
		{
			return RunningInSheepShaver::Test() || RunningInRosetta::Test();
		}
	};
	
	
	template < class Erratum >
	struct sys_mac_errata_Property : readonly_property
	{
		static const int fixed_size = 1;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			plus::deconstruct_bool::apply( result, Erratum::Test(), binary );
		}
	};
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	const vfs::fixed_mapping sys_mac_errata_Mappings[] =
	{
		{ "async-io-race",            PROPERTY( sys_mac_errata_Property< RunningInClassic      > ) },
		{ "fatal-powerpc-exceptions", PROPERTY( sys_mac_errata_Property< RunningInWeakEmulator > ) },
		
		{ NULL, NULL }
	};
	
}

