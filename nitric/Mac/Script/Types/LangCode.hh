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
#ifndef __TEXTCOMMON__
#include <TextCommon.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum LangCode
	{
		langUnspecified = ::langUnspecified,
		
		kTextLanguageDontCare = ::kTextLanguageDontCare,
		
		kLangCode_Max = nucleus::enumeration_traits< ::LangCode >::max
	};
	
}

#endif

