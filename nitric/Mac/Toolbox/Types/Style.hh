/*
	Mac/Toolbox/Types/Style.hh
	--------------------------
*/

#ifndef MAC_TOOLBOX_TYPES_STYLE_HH
#define MAC_TOOLBOX_TYPES_STYLE_HH

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
	
	enum Style
	{
		kStyle_Max = nucleus::enumeration_traits< ::Style >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( Style )
	
}

#endif

