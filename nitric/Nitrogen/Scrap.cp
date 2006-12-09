// Scrap.cp

#ifndef NITROGEN_SCRAP_H
#include "Nitrogen/Scrap.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
{
	
	ScrapManagerErrorsRegistrationDependency::ScrapManagerErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterScrapManagerErrors();
	
	
	class ScrapManagerErrorsRegistration
	{
		public:
			ScrapManagerErrorsRegistration()  { RegisterScrapManagerErrors(); }
	};
	
	static ScrapManagerErrorsRegistration theRegistration;
	
	
	ScrapRef GetCurrentScrap()
	{
		ScrapRef scrap;
		ThrowOSStatus( ::GetCurrentScrap( &scrap ) );
		return scrap;
	}
	
	void ClearCurrentScrap()
	{
		ThrowOSStatus( ::ClearCurrentScrap() );
	}
	
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
	
}

