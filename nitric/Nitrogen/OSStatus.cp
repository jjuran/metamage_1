// OSStatus.cp

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
  {
   void ThrowOSStatus( OSStatus error )
     {
      if ( error != noErr )
         ThrowErrorCode< OSStatus >( error );
     }
  }
