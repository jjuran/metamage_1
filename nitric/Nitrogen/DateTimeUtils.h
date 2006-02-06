// DateTimeUtils.h

#ifndef NITROGEN_DATETIMEUTILS_H
#define NITROGEN_DATETIMEUTILS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __DATETIMEUTILS__
#include FRAMEWORK_HEADER(CarbonCore,DateTimeUtils.h)
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_STR_H
#include "Nitrogen/Str.h"
#endif

#if CALL_NOT_IN_CARBON

#ifdef DateString
#undef DateString
#endif

#ifdef TimeString
#undef TimeString
#endif

#ifdef LongDateString
#undef LongDateString
#endif

#ifdef LongTimeString
#undef LongTimeString
#endif

#endif  // #if CALL_NOT_IN_CARBON

namespace Nitrogen
  {
   using ::LongDateTime;
   using ::LongDateRec;
   using ::DateTimeRec;

   class DateForm_Tag {};
   typedef Nucleus::SelectorType< DateForm_Tag, SInt8, shortDate > DateForm;
   
   typedef UInt32 DateTime;
   
   // 260
   inline Str255 LongDateString( const LongDateTime& dateTime,
                                 DateForm            longFlag,
                                 Handle              intlHandle = Handle() )
     {
      Str255 result;
      ::LongDateString( &dateTime, longFlag, result, intlHandle );
      return result;
     }

   // 276
   inline Str255 LongTimeString( const LongDateTime& dateTime,
                                 bool                wantSeconds,
                                 Handle              intlHandle  = Handle() )
     {
      Str255 result;
      ::LongTimeString( &dateTime, wantSeconds, result, intlHandle );
      return result;
     }
	
	// ...
	
	// 342
	inline LongDateTime LongDateToSeconds( const LongDateRec& lDate )
	{
		LongDateTime lSecs;
		::LongDateToSeconds( &lDate, &lSecs );
		return lSecs;
	}
	
	// 356
	inline LongDateRec LongSecondsToDate( const LongDateTime& lSecs )
	{
		LongDateRec lDate;
		::LongSecondsToDate( &lSecs, &lDate );
		return lDate;
	}
	
	// ...
	
	// 417
	inline DateTime GetDateTime()
	{
		UInt32 secs;
		::GetDateTime( &secs );
		return secs;
	}
	
	// ...
	
	// 477
	inline DateTime DateToSeconds( const DateTimeRec& d )
	{
		UInt32 secs;
		::DateToSeconds( &d, &secs );
		return secs;
	}
	
	// 491
	inline DateTimeRec SecondsToDate( DateTime secs )
	{
		DateTimeRec d;
		::SecondsToDate( secs, &d );
		return d;
	}
	
  }

namespace Nucleus
  {
	template <>
	struct Converter< Nitrogen::LongDateTime, Nitrogen::LongDateRec > : public std::unary_function< Nitrogen::LongDateRec, Nitrogen::LongDateTime >
	{
		Nitrogen::LongDateTime operator()( const Nitrogen::LongDateRec& input ) const
		{
			return Nitrogen::LongDateToSeconds( input );
		}
	};
	
	template <>
	struct Converter< Nitrogen::LongDateRec, Nitrogen::LongDateTime > : public std::unary_function< Nitrogen::LongDateTime, Nitrogen::LongDateRec >
	{
		Nitrogen::LongDateRec operator()( const Nitrogen::LongDateTime& input ) const
		{
			return Nitrogen::LongSecondsToDate( input );
		}
	};
	
	template <>
	struct Converter< Nitrogen::DateTime, Nitrogen::DateTimeRec > : public std::unary_function< Nitrogen::DateTimeRec, Nitrogen::DateTime >
	{
		Nitrogen::DateTime operator()( const Nitrogen::DateTimeRec& input ) const
		{
			return Nitrogen::DateToSeconds( input );
		}
	};
	
	template <>
	struct Converter< Nitrogen::DateTimeRec, Nitrogen::DateTime > : public std::unary_function< Nitrogen::DateTime, Nitrogen::DateTimeRec >
	{
		Nitrogen::DateTimeRec operator()( const Nitrogen::DateTime& input ) const
		{
			return Nitrogen::SecondsToDate( input );
		}
	};
  }

#endif
