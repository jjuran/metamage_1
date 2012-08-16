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

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __ATSTYPES__
#include <ATSTypes.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
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

