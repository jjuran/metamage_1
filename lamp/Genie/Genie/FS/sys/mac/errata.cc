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

// mac-sys-utils
#include "mac_sys/get_machine_name.hh"
#include "mac_sys/unit_table.hh"

// plus
#include "plus/deconstruct_bool.hh"
#include "plus/var_string.hh"

// MacFeatures
#include "MacFeatures/BlueBoxed.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"


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
	
	
	static
	void get( plus::var_string& result, const vfs::node* that, bool binary, const plus::string& name )
	{
		const char c = name[ 0 ];
		
		const bool test = c == 'a' ? RunningInClassic::Test()
		                : c == 'f' ? RunningInWeakEmulator::Test()
		                :            false;
		
		plus::deconstruct_bool::apply( result, test, binary );
	}
	
	static const vfs::property_params sys_mac_errata_mux_params =
	{
		1,  // fixed size,
		(vfs::property_get_hook) &get,
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_errata_Mappings[] =
	{
		{ "async-io-race",            PROPERTY( sys_mac_errata_mux ) },
		{ "fatal-powerpc-exceptions", PROPERTY( sys_mac_errata_mux ) },
		
		{ NULL, NULL }
	};
	
}
