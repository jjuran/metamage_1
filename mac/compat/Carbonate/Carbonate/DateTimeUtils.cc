// Carbonate/DateTimeUtils.cc


#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif

#if !TARGET_API_MAC_CARBON

#if ACCESSOR_CALLS_ARE_FUNCTIONS
// Compile the Carbon accessors as extern pascal functions.
#define CARBONATE_LINKAGE pascal
#endif

#include "Carbonate/DateTimeUtils.hh"

// These functions are always declared in the headers and are always extern.

extern "C"
{
	
#ifndef __MC68K__
	
	pascal void DateString( long      dateTime,
							DateForm  longFlag,
							Str255    result,
							Handle    intlHandle )
	{
		IUDatePString( dateTime,
		               longFlag,
		               result,
		               intlHandle );
	}
	
	pascal void TimeString( long     dateTime,
							Boolean  wantSeconds,
							Str255   result,
							Handle   intlHandle )
	{
		IUTimePString( dateTime,
		               wantSeconds,
		               result,
		               intlHandle );
	}
	
#endif
	
	pascal void LongDateString( const LongDateTime*  dateTime,
								DateForm             longFlag,
								Str255               result,
								Handle               intlHandle )
	{
		IULDateString( const_cast< LongDateTime* >( dateTime ),
		               longFlag,
		               result,
		               intlHandle );
	}
	
	pascal void LongTimeString( const LongDateTime*  dateTime,
								Boolean              wantSeconds,
								Str255               result,
								Handle               intlHandle )
	{
		IULTimeString( const_cast< LongDateTime* >( dateTime ),
		               wantSeconds,
		               result,
		               intlHandle );
	}
	
}

#endif
