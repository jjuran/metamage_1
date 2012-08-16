/*
	Mac/Toolbox/Types/OptionBits.hh
	-------------------------------
*/

#ifndef MAC_TOOLBOX_TYPES_OPTIONBITS_HH
#define MAC_TOOLBOX_TYPES_OPTIONBITS_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif


namespace Mac
{
	
	enum OptionBits
	{
		kNilOptions = ::kNilOptions,
		
		kOptionBits_Max = nucleus::enumeration_traits< ::OptionBits >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( OptionBits )
	
}

#endif

