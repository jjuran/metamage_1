/*
	Mac/Script/Types/LangCode.hh
	----------------------------
*/

#ifndef MAC_SCRIPT_TYPES_LANGCODE_HH
#define MAC_SCRIPT_TYPES_LANGCODE_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif
#ifndef __SCRIPT__
#include <Script.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"


namespace Mac
{
	
	enum LangCode
	{
		langUnspecified = ::langUnspecified,
		
		kLangCode_Max = nucleus::enumeration_traits< ::LangCode >::max
	};
	
}

#endif

