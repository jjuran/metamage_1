/*
	Genie/FS/sys/mac/errata.cc
	--------------------------
*/

#include "Genie/FS/sys/mac/errata.hh"

// plus
#include "plus/deconstruct_bool.hh"
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/Gestalt.hh"

// MacFeatures
#include "MacFeatures/BlueBoxed.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/Utilities/GetMachineName.hh"

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON
	
	#include "Genie/FS/sys/mac/unit.hh"
	
#endif


namespace Genie
{
	namespace N = Nitrogen;
	
	
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
			
			const unsigned char* name = GetMachineName();
			
			const bool powerpc = name != NULL  &&  name[1] == 'P';
			
			return !powerpc;
		}
	};
	
#if !TARGET_API_MAC_CARBON
	
	static bool DriverIsFromSheepShaver( AuxDCEHandle dce )
	{
		const unsigned char* name = GetDriverName_WithinHandle( dce );
		
		return name[0] != 0 && std::equal( name + 1 + (name[0] == '.'),
		                                   name + 1 + name[0],
		                                   "Display_Video_Apple_Sheep" );
	}
	
#endif
	
	struct RunningInSheepShaver
	{
		static bool Test();
	};
	
	bool RunningInSheepShaver::Test()
	{
	#if !TARGET_API_MAC_CARBON
		
		N::UnitTableDrivers_Container drivers = N::UnitTableDrivers();
		
		N::UnitTableDrivers_Container::const_iterator it = std::find_if( drivers.begin(),
		                                                                 drivers.end(),
		                                                                 std::ptr_fun( DriverIsFromSheepShaver ) );
		
		return it != drivers.end();
		
	#endif
		
		return false;  // FIXME:  What about Carbon?
	}
	
	struct RunningInWeakEmulator
	{
		static bool Test()
		{
			return RunningInSheepShaver::Test() || RunningInRosetta::Test();
		}
	};
	
	
	template < class Erratum >
	struct sys_mac_errata_Property
	{
		static void Read( plus::var_string& result, const FSTree* that, bool binary )
		{
			plus::deconstruct_bool::apply( result, Erratum::Test(), binary );
		}
	};
	
	template < class Erratum >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		typedef sys_mac_errata_Property< Erratum > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_errata_Mappings[] =
	{
		{ "async-io-race",            &Property_Factory< RunningInClassic      > },
		{ "fatal-powerpc-exceptions", &Property_Factory< RunningInWeakEmulator > },
		
		{ NULL, NULL }
	};
	
}

