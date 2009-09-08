// Nitrogen/Scrap.cp
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/Scrap.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( ScrapManager )
	
	
	static void RegisterScrapManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class ScrapManagerErrorsRegistration
	{
		public:
			ScrapManagerErrorsRegistration()  { RegisterScrapManagerErrors(); }
	};
	
	static ScrapManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
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

