// Gestalt.cp

#ifndef NITROGEN_GESTALT_H
#include "Nitrogen/Gestalt.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif

namespace Nitrogen
  {
   long Gestalt( OSType selector )
     {
      Nucleus::OnlyOnce<RegisterGestaltManagerErrors>();
      
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
      catch ( const Nucleus::ErrorCode< OSStatus, gestaltUndefSelectorErr >& )
        {}
	  return defaultValue;
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
