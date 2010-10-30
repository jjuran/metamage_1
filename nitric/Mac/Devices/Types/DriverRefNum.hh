/*
	Mac/Devices/Types/DriverRefNum.hh
	---------------------------------
*/

#ifndef MAC_DEVICES_TYPES_DRIVERREFNUM_HH
#define MAC_DEVICES_TYPES_DRIVERREFNUM_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum DriverRefNum
	{
		kDriverRefNum_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
}

#endif

