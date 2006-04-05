// DateTimeUtils.cp

#ifndef NITROGEN_DATETIMEUTILS_H
#include "Nitrogen/DateTimeUtils.h"
#endif


#if CALL_NOT_IN_CARBON

extern "C"
{
	
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
	
}  // extern "C"

#endif  // #if CALL_NOT_IN_CARBON

