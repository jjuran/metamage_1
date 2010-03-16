/*	======================
 *	FSTree_sys_mac_unit.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACUNIT_HH
#define GENIE_FILESYSTEM_FSTREESYSMACUNIT_HH

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif

// nucleus
#include "nucleus/advance_until_done_sequence.hh"

// ClassicToolbox
#include "ClassicToolbox/Devices.hh"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Nitrogen
{
	
	struct UnitTableDrivers_Container_Specifics
	{
		typedef UInt16             size_type;
		typedef SInt16             difference_type;
		typedef AuxDCEHandle       value_type;
		typedef const value_type&  const_reference;
		typedef const value_type*  const_pointer;
		typedef const_pointer      key_type;
		
		static key_type get_next_key( key_type key );
		
		static const_pointer GetPointer( key_type ptr )  { return ptr; }
		
		static key_type begin_key()  { return get_next_key( reinterpret_cast< AuxDCEHandle* >( LMGetUTableBase() ) - 1 ); }
		static key_type end_key()    { return (AuxDCEHandle*) LMGetUTableBase() + LMGetUnitTableEntryCount(); }
	};
	
	class UnitTableDrivers_Container : public nucleus::advance_until_done_sequence< UnitTableDrivers_Container_Specifics >
	{
		friend UnitTableDrivers_Container UnitTableDrivers();
		
		private:
			UnitTableDrivers_Container()
			: nucleus::advance_until_done_sequence< UnitTableDrivers_Container_Specifics >( UnitTableDrivers_Container_Specifics() )
			{}
	};
	
	inline UnitTableDrivers_Container UnitTableDrivers()
	{
		return UnitTableDrivers_Container();
	}
	
}

namespace Genie
{
	
	const unsigned char* GetDriverName_WithinHandle( AuxDCEHandle dceHandle );
	
	FSTreePtr New_FSTree_sys_mac_unit( const FSTreePtr& parent, const std::string& name );
	
}

#endif

