// Nitrogen/ATSTypes.h
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_ATSTYPES_H
#define NITROGEN_ATSTYPES_H

#ifndef __ATSTYPES__
#include <ATSTypes.h>
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_MIXEDMODE_H
#include "Nitrogen/MixedMode.h"
#endif
#include "Nucleus/Enumeration.h"


namespace Nitrogen
{
	
	enum FMFontFamily
	{
		kFMFontFamily_Max = Nucleus::Enumeration_Traits< ::FMFontFamily >::max
	};
	
	typedef FMFontFamily FontID;
	
	enum FMFontStyle
	{
		kFMFontStyle_Max = Nucleus::Enumeration_Traits< ::FMFontStyle >::max
	};
	
}

#endif

