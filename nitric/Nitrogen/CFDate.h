// CFDate.h

#ifndef NITROGEN_CFDATE_H
#define NITROGEN_CFDATE_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFDATE__
#include FRAMEWORK_HEADER(CoreFoundation,CFDate.h)
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

namespace Nitrogen {
	using ::CFDateRef;
	using ::CFDateGetTypeID;
	template <> struct OwnedDefaults< CFDateRef >: OwnedDefaults< CFTypeRef >  {};
	template <> struct CFType_Traits< CFDateRef >: Basic_CFType_Traits< CFDateRef, ::CFDateGetTypeID > {};

	inline Owned<CFDateRef> CFDateCreate ( CFAllocatorRef allocator, CFAbsoluteTime at ) {
		return Owned<CFDateRef>::Seize ( ::CFDateCreate ( allocator, at ));
		}
	
//	CFAbsoluteTime CFDateGetAbsoluteTime ( CFDateRef theDate );
	using ::CFDateGetAbsoluteTime;

//	CFTimeInterval CFDateGetTimeIntervalSinceDate ( CFDateRef theDate, CFDateRef otherDate );
	using ::CFDateGetTimeIntervalSinceDate;
	
	inline CFComparisonResult CFDateCompare ( CFDateRef theDate, CFDateRef otherDate, void *context = NULL ) {
		return ::CFDateCompare ( theDate, otherDate, context );
		}

	inline Boolean CFGregorianDateIsValid ( CFGregorianDate gDate, CFOptionFlags unitFlags = kCFGregorianAllUnits ) {
		return ::CFGregorianDateIsValid ( gDate, unitFlags );
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
