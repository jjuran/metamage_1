/*
	Mac/Script/Types/ScriptCode.hh
	-------------------------------
*/

#ifndef MAC_SCRIPT_TYPES_SCRIPTCODE_HH
#define MAC_SCRIPT_TYPES_SCRIPTCODE_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
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
	
	enum ScriptCode
	{
		smSystemScript = ::smSystemScript,
		
		kTextScriptDontCare = ::kTextScriptDontCare,
		
		kScriptCode_Max = nucleus::enumeration_traits< ::ScriptCode >::max
	};
	
}

#endif

