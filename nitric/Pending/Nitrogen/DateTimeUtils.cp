// DateTimeUtils.cp

#ifndef NITROGEN_DATETIMEUTILS_H
#include "Nitrogen/DateTimeUtils.h"
#endif


#if CALL_NOT_IN_CARBON

void DateString( long     dateTime,
                 DateForm longFlag,
                 Str255   result,
                 Handle   intlHandle )
{
	IUDatePString( dateTime, longFlag, result, intlHandle );
}

void TimeString( long     dateTime,
                 DateForm longFlag,
                 Str255   result,
                 Handle   intlHandle )
{
	IUTimePString( dateTime, longFlag, result, intlHandle );
}

void LongDateString( const LongDateTime* dateTime,
                     DateForm            longFlag,
                     Str255              result,
                     Handle              intlHandle )
{
	IULDateString( const_cast< LongDateTime* >( dateTime ), longFlag, result, intlHandle );
}

void LongTimeString( const LongDateTime* dateTime,
                     Boolean             wantSeconds,
                     Str255              result,
                     Handle              intlHandle )
{
	IULTimeString( const_cast< LongDateTime* >( dateTime ), longFlag, result, intlHandle );
}

#endif  // #if CALL_NOT_IN_CARBON

