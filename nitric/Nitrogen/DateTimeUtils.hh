// Nitrogen/DateTimeUtils.hh
// -------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_DATETIMEUTILS_HH
#define NITROGEN_DATETIMEUTILS_HH

#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif

#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
#endif


namespace Nitrogen
  {
   using ::LongDateTime;
   using ::LongDateRec;
   using ::DateTimeRec;
	
	enum DateForm
	{
		shortDate  = ::shortDate,
		longDate   = ::longDate,
		abbrevDate = ::abbrevDate,
		
		kDateForm_Max = nucleus::enumeration_traits< SInt8 >::max
	};
	
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

namespace nucleus
{
	
	template <>
	struct converter< LongDateTime, LongDateRec >
	{
		typedef LongDateRec   argument_type;
		typedef LongDateTime  result_type;
		
		LongDateTime operator()( const LongDateRec& input ) const
		{
			return Nitrogen::LongDateToSeconds( input );
		}
	};
	
	template <>
	struct converter< LongDateRec, LongDateTime >
	{
		typedef LongDateTime  argument_type;
		typedef LongDateRec   result_type;
		
		LongDateRec operator()( const LongDateTime& input ) const
		{
			return Nitrogen::LongSecondsToDate( input );
		}
	};
	
	template <>
	struct converter< Nitrogen::DateTime, DateTimeRec >
	{
		typedef DateTimeRec         argument_type;
		typedef Nitrogen::DateTime  result_type;
		
		Nitrogen::DateTime operator()( const DateTimeRec& input ) const
		{
			return Nitrogen::DateToSeconds( input );
		}
	};
	
	template <>
	struct converter< DateTimeRec, Nitrogen::DateTime >
	{
		typedef Nitrogen::DateTime  argument_type;
		typedef DateTimeRec         result_type;
		
		DateTimeRec operator()( const Nitrogen::DateTime& input ) const
		{
			return Nitrogen::SecondsToDate( input );
		}
	};
	
}

#endif

