// Scrap.h

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

namespace Nitrogen {
	
	void RegisterScrapManagerErrors();
	
	using ::ScrapRef;
	
	inline ScrapRef ScrapRefNone()  { return kScrapRefNone; }
	
	ScrapRef GetCurrentScrap();
	
	void ClearCurrentScrap();
	
}

#endif

