// Nitrogen/Scrap.hh
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_SCRAP_HH
#define NITROGEN_SCRAP_HH

#ifndef __SCRAP__
#include <Scrap.h>
#endif

// nucleus
#include "nucleus/errors_registered.hh"

// Replace macro

#ifdef kScrapRefNone
#undef kScrapRefNone

static const ScrapRef kScrapRefNone = ScrapRef( NULL );

#endif

namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( ScrapManager );
	
	using ::ScrapRef;
	
	ScrapRef GetCurrentScrap();
	
	void ClearCurrentScrap();
	
}

#endif

