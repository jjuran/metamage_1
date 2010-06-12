/*
	Mac/Controls/Types/ControlPartCode.hh
	-------------------------------------
*/

#ifndef MAC_CONTROLS_TYPES_CONTROLPARTCODE_HH
#define MAC_CONTROLS_TYPES_CONTROLPARTCODE_HH

#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __CONTROLS__
#include <Controls.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"


namespace Mac
{
	
	enum ControlPartCode
	{
		kControlNoPart        = ::kControlNoPart,
		kControlIndicatorPart = ::kControlIndicatorPart,
		kControlDisabledPart  = ::kControlDisabledPart,
		kControlInactivePart  = ::kControlInactivePart,
		
		kControlEntireControl = ::kControlEntireControl,
		
		kControlPartCode_Max = nucleus::enumeration_traits< ::ControlPartCode >::max
	};
	
}

#endif

