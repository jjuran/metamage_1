// Gestalt.cp

#ifndef NITROGEN_GESTALT_H
#include "Nitrogen/Gestalt.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen
  {
   long Gestalt( OSType selector )
     {
      OnlyOnce<RegisterGestaltManagerErrors>();
      
      long result;
      ThrowOSStatus( ::Gestalt( selector, &result ) );
      return result;
     }

   long Gestalt( OSType selector, long defaultValue )
     {
      try
        {
         return Gestalt( selector );
        }
      catch ( const ErrorCode< OSStatus, gestaltUndefSelectorErr >& )
        {
         return defaultValue;
        }
     }

   void RegisterGestaltManagerErrors()
     {
      RegisterOSStatus< memFullErr              >();
      RegisterOSStatus< envNotPresent           >();
      RegisterOSStatus< envBadVers              >();
      RegisterOSStatus< envVersTooBig           >();
      RegisterOSStatus< gestaltUnknownErr       >();
      RegisterOSStatus< gestaltUndefSelectorErr >();
      RegisterOSStatus< gestaltDupSelectorErr   >();
      RegisterOSStatus< gestaltLocationErr      >();
     }
  }
