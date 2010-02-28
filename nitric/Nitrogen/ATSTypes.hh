// Nitrogen/ATSTypes.hh
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_ATSTYPES_HH
#define NITROGEN_ATSTYPES_HH

#ifndef __ATSTYPES__
#include <ATSTypes.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"

#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif
#ifndef NITROGEN_MIXEDMODE_HH
#include "Nitrogen/MixedMode.hh"
#endif


namespace Nitrogen
{
	
	enum FMFontFamily
	{
		kFMFontFamily_Max = nucleus::enumeration_traits< ::FMFontFamily >::max
	};
	
	typedef FMFontFamily FontID;
	
	enum FMFontStyle
	{
		kFMFontStyle_Max = nucleus::enumeration_traits< ::FMFontStyle >::max
	};
	
}

#endif

