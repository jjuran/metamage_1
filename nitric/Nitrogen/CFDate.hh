// Nitrogen/CFDate.hh
// ------------------

// Part of the Nitrogen project.
//
// Written 2003-2007 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFDATE_HH
#define NITROGEN_CFDATE_HH

#ifndef __CFDATE__
#include <CFDate.h>
#endif
#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif

namespace Nitrogen {
	using ::CFDateRef;
	using ::CFDateGetTypeID;
  }

namespace nucleus
  {
	template <> struct disposer_traits< Nitrogen::CFDateRef >: disposer_traits< Nitrogen::CFTypeRef >  {};
  }

namespace Nitrogen
  {	
  	template <> struct CFType_Traits< CFDateRef >: Basic_CFType_Traits< CFDateRef, ::CFDateGetTypeID > {};

	inline nucleus::owned<CFDateRef> CFDateCreate ( CFAllocatorRef allocator, CFAbsoluteTime at ) {
		return nucleus::owned<CFDateRef>::seize ( ::CFDateCreate ( allocator, at ));
		}
	
//	CFAbsoluteTime CFDateGetAbsoluteTime ( CFDateRef theDate );
	using ::CFDateGetAbsoluteTime;

//	CFTimeInterval CFDateGetTimeIntervalSinceDate ( CFDateRef theDate, CFDateRef otherDate );
	using ::CFDateGetTimeIntervalSinceDate;
	
	inline CFComparisonResult CFDateCompare ( CFDateRef theDate, CFDateRef otherDate, void *context = NULL ) {
		return ::CFDateCompare ( theDate, otherDate, context );
		}

	inline Boolean CFGregorianDateIsValid ( CFGregorianDate gDate, CFOptionFlags unitFlags ) {
		return ::CFGregorianDateIsValid ( gDate, unitFlags );
		}
	
	inline Boolean CFGregorianDateIsValid( CFGregorianDate date, CFGregorianUnitFlags unitFlags = kCFGregorianAllUnits )
	{
		return CFGregorianDateIsValid( date, CFOptionFlags( unitFlags ) );
	}

//	CFAbsoluteTime CFGregorianDateGetAbsoluteTime ( CFGregorianDate gdate, CFTimeZoneRef tz );
	using ::CFGregorianDateGetAbsoluteTime;
	
//	CFGregorianDate CFAbsoluteTimeGetGregorianDate ( CFAbsoluteTime at, CFTimeZoneRef tz );
	using ::CFGregorianDateGetAbsoluteTime;

//	CFAbsoluteTime CFAbsoluteTimeAddGregorianUnits ( CFAbsoluteTime at, CFTimeZoneRef tz, CFGregorianUnits units );
	using ::CFAbsoluteTimeAddGregorianUnits;
	
//	CFGregorianUnits CFAbsoluteTimeGetDifferenceAsGregorianUnits ( CFAbsoluteTime at1, CFAbsoluteTime at2, CFTimeZoneRef tz, CFOptionFlags unitFlags );
	using ::CFAbsoluteTimeGetDifferenceAsGregorianUnits;

//	SInt32 CFAbsoluteTimeGetDayOfWeek ( CFAbsoluteTime at, CFTimeZoneRef tz );
	using ::CFAbsoluteTimeGetDayOfWeek;

//	SInt32 CFAbsoluteTimeGetDayOfYear ( CFAbsoluteTime at, CFTimeZoneRef tz );
	using ::CFAbsoluteTimeGetDayOfYear;

//	SInt32 CFAbsoluteTimeGetWeekOfYear ( CFAbsoluteTime at, CFTimeZoneRef tz );
	using ::CFAbsoluteTimeGetWeekOfYear;
	}

#endif
