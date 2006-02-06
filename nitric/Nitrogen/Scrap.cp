// Scrap.cp

#ifndef NITROGEN_SCRAP_H
#include "Nitrogen/Scrap.h"
#endif

#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen {
	
	void RegisterScrapManagerErrors()
	{
		// From Inside Macintosh
		RegisterOSStatus< dskFulErr  >();
		RegisterOSStatus< ioErr      >();
		RegisterOSStatus< noScrapErr >();
		RegisterOSStatus< noTypeErr  >();
		RegisterOSStatus< memFullErr >();
		
		// From Carbon docs
		//RegisterOSStatus< needClearScrapErr           >();  // same as noScrapErr
		//RegisterOSStatus< scrapFlavorNotFoundErr      >();  // same as noTypeErr
		RegisterOSStatus< internalScrapErr            >();
		RegisterOSStatus< duplicateScrapFlavorErr     >();
		RegisterOSStatus< badScrapRefErr              >();
		RegisterOSStatus< processStateIncorrectErr    >();
		RegisterOSStatus< scrapPromiseNotKeptErr      >();
		RegisterOSStatus< noScrapPromiseKeeperErr     >();
		RegisterOSStatus< nilScrapFlavorDataErr       >();
		RegisterOSStatus< scrapFlavorFlagsMismatchErr >();
		RegisterOSStatus< scrapFlavorSizeMismatchErr  >();
		RegisterOSStatus< illegalScrapFlavorFlagsErr  >();
		RegisterOSStatus< illegalScrapFlavorTypeErr   >();
		RegisterOSStatus< illegalScrapFlavorSizeErr   >();
	}
	
	ScrapRef GetCurrentScrap()
	{
		Nucleus::OnlyOnce< RegisterScrapManagerErrors >();
		
		ScrapRef scrap;
		ThrowOSStatus( ::GetCurrentScrap( &scrap ) );
		return scrap;
	}
	
	void ClearCurrentScrap()
	{
		Nucleus::OnlyOnce< RegisterScrapManagerErrors >();
		
		ThrowOSStatus( ::ClearCurrentScrap() );
	}
	
}

