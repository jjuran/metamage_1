// Nitrogen/Scrap.h
// ----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_SCRAP_H
#define NITROGEN_SCRAP_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __SCRAP__
#include FRAMEWORK_HEADER(HIToolbox,Scrap.h)
#endif

// Replace macro

#ifdef kScrapRefNone
#undef kScrapRefNone

static const ScrapRef kScrapRefNone = ScrapRef( NULL );

#endif

namespace Nitrogen
{
	
	class ScrapManagerErrorsRegistrationDependency
	{
		public:
			ScrapManagerErrorsRegistrationDependency();
	};
 	
	using ::ScrapRef;
	
	ScrapRef GetCurrentScrap();
	
	void ClearCurrentScrap();
	
}

#endif

