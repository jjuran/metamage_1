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

namespace Nitrogen
  {
   class DateFormTag {};
   typedef SelectorType< DateFormTag, SInt8, shortDate > DateForm;
   
   inline Str255 LongDateString( const LongDateTime& dateTime,
                                 DateForm            longFlag,
                                 Handle              intlHandle = Handle() )
     {
      Str255 result;
      ::LongDateString( &dateTime, longFlag, result, intlHandle );
      return result;
     }

   inline Str255 LongTimeString( const LongDateTime& dateTime,
                                 bool                wantSeconds,
                                 Handle              intlHandle  = Handle() )
     {
      Str255 result;
      ::LongTimeString( &dateTime, wantSeconds, result, intlHandle );
      return result;
     }
  }

#endif
