/*
	virtualization.hh
	-----------------
*/

#ifndef MACSYS_HAS_VIRTUALIZATION_HH
#define MACSYS_HAS_VIRTUALIZATION_HH

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/is_driver_installed.hh"


namespace mac {
namespace sys {
	
	unsigned long core_signature();
	
	inline
	bool has_v68k()
	{
		return core_signature() == 'v68k';
	}
	
	bool has_MinivMac();
	
	inline
	bool has_AUX()
	{
		return gestalt_defined( 'a/ux' );
	}
	
	inline
	bool has_MAE()
	{
		return gestalt_defined( 'cith' );
	}
	
	inline
	bool has_Rosetta()
	{
		return gestalt( 'ppcf' ) == 0x0011;
	}
	
	inline
	bool has_QEMU()
	{
		return is_driver_installed( "\p" ".Display_Video_QemuVGA" );
	}
	
	inline
	bool has_BasiliskII()
	{
		return is_driver_installed( "\p" ".Display_Video_Apple_Basilisk" );
	}
	
	inline
	bool has_ShapeShifter()
	{
		return is_driver_installed( "\p" ".Display_Video_Apple_Amiga1" )  ||
		       is_driver_installed( "\p" ".Display_Video_Apple_Amiga2" );
	}
	
	inline
	bool has_SheepShaver()
	{
		return is_driver_installed( "\p" "Display_Video_Apple_Sheep" )  ||
		       is_driver_installed( "\p" ".Display_Video_Apple_Sheep" );
	}
	
}
}

#endif
